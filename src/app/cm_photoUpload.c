#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "cm_photoUpload.h"
#include "socket.h"


#undef  memcpy
#define memcpy  Mem_Copy

/* define command sequence */
static INT16U cmdSequence = 0;
/* define camera ID*/
static INT16U cmdCameraID = 0;

/* define send buffer */
#define SND_BUF_LEN         1600
static  INT8U bufSend[SND_BUF_LEN];

#define MAX_FILENAME_LEN    256
static  char remoteFileNameBuf[MAX_FILENAME_LEN];  // TODO: It is wide char (2 Bytes) string, should not use strlen()
static  int  remoteFileNameLen = 0;                /* Bytes length */


/* command header build */
int cmdBuild_head(cmdHeader *pHead, int cmdLen, INT8U cmdType)
{
    PUT_CMD_WORD(pHead->len,      cmdLen);  
    PUT_CMD_WORD(pHead->cameraID, cmdCameraID);
    PUT_CMD_BYTE(pHead->type,     cmdType);    
    
    /* increase the sequence number */
    ++cmdSequence;
    
    PUT_CMD_WORD(pHead->seq, cmdSequence);   
    
    return sizeof(cmdHeader);
}

/* checkSum calculate */
INT16U checkSum_calc(INT8U* pBuf, int bufLen)
{
    INT16U checkSum = 0;
	INT16U i;

    for(i=0; i<bufLen; i++)
    {
        checkSum += (INT16U)pBuf[i];
    }
    return checkSum;
}

/* command header build login */
int cmdBuild_login(INT8U* pBuf, int bufLen, INT8U* pHwMode, INT8U* pSwVer, char* sn)
{
    INT16U checkSum;
    int    cmdLen = 0;
    int    snLen=0;
    cmdLogin *pCmd = (cmdLogin *)pBuf;  

    /* get sn length */
    snLen = strlen(sn) >= 255?255:strlen(sn);

    /* check buffer length enough */
    if((snLen + sizeof(cmdLogin)) > bufLen)
    {
        return cmdLen;
    }

    /* fill hardware model */
    memcpy(pCmd->Body.hwModel.by.B, pHwMode, sizeof(pCmd->Body.hwModel));

    /* fill software version */
    memcpy(pCmd->Body.swVersion.by.B, pSwVer, sizeof(pCmd->Body.swVersion));

    /* fill sn */    
    PUT_CMD_BYTE(pCmd->Body.snLen, snLen);

#ifdef SUPPORT_ARRAY_ZERO_LENGTH 
    memcpy(pCmd->Body.sn[0].by.B, sn, pCmd->Body.snLen.u_c);
#else
    memcpy((void*)&pCmd[1], sn, pCmd->Body.snLen.u_c);
#endif
	
    /* fill the cmd header */
    cmdLen = sizeof(cmdLogin) + pCmd->Body.snLen.u_c + CMD_CHECKSUM_LEN ;
    cmdBuild_head(&pCmd->Head, cmdLen, CMD_TYPE_LOGIN);

    /* calculate total command length: include checkSum*/
    checkSum = checkSum_calc(pBuf, (cmdLen-CMD_CHECKSUM_LEN));
    PUT_CMD_CHECKSUM(pBuf, cmdLen, checkSum);

    return cmdLen;
}

/* command header build transmit start */
int cmdBuild_transStart(INT8U* pBuf, int bufLen, int fileOffset, int  fileLen, 
	                    char* fileName, int fileNameLen)
{
    int cmdLen = 0;
//  int fileNameLen=0;
    INT16U checkSum;
    cmdTransStart *pCmd = (cmdTransStart *)pBuf;  

    /* get file name length */
//  fileNameLen = strlen(fileName) >= 255?255:strlen(fileName);

    /* check buffer length enough */
    if((fileNameLen + sizeof(cmdTransStart)) > bufLen)
    {
        return cmdLen;
    }

    /* fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.fileOffset, fileOffset);
	
    /* fill file length*/
    PUT_CMD_DWORD(pCmd->Body.fileLen, fileLen);   
	
    /* fill file name*/    
    PUT_CMD_BYTE(pCmd->Body.fileNameLen, fileNameLen);
	
#ifdef SUPPORT_ARRAY_ZERO_LENGTH     
    memcpy(pCmd->Body.fileName[0].by.B, fileName, pCmd->Body.fileNameLen.u_c);
#else
    memcpy((void*)&pCmd[1], fileName, pCmd->Body.fileNameLen.u_c);
#endif

    /* fill the cmd header*/
    cmdLen = sizeof(cmdTransStart) + pCmd->Body.fileNameLen.u_c + CMD_CHECKSUM_LEN ;    
    cmdBuild_head(&pCmd->Head, cmdLen, CMD_TYPE_START_UPLOAD_PHOTO);

    /* calculate total command length: include checkSum */
    checkSum = checkSum_calc(pBuf, (cmdLen-CMD_CHECKSUM_LEN));
    PUT_CMD_CHECKSUM(pBuf, cmdLen, checkSum);

    return cmdLen;
}

