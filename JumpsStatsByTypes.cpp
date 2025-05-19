//
// @ORIGINAL_AUTHOR: Lorenzo Polo Arévalo
//

/*! @file
* This file contain a PIN Tool to get stats about jumps by type.
*/


#include "pin.H"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>

/**
 * @brief Estructura de estadísticas por tipo de salto
 * 
 */

struct JumpStats {
	UINT64 taken;
	UINT64 notTaken;

	JumpStats() : taken(0), notTaken(0) {}
};

/**
 * @brief Mapa de estadísticas por mnemonic
 * 
 */

std::map<std::string, JumpStats> stats;
UINT64 totalTaken = 0;
UINT64 totalNotTaken = 0;

//Se declara un archivo de salida para los resultados.
std::ofstream outFile;

// opcion -o para declarar nombre del archivo de salida
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
	"o", "", "Output file name");


/**
 * @brief Función llamada antes de cada instrucción de salto. Cada mnemonico se guarda en una fila del mapa y
 *  para cada uno se cuenta si el salto es tomado o no.
 * 
 * @param mnemonic mnemonico del tipo de salto (jne, jmp, jz, etc. )
 * @param taken bool para indicar si el salto fue tomado o no
 * @return VOID 
 */

VOID CountBranch(std::string* mnemonic, BOOL taken)
{
	if (taken) {
		stats[*mnemonic].taken++;
		totalTaken++;
	}
	else {
		stats[*mnemonic].notTaken++;
		totalNotTaken++;
	}
}

/**
 * @brief 
 * 
 * @param rtn 
 * @return VOID 
 */
VOID InstrumentRoutine(RTN rtn)
{
	RTN_Open(rtn);

	for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)) {

		// si la instrucción es de salto (Branch) 
		if (INS_IsBranch(ins)) {

			std::string* mnemonic = new std::string(INS_Mnemonic(ins)); // Se se copia en mnemonico del salto (nombre del tipo de salto)

			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountBranch,
				IARG_PTR, mnemonic,
				IARG_BRANCH_TAKEN,
				IARG_END);
		}
	}

	RTN_Close(rtn);
}

/**
 * @brief Se llama cuando se carga una imagen, si la imagen pertenece al programa
 *  principal se ejecuta la instrumentación, en caso contrario no.
 * 
 * @param img 
 * @param v 
 * @return VOID 
 */
VOID ImageLoad(IMG img, VOID* v)
{
	if (!IMG_IsMainExecutable(img)) return;


	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
		for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
			InstrumentRoutine(rtn);
		}
	}
}

/**
 * @brief Impresión de las estadísticas
 * 
 * @param code 
 * @param v 
 * @return * VOID 
 */
VOID Fini(INT32 code, VOID* v)
{
	std::ostream& out = outFile.is_open() ? outFile : std::cerr; // si no se ha abierto el archivo de resultados estos se pasan por standard error

																 //Se imprimen las cabeceras de los resultados (nombres de columna)
	out << "======= Branch Statistics =======\n";
	out << std::left << std::setw(10) << "Type"
		<< std::setw(10) << "Taken"
		<< std::setw(15) << "Not-Taken"
		<< std::setw(10) << "Total"
		<< "Percent Taken" << endl;

	// Se itera cada entrada del mapa para imprimir las estadisticas de cada tipo de salto
	std::map<std::string, JumpStats>::iterator it;

	for (it = stats.begin(); it != stats.end(); ++it) {
		const std::string& mnemonic = it->first;
		const JumpStats& s = it->second;
		UINT64 total = s.taken + s.notTaken; // se calcula el total de saltos del actual tipo
		double percent = (total > 0) ? (100.0 * s.taken / total) : 0.0; // se calcula el porcentaje de saltos tomados

																		//se imprimen los resultados
		out << std::left << std::setw(10) << mnemonic
			<< std::setw(10) << s.taken
			<< std::setw(15) << s.notTaken
			<< std::setw(10) << total
			<< std::fixed << std::setprecision(2) << percent << endl;
	}

	// Se imprimen los resultados finales
	out << "=======================================\n";
	out << "Total Taken     = " << totalTaken << endl;
	out << "Total Not Taken = " << totalNotTaken << endl;
	out << "Total Branches  = " << (totalTaken + totalNotTaken) << endl;
}

/**
 * @brief Funcion llamada cuando falla la pin tool.
 * 
 * @return INT32 
 */
INT32 Usage()
{
	std::cerr << "Usage: pin -t JumpsStatsByType.so [-o output.txt] -- <program>\n"; // Mensaje help si hay fallo en la ejecución 
	return -1;
}

/**
 * @brief Inicio de programa.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
	if (PIN_Init(argc, argv)) return Usage(); // Si falla la ejecucion de la pin tool

	if (!KnobOutputFile.Value().empty()) { // Si se ha introducido un nombre de archivo
		outFile.open(KnobOutputFile.Value().c_str()); // se crea el archivo con el nombre introducido
	}

	IMG_AddInstrumentFunction(ImageLoad, 0);// Cuando se inicia una nueva imagen se llama a la función image load (comprueva si pertenece al programa principal)
	PIN_AddFiniFunction(Fini, 0);// Al finalizar la ejecución se llama a la función Fini

	PIN_StartProgram(); // No retorna

	return 0;
}