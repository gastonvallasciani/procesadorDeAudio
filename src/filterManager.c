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
static int16_t xBuffer[18];
/*=========================[definiciones de datos externos]=========================*/
/**
 * @brief Definicion de filtros utilizados en el archivo audioProcessor.c
 */
int16_t lpf15Khz[12] = {
						 971, -1271, 1192, 148, -4008, 19777, 19777, -4008, 148,
	     	 	 	     1192, -1271, 971
};
int16_t lpf4Khz[16] = {
						-346, -1078, -1283, -888, 836, 3502, 6343, 8158, 8158,
                        6343, 3502, 836, -888, -1283, -1078, -346
};
int16_t hpf4Khz[17] = {
						-620, -785, -1187, -1635, -2092, -2512, -2851, -3072,
					    29620, -3072, -2851, -2512, -2092, -1635, -1187, -785, -620
};
int16_t lpf2Khz[10] = {
						507, 1926, 3513, 5342, 6418, 6418, 5342, 3513, 1926, 507
};
int16_t hpf2Khz[11] = {
	     	 	 	 	1474, 66, -1879, -4740, -7322, 24408, -7322, -4740, -1879, 66,
						1474
};
/*=========================[declaraciones de funciones internas]====================*/
void shiftDelayLine(uint8_t htaps, int16_t *x);
int16_t filterProcessor(uint8_t htaps, int16_t *x, int16_t *h, int16_t inValue);
/*==========================[definiciones de funciones internas]=====================*/
/**
* @brief Funcion que genera un corrimiento en el vector de muestras
* 		 del filtro FIR
* @param htaps cantidad de elementos del filtro FIR
* @param x puntero al vector de entradas del filtro FIR
* @return none
*/
void shiftDelayLine(uint8_t htaps, int16_t *x){
	uint8_t i;
	for(i = htaps-1; i>0; i--){
		x[i]=x[i-1];
	}
}
/**
* @brief Funcion que calcula el filtrado en funcion
* 		 de 1 elemento ingresado
* @param htaps cantidad de elementos del filtro FIR
* @param continousGain ganancia de continua del filtro FIR
* @param x puntero al vector de entradas del filtro FIR
* @param h puntero al vector de coeficientes del filtro FIR
* @param inValue valor de entrada a ser filtrado
* @return valor filtrado
*/
int16_t filterProcessor(uint8_t htaps,int16_t *x, int16_t *h, int16_t inValue){
	uint8_t i;
	int64_t filterAcumulator = 0;
	shiftDelayLine(htaps,&x[0]);
	x[0] = inValue;

	for(i=0; i<htaps; i++)
	{
		filterAcumulator += (int64_t)(x[i]*h[i]);
	}
	filterAcumulator = (filterAcumulator) >> 16;
	return (((int16_t)(filterAcumulator)));
}
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que calcula el filtrado en funcion
* 	     de un vector de n elementos de entrada
* @param filterLength cantidad de elementos del filtro FIR
* @param continousGain ganancia de continua del filtro FIR
* @param coeffVector puntero al vector de coeficientes del filtro FIR
* @param inputLength longitud vector de entrada
* @param inputVector vector de muestras a ser filtrado
* @param outputVector  vector de salida filtrado
* @return 1 cuando se termina el procesamiento del vector de entrada
*/
uint8_t filterVectorProcessor(uint8_t filterLength, int16_t *coeffVector,
							  uint16_t inputLength, int16_t *inputVector,
							  int16_t *outputVector){
	uint16_t i;

	for (i=0; i<inputLength ;i++){
		outputVector[i] = filterProcessor(filterLength, &xBuffer[0], &coeffVector[0],
						  inputVector[i]);
	}
	return 1;
}
/*==========================[fin del archivo]========================================*/
