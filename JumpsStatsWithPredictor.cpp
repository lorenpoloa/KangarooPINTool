///
/// @ORIGINAL_AUTHOR: Lorenzo Polo Arévalo
///

/*!
 * @file
 * @brief PIN Tool para recopilar estadísticas de distintos simuladores de predictores de salto.
 *
 * Este archivo implementa una herramienta basada en Intel PIN que instrumenta saltos condicionales
 * y simula varios tipos de predictores de salto (always taken, always not taken, 1-bit, 2-bit).
 * Al finalizar la ejecución del programa instrumentado, se imprimen estadísticas de precisión
 * para cada predictor.
 */


 ///se importan las librerias necesarias: pin, entrada y salida estandar, formateo de salida, uso de mapas y el simulador de predictores de salto
#include "pin.H" 
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include "Jump_Predictor_Sim.h"

///importación de simbolos del espacio std
using std::string;
using std::map;
using std::ostream;
using std::ofstream;
using std::cerr;

/// @brief Instancia global del simulador de predictores de salto
BranchPredictor predictor;

/// @brief Opción de línea de comandos para especificar archivo de salida.
/// @details Si se utiliza `-o <archivo>`, los resultados se imprimen en ese archivo.
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "Archivo de salida");

/// @brief Flujo de salida para imprimir los resultados.
/// @details Por defecto, se escribe a `stderr`, a menos que se especifique un archivo con la opción `-o`.
ostream* out = &cerr;


/**
 * @brief Función de conteo de saltos condicionales.
 *
 * Esta función es invocada por PIN antes de la ejecución de cada salto condicional
 * para informar si el salto fue tomado o no.
 *
 * @param addr Dirección de la instrucción de salto.
 * @param taken Booleano que indica si el salto fue tomado.
 */
VOID CountBranch(ADDRINT addr, BOOL taken) {
    predictor.Update(addr, taken);
}

/**
 * @brief Función de instrumentación de instrucciones.
 *
 * Recorre cada instrucción del programa, y si es un salto condicional con camino alternativo,
 * inserta una llamada a `CountBranch` justo antes de ejecutarla.
 *
 * @param ins Instrucción a instrumentar.
 * @param v Argumento adicional (no utilizado en este caso).
 */
VOID Instruction(INS ins, VOID* v) {

	///comprueba si es una instrucción de salto y condicional (INS_HasFallThrough)  
	if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) {
		
		///Se inserta una llamada a la función CountBranch antes de ejecutar la instrucción ins. 
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountBranch,
            IARG_INST_PTR, /// Dirección de la instrucción
            IARG_BRANCH_TAKEN, /// Indica si es tomado
            IARG_END);         /// Fin de argumentos
    }
}

/**
 * @brief Función que se ejecuta al finalizar la ejecución del programa instrumentado.
 *
 * Imprime estadísticas de aciertos, totales y precisión para cada tipo de predictor simulado.
 *
 * @param code Código de finalización del programa (no utilizado).
 * @param v Argumento adicional (no utilizado).
 */
VOID Fini(INT32 code, VOID* v) {

	/// se imprime la cabecera de los resultados
    *out << "===== Resultados de Predictores de Salto =====\n";
    const char* names[] = { "Always Taken", "Always Not Taken", "1-bit", "2-bit" };

	/// se crea una estructura de tipo PredictorStats.
	/// Se guardan los resultados de cada predictor llamando a la funcion getStats de la instancia predictor
    const PredictorStats* stats = predictor.GetStats();

	///Por cada simulador de predictor se guardan sus resultados 
    for (int i = 0; i < NUM_PREDICTORS; ++i) {
        UINT64 correct = stats[i].correct; /// numero aciertos
        UINT64 total = stats[i].total; ///total saltos
        double accuracy = (total > 0) ? (100.0 * correct / total) : 0.0; /// porcentaje de aciertos

		/// se imprimen los resultados
        *out << names[i]
             << "\n\tAciertos: " << correct 
             << "\n\tTotal: " << total
             << "\n\tPrecisión: " << std::fixed << std::setprecision(2) << accuracy << endl;
    }
}

/**
 * @brief Función de ayuda en caso de error de uso.
 *
 * Muestra la forma correcta de ejecutar la herramienta cuando se detecta un error en los argumentos.
 *
 * @return -1 Siempre, indicando error.
 */
INT32 Usage()
{
	// Mensaje help si hay fallo en la ejecución 
	cerr << "Usage: pin -t JumpsStatsByType.so [-o output.txt] -- <program>\n"; 	return -1;
}

/**
 * @brief Función principal de la herramienta PIN.
 *
 * Inicializa el entorno de PIN, configura el archivo de salida si se especifica,
 * registra las funciones de instrumentación y finalización, y lanza el programa instrumentado.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Vector de argumentos.
 * @return 0 si termina correctamente (aunque normalmente no retorna).
 */
int main(int argc, char* argv[]) {

	/// Se ejecuta PIN con los argumentos recibidos en main.
	/// Si la ejecución devuelve un valor diferente de 0
	/// se ha producido un error y se informa por standar error
	/// de su utilización correcta
    if (PIN_Init(argc, argv)) {
        return Usage();
    }

	/// si se ha pasado el nombre de un archivo de salida,
	/// redirige estandar error al archivo de salida
    if (!KnobOutputFile.Value().empty()) {
        out = new ofstream(KnobOutputFile.Value().c_str());
    }

	/// Se registra la función que analizara las instrucciones.
    INS_AddInstrumentFunction(Instruction, 0);

	/// Se registra la función Fini que se llama al finalizar el
	/// programa
    PIN_AddFiniFunction(Fini, 0);

	/// se inicia la ejecución del programa a instrumentar
    PIN_StartProgram(); /// No retorna
    return 0;
}
