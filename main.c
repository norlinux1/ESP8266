/*
 * ESP8266 Norlinux Library.c
 *
 * Created: 02-01-2018
 * Author : Norlinux
 * Based in Source Code Piotr Styczy?ski
 * http://www.microelectronic.pt
 * http://maquina.96.lt
 * https://www.facebook.com/MundoDosMicrocontroladores/
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 * which can be found at http://www.gnu.org/licenses/gpl.txt
 */

#define F_CPU 8000000UL				/* Define CPU Frequency e.g. here its Ext. 8MHz */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>			/* for sei() */
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>					/* Include standard IO library */
#include <string.h>					/* Include string library */
#include <stdlib.h>					/* Include standard library */
#include "USART_V1.h"					/* Include USART header file */
#include "lcd_lib.h"				/* Include HD44780 header file */
#include "ESP8266.h"				/* Include ESP8266 header file */
#include "Timer2.h"
#define Wait() _delay_ms(1800);

//Strings stored in AVR Flash memory
const uint8_t Arranque[] PROGMEM="ESP8266 DEMO ...\0";
char msgBox[18]={};
/* Wifi access point ussid and password (to connect to) */
char* USSID = "MEO-Carvalho";
char* PASSWORD = "warkaput";
/* ESP working structure and result enumeration */
ESP_Result espResp;
int main(void)
{
    //char segundos=0;
	//uint8_t esp_Result;
	char msgData[160];
	char hostString[68]={"Connection: keep-alive\r\nCache-Control: max-age=0\r\n\r\n\r\n"};
	char Manobras=0,Energia=0;
	/*Start Timer2*/
	TimerInit();
	/* Start LCD 16x2 */
	LCDinit();

	LCDStringtoLCD(Arranque,0,0);
		if ((espResp = ESP_Begin(115200)) == espOK) {
			LCDprintXY("ESP INIT OK...",0,0);
			Wait();
			} 
			else {sprintf(msgBox,"ESP INIT ERR:%d",espResp);
			LCDprintXY(msgBox,0,0);
			Wait();

			}
			if ((espResp =ESP_WIFIMode(ESP_Mode_STA,Single))== espOK)
			{
				LCDprintXY("STATION OK...",0,0); Wait();	
			}
			else {sprintf(msgBox,"STATION ERRO:%d",espResp);
				LCDprintXY(msgBox,0,0);
				Wait();	
			}
			
			if (ESP_WIFIConnect(USSID,PASSWORD)==espOK)
				{
					sprintf(msgBox,"WIFI %s",USSID);
					LCDprintXY(msgBox,0,0);
					Wait();
					}else{
							LCDprintXY("ERRO WIFI RESET",0,0);
							Wait();
							if (ESP_Restart()==espOK)
							{
								sprintf(msgBox,"WIFI %s",USSID);
								LCDprintXY(msgBox,0,0);
								Wait();
							}else{
							loop:
							LCDprintXY("SOMETHING WRONG...",0,0);
							Wait();
							LCDprintXY("REMOVE ESP8266...",0,0);
							Wait();
							LCDprintXY("CHECK USSID....",0,0);
							Wait();
							LCDprintXY("CHECK PASSWORD..",0,0);
							Wait();
							goto loop;
							}
						}

	
    while (1) 
    {
		
		    LCDclr();
			LCDprintXY("BEGIN SEND DATA",0,0);
		    // Connect to host
			Wait();
			uart_flush();
		    Manobras=1+(rand() % 40); // For aleatory numbers
		    Energia=1+(rand() % 30); // For aleatory numbers
		if (WIFI_Client_Connection("TCP", "maquina.96.lt", 80)==espOK)
		{
			sprintf(msgData,"GET /gravar.php?man=%d&energy=%d HTTP/1.1\r\nHost: maquina.96.lt\r\n%s",Manobras,Energia,hostString);
			WIFI_Send(msgData);
		}
		else
		{
			LCDclr();
			LCDprintXY("ERROR OPEN URL",0,0);
			Wait();
		}
	
		
		
		
		   // Check data in http://maquina.96.lt/default.php
		   while(timeout_event(25));

    }
}

