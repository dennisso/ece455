/*
 * Lab 1: Simple Timing
 * Measuring time
 * 1) A delay function using a pair of nested for loops
 * 2) A clock that shows minutes and seconds on the screen using the delay function
 * 3) A delay function using a clock
 */

#include <lpc17xx.h>
#include <stdio.h>
#include <stdlib.h>
#include "glcd.h"

typedef enum states {
	START,
	DOT1,	// 1
	DASH1,	// 2
	DASH2,	// 2
	DOT2,	// 1
	DASH3,	// 2
	DOT3,	// 1
	DOT4	// 1
} states_t;

int button_held_count = 0;
int button_state = 0, previous_button_state = 0;
char buffer[64] = "";
int is_complete = 0;
states_t state = START;

// A timer to determine how long the button is pressed
void timer0_init(void)
{
	LPC_TIM0->TCR = 0x02; // reset timer		
	LPC_TIM0->TCR = 0x01; // enable timer
	LPC_TIM0->MR0 = 500000; // 500000 === 20 msec
	LPC_TIM0->MCR |= 0x03; // on match, generate interrupt and reset
	NVIC_EnableIRQ(TIMER0_IRQn); // allow interrupts from the timer	
}

void TIMER0_IRQHandler (void)
{
	if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
	{		
		LPC_TIM0->IR |= 1 << 0; // Clear MR0 interrupt flag
		// below only runs when the button is pressed
		if (~(LPC_GPIO2->FIOPIN >> 10) & 0x01)
		{
			button_held_count++;			
			if (button_held_count > 15) 
			{
				button_state = 2;				
				previous_button_state = button_state;
			} 
			else if (button_held_count > 5) 
			{
				button_state = 1;
				previous_button_state = button_state;
			}
		} 
		else 
		{
			if (button_held_count > 5)
			{
				switch(state) 
				{
					case START:
						state = (previous_button_state == 1) ? DOT1 : START;
						break;
					case DOT1:
						if (previous_button_state == 1)
						{
							state = DOT1;
						}
						else if (previous_button_state == 2)
						{
							state = DASH1;
						}
						break;
					case DASH1:
						if (previous_button_state == 1)
						{
							state = DOT1;
						} 
						else if (previous_button_state == 2)
						{
							state = DASH2;
						}
						break;
					case DASH2:
						if (previous_button_state == 1)
						{
							state = DOT2;
						} 
						else if (previous_button_state == 2)
						{
							state = START;
						}						
						break;
					case DOT2:
						if (previous_button_state == 1)
						{
							state = DOT1;
						} 
						else if (previous_button_state == 2)
						{
							state = DASH3;
						}	
						break;
					case DASH3:
						if (previous_button_state == 1)
						{
							state = DOT3;
						} 
						else if (previous_button_state == 2)
						{
							state = DASH2;
						}	
						break;
					case DOT3:
						if (previous_button_state == 1)
						{
							state = DOT4;
						} 
						else if (previous_button_state == 2)
						{
							state = DASH1;
						}	
						break;
					case DOT4:
						if (previous_button_state == 1)
						{
							state = DOT1;
						} 
						else if (previous_button_state == 2)
						{
							state = DASH1;
						}	
						break;
				}
			}
			
			button_held_count = 0;
			button_state = 0;			 
		}
	}
}

// GPIO
void gpio_init(void) 
{
	LPC_PINCON->PINSEL4 &= ~(3<<20);
	LPC_GPIO2->FIODIR &= ~(1<<10);
}

// Main
int main(void)
{
	SystemInit();
	GLCD_Init();
	GLCD_Clear(White);

	gpio_init();
	timer0_init();	
	
	while(1)
	{
		if (state == DOT4)
		{
			is_complete = 1;
		}
		
		sprintf(buffer, "%s", (is_complete == 1 ? "  CORRECT" : "INCORRECT"));
		GLCD_DisplayString(0, 0, 1, (unsigned char*) buffer);
		sprintf(buffer, "STATE: %02d", state);
		GLCD_DisplayString(1, 0, 1, (unsigned char*) buffer);
		
		if (previous_button_state == 0)
		{			
			sprintf(buffer, "BUTTONS: NULL");
		}
		else if (previous_button_state == 1)
		{			
			sprintf(buffer, "BUTTONS:  DOT");
		}
		else if (previous_button_state == 2)
		{			
			sprintf(buffer, "BUTTONS: DASH");
		}
		GLCD_DisplayString(2, 0, 1, (unsigned char*) buffer);			
	}
}
