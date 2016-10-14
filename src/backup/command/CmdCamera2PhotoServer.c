#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "..\Util\PortTYpes.h"
#include "setup.h"
#include "CmdCamera2PhotoServer.h"
#include "Socket.h"

#define TRUE true
#define FALSE false

#undef memcpy
#define memcpy  Mem_Copy

/*Define command sequence*/
static unsigned short CmdSequence=0;
/*Define camera ID*/
static unsigned short CmdCameraID=0;

/*Define send buffer*/
#define SND_BUF_LEN     1600
static unsigned char SendBuf[SND_BUF_LEN];
#define MAX_FILENAME_LEN    256
static char RemoteFileNameBuf[MAX_FILENAME_LEN]; // TODO: It is wide char (2 Bytes) string, should not use strlen()
static int RemoteFileNameLen = 0; /*Bytes length*/

/*************************Build all kinds of command*****************************/

/*Build command header*/
int BuildCmdHead(CmdHeader * pHead, int CmdLen, unsigned char CmdType)
{

    PUT_CMD_WORD(pHead->Len,CmdLen);   

    PUT_CMD_WORD(pHead->CameraID,CmdCameraID);

    PUT_CMD_BYTE(pHead->Type,CmdType);    
    
    /*Increase the sequence number*/
    ++CmdSequence;
    
    PUT_CMD_WORD(pHead->Seq,CmdSequence);   
    
    return sizeof(CmdHeader);
}
/*Calculate command checksum*/
unsigned short CalculateCheckSum(unsigned char* pBuf, int BufLen)
{
    unsigned short CheckSum=0;
    for(int i=0;i<BufLen;i++)
    {
        CheckSum += (unsigned short)pBuf[i];
    }
    return CheckSum;
}

