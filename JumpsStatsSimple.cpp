#include "pin.H"
#include <iostream>
#include <fstream>

// Variables globales para el conteo de saltos
UINT64 jump_count = 0;

// Función que se ejecuta cada vez que se encuentra un salto
VOID CountBranches(INS ins, VOID* v)
{
	// Comprobamos si la instrucción es un salto
	if (INS_IsBranch(ins)) {
		jump_count++;
	}
}

// Función que se ejecuta al finalizar el programa
VOID Fini(INT32 code, VOID* v)
{
	// Imprimir el total de saltos en la salida
	std::cerr << "Total de saltos condicionales: " << jump_count << std::endl;
}

// Función principal
int main(int argc, char* argv[])
{
	// Inicialización de PIN
	if (PIN_Init(argc, argv)) {
		std::cerr << "Error al iniciar PIN.\n";
		return 1;
	}

	// Instrumentación: insertar la función CountBranches en todas las instrucciones
	INS_AddInstrumentFunction(CountBranches, 0);

	// Función final: se ejecuta cuando el programa termina
	PIN_AddFiniFunction(Fini, 0);

	// Iniciar la ejecución del programa instrumentado
	PIN_StartProgram();

	return 0; // Este punto no se alcanza
}