/* command header build transmit 1 segment */
int cmdBuild_trans1Seg(INT8U* pBuf, int bufLen, int fileOffset, int segLen, INT8U* SegData)
{
    INT16U checkSum;
    int    cmdLen = 0;
    cmdTrans1Seg *pCmd = (cmdTrans1Seg *)pBuf;  

    /* check buffer length enough*/
    if((segLen+sizeof(cmdTrans1Seg))>bufLen)
    {
        return cmdLen;
    }

    /* fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.fileOffset,fileOffset);
    /* fill segment length*/
    
    PUT_CMD_WORD(pCmd->Body.segLen,segLen);
    /* fill Seg data*/
#ifdef SUPPORT_ARRAY_ZERO_LENGTH     
    memcpy(pCmd->Body.SegData[0].by.B,SegData,segLen);
#else
    memcpy((void*)&pCmd[1],SegData,segLen);
#endif

    /* fill the cmd header*/
    cmdLen = sizeof(cmdTrans1Seg) + segLen + CMD_CHECKSUM_LEN ;    
    cmdBuild_head(&pCmd->Head, cmdLen, CMD_TYPE_UPLOAD_PHOTO_SEG);

    /* calculate total command length: include checkSum*/
    checkSum = checkSum_calc(pBuf, cmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,cmdLen,checkSum);

    return cmdLen;
}

/* command header build transmit end */
int cmdBuild_transEnd(INT8U* pBuf, int bufLen, unsigned long fileOffset, unsigned long  fileLen)
{
    int cmdLen = 0;
    INT16U checkSum;
    cmdTransEnd *pCmd = (cmdTransEnd *)pBuf;  

    /* check buffer length enough*/
    if(sizeof(cmdTransEnd)>bufLen)
    {
        return cmdLen;
    }

    /* fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.fileOffset,fileOffset);
    /* fill file length*/
    PUT_CMD_DWORD(pCmd->Body.fileLen,fileLen);

    /* fill the cmd header*/
    cmdLen = sizeof(cmdTransEnd) + CMD_CHECKSUM_LEN ;    
    cmdBuild_head(&pCmd->Head, cmdLen, CMD_TYPE_END_UPLOAD_PHOTO);

    /* calculate total command length: include checkSum*/
    checkSum = checkSum_calc(pBuf, cmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,cmdLen,checkSum);

    return cmdLen;
}

/* command header build logout */
int cmdBuild_logout(INT8U* pBuf, int bufLen)
{
    int cmdLen = 0;
    INT16U checkSum;
    cmdLogout *pCmd = (cmdLogout *)pBuf;  

    /* check buffer length enough */
    if(sizeof(cmdLogout) > bufLen)
    {
        return cmdLen;
    }
    
    /* fill the cmd header */
    cmdLen = sizeof(cmdLogout) + CMD_CHECKSUM_LEN;    
    cmdBuild_head(&pCmd->Head, cmdLen, CMD_TYPE_LOGOUT);

    /* calculate total command length: include checkSum */
    checkSum = checkSum_calc(pBuf, cmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf, cmdLen, checkSum);

    return cmdLen;
}


/*  command header resolve*/
int cmdHeader_resolve(INT8U *pBuf, int *pCmdLen, INT16U *pID ,INT8U *pCmdType, INT16U *pCmdSeq)
{
        cmdLoginAck *pCmd = (cmdLoginAck *)pBuf;
        if(pCmdLen)
        {
            GET_CMD_WORD(pCmd->Head.len, (*pCmdLen));
        }
        if(pID)
        {
            GET_CMD_WORD(pCmd->Head.cameraID, (*pID));
        }
        if(pCmdType)
        {
            GET_CMD_BYTE(pCmd->Head.type, (*pCmdType));
        }
        if(pCmdSeq)
        {
            GET_CMD_WORD(pCmd->Head.seq, (*pCmdSeq));  
        }
        return sizeof(cmdLoginAck);
}

