#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>

// Contador de tipos de salto
std::map<std::string, UINT64> branchTypeCounts;
UINT64 totalBranches = 0;

// Opciones de salida
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "Archivo de salida");

std::ostream* out = &std::cerr;
std::ofstream outFile;

// Función que se llama en cada ejecución de un salto
VOID CountBranch(std::string* mnemonic)
{
	branchTypeCounts[*mnemonic]++;
	totalBranches++;
}

// Instrumentar instrucciones de salto
VOID Instruction(INS ins, VOID* v)
{
	if (INS_IsBranch(ins)) {
		std::string disasm = INS_Disassemble(ins);
		size_t space = disasm.find(' ');
		std::string mnemonic = (space != std::string::npos) ? disasm.substr(0, space) : disasm;

		std::string* copyMnemonic = new std::string(mnemonic);

		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountBranch,
			IARG_PTR, copyMnemonic,
			IARG_END);
	}
}

// Mostrar resultados al finalizar
VOID Fini(INT32 code, VOID* v)
{
	*out << "======= Informe de saltos por tipo =======\n";
	*out << "Tipo\t\tConteo\t\tPorcentaje\n";

	std::map<std::string, UINT64>::iterator it;
	for (it = branchTypeCounts.begin(); it != branchTypeCounts.end(); ++it) {
		double porcentaje = (totalBranches > 0)
			? (100.0 * it->second) / totalBranches
			: 0.0;
		*out << std::setw(6) << it->first
			<< "\t\t" << std::setw(12) << it->second
			<< "\t\t" << std::fixed << std::setprecision(2)
			<< porcentaje << "%\n";
	}

	*out << "==========================================\n";

	if (outFile.is_open())
		outFile.close();
}

// Función principal
int main(int argc, char* argv[])
{
	if (PIN_Init(argc, argv)) {
		std::cerr << "Error al iniciar Pin.\n";
		return 1;
	}

	// Abrir archivo de salida si se especificó
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
