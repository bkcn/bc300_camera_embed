/****************************************Copyright (c)****************************************************
**                                      
**                                 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               modem.c
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "modem.h"

#undef  memcpy
#define memcpy  Mem_Copy

const INT8U GSM_socket_sendOver[2] = {0x1A,'\0'}; 	


void AT_sendCmd(USART_PORT *uart, char* AT_cmd, INT32U retryNum)
{
    INT8U  i;
	
    for(i=0; i<retryNum; i++)	    // .
    {
		USART_putstr(uart, AT_cmd);
	}
}

/****************************************************************
 ** fuction:      AT_sendCmd_and_chkAck() 
 ** description:  add "\r\n" auto at the end of AT command. 
 **               unit(time out): 100mS.
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 ** example: AT_sendCmd_and_chkAck(uart0, AT_IMEI_REQ, "OK", 3, 10);
 *****************************************************************/
INT8U AT_sendCmd_and_chkAck(USART_PORT *uart, char* AT_cmd, char* AT_ackTail, INT32U retryNum, INT32U timeout)
{
	ATE_response result = ATE_NO_RESPONSE;     //
    INT8U  i, j;

	USART_rx_strModeSet(uart, true);
    USART_rx_strIdSet(uart, AT_ackTail);       // .
    USART_rx_bufRst(uart);	
    USART_rx_enable(uart, true);	
	
    for(i=0; i<retryNum; i++)	               // .
    {		
		USART_rx_hwClr(uart);
		USART_putstr(uart, AT_cmd);
		USART_putstr(uart, "\r\n");

		for(j=0; j<timeout; j++)
		{
			if( USART_rx_strIdIsVld(uart) )        // .
			{	
				result = ATE_SUCCESS;  
				return result;
			}	
			else
			{
				#ifdef OS_UCOSII_USED			
				OSTimeDlyHMSM(0, 0, 0, 100);      // 100mS.   
				#else
				delay_ms(10); 
				#endif			
			}
		}
	}
	
	return result;	           // fail.
}

INT8U AT_sendCmd_and_chkAck_multi(USART_PORT *uart, char* AT_cmd, char* AT_ackTail, INT32U retryNum, INT32U timeout)
{
    INT8U  i;
	INT8U  *pStr1;
	ATE_response result = ATE_NO_RESPONSE;    // .
	
    USART_rx_strIdSet(uart, AT_ackTail);       // .
//  USART_rx_strIdSet("\r\n");
//  USART_rx_bufRst(uart);	
//  USART_rx_enable(uart, true);	
	
    for(i=0; i<retryNum; i++)	              // .
    {		
        USART_rx_bufRst(uart);	
		USART_rx_hwClr(uart);
        USART_rx_enable(uart, true);	

		USART_putstr(uart, AT_cmd);
		USART_putstr(uart, "\r\n");

		result = AT_chkAck(uart, timeout);    // .         
			
		if(result != ATE_NO_RESPONSE)         // time out or error. 
		{				
			break;                            // .
		}			
	}

	return result;	                          // .
}

INT8U AT_chkAck(USART_PORT *uart, INT32U timeout)
{
	ATE_response result = ATE_NO_RESPONSE;               //
	INT32U i;
	
	for(i=0; i<timeout; i++)
	{	
		if( USART_rx_strIdIsVld(uart) )                      // .
		{					
			if(strstr((char*)(uart->rx_buffer ), "+IPD"))
			{
				result = ATE_RECEIVE_READY;    
			}
			else if(strstr((char*)(uart->rx_buffer ), "SEND OK") ||     // CIPQSEND=0.
				   (strstr((char*)(uart->rx_buffer ), "DATA ACCEPT")))  // CIPQSEND=1.
			{
				result = ATE_SEND_OK;
			}
			else if((strstr((char*)(uart->rx_buffer ), "OK")) || 
					(strstr((char*)(uart->rx_buffer ), "CONNECT")))
			{
				result = ATE_SUCCESS;
			}
			else if((strstr((char*)(uart->rx_buffer ), "ERROR")) ||
					(strstr((char*)(uart->rx_buffer ), "NO DIALTONE")))       
			{			
				result = ATE_ERROR;
			}
			else if(strstr((char*)(uart->rx_buffer ), "BUSY"))
			{
				result = ATE_BUSY;
			}
			else if(strstr((char*)(uart->rx_buffer ), ">"))
			{
				result = ATE_SEND_READY;           
			}
			else
			{
				result = ATE_NO_RESPONSE;
			}
			
		    USART_rx_bufRst(uart);               // reset the receive buffer.
			USART_rx_enable(uart, true);         // enable UART0's receive.	

            break;                                // maybe correct or fail. 			
		}	
		else
		{
			#ifdef OS_UCOSII_USED			
			OSTimeDlyHMSM(0, 0, 0, 100);           // 10mS.   
			#else
			delay_ms(10); 
			#endif			
		}
	}
	
	return result;	                       // fail.
}

char* AT_ChkAckStr(USART_PORT *uart, char* str, INT32U timeout)
{
//	ATE_response result = ATE_NO_RESPONSE;            //
	INT32U i;
	char *pStr1;

    for(i=0; i<timeout; i++)
    { 	
		pStr1 = strstr((char*)(uart->rx_buffer ), str);
		if(pStr1)                                         // . 
		{
			return pStr1;  
		}
		else
		{
			#ifdef OS_UCOSII_USED			
			OSTimeDlyHMSM(0, 0, 0, 10);     // 10mS.   
			#else
			delay_ms(10); 
			#endif			
		}
    }
	
	return 0;	                            // fail.
}