/*Build login command*/
int BuildLoginCmd(unsigned char* pBuf, int BufLen, unsigned char* pHWMod, unsigned char* pSWVer, char* SN)
{
    int CmdLen = 0;
    unsigned short CheckSum;
    int SNLen=0;
    CmdLogin * pCmd = (CmdLogin *)pBuf;  

    /*Get SN length*/
    SNLen= strlen(SN)>=255?255:strlen(SN);

    /*Check buffer length enough*/
    if((SNLen+sizeof(CmdLogin))>BufLen)
    {
        return CmdLen;
    }


    /*Fill hardware model*/
    memcpy(pCmd->Body.HWModel.by.B,pHWMod,sizeof(pCmd->Body.HWModel));
    /*Fill software version*/
    memcpy(pCmd->Body.SWVersion.by.B,pSWVer,sizeof(pCmd->Body.SWVersion));
    /*Fill SN*/    
    PUT_CMD_BYTE(pCmd->Body.SNLen,SNLen);
#ifdef SUPPORT_ARRAY_ZERO_LENGTH 
    memcpy(pCmd->Body.SN[0].by.B,SN,pCmd->Body.SNLen.u_c);
#else
    memcpy((void*)&pCmd[1],SN,pCmd->Body.SNLen.u_c);
#endif
    /*Fill the cmd header*/
    CmdLen = sizeof(CmdLogin) + pCmd->Body.SNLen.u_c + CMD_CHECKSUM_LEN ;
    BuildCmdHead(&pCmd->Head, CmdLen, CMD_TYPE_LOGIN);

    /*Calculate total command length: include checksum*/
    CheckSum = CalculateCheckSum(pBuf, CmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,CmdLen,CheckSum);

    return CmdLen;
}
/*Build start to transfer photo command*/
int BuildStartTransPhotoCmd(unsigned char* pBuf, int BufLen, int FileOffset, int  FileLen, char* FileName, int FileNameLen)
{
    int CmdLen = 0;
    //int FileNameLen=0;
    unsigned short CheckSum;
    CmdStartTransPhoto * pCmd = (CmdStartTransPhoto *)pBuf;  

    /*Get file name length*/
    //FileNameLen= strlen(FileName)>=255?255:strlen(FileName);

    /*Check buffer length enough*/
    if((FileNameLen+sizeof(CmdStartTransPhoto))>BufLen)
    {
        return CmdLen;
    }

    /*Fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.FileOffset,FileOffset);
    /*Fill file length*/
    PUT_CMD_DWORD(pCmd->Body.FileLen,FileLen);    
    /*Fill file name*/    
    PUT_CMD_BYTE(pCmd->Body.FileNameLen,FileNameLen);
#ifdef SUPPORT_ARRAY_ZERO_LENGTH     
    memcpy(pCmd->Body.FileName[0].by.B,FileName,pCmd->Body.FileNameLen.u_c);
#else
    memcpy((void*)&pCmd[1],FileName,pCmd->Body.FileNameLen.u_c);
#endif

    /*Fill the cmd header*/
    CmdLen = sizeof(CmdStartTransPhoto) + pCmd->Body.FileNameLen.u_c + CMD_CHECKSUM_LEN ;    
    BuildCmdHead(&pCmd->Head, CmdLen, CMD_TYPE_START_UPLOAD_PHOTO);

    /*Calculate total command length: include checksum*/
    CheckSum = CalculateCheckSum(pBuf, CmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,CmdLen,CheckSum);

    return CmdLen;
}
/*Build transfer 1 segment photo command*/
int BuildTrans1SegPhotoCmd(unsigned char* pBuf, int BufLen, int FileOffset, int  SegLen, unsigned char* SegData)
{
    int CmdLen = 0;
    unsigned short CheckSum;
    CmdTrans1SegPhoto * pCmd = (CmdTrans1SegPhoto *)pBuf;  

    /*Check buffer length enough*/
    if((SegLen+sizeof(CmdTrans1SegPhoto))>BufLen)
    {
        return CmdLen;
    }

    /*Fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.FileOffset,FileOffset);
    /*Fill segment length*/
    
    PUT_CMD_WORD(pCmd->Body.SegLen,SegLen);
    /*Fill Seg data*/
#ifdef SUPPORT_ARRAY_ZERO_LENGTH     
    memcpy(pCmd->Body.SegData[0].by.B,SegData,SegLen);
#else
    memcpy((void*)&pCmd[1],SegData,SegLen);
#endif

    /*Fill the cmd header*/
    CmdLen = sizeof(CmdTrans1SegPhoto) + SegLen + CMD_CHECKSUM_LEN ;    
    BuildCmdHead(&pCmd->Head, CmdLen, CMD_TYPE_UPLOAD_PHOTO_SEG);

    /*Calculate total command length: include checksum*/
    CheckSum = CalculateCheckSum(pBuf, CmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,CmdLen,CheckSum);

    return CmdLen;
}
/*Build end to transfer photo command*/
int BuildEndTransPhotoCmd(unsigned char* pBuf, int BufLen, unsigned long FileOffset, unsigned long  FileLen)
{
    int CmdLen = 0;
    unsigned short CheckSum;
    CmdEndTransPhoto * pCmd = (CmdEndTransPhoto *)pBuf;  

    /*Check buffer length enough*/
    if(sizeof(CmdEndTransPhoto)>BufLen)
    {
        return CmdLen;
    }

    /*Fill file offset*/
    PUT_CMD_DWORD(pCmd->Body.FileOffset,FileOffset);
    /*Fill file length*/
    PUT_CMD_DWORD(pCmd->Body.FileLen,FileLen);

    /*Fill the cmd header*/
    CmdLen = sizeof(CmdEndTransPhoto) + CMD_CHECKSUM_LEN ;    
    BuildCmdHead(&pCmd->Head, CmdLen, CMD_TYPE_END_UPLOAD_PHOTO);

    /*Calculate total command length: include checksum*/
    CheckSum = CalculateCheckSum(pBuf, CmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,CmdLen,CheckSum);

    return CmdLen;
}
/*Build logout command*/
int BuildLogoutCmd(unsigned char* pBuf, int BufLen)
{
    int CmdLen = 0;
    unsigned short CheckSum;
    CmdLogout * pCmd = (CmdLogout *)pBuf;  

    /*Check buffer length enough*/
    if(sizeof(CmdLogout)>BufLen)
    {
        return CmdLen;
    }
    
    /*Fill the cmd header*/
    CmdLen = sizeof(CmdLogout) + CMD_CHECKSUM_LEN ;    
    BuildCmdHead(&pCmd->Head, CmdLen, CMD_TYPE_LOGOUT);

    /*Calculate total command length: include checksum*/
    CheckSum = CalculateCheckSum(pBuf, CmdLen-CMD_CHECKSUM_LEN);
    PUT_CMD_CHECKSUM(pBuf,CmdLen,CheckSum);

    return CmdLen;
}
/*************************Build all kinds of command*****************************/





