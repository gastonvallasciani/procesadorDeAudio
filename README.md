# Procesador de Audio

# Proyecto Final de la Carrera de Especialización en Sistemas Embebidos, Universidad de Buenos Aires.

Este archivo sera utilizado para describir las tareas que se desarrollen durante el desarrollo del proyecto y los cambios en los commits que se agreguen al código del mismo. 

El proyecto se desarrolla sobre la placa EDU-CIAA-NXP que posee el microcontrolador LPC4337.

FECHA: 27/08/2018

Creación del repositorio y commit inicial.

FECHA: 27/08/2018

Se agregó el código al repositorio. Este posee la adquisición de audio con el ADC implementada con hardware proxy patterns. Se adquiere la información del ADC mediante la interrupción en modo ráfaga del periferico y se actualiza un buffer circular de adquisición mediante la interrupción de desborde del Timer 1. A su vez, incluye la generación de audio mediante el DAC implementado con hardware proxy patterns. 

Se midió la latencia de adquisición y generación de audio entre la entrada y la salida del microcontrolador. Esto se realizó generando tonos de audio de 1KHz, 5KHz, 10KHz y 15KHz con el software Audacity. Una vez generado el tono, se conectó la salida de la placa de audio de la PC a la entrada del ADC 1 del LPC4337 y se regeneró la señal de audio medida mediante el DAC. Luego, se midió la diferencia de tiempos entre la señal de entrada y de salida con un osciloscopio para obtener la latencia. 

Se obtuvieron imágenes de las mediciones con el osciloscopio y se encuentran guardadas en la carpeta de google drive del proyecto a la cual se accede mediante el siguiente enlace https://drive.google.com/open?id=11r3HlkTrEa5hAmaeK2yz8cP0KFATxhbh. 

Se realizó el diseño de un filtro FIR pasabajos cuya frecuencia de corte es de 15kHz, y a su vez, posee 20dB de atenuación entre 15KHz y 22KHz. Se generó el header file correpondiente en el cual se encuentra el vector con los coeficientes del filtro. Este se puede obvservar en la carpeta de google drive del proyecto a la que se accede mediante el enlace siguiente https://drive.google.com/open?id=1xjvo2t-v5r0JmQ1fiHiX1UNMOeUb9hF4.


FECHA: 13/09/2018

Se creó la rama developFilter.

Se realizó la implementación de un manejador de filtros FIR en el archivo filterManager.c. A esta función se le pasa como parámetros la longitud de los coeficientes del filtro, la longitud del vector de entrada, el vector de coeficientes de entrada, el vector de coeficientes del filtro y el vector de salida donde se entrega la senal filtrada. Se testeo con vectores de prueba basicos un filtro pasabajos de 15khz de atenuación 20db por década y este respondio de forma acorde a la esperada. Todavía se debe testear con vectores patrón que simulen distintos tonos de entrada generados con MATLAB.

FECHA: 21/09/2018

Se crea una función en filterManager.c para realizar el procesamiento del filtro con variables de tipo float para poder trabajar con el filtro normalizado. Para ello fue necesario crear una función que calcule la ganancia de continua del filtro. Se utiliza una compilación condicional para poder cambiar entre el calculo con coma flotante o punto fijo.

FECHA: 24/09/2018

Se crean distintos tonos puros en MATLAB para realizar el testeo offline de filtros. Se accede a la carpeta contenedora de google drive de estos tonos mediante el enlace siguiente https://drive.google.com/open?id=1xxAgcvk6ieFr17po1qhK-adxpBRgl_FO.

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
