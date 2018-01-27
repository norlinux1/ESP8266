/*
 * ESP8266.c
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
#include <inttypes.h>				/* Include uintxx_t library */
#include <string.h>					/* Include string library */
#include <stdio.h>					/* Include standard IO library */
#include <util/delay.h>				/* Include Delay header file */
//#include <stdlib.h>					/* Include free memory file */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "ESP8266.h"
#include "lcd_lib.h"
#include "USART.h"
#include "Timer2.h"
#define delay() _delay_ms(5);
#define Wait() _delay_ms(800);
//static uint8_t IPD_Data[ESP_CONNBUFFER_SIZE + 1];           /* Data buffer for incoming connection */
// Variavel para confirmar que foi enviado com sucesso.
//uint8_t WIFI_EVENT_IPD_OK=0;

volatile static Timer_Status_t Timer2;
char msg[18]={};
	/**
 * Internal command buffer.
 * The responses from ESP8266 normally should never fill this up!
 */
char Wifi_Buffer[Buffer_Len];
/******************************************************************************/
/***                              Public API                                 **/
/******************************************************************************/

ESP_Result WIFI_Disconnect(char* RespData);
int WIFI_Events ( ESP_Result esp_Events)
{
	switch(esp_Events){
		case espOK:
		LCDclr();
		LCDprintXY("ESP OK...",0,0);
		//return 1;
		break;
		case espBUSY:
		LCDclr();
		LCDprintXY("ESP Busy.RESTART",0,0);
		return 0;
		break;
		case espSENDERROR:
		LCDclr();
		LCDprintXY("ESP SEND ERROR",0,0);
		break;
		case espWIFINOTCONNECTED:
		LCDclr();
		LCDprintXY("WIFI NOT CONNECT",0,0);
		break;
		case espERROR:
		LCDclr();
		LCDprintXY("ERRO WIFI CONNEC",0,0);
		break;
		case espTIMEOUT:
		LCDclr();
		LCDprintXY("ESP TIMEOUT :<",0,0);
		return 0;
		break;
		case espWRONGPASSWORD:
		LCDclr();
		LCDprintXY("WRONG PASSWORD",0,0);
		break;
		case espNOTFOUNDTARGETAP:
		LCDclr();
		LCDprintXY("NOT FOUND TARGET",0,0);			// maybe USSID wrong
		break;
		case espDEVICENOTCONNECTED:
		LCDclr();
		LCDprintXY("NOT CONNECTED",0,0);			// maybe USSID wrong
		return 0;
		break;
		case espINVALIDPARAMETERS:
		LCDclr();
		LCDprintXY("INVALID PARAMETR",0,0);
		break;
		case espRESTART:
		LCDclr();
		LCDprintXY("ESP8266 RESTART",0,0);		// tried reconnect maybe wrong password WIFI
		Wait();
		break;
		case espTIMEandPASS:
		LCDclr();
		LCDprintXY("PASSWRD TIMEOUT",0,0);
		return 0;
		break;
	}
	return 0;
}