INT8U GSM_getIMEI(USART_PORT *uart, char *imei, INT8U strlen)
{
    ATE_response result = ATE_NO_RESPONSE;
	
//  char str[20];
	char *cp = 0;
    char *cp1;
    INT8U i;
//	INT8U ucTmpVal = 0;
	
    result = AT_sendCmd_and_chkAck(uart, AT_IMEI_REQ, "OK", 2, AT_TIMEOUT_OS_2S);

    if(result == ATE_SUCCESS)
    {
//      cp = (char*)strstr((void*)uart->rx_buffer, "86");
//      cp = (char*)strstr((void*)uart->rx_buffer, "+GSN");    // must echo on.

		cp = uart->rx_buffer;
//		if(cp)
		{
			i = 0;
            while(((INT8U)*cp < '0') || ((INT8U)*cp > '9') )
			{ 
                cp++;
				i++;
				if(i >= 20)                      // .
				{
//					result = ATE_ERROR;
                    return ATE_ERROR;
				}
			}	

			cp1 = (char*)strstr((void*)cp, "\r\n");
			if(cp1)
			{
				for(i=0; i<strlen; i++)
				{
					if(cp1 != cp)
					{
					    imei[i] = *cp;
						cp++;
					}
					else
					{
						break; 
					}
				}	
				imei[i] = '\0';                // 
				result = ATE_SUCCESS;          //  
			}
		}   				
    }

    return result;
}

INT8U GSM_getCSQ(USART_PORT *uart, INT16U retryNum)
{	
    ATE_response result = ATE_NO_RESPONSE;

//  char str[20];
	char *cp = 0;
    char *cp1;
    INT8U i;
    INT8U j;
	INT8U ucTmpVal = 0;
	
	for(i=0; i<retryNum; i++)
	{
		result = AT_sendCmd_and_chkAck(uart, AT_CSQ_REQ, "OK", 1, AT_TIMEOUT_OS_2S);
		if(result == ATE_SUCCESS)
		{
	//		result = ATE_ERROR;
			cp = (char*)strstr((void*)uart->rx_buffer, "+CSQ: ");

			if(cp)
			{
				j = 0;
				while(((INT8U)*cp < '0') || ((INT8U)*cp > '9'))
				{ 
					cp++;
					j++;
					if(j >= 10)                      // .
					{
						continue;                    // retry again.  
					}
				}	

				cp1 = (char *)strstr((void*)cp, ",");
				if(cp1)
				{
					for(j=0; j<3; j++)
					{
						if(cp1 != cp)             //
						{
							ucTmpVal *= 10;
							ucTmpVal += (*cp - '0');
							cp++;
						}
					}
					if(ucTmpVal < GSM_CSQ_LEVEL_MIN)      // 
					{
						result = ATE_ERROR;
					}
					else
					{
						result = ATE_SUCCESS;
						break;
					}
				} 
			}
		}
	}		
	
    return result;
}

INT8U GSM_getSimCardID(USART_PORT *uart, INT8U *simno, INT8U bufLen)
{
    ATE_response result = ATE_NO_RESPONSE;
	
//  char str[20];
	char *cp = 0;
    char *cp1;
    INT8U i;
//	INT8U ucTmpVal = 0;

    result = AT_sendCmd_and_chkAck(uart, AT_SIM_CARD_ID_REQ, "OK", 2, AT_TIMEOUT_OS_2S);
    if(result == ATE_SUCCESS)
    {
		result = ATE_ERROR;

//      cp = (char*)strstr((void*)uart->rx_buffer, "8986");
        cp = (char*)strstr((void*)uart->rx_buffer, "+CCID");    // 2 * \r.
        if(cp)
		{
			i = 0;
            while(((INT8U)*cp < '0') || ((INT8U)*cp > '9') )
			{ 
                cp++;
				i++;
				if(i >= 20)                      // .
				{
//					result = ATE_ERROR;       
                    return ATE_ERROR;            // . 
				}
			}	
	
			cp1 = (char*)strstr((void*)cp, "\r\n");
			if(cp1)
			{
				for(i=0; i<bufLen; i++)
				{
					if(cp1 != cp)             //
					{
						simno[i] = *cp;
						cp++;
					}
					else
					{
						break;
					}
				}
				simno[i] = '\0';	
				result = ATE_SUCCESS;
			}
		}
	}	
	   
    return result;
}

/* Get current clock from GSM network */
INT8U GSM_getCurrTime(USART_PORT *uart, struct tm *pCurrTime)
{
    ATE_response result = ATE_NO_RESPONSE;
	
//  char str[20];
	char *cp = 0;
//  char *cp1;
//  INT8U i;
//	INT8U ucTmpVal = 0;

    result = AT_sendCmd_and_chkAck(uart, AT_CLOCK_REQ, "OK", 2, AT_TIMEOUT_OS_5S);
    if(result == ATE_SUCCESS)
    {
        cp = (char*)strstr((void*)uart->rx_buffer, "+CCLK: \"");  // .
		if(cp)
		{
            cp += strlen("+CCLK: \"");
            sscanf(cp, "%d/%d/%d,%d:%d:%d+",     // "00/01/01,01:44:07+00".    
						&(pCurrTime->tm_year),
						&(pCurrTime->tm_mon),
						&(pCurrTime->tm_mday),
						&(pCurrTime->tm_hour),
						&(pCurrTime->tm_min),
						&(pCurrTime->tm_sec));	
		}
	}
		    
    return result;
}

