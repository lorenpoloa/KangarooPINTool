#include "Jump_Predictor_Sim.h"

/**
 * @brief Actualiza las estadísticas de todos los predictores de salto.
 *
 * Esta función simula cuatro tipos de predictores:
 * - Always Taken: siempre predice que el salto será tomado.
 * - Always Not Taken: siempre predice que el salto NO será tomado.
 * - Predictor de 1 bit: mantiene una predicción por dirección, que cambia directamente según el último resultado.
 * - Predictor de 2 bits: utiliza una máquina de estados de 2 bits.
 *
 * @param addr Dirección de la instrucción de salto (utilizada como índice en las tablas de predicción).
 * @param actualTaken Booleano para estado del salto (`true` si fue tomado, `false` si no).
 */
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

/**
 * @brief Obtiene las estadísticas actuales de los predictores.
 *
 * @return Puntero para un arreglo que almacena las estadísticas (`PredictorStats`) para cada tipo de predictor.
 */
const PredictorStats* BranchPredictor::GetStats() const {
    return stats;
}
