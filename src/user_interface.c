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
#define LEVEL_LOW 		341
#define LEVEL_MID       682
#define LEVEL_HIGH      1023
/*=========================[definiciones de datos externos]=========================*/
extern audioProcessorFsmStruct_t audioProcessorFsmStruct;
/*=========================[declaraciones de funciones internas]====================*/
void ui_inputLevelDetector(uint16_t input);
/*=========================[definiciones de funciones publicas]=====================*/
/**
* @brief Configuracion de la interrupcion de nivel para la lectura de TEC1.
* @return None
*/
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
/**
* @brief Funcion de deteccion de nivel de la señal de entrada. Realiza la lectura y
* 		 enciende el led rojo, verde o amarillo segun el nivel.
* @param inputLength
* @return None
*/
uint8_t ui_VectorProcessor(uint16_t inputLength, int16_t *inputVector){
	uint16_t i;

	for (i=0; i<1500 ;i++){
		ui_inputLevelDetector(inputVector[i]);
	}
	return 1;
}
/*==========================[definiciones de funciones internas]=====================*/
/**
* @brief Funcion de deteccion de nivel de la señal de entrada. Realiza la lectura y
* 		 enciende el led rojo, verde o amarillo segun el nivel.
* @param input nivel de la señal de entrada
* @return None
*/
void ui_inputLevelDetector(uint16_t input)
{
	if((input >= 0) && (input < LEVEL_LOW))
	{
		gpioWrite( LED3, ON );
		gpioWrite( LED2, OFF );
		gpioWrite( LED1, OFF );
	}
	else if((input >= 341) && (input < LEVEL_MID))
	{
		gpioWrite( LED3, OFF );
		gpioWrite( LED2, ON );
		gpioWrite( LED1, OFF );
	}
	else if((input >= LEVEL_MID) && (input < LEVEL_HIGH))
	{
		gpioWrite( LED3, OFF );
		gpioWrite( LED2, OFF );
		gpioWrite( LED1, ON );
	}
}
/**
* @brief Handler de la interrupcion de nivel del pin conectado a la
* 		 TECLA 1
* @return None
*/
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



