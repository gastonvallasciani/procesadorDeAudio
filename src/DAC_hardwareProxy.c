/*==================[inclusiones]============================================*/
#include "DAC_hardwareProxy.h"
/*==================[macros and definitions]=================================*/
static uint16_t indexReadTx, indexWriteTx, bufferTx[DAC_TX_BUFFER_LENGTH];
/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void DACHARDWAREPROXY_initialize(void){
	Chip_DAC_Init(DAC_CHANNEL);		//Enable DAC channel
}

void DACHARDWAREPROXY_config(void){
	/* Enables the DMA operation and controls DMA timer */
	Chip_DAC_ConfigDAConverterControl(DAC_CHANNEL, DAC_DMA_ENA);
	/* DCAR DMA access */
	/* Update value to DAC buffer*/
	Chip_DAC_UpdateValue(DAC_CHANNEL, 1);
}

void DACHARDWAREPROXY_disable(void){
	Chip_DAC_DeInit( DAC_CHANNEL );
}

uint8_t DACHARDWAREPROXY_marshal(void){

	uint16_t dacData;

	if (indexReadTx == indexWriteTx)
		return 0; // Buffer vacio
	else {
		dacData = bufferTx[indexReadTx];
		if( dacData > 1023 ){
			dacData = 1023;
		}
		else if(dacData<0){
			dacData = 0;
		}
		Chip_DAC_UpdateValue( DAC_CHANNEL, dacData ); // escribe en el DAC
		indexReadTx = (indexReadTx+1)%DAC_TX_BUFFER_LENGTH;
		return 1;
	}

}

uint8_t DACPROXYCLIENT_mutate(uint16_t dacData){

	 if( (indexWriteTx+1)%DAC_TX_BUFFER_LENGTH == indexReadTx )
		 // Buffer Lleno (error)
			 return 0;
		 else{
			 bufferTx[indexWriteTx] = dacData;
			 indexWriteTx =(indexWriteTx+1)%DAC_TX_BUFFER_LENGTH;
			 if(DACHARDWAREPROXY_marshal()){
				 return 1; // dato escrito en el DAC
			 }else{
				 return 2; // Buffer Vacio (error)
			 }
		 }

}

/*==================[end of file]============================================*/
