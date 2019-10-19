/*
 * user_interface.c
 *
 *  Created on: 14/10/2019
 *      Author: gastonvallasciani
 */
/*=========================[inclusiones]============================================*/
#include "user_interface.h"
#include "sapi.h"
#include "stdint.h"
#include "stdlib.h"
#include "audioProcessor.h"
/*=========================[definiciones de datos internos]=========================*/
#define FREE 0x00
#define DESCENDENTE 0x02
#define ASCENDENTE 0x03
/*=========================[definiciones de datos externos]=========================*/
extern audioProcessorFsmStruct_t audioProcessorFsmStruct;

/*=========================[declaraciones de funciones internas]====================*/

/*==========================[definiciones de funciones internas]=====================*/

/*=========================[definiciones de funciones publicas]=====================*/
void ui_Config(void){

	//CONFIGURACION
	/*Seteo la interrupción para el flanco descendente
	 *                channel, GPIOx, [y]    <- no es la config del pin, sino el nombre interno de la señal
	 *                      |  |      |    */
	Chip_SCU_GPIOIntPinSel( 0, 0,     4 ); //TEC1


	//Borra el pending de la IRQ
	// INT0 (canal 0 -> handler GPIO0)
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ));
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 0 ));
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 0 ));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH( 0 ));

	//Borra el clear pending de la IRQ y lo activa
	NVIC_ClearPendingIRQ( PIN_INT0_IRQn );
	NVIC_EnableIRQ( PIN_INT0_IRQn );
}

void GPIO0_IRQHandler(void){
	uint8_t status;
	status = getAudioProcessorFsmStatus(&audioProcessorFsmStruct);
	// TECLA 1
	if ( Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH(0) ) {
//		 RISING
		if(status == ENABLE)status = DISABLE;
		else if (status == DISABLE)status = ENABLE;

		setAudioProcessorFsmStatus(&audioProcessorFsmStruct, status);

		   Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT,PININTCH(0));
	 }
	 else{
//		 FALLING
		 Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT,PININTCH(0));
	 }

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,PININTCH( 0 ));
}

/*==========================[fin del archivo]========================================*/



