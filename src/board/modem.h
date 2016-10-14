/****************************************Copyright (c)****************************************************
**                                      
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               Modem.h
** Descriptions:            ∂‘GPRS ∑¢AT √¸¡Ó
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Arthur Wu
** Created date:            2014-02-20
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#ifndef __MODEM_H
#define __MODEM_H


/*---------------------- Includes ---------------------------------------------*/
#include <includes.h>
#include <time.h>
#include "target.h"
#include "usart.h"


/* define all GPRS parameters */
static char GPRS_apn[64] = "cmnet";
static char GPRS_userName[64] = "";
static char GPRS_password[64] = "";

//char strIMEI[24];

static char   GSM_IP_Local[20];
static char   GSM_IMEI[20];              // 15-BYTES / 17-BYTES.
static INT8U  GSM_CSQ;
static char   GSM_SIM_ID[21];            // 20-BYTES. 


/*---------------------- Definition -------------------------------------------*/
#define USE_PROMPT
#define USE_ACCEPT        

#define OS_UCOSII_USED      1 

#define GSM_CSQ_LEVEL_MIN   8                  // .


#define DELAY_OS_02S   (OS_TICKS_PER_SEC/5)    // 0.2S.       
#define DELAY_OS_05S   (OS_TICKS_PER_SEC/2)    // 0.5S.
#define DELAY_OS_1S    (OS_TICKS_PER_SEC)      // 1S.
#define DELAY_OS_2S    (OS_TICKS_PER_SEC*2)    // 2S.
#define DELAY_OS_5S    (OS_TICKS_PER_SEC*5)    // 5S.
#define DELAY_OS_10S   (OS_TICKS_PER_SEC*10)   // 10S.	
#define DELAY_OS_20S   (OS_TICKS_PER_SEC*20)   // 20S.	

#define AT_TIMEOUT_OS_02S      2    //   2 * 100mS = 2S.
#define AT_TIMEOUT_OS_05S      5    //   5 * 100mS = 2S.
#define AT_TIMEOUT_OS_1S      10    //  10 * 100mS = 2S.
#define AT_TIMEOUT_OS_2S      20    //  20 * 100mS = 2S.
#define AT_TIMEOUT_OS_5S      50    //  50 * 100mS = 2S.
#define AT_TIMEOUT_OS_10S    100    // 100 * 100mS = 2S.
#define AT_TIMEOUT_OS_20S    200    // 200 * 100mS = 2S.


/* AT command response */
typedef enum 
{
    ATE_NO_RESPONSE   = 0,
	ATE_SUCCESS       = 1,
	ATE_ERROR         = 2,
	ATE_BUSY          = 3,
	ATE_SEND_READY    = 4,
	ATE_RECEIVE_READY = 5,
	ATE_SEND_OK       = 6,
	ATE_GPRS_DETACHED = 7,
	ATE_TCPCLOSED     = 8,
	ATE_ACCPET        = ATE_SEND_OK
} ATE_response;


/* Socket state */
typedef enum 
{
	SOCKET_STAT_IPINITIAL  = 0,
	SOCKET_STAT_IPSTART    = 1,
	SOCKET_STAT_IPCONFIG   = 2,
	SOCKET_STAT_IPGPRSACT  = 3,
	SOCKET_STAT_IPSTATUS   = 4,
	SOCKET_STAT_CONNECTED  = 6,
	SOCKET_STAT_CONNECTING = 7,
	SOCKET_STAT_UNKNOWN    = 8
} SOCKET_state;


#define AT_IMEI_REQ                     "AT+GSN"
#define AT_CSQ_REQ                      "AT+CSQ"
#define AT_SIM_CARD_ID_REQ              "AT+CCID"       // "AT^SCID"
#define AT_CLOCK_REQ                    "AT+CCLK?"      // 

#define AT_ECHO_ENABLE                  "ATE"           // step1: ATE1.             
#define AT_NETREG_REQ                   "AT+CGREG"      // step2: net register? 
#define AT_GPRS_ATTACH                  "AT+CGATT"      // step3: GPRS attach?
#define AT_TCPIP_SET_APN_USR_PSW        "AT+CSTT"       // step4: set Apn.
#define AT_GPRS_ACTIVE                  "AT+CIICR"      // step5: active.
#define AT_IP_ADDR_REQ                  "AT+CIFSR"      // step6: get Local IP.  


#define AT_IP_APP_SETTING               "AT+SAPBR"
#define AT_DEFINE_PDP_CONTEXT           "AT+CGDCONT"


#define AT_TCPIP_SET_MUX_NUM            "AT+CIPMUX"
#define AT_TCPIP_SET_SEND_MODE          "AT+CIPQSEND"
#define AT_TCPIP_SET_SEND_PROMPT        "AT+CIPSPRT"
#define AT_TCPIP_SET_RCV_IP_HEAD        "AT+CIPHEAD"


#define AT_SOCKET_OPEN                  "AT+CIPSTART"   // step7: 
#define AT_SOCKET_SEND_DATA             "AT+CIPSEND"    // step8: 
#define AT_SOCKET_SHUTDOWN              "AT+CIPSHUT"    // 
#define AT_SOCKET_CLOSE                 "AT+CIPCLOSE"   // 
#define AT_SOCKET_GET_STATUS            "AT+CIPSTATUS"




