#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>

// =================================================================================
// Estructuras
// =================================================================================

struct BranchStats {
	UINT64 count_taken;
	UINT64 cycles_taken;
	UINT64 count_nottaken;
	UINT64 cycles_nottaken;

	BranchStats() : count_taken(0), cycles_taken(0), count_nottaken(0), cycles_nottaken(0) {}
};

struct BranchRecord {
	UINT64 startCycle;
	ADDRINT resolveAddr;
	std::string mnemonic;
	BOOL taken;
};

std::map<THREADID, BranchRecord> activeBranches;
std::map<std::string, BranchStats> branchStats;

// =================================================================================
// RDTSC en Linux sin librerías
// =================================================================================

inline UINT64 ReadTSC() {
	unsigned int lo, hi;
	asm volatile (
		"rdtsc"
		: "=a"(lo), "=d"(hi)
		);
	return ((UINT64)hi << 32) | lo;
}

// =================================================================================
// Funciones de instrumentación
// =================================================================================

VOID StartCondBranch(ADDRINT pc, BOOL taken, ADDRINT target, ADDRINT fallthrough,
	THREADID tid, const std::string *mnemonic)
{
	BranchRecord rec;
	rec.startCycle = ReadTSC();
	rec.resolveAddr = taken ? target : fallthrough;
	rec.mnemonic = *mnemonic;
	rec.taken = taken;
	activeBranches[tid] = rec;
}

VOID MaybeResolveBranch(ADDRINT currentPC, THREADID tid)
{
	std::map<THREADID, BranchRecord>::iterator it = activeBranches.find(tid);
	if (it != activeBranches.end() && currentPC == it->second.resolveAddr) {
		UINT64 endCycle = ReadTSC();
		UINT64 duration = endCycle - it->second.startCycle;

		BranchStats &stats = branchStats[it->second.mnemonic];
		if (it->second.taken) {
			stats.count_taken++;
			stats.cycles_taken += duration;
		}
		else {
			stats.count_nottaken++;
			stats.cycles_nottaken += duration;
		}

		activeBranches.erase(it);
	}
}

VOID Instruction(INS ins, VOID *v)
{
	if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) {
		std::string *mnemonic = new std::string(INS_Disassemble(ins));
		size_t space = mnemonic->find(' ');
		if (space != std::string::npos) {
			*mnemonic = mnemonic->substr(0, space);
		}

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)StartCondBranch,
			IARG_INST_PTR,
			IARG_BRANCH_TAKEN,
			IARG_BRANCH_TARGET_ADDR,
			IARG_FALLTHROUGH_ADDR,
			IARG_THREAD_ID,
			IARG_PTR, mnemonic,
			IARG_END);

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MaybeResolveBranch,
			IARG_INST_PTR,
			IARG_THREAD_ID,
			IARG_END);
	}
}

VOID Fini(INT32 code, VOID *v)
{
	std::ofstream out("branch_type_detail_report.txt");

	out << "======= Informe por tipo de salto condicional =======\n";
	out << "Tipo\tTomado\tCount\tCycles\tAvg\n";

	for (std::map<std::string, BranchStats>::iterator it = branchStats.begin(); it != branchStats.end(); ++it) {
		const std::string &type = it->first;
		const BranchStats &stats = it->second;

		out << type << "\tYES\t" << stats.count_taken << "\t"
			<< stats.cycles_taken << "\t"
			<< (stats.count_taken ? (stats.cycles_taken / stats.count_taken) : 0) << "\n";

		out << type << "\tNO\t" << stats.count_nottaken << "\t"
			<< stats.cycles_nottaken << "\t"
			<< (stats.count_nottaken ? (stats.cycles_nottaken / stats.count_nottaken) : 0) << "\n";
	}

	out << "=====================================================\n";
	out.close();
}

int main(int argc, char *argv[])
{
	if (PIN_Init(argc, argv)) {
		std::cerr << "Error al iniciar PIN.\n";
		return 1;
	}

	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram();  // No retorna
	return 0;
}

