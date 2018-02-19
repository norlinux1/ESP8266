/*
* USART.c
* Copyright by Peter Fleury (C)
* Created: 22-12-2017 
* Adapted By Author : Norlinux
*http://www.microelectronic.pt
*http://maquina.96.lt
*https://www.facebook.com/MundoDosMicrocontroladores/
*Released under GPLv3.
*Please refer to LICENSE file for licensing information.
*which can be found at http://www.gnu.org/licenses/gpl.txt
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include <stdio.h>					/* Include for printf */
#include <avr/sfr_defs.h>	//para bit especiais
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "USART_V1.h"
/*-----------------------------------------------------------------------------*/
//Adapted for printf(check at the end of this file )
#define WS_UCSR0A UCSR0A
#define WS_UDR0 UDR0
static int uart_putchar(char c, FILE *stream);  //Add custom putchar function
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);
/*----------------------------------------------------------------------------*/
//#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
/*************************************************************************
Function: uart0_init()
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
void uart0_init(uint16_t baudrate)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		UART_TxHead = 0;
		UART_TxTail = 0;
		UART_RxHead = 0;
		UART_RxTail = 0;
	}
	
	/* Set baud rate */
	if (baudrate & 0x8000) {
		UART0_STATUS = (1<<U2X0);  //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRR0H = (uint8_t)(baudrate>>8);
	UBRR0L = (uint8_t) baudrate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */

	UCSR0C = (3<<UCSZ00);
	stdout = &mystdout;       //Must add 
	sei();

	} /* uart0_init */
/* Another way to init Uart*/
void USART_Init_baud(uint16_t ubrr_value)
{

   //Set Baud rate
   UBRR0L = ubrr_value;
   UBRR0H = (ubrr_value>>8);

   /*Set Frame Format


   >> Asynchronous mode
   >> No Parity
   >> 1 StopBit
   >> char size 8

   */

	UART0_STATUS = (1<<U2X0);  //Enable 2x speed
	//UCSR0A=(1<<U2X0); // dobro velocidade
	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	//UCSR0C=(1<<UCSZ01)|(1<<UCSZ00); // MODO ASSINCRONO, SEM PARIDADE 1 STOP BIT, 8 BITS DADOS
	UCSR0C = (3<<UCSZ00);
	stdout = &mystdout;       //Must add 
	sei();
}	
	
/***************************************************************************************************
                         void UART_SetBaudRate(uint32_t v_baudRate_u32)
 ***************************************************************************************************
 * I/P Arguments: uint32_t : v_baudRate_u32 to be configured.
 * Return value	: none

 * description  :This function is used to Set/Change the baudrate on the fly.
                 If the requested baud rate is not within the supported range then
                 the default baudrate of 9600 is set.

		    Refer uart.h file for Supported range of baud rates.
***************************************************************************************************/
void UART_SetBaudRate(uint32_t v_baudRate_u32)
{
	uint16_t RegValue;

	if((v_baudRate_u32 >= C_MinBaudRate_U32) && (v_baudRate_u32<=C_MaxBaudRate_U32))
	{
		/* Check if the requested baudate is within range,
	     If yes then calculate the value to be loaded into baud rate generator. */
		RegValue = M_GetBaudRateGeneratorValue(v_baudRate_u32);
	}
	else
	{
		/*	 Invalid baudrate requested, hence set it to default baudrate of 9600 */
		RegValue = M_GetBaudRateGeneratorValue(9600);
	}

	UBRR0L = util_ExtractByte0to8(RegValue);
	UBRR0H = util_ExtractByte8to16(RegValue);
}
/***************************************************************************************************
                         void UART_Init(uint32_t v_baudRate_u32)
****************************************************************************************************
 * I/P Arguments: uint32_t : Baudrate to be configured.
 * Return value	: none

 * description  :This function is used to initialize the UART at specified baud rate.
                 If the requested baud rate is not within the supported range then
                 the default baud rate of 9600 is set.


		    Refer uart.h file for Supported(range) baud rates.		
***************************************************************************************************/
void USART_Init(uint32_t v_baudRate_u32)
{
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			UART_TxHead = 0;
			UART_TxTail = 0;
			UART_RxHead = 0;
			UART_RxTail = 0;
		}
	UART0_STATUS = (1<<U2X0);  //Enable 2x speed
	UCSR0B= _BV(RXCIE0)| (1<<RXEN) | (1<<TXEN);                  // Enable Receiver and Transmitter
	UCSR0C= (1<<UCSZ1) | (1<<UCSZ0);   // Async-mode
	
	UART_SetBaudRate(v_baudRate_u32);
}




	
ISR(UART0_RECEIVE_INTERRUPT)
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
{
	uint16_t tmphead;
	uint8_t data;
	uint8_t usr;
	uint8_t lastRxError;
	
	/* read UART status register and UART data register */
	usr  = UART0_STATUS;
	data = UART0_DATA;
	
	lastRxError = (usr & (_BV(FE0)|_BV(DOR0)));

	/* calculate buffer index */
	tmphead = (UART_RxHead + 1) & UART_RX0_BUFFER_MASK;
	
	if (tmphead == UART_RxTail) {
		/* error: receive buffer overflow */
		lastRxError = UART_BUFFER_OVERFLOW >> 8;
		} else {
		/* store new index */
		UART_RxHead = tmphead;
		/* store received data in buffer */
		UART_RxBuf[tmphead] = data;
	}
	UART_LastRxError = lastRxError;
}

