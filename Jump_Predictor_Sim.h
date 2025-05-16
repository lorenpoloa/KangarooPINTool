#ifndef JUMP_PREDICTOR_SIM_H
#define JUMP_PREDICTOR_SIM_H

#include "pin.H"
#include <map>

enum PredictorType {
    ALWAYS_TAKEN,
    ALWAYS_NOT_TAKEN,
    ONE_BIT,
    TWO_BIT,
    NUM_PREDICTORS
};

struct PredictorStats {
    static UINT64 correct = 0;
    static UINT64 total = 0;
};

class BranchPredictor {
public:
    void Update(ADDRINT addr, BOOL actualTaken);
    const PredictorStats* GetStats() const;

private:
    std::map<ADDRINT, bool> oneBitTable;
    std::map<ADDRINT, UINT8> twoBitTable;
    PredictorStats stats[NUM_PREDICTORS];
};

#endif // JUMP_PREDICTOR_SIM_H
