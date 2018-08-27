/*==================[inclusiones]============================================*/
#include "uart.h"
#include "sapi.h"
/*==================[macros and definitions]=================================*/
#define TX_BUFFER_LENGTH 300	//Tamaño buffer de transmision para la interfaz uart
#define RX_BUFFER_LENGTH 300	// Tamaño buffer de recepcion para la interfaz uart
/*==================[definiciones de datos internos]=========================*/
// variables utilizadas por las funciones que controlan los buffers y los perifericos con lainterfaz UART
static uint8_t indexReadTx, indexWriteTx, bufferTx[TX_BUFFER_LENGTH];
static uint8_t indexReadRx, indexWriteRx, bufferRx[RX_BUFFER_LENGTH];

/*==================[definiciones de funciones externas]=====================*/
//Funciones Productoras

/*Funcion utilizada para almacenar un byte en el buffer de transmision*/
int8_t UART_putChar(uint8_t dato){
	if( (indexWriteTx+1)%TX_BUFFER_LENGTH == indexReadTx )
		//Llena (error)
		return -1;
	else {
		bufferTx[indexWriteTx]=dato;
		indexWriteTx = (indexWriteTx+1)%TX_BUFFER_LENGTH;
		return 1;
	}
}

/*Funcion utilizada para almacenar un string en el buffer de transmision*/
void UART_putString(char *stringPointer){
	while(*stringPointer != 0){
		UART_putChar((uint8_t)*stringPointer);
		stringPointer++;
	}
}

/*Funcion utilizada para levantar un caracter del buffer de recepcion y procesarlo*/
bool_t UART_getChar(uint8_t *byteReceived){
	if (indexReadRx == indexWriteRx)
		//Vacia
		return 0;
	else {
		*byteReceived = bufferRx[indexReadRx];
		indexReadRx = (indexReadRx+1)%RX_BUFFER_LENGTH;
		return 1;
	}
}

// Funciones de interfaz con los perifericos

/* Funcion utilizada para enviar datos del buffer de transmision por la UART*/
bool_t UART_output(void){
	if (indexReadTx == indexWriteTx)
		//Buffer Empty
		return 0;
	else {
		uartWriteByte(UART_USB,bufferTx[indexReadTx]);
		indexReadTx=(indexReadTx+1)%TX_BUFFER_LENGTH;
		return 1;
	}
}

/* Funcion utilizada para recibir datos de la UART y guardarlos en el buffer de recepcion*/
int8_t UART_input(void){
	if( (indexWriteRx+1)%RX_BUFFER_LENGTH == indexReadRx )
		//Llena (error)
		return -1;
	else {
		if (uartReadByte( UART_USB, &bufferRx[indexWriteRx] ) != FALSE) {
			indexWriteRx=(indexWriteRx+1)%RX_BUFFER_LENGTH;
		}
		return 1;
	}
}
/*==================[fin del archivo]========================================*/
