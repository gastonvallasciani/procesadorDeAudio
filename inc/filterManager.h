/**
  @file filterManager.h
  @brief Funciones de manejo de filtros FIR

  @author Gaston Vallasciani
  @date 22/09/2018
*/
#ifndef _FILTERMANAGER_H_
#define _FILTERMANAGER_H_
/*==================[inclusiones]============================================*/
#include "sapi.h"        //  sAPI lib
#include "stdint.h"
/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[definiciones de datos internos]=========================*/
/*==================[definiciones de datos externos]=========================*/
/*==================[definiciones de funciones publicas]=====================*/
uint8_t filterVectorProcessor(uint8_t filterLength, int16_t *coeffVector,
							  uint16_t inputLength, int16_t *inputVector,
							  int16_t *outputVector);
#endif /* _FILTERMANAGER_H_ */
/*==================[fin del archivo]========================================*/
