#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "socket.h"
#include "board.h"
//#include "modem.h"


#define  SOCKET_OPEN_TEST       1

volatile INT8U  GSM_dataTransEvt = 0;        //

/* define all GPRS parameters */
/*
static char GPRS_apn[64] = "cmnet";
static char GPRS_userName[64] = "";
static char GPRS_password[64] = "";

char strIMEI[24];

static char   GSM_IP_Local[20];
static char   GSM_IMEI[20];              // 15-BYTES / 17-BYTES.
static INT8U  GSM_CSQ;
static char   GSM_SIM_ID[21];            // 20-BYTES. 
*/


static INT16S currSocketDescription = -1;
static INT16S currProtocol = OS_PROTO_INVALID;    /* Record the protocol */


/* socket create */
// socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
INT16S socket_open(INT16S family, INT16S type, INT16S protocol)
{
    char simCardNo[24];
    INT8U result;

    // possible protocols based on [FAMILY, TYPE].
	INT16S NU_proto_list[][5] =
	{
		{OS_PROTO_INVALID,  OS_PROTO_INVALID,  OS_PROTO_INVALID,
		 OS_PROTO_INVALID,  OS_PROTO_INVALID},
		
		{OS_PROTO_INVALID,  OS_PROTO_INVALID,  OS_PROTO_INVALID,
		 OS_PROTO_INVALID,  OS_PROTO_INVALID},
		
		{OS_PROTO_TCP,      OS_PROTO_UDP,      OS_PROTO_INVALID,
		 OS_PROTO_INVALID,  OS_PROTO_INVALID}
	};

	// make sure that family and type are in range.
	if ((family < 0) || (family > 2) || (type < 0) || (type > 4))
	{
		return (-1);
	}
	
	// verify that we support the programmer's choice.
	if ( !NU_proto_list[family][type] )
	{
		return(-2);
	}
	
    if(currSocketDescription > 0)
    {
        socket_close(currSocketDescription);
    }

    // check modem ready.
    result = AT_sendCmd_and_chkAck(uart0, "AT", "OK", 1, AT_TIMEOUT_OS_5S);	
    if(result != ATE_SUCCESS)
    {
        return -1;
    }
    
    result = GSM_setEcho(uart0, true);     // enable the echo.

	result = GSM_getCSQ(uart0, 3);         // 
    if(result == ATE_ERROR)                // GSM signal low.
	{
		return -3;
	}	
    if(result != ATE_SUCCESS)              // ATE_NO_RESPONSE.
    {
        return -2;
    }
	
    // check SIM card number.
    memset(GSM_SIM_ID, 0, sizeof(GSM_SIM_ID));  
	result = GSM_getSimCardID(uart0, GSM_SIM_ID, GSM_SIM_ID_LEN);
    if((result != ATE_SUCCESS) || (strlen(GSM_SIM_ID) == 0))
    {
        return -4;
    }
    
//  result = GSM_setEcho(uart0, false);                     // disable the echo.    
  
    memset(GSM_IMEI, 0, sizeof(GSM_IMEI));  
    result = GSM_getIMEI(uart0, GSM_IMEI, GSM_IMEI_LEN);    // get IMEI.
    if((result != ATE_SUCCESS) || (strlen(GSM_IMEI) == 0))
    {
        return -5;
    }
	
    // set socket sending data prompt.
#ifdef USE_PROMPT  
    result = GSM_tcpip_setSendDataPrompt(uart0, 1);	
#else
    result = GSM_tcpip_setSendDataPrompt(uart0, 0);	
#endif
    if(result != ATE_SUCCESS)
    {
        return -4;
    }
	
    // set socket send mode: ACCEPT.    
#ifdef USE_ACCEPT
	result = GSM_tcpip_setSendDataMode(uart0, 1);
#else
	result = GSM_tcpip_setSendDataMode(uart0, 0);
#endif
    if(result != ATE_SUCCESS)
    {
        return -4;
    }

    // set socket receiving IP head.
	result = GSM_tcpip_setIPHead(uart0, 1);
    if(result != ATE_SUCCESS)
    {
        return -5;
    }
	
    /* confirm socket closing */
//  result = GSM_socket_close(uart0, AT_TIMEOUT_OS_2S);
//  if(result != ATE_SUCCESS)
    {
        ;
    }
		
    /* confirm socket shutdown */
    result = GSM_socket_shutdown(uart0, AT_TIMEOUT_OS_2S);
    if(result != ATE_SUCCESS)
    {
        ;
    }

    result = GSM_getStatNetReg(uart0, 3);        // 
    if(result == ATE_ERROR)                      // not registered.
    {
        return -7;
    }
    if(result != ATE_SUCCESS)                    // ATE_NO_RESPONSE.
    {
        return -6;
    }
	
    // GPRS attach.
    result = GSM_GPRS_attach(uart0, true, 2);    // 
    if(result != ATE_SUCCESS)
    {
        return -8;
    }
	
	result = GSM_socket_getStatus(uart0, AT_TIMEOUT_OS_2S);
	if(result == SOCKET_STAT_IPINITIAL)
    {
        result = GSM_tcpip_setApnUsrPwd(uart0, GPRS_apn, GPRS_userName, GPRS_password);
        if(result != ATE_SUCCESS)
        {
            return -9;
        }
    }
			
    // active GPRS when "IP START" state. for repeat setting.
    if( SOCKET_STAT_IPSTART == GSM_socket_getStatus(uart0, AT_TIMEOUT_OS_2S) )
    {
        result = GSM_GPRS_active(uart0);
        if(result != ATE_SUCCESS)
        {
            return -10;
        }
    }

    // get local IP address, otherwise socket can not be setup.	
    result = GSM_getLocalIPAddr(uart0, GSM_IP_Local, AT_TIMEOUT_OS_2S);
    if(result != ATE_SUCCESS)
    {
        return -11;
    }

/*	
	result = GSM_definePDPcontext(uart0, GPRS_apn);  // 
    if(result != ATE_SUCCESS)
    {
        return -7;
    }
*/    
	result = GSM_socket_open(uart0, "TCP", "121.41.25.64", 8080, 2, AT_TIMEOUT_OS_2S);
	if(result == ATE_SUCCESS)
	{
        #ifdef SOCKET_OPEN_TEST
//		OSTimeDlyHMSM(0, 0, 10, 5);        // 10S. 
		#endif
	}
	else if(result == ATE_TCPCLOSED)
	{
        return -12;
	}
	else
	{
        return -13;
	}

    result = GSM_setEcho(uart0, false);              // disable the echo.    
    currSocketDescription = SOCKET_DESCRIPTION;
    currProtocol = NU_proto_list[family][type];     
    
    return currSocketDescription;
}

