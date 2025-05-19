#ifndef JUMP_PREDICTOR_SIM_H
#define JUMP_PREDICTOR_SIM_H

#include "pin.H"
#include <map>

/**
 * @enum PredictorType
 * @brief Enumeración de los tipos de predictores de salto simulados.
 *
 * Representa los distintos algoritmos de predicción que se implementan:
 * - ALWAYS_TAKEN: Siempre predice que el salto será tomado.
 * - ALWAYS_NOT_TAKEN: Siempre predice que el salto no será tomado.
 * - ONE_BIT: Predictor con un solo bit por dirección de salto.
 * - TWO_BIT: Predictor con contador saturado de 2 bits por dirección.
 * - NUM_PREDICTORS: Número total de predictores (utilizado como tamaño de array).
*/
enum PredictorType {
    ALWAYS_TAKEN,
    ALWAYS_NOT_TAKEN,
    ONE_BIT,
    TWO_BIT,
    NUM_PREDICTORS
};

/**
 * @struct PredictorStats
 * @brief Estructura para almacenar estadísticas de predicción.
 *
 * Almacena el número de aciertos y el número total de predicciones realizadas
 * por cada tipo de predictor.
 */
struct PredictorStats {
    UINT64 correct;
    UINT64 total;

	PredictorStats() : correct(0), total(0) {}
};

/**
 * @class BranchPredictor
 * @brief Clase que simula distintos tipos de predictores de salto.
 *
 * Esta clase mantiene las tablas de predicción para los algoritmos de 1-bit y 2-bit,
 * y actualiza estadísticas de precisión para cada tipo de predictor.
 */
class BranchPredictor {

/**
     * @brief Actualiza las estadísticas de predicción a partir de un nuevo salto.
     *
     * Esta función evalúa y actualiza los cuatro tipos de predictores implementados
     * según la dirección de la instrucción de salto y el resultado real del salto.
     *
     * @param addr Dirección de la instrucción de salto.
     * @param actualTaken Valor real del salto (`true` si fue tomado).
     */
public:
    void Update(ADDRINT addr, BOOL actualTaken);
    const PredictorStats* GetStats() const;

/**
     * @brief Devuelve las estadísticas de predicción acumuladas.
     *
     * @return Puntero constante a un arreglo de estructuras `PredictorStats` con los datos por predictor.
     */
private:
    std::map<ADDRINT, bool> oneBitTable;
    std::map<ADDRINT, UINT8> twoBitTable;
    PredictorStats stats[NUM_PREDICTORS];
};

#endif ///JUMP_PREDICTOR_SIM_H

