#ifndef _UART_H_
#define _UART_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"
/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

bool_t UART_getChar(uint8_t *byteReceived);
void UART_putString(char *stringPointer);
int8_t UART_putChar(uint8_t dato);
bool_t UART_output(void);
int8_t UART_input(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _UART_H_ */