//*/
void socket_comm_test(INT16S family, INT16S type, INT16S protocol)
{
	INT16S socketfd = 0;
    char testCmdStr[]="01234567890123456789\r\n";
	
	socketfd = socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
	if(socketfd > 0)
	{
		GSM_socket_sendData(uart0, testCmdStr, strlen(testCmdStr), 2, AT_TIMEOUT_OS_2S);

//		currSocketDescription = SOCKET_DESCRIPTION;
//      currProtocol = NU_proto_list[family][type]; 
	}
	else
	{
		socket_close(currSocketDescription);
	}	
}

void socket_comm_test2(INT16S socketfd)
{
	INT16S socketState = SOCKET_ERROR;
	INT16U i = 0;
//	SOCKET_state state = SOCKET_STAT_UNKNOWN; 
	
    char testCmdStr[]=
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 1.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 2.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 3.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 4.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 5.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 6.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 7.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 8.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 9.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 10.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 11.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 12.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 13.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 14.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 15.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 16.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 17.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 18.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 19.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 20.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 21.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 22.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 23.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 24.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 25.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 26.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 27.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 28.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 29.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 30.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 1.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 2.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 3.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 4.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 5.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 6.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 7.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 8.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 9.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 10.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 11.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 12.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 13.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 14.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 15.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 16.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 17.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 18.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 19.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 20.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 21.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 22.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 23.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 24.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 25.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 26.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 27.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 28.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 29.
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 30.
		'\r', '\n'
	};
	
