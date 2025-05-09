#include "pin.H"
#include <iostream>
#include <fstream>

// Variables globales para el conteo de saltos
UINT64 jump_count = 0;

// Funci�n que se ejecuta cada vez que se encuentra un salto
VOID CountBranches(INS ins, VOID* v)
{
	// Comprobamos si la instrucci�n es un salto
	if (INS_IsBranch(ins)) {
		jump_count++;
	}
}

// Funci�n que se ejecuta al finalizar el programa
VOID Fini(INT32 code, VOID* v)
{
	// Imprimir el total de saltos en la salida
	std::cerr << "Total de saltos condicionales: " << jump_count << std::endl;
}

// Funci�n principal
int main(int argc, char* argv[])
{
	// Inicializaci�n de PIN
	if (PIN_Init(argc, argv)) {
		std::cerr << "Error al iniciar PIN.\n";
		return 1;
	}

	// Instrumentaci�n: insertar la funci�n CountBranches en todas las instrucciones
	INS_AddInstrumentFunction(CountBranches, 0);

	// Funci�n final: se ejecuta cuando el programa termina
	PIN_AddFiniFunction(Fini, 0);

	// Iniciar la ejecuci�n del programa instrumentado
	PIN_StartProgram();

	return 0; // Este punto no se alcanza
}