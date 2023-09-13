
/*
 * Ejercicio N° 2 - Parcial 2022
 *
 *
 * Utilizando interrupciones por systick y por eventos externos EINT.
 * Realizar un programa que permita habilitar y deshabilitar el temporizador por
 *  el flanco ascendente en el pin P2.11.
 *
 * El temporizador debe desbordar cada 10 milisegundos utilizando un reloj de core cclk = 62MHz.
 * Por cada interrupcion del Systick se debe mostrar por el puerto P0 el promedio de los
 * valores guardados en la variables "uin8_t values[8]"
 *
 * Se pide ademas detallar los calculos realizados para obtener el valor a cargar en el
 * registro RELOAD y asegurar que la interrupcion por Systick sea mayor que la
 * prioridad de la interrupcion del evento externo.
 *
 * El codigo debe estar debidamente comentado.
 * */

#include "LPC17xx.h"

void configSysTick(void);
void configGPIO(void);
int promedio(uint8_t values[8]);
uint8_t values[8] = {6,2,7,4,9,6,7,1}; // no especifica los valores, ponemos cualquiera
volatile char toggle = 0;


int main(void)
{
	configSysTick();
	configGPIO();

	while (1) {}
	return 0;
}

void configSysTick(void)
{
	// 10 ms with 62MHz clock
	// RELOAD = (cclk/100)-1 = 62*10^6/100 - 1 = 619 999 =
	 uint32_t RELOAD = 619999 ;
	 SysTick->LOAD  = RELOAD;
	  NVIC_SetPriority (SysTick_IRQn,0);
	  SysTick->VAL   = 0;
	  SysTick->CTRL  = (1<<2) | // con 1 seleccionamos el clock del core
	                   (1<<1) | // habilitamos la interrupcion
	                   (1<<0) ; // habilitamos el systick

}

void configGPIO(void) {
	// input, interrupt rising edge P2.11, pull-down
	// EINT1
	LPC_PINCON -> PINSEL4 &= ~(0<<23);
	LPC_PINCON -> PINSEL4 |= (1<<22);	//as EINT1

	LPC_SC -> EXTMODE |= (1<<1);  //EINT1 edge sensitive
	LPC_SC -> EXTPOLAR |= (1<<1); //EINT1 rising edge sensitive

	LPC_PINCON -> PINMODE4 |= (0b11<<22); //pull-down on P2.11 (EINT1)

	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_SetPriority (EINT1_IRQn,1);

	//P0.0 - P0.7 output
	LPC_PINCON -> PINSEL0 &= ~(0b11<<14); // P0.7 as GPIO
	LPC_GPIO0 ->FIODIR0 |= (1<<7);		// P0.7 as output
	return;
}

void EINT1_IRQHandler(void)
{
	toggle = !toggle;
	if(toggle == 1){
		SysTick->CTRL &= ~(1<<1); // disable systick int
	}else	{
		SysTick->CTRL |= (1<<1);  // enable systick int
	}
}

void SysTick_IRQHandler(void)
{
	LPC_GPIO0 -> FIOPIN0 = promedio(values) & 0xFF;
}

int promedio(uint8_t values[8])
{
	uint16_t suma = 0;
	for(int i=0; i<8; i++)
	{
		suma = suma + values[i];
	}
	return suma/8;
}
