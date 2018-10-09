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
#include "compressorManager.h"

/*==================[definiciones y macros]==================================*/
DEBUG_PRINT_ENABLE
//#define TEST_OFFLINE_ENABLE
/*==================[definiciones de datos internos]=========================*/
#define WAIT_DELAY 50
#define NOLED LEDR+6
#define ACQUISITION_FRECUENCY_100KHZ() Timer_microsecondsToTicks( 10 )
#define ACQUISITION_FRECUENCY_44100HZ() Timer_microsecondsToTicks( 22 )
/**
 * Se utiliza un VECTOR_SIZE de 500 elementos, con 200 se cortaba la salida
 */
#define VECTOR_SIZE 500
#define COMPLETE 	   1
#define INIT   0

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
int16_t inpVector[VECTOR_SIZE];
int16_t outVector[VECTOR_SIZE];
int16_t firstOutputBuffer[VECTOR_SIZE];
int16_t secondOutputBuffer[VECTOR_SIZE];
filterData_t lpf;
/**
 * Declaracion de datos utilizados en la adquisicion del ADC con el uso del
 * PING-PONG-BUFFER
 */
///Buffers para la implementacion del PING-PONG
volatile uint16_t audioBuffer1[VECTOR_SIZE];
volatile uint16_t audioBuffer2[VECTOR_SIZE];
///background buffer
volatile uint16_t *backBuffer;
///foreground buffer
volatile uint16_t *activeBuffer;
///Puntero para elegir el buffer activo
volatile uint8_t bufferPtr = 0;
///Indica si la adquisicion del ADC esta activada
volatile uint8_t captureActive = 0;
//Sample del background buffer
volatile uint16_t currentSample = 0;
/**
 * Declaracion de datos utilizados en la generacion del DAC
 */
volatile uint16_t currentTransmissionSample = 0;
volatile uint8_t transmissionStatus = COMPLETE;
volatile uint16_t *transmitBuffer;
/*==================[definiciones de datos externos]=========================*/
extern adcProxyClient_t adcStruct;
/**
 * Vectores de coeficientes de filtros FIR definidos en el archivo
 * filterManager.c
 */
extern int16_t lpf15Khz[12];
//extern int16_t lpf4Khz[16];
//extern int16_t hpf4Khz[17];
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
void tickTimerDacHandler( void *ptr );
/*==================[declaraciones de funciones externas]====================*/
/*==================[funcion principal]======================================*/
int main( void ){
   /// Inicializacion y configuracion de la plataforma
   boardConfig();
   /// Inicializacion de UART_USB como salida de consola de debug
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "UART_USB configurada.\n\r" );
   ///Inicializacion y configuracion del conversor digital-analogico DAC
   DACHARDWAREPROXY_initialize();
   DACHARDWAREPROXY_config();
   ///Inicializacion de la esctructura de manejo del ADC
   adcStruct.adcSampleRate = AUDIO_SAMPLE_RATE;
   adcStruct.adcResolution = ADC_10BITS;
   adcStruct.adcRightChannel = ADC_CH1;
   adcStruct.adcLeftChannel = ADC_CH2;
   //Inicializo la adquisicion de datos por el ADC
   initAqcuisition();
   configAqcuisition();
   /// Led de debug
   gpioWrite(LED2,ON); // Board Alive
   /// Inicializacion TIMER 1  y TIMER 2 desborde con una frecuencia de 44.1KHz
    Timer_Init( TIMER1 , ACQUISITION_FRECUENCY_44100HZ(), tickTimerHandler );
    Timer_Init( TIMER2 , ACQUISITION_FRECUENCY_44100HZ(), tickTimerDacHandler );
/**
 * Inicializacion del PING-PONG-BUFFER
 */
   bufferPtr = 0;
   activeBuffer = audioBuffer1;
   backBuffer = audioBuffer2;
   currentSample = 0;
   captureActive = 1; ///Habilito la captura del ADC
/**
 * Calculo de la cantidad de elementos del filtro y de la ganancai de continua
 */
   lpf.filterSize = sizeof(lpf15Khz)/sizeof(int16_t);
   lpf.filterGain = continousFilterGain(lpf.filterSize, &lpf15Khz[0]);

   while( TRUE ){

	   ///Led loco para debug
	   if (!waitDelay.running){
			delayConfig(&waitDelay,WAIT_DELAY);
			delayRead(&waitDelay);
		}
	   if (delayRead(&waitDelay)){
	   gpioToggle( LED );
	   }
	   ///Se elimina el valor de continua del vector adquirido
	   eliminateContinous(VECTOR_SIZE, &activeBuffer[0], &firstOutputBuffer[0]);

	   ///Se procesa el vector con el filtro definido
	   filterVectorProcessor(lpf.filterSize, lpf.filterGain, &lpf15Khz[0],VECTOR_SIZE,
			   	   	   	     &firstOutputBuffer[0], &secondOutputBuffer[0]);

	   ///Se suma el nivel de continua
	   sumContinous(VECTOR_SIZE, &secondOutputBuffer[0], &outVector[0]);

	   ///Si la transmision previa del DAC fue completada se inicia una nueva
	   /// ya que se termino de procesar el vector posterior
	   if (transmissionStatus == COMPLETE){
		   transmissionStatus = INIT;
		   transmitBuffer = outVector;
	   }

	   /// Mientras se estan adquiriendo datos no se hace el PING-PONG

	   while(captureActive==1){
	   }

	   /// Se ejecuta la conmutacion de los buffer. PING-PONG!!
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

	   /// Se inicia una nueva adquisicion de datos
	   captureActive = 1;
   }
   // FIN DEL PROGRAMA
   return 0;
}
/*==================[definiciones de funciones internas]=====================*/
/**
 * Handler del TIMER1, maneja la adquisicion de datos con el ADC
 * frecuencia de interrupcion 44khz
 */
void tickTimerHandler( void *ptr ){
	uint16_t data;

	if(captureActive){
		if(currentSample<VECTOR_SIZE){
			dataAqcuisition(adcGetValue, pingPongBuffer, &data);
			backBuffer[currentSample]= data;
			currentSample++;
		}
		else{
			currentSample=0;
			captureActive=0;
		}
	}
}
/**
 * Handler del TIMER1, maneja la transmision de datos por el DAC
 * frecuencia de interrupcion 44khz
 */
void tickTimerDacHandler( void *ptr ){
		if(transmissionStatus == INIT){
		if(currentTransmissionSample<VECTOR_SIZE){
			DACPROXYCLIENT_mutate(transmitBuffer[currentTransmissionSample]);
			DACHARDWAREPROXY_marshal();
			currentTransmissionSample++;
		}
		else{
			transmissionStatus = COMPLETE;
			currentTransmissionSample = 0;
		}
	}
}
/*==================[fin del archivo]========================================*/
