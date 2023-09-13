#include "LPC17xx.h"
/*
 * Realizar un programa un guarde mediante interrupcion el dato presente en los pines de
 * entrada P2.0 al P2.7 cada vez que cambie su valor. Cada dato nuevo debe guardarse de forma
 * consecutiva en una region de memoria que permita tener disponible siempre de los ultimos 16
 * datos guardados
 *
 * */

	volatile uint8_t data [16];


int main(void)
{
	config_GPIO();

	while(1)
	{

	}


	return 0;
}


config_GPIO()
{
	LPC_PINCON -> PINSEL4 &= ~(0xFFFF<<0); 	 //GPIO from P2.0 to P2.7
	LPC_GPIO2 ->FIODIR0 &= ~(0xFF<<0);   	 //input P2.0 to P2.7

	LPC_PINCON -> PINMODE4 &=  ~(0xFFFF<<0); //pull-down P2.0 to P2.7
	LPC_GPIOINT ->IO2IntEnF |= ~(0xFF<<0); 	 //falling int P2.0 to P2.7
	LPC_GPIOINT ->IO2IntEnR |= ~(0xFF<<0);	 // rising int P2.0 to P2.7

	NVIC_EnableIRQ(EINT3_IRQn); // enable gpio interrupts
}


// handler

EINT3_IRQHandler(void)
{
	for(int i = 15; i>0; i--)
	{
		data[i] = data [i-1]; // roto a la derecha los datos, tambien puede ser a la izq
	}

	data[0] = LPC_GPIO2 ->FIOPIN0; // guardo el ultimo dato siempre en el lugar 0

	LPC_GPIOINT->IO2IntClr = 0xFF;	// limpio las posibles flags
}

