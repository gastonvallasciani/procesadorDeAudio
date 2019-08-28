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
#include "filterManager.h"
#include "compressorManager.h"
/*=========================[definiciones de datos internos]=========================*/
audioProcessorStates_t 	  audioProcessorStates;
audioProcessorFsmStruct_t audioProcessorFsmStruct;

static int16_t firstOutputBuffer[AUDIO_VECTOR_SIZE];
static int16_t lowBandBuffer[AUDIO_VECTOR_SIZE];
static int16_t trebleBandBuffer[AUDIO_VECTOR_SIZE];


static filterData_t lpf, lowBand, trebleBand;

/*=========================[definiciones de datos externos]=========================*/
extern compressorStruct_t compressorStruct;
/**
 * Vectores de coeficientes de filtros FIR definidos en el archivo
 * filterManager.c
 */
extern int16_t lpf15Khz[12];
extern int16_t lpf2Khz[10];
extern int16_t hpf2Khz[11];
extern int16_t lpf4Khz[16];
extern int16_t hpf4Khz[17];
/*=========================[declaracion de funciones internas]======================*/
/*=========================[definiciones de funciones internas]=====================*/
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
						   int16_t *outputVector, uint16_t continousValue)
{
	uint16_t counter;
	for(counter = 0; counter < inputLength; counter++)
	{
		outputVector[counter] = ((int16_t)(inputVector[counter]))+continousValue;
	}
	return 1;
}

/*=========================[definiciones de funciones publicas]=====================*/
void setAudioProcessorFsmStatus(audioProcessorFsmStruct_t *audioProcessorFsmStruct, uint8_t audioProcessorStatus)
{
	audioProcessorFsmStruct->audioProcessorStatus = audioProcessorStatus;
}
/**
* @brief Funcion de inicializacion de la maquina de estados que maneja el procesador
* 		 de audio. Inicialmente arranca habilitado para procesar. Tambien se inicializan
* 		 los compresores de audio y los filtros.
* @param *audioProcessorFsmStruct puntero a la estructura de manejo del procesador de audio
* @return None
*/
void initAudioProcessorFsm(audioProcessorFsmStruct_t *audioProcessorFsmStruct)
{
	audioProcessorFsmStruct->audioProcessorStatus = ENABLE;
	audioProcessorFsmStruct->actualState = AUDIO_PROCESSING_DELAY;
	audioProcessorFsmStruct->vectorLength = AUDIO_VECTOR_SIZE;
/**
 * Inicializo parametros del simetrizador de picos
 */
	compressorInit(&compressorStruct);
	setCompressorRatio(&compressorStruct, 2);
	setCompressorUmbral(&compressorStruct, 250);
	setTimeBetweenInputSamples(&compressorStruct, 23);
	setCompressorAttackTime(&compressorStruct, 10000);
	setCompressorHoldTime(&compressorStruct, 10000);
	setCompressorReleaseTime(&compressorStruct, 25000);
	setCompressorCompensationGain(&compressorStruct,1);
/**
 * Calculo de la cantidad de elementos del filtro y de la ganancia de continua
 */
   lpf.filterSize = sizeof(lpf15Khz)/sizeof(int16_t);
   lowBand.filterSize = sizeof(lpf2Khz)/sizeof(int16_t);
   trebleBand.filterSize = sizeof(hpf2Khz)/sizeof(int16_t);
}
/**
* @brief Funcion de actualizacion de la maquina de estados que maneja el procesador
* 		 de audio.
* @param *audioProcessorFsmStruct puntero a la estructura de manejo del procesador de audio
* @return None
*/
void updateAudioProcessorFsm(audioProcessorFsmStruct_t *audioProcessorFsmStruct)
{
	uint8_t status = 0;
	uint32_t k, stateCounter = 0;
	if(audioProcessorFsmStruct->audioProcessorStatus == ENABLE)
	{
		for(stateCounter = 0; stateCounter< 7;stateCounter++)
		{
			switch(audioProcessorFsmStruct->actualState)
			{
			case AUDIO_PROCESSING_DELAY:
				for(k=0;k<358500;k++)//432500 395000
				{

				}
				audioProcessorFsmStruct->actualState = ELIMINATE_CONTINOUS_LEVEL;
				break;
			case ELIMINATE_CONTINOUS_LEVEL:
				status = eliminateContinous(audioProcessorFsmStruct->vectorLength,
											&audioProcessorFsmStruct->inputVector[0],
											&firstOutputBuffer[0],
											audioProcessorFsmStruct->continousValue);
				audioProcessorFsmStruct->actualState = LPF_15KHZ_FILTER;
				break;
			case LPF_15KHZ_FILTER:
			   status = filterVectorProcessor(lpf.filterSize, &lpf15Khz[0],
					   	   	   	     audioProcessorFsmStruct->vectorLength,
									 &firstOutputBuffer[0], &firstOutputBuffer[0]);
				   audioProcessorFsmStruct->actualState = PEAK_SYMMETRIZER;
				break;
			case PEAK_SYMMETRIZER:
				status = compressorVectorProcessor(audioProcessorFsmStruct->vectorLength,
										  &firstOutputBuffer[0],
										  &firstOutputBuffer[0],
										  meanValueCompressor);
				audioProcessorFsmStruct->actualState = BAND_SPLIT;
				break;
			case BAND_SPLIT:
				status = filterVectorProcessor(lowBand.filterSize, &lpf2Khz[0],
											   audioProcessorFsmStruct->vectorLength,
											   &firstOutputBuffer[0], &lowBandBuffer[0]);
				status = filterVectorProcessor(trebleBand.filterSize, &hpf2Khz[0],
											   audioProcessorFsmStruct->vectorLength,
											   &firstOutputBuffer[0], &trebleBandBuffer[0]);
				audioProcessorFsmStruct->actualState = SUM_BANDS;
				break;
			case SUM_BANDS:
				for(k = 0; k < audioProcessorFsmStruct->vectorLength; k++)
				{
					firstOutputBuffer[k] = lowBandBuffer[k] + trebleBandBuffer[k];
				}
				audioProcessorFsmStruct->actualState = SUM_CONTINOUS_LEVEL;
				break;
			case SUM_CONTINOUS_LEVEL:
				status = sumContinous(audioProcessorFsmStruct->vectorLength,
									  &firstOutputBuffer[0],
									  &audioProcessorFsmStruct->outputVector[0],
									  audioProcessorFsmStruct->continousValue);
					audioProcessorFsmStruct->actualState = AUDIO_PROCESSING_DELAY;
				break;
			}
		}
	}
}
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
/*==========================[fin del archivo]========================================*/






