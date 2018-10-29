/*
 * compressorManager.h
 *
 *  Created on: 8/10/2018
 *      Author: gastonvallasciani
 */

#ifndef _COMPRESSORMANAGER_H_
#define _COMPRESSORMANAGER_H_
/*=========================[inclusiones]============================================*/
#include "sapi.h"
/*==================[cplusplus]=====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]========================================================*/
#define ACQUISITION_FRECUENCY_100KHZ() Timer_microsecondsToTicks( 10 )
/*==================[typedef]=======================================================*/
typedef enum{
	ATTACK_TIME = 0,
	RELEASE_TIME,
	HOLD_TIME
}timeType_t;
typedef enum{
	DISABLE_STATE = 0,
	ATTACK_STATE,
	HOLD_STATE,
	RELEASE_STATE
}triggerState_t;

typedef struct{
	timeType_t type;				//Tipo de tiempo(attack, hold o release)
	uint16_t timeValue;				//Tiempo en microsegundos
	uint16_t samplesTime; 			//Tiempo en microsegundos convertido a muestras
	uint8_t updateSamplePeriod;		//Tiempo de actualizacion de la salida del compresor
									//para cumplir con el tiempo de attack,hold o release
	uint16_t currentUpdateSample;
	uint16_t updateValue;
}timeStruct_t;
/**
 * IMPORTANTE: El compressorAttackTime y el compressorReleaseTime deben ser seteados en
 * microsegundos.
 */
typedef struct{
	uint8_t compressorStatus;
	triggerState_t triggerState;
	uint8_t compressorRatio;
	uint16_t umbral;
	uint16_t currentSample;
	uint16_t timeBetweenInputSamples;      //Tiempo de adquisicion del entrada, tiempo en microsegundos
	timeStruct_t compressorAttackTime;
	timeStruct_t compressorReleaseTime;
	timeStruct_t compressorHoldTime;
	uint16_t outputMaxLevel;			   //Maximo nivel de salida de la senial comprimida
	uint16_t inputMaxLevel;
}compressorStruct_t;
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[definiciones de funciones publicas]=====================*/
void compressorInit(compressorStruct_t *compressorStruct);
void compressorDeinit(compressorStruct_t *compressorStruct);
void setCompressorRatio(compressorStruct_t *compressorStruct, uint8_t compressorRatio);
void setCompressorUmbral(compressorStruct_t *compressorStruct,
						 uint16_t compressorUmbral);
void setCompressorAttackTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorAttackTime);
void setCompressorReleaseTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorReleaseTime);
void setCompressorHoldTime(compressorStruct_t *compressorStruct,
							 uint16_t compressorHoldTime);
int16_t compressorProccesor(compressorStruct_t *compressorStruct, int16_t input);
void setTimeBetweenInputSamples(compressorStruct_t *compressorStruct,
								uint16_t timeBetweenInputSamplesInUs);
uint8_t compressorVectorProcessor(uint16_t inputLength, int16_t *inputVector,
								  int16_t *outputVector);

/*==========================[fin del archivo]========================================*/
#endif /* _COMPRESSORMANAGER_H_ */