/*Get single socket current max avaialbe sending data number*/
//int iGetCurSockMaxAcceptData(SD_PORT *com)
INT16U GSM_socket_getAcceptDataMax(USART_PORT *uart, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;
	
	char   *cp = 0;
    char   *cp1;
    INT8U  i;
	INT32U ulTmpVal = 0;

    char pStr[32];
    memset(pStr, '\0', sizeof(pStr));	
	sprintf(pStr, "%s?", AT_SOCKET_SEND_DATA);
	
    result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, timeout);
    if(result == ATE_SUCCESS)
    {
        cp = (char*)strstr((void*)uart->rx_buffer, "+CIPSEND: ");                    
	    if(cp)
	    {
			cp = cp + strlen("+CIPSEND: ");
			i = 0;
            while(((INT8U)*cp < '0') || ((INT8U)*cp > '9') )
			{ 
                cp++;
				i++;
				if(i >= 20)                      // .
				{
//					result = ATE_ERROR;
                    return 0;
				}
			}	
	
			cp1 = (char *)strstr((void*)cp, "\r\n");
			if(cp1)
			{
				for(i=0; i<8; i++)
				{
					if(cp1 != cp)             //
					{
						ulTmpVal *= 10;
						ulTmpVal += (*cp - '0');
						cp++;
					}
					else
					{
						break;
					}
//					result = ATE_SUCCESS;
				}
			} 
        }
	}
	
    if(ulTmpVal >= 0xffff)
	{
        return 0xffff;             // 64kB.		
	}
	else
	{
        return (INT16U)ulTmpVal;   // 64kB.		
	}
}

INT8U GSM_getStatGPRSattach(USART_PORT *uart)
{
    ATE_response result = ATE_NO_RESPONSE;
	char  *cp;
    INT8U ucTmpVal;
	
    char pStr[32];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s?", AT_GPRS_ATTACH);     // GPRS attached ?.

	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, AT_TIMEOUT_OS_2S);
	if(result == ATE_SUCCESS)
	{
		cp = (char*)strstr((void*)uart->rx_buffer, "+CGATT: ");    
		if(cp)
		{
			cp += strlen("+CGATT: ");       
			ucTmpVal = (*cp - '0');
			if(ucTmpVal == 1)
			{
				result == ATE_SUCCESS;
			}
			else if(ucTmpVal == 0)
			{
				result == ATE_GPRS_DETACHED;
			}
			else
			{
				result == ATE_ERROR;					
			}
		}						
	}	
	
	return result;
}


/* attach GPRS from PPP modem driver :  Attach or Detach---*/
INT8U GSM_GPRS_attach(USART_PORT *uart, bool enable, INT16U retryNum)
{
    ATE_response result  = ATE_NO_RESPONSE;
	ATE_response result1 = ATE_NO_RESPONSE;
	INT8U  i;

    char pStr[32];
    memset(pStr, 0, sizeof(pStr));  
	
    if(enable)
	{
		for(i=0; i<retryNum; i++)
		{
			result1 = GSM_getStatGPRSattach(uart);
			if(result1 == ATE_SUCCESS)
			{
				result = ATE_SUCCESS;
				return result;
			}
			else
			{
				sprintf(pStr, "%s=%d", AT_GPRS_ATTACH, (INT16U)1);  // attach the GPRS.
	            AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, AT_TIMEOUT_OS_2S);   //				
			}
		}
		result = ATE_ERROR;
	}
    else        
	{
		for(i=0; i<retryNum; i++)
		{
			result1 = GSM_getStatGPRSattach(uart);
			if(result1 == ATE_GPRS_DETACHED)
			{
				result = ATE_SUCCESS;
				return result;
			}
			else
			{
				sprintf(pStr, "%s=%d", AT_GPRS_ATTACH, (INT16U)0);             // detach the GPRS.
	            AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, AT_TIMEOUT_OS_2S);  //				
			}
		}
		result = ATE_ERROR;
	}

	return result;
}

/* Define PDP context from PPP modem driver */
INT8U GSM_definePDPcontext(USART_PORT *uart, char *ApnName)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[32];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s=1,\"IP\",\"%s\"", AT_DEFINE_PDP_CONTEXT, ApnName);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/*Set TCPIP Application Toolkit MUX number*/
//INT8U ucSetTCPIPMuxNum(SD_PORT *com, int Number)
INT8U GSM_tcpip_setMuxNum(USART_PORT *uart, INT32S num)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[32];
    memset(pStr, 0, sizeof(pStr));  

    if(num == 1)
	{
		sprintf(pStr, "%s=%d", AT_TCPIP_SET_MUX_NUM, (INT16U)1);
	}
    else        
	{
        sprintf(pStr, "%s=%d", AT_TCPIP_SET_MUX_NUM, (INT16U)0);  // dismiss from the GPRS.
	}

	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/*Set TCPIP APN, username, password*/