/* checkSum verify */
bool checkSum_verify(INT8U *pBuf, int cmdLen)
{
      INT16U checkSum;
      GET_CMD_CHECKSUM(pBuf, cmdLen, checkSum);
      if(checkSum == checkSum_calc(pBuf, (cmdLen-CMD_CHECKSUM_LEN)))
      {
            return true;
      }
      else
      {
            return false;
      }
}

/* remote server login ACK command resolve */
int remoteServer_loginAckRes(INT8U *pBuf, int bufLen)
{
    int cmdLen;
    INT16U gotCameraID;
    INT8U cmdType;
    cmdLoginAck *pCmd = (cmdLoginAck *)pBuf;  

    /* resolve command header*/
    cmdHeader_resolve(pBuf,&cmdLen,&gotCameraID,&cmdType,NULL);
    
    /* verify check sum*/
    if((true == checkSum_verify(pBuf, cmdLen))
        &&(cmdLen<=bufLen))
    {
        if(CMD_TYPE_ACK_LOGIN == cmdType)
        {
             GET_CMD_WORD(pCmd->Body.cameraID, gotCameraID);
             if((0 != gotCameraID) && (0xFFFF != gotCameraID))
             {
                /*Update camera ID*/
                cmdCameraID = gotCameraID;
             }
             return cmdLen;
        }        
    }
    return -1;
}

/* transmit start ACK command resolve */
int transData_startAckRes(INT8U * pBuf, int bufLen, int *pCmdPeerSeq, int* pFileOffset)
{
    int    cmdLen;
    int    fileNameLen=0;
    INT16U gotCameraID;
    INT8U  cmdType;
    cmdTransStartAck *pCmd = (cmdTransStartAck *)pBuf;  

    /* resolve command header*/
    cmdHeader_resolve(pBuf, &cmdLen, &gotCameraID, &cmdType, NULL);
    
    /* verify check sum*/
    if((true == checkSum_verify(pBuf, cmdLen)) &&
	   (cmdLen <= bufLen) && (gotCameraID == cmdCameraID))
    {
        if(CMD_TYPE_ACK_START_UPLOAD_PHOTO == cmdType)
        {
            if(pCmdPeerSeq)
            {
                GET_CMD_WORD(pCmd->Body.cmdPeerSeq, (*pCmdPeerSeq));
            }
            if(pFileOffset)
            {
                GET_CMD_DWORD(pCmd->Body.fileOffset, (*pFileOffset));
            }
            GET_CMD_BYTE(pCmd->Body.fileNameLen, fileNameLen);
            if(fileNameLen>sizeof(remoteFileNameBuf))
            {
                fileNameLen = sizeof(remoteFileNameBuf);
            }

            remoteFileNameLen = fileNameLen;

			#ifdef SUPPORT_ARRAY_ZERO_LENGTH              
            memcpy(remoteFileNameBuf,(char*)pCmd->Body.fileName[0].by.B, fileNameLen);
            #else
            memcpy(remoteFileNameBuf,(char*)&pCmd[1], fileNameLen);
            #endif
			
            return cmdLen;
        }        
    }
    return -1;
}

/*  transmit 1 segment ACK command resolve */
int transData_1SegAckRes(INT8U *pBuf, int bufLen, int *pCmdPeerSeq, int *pFileOffset, int *pFileLen)
{
    int    cmdLen;
    INT16U gotCameraID;
    INT8U  cmdType;
    cmdTrans1SegAck *pCmd = (cmdTrans1SegAck *)pBuf;  

    /* resolve command header */
    cmdHeader_resolve(pBuf,&cmdLen,&gotCameraID,&cmdType,NULL);
    
    /* verify check sum */
    if((true == checkSum_verify(pBuf, cmdLen)) &&
	   (cmdLen<=bufLen) && (gotCameraID == cmdCameraID))
    {
        if(CMD_TYPE_ACK_UPLOAD_PHOTO_SEG == cmdType)
        {
             if(pCmdPeerSeq)
             {
                GET_CMD_WORD(pCmd->Body.cmdPeerSeq, (*pCmdPeerSeq));
             }
             if(pFileOffset)
             {
                GET_CMD_DWORD(pCmd->Body.fileOffset, (*pFileOffset));
             }
             if(pFileLen)
             {
                GET_CMD_DWORD(pCmd->Body.fileLen, (*pFileLen));
             }
                          
             return cmdLen;
        }        
    }
    return -1;
}

