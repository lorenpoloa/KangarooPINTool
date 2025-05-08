#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <x86intrin.h>

// =================================================================================
// Estructuras
// =================================================================================

// Agrupamos datos por tipo de salto y si fue tomado o no
struct BranchStats {
	UINT64 count_taken = 0;
	UINT64 cycles_taken = 0;
	UINT64 count_nottaken = 0;
	UINT64 cycles_nottaken = 0;
};


struct BranchRecord {
	UINT64 startCycle;
	ADDRINT resolveAddr;
	std::string mnemonic;
	BOOL taken;
};

// Por hilo
std::map<THREADID, BranchRecord> activeBranches;

// Estadísticas por tipo de salto
std::map<std::string, BranchStats> branchStats;

// =================================================================================
// Funciones de instrumentación
// =================================================================================

VOID StartCondBranch(ADDRINT pc, BOOL taken, ADDRINT target, ADDRINT fallthrough,
	THREADID tid, const std::string *mnemonic)
{
	BranchRecord rec;
	rec.startCycle = __rdtsc();
	rec.resolveAddr = taken ? target : fallthrough;
	rec.mnemonic = *mnemonic;
	rec.taken = taken;

	activeBranches[tid] = rec;
}

VOID MaybeResolveBranch(ADDRINT currentPC, THREADID tid)
{
	auto it = activeBranches.find(tid);
	if (it != activeBranches.end() && currentPC == it->second.resolveAddr) {
		UINT64 endCycle = __rdtsc();
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

// =================================================================================
// Instrumentación de instrucciones
// =================================================================================

VOID Instruction(INS ins, VOID *v)
{
	if (INS_IsBranch(ins) && INS_IsConditionalBranch(ins) && INS_HasFallThrough(ins)) {
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
	else {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MaybeResolveBranch,
			IARG_INST_PTR,
			IARG_THREAD_ID,
			IARG_END);
	}
}

// =================================================================================
// Finalización
// =================================================================================

VOID Fini(INT32 code, VOID *v)
{
	std::ofstream out("branch_type_detail_report.txt");

	out << "======= Informe por tipo de salto condicional =======\n";
	out << "Tipo\tTomado\tCount\tCycles\tAvg\n";

	for (const auto &entry : branchStats) {
		const std::string &type = entry.first;
		const BranchStats &stats = entry.second;

		// Tomados
		out << type << "\tYES\t" << stats.count_taken << "\t"
			<< stats.cycles_taken << "\t"
			<< (stats.count_taken ? (stats.cycles_taken / stats.count_taken) : 0) << "\n";

		// No tomados
		out << type << "\tNO\t" << stats.count_nottaken << "\t"
			<< stats.cycles_nottaken << "\t"
			<< (stats.count_nottaken ? (stats.cycles_nottaken / stats.count_nottaken) : 0) << "\n";
	}

	out << "=====================================================\n";
	out.close();
}

// =================================================================================
// Inicialización
// =================================================================================

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