//	socketfd = socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
	
    socketState = socket_connect(socketfd);
	if(socketState == SOCKET_SUCCESS)
	{	
		for(;;)
		{
	        socketState = socket_send_to(socketfd, testCmdStr, strlen(testCmdStr), 2, AT_TIMEOUT_OS_2S);

            GSM_setEcho(uart0, true);
			printf("socketState = %d, i=%d\r\n", socketState, i);
            GSM_setEcho(uart0, false);
			
            if(socketState > 0)
			{
				i++;
				if(i > 5)
				{
					i = 0;
					GSM_dataTransEvt = 1;         // 
					OSTimeDlyHMSM(0, 0, 30, 0);      // 5 minutes. 	
					break;				
				}
			}
			else
			{
				i = 0;
				break;
			}
		}		
	}
	else
	{
		socketfd = socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
	}
}

/* socket connect */
INT16S socket_connect(INT16S socketd)
{
    ATE_response result = ATE_NO_RESPONSE;
	SOCKET_state state = SOCKET_STAT_UNKNOWN; 
	INT16S socketState = SOCKET_ERROR;
	INT8U  i;

    if((SOCKET_DESCRIPTION != socketd) || (currSocketDescription != socketd))
    {
		socket_close(currSocketDescription);
	    socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
    }
	
    /* confirm closing socket */
//  result = GSM_socket_close(uart0, AT_TIMEOUT_OS_2S);
//	result = GSM_socket_shutdown(uart0, AT_TIMEOUT_OS_2S);
//  if(result != ATE_SUCCESS)
    {
        ; /*Ignore the error*/
    }

    /* open socket, TCP only */
/*	
    if(OS_PROTO_UDP == currProtocol)
    {
        result = GSM_socket_open(uart0, "UDP", servAddr->serverName, servAddr->port, 2, AT_TIMEOUT_OS_2S);
    }
    else if (OS_PROTO_TCP == currProtocol)
    {
        result = GSM_socket_open(uart0, "TCP", servAddr->serverName, servAddr->port, 2, AT_TIMEOUT_OS_2S);
    }
    else
    {
        return -2;
    }
    if(result != ATE_SUCCESS)
    {
        return -2;
    }
*/

    /* check status */
	for(i=0; i<5; i++)
	{
        state = GSM_socket_getStatus(uart0, AT_TIMEOUT_OS_2S);   // .     
        if(state == SOCKET_STAT_CONNECTED)  
		{
			socketState = SOCKET_SUCCESS;
			i = 5;
			break;
		}
		else if(state == SOCKET_STAT_IPSTATUS)
		{
			socketState = SOCKET_SUCCESS;
			i = 5;
			break;
		}
		else if(state == SOCKET_STAT_IPINITIAL)
		{
			socketState = SOCKET_SUCCESS;
			i = 5;
			break;
		}
		else if(state == SOCKET_STAT_IPSTART)
		{
			result = GSM_GPRS_active(uart0);
			if(result != ATE_SUCCESS)
			{
//				socketState = SOCKET_ERROR;
			    break;
			}
		}
		else if(state == SOCKET_STAT_IPGPRSACT)    
		{
			result = GSM_socket_shutdown(uart0, AT_TIMEOUT_OS_2S);  // shutdown GPRS.
			if(result != ATE_SUCCESS)
			{
//				socketState = SOCKET_ERROR;
			    break;
			}
		}		
		else
		{
			socketState = SOCKET_ERROR;
			OSTimeDly(DELAY_OS_1S / 10); 
		}
	}
	
	if(socketState == SOCKET_SUCCESS)
	{
/*		
		// get local IP address, otherwise socket can not be setup.	
		result = GSM_getLocalIPAddr(uart0, GSM_IP_Local, AT_TIMEOUT_OS_2S);
		if(result != ATE_SUCCESS)
		{
			state = SOCKET_ERROR;
		}	
*/
		result = GSM_socket_open(uart0, "TCP", "121.41.25.64", 8080, 2, AT_TIMEOUT_OS_5S);
		if(result == ATE_TCPCLOSED)
		{
			socketState = SOCKET_TCPCLOSED;
		}
		if(result != ATE_SUCCESS)
		{
			socketState = SOCKET_ERROR;
		}
	}	
	
    return socketState;     // SUCCESS or ERROR.
}

