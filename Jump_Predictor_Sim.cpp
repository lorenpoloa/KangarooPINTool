#include "Jump_Predictor_Sim.h"

void BranchPredictor::Update(ADDRINT addr, BOOL actualTaken) {
    // Always Taken
    stats[ALWAYS_TAKEN].correct += actualTaken;
    stats[ALWAYS_TAKEN].total++;

    // Always Not Taken
    stats[ALWAYS_NOT_TAKEN].correct += !actualTaken;
    stats[ALWAYS_NOT_TAKEN].total++;

    // 1-bit Predictor
    bool pred1 = oneBitTable[addr];
    if (pred1 == actualTaken) stats[ONE_BIT].correct++;
    stats[ONE_BIT].total++;
    oneBitTable[addr] = actualTaken;

    // 2-bit Predictor
    UINT8& counter = twoBitTable[addr];
    bool pred2 = counter >= 2;
    if (pred2 == actualTaken) stats[TWO_BIT].correct++;
    stats[TWO_BIT].total++;
    if (actualTaken && counter < 3) counter++;
    else if (!actualTaken && counter > 0) counter--;
}

const PredictorStats* BranchPredictor::GetStats() const {
    return stats;
}
