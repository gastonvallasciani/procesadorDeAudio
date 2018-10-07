/**
  @file ADC_hardwareProxy.c.c
  @brief Funciones de abstraccion de hardware del ADC

  @author Gaston Vallasciani
  @date 10/06/2018
*/
/*==================[inclusiones]============================================*/
#include "ADC_hardwareProxy.h"
#include "ADC_proxyClient.h"
#include "adc_18xx_43xx.h"

#include "sapi.h"
#include "board.h"
#include "uart.h"
#include "sapi_peripheral_map.h"
#include "core_cm4.h"
/*==================[macros and definitions]=================================*/
/*==================[definiciones de datos internos]=========================*/
static ADC_CLOCK_SETUP_T ADCSetup;
static uint16_t dataAcquired;
volatile uint8_t adcDataReadyFlag = FALSE;
/*==================[definiciones de datos publicos]=========================*/
/*==================[prototipos de funciones internas]=======================*/
void adcChannelEnable(ADC_CHANNEL_T adcMultiplexedChannel,LPC_ADC_T *channel);
void adcChannelDisableAll(ADC_CHANNEL_T adcMultiplexedChannel,LPC_ADC_T *channel);
/*==================[definiciones de funciones externas]=====================*/
/**
* @brief Funcion que permite inicializar el periferico del ADC.
*
* @param channel puntero al canal de adquisicion del ADC
* @return none
*/
void adcInitialize(LPC_ADC_T *channel){
	Chip_ADC_Init( channel, &ADCSetup );
}
/**
* @brief Funcion que permite configurar el periferico del ADC.
*
* @param channel puntero al canal de adquisicion del ADC
*
* @return none
*/

void adcConfiguration(LPC_ADC_T *channel,adcHardwareProxyConfigMode_t mode,ADC_CHANNEL_T adcMultiplexedChannel,uint32_t adcSampleRate, uint8_t resolution){

	switch(mode){

		case BURST_MODE:
			/* Enable burst mode */
			Chip_ADC_SetBurstCmd( channel, ENABLE );
			/* Set sample rate to 44KHz */
			Chip_ADC_SetSampleRate( channel, &ADCSetup, adcSampleRate );
			Chip_ADC_SetResolution( channel, &ADCSetup, resolution);
			/* Enable channel 1, disable the rest */
			adcChannelEnable(adcMultiplexedChannel,channel);
			/*Enable ADC0 Interrupt*/
			NVIC_EnableIRQ(ADC0_IRQn);
			break;
		case DMA_MODE:
			break;
		case NORMAL_MODE:
			/* Enable burst mode */
			Chip_ADC_SetBurstCmd( channel, DISABLE );
			/* Set sample rate to 44KHz */
			Chip_ADC_SetSampleRate( channel, &ADCSetup, adcSampleRate );
			Chip_ADC_SetResolution( channel, &ADCSetup, resolution);
			/* Enable channel 1, disable the rest */
			adcChannelEnable(adcMultiplexedChannel,channel);
			/*Enable ADC0 Interrupt*/
			NVIC_DisableIRQ(ADC0_IRQn);

			break;
		default:
			break;
	}
}

void adcDisable(LPC_ADC_T *channel){
	Chip_ADC_DeInit( channel );
}

void adcSet(LPC_ADC_T *channel, ADC_CHANNEL_T adcMultiplexedChannel, uint32_t sampleRate, uint8_t resolution, uint8_t action){
	switch(action){
	case SET_RESOLUTION:
		Chip_ADC_SetResolution( channel, &ADCSetup, resolution);
		break;
	case SET_SAMPLE_RATE:
		Chip_ADC_SetSampleRate( channel, &ADCSetup, sampleRate);
		break;
	case SET_LEFT_CHANNEL:
		adcChannelEnable(adcMultiplexedChannel, channel);
		break;
	case SET_RIGHT_CHANNEL:
		adcChannelEnable(adcMultiplexedChannel, channel);
			break;
	default:
		break;
	}
}