//INT8U ucSetTCPIPApnUsrPwd(SD_PORT * com, char* Apn, char* UserName, char* Password)
INT8U GSM_tcpip_setApnUsrPwd(USART_PORT *uart, char* Apn, char* UserName, char* Password)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s=\"%s\",\"%s\",\"%s\"", AT_TCPIP_SET_APN_USR_PSW, Apn, UserName, Password);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/* set Active GPRS */
//INT8U ucActiveGPRS(SD_PORT * com)
INT8U GSM_GPRS_active(USART_PORT *uart)
{
    ATE_response result = ATE_NO_RESPONSE;

//  char pStr[48];
//  memset(pStr, 0, sizeof(pStr));  

//  sprintf(pStr, "%s\r", AT_GPRS_ACTIVE);
	result = AT_sendCmd_and_chkAck(uart, AT_GPRS_ACTIVE, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/* net registered ? */
INT8U GSM_getStatNetReg(USART_PORT *uart, INT16U retryNum)
{
    ATE_response result = ATE_NO_RESPONSE;
	INT8U i;
	INT8U ucTmpVal = 0;
	char  *cp;
//	char  *cp1;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s?", AT_NETREG_REQ);
	for(i=0; i<retryNum; i++)
	{
	    result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, AT_TIMEOUT_OS_2S);
		if(result == ATE_SUCCESS)
		{
            cp = (char*)strstr((void*)uart->rx_buffer, ",");    
            if(cp)
			{
                cp += 1;
				ucTmpVal = (*cp - '0');
				if((ucTmpVal == 1) || (ucTmpVal == 5))
				{
					result == ATE_SUCCESS;
					break;
				}
				else
				{
					result == ATE_ERROR;					
				}
			}						
		}
	}
	
	return result;
}




/*Query Socket status: Only for one socket, client*/
//INT8U ucQuerySocketStatus(SD_PORT * com)
INT8U GSM_socket_getStatus(USART_PORT *uart, INT32U timeout)
{
    SOCKET_state state = SOCKET_STAT_UNKNOWN;     // INT8U.
	INT8U  i;

//  char pStr[64];
//  memset(pStr, 0, sizeof(pStr));  

    USART_rx_strModeSet(uart, false);	

    USART_rx_bufRst(uart);	
    USART_rx_enable(uart, true);		
	USART_rx_hwClr(uart);
	USART_putstr(uart, AT_SOCKET_GET_STATUS);
	USART_putstr(uart, "\r\n");
	
	for(i=0; i<timeout; i++)
	{
//			OSTimeDly(DELAY_OS_1S / 10);                  // 100/10 = 10. 
		
		#ifdef OS_UCOSII_USED			
		OSTimeDlyHMSM(0, 0, 0, 100);                  // 100mS.   
		#else
		delay_ms(10); 
		#endif						

		if (strstr(uart->rx_buffer,      "IP INITIAL"))    // IP INITIAL.
		{
			state = SOCKET_STAT_IPINITIAL;                   /*IP INITIAL*/
		}
		else if (strstr(uart->rx_buffer, "IP START"))
		{
			state = SOCKET_STAT_IPSTART;
		}
		else if (strstr(uart->rx_buffer, "IP CONFIG"))
		{
			state = SOCKET_STAT_IPCONFIG;
		}
		else if (strstr(uart->rx_buffer, "IP GPRSACT"))
		{
			state = SOCKET_STAT_IPGPRSACT;
		}
		else if (strstr(uart->rx_buffer, "IP STATUS"))
		{
			state = SOCKET_STAT_IPSTATUS;
		}
		else if (strstr(uart->rx_buffer, "CONNECT OK"))
		{
			state = SOCKET_STAT_CONNECTED;
		}
		else if (strstr(uart->rx_buffer, "CONNECTING"))
		{
			state = SOCKET_STAT_CONNECTING;
		}
		else
		{
			state = SOCKET_STAT_UNKNOWN;
		}
		
		if(state != SOCKET_STAT_UNKNOWN)
		{
			break;
		}
    }
	
    USART_rx_strModeSet(uart, true);	
    return state;
}
	
/* open socket : single socket */
//INT8U ucOpenSocket(SD_PORT * com, char* Mode, char*IPDomain, int16_t peerPort)
INT8U GSM_socket_open(USART_PORT *uart, char* mode, char* IP_domain, INT16U peerPort, INT16U retryNum, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;
    INT8U  i;
    INT8U  j;
	INT16U uiTmpVal;
	char   *cp;
	char   *cp1;
	
    char pStr[80];
    memset(pStr, 0, sizeof(pStr));  
	
    sprintf(pStr, "%s=\"%s\",\"%s\",%d", AT_SOCKET_OPEN, mode, IP_domain, peerPort);

    for(i=0; i<retryNum; i++)
	{
		USART_rx_strModeSet(uart, false);	
		USART_rx_bufRst(uart);	
		USART_rx_enable(uart, true);		
		USART_rx_hwClr(uart);
		USART_putstr(uart, pStr);
		USART_putstr(uart, "\r\n");
		
		for(j=0; j<timeout; j++)
		{
			#ifdef OS_UCOSII_USED			
			OSTimeDlyHMSM(0, 0, 0, 100);                  // 100mS.   
//			OSTimeDly(DELAY_OS_1S / 10);                  // 100/10 = 10. 		
			#else
			delay_ms(10); 
			#endif						

//			AT+CIPSTART="TCP","121.41.25.64 ",8080<CR>
//          <LF>			
//          OK<CR>
//          <LF>			
//          STATE: IP STATUS<CR>
//          <LF>			
//          CONNECT FAIL<CR>
//          <LF>	

			cp = strstr(uart->rx_buffer, "ALREADY CONNECT");
			if(cp)
			{
                USART_rx_strModeSet(uart, true);	
//			    result = ATE_SUCCESS;
//				return result;
				return ATE_SUCCESS;
			}			

			cp = strstr(uart->rx_buffer, "CONNECT");
			if(cp)
			{
				if(strstr((void *)cp, "OK"))
				{
                    USART_rx_strModeSet(uart, true);	
//					result = ATE_SUCCESS;
//					return result;
					return ATE_SUCCESS;
				}
				if(strstr((void *)cp, "CLOSED"))
				{
					result = ATE_TCPCLOSED;
					break;
				}
				else
				{
					result = ATE_ERROR;
					break;
				}
			}	
		}
	}
	
    USART_rx_strModeSet(uart, true);	
    return result;	
}

/* send data, and receive data:  single socket, client, "> ", "SEND OK"*/
INT8U GSM_socket_sendData(USART_PORT *uart, INT8U* dataBuf, INT16U dataLen, INT16U retryNum, INT32U timeout)
{
    ATE_response result = ATE_ERROR;
    INT8U  i;
    INT8U  j;
//	INT16U uiTmpVal = 0;
    char pStr[80];
//	char *cp;
//	char *cp1;

/*	
    result = GSM_setEcho(uart0, false);            // disable the echo.  
	{
        if(result != ATE_SUCCESS)
		{
//			return ATE_ERROR;  
		}
	}		
*/
	
#ifdef USE_ACCEPT
    INT16U time_out = timeout;
#else
    INT16U time_out = timeout << 3;    // *8.
#endif

    memset(pStr, 0, sizeof(pStr));  
    sprintf(pStr, "%s=%d", AT_SOCKET_SEND_DATA, (INT16U)dataLen);   // AT+CIPSEND=1.
//  sprintf(pStr, "%s", AT_SOCKET_SEND_DATA, (INT16U)dataLen);      // AT+CIPSEND
	
    for(i=0; i<retryNum; i++)
	{
		if(result == ATE_SEND_READY)
		{
			break;
		}
		
		USART_rx_enable(uart, false);		
		USART_rx_strModeSet(uart, true);	
//		USART_rx_strIdSet(uart, "\r\n");
		USART_rx_strIdSet(uart, "> ");
		USART_rx_bufRst(uart);	
//	    USART_rx_charMoreNumSet(uart, 1);         // "DATA ACCEPT: > ".
	    USART_rx_charMoreNumSet(uart, 0);         // "DATA ACCEPT: > ".
		USART_rx_enable(uart, true);		
		USART_rx_hwClr(uart);
		USART_putstr(uart, pStr);
		USART_putstr(uart, "\r\n");
		
		for(j=0; j<timeout; j++)
		{
//			AT+CIPSEND<CR>    // AT+CIPSEND=902<CR>
//          <LF>			
//          >

			if( USART_rx_strIdIsVld(uart) )
			{
				result = ATE_SEND_READY;
			    break;                     // .
/*
				cp  = strstr(uart->rx_buffer, "\r\n");
				cp1 = cp + 2;
				cp  = strstr((void *)cp1, ">");				
				if(cp)
				{
					result = ATE_SEND_READY;
				    break;                     // retry again.
				}	
*/				
			}
			else                               // retry again.
			{
				result = ATE_ERROR;
				
				#ifdef OS_UCOSII_USED			
				OSTimeDlyHMSM(0, 0, 0, 100);                  // 100mS.   
				#else
				delay_ms(10); 
				#endif						
			}			
		}
	}
	
	if(result == ATE_SEND_READY)
	{
//		USART_rx_strIdSet(uart, "\r\n");
		USART_rx_strIdSet(uart, "T:");                // "DATA ACCEPT:"  .
		USART_rx_bufRst(uart);	
		USART_rx_enable(uart, true);		
		USART_rx_hwClr(uart);
/*		
		if(dataLen > 1000)
		{
		    USART_rx_charMoreNumSet(uart, 5);         // "DATA ACCEPT:"  .
	    }
		else if(dataLen > 100)
		{
		    USART_rx_charMoreNumSet(uart, 4);         // "DATA ACCEPT:"  .
	    }
		else if(dataLen > 10)
		{
		    USART_rx_charMoreNumSet(uart, 3);         // "DATA ACCEPT:"  .
	    }
		else 
		{
		    USART_rx_charMoreNumSet(uart, 2);         // "DATA ACCEPT:"  .
	    }		
*/		
	    USART_rx_charMoreNumSet(uart, 0);         // "DATA ACCEPT" only .
        USART_rx_strModeSet(uart, true);	      //		

		// vInitPort(com);                        /* Initalize the port */        
        USART_putbinary(uart, dataBuf, dataLen);  /* Send out data */ 
        USART_putstr(uart, (char *)GSM_socket_sendOver);  // ctrl_Z added.		
		
		result = AT_chkAck(uart, timeout);        //			
//		USART_rx_enable(uart, false);             // disable UART0's receive.	

        if(result == ATE_SEND_OK)	
		{
/*			
			cp = strstr(uart->rx_buffer, "DATA ACCEPT:");				
			cp = cp + strlen("DATA ACCEPT:");
			cp1 = (char *)strstr((void*)cp, "\r");
			if(cp1)
			{
				for(i=0; i<4; i++)
				{
					if(cp1 != cp)             //
					{
						uiTmpVal *= 10;
						uiTmpVal += (*cp - '0');
						cp++;
					}
				}
				if(dataLen = uiTmpVal)
				{
					result == ATE_SEND_OK;
				}
				else
				{
					result == ATE_ERROR;
				}
			}
*/			
		}			
		else
		{
			result = ATE_ERROR;
		}
/*		
		USART_rx_enable(uart, true);          // enable UART0's receive.	
*/		
	}
	
//  GSM_setEcho(uart0, true);                 // enable the echo.    
    return result;
}



/* set send data prompt */
/*
 *  0: No ">" indication,  "SEND OK"  return.
 *  1: ">" indication,     "SEND OK" return.
 *  2: no ">" indicaion,   no "SEND OK" return.
 */
//INT8U ucSetSendDataPrompt(SD_PORT * com, int  prompt)
INT8U GSM_tcpip_setSendDataPrompt(USART_PORT *uart, int prompt)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  
	
    if(prompt > 2)
	{
        prompt = 1;  /* Set default prompt */
	}
	
    sprintf(pStr, "%s=%d", AT_TCPIP_SET_SEND_PROMPT, prompt);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);
	
	return result;
}

