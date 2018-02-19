/*
* USART.h
* Created: 22-12-2017 
* Copyright by Peter Fleury (C)
* Adapted By Author : Norlinux
*http://www.microelectronic.pt
*http://maquina.96.lt
*https://www.facebook.com/MundoDosMicrocontroladores/
*Released under GPLv3.
*Please refer to LICENSE file for licensing information.
*which can be found at http://www.gnu.org/licenses/gpl.txt
*/


#ifndef USART_H_
#define USART_H_
#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)
/***************************************************************************************************
                     Macros to extract the Byte
***************************************************************************************************/
#define util_ExtractByte0to8(x)    (uint8_t) ((x) & 0xFFu)
#define util_ExtractByte8to16(x)   (uint8_t) (((x)>>8) & 0xFFu)
// baude for 16Mhz
#define M_GetBaudRateGeneratorValue(baudrate)  (((16000000UL -((baudrate) * 8L)) / ((baudrate) * 16UL)))
/***************************************************************************************************
                             Baudrate configurations
***************************************************************************************************/
#define C_MinBaudRate_U32 2400
#define C_MaxBaudRate_U32 115200UL

/**************************************************************************************************/

/*
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /**< Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /**< Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /**< receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /**< no receive data available   */
/* Set size of receive and transmit buffers */


#define UART_RX0_BUFFER_SIZE 256 /**< 128 Size of the circular receive buffer, must be power of 2 */
#define UART_TX0_BUFFER_SIZE 256 /**< 128 Size of the circular transmit buffer, must be power of 2 */


/* size of RX/TX buffers */
#define UART_RX0_BUFFER_MASK (UART_RX0_BUFFER_SIZE - 1)
#define UART_TX0_BUFFER_MASK (UART_TX0_BUFFER_SIZE - 1)
static volatile uint8_t UART_TxBuf[UART_TX0_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf[UART_RX0_BUFFER_SIZE];
static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static volatile uint8_t UART_LastRxError;
/* Macros, to allow use of legacy names */

/** @brief Macro to initialize USART0 (only available on selected ATmegas) @see uart0_init */
#define uart_init(b)      uart0_init(b)

/** @brief Macro to get received byte of USART0 from ringbuffer. (only available on selected ATmega) @see uart0_getc */
#define uart_getc()       uart0_getc()

///** @brief Macro to peek at next byte in USART0 ringbuffer */
//#define uart_peek()       uart0_peek()

/** @brief Macro to put byte to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_putc */
#define uart_putc(d)      uart0_putc(d)

/** @brief Macro to put string to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_puts */
#define uart_puts(s)      uart0_puts(s)

/** @brief Macro to put string from program memory to ringbuffer for transmitting via USART0 (only available on selected ATmega) @see uart0_puts_p */
#define uart_puts_p(s)    uart0_puts_p(s)

/** @brief Macro to return number of bytes waiting in the receive buffer of USART0 @see uart0_available */
#define uart_available()  uart0_available()

/** @brief Macro to flush bytes waiting in receive buffer of USART0 @see uart0_flush */
#define uart_flush()      uart0_flush()

#define Baud_9600   103
#define Baud_115200 8

extern void		USART_Init(uint32_t);
extern void		USART_Init_baud(uint16_t);
extern void		uart0_init(uint16_t);
extern uint16_t uart0_getc();
extern void		uart0_putc(uint8_t );
extern void		uart0_puts(const char *);
extern void		uart0_puts_p(const char *);
extern uint16_t uart0_available();
extern void		uart0_flush();
// declaration functions
extern void		putUsart0(unsigned char );
// declarations for ESP8266
extern void		ESP8266_Send(uint8_t* , uint16_t );
#endif /* USART_H_ */