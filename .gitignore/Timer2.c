/*
 * Timer2.c
 *
 * Created: 14-01-2018 20:06:59
 *  Author: Utilizador
 */ 

#define F_CPU 8000000UL				/* Define CPU Frequency e.g. here its Ext. 8MHz */
#include <util/delay.h>				/* Include Delay header file */
#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <avr/sfr_defs.h>	/* for special bits */
#include "Timer2.h"


volatile static Timer_Status_t Timer2={0,1,0};
static volatile uint8_t gsec=0;
volatile unsigned char seconds=0,counter=0;

void TimerInit(void)
{
	/****************************************************************/
	//Clock input of TIMER2 = 8MHz/256 = 31250 Hz Frequency of Overflow = 31250 /256(8bits) = 122,07 Hz
	INIT_TIMER2;					//|1<<CS21); // Normal, Normal modo OC0 desconected,prescaller 256 ~122 interrupts/s
	TIMSK|=(1<<TOIE2);				//Enable Timer2 Overflow interrupts
	CLEAR_TIMER2;					// Init to count seconds
	sei();
	/*-------------------------------------------------------------*/
}

uint8_t timeout_event(uint8_t timer)
{
	if (Timer2.TimerActive==1)
	{
		Timer2.Timeout=0;
		seconds=0;
		CLEAR_TIMER2;
		INIT_TIMER2;
		Timer2.TimerActive=0;
		return 1;
	}
	else if(seconds>=timer)
	{
		STOP_TIMER2;
		_delay_ms(10);
		Timer2.Timeout=1;
		Timer2.TimerActive=1;
		return 0;
	}
	return 1;
}
ISR(TIMER2_OVF_vect)
{
	Timer2.TimerActive=0;
	counter++;
	if (counter==122) // corresponding to 1 sec
	{
		
		seconds++;
		counter=0;
	}

}