ISR(UART0_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
	uint16_t tmptail;

	if (UART_TxHead != UART_TxTail) {
		/* calculate and store new buffer index */
		tmptail = (UART_TxTail + 1) & UART_TX0_BUFFER_MASK;
		UART_TxTail = tmptail;
		/* get one byte from buffer and write it to UART */
		UART0_DATA = UART_TxBuf[tmptail];  /* start transmission */
		} else {
		/* tx buffer empty, disable UDRE interrupt */
		UART0_CONTROL &= ~_BV(UART0_UDRIE);
	}
}	

/*************************************************************************
Function: uart0_getc()
Purpose:  return byte from ringbuffer
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
uint16_t uart0_getc(void)
{
	uint16_t tmptail;
	uint8_t data;

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		if (UART_RxHead == UART_RxTail) {
			return UART_NO_DATA;   /* no data available */
		}
	}
	/* calculate / store buffer index */
	tmptail = (UART_RxTail + 1) & UART_RX0_BUFFER_MASK;
	
	UART_RxTail = tmptail;
	
	/* get data from receive buffer */
	data = UART_RxBuf[tmptail];

	return (UART_LastRxError << 8) + data;

} /* uart0_getc */


/*************************************************************************
Function: uart0_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none
**************************************************************************/
void uart0_putc(uint8_t data)
{

		uint16_t tmphead;
		
		tmphead = (UART_TxHead + 1) & UART_TX0_BUFFER_MASK;
		
		while (tmphead == UART_TxTail); /* wait for free space in buffer */

		UART_TxBuf[tmphead] = data;
		UART_TxHead = tmphead;

		/* enable UDRE interrupt */
		UART0_CONTROL |= _BV(UART0_UDRIE);

		} /* uart0_putc */

/*************************************************************************
Function: uart0_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none
**************************************************************************/
void uart0_puts(const char *s)
{
	while (*s) {
		uart0_putc(*s++);
	}

	} /* uart0_puts */


/*************************************************************************
Function: uart0_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart0_puts_p(const char *progmem_s)
{
	register char c;

	while ((c = pgm_read_byte(progmem_s++))) {
		uart0_putc(c);
	}

	} /* uart0_puts_p */



	/*************************************************************************
	Function: uart0_available()
	Purpose:  Determine the number of bytes waiting in the receive buffer
	Input:    None
	Returns:  Integer number of bytes in the receive buffer
	**************************************************************************/
	uint16_t uart0_available(void)
	{
		uint16_t ret;
		
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			ret = (UART_RX0_BUFFER_SIZE + UART_RxHead - UART_RxTail) & UART_RX0_BUFFER_MASK;
		}
		return ret;
	} /* uart0_available */

		/*************************************************************************
		Function: uart0_flush()
		Purpose:  Flush bytes waiting the receive buffer. Actually ignores them.
		Input:    None
		Returns:  None
		**************************************************************************/
		void uart0_flush(void)
		{
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				UART_RxHead = UART_RxTail;
			}
		} /* uart0_flush */

/*************************************************************************
		Function: uart_putchar()
		Purpose:  From RS232 sends a byte
		Input:    None
		Returns:  None
**************************************************************************/
static int uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

void putUsart0(unsigned char c)
{
	while(!(WS_UCSR0A & 0x20));
	WS_UDR0 = c;
}
//From receiving a RS232 bytes
unsigned int getUsart0(void)
{
	unsigned char status,res;
	if(!(WS_UCSR0A & 0x80)) return 0xFFFF;        //no data to be received
	status = WS_UCSR0A;
	res = WS_UDR0;
	if (status & 0x1c) return 0xFFFF;        // If error, return 0xFFFF
	return res;
}
//Waiting for receiving a from RS232 effective bytes
unsigned char waitUsart0(void)
{
	unsigned int c;
	while((c=getUsart0())==0xFFFF);
	return (unsigned char)c;
}

/*************************************************************************
		Function: FOR ESP8266 Project by @author  Norlinux
**************************************************************************/
// New Functions for ESP8266 Project

void ESP8266_Send(uint8_t* DataArray, uint16_t count) {
	/* Go through entire data array */
	while (count--) {
		/* Wait to be ready, buffer empty */
		//uart0_flush();
		/* Send data */
		uart0_putc(*DataArray++);
		/* Wait to be ready, buffer empty */
		uart0_flush();
	}
}

//void ESP8266_Send(uint8_t* DataArray, uint16_t count) {
	//while (count--) {
		//uart0_flush();
		//uart0_putc(*DataArray++);
		//}
//}