/*Set send data mode*/
/*
 *  0:  "SEND OK"  return
 *  1:  "ACCEPT,<n>,<length>" return
 *   
 */
//INT8U ucSetSendDataMode(SD_PORT * com, int  Mode)
INT8U GSM_tcpip_setSendDataMode(USART_PORT *uart, int mode)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr)); 
	
    if(mode > 2)
	{
        mode = 1;      /* Set default mode */
	}
	
    sprintf(pStr, "%s=%d", AT_TCPIP_SET_SEND_MODE, mode);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/*Set receive data IP head: For +CIPMUX=0*/
//INT8U ucSetIPHead(SD_PORT * com, int  IPHead)
INT8U GSM_tcpip_setIPHead(USART_PORT *uart, int IP_head)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  
	
    if(IP_head == 1)
	{
        sprintf(pStr, "%s=%d", AT_TCPIP_SET_RCV_IP_HEAD, 1);
	}
	else 
	{
        sprintf(pStr, "%s=%d", AT_TCPIP_SET_RCV_IP_HEAD, 0);
	}
	
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);
	return result;
}

/*Get GPRS local IP address*/
//INT8U ucGetLocalIPAddr(SD_PORT * com)
INT8U GSM_getLocalIPAddr(USART_PORT *uart, char *IP_Local, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;

	INT8U i;
	char *cp;
	char *cp1;
	
//  char pStr[64];
//  memset(pStr, 0, sizeof(pStr));  
//  sprintf(pStr, "%s", AT_IP_ADDR_REQ);	

    USART_rx_strModeSet(uart, false);	

    USART_rx_bufRst(uart);	
    USART_rx_enable(uart, true);		
	USART_rx_hwClr(uart);
	USART_putstr(uart, AT_IP_ADDR_REQ);
	USART_putstr(uart, "\r\n");
		
	for(i=0; i<timeout; i++)
	{		
		#ifdef OS_UCOSII_USED			
		OSTimeDlyHMSM(0, 0, 0, 100);                  // 100mS.   
		#else
		delay_ms(100); 
		#endif			

		//	STATE: IP STATUS
		//  AT+CIFSR<CR>
		//  <LF>
		//  10.72.7.22<CR>
		//  <LF>
		
		cp = strstr(uart->rx_buffer, "\r\n");
		if(cp)
		{
			cp += 2; 			
		    if (strstr((void *)cp, "."))
			{
			    cp1 = strstr((void *)cp, "\r");
                if(cp1)
				{
					strncpy(IP_Local, cp, (cp1-cp));  
					IP_Local[cp1-cp] = '\0';

					result = ATE_SUCCESS; 
				}
			}
		}		
    }
	
    USART_rx_strModeSet(uart, true);	
    return result;
}

