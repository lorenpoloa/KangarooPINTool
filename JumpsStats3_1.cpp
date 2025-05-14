#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>

// Estructura para llevar las estadísticas por tipo de salto
struct BranchStats {
	UINT64 taken;
	UINT64 notTaken;
};

// Mapa: tipo de salto -> estadísticas
std::map<std::string, BranchStats> branchStats;

UINT64 totalBranches = 0;
UINT64 totalTaken = 0;
UINT64 totalNotTaken = 0;

KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "Archivo de salida");

std::ostream* out = &std::cerr;
std::ofstream outFile;

VOID CountBranchTaken(std::string* mnemonic, BOOL taken)
{
	BranchStats& stats = branchStats[*mnemonic];
	if (taken) {
		stats.taken++;
		totalTaken++;
	}
	else {
		stats.notTaken++;
		totalNotTaken++;
	}
	totalBranches++;
}

VOID Instruction(INS ins, VOID* v)
{
	if (INS_IsBranch(ins)) {
		std::string disasm = INS_Disassemble(ins);
		size_t space = disasm.find(' ');
		std::string mnemonic = (space != std::string::npos) ? disasm.substr(0, space) : disasm;

		std::string* copyMnemonic = new std::string(mnemonic);

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountBranchTaken,
			IARG_PTR, copyMnemonic,
			IARG_BRANCH_TAKEN,
			IARG_END);
	}
}

VOID Fini(INT32 code, VOID* v)
{
	*out << "======= Estadisticas de saltos =======\n";
	*out << "Tipo\n\tTomados\n\tNo tomados\n\tTotal\n\tPorcentaje Tomado\n";

	std::map<std::string, BranchStats>::iterator it;
	for (it = branchStats.begin(); it != branchStats.end(); ++it) {
		const std::string& mnemonic = it->first;
		const BranchStats& stats = it->second;
		UINT64 subtotal = stats.taken + stats.notTaken;
		double pctTaken = (subtotal > 0) ? (100.0 * stats.taken / subtotal) : 0.0;

		*out << "type: " << std::fixed << mnemonic
			<< "\n\tTaken: " << std::fixed << stats.taken
			<< "\n\tNot-Taken: " << std::fixed << stats.notTaken
			<< "\n\tTotal: " << std::fixed << subtotal
			<< "\n\tPercent-Taken: " << std::fixed << std::setprecision(2)
			<< pctTaken << "%\n";
	}

	*out << "=======================================\n";
	*out << "Total:\n"
		<< "  Tomados     = " << totalTaken << "\n"
		<< "  No tomados  = " << totalNotTaken << "\n"
		<< "  Saltos totales = " << totalBranches << "\n";

	if (outFile.is_open())
		outFile.close();
}

int main(int argc, char* argv[])
{
	if (PIN_Init(argc, argv)) {
		std::cerr << "Uso incorrecto del Pin Tool.\n";
		return 1;
	}

	if (!KnobOutputFile.Value().empty()) {
		outFile.open(KnobOutputFile.Value().c_str());
		if (outFile.is_open()) {
			out = &outFile;
		}
		else {
			std::cerr << "No se pudo abrir el archivo de salida.\n";
			return 1;
		}
	}

	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(Fini, 0);

	PIN_StartProgram(); // No retorna
	return 0;
}