/*************************Resolve all kinds of ACK command *****************************/
/*Resolve command header*/
int ResolveCmdHeader(unsigned char * pBuf, int* pCmdLen, unsigned short* pID ,unsigned char* pCmdType, unsigned short* pCmdSeq)
{
        CmdLoginACK * pCmd = (CmdLoginACK *)pBuf;
        if(pCmdLen)
        {
            GET_CMD_WORD(pCmd->Head.Len, (* pCmdLen));
        }
        if(pID)
        {
            GET_CMD_WORD(pCmd->Head.CameraID, (* pID));
        }
        if(pCmdType)
        {
            GET_CMD_BYTE(pCmd->Head.Type, (* pCmdType));
        }
        if(pCmdSeq)
        {
            GET_CMD_WORD(pCmd->Head.Seq, (* pCmdSeq));  
        }
        return sizeof(CmdLoginACK);
}
/*Check ACK command checksum*/
bool VerifyCheckSum(unsigned char * pBuf,int CmdLen)
{
      unsigned short CheckSum;
      GET_CMD_CHECKSUM(pBuf, CmdLen, CheckSum);
      if(CheckSum == CalculateCheckSum(pBuf,CmdLen-CMD_CHECKSUM_LEN))
      {
            return TRUE;
      }
      else
      {
            return FALSE;
      }
}
/*Rcv login ACK command*/
int RcvLoginACK(unsigned char * pBuf, int BufLen)
{
    int CmdLen;
    unsigned short GotCameraID;
    unsigned char CmdType;
    CmdLoginACK * pCmd = (CmdLoginACK *)pBuf;  

    /*Resolve command header*/
    ResolveCmdHeader(pBuf,&CmdLen,&GotCameraID,&CmdType,NULL);
    
    /*Verify check sum*/
    if((TRUE == VerifyCheckSum(pBuf, CmdLen))
        &&(CmdLen<=BufLen))
    {
        if(CMD_TYPE_ACK_LOGIN == CmdType)
        {
             GET_CMD_WORD(pCmd->Body.CameraID, GotCameraID);
             if((0 != GotCameraID) && (0xFFFF != GotCameraID))
             {
                /*Update camera ID*/
                CmdCameraID = GotCameraID;
             }
             return CmdLen;
        }        
    }
    return -1;
}
/*Rcv start to transfer photo ACK command*/
int RcvStartTransPhotoACK(unsigned char * pBuf, int BufLen,
         int * pPeerCmdSeq, int* pFileOffset)
{
    int CmdLen;
    int FileNameLen=0;
    unsigned short GotCameraID;
    unsigned char CmdType;
    CmdStartTransPhotoACK * pCmd = (CmdStartTransPhotoACK *)pBuf;  

    /*Resolve command header*/
    ResolveCmdHeader(pBuf,&CmdLen,&GotCameraID,&CmdType,NULL);
    
    /*Verify check sum*/
    if((TRUE == VerifyCheckSum(pBuf, CmdLen))
        &&(CmdLen<=BufLen)
        &&(GotCameraID == CmdCameraID))
    {
        if(CMD_TYPE_ACK_START_UPLOAD_PHOTO == CmdType)
        {
             if(pPeerCmdSeq)
             {
                GET_CMD_WORD(pCmd->Body.PeerCmdSeq,(* pPeerCmdSeq));
             }
             if(pFileOffset)
             {
                GET_CMD_DWORD(pCmd->Body.FileOffset,(* pFileOffset));
             }
             GET_CMD_BYTE(pCmd->Body.FileNameLen,FileNameLen);
             if(FileNameLen>sizeof(RemoteFileNameBuf))
             {
                FileNameLen = sizeof(RemoteFileNameBuf);
             }

             RemoteFileNameLen = FileNameLen;
#ifdef SUPPORT_ARRAY_ZERO_LENGTH              
             memcpy(RemoteFileNameBuf,(char*)pCmd->Body.FileName[0].by.B,FileNameLen);
#else
             memcpy(RemoteFileNameBuf,(char*)&pCmd[1],FileNameLen);

#endif
             return CmdLen;
        }
        
    }
    return -1;
}
/*Resolve transfer 1 segment photo ACK command*/
int RcvTrans1SegPhotoACK(unsigned char * pBuf, int BufLen,
         int * pPeerCmdSeq, int* pFileOffset, int* pFileLen)
{
    int CmdLen;
    unsigned short GotCameraID;
    unsigned char CmdType;
    CmdTrans1SegPhotoACK * pCmd = (CmdTrans1SegPhotoACK *)pBuf;  

    /*Resolve command header*/
    ResolveCmdHeader(pBuf,&CmdLen,&GotCameraID,&CmdType,NULL);
    
    /*Verify check sum*/
    if((TRUE == VerifyCheckSum(pBuf, CmdLen))
        &&(CmdLen<=BufLen)
        &&(GotCameraID == CmdCameraID))
    {
        if(CMD_TYPE_ACK_UPLOAD_PHOTO_SEG == CmdType)
        {
             if(pPeerCmdSeq)
             {
                GET_CMD_WORD(pCmd->Body.PeerCmdSeq,(* pPeerCmdSeq));
             }
             if(pFileOffset)
             {
                GET_CMD_DWORD(pCmd->Body.FileOffset,(* pFileOffset));
             }
             if(pFileLen)
             {
                GET_CMD_DWORD(pCmd->Body.FileLen,(* pFileLen));
             }
                          
             return CmdLen;
        }
        
    }
    return -1;
}
/*Resolve end to transfer photo ACK command*/
int RcvEndTransPhotoACK(unsigned char * pBuf, int BufLen,
         int * pPeerCmdSeq, int* pFileOffset, int* pFileLen)
{
    int CmdLen;
    unsigned short GotCameraID;
    unsigned char CmdType;
    CmdEndTransPhotoACK * pCmd = (CmdEndTransPhotoACK *)pBuf;  

    /*Resolve command header*/
    ResolveCmdHeader(pBuf,&CmdLen,&GotCameraID,&CmdType,NULL);
    
    /*Verify check sum*/
    if((TRUE == VerifyCheckSum(pBuf, CmdLen))
        &&(CmdLen<=BufLen)
        &&(GotCameraID == CmdCameraID))
    {
        if(CMD_TYPE_ACK_END_UPLOAD_PHOTO == CmdType)
        {
             if(pPeerCmdSeq)
             {
                GET_CMD_WORD(pCmd->Body.PeerCmdSeq,(* pPeerCmdSeq));
             }
             if(pFileOffset)
             {
                GET_CMD_DWORD(pCmd->Body.FileOffset,(* pFileOffset));
             }
             if(pFileLen)
             {
                GET_CMD_DWORD(pCmd->Body.FileLen,(* pFileLen));
             }
                          
             return CmdLen;
        }
        
    }
    return -1;
}
/*************************Resolve all kinds of ACK command*****************************/


