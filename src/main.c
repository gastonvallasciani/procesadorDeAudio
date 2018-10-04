/**
  @file main.c
  @brief Procesador de Audio Digital

  @author Gaston Vallasciani
  @date 05/05/2018
*/
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
//#define TEST_OFFLINE_ENABLE
/*==================[definiciones de datos internos]=========================*/
#define WAIT_DELAY 50
#define NOLED LEDR+6
#define ACQUISITION_FRECUENCY_100KHZ() Timer_microsecondsToTicks( 10 )
#define ACQUISITION_FRECUENCY_44100HZ() Timer_microsecondsToTicks( 22 )
#define TIMER1 1
#define INPUT_VECTOR_SIZE 200
#define OUTPUT_VECTOR_SIZE 200
#define VECTOR_SIZE 200

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
int16_t outVector[OUTPUT_VECTOR_SIZE];
filterData_t lpf;
/*----------------------------------ADC SET----------------------------------*/
/*----------------------------------PING PONG--------------------------------*/
//Buffers para la implementacion del PING-PONG
volatile uint16_t audioBuffer1[VECTOR_SIZE];
volatile uint16_t audioBuffer2[VECTOR_SIZE];

//background buffer
volatile uint16_t *backBuffer;
//foreground buffer
volatile uint16_t *activeBuffer;

//Puntero para elegir el buffer activo
volatile uint8_t bufferPtr = 0;

//Indica si la adquisicion del ADC esta activada
volatile uint8_t captureActive = 0;

//Sample del background buffer
volatile uint8_t currentSample = 0;
/*---------------------------------------------------------------------------*/

/*==================[definiciones de datos externos]=========================*/
extern adcProxyClient_t adcStruct;
/**
 * Vectores de coeficientes de filtros FIR definidos en el archivo
 * filterManager.c
 */
extern int16_t lpf15Khz[12];
extern int16_t lpf4Khz[16];
extern int16_t hpf4Khz[17];
/**
 * Vectores de entrada definidos en el archivo test.c para testeo offline
 */
#ifdef TEST_OFFLINE_ENABLE
	extern int16_t inVector400Hz[500];
	extern int16_t inVector20Khz[500];
	extern int16_t inVector15Khz[500];
	extern int16_t inVector10Khz[500];
	extern int16_t inVector5Khz[500];
	extern int16_t inVector1Khz[500];
#endif
/*==================[declaraciones de funciones internas]====================*/
void tickTimerHandler( void *ptr );
/*==================[declaraciones de funciones externas]====================*/
/*==================[funcion principal]======================================*/
int main( void ){
   // Inicializacion y configuracion de la plataforma
   boardConfig();

   // Inicializacion de UART_USB como salida de consola de debug
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "UART_USB configurada.\n\r" );

   //Inicializacion y configuracion del conversor digital-analogico DAC
   DACHARDWAREPROXY_initialize();
   DACHARDWAREPROXY_config();

   //Inicializacion de la esctructura de manejo del ADC
   adcStruct.adcSampleRate = AUDIO_SAMPLE_RATE;
   adcStruct.adcResolution = ADC_10BITS;
   adcStruct.adcRightChannel = ADC_CH1;
   adcStruct.adcLeftChannel = ADC_CH2;
   //Inicializo ADC
   ADCPROXYCLIENT_initialize();
   ADCPROXYCLIENT_configAqcuisition();

 //  gpioWrite(LED2,ON); // Board Alive
   gpioWrite(LED3,ON); // Board Alive

   /*-------------------------Inicializacion ping pong buffer------------------------------*/
   bufferPtr = 0;
   activeBuffer = audioBuffer1;
   backBuffer = audioBuffer2;
   currentSample = 0;
   captureActive = 1; //Habilito la captura del ADC
   /*--------------------------------------------------------------------------------------*/

   lpf.filterSize = sizeof(lpf15Khz)/sizeof(int16_t);
   lpf.filterGain = continousFilterGain(lpf.filterSize, &lpf15Khz[0]);

   // Inicializacion TIMER 1 desborde con una frecuencia de 100KHz
   Timer_Init( TIMER1 , ACQUISITION_FRECUENCY_44100HZ(), tickTimerHandler );

   while( TRUE ){

	   eliminateContinous(INPUT_VECTOR_SIZE, &activeBuffer[0]);

	   filterVectorProcessor(lpf.filterSize, lpf.filterGain, &lpf15Khz[0],INPUT_VECTOR_SIZE, &activeBuffer[0],
	   							 &outVector[0]);
	   // Se extrae un dato del buffer circular de adquisicion del ADC
	   //if(ADCPROXYCLIENT_access(adcGetValue, &audioChannel.audioRightChannel)==datoAdquirido){
	   //}

	   // Actualizacion de la salida DAC con el dato obtenido del buffer circular del adc
	   DACPROXYCLIENT_mutate(audioChannel.audioRightChannel);
	   uint8_t k;
	   for(k=0;k<VECTOR_SIZE;k++){
		   DACPROXYCLIENT_mutate(outVector[k]);
	   }
		   //Debug
	   if (!waitDelay.running){
				delayConfig(&waitDelay,WAIT_DELAY);
				delayRead(&waitDelay);
			}
	   if (delayRead(&waitDelay)){
		   // Led de debug para ver que no se trabe el funcionamiento
		   gpioToggle( LED );
		   // muestro por uart el valor adquirido del adc cada 50ms para debug
		   //itoa(audioChannel.audioRightChannel,buffer,10);
		   //debugPrintlnString(buffer);
		   //debugPrintlnString("\n\r");
		   }

	   while(captureActive==1){
	   }

	   // Se ejecuta la conmutacion de los buffer. PING-PONG!!
	   if(bufferPtr==0){
		   bufferPtr=1;
		   activeBuffer=audioBuffer2;
		   backBuffer=audioBuffer1;
	   }
	   else{
		   bufferPtr=0;
		   activeBuffer=audioBuffer1;
		   backBuffer=audioBuffer2;
	   }
	   captureActive = 1;
   }
   // FIN DEL PROGRAMA
   return 0;
}
/*==================[definiciones de funciones internas]=====================*/
// Funcion que se ejecuta cada vez que se desborda el TIMER 1
// El TIMER 1 se desborda con una frecuencia de 44.1KHz
void tickTimerHandler( void *ptr ){
	uint16_t data;
	//if(ADCPROXYCLIENT_access(adcUpdateValue, &audioChannel.audioRightChannel) == bufferLleno){
	//		//Buffer lleno
	//	}
	if(captureActive){
		if(currentSample<VECTOR_SIZE){
			ADCHARDWAREPROXY_adcRead(LPC_ADC0, BURST_MODE, CH1, &backBuffer[currentSample]);
			currentSample++;
		}
		else{
			currentSample=0;
			captureActive=0;
		}
	}
}
/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
