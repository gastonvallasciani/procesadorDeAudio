/**
  @file ADC_proxyClient.c
  @brief Modulo de abstraccion entre la adquisicion del ADC y el main.c
  	  	 Permite elegir la adquisicion entre buffer circular y PING-PONG-BUFFER

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
uint16_t indexReadRx, indexWriteRx, bufferRx[RX_BUFFER_LENGTH_ADC];
/*==================[definiciones de funciones publicas]=====================*/
/**
* @brief Funcion que inicializa la adquisicion de datos
* 		 Default: Fs = 400KHz, Resolucion = 10bits
* 		 Canal Izquierdo ADC_CH1.
* 		 Canal Drecho = ADC_CH2.
* @return none
*/
void initAqcuisition(void){
	adcInitialize(CHANNEL0);
	adcStruct.adcSampleRate = AUDIO_SAMPLE_RATE;
	adcStruct.adcResolution = ADC_10BITS;
	adcStruct.adcRightChannel = ADC_CH1;
	adcStruct.adcLeftChannel = ADC_CH2;
}
/**
* @brief Funcion publica de configuracion de la adquisicion de datos
* @return none
*/
void configAqcuisition(void){
	adcConfiguration(CHANNEL0, BURST_MODE, adcStruct.adcRightChannel,
					 adcStruct.adcSampleRate, adcStruct.adcResolution);
}
/**
* @brief Funcion que deshabilita la adquisicion de datos.
* @return none
*/
void disableAqcuisition(void){
	adcDisable(CHANNEL0);
}
/**
* @brief Funcion que permite actualizar levantar de la capa mas baja
* 		 el valor de tension digitalizado.
* @param action 		 adcUpdateValue: Actualiza el valor levantado del driver
* 						 adcGetValue: Levanta el dato para ser utilizado en el main
* @param aqcuisitionType circularBuffer: Levanta el dato sobre un buffer circular
* 						 adcGetValue: Levanta el dato sobre un ping-pong-buffer
* @param dataAcquired puntero al dato adquirido
* @return estado de la operacion
*/
uint8_t dataAqcuisition(accessAction_t  action,
						aqcuisitionType_t aqcuisitionType,
						uint16_t *dataAcquired){
	uint8_t state=0;
	uint16_t dataAqcuire = 0;
	switch(action){
	case adcUpdateValue:
		adcReadData(CHANNEL0, BURST_MODE, adcStruct.adcRightChannel, &dataAqcuire);
		if(aqcuisitionType == circularBuffer){
			if( (indexWriteRx+1)%RX_BUFFER_LENGTH_ADC == indexReadRx ){
				return bufferLleno; // condicion de buffer lleno
			}
			else{
				bufferRx[indexWriteRx] = dataAqcuire;
				indexWriteRx=(indexWriteRx+1)%RX_BUFFER_LENGTH_ADC;
				return bufferActualizado; // buffer actualizado
			}
		}
		else if(aqcuisitionType == pingPongBuffer){
			return 0;

		}
		break;
	case adcGetValue:
		if(aqcuisitionType == circularBuffer){
			if (indexReadRx == indexWriteRx)
					return bufferVacio; // Buffer vacio
			else {
					*dataAcquired = bufferRx[indexReadRx];
					indexReadRx = (indexReadRx+1)%RX_BUFFER_LENGTH_ADC;
					return datoAdquirido; // valor del adc adquirido
			}
		}
		else if(aqcuisitionType == pingPongBuffer){
			adcReadData(CHANNEL0, BURST_MODE, adcStruct.adcRightChannel, &dataAqcuire);
			*dataAcquired = dataAqcuire;
			return 3;
		}
		break;
	default:
		return 1;
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
void ADCPROXYCLIENT_mutate(uint8_t setAction,
						   ADC_CHANNEL_T adcMultiplexedChannel){
	adcSet(CHANNEL0, adcMultiplexedChannel, adcStruct.adcSampleRate,
		   adcStruct.adcResolution,setAction);
}

/*==================[fin del archivo]========================================*/
