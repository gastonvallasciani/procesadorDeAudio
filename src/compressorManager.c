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
		samples = (compressorStruct->compressorAttackTime.timeValue*1000)/timeBetweenSamplesInUs;
		break;
	case RELEASE_TIME:
		samples = (compressorStruct->compressorReleaseTime.timeValue*1000)/timeBetweenSamplesInUs;
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

		differenceInputOutMax = (int16_t)(compressorStruct->outputMaxLevel) - input ;

		if(differenceInputOutMax < 0){differenceInputOutMax = differenceInputOutMax*(-1);}

		compressorStruct->compressorAttackTime.updateSamplePeriod =
				(uint16_t)(round((float)(compressorStruct->compressorAttackTime.samplesTime)
						 /(float)(differenceInputOutMax)));
		compressorStruct->compressorReleaseTime.updateSamplePeriod =
				(uint16_t)(round((float)(compressorStruct->compressorReleaseTime.samplesTime)
						/((float)(differenceInputOutMax))));
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
		compressorDescriptor_t compressorDescriptor, uint16_t audioMeanValue)
{
	if(compressorStruct->compressorStatus == ENABLE)
	{
		calculateMaxOutputCompresion(compressorStruct, input);
		switch(compressorStruct->triggerState)
		{
		case DISABLE_STATE:
			if((input) > compressorStruct->umbral)
			{
				compressorStruct->triggerState = ATTACK_STATE;
				calculateUpdateOutputPeriod(compressorStruct, input);
			}
			break;
		case ATTACK_STATE:
			compressorStruct->currentSample++;
			compressorStruct->compressorAttackTime.currentUpdateSample++;
			if(compressorStruct->compressorAttackTime.currentUpdateSample ==
					compressorStruct->compressorAttackTime.updateSamplePeriod)
			{
				compressorStruct->compressorFactor++;
				compressorStruct->compressorAttackTime.currentUpdateSample = 0;
			}
			if(input >= audioMeanValue)
			{
				input = input - compressorStruct->compressorFactor;
			}
			else
			{
				input = input + compressorStruct->compressorFactor;
			}

			if (compressorStruct->currentSample ==
					compressorStruct->compressorAttackTime.samplesTime)
			{
				compressorStruct->compressorAttackTime.currentUpdateSample = 0;
				compressorStruct->currentSample = 0;
				compressorStruct->triggerState = RELEASE_STATE;
			}
			break;
		case RELEASE_STATE:
			compressorStruct->compressorReleaseTime.currentUpdateSample++;
			compressorStruct->currentSample++;
			if(compressorStruct->compressorReleaseTime.currentUpdateSample ==
					compressorStruct->compressorReleaseTime.updateSamplePeriod)
			{
			   if (compressorStruct->compressorFactor != 0)
			   {
				   compressorStruct->compressorFactor--;
			   }
			   else
			   {
				   compressorStruct->compressorFactor = 0;
			   }
			   compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
			}
			if(input >= audioMeanValue)
			{
				input = input - compressorStruct->compressorFactor;
			}
			else
			{
				input = input + compressorStruct->compressorFactor;
			}
			if(compressorStruct->currentSample ==
					compressorStruct->compressorReleaseTime.samplesTime)
			{
				compressorStruct->compressorReleaseTime.currentUpdateSample = 0;
				compressorStruct->currentSample = 0;
				compressorStruct->compressorFactor = 0;
				compressorStruct->triggerState = DISABLE_STATE;
			}
			break;
		}
	}
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
	compressorStruct->compressorFactor = 0;
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
* @brief Funcion que procesa un vector de muestras de entrada con el compresor de audio.
* @param inputLength Longitud del vector de entrada.
* @param inputVector Puntero al primer elemento del vector de entrada.
* @param outputVector Puntero al primer elemento del vector de salida.
* @return none
*/
uint8_t compressorVectorProcessor(uint16_t inputLength, int16_t *inputVector,
		int16_t *outputVector, compressorDescriptor_t compressorDescriptor,
		uint16_t audioMeanValue)
{
	uint16_t i;

	for (i=0; i<inputLength ;i++){
		outputVector[i] = compressorProccesor(&compressorStruct, inputVector[i],
											  compressorDescriptor, audioMeanValue);
	}
	return 1;
}
/*==========================[fin del archivo]========================================*/





