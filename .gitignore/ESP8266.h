/*
 * ESP8266.h
 *
 * Created: 02-01-2018
 * Author : Norlinux
 * Based in the source code Piotr Styczy?ski
 * http://www.microelectronic.pt
 * http://maquina.96.lt
 * https://www.facebook.com/MundoDosMicrocontroladores/
 * Released under GPLv3.
 * Please refer to LICENSE file for licensing information.
 * which can be found at http://www.gnu.org/licenses/gpl.txt
 */ 


#ifndef ESP8266_H_
#define ESP8266_H_

/**
 * @def Buffer_Len
 * Determines buffer length of internal command buffer.
 *
 * The responses from ESP8266 normally should never fill this up!
 */
#define Buffer_Len 700


/**
 * @def WAIT_FOR_DATA
 *
 * Value signalizator for WIFI_Listen to listen for incoming data.
 * Listening would end if and only if:
 *   * status OK/ERROR is returned from ESP8266
 *   * timeout event occurs when waiting for data
 *   * ESP8266 received data and returns +IPD command
 *     then the data is captured and listening ends.
 */
#define WAIT_FOR_DATA 1
#define ESP_Mode_STA  1                                /*!< ESP in station mode */
#define ESP_Mode_AP  2                                 /*!< ESP as software Access Point mode */
#define ESP_Mode_STA_AP  3							   /*!< ESP station mode Both*/
#define Single  0									   /*!< ESP Single Connection*/
#define Multiple  1									   /*!< ESP Multiple Connection*/

/******************************************************************************/
/***                           Private structures                           ***/
/******************************************************************************/

typedef struct {
	uint8_t Length;
	uint8_t Data[128];
} Received_t;
/**
 * \brief   Low level structure for driver
 * \note    For now it has basic settings only without hardware flow control.
 */
typedef struct _ESP_Baud_t {
    uint32_t Baudrate;          /*!< Baudrate to be used for UART */
} ESP_Baud_t;

/**
 * \brief           ESP8266 library possible return statements on function calls
 */
typedef enum _ESP_Result {
	espERROR = 0,                                    /*!< An error occurred */
	espOK,											    /*!< Everything is OK */
	espDEVICENOTCONNECTED,                              /*!< Device is not connected to UART or wrong USSID */
	espTIMEOUT,                                         /*!< Timeout was detected when sending command to ESP module */
	espTIMEandPASS,										/*!< Timeout or Wrong password detected when sending command to ESP module */
	espWIFINOTCONNECTED,                                /*!< Wifi is not connected to network */
	espBUSY,                                            /*!< Device is busy, new command is not possible */
	espINVALIDPARAMETERS,                               /*!< Parameters for functions are invalid */
    espSENDERROR,                                       /*!< Error trying to send data on connection */
    espNOTFOUNDTARGETAP,                                /*!< Fail on connect from ESP-STA to ESP-AP */ 
    espWRONGPASSWORD,                                    /*!< Password is wrong */
	espRESTART											/*!< ESP8266 RESTART */
} ESP_Result;


typedef enum _ESP_Control_t {
    /**
     * \brief       Called to initialize low-level part of device, such as UART and GPIO configuration
     *
     * \param[in]   *param: Pointer to \ref ESP_LL_t structure with baudrate setup
     * \param[out]  *result: Pointer to \ref uint8_t variable with result. Set to 0 when OK, or non-zero on ERROR.
     */
    ESP_Control_Init = 0x00,     /*!< Initialization control */
    
    /**
     * \brief       Called to send data to ESP device
     *
     * \param[in]   *param: Pointer to \ref ESP_LL_Send_t structure with data to send
     * \param[out]  *result: Pointer to \ref uint8_t variable with result. Set to 0 when OK, or non-zero on ERROR.
     */
    ESP_Control_Send,            /*!< Send data control */   
} ESP_Control_t;

/**
 * \brief   Structure for sending data to low-level part
 */
typedef struct _ESP_Send_t {
    const uint8_t* Data;            /*!< Pointer to data to send */
    uint16_t Count;                 /*!< Number of bytes to send */
    uint8_t Result;                 /*!< Result of last send */
} ESP_Send_t;

extern ESP_Result   ESP_Begin(uint8_t);
extern uint8_t		ESP_Callback(ESP_Control_t , void* , void* );
extern ESP_Result   WIFI_Listening(char*, char*,char*);
extern ESP_Result	ESP_Init();
extern ESP_Result	ESP_Restart(void);
extern ESP_Result	ESP_WIFIMode(uint8_t , uint8_t );
extern ESP_Result	ESP_WIFIConnect(const char* , const char*);
extern ESP_Result   WIFI_Client_Connection (const char*, const char*, const int);
extern ESP_Result	WIFI_Send (const char* );
static ESP_Send_t Send;                                         /* Send data setup */

#define FROMMEM(x)                          ((const char *)(x))
#define UART_SEND_STR(str)                  do { Send.Data = (const uint8_t *)(str); Send.Count = strlen((const char *)(str)); ESP_Callback(ESP_Control_Send, &Send, &Send.Result); } while (0)
#define UART_SEND(str, len)                 do { Send.Data = (const uint8_t *)(str); Send.Count = (len); ESP_Callback(ESP_Control_Send, &Send, &Send.Result); } while (0)
#define UART_SEND_CH(ch)                    do { Send.Data = (const uint8_t *)(ch); Send.Count = 1; ESP_Callback(ESP_Control_Send, &Send, &Send.Result); } while (0)

#define RESP_OK                             ("\rOK\r")
#define RESP_OK_CR							("OK\r")
#define RESP_ATE_OK							("ATE0\r\rOK\r")
#define RESP_ERROR                          ("\rERROR\r")
#define RESP_BUSY                           ("\rbusy p...\r")
#define RESP_READY                          ("\rready\r")
#define RESP_GOT_IP							("WIFI GOT IP\r")
#define RESP_CONNECT						("WIFI CONNECTED\r")
#define RESP_CONNECTED						("CONNECT\r")
#define RESP_CONNECT_CR						("\rCONNECT\r")
#define RESP_CONNECT_OK						("CONNECT\r\rOK\r")
#define RESP_DISCONNECT						("WIFI DISCONNECT\r")
#define RESP_ATMEGA							("atmega.php\r")
#define RESP_CLOSED							("CLOSED\r")
#define RESP_SEND_OK						("SEND OK\r")
#define _CRLF                               FROMMEM("\r\n")

#define ESP8266_listen(str,alt1,alt2)		WIFI_Listening(str,alt1,alt2)			

#endif /* ESP8266_H_ */