/*  transmit end ACK command resolve*/
int transData_endAckRes(INT8U *pBuf, int bufLen, int *pCmdPeerSeq, int* pFileOffset, int* pFileLen)
{
    int    cmdLen;
    INT16U gotCameraID;
    INT8U  cmdType;
    cmdTransEndAck *pCmd = (cmdTransEndAck *)pBuf;  

    /* resolve command header */
    cmdHeader_resolve(pBuf, &cmdLen, &gotCameraID, &cmdType, NULL);
    
    /* verify check sum */
    if((true == checkSum_verify(pBuf, cmdLen)) &&
	   (cmdLen<=bufLen) && (gotCameraID == cmdCameraID))
    {
        if(CMD_TYPE_ACK_END_UPLOAD_PHOTO == cmdType)
        {
             if(pCmdPeerSeq)
             {
                GET_CMD_WORD(pCmd->Body.cmdPeerSeq, (*pCmdPeerSeq));
             }
             if(pFileOffset)
             {
                GET_CMD_DWORD(pCmd->Body.fileOffset, (*pFileOffset));
             }
             if(pFileLen)
             {
                GET_CMD_DWORD(pCmd->Body.fileLen, (*pFileLen));
             }
                          
             return cmdLen;
        }        
    }
    return -1;
}


/* remote server login */
int remoteServer_login(int fd)
{
    int cmdLen;

    memset(bufSend, 0, sizeof(bufSend));    
    cmdCameraID = 0;                                  /* reset cmdCameraID */    
    cmdLen = cmdBuild_login(bufSend, sizeof(bufSend), (INT8U*)HW_MODEL, 
		                   (INT8U*)SW_VERSION, GSM_IMEI);                    // NOTSURE.

    return socket_send_to(fd, bufSend, cmdLen, 2, AT_TIMEOUT_OS_2S);
}

/*  transmit start */
int transData_start(int fd, int fileOffset,int fileLen)
{
    int cmdLen;

    memset(bufSend, 0, sizeof(bufSend));    
    cmdLen = cmdBuild_transStart(bufSend, sizeof(bufSend), fileOffset, fileLen, 
		                         remoteFileNameBuf, remoteFileNameLen);

    return socket_send_to(fd, bufSend, cmdLen, 2, AT_TIMEOUT_OS_2S);
}

/* transmit 1 segment */
int transData_1Seg(int fd, int fileOffset, int segLen, INT8U* SegData)
{
    int cmdLen;

    memset(bufSend, 0, sizeof(bufSend));    
    cmdLen = cmdBuild_trans1Seg(bufSend, sizeof(bufSend), fileOffset, segLen, SegData);

    return socket_send_to(fd, bufSend, cmdLen, 2, AT_TIMEOUT_OS_2S);     // KEYPOINT. 
}

/* transmit end */
int transData_end(int fd, int fileOffset, int fileLen)
{
    int cmdLen;

    memset(bufSend, 0, sizeof(bufSend));    
    cmdLen = cmdBuild_transEnd(bufSend, sizeof(bufSend), fileOffset, fileLen);

    return socket_send_to(fd, bufSend, cmdLen, 2, AT_TIMEOUT_OS_2S);
}

/* logout from remote server */
int remoteServer_logout(int fd)
{
    int cmdLen;

    memset(bufSend, 0, sizeof(bufSend));    
    cmdLen = cmdBuild_logout(bufSend, sizeof(bufSend));

    return socket_send_to(fd, bufSend, cmdLen, 2, AT_TIMEOUT_OS_2S);
}


/* photo data structure initialize */
void TransData_InitStruct(void)
{
    // TODO: Should be removed when commit. Arthur@2014/12/1
    int structLen;    
    int allStructLen[]=
	{
        sizeof(cmdHeader),
        sizeof(cmdLogin),
        sizeof(cmdTransStart),
        sizeof(cmdTrans1Seg),
        sizeof(cmdTransEnd),
        sizeof(cmdLogout),
        sizeof(cmdLoginAck),
        sizeof(cmdTransStartAck),
        sizeof(cmdTrans1SegAck),
        sizeof(cmdTransEndAck)
	};
        
    memset(remoteFileNameBuf, 0, sizeof(remoteFileNameBuf));
    remoteFileNameLen = 0;
}