ESP_Result WIFI_Listening(char* RespData, char* RespAlternData1, char* RespAlternData2) {
	int i = 0;
	uart_flush();
	int count = 0;
	//Wifi_Buffer[count] = '\0';
	memset(Wifi_Buffer, 0, Buffer_Len);        			/* Set buffer values to all zeros */
	uint16_t input_count = 0;
	uint16_t Rx_Usart = 0;
	//int input_hb = 0;
	int input_lb = 0;
	while(1) {
		++i;
		if(i > F_CPU) {
			WIFI_Events(espTIMEOUT);
			return 0;
		}
		//Waiting for buffer
		delay();
		if((input_count = uart_available()) > 0)
		{ 
			//for(int i=0;i<input_count;++i) {
				while (input_count--) {
				Rx_Usart = uart_getc();
				input_lb = Rx_Usart & 0xFF; /* Convert value to string */
				if(input_lb != '\n') {
					
					Wifi_Buffer[count] = input_lb;
					++count;
					Wifi_Buffer[count] = '\0';
				} else {
				//WIFI_Events(espBUSY);
				if(count <= 1) {LCDclr(); LCDprintXY("NO BUFFER",0,0); // Will imcrease this code in future
				return espRESTART;
				}
				else {
					LCDclr();
					LCDprintXY(Wifi_Buffer,0,0);
					if(strcmp(Wifi_Buffer, RespData) == 0){
					return espOK;
					}else if(strcmp(Wifi_Buffer, RespAlternData1) == 0){
					return espOK;
					}else if(strcmp(Wifi_Buffer, RespAlternData2) == 0){
					return espOK;
					}else if(strncmp(Wifi_Buffer, "+IPD,", 5)==0){
					return espOK;	
					}else if (strncmp(Wifi_Buffer, FROMMEM("+IPD"), 4) == 0){
					return espOK;
					}else if (strcmp(Wifi_Buffer, "busy p...\r") == 0){
					return espBUSY;
					}else if (strcmp(Wifi_Buffer, "ALREADY CONNECTED\r") == 0){
					return espOK;
					}else if (strcmp(Wifi_Buffer,RESP_DISCONNECT)==0){
						memset(Wifi_Buffer, 0, Buffer_Len);
						 WIFI_Events(espDEVICENOTCONNECTED);
						 //return espDEVICENOTCONNECTED;
					}
					else if (strstr(Wifi_Buffer,"ets"))
					{
						memset(Wifi_Buffer, 0, Buffer_Len);
						WIFI_Events(espRESTART);
						return espRESTART;
					}
					else if(strstr(Wifi_Buffer, "+CWJAP:1"))
					WIFI_Events(espTIMEandPASS);
					else if(strstr(Wifi_Buffer, "+CWJAP:2"))
					WIFI_Events(espWRONGPASSWORD);
					else if(strstr(Wifi_Buffer, "+CWJAP:3"))
					WIFI_Events(espNOTFOUNDTARGETAP);
					else if(strstr(Wifi_Buffer, "+CWJAP:4"))
					WIFI_Events(espDEVICENOTCONNECTED);
					}
				//count = 0;
				//Wifi_Buffer[count] = '\0';
				}
			}
		}
		return 0; 
	}
	
}



	
uint8_t ESP_Callback(ESP_Control_t ctrl, void* param, void* result) {
    	
	switch (ctrl) {
        case ESP_Control_Init: {                 /* Initialize low-level part of communication */
            ESP_Baud_t* USART = (ESP_Baud_t *)param;       /* Get low-level value from callback */
            
            /************************************/
            /*  Device specific initialization  */
            /************************************/
				USART_Init_baud(USART->Baudrate);
				LCDprintXY("USART INIT...",0,0);
			    Wait();   
            if (result) {
				*(uint8_t *)result = 0;             /* Successfully initialized */
            }
            return 1;                               /* Return 1 = command was processed */
        }
        case ESP_Control_Send: {
            ESP_Send_t* send = (ESP_Send_t *)param;   /* Get send parameters */
            /* Send actual data to UART */
            ESP8266_Send((uint8_t *)send->Data, send->Count);   /* Send actual data */
            if (result) {
                *(uint8_t *)result = 0;             /* Successfully send */
            }
            return 1;                               /* Command processed */
        }
         default: 
            return 0;
    }
}
/* Initialize necessary parts */
ESP_Result ESP_Init(void) {
	UART_SEND_STR(FROMMEM("ATE0"));               /* Send data ATE0 Echo*/
	UART_SEND_STR(_CRLF);
	delay()
	while(!ESP8266_listen(RESP_OK,RESP_ATE_OK,RESP_CONNECT));
	LCDprintXY("ATE0  OK...",0,0);
	Wait();
	return espOK;
}



