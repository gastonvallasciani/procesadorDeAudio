/**
  @file filterManager.c
  @brief Funciones de manejo de filtros FIR

  @author Gaston Vallasciani
  @date 22/09/2018
*/
/*=========================[inclusiones]============================================*/
#include "filterManager.h"
#include "sapi.h"        //  sAPI lib
#include "stdint.h"        //  sAPI lib
/*=========================[definiciones de datos internos]=========================*/
//#define FIXED_POINT_FILTER
#define FLOATING_POINT_FILTER
/*=========================[definiciones de datos externos]=========================*/
/**
 * @brief Definicion de filtros utilizados en el archivo main.c
 */
int16_t lpf15Khz[12] = {971, -1271, 1192, 148, -4008, 19777, 19777, -4008, 148,
	     	 	 	 1192, -1271, 971};
int16_t lpf4Khz[16] = {-346, -1078, -1283, -888, 836, 3502, 6343, 8158, 8158,
                     6343, 3502, 836, -888, -1283, -1078, -346};
int16_t hpf4Khz[17] = {-620, -785, -1187, -1635, -2092, -2512, -2851, -3072,
					 29620, -3072, -2851, -2512, -2092, -1635, -1187, -785,
					 -620};
/*=========================[declaraciones de funciones internas]====================*/
/*=========================[definiciones de funciones publicas]=====================*/
#ifdef FIXED_POINT_FILTER
/**
* @brief Funcion que calcula la ganancia de continua del filtro FIR
* @param filterLength cantidad de elementos del filtro FIR
* @param coeffVector puntero al vector de coeficientes del filtro FIR
* @return filterAcumulator ganancia de continua del filtro
*/
int32_t continousFilterGain(uint8_t filterLength,int16_t *coeffVector){
	int32_t filterAcumulator = 0;
	uint16_t filterCounter = 0;
	for(filterCounter = 0; filterCounter < filterLength; filterCounter++){
		filterAcumulator += coeffVector[filterCounter];
			}
	return filterAcumulator;
}
/**
* @brief Funcion manejadora de filtros FIR, procesa los datos de entrada sin FPU
* @param filterLength cantidad de elementos del filtro FIR
* @param continousGain ganancia de continua del filtro FIR
* @param coeffVector puntero al vector de coeficientes del filtro FIR
* @param inputLength cantidad de elementos del vector de entrada a procesar
* @param inputVector puntero al vector de entrada
* @param outputVector puntero al vector de salida
* @return empty
*/
void filterProcessing(uint8_t filterLength, int32_t continousGain,
										int16_t *coeffVector, uint16_t inputLength,
										int16_t *inputVector, int32_t *outputVector){
	int32_t filterAcumulator = 0;
	uint16_t filterCounter = 0,i;

	for(i = 0; i < inputLength;i++){
		for(filterCounter = 0; filterCounter < filterLength; filterCounter++){
			//paso de resolucion 16 bits del filtro a 10 bits que
			//seria la de la adquisicion del ADC
			filterAcumulator +=  (((coeffVector[filterCounter])/64) *
										(inputVector[i+filterCounter]));
		}
		outputVector[i] = filterAcumulator;
		filterAcumulator = 0;
	}
}
#else
	#ifdef FLOATING_POINT_FILTER
	/**
	* @brief Funcion que elimina la continua del vector de datos de entrada
	* @param inputLength cantidad de elementos del vector de entrada
	* @param inputVector puntero a los elementos del vector de entrada
	* @return 1 cuando se completa el procesamiento del vector
	*/
	uint8_t eliminateContinous(uint16_t inputLength, int16_t *inputVector){
		uint16_t counter;
		for(counter = 0; counter < inputLength; counter++){
			inputVector[counter] = inputVector[counter]-512;
				}
		return 1;
	}
	/**
	* @brief Funcion que calcula la ganancia de continua del filtro FIR
	* @param filterLength cantidad de elementos del filtro FIR
	* @param coeffVector puntero al vector de coeficientes del filtro FIR
	* @return filterAcumulator ganancia de continua del filtro
	*/
	int32_t continousFilterGain(uint8_t filterLength,int16_t *coeffVector){
		int32_t filterAcumulator = 0;
		uint16_t filterCounter = 0;
		for(filterCounter = 0; filterCounter < filterLength; filterCounter++){
			filterAcumulator += coeffVector[filterCounter];
				}
		return filterAcumulator;
	}
	/**
	* @brief Funcion manejadora de filtros FIR, procesa los datos de entrada con la FPU
	* @param filterLength cantidad de elementos del filtro FIR
	* @param continousGain ganancia de continua del filtro FIR
	* @param coeffVector puntero al vector de coeficientes del filtro FIR
	* @param inputLength cantidad de elementos del vector de entrada a procesar
	* @param inputVector puntero al vector de entrada
	* @param outputVector puntero al vector de salida
	* @return 1 cuando termino el procesamiento
	*/
	uint8_t filterProcessing(uint8_t filterLength, int32_t continousGain,
										int16_t *coeffVector, uint16_t inputLength,
										int16_t *inputVector, int16_t *outputVector){
		float filterAcumulator = 0, coefFilter = 0, filterGain = 0, inputVectorValue = 0;
		uint16_t filterCounter = 0,i;

		for(i = 0; i < inputLength;i++){
			for(filterCounter = 0; filterCounter < filterLength; filterCounter++){
				coefFilter = (float)(coeffVector[filterCounter]);
				filterGain = (float)(continousGain);
				inputVectorValue = (float)(inputVector[i+filterCounter]);

				filterAcumulator +=  ((coefFilter/(filterGain)) * (inputVectorValue)/1023);
			}
			outputVector[i] = (int16_t)(filterAcumulator*1023);
			filterAcumulator = 0;
		}
		return 1;
	}
	#endif
#endif
/*==========================[definiciones de funciones publicas]=====================*/

/*==========================[fin del archivo]========================================*/
