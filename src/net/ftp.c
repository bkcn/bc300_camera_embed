#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <includes.h>
#include "socket.h"
//#include "modem.h"


#define  MAX_ONCE_TRANSFER_LENGTH        1024

static int ftp_open(char* apnName, char* serverName, INT16U ftpPort, char* userName, char* password)
{
    INT8U result;
    int   errorcode;
    char  simCardNo[24];

    // check modem ready.
    result = AT_sendCmd_and_chkAck(uart0, "AT", "OK", 1, AT_TIMEOUT_OS_2S);	
    if(result != ATE_SUCCESS)
    {
        return -3;
    }
    
    result = GSM_setEcho(uart0, true);     // enable the echo.

    if(GSM_getCSQ(uart0, 10) < 8)          // check GSM signal.
    {
        return -4;
    }
	
    // check SIM card number.
    memset(simCardNo, 0, sizeof(simCardNo));  
	result = GSM_getSimCardID(uart0, simCardNo, GSM_SIM_ID_LEN);
    if((result != ATE_SUCCESS) || (strlen(simCardNo) == 0))
    {
        return -5;
    }
    
    result = GSM_setEcho(uart0, false);            //disable the echo.  
  
    /* set connectting type */
	result = GSM_setIPApplication(uart0, 3, 1, "CONTYPE", "GPRS");
    if(ATE_SUCCESS != result)		
    {
        return -4;
    }
	
    /* set APN name */
	result = GSM_setIPApplication(uart0, 3, 1, "APN", apnName);
    if(ATE_SUCCESS != result)		
    {
        return -4;
    }

    /* close carrier ID */
	result = GSM_setIPApplication(uart0, 0, 1, NULL, NULL);
    if(ATE_SUCCESS != result)		
	{
        ;
    }
	
    /* open the carrier ID */
	result = GSM_setIPApplication(uart0, 1, 1, NULL, NULL);
    if(ATE_SUCCESS != result)		
    {
        return -6;
    }

    /* set the FTP carrier ID */
	result = GSM_ftp_setCid(uart0, 1);
    if(ATE_SUCCESS != result)		
    {
        return -7;
    }
	
    /* Set the FTP port */
	result = GSM_ftp_setPort(uart0, ftpPort);
    if(ATE_SUCCESS != result)		
    {
        return -8;
    }
	
    /* 主动模式 */
	result = GSM_ftp_setMode(uart0, 0);
    if(ATE_SUCCESS != result)		
    {
        return -9;
    }
	
    /* 数据模式 */    
	result = GSM_ftp_setType(uart0, "I");
    if(ATE_SUCCESS != result)		
    {
        return -10;
    }
    /* 存储模式 */
	result = GSM_ftp_setInputType(uart0, "STOR");
    if(ATE_SUCCESS != result)		
    {
        return -11;
    }
	
    /* 服务器地址 */
	result = GSM_ftp_setServer(uart0, serverName);
    if(ATE_SUCCESS != result)		
    {
        return -12;
    }
	
    /* 用户名 */  
	result = GSM_ftp_setUserName(uart0, userName);
    if(ATE_SUCCESS != result)		
    {
        return -15;
    }
	
    /* 密码 */
	result = GSM_ftp_setPassword(uart0, password);
    if(ATE_SUCCESS != result)		
    {
        return -16;
    }   
    return 0;
}

