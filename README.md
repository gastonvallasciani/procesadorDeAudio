# Procesador de Audio

# Proyecto Final de la Carrera de Especialización en Sistemas Embebidos, Universidad de Buenos Aires.

 Este archivo sera utilizado para describir las tareas que se desarrollen durante el desarrollo del proyecto y los cambios en los commits que se agreguen al código del mismo. 

 El proyecto se desarrolla sobre la placa EDU-CIAA-NXP que posee el microcontrolador LPC4337.

FECHA: 27/08/2018

Creación del repositorio y commit inicial.

FECHA: 27/08/2018

 Se agregó el código al repositorio. Este posee la adquisición de audio con el ADC implementada con hardware proxy patterns. Se adquiere la información del ADC mediante la interrupción en modo ráfaga del periferico y se actualiza un buffer circular de adquisición mediante la interrupción de desborde del Timer 1. A su vez, incluye la generación de audio mediante el DAC implementado con hardware proxy patterns. 

 Se midió la latencia de adquisición y generación de audio entre la entrada y la salida del microcontrolador. Esto se realizó generando tonos de audio de 1KHz, 5KHz, 10KHz y 15KHz con el software Audacity. Una vez generado el tono, se conectó la salida de la placa de audio de la PC a la entrada del ADC 1 del LPC4337 y se regeneró la señal de audio medida mediante el DAC. Luego, se midió la diferencia de tiempos entre la señal de entrada y de salida con un osciloscopio para obtener la latencia. 

 Se obtuvieron imágenes de las mediciones con el osciloscopio y se encuentran guardadas en la carpeta de google drive del proyecto a la cual se accede mediante el siguiente enlace 

https://drive.google.com/open?id=11r3HlkTrEa5hAmaeK2yz8cP0KFATxhbh. 

 Se realizó el diseño de un filtro FIR pasabajos cuya frecuencia de corte es de 15kHz, y a su vez, posee 20dB de atenuación entre 15KHz y 22KHz. Se generó el header file correpondiente en el cual se encuentra el vector con los coeficientes del filtro. Este se puede obvservar en la carpeta de google drive del proyecto a la que se accede mediante el enlace siguiente,

https://drive.google.com/open?id=1xjvo2t-v5r0JmQ1fiHiX1UNMOeUb9hF4.


FECHA: 13/09/2018

Se creó la rama developFilter.

 Se realizó la implementación de un manejador de filtros FIR en el archivo filterManager.c. A esta función se le pasa como parámetros la longitud de los coeficientes del filtro, la longitud del vector de entrada, el vector de coeficientes de entrada, el vector de coeficientes del filtro y el vector de salida donde se entrega la senal filtrada. Se testeo con vectores de prueba basicos un filtro pasabajos de 15khz de atenuación 20db por década y este respondio de forma acorde a la esperada. Todavía se debe testear con vectores patrón que simulen distintos tonos de entrada generados con MATLAB.

FECHA: 21/09/2018

 Se crea una función en filterManager.c para realizar el procesamiento del filtro con variables de tipo float para poder trabajar con el filtro normalizado. Para ello fue necesario crear una función que calcule la ganancia de continua del filtro. Se utiliza una compilación condicional para poder cambiar entre el calculo con coma flotante o punto fijo.

FECHA: 24/09/2018

 Se generan los branch developFilterOnline y developFilterOffline para implementar los testeos online y offline respectivamente.

 Se crean distintos tonos puros en MATLAB para realizar el testeo offline de filtros. Se accede a la carpeta contenedora de google drive de estos tonos mediante el enlace siguiente,

https://drive.google.com/open?id=1xxAgcvk6ieFr17po1qhK-adxpBRgl_FO.

FECHA: 25/09/2018

 Se crea eel archivo test.c en el que se agregan los vectores de prueba generados mediante MATLAB. Se crea una función en filterManager.c para eliminar el nivel de continua del vector de entrada. Se testeo el filtro de 15KHz con los vectores de prueba generados con MATLAB de forma offline. Falta calcular la latencia del filtro de manera offline. Verificar el filtrado mediante la fft en MATLAB. Hacer un procesamiento online y medir latencia online para ello faltan hacer algunos cambios en el código. 

FECHA: 27/09/2018

 Se generaron 3 filtros FIR en MATLAB para ser utilizados en el main.c. Un filtro pasabajos de 60 db de atenuación entre 15KHz y 22KHz, un filtro pasabajos de 4 db de atenuación entre 4KHz y 8KHz, y por último, un filtro pasa-altos de 40 db de atenuación entre 4Hz y 4KHz.
Los archivos de matlab, header file e imágenes se encuentran en la carpeta de google drive siguiente,

https://drive.google.com/open?id=1SkKYLhn-Xu6lMNjhMThhnJoZ-xsGrob5.

 Además, se midió el tiempo de procesamiento de los filtros de forma offline obteniendose los siguientes resultados:
