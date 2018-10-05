#ifndef _ADC_PROXYCLIENT_H_
#define _ADC_PROXYCLIENT_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
#include "sapi_datatypes.h"
#include "sapi_peripheral_map.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define RX_BUFFER_LENGTH_ADC 200	// Tamaño buffer de recepcion para la interfaz adc
#define AUDIO_SAMPLE_RATE 400000
/*==================[typedef]================================================*/
typedef struct{
	uint32_t adcSampleRate;
	uint8_t adcLeftChannel;
	uint8_t adcRightChannel;
	uint8_t adcResolution;
}adcProxyClient_t;

typedef enum{circularBuffer, pingPongBuffer}aqcuisitionType_t;

typedef enum{adcUpdateValue,adcGetValue}accessAction_t;
enum uint8_t{bufferActualizado,
			 bufferLleno,
			 bufferVacio,
			 datoAdquirido};
/*==================[external data declaration]==============================*/
/*==================[external functions declaration]=========================*/
void initAqcuisition(void);
void configAqcuisition(void);
void disableAqcuisition(void);
uint8_t dataAqcuisition(accessAction_t  action,
						aqcuisitionType_t aqcuisitionType,
						uint16_t *dataAcquired);
void ADCPROXYCLIENT_mutate(uint8_t setAction,ADC_CHANNEL_T adcMultiplexedChannel);
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _ADC_PROXYCLIENT_H_ */
