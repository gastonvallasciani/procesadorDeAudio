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
#include "clipperManager.h"
#include "compressorManager.h"
#include "stdlib.h"
/*=========================[definiciones de datos internos]=========================*/
audioProcessorStates_t 	  audioProcessorStates;
audioProcessorFsmStruct_t audioProcessorFsmStruct;

static int16_t firstOutputBuffer[AUDIO_VECTOR_SIZE];
static int16_t lowBandBuffer[AUDIO_VECTOR_SIZE];
static int16_t trebleBandBuffer[AUDIO_VECTOR_SIZE];


static filterData_t lpf, lowBand, trebleBand;

static IncreaseContinousValue = 0;
/*=========================[definiciones de datos externos]=========================*/
extern compressorStruct_t compressorStruct;
extern clipperStruct_t hardClipperStruct;;
/**
 * Vectores de coeficientes de filtros FIR definidos en el archivo
 * filterManager.c
 */
extern int16_t lpf15Khz[22];
extern int16_t lpf2Khz[10];
extern int16_t hpf2Khz[11];
/*=========================[declaracion de funciones internas]======================*/
uint8_t sumBands(uint16_t inputLength, int16_t *lowBandVector, int16_t *trebleVector,
				 int16_t *outputeVector);
/*=========================[definiciones de funciones internas]=====================*/

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
	setCompressorUmbral(&compressorStruct, 750);
	setTimeBetweenInputSamples(&compressorStruct, 23);
	setCompressorAttackTime(&compressorStruct, 3);
	setCompressorReleaseTime(&compressorStruct, 3);
/**
 * Calculo de la cantidad de elementos del filtro y de la ganancia de continua
 */
   lpf.filterSize = sizeof(lpf15Khz)/sizeof(int16_t);
   lowBand.filterSize = sizeof(lpf2Khz)/sizeof(int16_t);
   trebleBand.filterSize = sizeof(hpf2Khz)/sizeof(int16_t);
/**
 *  Inicio el driver del clipper
 */
   clipperInit(&hardClipperStruct);
   setClipperThreshold(&hardClipperStruct, 900);

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
				for(k=0;k<550000;k++)//432500 395000
				{

				}
				audioProcessorFsmStruct->actualState = GAIN_CONTROL;
				break;
			case GAIN_CONTROL:
				for(k=0; k < audioProcessorFsmStruct->vectorLength; k++)
				{
					firstOutputBuffer[k]= 2*audioProcessorFsmStruct->inputVector[k];
				}
				if(IncreaseContinousValue == 0)
				{
					IncreaseContinousValue = 1;
					audioProcessorFsmStruct->continousValue = audioProcessorFsmStruct->continousValue*2;
				}
				audioProcessorFsmStruct->actualState = LPF_15KHZ_FILTER;
				break;
			case LPF_15KHZ_FILTER:
//			    status = filterVectorProcessor(lpf.filterSize, &lpf15Khz[0],
//					   	   	   	     audioProcessorFsmStruct->vectorLength,
//									 &firstOutputBuffer[0],
//									 &firstOutputBuffer[0]);
				audioProcessorFsmStruct->actualState = PEAK_SYMMETRIZER;
				break;
			case PEAK_SYMMETRIZER:
				status = compressorVectorProcessor(audioProcessorFsmStruct->vectorLength,
										  &firstOutputBuffer[0],
										  &audioProcessorFsmStruct->outputVector[0],
										  meanValueCompressor,
										audioProcessorFsmStruct->continousValue);
				audioProcessorFsmStruct->actualState = BAND_SPLIT;
				break;
			case BAND_SPLIT:
//				status = filterVectorProcessor(lowBand.filterSize, &lpf2Khz[0],
//											   audioProcessorFsmStruct->vectorLength,
//											   &firstOutputBuffer[0],
//											   &lowBandBuffer[0]);
//				status = filterVectorProcessor(trebleBand.filterSize, &hpf2Khz[0],
//											   audioProcessorFsmStruct->vectorLength,
//											   &firstOutputBuffer[0],
//											   &trebleBandBuffer[0]);
				audioProcessorFsmStruct->actualState = SUM_BANDS;
				break;
			case SUM_BANDS:
//				sumBands(audioProcessorFsmStruct->vectorLength, &lowBandBuffer[0],
//						 &trebleBandBuffer[0], &firstOutputBuffer[0]);
				audioProcessorFsmStruct->actualState = CLIPPER;
				break;
			case CLIPPER:
				//hardClipperVectorProcessor(audioProcessorFsmStruct->vectorLength,
				//						   &firstOutputBuffer[0],
				//						   &audioProcessorFsmStruct->outputVector[0],
				//						   audioProcessorFsmStruct->continousValue);
				audioProcessorFsmStruct->actualState = AUDIO_PROCESSING_DELAY;
				break;
			}
		}
	}
}
/**
* @brief Realiza la suma de bandas del procesador de audio.
* @param inputLength cantidad de elementos del vector de entrada
* @param lowBandVector puntero al primer byte de los elementos del vector de la banda de los bajos
* @param trebleVector puntero al primer byte de los elementos del vector de la banda de los medios y agudos
* @param outputeVector puntero al primer byte de los elementos de salida que tiene ambas bandas sumadas
* @return 1
*/
uint8_t sumBands(uint16_t inputLength, int16_t *lowBandVector, int16_t *trebleVector,
				 int16_t *outputeVector)
{
	uint16_t i;
	int16_t inValue = 0;
	for(i = 0; i < inputLength; i++)
	{

		outputeVector[i] = lowBandVector[i] + trebleVector[i];

	}
	return(1);
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






