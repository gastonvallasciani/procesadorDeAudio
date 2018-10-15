/*
 * compressorManager.c
 *
 *  Created on: 8/10/2018
 *      Author: gastonvallasciani
 */
/*=========================[inclusiones]============================================*/
#include "compressorManager.h"
#include "sapi.h"
#include "sapi_timer.h"
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
compressorStruct_t compressorStruct;
/*=========================[declaracion de funciones internas]======================*/
void peakDetector(compressorStruct_t *compressorStruct, int16_t input);
void calculateMaxOutputCompresion(compressorStruct_t *compressorStruct, int16_t input);
uint16_t calculateSamples(compressorStruct_t *compressorStruct, timeType_t timeType);
/*=========================[definiciones de funciones internas]=====================*/
/**
* @brief Funcion que calcula la salida maxima en funcion de la relacion de compresion
* 		 del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param input muestra de la senal de entrada
* @return none
*/
void calculateMaxOutputCompresion(compressorStruct_t *compressorStruct, int16_t input){
	compressorStruct->outputMaxLevel = ((((uint16_t)input) - compressorStruct->umbral)/compressorStruct->compressorRatio) +
										   ((uint16_t)input);
}
/**
* @brief Funcion que detecta picos de amplitud mayores a un valor umbral
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param input muestra de la senal de entrada
* @return none
*/
void peakDetector(compressorStruct_t *compressorStruct, int16_t input){
	if(input < 0){
		input = input*(-1);
	}
	if(input > compressorStruct->umbral){
		if(compressorStruct->triggerState == DISABLE_STATE){
			calculateMaxOutputCompresion(compressorStruct, input);
			compressorStruct->triggerState = ATTACK_STATE;
			compressorStruct->currentSample = 0;
		}
	}
}
/**
* @brief Funcion que calcula la duracion en muestras del tiempo de ataque o release de
* 		 la compresion
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @return devuelve la cantidad de muestra del tiemp ode ataque o release de compresion
*/
uint16_t calculateSamples(compressorStruct_t *compressorStruct, timeType_t timeType){
	uint16_t attackSamples;

	switch(timeType){
	case ATTACK_TIME:
		attackSamples = compressorStruct->compressorAttackTime/22;
		break;
	case RELEASE_TIME:
		attackSamples = compressorStruct->compressorReleaseTime/22;
		break;
	default:
		break;
	}

	return attackSamples;
}
void compressorProccesor(compressorStruct_t *compressorStruct, int16_t input){
	if(compressorStruct->compressorStatus == ENABLE){
		peakDetector(compressorStruct, input);
		switch(compressorStruct->triggerState){
		case DISABLE_STATE:
			break;
		case ATTACK_STATE:
			if(compressorStruct->currentSample < compressorStruct->compressorSamplesAttackTime){
				compressorStruct->currentSample++;
			}
			else{
				compressorStruct->currentSample = 0;
				compressorStruct->triggerState = RELEASE_STATE;
			}
			break;
		case RELEASE_STATE:
			if(compressorStruct->currentSample < compressorStruct->compressorSamplesReleaseTime){
				compressorStruct->currentSample++;
			}
			else{
				compressorStruct->currentSample = 0;
				compressorStruct->triggerState = DISABLE_STATE;
			}
			break;
		default:
			break;
		}
	}
}
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que inicializa el compressor VCA con valores default
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @return none
*/
void compressorInit(compressorStruct_t *compressorStruct){
	compressorStruct->compressorStatus = ENABLE;
	compressorStruct->triggerState = DISABLE_STATE;
	compressorStruct->compressorRatio = 1;
	compressorStruct->umbral = 512;
}
/**
* @brief Funcion que deshabilita el compressor VCA con valores default
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @return none
*/
void compressorDeinit(compressorStruct_t *compressorStruct){
	compressorStruct->compressorStatus = DISABLE;
}
/**
* @brief Funcion que setea la relacion de compresion del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorRatio relacion de compresion a ser seteada en la estructura de manejo
* 		 del compresor
* @return none
*/
void setCompressorRatio(compressorStruct_t *compressorStruct, uint8_t compressorRatio){
	compressorStruct->compressorRatio = compressorRatio;
}
/**
* @brief Funcion que setea el umbral de compresion del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorUmbral umbral a ser seteado en la estructura de manejo del compresor
* @return none
*/
void setCompressorUmbral(compressorStruct_t *compressorStruct, uint8_t compressorUmbral){
	compressorStruct->umbral = compressorUmbral;
}
/**
* @brief Funcion que setea el tiempo de ataque del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorAttackTime setea el tiempo de ataque de compresion del compresor
* @return none
*/
void setCompressorAttackTime(compressorStruct_t *compressorStruct,
							 uint8_t compressorAttackTime){
	compressorStruct->compressorAttackTime = compressorAttackTime;
	compressorStruct->compressorSamplesAttackTime = calculateSamples(compressorStruct, ATTACK_TIME);
}
/**
* @brief Funcion que setea el tiempo de release del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorReleaseTime setea el tiempo de release de compresion del compresor
* @return none
*/
void setCompressorReleaseTime(compressorStruct_t *compressorStruct,
							 uint8_t compressorReleaseTime){
	compressorStruct->compressorReleaseTime = compressorReleaseTime;
	compressorStruct->compressorSamplesReleaseTime = calculateSamples(compressorStruct, RELEASE_TIME);
}
/*==========================[fin del archivo]========================================*/





