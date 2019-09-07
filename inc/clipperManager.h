/*
 * clipperManager.h
 *
 *  Created on: 29/6/2019
 *      Author: gastonvallasciani
 */

#ifndef MISPROYECTOS_PROCESADORDEAUDIO_INC_CLIPPERMANAGER_H_
#define MISPROYECTOS_PROCESADORDEAUDIO_INC_CLIPPERMANAGER_H_
/*=========================[inclusiones]============================================*/
#include "sapi.h"
/*==================[cplusplus]=====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]========================================================*/
/*==================[typedef]=======================================================*/
typedef struct{
	int16_t threshold;
	uint8_t status;
}clipperStruct_t;
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[declaracion de funciones publicas]======================*/
void clipperInit(clipperStruct_t *clipperStruct);
void clipperDeInit(clipperStruct_t *clipperStruct);
void setClipperThreshold(clipperStruct_t *clipperStruct, int16_t threshold);
uint8_t hardClipperVectorProcessor(uint16_t inputLength, int16_t *inputVector,
								   int16_t *outputVector, uint16_t audioMeanValue);
/*==================[c++]===========================================================*/
#ifdef __cplusplus
}
#endif
#endif /* MISPROYECTOS_PROCESADORDEAUDIO_INC_CLIPPERMANAGER_H_ */