Filtro pasabajos 15khz: 2.77ms contador:565868
Filtro pasabajos 4khz: 3.67ms  contador:749238
Filtro pasaaltos 4khz: 4.03ms 823048

FECHA: 30/09/2018

 Se corrigio la forma de procesamiento de los filtros fir en el archivo filterManageer.c. Faltaba el corrimiento del vector de entrada en el procesamiento. Por lo tanto, se recalcularon los tiempo de procesamiento offline de la funcion y se obtuvieron los siguientes valores.

Filtro Pasabajos 15KHz (vector de 500 muestras de entrada): 2.19ms contador:448643
Filtro Pasabajos 4KHz (vector de 500 muestras de entrada): 2.8ms contador:571142
El filtro pasaaltos hay que recalcularlo.

FECHA: 02/10/2018

 Se implementó un PING-PONG Buffer para la adquisición de audio. De esta forma, se procesa en el foreground mientras se adquiere en background, de esta forma, no se pierde tiempo en adquirir datos luego del procesamiento.

FECHA: 07/10/2018

 Se realizaaron varios cambios. Se finalizó el testeo online de filtros. Para dicho testeo se utilizó el filtro pasabajos de 15KHz. En la siguiente carpeta se pueden observar imágenes de los testeos realizados.

https://drive.google.com/open?id=1BT_Q2OBRWeaNPGiounUDonTia_VYmWER

 En esta se observan 3 carpetas 5KHz, 15KHz y 20KHz, los nombres de las carpetas representan el tono que fue inyectado a la EDU-CIAA-NXP y sensado a la salida de la misma para constatar la respuesta del filtro.
 
 Para realizar dicha implementación fue necesario sincronizar de forma correcta la generación de audio mediante el TIMER2. Esto fue necesario ya que el DAC de la EDU-CIAA-NXP genera datos a uan frecuencia de 400KHz o 1MHZ. Por lo tanto, mediante el TIMER 2 se logra generar audio a una frecuencia de 44.1KHZ, frecuencia a la que fue adquirido el mismo.
Fue necesario agrandar los buffers de procesamiento de 200 elementos a 500 elementos para poder generar audio de manera continua. 

 En los testeos online se verifica que luego del procesamiento hay un retardo de 140uSeg entre la señal de entrada y la señal de salida.
Por último, se pusheo el branch developFilterOnline al branch developFilter, y luego, el branch developFilter al branch master.

FECHA: 27/05/2019

Se fabricó el poncho procesador de audio. Se testearon los leds del poncho que se encuentran conectados a los pines GPIO4, GPIO5 y GPIO6 del LPC4337 de la EDU-CIAA NXP. Se testeo la entrada input mono stereo y la generación output mono stereo mediante el DAC de la EDU-CIAA NXP y funciona correctamente, se puede modificar el nivel de continua y el volumen de la señal ingresada al mezclador. Como se uso la entrada input stereo se modificó el canal del ADC que se usa en el programa. Se cambió del ADC 1 al ADC 3.

La entrada y salida de audio se testeo inyectando una canción y en su reproducción se escucha un sonido limpio.

Falta testear la adquisición y la generación stereo.

FECHA: 28/05/2019

Se continuo con los testeos de hardware. Se escucha un ruido de fondo a la música. Se midió en la entrada "Input Mono Stereo" y se observan 20mV de piso de ruido. A su vez, se midió en la salida de audio "Output Mono Stereo" y se midieron 40mV de piso de ruido. Este ruido hay que filtrarlo a la entrada por hardware.

Se testeo el filtro de 15KHz sobre audio y no sumo distorsión a la música. Habría que analizar el espectro de radiofrecuencia para ver que tan atenuadas estan las componentes superiores a 15KHz.

FECHA: 15/06/2019

Se agregan los modulo AudioProcessor.c y .h para guardar en el mismo las funciones generales del procesador. Se agrega la funcion calculateAudioMeanValue para actualizar el valor medio de la adquisicion de audio de forma automatizada. Este valor medio se utiliza con las funciones eliminateContinous y sumContinous. Estas funciones se pasaron del modulo filterManager al modulo audioProcessor ya que son funciones generales del procesador. Falta trabajar un poco mas el gatillado.

FECHA: 28/07/2019

Se creo una máquina de estados de manejo general del procesador de audio. Esta máquina se encuentra alojada en AudioProcessor.c, por lo tanto, se libero en gran medida el archivo main.c. 
El compresor VCA funciona de manera correcta, este fue testeado con audio real. Es crítica la temporización entre el retardo y el tiemp ode procesamiento de audio, si hay diferencia se escucha ruido en el audio una vez reproducido sobre el parlante.
En lo que respecta al compresor falta únicamente modificar la función del compresor para que admita que el gatillado se ejecute por medio de una promediación y no úna única muestra. Este tipo de compresor es utilizado para implementar el simetrizador de picos uqe debe controlar el valor medio de la mezcla.
Se creo el módulo que ejecuta la funcionalidad del clipper. Esto se utiliza en conjunto con el compresor VCA en la última etapa de procesamiento de audio.