/*Enable or disable Echo*/
//INT8U ucEchoEnable(SD_PORT * com,int Enable)
INT8U GSM_setEcho(USART_PORT *uart, int enable)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[32];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s%d", AT_ECHO_ENABLE, enable);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, AT_TIMEOUT_OS_2S);   // 20 * 100mS = 2S.

	return result;
}

/* Close socket: Fast close */
//INT8U ucCloseSocket(SD_PORT * com)
INT8U GSM_socket_close(USART_PORT *uart, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;

    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  

    sprintf(pStr, "%s=1", AT_SOCKET_CLOSE);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 1, timeout);

	return result;
}

/* GPRS shutdown */
//INT8U ucShutdownGPRS(SD_PORT * com)
INT8U GSM_socket_shutdown(USART_PORT *uart, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;

//  char pStr[48];
//  memset(pStr, 0, sizeof(pStr));  

//  sprintf(pStr, "%s", AT_SOCKET_SHUTDOWN);
	result = AT_sendCmd_and_chkAck(uart, AT_SOCKET_SHUTDOWN, "OK", 2, timeout);

	return result;
}

/* socket receive data */
// INT8U ucGetSocketData(SD_PORT * com, INT8U* buf, int bufLen, int*gotLen, int32_t Wait_ms)
INT8U GSM_socket_getData(USART_PORT *uart, INT8U *buf, INT16U bufLen, INT16U *gotLen, INT32U timeout)
{
    ATE_response result = ATE_NO_RESPONSE;
    int resolvedlen = 0;

//  char pStr[48];
//  memset(pStr, 0, sizeof(pStr));  
 
    result = AT_chkAck(uart, timeout);

    if(result == ATE_RECEIVE_READY)
    {           
        OSTimeDly(DELAY_OS_1S / 10);                            /* wait entire data ready */        
        resolvedlen = atoi(strstr(uart->rx_buffer, "+IPD")+5);  /* resolve data length */

        if(resolvedlen > bufLen)
		{
            resolvedlen = bufLen;
		}
        memcpy(buf, strstr(strstr(uart->rx_buffer,"+IPD"), ":")+1, resolvedlen);
        *gotLen = resolvedlen;
    }
	
    return result;
}


