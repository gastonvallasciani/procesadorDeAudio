/*============================================================================
 * Licencia:
 * Autor:
 * Fecha:
 *===========================================================================*/

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

/*==================[inclusiones]============================================*/

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define TIME_BETWEEN_SAMPLES_IN_US 22
#define ACQUISITION_FRECUENCY_10KHZ() Timer_microsecondsToTicks( 50 )
#define ACQUISITION_FRECUENCY_100KHZ() Timer_microsecondsToTicks( 10 )
#define ACQUISITION_FRECUENCY_44100HZ() Timer_microsecondsToTicks( 23 )
/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _PROGRAM_H_ */