ESP_Result ESP_Begin(uint8_t baudrate) {
	uint8_t result=1;									/* Set to default value first */
	if(!ESP_Callback(ESP_Control_Init, (void *)&baudrate, &result)|| result){
	return espERROR;									/* Return error */
	}
		return (ESP_Init());
   }
   
  ESP_Result ESP_Restart(void)
  {	
	  UART_SEND_STR(FROMMEM("AT+RST"));               /* Send data RESET*/
	  UART_SEND_STR(_CRLF);
	  delay();
	 while (timeout_event(60) && !ESP8266_listen(RESP_GOT_IP,RESP_CONNECT,RESP_OK)); //
	 if(Timer2.Timeout==1)
	 return espERROR;
	 else
	 return espOK;
	 
  }
 ESP_Result ESP_WIFIMode(uint8_t Mode, uint8_t Connection)
 {
	 char atBuffer[16],str[32];
	 memset(atBuffer, 0, strlen(atBuffer));
	 memset(str,0,strlen(str));
	 /* 1 = Station mode (client)
	   2 = AP mode (host)
	   3 = AP + Station mode (Yes, ESP8266 has a dual mode!)*/
	 sprintf(atBuffer, "AT+CWMODE=%d", Mode);
	 strcpy(str, atBuffer);
	 strcat(str,"\r\rOK\r");
	 UART_SEND_STR(atBuffer);               /* Send data MODE*/
	 UART_SEND_STR(_CRLF);
	 while (!ESP8266_listen(RESP_OK,RESP_OK_CR,str));
	 LCDprintXY(str,0,0);
	 memset(atBuffer, 0, strlen(atBuffer));
	 memset(str,0,strlen(str));
	  /* 0 = Single Connection
		1 = Multiple Connection*/	
	 sprintf(atBuffer, "AT+CIPMUX=%d",Connection); 
	 strcpy(str, atBuffer);
	 strcat(str,"\r\rOK\r");
		UART_SEND_STR(atBuffer);
		UART_SEND_STR(_CRLF);
	while (!ESP8266_listen(RESP_OK,RESP_OK_CR,str));
	LCDprintXY(str,0,0);
	return espOK;

 }

ESP_Result ESP_WIFIConnect(const char* ssid, const char* passwd){
	 char atBuffer[90];
	 /*String, AP’s SSID
	 pwd?String, not longer than 64 characters*/
	memset(atBuffer, 0, strlen(atBuffer));
	sprintf(atBuffer, "AT+CWJAP=\"%s\",\"%s\"", ssid, passwd);
	//atBuffer[strlen(atBuffer-1)] = '\0';
		UART_SEND_STR(atBuffer);               /* Send data */
		UART_SEND_STR(_CRLF);
		delay();
	//while (timeout_event(10) && 1);
	while (timeout_event(120) && !ESP8266_listen(RESP_GOT_IP,RESP_OK,RESP_OK_CR));
	//return espERROR == Timer2.Timeout ? espERROR : espOK;
		 if(Timer2.Timeout==1)
		 return espERROR;
		 else
		 return espOK;
}

ESP_Result WIFI_Client_Connection (const char* Protocol, const char* Host, const int Port)
{
	char atBuffer[128];
	//resent:
	memset(atBuffer, 0, strlen(atBuffer));
	sprintf(atBuffer, "AT+CIPSTART=\"%s\",\"%s\",%d", Protocol, Host, Port);
	
		//UART_SEND_STR(atBuffer);               /* Send data */
		//UART_SEND_STR(_CRLF);
		do 
		{
		Wait();
		UART_SEND_STR(atBuffer);               /* Send data */
		UART_SEND_STR(_CRLF);
		} while(timeout_event(30) && !ESP8266_listen(RESP_CONNECT_OK,RESP_CONNECT_CR,RESP_CONNECTED));
		 if(Timer2.Timeout==1)
		 return espERROR;
		 else
		 return espOK;
}

ESP_Result WIFI_Send (const char* Data)
{
	const int Length = strlen(Data);
	char atBuffer[Buffer_Len];
	memset(atBuffer, 0, Buffer_Len);
	sprintf(atBuffer, "AT+CIPSEND=%d", Length);
	UART_SEND_STR(atBuffer);               /* Send data */
	UART_SEND_STR(_CRLF);
	//delay();
	while (!ESP8266_listen(RESP_OK,RESP_OK_CR,RESP_SEND_OK));
	UART_SEND_STR(Data);               /* Send data */
	UART_SEND_STR(_CRLF);
	delay();
	while (!ESP8266_listen(RESP_ATMEGA,RESP_CLOSED,RESP_OK));
	return espOK;
}


