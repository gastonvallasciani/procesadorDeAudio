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
#include "program.h"
#include "math.h"
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
compressorStruct_t compressorStruct;
/*=========================[declaracion de funciones internas]======================*/
void calculateMaxOutputCompresion(compressorStruct_t *compressorStruct, int16_t input);
uint16_t calculateSamples(compressorStruct_t *compressorStruct, timeType_t timeType,
		uint16_t timeBetweenSamplesInUs);
void calculateUpdateOutputPeriod(compressorStruct_t *compressorStruct, int16_t input);
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
	compressorStruct->outputMaxLevel = ((((uint16_t)input) -
			            compressorStruct->umbral)/compressorStruct->compressorRatio) +
			            compressorStruct->umbral;
}
/**
* @brief Funcion que calcula la duracion en muestras del tiempo de ataque o release de
* 		 la compresion
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @return devuelve la cantidad de muestra del tiempo de ataque o release de compresion
*/
uint16_t calculateSamples(compressorStruct_t *compressorStruct, timeType_t timeType,
						  uint16_t timeBetweenSamplesInUs){
	uint16_t samples;

	switch(timeType){
	case ATTACK_TIME:
		samples = compressorStruct->compressorAttackTime.timeValue/timeBetweenSamplesInUs;
		break;
	case RELEASE_TIME:
		samples = compressorStruct->compressorReleaseTime.timeValue/timeBetweenSamplesInUs;
		break;
	case HOLD_TIME:
		samples = compressorStruct->compressorHoldTime.timeValue/timeBetweenSamplesInUs;
		break;
	default:
		break;
	}
	return samples;
}
/**
* @brief Funcion que calcula el periodo con el que se actualiza la salida para cumplir
* 		 con el tiempo de ataque, de mantenimiento o de liberacion del compresor
*
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param input puntero tension de entrada digitalizada por el ADC
*/
void calculateUpdateOutputPeriod(compressorStruct_t *compressorStruct, int16_t input){
	uint16_t differenceInputOutMax;
/*
 * Hay que validar que la entrada sea distinta de outputMaxLevel porque sino se divide
 * por cero.
 */
	if ((input>0)&&(input!=compressorStruct->outputMaxLevel)){
		differenceInputOutMax = (uint16_t)(input)- compressorStruct->outputMaxLevel;
		compressorStruct->compressorAttackTime.updateSamplePeriod =
				(uint16_t)(round((float)(compressorStruct->compressorAttackTime.samplesTime)
						/(float)(differenceInputOutMax)));
		compressorStruct->compressorReleaseTime.updateSamplePeriod =
				(uint16_t)(round((float)(compressorStruct->compressorReleaseTime.samplesTime)
						/(float)(differenceInputOutMax)));
	}
}
/**
* @brief Funcion que actualiza la maquina de estados de manejo del compressor
*
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param input puntero tension de entrada digitalizada por el ADC
*/
int16_t compressorProccesor(compressorStruct_t *compressorStruct, int16_t input){
	if(compressorStruct->compressorStatus == ENABLE){
		switch(compressorStruct->triggerState){
		case DISABLE_STATE:
			if(input > (int16_t)(compressorStruct->umbral)){
				calculateMaxOutputCompresion(compressorStruct, input);
				calculateUpdateOutputPeriod(compressorStruct, input);
				compressorStruct->triggerState = ATTACK_STATE;
				compressorStruct->currentSample = 0;
				compressorStruct->compressorAttackTime.currentUpdateSample = 0;
				compressorStruct->compressorAttackTime.updateValue = 0;
			}
			break;
		case ATTACK_STATE:
			if(compressorStruct->currentSample < compressorStruct->compressorAttackTime.samplesTime){
				compressorStruct->currentSample++;
				compressorStruct->compressorAttackTime.currentUpdateSample++;
				if(compressorStruct->compressorAttackTime.currentUpdateSample ==
						compressorStruct->compressorAttackTime.updateSamplePeriod){
					compressorStruct->compressorAttackTime.updateValue++;
					compressorStruct->compressorAttackTime.currentUpdateSample = 0;
				}
				if(input > 0){
					input = input - (int16_t)(compressorStruct->compressorAttackTime.updateValue);
				}
				else{
					input = input + (int16_t)(compressorStruct->compressorAttackTime.updateValue);
				}
			}
			else{
				if(input > 0){
					input = input - (int16_t)(compressorStruct->compressorAttackTime.updateValue);
				}
				else{
					input = input + (int16_t)(compressorStruct->compressorAttackTime.updateValue);
				}
				compressorStruct->currentSample = 0;
				compressorStruct->compressorAttackTime.currentUpdateSample = 0;
				compressorStruct->compressorHoldTime.updateValue =
						compressorStruct->compressorAttackTime.updateValue;
				compressorStruct->compressorReleaseTime.updateValue =
						compressorStruct->compressorAttackTime.updateValue;
				compressorStruct->compressorAttackTime.updateValue = 0;
				compressorStruct->triggerState = HOLD_STATE;
			}
			break;
		case HOLD_STATE:
			if(compressorStruct->currentSample < compressorStruct->compressorHoldTime.samplesTime){
				compressorStruct->currentSample++;
				if(input > 0){
					input = input - (int16_t)(compressorStruct->compressorHoldTime.updateValue);
				}
				else{
					input = input + (int16_t)(compressorStruct->compressorHoldTime.updateValue);
				}
			}
			else{
				if(input > ((int16_t)compressorStruct->umbral)||(input<((-1)*((int16_t)compressorStruct->umbral)))){
					if(input > 0){
						input = input - (int16_t)(compressorStruct->compressorHoldTime.updateValue);
					}
					else{
						input = input + (int16_t)(compressorStruct->compressorHoldTime.updateValue);
					}
					compressorStruct->currentSample = 0;
					compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
					compressorStruct->triggerState = HOLD_STATE;
				}
				else{
					if(input > 0){
						input = input - (int16_t)(compressorStruct->compressorHoldTime.updateValue);
					}
					else{
						input = input + (int16_t)(compressorStruct->compressorHoldTime.updateValue);
					}
					compressorStruct->currentSample = 0;
					compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
					compressorStruct->compressorHoldTime.updateValue = 0;
					compressorStruct->triggerState = RELEASE_STATE;
				}
			}
			break;
		case RELEASE_STATE:
			if(compressorStruct->currentSample < compressorStruct->compressorReleaseTime.samplesTime){
				compressorStruct->currentSample++;
				compressorStruct->compressorReleaseTime.currentUpdateSample++;
				if(compressorStruct->compressorReleaseTime.currentUpdateSample ==
						compressorStruct->compressorReleaseTime.updateSamplePeriod){
					compressorStruct->compressorReleaseTime.updateValue--;
					compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
				}
				if(input > 0){
					input = input - (int16_t)(compressorStruct->compressorReleaseTime.updateValue);
				}
				else{
					input = input + (int16_t)(compressorStruct->compressorReleaseTime.updateValue);
				}
			}
			else{
				if(input > 0){
					input = input - (int16_t)(compressorStruct->compressorReleaseTime.updateValue);
				}
				else{
					input = input + (int16_t)(compressorStruct->compressorReleaseTime.updateValue);
				}
				compressorStruct->currentSample = 0;
				compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
				compressorStruct->compressorReleaseTime.updateValue = 0;
				compressorStruct->triggerState = DISABLE_STATE;
			}
			break;
		default:
			compressorStruct->currentSample = 0;
			compressorStruct->triggerState = DISABLE_STATE;
			break;
		}
	}
	return input;
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
	compressorStruct->timeBetweenInputSamples = TIME_BETWEEN_SAMPLES_IN_US;
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
void setCompressorUmbral(compressorStruct_t *compressorStruct, uint16_t compressorUmbral){
	compressorStruct->umbral = compressorUmbral;
}
/**
* @brief Funcion que setea el tiempo entre muestras de entrada que usa el compresor para trabajar
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param timeBetweenInputSamplesInUs tiempo entre muestras de entrada en microsegundos
* @return none
*/
void setTimeBetweenInputSamples(compressorStruct_t *compressorStruct,
		uint16_t timeBetweenInputSamplesInUs){
	compressorStruct->timeBetweenInputSamples = timeBetweenInputSamplesInUs;
}
/**
* @brief Funcion que setea el tiempo de ataque del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorAttackTime setea el tiempo de ataque de compresion del compresor
* @return none
*/
void setCompressorAttackTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorAttackTime){
	compressorStruct->compressorAttackTime.type = ATTACK_TIME;
 	compressorStruct->compressorAttackTime.timeValue = compressorAttackTime;
	compressorStruct->compressorAttackTime.samplesTime = calculateSamples(compressorStruct,
			compressorStruct->compressorAttackTime.type, compressorStruct->timeBetweenInputSamples);
}

