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
	ATTACK_TIME,
	RELEASE_TIME
}timeType_t;
typedef enum{
	DISABLE_STATE = 0,
	ATTACK_STATE,
	RELEASE_STATE
}triggerState_t;
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
	uint16_t compressorAttackTime;
	uint16_t compressorSamplesAttackTime;
	uint16_t compressorReleaseTime;
	uint16_t compressorSamplesReleaseTime;
	uint16_t outputMaxLevel;
	uint16_t inputMaxLevel;
}compressorStruct_t;
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[definiciones de funciones publicas]=====================*/
void compressorInit(compressorStruct_t *compressorStruct);
void compressorDeinit(compressorStruct_t *compressorStruct);
void setCompressorRatio(compressorStruct_t *compressorStruct, uint8_t compressorRatio);
void setCompressorUmbral(compressorStruct_t *compressorStruct,
						 uint8_t compressorUmbral);
void setCompressorAttackTime(compressorStruct_t *compressorStruct,
							 uint8_t compressorAttackTime);
void setCompressorReleaseTime(compressorStruct_t *compressorStruct,
							 uint8_t compressorReleaseTime);
void compressorProccesor(compressorStruct_t *compressorStruct, int16_t input);
/*==========================[fin del archivo]========================================*/
#endif /* _COMPRESSORMANAGER_H_ */