/* socket send to */
INT16S socket_send_to(INT16S socketd, INT8U *buff, INT16S dataLen, INT16U retryNum, INT32U timeout)
{       
	INT16S result = -2;
	INT16U i;
	INT16U uiTmpVal;
	
    if((SOCKET_DESCRIPTION != socketd) || (currSocketDescription != socketd))
    {
        return -1;
    }
	
	for(i=0; i<retryNum; i++)
	{
        uiTmpVal = GSM_socket_getAcceptDataMax(uart0, AT_TIMEOUT_OS_1S);	
	    if(uiTmpVal >= dataLen)
		{
			if(ATE_SEND_OK == GSM_socket_sendData(uart0, buff, dataLen, 2, timeout))
			{
			    return dataLen;
			}
			else
            {			
				return -2;
			}
		}
		else
		{			
		    result = -1;
	    }
	}
	
    return result;
}

/* socket receive */
INT16U socket_recv_from(INT16S socketd, INT8U *buff, INT16U nbytes, INT32U timeout)
{
    INT8U  result;
    INT16U gotbytes;

    if((SOCKET_DESCRIPTION != socketd) || (currSocketDescription != socketd))
    {
        return -1;
    }   
    result = GSM_socket_getData(uart0, buff, nbytes, &gotbytes, timeout);
    if((ATE_RECEIVE_READY == result) && (gotbytes > 0))
    {
        return gotbytes;
    }
    return SOCKET_NO_DATA;
}

/* socket close */
INT16S socket_close(INT16S socketd)
{
    INT8U result;

    /* confirm socket closing */
//  result = GSM_socket_close(uart0);
//  if(ATE_SUCCESS != result)
    {
//      return -1;
    }

    /*Confirm socket shutdown */
    result = GSM_socket_shutdown(uart0, AT_TIMEOUT_OS_2S);
    if(ATE_SUCCESS != result)
    {
        return -2;
    }

    currSocketDescription = -1;
    currProtocol = -1;
    return SOCKET_SUCCESS;
}

/* change default GPRS parameter */
INT16S GPRS_changeParam(char* apn, char* userName, char* password)
{
    strcpy(GPRS_apn, apn);
    strcpy(GPRS_userName, userName);
    strcpy(GPRS_password, password);
    return 0;
}

/* Get single socket available data size */
INT16U socket_availableData(void)
{
    return GSM_socket_getAcceptDataMax(uart0, AT_TIMEOUT_OS_2S);
}

/* check socket established */
bool socket_established(void)
{
    INT8U result;
    char TestCmdStr[]="AT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\nAT\r\n";

	result = AT_sendCmd_and_chkAck(uart0, "AT", "OK", 2, AT_TIMEOUT_OS_2S);
    if(ATE_NO_RESPONSE != result)
    {
		USART_putbinary(uart0, (INT8U*)TestCmdStr, strlen(TestCmdStr));
//		USART_putbinary(uart0, (INT8U*)TestCmdStr, strlen(TestCmdStr));
//		USART_putbinary(uart0, (INT8U*)TestCmdStr, strlen(TestCmdStr));
//		USART_putbinary(uart0, (INT8U*)TestCmdStr, strlen(TestCmdStr));
		
        OSTimeDly(DELAY_OS_1S);
    }

    result = GSM_socket_getStatus(uart0, AT_TIMEOUT_OS_2S);

    if( SOCKET_STAT_CONNECTED == result)
    {
        return true;
    }
    else
    {
        return false;
    }
}