/*Set IP application */
//INT8U ucSetIPApplication(SD_PORT * com, int cmd_type,int cid,char* ConParamTag,char*ConParamValue)
INT8U GSM_setIPApplication(USART_PORT *uart, int cmd_type, int cid, char* ConParamTag, char* ConParamValue)
{
    ATE_response result = ATE_NO_RESPONSE;
    int timeout = 0;
	
    char pStr[48];
    memset(pStr, 0, sizeof(pStr));  	

	if(NULL == ConParamTag)
    {
        sprintf(pStr, "%s=%d,%d", AT_IP_APP_SETTING, cmd_type, cid);
        if(cmd_type == 0)
        {
            timeout = AT_TIMEOUT_OS_2S;
        }
        else
        {
            timeout = AT_TIMEOUT_OS_5S;
        }        
    }
    else
    {
        sprintf(pStr, "%s=%d,%d,\"%s\",\"%s\"", AT_IP_APP_SETTING, cmd_type, cid, ConParamTag, ConParamValue);
        timeout = AT_TIMEOUT_OS_5S;
    }
	
	result = AT_sendCmd_and_chkAck(uart, pStr, "\r\n", 2, timeout);   // .  
    return result;
        
}

/* Set FTP port: Default 21 */
//INT8U ucSetFTPPort(SD_PORT * com, int16_t Port)
INT8U GSM_ftp_setPort(USART_PORT *uart, INT16U Port)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=%d", AT_FTP_SET_PORT, (INT16U)Port);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);  // .  
    return result;
}

/*Set FTP mode: 0, 1*/
//INT8U ucSetFTPMode(SD_PORT * com,int Mode)
INT8U GSM_ftp_setMode(USART_PORT *uart, INT16U Mode)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=%d", AT_FTP_SET_MODE, (INT16U)Mode);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);  // .  
    return result;
}

/* Set FTP Type: "A", "I" */
//INT8U ucSetFTPType(SD_PORT * com,char * Type)
INT8U GSM_ftp_setType(USART_PORT *uart, char* Type)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_TYPE, Type);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);  // .  
    return result;
}

/* Set FTP Input Type: "APPE", "STOU", "STOR" */
//INT8U ucSetFTPInputType(SD_PORT * com,char * Type)
INT8U GSM_ftp_setInputType(USART_PORT *uart, char* Type)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_INPUTOPT, Type);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);  // .  
    return result;
}

/*Set FTP cid*/
//INT8U ucSetFTPCid(SD_PORT * com,int cid)
INT8U GSM_ftp_setCid(USART_PORT *uart, int cid)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=%d", AT_FTP_SET_CID, (INT32U)cid);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_5S);  // .  
    return result;
}

/* Set FTP Server Name */
//INT8U ucSetFTPServer(SD_PORT * com, char * Server)
INT8U GSM_ftp_setServer(USART_PORT *uart, char* Server)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_SERVER, Server);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/*Set FTP User Name*/
//INT8U ucSetFTPUserName(SD_PORT * com,char * UserName)
INT8U GSM_ftp_setUserName(USART_PORT *uart, char* UserName)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_USERNAME, UserName);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/*Set FTP Pass Word*/
//INT8U ucSetFTPPassWord(SD_PORT * com,char * PassWord)
INT8U GSM_ftp_setPassword(USART_PORT *uart, char* PassWord)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_PASSWORD, PassWord);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/*Set FTP Get File Name */
//INT8U ucSetFTPGetFileName(SD_PORT * com,char * fileName)
INT8U GSM_ftp_setGetFileName(USART_PORT *uart, char* fileName)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr,"%s=\"%s\"", AT_FTP_SET_GETNAME, fileName);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/* Set FTP Get File Path */
//INT8U ucSetFTPGetFilePath(SD_PORT * com,char * filePath)
INT8U GSM_ftp_setGetFilePath(USART_PORT *uart, char* filePath)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_GETPATH, filePath);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/* Set FTP Output file path */
//INT8U ucSetFTPPutFileName(SD_PORT * com,char * fileName)
INT8U GSM_ftp_setPutFileName(USART_PORT *uart, char* fileName)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_PUTNAME, fileName);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/* Set FTP Put File Path */
//INT8U ucSetFTPPutFilePath(SD_PORT * com,char * filePath)
INT8U GSM_ftp_setPutFilePath(USART_PORT *uart, char* filePath)
{
    ATE_response result = ATE_NO_RESPONSE;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=\"%s\"", AT_FTP_SET_PUTPATH, filePath);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  
    return result;
}

/*FTP Get Open */
//INT8U ucFTPGetOpen(SD_PORT * com,int *pError)
INT8U GSM_ftp_getOpen(USART_PORT *uart, char* pError)
{
    ATE_response result = ATE_NO_RESPONSE;
    char *pState;
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr));  	

    sprintf(pStr, "%s=%d", AT_FTP_GET, 1);
	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // .  

    if(ATE_SUCCESS == result)
    {
//		USART_rx_bufRst(uart);
		USART_rx_enable(uart, true);
		
		pState = AT_ChkAckStr(uart, "+FTPGET:1,", AT_TIMEOUT_OS_2S);   // 
		if( pState )
		{
			pState += strlen("+FTPGET:1,");
			*pError = (int)atoi(pState);       
		}
	}

    return result;
}

/* FTP Get Data */
//INT8U ucFTPGetData(SD_PORT * com,INT8U * reqBuff, int reqLen, int* pConfLen)
INT8U GSM_ftp_getData(USART_PORT *uart, INT8U* reqBuff, int reqLen, int* pConfLen)
{
    ATE_response result = ATE_NO_RESPONSE;
    char *pState;
    int  CopyLen = 0;  
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr)); 

//  vInitPort(com);     
    sprintf(pStr, "%s=%d,%d", AT_FTP_GET, 2, reqLen);
 	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // . 
