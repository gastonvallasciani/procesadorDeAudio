/*
 * audioProcessor.c
 *
 *  Created on: 15/6/2019
 *      Author: gastonvallasciani
 */
/*=========================[inclusiones]============================================*/
#include "audioProcessor.h"
#include "sapi.h"
#include "sapi_timer.h"
#include "program.h"
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[declaracion de funciones internas]======================*/
/*=========================[definiciones de funciones internas]=====================*/
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Calcula el valor medio de la senial de audio en funcion del vector de entrada
* 		 para corregir el valor medio de la senial con la funcion eliminateContinous
* 		 posteriormente
* @param inputLength cantidad de elementos del vector de entrada
* @param inputVector puntero al primer byte de los elementos del vector de entrada
* @return accumulator valor medio de la senial de audio
*/
uint16_t calculateAudioMeanValue(uint16_t inputLength, uint16_t *inputVector)
{
	uint16_t index;
	uint32_t accumulator = 0;

	for(index = 0; index < inputLength; index++)
	{
		accumulator += inputVector[index];
	}
	accumulator = accumulator/inputLength;

	return((uint16_t)accumulator);
}
/**
* @brief Funcion que elimina la continua del vector de datos de entrada
* @param inputLength cantidad de elementos del vector de entrada
* @param inputVector puntero al primer byte de los elementos del vector de entrada
* @param outputVector puntero al primer byte de los elementos del vector de salida
* @return 1 cuando se completa el procesamiento del vector
*/
uint8_t eliminateContinous(uint16_t inputLength, uint16_t *inputVector,
						   int16_t *outputVector, uint16_t continousValue){
	uint16_t counter;
	for(counter = 0; counter < inputLength; counter++){
		outputVector[counter] = ((int16_t)(inputVector[counter]))-continousValue;
			}
	return 1;
}
/**
* @brief Funcion que suma el valor de continua restado a la entrada al vector de
* 		 datos de salida
* @param inputLength cantidad de elementos del vector de entrada a ser incrementados
* @param inputVector puntero al primer byte de los elementos del vector de entrada
* @param outputVector puntero al primer byte de los elementos del vector de salida
* @return 1 cuando se completa el procesamiento del vector
*/
uint8_t sumContinous(uint16_t inputLength, uint16_t *inputVector,
						   int16_t *outputVector, uint16_t continousValue){
	uint16_t counter;
	for(counter = 0; counter < inputLength; counter++){
		outputVector[counter] = ((int16_t)(inputVector[counter]))+continousValue;
			}
	return 1;
}
/*==========================[fin del archivo]========================================*/






