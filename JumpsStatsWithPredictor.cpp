#include "pin.H"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Jump_Predictor_Sim.h"
#include <map>

using std::string;
using std::map;
using std::ostream;
using std::ofstream;
using std::cerr;

BranchPredictor predictor;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "Archivo de salida");
ostream* out = &cerr;

VOID CountBranch(ADDRINT addr, BOOL taken) {
    predictor.Update(addr, taken);
}

VOID Instruction(INS ins, VOID* v) {
    if (INS_IsBranch(ins) && INS_HasFallThrough(ins)) {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CountBranch,
            IARG_INST_PTR,
            IARG_BRANCH_TAKEN,
            IARG_END);
    }
}

VOID Fini(INT32 code, VOID* v) {
    *out << "===== Resultados de Predictores de Salto =====\n";
    const char* names[] = { "Always Taken", "Always Not Taken", "1-bit", "2-bit" };

    const PredictorStats* stats = predictor.GetStats();
    for (int i = 0; i < NUM_PREDICTORS; ++i) {
        UINT64 correct = stats[i].correct;
        UINT64 total = stats[i].total;
        double accuracy = (total > 0) ? (100.0 * correct / total) : 0.0;

        *out << names[i]
             << "\n\tAciertos: " << correct
             << "\n\tTotal: " << total
             << "\n\tPrecisión: " << std::fixed << std::setprecision(2) << accuracy << endl;
    }
}

int main(int argc, char* argv[]) {
    if (PIN_Init(argc, argv)) {
        cerr << "Error al inicializar PIN.\n";
        return 1;
    }

    if (!KnobOutputFile.Value().empty()) {
        out = new ofstream(KnobOutputFile.Value().c_str());
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram(); // No retorna
    return 0;
}
