#ifndef _DAC_HARDWAREPROXY_H_
#define _DAC_HARDWAREPROXY_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
#include "board.h"
#include "uart.h"
#include "sapi_peripheral_map.h"
#include "adc_18xx_43xx.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define DAC_CHANNEL LPC_DAC
#define DAC_TX_BUFFER_LENGTH 1000	// Tamaño buffer de transmision para la interfaz dac
/*==================[typedef]================================================*/
/*==================[external data declaration]==============================*/

void DACHARDWAREPROXY_initialize(void);
void DACHARDWAREPROXY_config(void);
void DACHARDWAREPROXY_disable(void);
uint8_t DACHARDWAREPROXY_marshal(void);
uint8_t DACPROXYCLIENT_mutate(uint16_t data);
//void DAC_config( uint8_t config );
//void DAC_write( dacMap_t analogOutput, uint16_t value );
/*==================[external functions declaration]=========================*/


/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _DAC_HARDWAREPROXY_H_ */