//  USART_putstr(uart, pStr);                                            // NOSURE.
	
    if(ATE_SUCCESS == result)
    {
//		USART_rx_bufRst(uart);
		USART_rx_enable(uart, true);

		pState = AT_ChkAckStr(uart, "+FTPGET:2,", AT_TIMEOUT_OS_2S);        // 
		if( pState )
		{
			pState += strlen("+FTPGET:2,");
			CopyLen = atoi(pState);           
			result = ATE_SUCCESS;
		}
		else
		{
			result = ATE_ERROR;
		}
	}
	
    if(CopyLen > 0)
    {
        OSTimeDly(DELAY_OS_1S * 5);               // NOSURE, 5S?.
        pState =strstr(pState,"\r\n");
		
        if(pState)
        {
            pState += strlen("\r\n");
            if(CopyLen > reqLen)
			{
                CopyLen = reqLen;
			}
            memcpy(reqBuff, pState, CopyLen);
            *pConfLen = CopyLen;
        }
    }
    
    return result;
}

/* FTP Put Open */
//INT8U ucFTPPutOpen(SD_PORT *com, int *pError, int* pMaxLen)
INT8U GSM_ftp_putOpen(USART_PORT *uart, int *pError, int *pMaxLen)
{
    ATE_response result = ATE_NO_RESPONSE;
    char* pState;
//  int CopyLen = 0;  
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr)); 

    sprintf(pStr, "%s=%d", AT_FTP_PUT, 1);
 	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // . 
	
    if(ATE_SUCCESS == result)
    {
		USART_rx_enable(uart, true);

		pState = AT_ChkAckStr(uart, "+FTPPUT:1,", AT_TIMEOUT_OS_2S);        // 
		if( pState )
		{
			pState += strlen("+FTPPUT:1,");
            *pError=(int)atoi(pState);

            pState =strstr(pState,",");
            if( pState )
            {
                pState++;
                *pMaxLen = atoi(pState);
            }			
		}
		else
		{
			result = ATE_ERROR;
		}
	}
    return result;
}

/* FTP Put Data */
//INT8U ucFTPPutData(SD_PORT * com,INT8U * reqBuff, int reqLen, int* pConfLen)
INT8U GSM_ftp_putData(USART_PORT *uart, INT8U *reqBuff, int reqLen, int *pConfLen)
{
    ATE_response result = ATE_NO_RESPONSE;
    char*  pState;
	INT8U  i;
	INT32U AllowedReqLen;
//  int CopyLen = 0;  
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr)); 

    sprintf(pStr, "%s=%d,%d", AT_FTP_PUT, 2, reqLen);
 	result = AT_sendCmd_and_chkAck(uart, pStr, "OK", 2, AT_TIMEOUT_OS_2S);  // . 
//  USART_putstr(uart, pStr);                                            // NOSURE.
	
    if(ATE_SUCCESS == result)
    {
		USART_rx_enable(uart, true);
				
		if(reqLen > 0)
		{			
			pState = AT_ChkAckStr(uart, "+FTPPUT:2,", AT_TIMEOUT_OS_2S);        // 
			if( pState )
			{				
			    pState += strlen("+FTPPUT:2,");
			    AllowedReqLen = atoi(pState);
			
				if((AllowedReqLen > 0) && 
				   (AllowedReqLen < reqLen))   /* Adjust the reqest length */
				{
					reqLen = AllowedReqLen;
				}
//			    SDC_Send_Bytes(reqBuff, com, reqLen);
				USART_putbinary(uart, reqBuff, reqLen);  
			}
		}
    }
	
    /* Should not have AT command echo */
	USART_rx_enable(uart, true);

	for(i=0; i<10; i++)                             // NOTSURE.
	{
	    if(0 == AT_ChkAckStr(uart, "ERROR", AT_TIMEOUT_OS_02S))     // 
		{
			result = ATE_ERROR;
		}
	    else if(0 == AT_ChkAckStr(uart, "OK", AT_TIMEOUT_OS_02S))     // 
		{
            *pConfLen = reqLen;
			result = ATE_SUCCESS;
            break;
		}
		else
		{
			#ifdef OS_UCOSII_USED			
			OSTimeDlyHMSM(0, 0, 0, 10);     // 10mS.   
			#else
			delay_ms(10); 
			#endif						
		}
	}
	
    /* wait the max transfer packet, wait ready */
	result = ATE_ERROR;
	
    pState = AT_ChkAckStr(uart, "OK", AT_TIMEOUT_OS_02S);
    if( pState )
    {
		pState = AT_ChkAckStr(uart, "+FTPPUT:1,1,", AT_TIMEOUT_OS_10S);    // NOTSURE.
		if(pState)
		{
			result = ATE_SUCCESS;
		}
	}
    
    return result;
}

/* FTP Get State */
//INT8U ucFTPGetState(SD_PORT * com, int* pState)
INT8U GSM_ftp_getState(USART_PORT *uart, int *pState)
{
    ATE_response result = ATE_NO_RESPONSE;
    char* pState1;
//  int CopyLen = 0;  
	
    char pStr[64];
    memset(pStr, 0, sizeof(pStr)); 

//  sprintf(pStr, "%s", AT_FTP_GET_STATE);
 	result = AT_sendCmd_and_chkAck(uart, AT_FTP_GET_STATE, "OK", 2, AT_TIMEOUT_OS_2S);  // . 
	
    if(ATE_SUCCESS == result)
	{
        pState1 = strstr(uart->rx_buffer, ":");
        if(pState1)
        {
            pState1++;
            *pState = (int)(pState1[0]-'0');
        }
        else
        {
            *pState = 0;
        }
	}	

    return result;
}





