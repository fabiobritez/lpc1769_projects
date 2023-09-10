# Problema - Recepción de datos

## Enunciado
En el puerto P0.0 se debe recibir una secuencia 
de bits que representan un valor de 0 a 1023. Que tiene la 
siguiente forma:

            _________________________
     ______|                         |_____
      30 ms       10 bits de 5ms      30 ms  

El valor recibido debe ser convertido a una señal
modulada en amplitud de 50 ms de duración y enviada por el puerto P0.2.

            ________________          
     ______|                |_____________
             ancho de pulso     off time
                        50 ms
 


## Solución propuesta
Para solucionar el problema se eligió hacerlo por estados que se actualizan en el programa principal. Dependiendo del valor de las [variables](#variables) correspondientes se ejecuta una acción.

## Estados
⌛ **IDLE (INACTIVO)**: Cuando no se esta recibiendo ningún dato.

⚠️ **PRE_SYNC (PRE_SINCRONIZACION)**: Los primeros 30 ms de la señal.

🔄 **SYNC (SINCRONIZADO)**: Los 10 bits de 5 ms.

⚠️**POST_SYNC (POST_SINCRONIZACION)**: Los últimos 30 ms de la señal.

❌ **ERROR**: Cuando se detecta un valor no deseado en los estados pre y post sincronizacion.

Inicialmente el estado es IDLE. Cuando se detecta un cambio en el pin de entrada, se cambia al estado PRE_SYNC.

En los estado PRE_SYNC y POST_SYNC se chequea cada 1ms que su valor se mantenga en cero durante 30ms. Si no se mantiene en cero, se cambia al estado ERROR.

En el estado sync se va a ir almacenando los bits recibidos en una variable.

Como se lee el valor del pin cada 1ms se va tomar el valor del 3er ms de cada bit.

``` mermaid
     Duracion de 1 bit
    ___ ___ ___ ___ ___
___| 1 | 2 | 3 | 4 | 5 |___
             ^
Se toma el valor en este instante

```

Se realiza un cambio de estado a POST_SYNC cuando se reciben los 10 bits.

## Variables
- **currentState**: El estado actual.
- **milliseconds**: Los milisegundos desde que se inicio el programa.
- **syncStartTime**: Tiempo de inicio de la sincronización.
- **syncTime**: Tiempo de duración de la sincronización.
- **receivedData**: Almacena los bits recibidos.


## Handler de la interrupcion del systick

En el handler de la interrupcion del systick, vamos a actualizar el valor del pin de entrada. Y evaluarlo dependiendo su estado.

## Modulación en amplitud
 
El valor recibido que lo convertimos a tipo entero sin signo puede tener un valor de 0 a 1023. Tenemos que convertirlo a una señal modulada en amplitud de 50 ms de duración.

``` c
    0 a 1023 -> 0 a 50 ms
```

Esto lo podemos hacer con una regla de 3 simple.

Primero calculamos el ancho el ancho de pulso y luego el off time.

 

## Limitaciones
   Este algoritmo no detecta errores de transmisión.
   No detecta cambios en un tiempo menor a 1ms.

