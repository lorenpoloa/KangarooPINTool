#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <stdint.h>  // Para tipos como uint64_t
#ifdef _MSC_VER
#include <intrin.h>  // Para __rdtsc en MSVC
#else
#include <x86intrin.h>  // Para __rdtsc en GCC
#endif

// =================================================================================
// Estructuras
// =================================================================================

// Agrupamos datos por tipo de salto y si fue tomado o no
struct BranchStats {
	UINT64 count_taken;
	UINT64 cycles_taken;
	UINT64 count_nottaken;
	UINT64 cycles_nottaken;

	// Constructor explícito
	BranchStats() : count_taken(0), cycles_taken(0), count_nottaken(0), cycles_nottaken(0) {}
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

// Función para leer el ciclo de la CPU (TSC)
inline UINT64 ReadTSC() {
	return __rdtsc();
}

// Inicia el seguimiento de un salto condicional
VOID StartCondBranch(ADDRINT pc, BOOL taken, ADDRINT target, ADDRINT fallthrough,
	THREADID tid, const std::string *mnemonic)
{
	BranchRecord rec;
	rec.startCycle = ReadTSC();  // Leer el ciclo de CPU en el inicio
	rec.resolveAddr = taken ? target : fallthrough;
	rec.mnemonic = *mnemonic;
	rec.taken = taken;

	activeBranches[tid] = rec;
}

// Verifica si el salto ha sido resuelto
VOID MaybeResolveBranch(ADDRINT currentPC, THREADID tid)
{
	auto it = activeBranches.find(tid);
	if (it != activeBranches.end() && currentPC == it->second.resolveAddr) {
		UINT64 endCycle = ReadTSC();
		UINT64 duration = endCycle - it->second.startCycle;

		// Actualiza las estadísticas según si fue tomado o no
		BranchStats &stats = branchStats[it->second.mnemonic];
		if (it->second.taken) {
			stats.count_taken++;
			stats.cycles_taken += duration;
		}
		else {
			stats.count_nottaken++;
			stats.cycles_nottaken += duration;
		}

		// El salto ya ha sido resuelto, lo eliminamos del seguimiento
		activeBranches.erase(it);
	}
}

// =================================================================================
// Instrumentación de instrucciones
// =================================================================================

// Esta función es llamada para cada instrucción
VOID Instruction(INS ins, VOID *v)
{
	if (INS_IsBranch(ins) && INS_IsConditionalBranch(ins) && INS_HasFallThrough(ins)) {
		// Obtenemos el mnemónico de la instrucción
		std::string *mnemonic = new std::string(INS_Disassemble(ins));

		// Extraer solo el mnemónico (ej: "je", "jne", etc.)
		size_t space = mnemonic->find(' ');
		if (space != std::string::npos) {
			*mnemonic = mnemonic->substr(0, space);
		}

		// Insertamos el seguimiento del salto
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)StartCondBranch,
			IARG_INST_PTR,
			IARG_BRANCH_TAKEN,
			IARG_BRANCH_TARGET_ADDR,
			IARG_FALLTHROUGH_ADDR,
			IARG_THREAD_ID,
			IARG_PTR, mnemonic,
			IARG_END);

		// Cada instrucción también verifica si estamos resolviendo un salto
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MaybeResolveBranch,
			IARG_INST_PTR,
			IARG_THREAD_ID,
			IARG_END);
	}
	else {
		// Verifica en todas las demás instrucciones si estamos resolviendo un salto activo
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MaybeResolveBranch,
			IARG_INST_PTR,
			IARG_THREAD_ID,
			IARG_END);
	}
}

// =================================================================================
// Finalización: generar informe
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
