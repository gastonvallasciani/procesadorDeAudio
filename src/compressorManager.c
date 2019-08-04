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
compressorStruct_t 	   compressorStruct;
meanInputStruct_t 	   inputStruct, inputStructNeg;
/*=========================[declaracion de funciones internas]======================*/
void calculateMaxOutputCompresion(compressorStruct_t *compressorStruct, int16_t input);
uint16_t calculateSamples(compressorStruct_t *compressorStruct, timeType_t timeType,
		uint16_t timeBetweenSamplesInUs);
void calculateUpdateOutputPeriod(compressorStruct_t *compressorStruct, int16_t input);
uint8_t calculateMeanInput(meanInputStruct_t *inputStruct,
						   meanInputStruct_t *inputStructNeg, int16_t input);
void initCalculateMeanInput(meanInputStruct_t *inputStruct,
							meanInputStruct_t *inputStructNeg);
/*=========================[definiciones de funciones internas]=====================*/
void initCalculateMeanInput(meanInputStruct_t *inputStruct,
							meanInputStruct_t *inputStructNeg)
{
	inputStruct->currentSample = 0;
	inputStruct->accumulator = 0;
	inputStruct->meanInput = 0;

	inputStructNeg->currentSample = 0;
	inputStructNeg->accumulator = 0;
	inputStructNeg->meanInput = 0;
}
uint8_t calculateMeanInput(meanInputStruct_t *inputStruct,
						   meanInputStruct_t *inputStructNeg, int16_t input)
{
	if(input>0)
	{
		if(inputStruct->currentSample	<	MEAN_INPUT_SAMPLES_QUANTITY)
		{
			inputStruct->accumulator += (int32_t)input;
			inputStruct->currentSample++;
		}
		else if(inputStruct->currentSample == MEAN_INPUT_SAMPLES_QUANTITY)
		{

			inputStruct->accumulator += (int32_t)input;
			inputStruct->meanInput =
								(uint16_t)((inputStruct->accumulator)/((MEAN_INPUT_SAMPLES_QUANTITY+1)));
			inputStruct->currentSample = 0;
			inputStruct->accumulator = 0;
		}
		return(1);
	}
	else
	{
		if(inputStructNeg->currentSample	<	MEAN_INPUT_SAMPLES_QUANTITY)
		{
			inputStructNeg->accumulator += (int32_t)input;
			inputStructNeg->currentSample++;
		}
		else if(inputStructNeg->currentSample == MEAN_INPUT_SAMPLES_QUANTITY)
		{

			inputStructNeg->accumulator += (int32_t)input;
			inputStructNeg->meanInput =
								(uint16_t)(((-1)*inputStructNeg->accumulator)/(MEAN_INPUT_SAMPLES_QUANTITY+1));
			inputStructNeg->currentSample = 0;
			inputStructNeg->accumulator = 0;
		}
		return(0);
	}
}
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
	int16_t differenceInputOutMax;
/*
 * Hay que validar que la entrada sea distinta de outputMaxLevel porque sino se divide
 * por cero.
 */
	if (input!=(int16_t)compressorStruct->outputMaxLevel){

		differenceInputOutMax = input - (int16_t)(compressorStruct->outputMaxLevel);

		if(differenceInputOutMax > 0)
		{
			compressorStruct->compressorAttackTime.updateSamplePeriod =
					(uint16_t)(round((float)(compressorStruct->compressorAttackTime.samplesTime)
							 /(float)(differenceInputOutMax)));
			compressorStruct->compressorReleaseTime.updateSamplePeriod =
					(uint16_t)(round((float)(compressorStruct->compressorReleaseTime.samplesTime)
							/((float)(differenceInputOutMax))));
		}
		else
		{
			compressorStruct->compressorAttackTime.updateSamplePeriod =
					(uint16_t)(round((float)(compressorStruct->compressorAttackTime.samplesTime)
							 /(((float)(differenceInputOutMax))*(-1))));
			compressorStruct->compressorReleaseTime.updateSamplePeriod =
					(uint16_t)(round((float)(compressorStruct->compressorReleaseTime.samplesTime)
							/(((float)(differenceInputOutMax))*(-1))));
		}
	}
}
/**
* @brief Funcion que actualiza la maquina de estados de manejo del compressor
*
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param input puntero tension de entrada digitalizada por el ADC
*/
int16_t compressorProccesor(compressorStruct_t *compressorStruct, int16_t input,
		compressorDescriptor_t compressorDescriptor){
	float compensationGain;
	if(compressorStruct->compressorStatus == ENABLE){
		//if(compressorDescriptor == meanValueCompressor){
			//calculateMeanInput(&inputStruct, &inputStructNeg, input);
		//}
		//else{
			inputStruct.meanInput = input;
		//}
		switch(compressorStruct->triggerState){
		case DISABLE_STATE:
			if(input>0)
			{
				if((inputStruct.meanInput) > compressorStruct->umbral)
				{
					calculateMaxOutputCompresion(compressorStruct, input);
					calculateUpdateOutputPeriod(compressorStruct, input);
					compressorStruct->triggerState = ATTACK_STATE;
					compressorStruct->currentSample = 0;
					compressorStruct->compressorAttackTime.currentUpdateSample = 0;
					compressorStruct->compressorAttackTime.updateValue = 0;
				}
				else
				{
				}
			}
			else
			{
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
				if((inputStruct.meanInput) > compressorStruct->umbral)
				{
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
			//compressorStruct->currentSample = 0;
			//compressorStruct->triggerState = DISABLE_STATE;
			break;
		}
	}
	//compensationGain = round(((float)input)*compressorStruct->compensationGain);

	//return((int16_t)compensationGain);

	return(input);
}
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que inicializa el compressor con valores default
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
	initCalculateMeanInput(&inputStruct, &inputStructNeg);
}
/**
* @brief Funcion que deshabilita el compressor
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

/**
* @brief Funcion que setea la ganancia de compensacion del compresor
* @param compressorStruct puntero a la estructura de manejo de los parametros del
*        compresor
* @param compressorCompensationGain setea la ganancia de compensacion del compresor
* @return none
*/
void setCompressorCompensationGain(compressorStruct_t *compressorStruct,
		 float compressorCompensationGain)
{
	compressorStruct->compensationGain = compressorCompensationGain;
}

/**
* @brief Funcion que procesa un vector de muestras de entrada con el compresor de audio.
* @param inputLength Longitud del vector de entrada.
* @param inputVector Puntero al primer elemento del vector de entrada.
* @param outputVector Puntero al primer elemento del vector de salida.
* @return none
*/
uint8_t compressorVectorProcessor(uint16_t inputLength, int16_t *inputVector,
		int16_t *outputVector, compressorDescriptor_t compressorDescriptor)
{
	uint16_t i;

	for (i=0; i<inputLength ;i++){
		outputVector[i] =
				compressorProccesor(&compressorStruct,
									inputVector[i],
									compressorDescriptor);
	}
	return 1;
}
/*==========================[fin del archivo]========================================*/





