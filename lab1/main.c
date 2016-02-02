/*
 * Lab 1: Simple Timing
 * Measuring time
 * 1) A delay function using a pair of nested for loops
 * 2) A clock that shows minutes and seconds on the screen using the delay function
 * 3) A delay function using a clock
 */

#define BUSY_WAIT  1

#include <lpc17xx.h>
#include <stdio.h>
#include "glcd.h"

// clock
typedef struct timespec {
	int minutes;
	int seconds;
	char text[10];
} timespec;

timespec clock;

// clock init
void clock_init(void)
{
	clock.minutes = 0;
	clock.seconds = 0;
	clock.text[0] = '\0';
	return;
}

void clock_increment(void)
{
	// update clock
	clock.seconds++;
	if (clock.seconds == 60)
	{
		clock.minutes++;
		clock.seconds = 0;
	}	
	return;
}
void clock_print(void)
{
	sprintf(clock.text, "%d:%d", clock.minutes, clock.seconds);
	return;
}

// A delay using a pair of nested for loops (busy waiting)
void delay_busy_wait(int millisec)
{
	int i, j;
	//TODO: set const for this
	int init_t = millisec;
	
	__disable_irq();
	for(i = init_t; i > 0; i--)
	{
		// not sure why we need a nested loop
		for(j = 1000; j > 0; j--)
		{
		}
	}
	__enable_irq();
	return;
}

// A delay using a hardware timer
void timer0_init(void)
{
	LPC_TIM0->TCR = 0x02; // reset timer	
	LPC_TIM0->TCR = 0x01; // enable timer
	LPC_TIM0->MR0 = 2048; // TODO: set timer for 1 second
	LPC_TIM0->MCR |= 0x03; // on match, generate interrupt and reset
	NVIC_EnableIRQ(TIMER0_IRQn); // allow interrupts from the timer	
}

void TIMER0_IRQHandler (void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag
		clock_increment();
    }
}					 

int main(void)
{
	SystemInit();
	GLCD_Init();
	clock_init();

#ifndef BUSY_WAIT
	timer0_init();
#endif /* ifndef BUSY_WAIT */
	
	while(1)
	{
		GLCD_Clear(White);
		GLCD_DisplayString(0, 0, 1, (unsigned char*) clock.text);
		
		// Delay by 1000 milliseconds
#ifdef BUSY_WAIT
		delay_busy_wait(1000);
		clock_increment();
		clock_print();
#else
		// incrementing clock done within interrupt handlers
		clock_print();
#endif /* ifdef BUSY_WAIT */		
	}
}
