/**
  @file ADC_proxyClient.c
  @brief Modulo de abstraccion entre la adquisicion del ADC y el programa

  @author Gaston Vallasciani
  @date 10/06/2018
*/
/*==================[inclusiones]============================================*/
#include "ADC_hardwareProxy.h"
#include "ADC_proxyClient.h"
#include "sapi.h"
#include "board.h"
#include "uart.h"
#include "sapi_peripheral_map.h"
#include "core_cm4.h"
/*==================[macros and definitions]=================================*/
/*==================[definiciones de datos internos]=========================*/
adcProxyClient_t adcStruct;
extern uint16_t indexReadRx, indexWriteRx, bufferRx[RX_BUFFER_LENGTH_ADC];
/*==================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que inicializa el ADC.
* 		 Default: Fs = 400KHz, Resolucion = 10bits
* 		 Canal Izquierdo ADC_CH1.
* 		 Canal Drecho = ADC_CH2.
* @return none
*/
void ADCPROXYCLIENT_initialize(void){
	ADCHARDWAREPROXY_initialize(CHANNEL0);
	adcStruct.adcSampleRate = AUDIO_SAMPLE_RATE;
	adcStruct.adcResolution = ADC_10BITS;
	adcStruct.adcRightChannel = ADC_CH1;
	adcStruct.adcLeftChannel = ADC_CH2;
}
/**
* @brief Funcion publica de configuracion de la adquisicion por ADC.
* @return none
*/
void ADCPROXYCLIENT_config(void){
	ADCHARDWAREPROXY_config(CHANNEL0, BURST_MODE, adcStruct.adcRightChannel, adcStruct.adcSampleRate, adcStruct.adcResolution);
}
/**
* @brief Funcion que deshabilita el ADC.
* @return none
*/
void ADCPROXYCLIENT_disable(void){
	ADCHARDWAREPROXY_disable(CHANNEL0);
}
/**
* @brief Funcion que permite actualizar el buffer circular de adquisicion
* 		 del ADC y levantar un dato del mismo.
* @param action adcUpdateValue: permite actualizar el buffer circular
* 				adcGetValue: permite levantar un dato del buffer
* @param dataAcquired puntero a la variable de adquisicion
* @return estado del buffer circular
*/
uint8_t ADCPROXYCLIENT_access(accessAction_t  action, uint16_t *dataAcquired){
	uint8_t state=0;
	switch(action){
	case adcUpdateValue:
		if(ADCHARDWAREPROXY_unmarshal(CHANNEL0, BURST_MODE, adcStruct.adcRightChannel)){
			return bufferActualizado; // ADC buffer actualizado
		}else{
			return bufferLleno; // Buffer lleno
		}
		break;
	case adcGetValue:
		if (indexReadRx == indexWriteRx)
				return bufferVacio; // Buffer vacio
			else {
				*dataAcquired = bufferRx[indexReadRx];
				indexReadRx = (indexReadRx+1)%RX_BUFFER_LENGTH_ADC;
				return datoAdquirido; // valor del adc adquirido
			}
		break;
	}
}
/**
* @brief Funcion publica utilizada para realizar modificaciones en la
* 		 configuracion del ADC. (EN DESARROLLO)
* @param setAction accion a ejecutar
* @param adcMultiplexedChannel canal del ADC.
* @return none
*/
void ADCPROXYCLIENT_mutate(uint8_t setAction,ADC_CHANNEL_T adcMultiplexedChannel){
	ADCHARDWAREPROXY_marshal(CHANNEL0, adcMultiplexedChannel, adcStruct.adcSampleRate, adcStruct.adcResolution,setAction);
}

/*==================[fin del archivo]========================================*/
