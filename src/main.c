/*============================================================================
 * Autor:Gastón Vallasciani
 * Proyecto: Procesador de Audio Para radios Online
 * Licencia: None
 * Fecha: 05-05-2018
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "sapi.h"        //  sAPI lib
#include "sapi_peripheral_map.h"
#include "ADC_hardwareProxy.h"
#include "ADC_proxyClient.h"
#include "DAC_hardwareProxy.h"
#include "sapi_timer.h"
#include "filterManager.h"
#include "test.h"

/*==================[definiciones y macros]==================================*/
DEBUG_PRINT_ENABLE
/*==================[definiciones de datos internos]=========================*/
#define WAIT_DELAY 50
#define NOLED LEDR+6
#define ACQUISITION_FRECUENCY_100KHZ() Timer_microsecondsToTicks( 10 )
#define TIMER1 1
#define INPUT_VECTOR_SIZE 500
#define OUTPUT_VECTOR_SIZE 500

typedef struct{
	uint16_t audioLeftChannel;
	uint16_t audioRightChannel;
}audioChannel_t;

typedef struct{
	uint8_t filterSize;
	int32_t filterGain;
}filterData_t;
/*==================[definiciones de datos internos]=========================*/
gpioMap_t LED = LEDR;
delay_t waitDelay;
char buffer [33];
audioChannel_t audioChannel;
int16_t inpVector[INPUT_VECTOR_SIZE];
int32_t outVector[OUTPUT_VECTOR_SIZE];
uint8_t filterSize;
int32_t filterGain;

int16_t lpf15Khz[11] = {564, -1409, 2642, -3963, 4986, 27395, 4986, -3963,
								2642, -1409, 564}; // low pass filter 15KHz

filterData_t lpf;
/*==================[definiciones de datos externos]=========================*/
extern adcProxyClient_t adcStruct;
extern int16_t inVector400Hz[500];
extern int16_t inVector20Khz[500];
extern int16_t inVector15Khz[500];
extern int16_t inVector10Khz[500];
/*==================[declaraciones de funciones internas]====================*/
void tickTimerHandler( void *ptr );
/*==================[declaraciones de funciones externas]====================*/
/*==================[funcion principal]======================================*/
int main( void ){
// ---------------------CONFIGURACIONES INICIALES-----------------------------

   // Inicializacion y configuracion de la plataforma
   boardConfig();
   ADCHARDWAREPROXY_acquireDisable();

   // Inicializacion de UART_USB como salida de consola de debug
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "UART_USB configurada.\n\r" );

   //Inicializacion y configuracion del conversor digital-analogico DAC
   debugPrintlnString("Inicializando DAC..");
   DACHARDWAREPROXY_initialize();
   debugPrintlnString("DAC Inicializado\n\r");
   DACHARDWAREPROXY_config();
   debugPrintlnString("DAC Configurad0\n\r");

   //Inicializacion y configuracion del conversor analogico-digital ADC
   debugPrintlnString("Inicializando ADC..");

   //Inicializo DAC
   debugPrintlnString( "Inicializando DAC.." );
   DACHARDWAREPROXY_initialize();
   debugPrintlnString( "DAC Inicializado\n\r" );
   DACHARDWAREPROXY_config();
   debugPrintlnString( "DAC Configurado\n\r" );

   //Inicializo ADC
   debugPrintlnString( "Inicializando ADC.." );
   ADCPROXYCLIENT_initialize();

   //Inicializacion de la esctructura de manejo del ADC
   adcStruct.adcSampleRate = AUDIO_SAMPLE_RATE;
   adcStruct.adcResolution = ADC_10BITS;
   adcStruct.adcRightChannel = ADC_CH1;
   adcStruct.adcLeftChannel = ADC_CH2;
   debugPrintlnString("ADC Inicializado\n\r");

   ADCPROXYCLIENT_config();
   debugPrintlnString("ADC Configurado\n\r");

   gpioWrite(LED2,ON); // Board Alive
   gpioWrite(LED3,ON); // Board Alive

   lpf.filterSize = sizeof(lpf15Khz)/sizeof(int16_t);
   lpf.filterGain = continousFilterGain(lpf.filterSize, &lpf15Khz[0]);

   // Inicializacion TIMER 1 desborde con una frecuencia de 100KHz
   Timer_Init( TIMER1 , ACQUISITION_FRECUENCY_100KHZ(), tickTimerHandler );
   uint16_t j;
   while( TRUE ){

	  /* for (j=0; j<500;j++){
		   inpVector[j]=j;
	   }*/

	   for (j=0; j<500;j++){
		   //inVector400Hz[j] = inVector400Hz[j]/2;
		   inVector10Khz[j] = inVector10Khz[j]/2;
	   	   }

	   //filterProcessing(lpf.filterSize, lpf.filterGain, &lpf15Khz[0], INPUT_VECTOR_SIZE,
	   	   	   	   //&inpVector[0], &outVector[0]);
	   filterProcessing(lpf.filterSize, lpf.filterGain, &lpf15Khz[0], INPUT_VECTOR_SIZE,
	   			   &inVector10Khz[0], &outVector[0]);

	   //filterProcessing(lpf.filterSize, INPUT_VECTOR_SIZE, &lpf15Khz[0], &inpVector[0], &outVector[0]);
	   // Se extrae un dato del buffer circular de adquisicion del ADC
	   //if(ADCPROXYCLIENT_access(adcGetValue, &audioChannel.audioRightChannel)==datoAdquirido){
	   //}

	   // Actualizacion de la salida DAC con el dato obtenido del buffer circular del adc
	   //DACPROXYCLIENT_mutate(audioChannel.audioRightChannel);
	   //Debug
	   if (!waitDelay.running){
				delayConfig(&waitDelay,WAIT_DELAY);
				delayRead(&waitDelay);
			}
	   if (delayRead(&waitDelay)){
		   // Led de debug para ver que no se trabe el funcionamiento
		   gpioToggle( LED );
		   // muestro por uart el valor adquirido del adc cada 50ms para debug
		   itoa(audioChannel.audioRightChannel,buffer,10);
		   debugPrintlnString(buffer);
		   debugPrintlnString("\n\r");
		   }
   }

   // FIN DEL PROGRAMA
   return 0;
}
/*==================[definiciones de funciones internas]=====================*/
// Funcion que se ejecuta cada vez que se desborda el TIMER 1
// El TIMER 1 se desborda con una frecuencia de 100KHz
void tickTimerHandler( void *ptr ){
	if(ADCPROXYCLIENT_access(adcUpdateValue, &audioChannel.audioRightChannel) == bufferLleno){
			//Buffer lleno
		}
}
/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
