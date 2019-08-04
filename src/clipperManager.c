/*
 * clipperManager.c
 *
 *  Created on: 29/6/2019
 *      Author: gastonvallasciani
 */
/*=========================[inclusiones]============================================*/
#include "clipperManager.h"
#include "sapi.h"
#include "sapi_timer.h"
#include "program.h"
/*=========================[definiciones de datos internos]=========================*/
clipperStruct_t hardClipperStruct;
/*=========================[definiciones de datos externos]=========================*/
/*=========================[declaracion de funciones internas]======================*/
int16_t hardClipperProcessor(clipperStruct_t *clipperStruct, int16_t input);
/*=========================[definiciones de funciones internas]=====================*/
/**
* @brief Funcion que actualiza la maquina de estados de manejo del hard clipper.
*
* @param clipperStruct puntero a la estructura de manejo de los parametros del
*        hard clipper.
* @param input puntero valor a procesar.
* @return input devuelve la entrada procesada.
*/
int16_t hardClipperProcessor(clipperStruct_t *clipperStruct, int16_t input)
{
	if(input > 0)
	{
		if(input > clipperStruct->threshold)
		{
			input = clipperStruct->threshold;
		}
	}
	else
	{
		if(input < (clipperStruct->threshold*(-1)))
		{
			input = clipperStruct->threshold;
		}

	}
	return(input);
}
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que inicializa el clipper
* @param clipperStruct puntero a la estructura de manejo de los parametros del
*        clipper
* @return none
*/
void clipperInit(clipperStruct_t *clipperStruct)
{
	clipperStruct->status = ENABLE;
	// se setea en 2 volt inicialmente, (1024 = 3.3v)
	clipperStruct->threshold = 620;
}
/**
* @brief Funcion que deshabilita el clipper.
* @param clipperStruct puntero a la estructura de manejo de los parametros del
*        clipper.
* @return none
*/
void clipperDeInit(clipperStruct_t *clipperStruct)
{
	clipperStruct->status = DISABLE;
}
/**
* @brief Funcion que setea el umbral a partir del cual actua el clipper.
* @param clipperStruct puntero a la estructura de manejo de los parametros del
*        clipper.
* @param threshold umbral a partir del cual actua el clipper.
* @return none
*/
void setClipperThreshold(clipperStruct_t *clipperStruct, int16_t threshold)
{
	clipperStruct->threshold = threshold;
}
/**
* @brief Funcion que procesa un vector de muestras de entrada con el hard clipper.
* @param inputLength Longitud del vector de entrada.
* @param inputVector Puntero al primer elemento del vector de entrada.
* @param outputVector Puntero al primer elemento del vector de salida.
* @return 1 si el clipper esta habilitado.
* @return 0 si el clipper no esta habilitado.
*/
uint8_t hardClipperVectorProcessor(uint16_t inputLength, int16_t *inputVector,
		int16_t *outputVector)
{
	uint16_t i;

	if(hardClipperStruct.status == ENABLE)
	{
		for (i=0; i<inputLength ;i++)
		{
			outputVector[i] = hardClipperProcessor(&hardClipperStruct, inputVector[i]);
		}
		return 1;
	}
	return 0;
}
/*==========================[fin del archivo]========================================*/