/**
* @brief Funcion que setea el tiempo de mantenimiento del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorHoldTime setea el tiempo de mantenimiento del compresor
* @return none
*/
void setCompressorHoldTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorHoldTime){
	compressorStruct->compressorHoldTime.type = HOLD_TIME;
	compressorStruct->compressorHoldTime.timeValue = compressorHoldTime;
	compressorStruct->compressorHoldTime.samplesTime = calculateSamples(compressorStruct,
			compressorStruct->compressorHoldTime.type, compressorStruct->timeBetweenInputSamples);
}

/**
* @brief Funcion que setea el tiempo de release del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorReleaseTime setea el tiempo de release de compresion del compresor
* @return none
*/
void setCompressorReleaseTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorReleaseTime){
	compressorStruct->compressorReleaseTime.type = RELEASE_TIME;
	compressorStruct->compressorReleaseTime.timeValue = compressorReleaseTime;
	compressorStruct->compressorReleaseTime.samplesTime = calculateSamples(compressorStruct,
			compressorStruct->compressorReleaseTime.type, compressorStruct->timeBetweenInputSamples);

}
uint8_t compressorVectorProcessor(uint16_t inputLength, int16_t *inputVector, int16_t *outputVector){
	uint16_t i;

	for (i=0; i<inputLength ;i++){
		outputVector[i] = compressorProccesor(&compressorStruct, inputVector[i]);
	}
	return 1;
}
/*==========================[fin del archivo]========================================*/





