/*
 * audioProcessor.h
 *
 *  Created on: 15/6/2019
 *      Author: gastonvallasciani
 */
#ifndef _AUDIOPROCESSOR_H_
#define _AUDIOPROCESSOR_H_
/*=========================[inclusiones]============================================*/
#include "sapi.h"
/*==================[cplusplus]=====================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[macros]========================================================*/
/*==================[typedef]=======================================================*/
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[definiciones de funciones publicas]=====================*/
uint16_t calculateAudioMeanValue(uint16_t inputLength, uint16_t *inputVector);
uint8_t eliminateContinous(uint16_t inputLength, uint16_t *inputVector,
						   int16_t *outputVector, uint16_t continousValue);
uint8_t sumContinous(uint16_t inputLength, uint16_t *inputVector,
				     int16_t *outputVector, uint16_t continousValue);
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==========================[fin del archivo]========================================*/
#endif /* _AUDIOPROCESSOR_H_ */