/*************************Send command*****************************/
/*Login remote server*/
int LoginRemoteServer(int fd)
{
    int CmdLen;

    memset(SendBuf,0,sizeof(SendBuf));

    /*Reset CmdCameraID*/
    CmdCameraID = 0;
    
    CmdLen = BuildLoginCmd(SendBuf, sizeof(SendBuf), (unsigned char*)HW_MODEL, (unsigned char*)SW_VERSION, strIMEI);

    return Send_To(fd,SendBuf,CmdLen,0,NULL,0);
}

/*Start to transfer photo*/
int StartTransPhoto(int fd, int FileOffset,int FileLen)
{
    int CmdLen;

    memset(SendBuf,0,sizeof(SendBuf));
    
    CmdLen = BuildStartTransPhotoCmd(SendBuf, sizeof(SendBuf), FileOffset, FileLen, RemoteFileNameBuf,RemoteFileNameLen);

    return Send_To(fd,SendBuf,CmdLen,0,NULL,0);
}

/*Transfer 1 segment photo*/
int Trans1SegPhoto(int fd, int FileOffset,int SegLen, unsigned char* SegData)
{
    int CmdLen;

    memset(SendBuf,0,sizeof(SendBuf));
    
    CmdLen = BuildTrans1SegPhotoCmd(SendBuf, sizeof(SendBuf), FileOffset, SegLen, SegData);

    return Send_To(fd,SendBuf,CmdLen,0,NULL,0);
}

/*End to transfer photo*/
int EndTransPhoto(int fd, int FileOffset,int FileLen)
{
    int CmdLen;

    memset(SendBuf,0,sizeof(SendBuf));
    
    CmdLen = BuildEndTransPhotoCmd(SendBuf, sizeof(SendBuf), FileOffset, FileLen);

    return Send_To(fd,SendBuf,CmdLen,0,NULL,0);
}

/*Logout remote server*/
int LogoutRemoteServer(int fd)
{
    int CmdLen;

    memset(SendBuf,0,sizeof(SendBuf));
    
    CmdLen = BuildLogoutCmd(SendBuf, sizeof(SendBuf));

    return Send_To(fd,SendBuf,CmdLen,0,NULL,0);
}

/*************************Send command*****************************/


/*Initialize camera to photo server*/
void InitCmdCamera2PhotoServer(void)
{
    // TODO: Should be removed when commit. Arthur@2014/12/1
    int structlen;    
    int allStructLen[]={
        sizeof(CmdHeader),
        sizeof(CmdLogin),
        sizeof(CmdStartTransPhoto),
        sizeof(CmdTrans1SegPhoto),
        sizeof(CmdEndTransPhoto),
        sizeof(CmdLogout),
        sizeof(CmdLoginACK),
        sizeof(CmdStartTransPhotoACK),
        sizeof(CmdTrans1SegPhotoACK),
        sizeof(CmdEndTransPhotoACK)};

        
    memset(RemoteFileNameBuf,0,sizeof(RemoteFileNameBuf));
    RemoteFileNameLen = 0;

}

