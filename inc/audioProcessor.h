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
#define AUDIO_VECTOR_SIZE 1500
/*==================[typedef]=======================================================*/
/**
 * Brief parametros
 */
typedef struct{
	uint8_t filterSize;
}filterData_t;

/**
 * Brief estados de la maquina de estados general del procesador de audio
 */
typedef enum{
	AUDIO_PROCESSING_DELAY    = 0,
	ELIMINATE_CONTINOUS_LEVEL = 1,
	LPF_15KHZ_FILTER		  = 2,
	PEAK_SYMMETRIZER		  = 3,
	BAND_SPLIT				  = 4,
	SUM_BANDS                 = 5,
	SUM_CONTINOUS_LEVEL		  = 6
}audioProcessorStates_t;
/**
 * Brief estructura general de la maquina de estados general del procesador de audio
 */
typedef struct{
	uint8_t				   audioProcessorStatus;
	audioProcessorStates_t actualState;
	uint16_t			   *inputVector;
	uint16_t			   vectorLength;
	uint16_t			   *outputVector;
	uint16_t			   continousValue;
}audioProcessorFsmStruct_t;
/*=========================[definiciones de datos internos]=========================*/
/*=========================[definiciones de datos externos]=========================*/
/*=========================[definiciones de funciones publicas]=====================*/
void initAudioProcessorFsm(audioProcessorFsmStruct_t *audioProcessorFsmStruct);
void updateAudioProcessorFsm(audioProcessorFsmStruct_t *audioProcessorFsmStruct);
uint16_t calculateAudioMeanValue(uint16_t inputLength, uint16_t *inputVector);
uint8_t eliminateContinous(uint16_t inputLength, uint16_t *inputVector,
						   int16_t *outputVector, uint16_t continousValue);
uint8_t sumContinous(uint16_t inputLength, uint16_t *inputVector,
				     int16_t *outputVector, uint16_t continousValue);
void setAudioProcessorFsmStatus(audioProcessorFsmStruct_t *audioProcessorFsmStruct,
								uint8_t audioProcessorStatus);
/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif
/*==========================[fin del archivo]========================================*/
#endif /* _AUDIOPROCESSOR_H_ */



