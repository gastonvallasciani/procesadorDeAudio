# Procesador de Audio

# Proyecto Final de la Carrera de Especialización en Sistemas Embebidos, Universidad de Buenos Aires.

Este archivo sera utilizado para describir las tareas que se desarrollen durante el desarrollo del proyecto y los cambios en los commits que se agreguen al código del mismo. 

El proyecto se desarrolla sobre la placa EDU-CIAA-NXP que posee el microcontrolador LPC4337.

COMMIT 1:

FECHA: 27/08/2018

Creación del repositorio y commit inicial.

COMMIT 2:

FECHA: 27/08/2018

Se agregó el código al repositorio. Este posee la adquisición de audio con el ADC implementada con hardware proxy patterns. Se adquiere la información del ADC mediante la interrupción en modo ráfaga del periferico y se actualiza un buffer circular de adquisición mediante la interrupción de desborde del Timer 1. A su vez, incluye la generación de audio mediante el DAC implementado con hardware proxy patterns. 

Se midió la latencia de adquisición y generación de audio entre la entrada y la salida del microcontrolador. Esto se realizó generando tonos de audio de 1KHz, 5KHz, 10KHz y 15KHz con el software Audacity. Una vez generado el tono, se conecto la salida de la placa de audio de la PC a la entrada del ADC 1 del LPC4337 y se regeneró la señal de audio medida mediante el DAC. De esta forma, se midió la diferencia de tiempos de señal entre la entrada y salida con un osciloscopio. Se obtuvieron imágenes de las mediciones y se encuentran guardadas en la carpeta de google drive del proyecto a la cual se accede mediante el siguiente enlace https://drive.google.com/open?id=11r3HlkTrEa5hAmaeK2yz8cP0KFATxhbh. 