int GSM_ftp_putFile(char* apnName, char* serverName, INT16S ftpPort,char* userName,
	                char* password, char* FilePath, char*FileName, INT8U* Buff, int BuffLen)
{
    INT8U result;
    int  OnceLen, PutLen;
    int  TransferLen, ConfLen;
    int  errorcode;
    char simCardNo[24];

    /* ftp common open */
	result = ftp_open(apnName, serverName, ftpPort, userName, password);
    if( result < 0)
    {
        return -100;
    }
    
    /* 存储路径 */
	result = GSM_ftp_setPutFilePath(uart0, FilePath);
    if(ATE_SUCCESS !=  result)
    {
        return -13;
    }
	
    /* 文件名称 */
	result = GSM_ftp_setPutFileName(uart0, FileName);
    if(ATE_SUCCESS !=  result)
    {
        return -14;
    }
    
    /* ftp opened */
    errorcode = 0;
    OnceLen = 0;
	result = GSM_ftp_putOpen(uart0, &errorcode, &OnceLen);
    if(ATE_SUCCESS != result)	
    {
        OSTimeDly( DELAY_OS_2S );
		
        /* repeat to open the FTP application */
	    result = GSM_ftp_putOpen(uart0, &errorcode, &OnceLen);
        if(ATE_SUCCESS != result)	
        {
            return -17;
        }
    }
	
    /* check error code and man length */
    if(1 != errorcode)
    {
        errorcode=0;
        OnceLen=0;

        /* repeat to check */
        GSM_ftp_putOpen(uart0, &errorcode, &OnceLen);
        if(1 != errorcode)
        {
            return -18;
        }
    }
    
    if(OnceLen == 0)
	{
        OnceLen = MAX_ONCE_TRANSFER_LENGTH;
	}
    TransferLen = MAX_ONCE_TRANSFER_LENGTH;
    
    if(OnceLen < TransferLen)
	{
        TransferLen = OnceLen;
	}

    /* put data */
    PutLen = 0;
    do
	{     
        if(TransferLen > (BuffLen-PutLen))
        {
            TransferLen = BuffLen-PutLen;
        }
        ConfLen = 0;
        result = GSM_ftp_putData(uart0, &Buff[PutLen], TransferLen, &ConfLen);

        if(result != ATE_SUCCESS)
        {
            OSTimeDly(DELAY_OS_1S / 2);          // 
			
            /* retry to send data */
            result = GSM_ftp_putData(uart0, &Buff[PutLen], TransferLen, &ConfLen);
            if(result != ATE_SUCCESS)
            {       
                OSTimeDly(DELAY_OS_1S * 10);     // NOTSURE, 10S ?
                break;
            }
        }
		
        PutLen += ConfLen;
    } while(PutLen < BuffLen);

    /* mark end */
    if(ATE_SUCCESS != GSM_ftp_putData(uart0, &Buff[PutLen], 0, &ConfLen))
    {
        return -20;
    }
    
    return BuffLen;    
}

int GSM_ftp_getFile(char* apnName, char* serverName, INT16S ftpPort, char* userName, 
	                char* password, char* FilePath, char* FileName, INT8U* Buff, int BuffLen)
{
    INT8U result;
    int   GotLen;
    int   TransferLen, ConfLen;
    char  errorcode;
    char  simCardNo[24];

    /* ftp common open */
	result = ftp_open(apnName, serverName, ftpPort, userName, password);
    if(result < 0)
    {
        return -100;
    }
    
    /* 获取路径 */
	result = GSM_ftp_setGetFilePath(uart0, FilePath);
    if(ATE_SUCCESS != result)
    {
        return -13;
    }
	
    /* 文件名称 */
	result = GSM_ftp_setGetFileName(uart0, FileName);
    if(ATE_SUCCESS != result)
    {
        return -14;
    }
    
    /* open ftp */
    errorcode = 0;
	
	result = GSM_ftp_getOpen(uart0, &errorcode);
    if(ATE_SUCCESS != result)
    {
        /* repeat to open the FTP application */
	    result = GSM_ftp_getOpen(uart0, &errorcode);
        if(ATE_SUCCESS != result)
        {
            return -17;
        }
    }
	
    /* check error code and man length */
    if(1 != errorcode)
    {
        errorcode = 0;
		
        /* repeat to check */
        GSM_ftp_getOpen(uart0, &errorcode);
        if(1 != errorcode)
        {
            return -18;
        }
    }
        
    TransferLen = MAX_ONCE_TRANSFER_LENGTH;
    
    /* get data */
    GotLen = 0;
    do
	{     
        if(TransferLen > (BuffLen-GotLen))
        {
            TransferLen = BuffLen-GotLen;
        }
        ConfLen = 0;
		
        result = GSM_ftp_getData(uart0, &Buff[GotLen], TransferLen, &ConfLen);
        if(result != ATE_SUCCESS)
        {
            break;
        }
        GotLen += ConfLen;
        if((ConfLen < TransferLen) || (ConfLen == 0))
        {
            break;
        }
    } while(GotLen < BuffLen);    
    
    return BuffLen;    
}





