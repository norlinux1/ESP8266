/*
 * Timer.h
 *
 * Created: 13-01-2018 10:49:00
 *  Author: Utilizador
 */ 


#ifndef TIMER2_H_
#define TIMER2_H_
/*
* Timer2.c
*
* Created: 13-01-2018 
* Author : Norlinux
* http://www.microelectronic.pt
* http://maquina.96.lt
* https://www.facebook.com/MundoDosMicrocontroladores/
* Released under GPLv3.
* Please refer to LICENSE file for licensing information.
* which can be found at http://www.gnu.org/licenses/gpl.txt
*/


#define INIT_TIMER2 TCCR2 |= 1<<CS22  
#define STOP_TIMER2 TCCR2 &= 0<<CS22
#define CLEAR_TIMER2 TCNT2 = 0

typedef struct _Timer_Status_t {
	uint8_t Timeout:1;                                       /*!< For internal use only */
   uint8_t TimerActive:1;
   uint8_t secnd;                                   
} Timer_Status_t;


extern void TimerInit(void);
extern uint8_t timeout_event(uint8_t);

#endif /* TIMER_H_ */