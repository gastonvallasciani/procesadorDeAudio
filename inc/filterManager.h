/*
 * filterManager.h
 *
 *  Created on: 22/9/2018
 *      Author: gastonvallasciani
 */

#ifndef _FILTERMANAGER_H_
#define _FILTERMANAGER_H_

/*==================[inclusiones]============================================*/
#include "sapi.h"        //  sAPI lib
#include "stdint.h"

/*==================[definiciones de datos internos]=========================*/
/*==================[definiciones de datos externos]=========================*/
/*==================[definiciones de funciones publicas]=====================*/
int32_t continousFilterGain(uint8_t filterLength,int16_t *coeffVector);
void filterProcessing(uint8_t filterLength, int32_t continousGain,
		int16_t *coeffVector, uint16_t inputLength,
		int16_t *inputVector, int32_t *outputVector);
#endif /* _FILTERMANAGER_H_ */
/*==================[fin del archivo]========================================*/
