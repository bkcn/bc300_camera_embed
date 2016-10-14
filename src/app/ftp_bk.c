#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "modem.h"
#include "socket.h"
#include "ftp.h"


#define CMD_TRANS_SEG_HEAD_SIZE           15   // 13 + 2


static unsigned char bufRcv[2048];

int FtpPutFile_BK(char* apn, char* userName, char* password, char* serverName, 
	              int ftpPort, unsigned char* buff, int bufLen, int* currPos )
{
    int result;
    int socketfd;
    int onceLen, putLen;
    int transferLen, ConfLen;
    int sockBuffLen;
    int errorCode;
    char simCardNo[24];
    struct addr_struct remoteAddr;
    int  fileOffset = 0, fileLen;    

    if(strlen(serverName) >= sizeof(remoteAddr.serverName))
    {
        return -1;
    }

    GPRS_changeParam(apn, userName, password);

    socketfd = socket_open(OS_FAMILY_IP, OS_TYPE_STREAM, OS_PROTO_TCP);
    if(socketfd >= 0)
    {
        memset(remoteAddr.serverName, 0, sizeof(remoteAddr.serverName));
        strcpy(remoteAddr.serverName, serverName);
        remoteAddr.port = ftpPort;
//      result = socket_connect(socketfd, &remoteAddr, sizeof(remoteAddr));
        result = socket_connect(socketfd);
    }
    else
    {
        return -1;
    }

    if(0 > result)
    {
        return result;
    }

    /* Login */
    result = remoteServer_login(socketfd);
    if(0 > result)
    {
        return result;
    }

    do
	{
//      result = socket_recv_from(socketfd, bufRcv, sizeof(bufRcv), 0, NULL, NULL, 5000);
        result = socket_recv_from(socketfd, bufRcv, sizeof(bufRcv), AT_TIMEOUT_OS_2S);
    } while(0 == result);

    if(result > 0)
    {
         if(0 >= remoteServer_loginAckRes(bufRcv, result))
         {
             return -1;
         }
    }
    else
    {
        return -1;
    }

    /* transfer file start*/
    fileOffset = *currPos;
    result = transData_start(socketfd, fileOffset, bufLen);
    if(0 > result)
    {
        return result;
    }

    do
	{
//      result = socket_recv_from(socketfd, bufRcv, sizeof(bufRcv), 0, NULL, NULL, 5000);
        result = socket_recv_from(socketfd, bufRcv, sizeof(bufRcv), AT_TIMEOUT_OS_2S);
    } while(0 == result);

    if(result>0)
    {
         if(0 >= transData_startAckRes(bufRcv, result, NULL, &fileOffset))
         {
             return -1;
         }
    }
    else
    {
        return -1;
    }

    /* put data */
    putLen = fileOffset;
    do{           
        sockBuffLen = socket_availableData();
        if(0 > sockBuffLen)  
        {
            *currPos = fileOffset;
            return -1;
        }
        if(CMD_TRANS_SEG_HEAD_SIZE >= sockBuffLen)
        {
            OSTimeDly(1);
            continue;
        }

        transferLen = sockBuffLen - CMD_TRANS_SEG_HEAD_SIZE;        
        
        if(transferLen > (bufLen-putLen))
        {
            transferLen = bufLen-putLen;
        }          
        
        /* transfer file 1 segment */
        if(0 > transData_1Seg(socketfd,fileOffset, transferLen, &buff[fileOffset]))
        {
            if(true == socket_established())
            {
                continue;
            }
            socket_close(socketfd);
            *currPos = fileOffset;
            return -1;
            break;
        }
        putLen     += transferLen;
        fileOffset += transferLen;
    } while(putLen < bufLen);
    
    /* transfer file end */
    result = transData_end(socketfd, fileOffset, bufLen);
    if(0 > result)
    {
        *currPos = fileOffset;
        return result;
    }

    do
	{
        result = socket_recv_from(socketfd, bufRcv, sizeof(bufRcv), AT_TIMEOUT_OS_2S);
    } while(0 == result);

    if(result > 0)
    {
		if(0 >= transData_endAckRes(bufRcv, result, NULL, &fileOffset, &fileLen))
		{
		 *currPos = fileOffset;
		 return -1;
		}
    }
    else
    {
        *currPos = fileOffset;
        return -1;
    }

    result = remoteServer_logout(socketfd);

    if(0 <= result)
    {
        OSTimeDly(HZ * 2);
        socket_close(socketfd);
    }

    *currPos = fileOffset;
    return bufLen;
}




