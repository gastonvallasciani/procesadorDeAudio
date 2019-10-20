/*
 * user_interface.h
 *
 *  Created on: 14/10/2019
 *      Author: gastonvallasciani
 */

#ifndef MISPROYECTOS_PROCESADORDEAUDIO_INC_USER_INTERFACE_H_
#define MISPROYECTOS_PROCESADORDEAUDIO_INC_USER_INTERFACE_H_

/*==================[inclusiones]============================================*/
#include "sapi.h"        //  sAPI lib
#include "core_cm4.h"

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*==================[definiciones de datos internos]=========================*/
/*==================[definiciones de datos externos]=========================*/
void ui_Config(void);
uint8_t ui_VectorProcessor(uint16_t inputLength, int16_t *inputVector);
/*==================[definiciones de funciones publicas]=====================*/
#ifdef __cplusplus
}
#endif
#endif /* MISPROYECTOS_PROCESADORDEAUDIO_INC_USER_INTERFACE_H_ */