/* Define FTP application setting */
#define  AT_FTP_SET_PORT        "AT+FTPPORT"
#define  AT_FTP_SET_CID         "AT+FTPCID"
#define  AT_FTP_SET_TYPE        "AT+FTPTYPE"
#define  AT_FTP_SET_INPUTOPT    "AT+FTPPUTOPT"
#define  AT_FTP_SET_MODE        "AT+FTPMODE"
#define  AT_FTP_SET_SERVER      "AT+FTPSERV"
#define  AT_FTP_SET_USERNAME    "AT+FTPUN"
#define  AT_FTP_SET_PASSWORD    "AT+FTPPW"
#define  AT_FTP_SET_GETNAME     "AT+FTPGETNAME"
#define  AT_FTP_SET_GETPATH     "AT+FTPGETPATH"
#define  AT_FTP_SET_PUTNAME     "AT+FTPPUTNAME"
#define  AT_FTP_SET_PUTPATH     "AT+FTPPUTPATH"
#define  AT_FTP_GET             "AT+FTPGET"
#define  AT_FTP_PUT             "AT+FTPPUT"
#define  AT_FTP_GET_STATE       "AT+FTPSTATE"


// const INT8U GSM_socket_sendOver[2] = {0x1A,'\0'}; 
	

void   AT_sendCmd(USART_PORT *uart, char* AT_cmd, INT32U retryNum);
ATE_response  AT_sendCmd_and_chkAck(USART_PORT *uart, char* AT_cmd, char* AT_ackTail, 
	                                INT32U retryNum, INT32U timeout);
ATE_response  AT_sendCmd_and_chkAck_multi(USART_PORT *uart, char* AT_cmd, char* AT_ackTail, 
	                                      INT32U retryNum, INT32U timeout);
INT8U  AT_chkAck(USART_PORT *uart, INT32U timeout);
char*  AT_chkAckStr(USART_PORT *uart, char* str, INT32U timeout);         //

INT8U  GSM_setEcho(USART_PORT *uart, int enable);
INT8U  GSM_getCSQ(USART_PORT *uart, INT16U retryNum);                     // .
INT8U  GSM_getIMEI(USART_PORT *uart, char *imei, INT8U strLen);
INT8U  GSM_getSimCardID(USART_PORT *uart, INT8U *simno, INT8U bufLen);
INT8U  GSM_getCurrTime(USART_PORT *uart, struct tm *pCurrTime);


INT8U  GSM_GPRS_attach(USART_PORT *uart, bool enable, INT16U retryNum);
INT8U  GSM_GPRS_active(USART_PORT *uart);
INT8U  GSM_definePDPcontext(USART_PORT *uart, char *apnName);
INT8U  GSM_getLocalIPAddr(USART_PORT *uart, char *IP_Local, INT32U timeout);
INT8U  GSM_setIPApplication(USART_PORT *uart, int cmd_type, int cid, char* ConParamTag, 
	                       char* ConParamValue);
INT8U  GSM_getStatNetReg(USART_PORT *uart, INT16U retryNum);


INT8U  GSM_socket_open(USART_PORT *uart, char* mode, char* IP_domain, INT16U peerPort, 
	                  INT16U retryNum, INT32U timeout);
INT8U  GSM_socket_shutdown(USART_PORT *uart, INT32U timeout);
INT8U  GSM_socket_close(USART_PORT *uart, INT32U timeout);
INT8U  GSM_socket_sendData(USART_PORT *uart, INT8U* dataBuf, INT16U dataLen, INT16U retryNum, INT32U timeout);
INT8U  GSM_socket_getData(USART_PORT *uart, INT8U *buf, INT16U bufLen, INT16U *gotLen, 
                         INT32U timeout);
INT8U  GSM_socket_getStatus(USART_PORT *uart, INT32U timeout);
INT16U GSM_socket_getAcceptDataMax(USART_PORT *uart, INT32U timeout);

INT8U  GSM_tcpip_setMuxNum(USART_PORT *uart, INT32S num);
INT8U  GSM_tcpip_setApnUsrPwd(USART_PORT *uart, char* apn, char* userName, char* password);
INT8U  GSM_tcpip_setSendDataPrompt(USART_PORT *uart, int prompt);
INT8U  GSM_tcpip_setSendDataMode(USART_PORT *uart, int mode);
INT8U  GSM_tcpip_setIPHead(USART_PORT *uart, int IP_head);

INT8U  GSM_ftp_setPort(USART_PORT *uart, INT16U port);
INT8U  GSM_ftp_setMode(USART_PORT *uart, INT16U mode);
INT8U  GSM_ftp_setType(USART_PORT *uart, char* type);
INT8U  GSM_ftp_setInputType(USART_PORT *uart, char* type);
INT8U  GSM_ftp_setCid(USART_PORT *uart, int cid);
INT8U  GSM_ftp_setServer(USART_PORT *uart, char* server);
INT8U  GSM_ftp_setUserName(USART_PORT *uart, char* userName);
INT8U  GSM_ftp_setPassword(USART_PORT *uart, char* password);
INT8U  GSM_ftp_setGetFileName(USART_PORT *uart, char* fileName);
INT8U  GSM_ftp_setGetFilePath(USART_PORT *uart, char* filePath);
INT8U  GSM_ftp_setPutFileName(USART_PORT *uart, char* fileName);
INT8U  GSM_ftp_setPutFilePath(USART_PORT *uart, char* filePath);
INT8U  GSM_ftp_putOpen(USART_PORT *uart, int *pError, int *pMaxLen);
INT8U  GSM_ftp_putData(USART_PORT *uart, INT8U *reqBuf, int reqLen, int *pConfLen);
INT8U  GSM_ftp_getOpen(USART_PORT *uart, char* pError);
INT8U  GSM_ftp_getData(USART_PORT *uart, INT8U* reqBuf, int reqLen, int* pConfLen);
INT8U  GSM_ftp_getState(USART_PORT *uart, int *pState);


#endif