/* Funcion utilizada para adquirir datos del ADC y guardarlos en el buffer de recepcion*/
uint8_t adcReadData(LPC_ADC_T *channel, adcHardwareProxyConfigMode_t mode, ADC_CHANNEL_T adcMultiplexedChannel, uint16_t *adcData){
	uint16_t adcDataRead = 0;
	switch(mode){
	case BURST_MODE:
		if (adcDataReadyFlag == TRUE){
			adcDataReadyFlag = FALSE;
			*adcData = dataAcquired;
		}
		break;
	case DMA_MODE:
		break;
	case NORMAL_MODE:
		Chip_ADC_EnableChannel(channel, adcMultiplexedChannel, ENABLE);
		Chip_ADC_SetStartMode(channel, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		while(
			(Chip_ADC_ReadStatus( channel, adcMultiplexedChannel, ADC_DR_DONE_STAT) != SET)
		);
		Chip_ADC_ReadValue( channel, adcMultiplexedChannel, &adcDataRead );
		*adcData = adcDataRead;
		Chip_ADC_EnableChannel( channel, adcMultiplexedChannel, DISABLE );
		break;
	default:
		break;
	}
	return 1;
}

void ADC0_IRQHandler(void){
	adcDataReadyFlag = TRUE;
	Chip_ADC_ReadValue(LPC_ADC0, ADC_CH1, &dataAcquired);
}

void adcChannelEnable(ADC_CHANNEL_T adcMultiplexedChannel,LPC_ADC_T *channel){
	switch (adcMultiplexedChannel){
	case ADC_CH1:
		/* Enable channel*/
		Chip_ADC_EnableChannel( channel,ADC_CH1, ENABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH1, ENABLE ); // habilita la interrupcion de conversion para el channel

		/* Disable the rest off the channels */
		Chip_ADC_EnableChannel( channel, ADC_CH2, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH2, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH3, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH3, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH4, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH4, DISABLE );
		break;
	case ADC_CH2:
		/* Enable channel*/
		Chip_ADC_EnableChannel( channel,ADC_CH2, ENABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH2, ENABLE ); // habilita la interrupcion de conversion para el channel

		/* Disable the rest off the channels */
		Chip_ADC_EnableChannel( channel, ADC_CH1, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH1, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH3, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH3, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH4, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH4, DISABLE );
		break;
	case ADC_CH3:
		/* Enable channel*/
		Chip_ADC_EnableChannel( channel,ADC_CH3, ENABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH3, ENABLE ); // habilita la interrupcion de conversion para el channel

		/* Disable the rest off the channels */
		Chip_ADC_EnableChannel( channel, ADC_CH2, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH2, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH1, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH1, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH4, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH4, DISABLE );
		break;
	case ADC_CH4:
		/* Enable channel*/
		Chip_ADC_EnableChannel( channel,ADC_CH4, ENABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH4, ENABLE ); // habilita la interrupcion de conversion para el channel

		/* Disable the rest off the channels */
		Chip_ADC_EnableChannel( channel, ADC_CH2, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH2, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH3, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH3, DISABLE );

		Chip_ADC_EnableChannel( channel, ADC_CH1, DISABLE );
		Chip_ADC_Int_SetChannelCmd( channel, ADC_CH1, DISABLE );
		break;
	default:
		break;
	}
}
void adcChannelDisableAll(ADC_CHANNEL_T adcMultiplexedChannel,LPC_ADC_T *channel){
	/* Disable all channels*/
	Chip_ADC_EnableChannel( channel,ADC_CH1, DISABLE );
	Chip_ADC_Int_SetChannelCmd( channel, ADC_CH1, DISABLE );

	Chip_ADC_EnableChannel( channel, ADC_CH2, DISABLE );
	Chip_ADC_Int_SetChannelCmd( channel, ADC_CH2, DISABLE );

	Chip_ADC_EnableChannel( channel, ADC_CH3, DISABLE );
	Chip_ADC_Int_SetChannelCmd( channel, ADC_CH3, DISABLE );

	Chip_ADC_EnableChannel( channel, ADC_CH4, DISABLE );
	Chip_ADC_Int_SetChannelCmd( channel, ADC_CH4, DISABLE );

}


/*==================[fin del archivo]========================================*/
