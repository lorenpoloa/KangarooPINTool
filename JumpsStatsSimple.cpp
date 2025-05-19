#include "pin.H"
#include <iostream>
#include <fstream>

/// @brief Variables globales para el conteo de saltos
 
UINT64 jump_count = 0;

/**
 * @brief Funcion que se ejecuta cada vez que se encuentra un salto
 * 
 * @param ins 
 * @param v 
 * @return VOID 
 */
VOID CountBranches(INS ins, VOID* v)
{
	// Comprobamos si la instruccion es un salto
	if (INS_IsBranch(ins)) {
		jump_count++;
	}
}

/**
 * @brief Funcion que se ejecuta al finalizar el programa
 * 
 * @param code 
 * @param v 
 * @return VOID 
 */

 /**
  * @brief Funcion que imprime los resultados al finalizar la ejecución del programa a instrumentar
  * 
  * @param code 
  * @param v 
  * @return VOID 
  */
VOID Fini(INT32 code, VOID* v)
{
	// Imprimir el total de saltos en la salida
	std::cerr << "Total de saltos condicionales: " << jump_count << std::endl;
}

/**
 * @brief Funcion principal
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
	// Inicializacion de PIN
	if (PIN_Init(argc, argv)) {
		std::cerr << "Error al iniciar PIN.\n";
		return 1;
	}

	// Instrumentacion: insertar la funcion CountBranches en todas las instrucciones
	INS_AddInstrumentFunction(CountBranches, 0);

	// Funcion final: se ejecuta cuando el programa termina
	PIN_AddFiniFunction(Fini, 0);

	// Iniciar la ejecucion del programa instrumentado
	PIN_StartProgram();

	return 0; // Este punto no se alcanza
}