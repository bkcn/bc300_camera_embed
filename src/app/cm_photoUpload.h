#ifndef __CM_PHOTOUPLOAD_H
#define __CM_PHOTOUPLOAD_H

#include <includes.h>


// TODO: Should open compiling option(pack struct) in IDE. Arthur@2014/12/1
#ifdef __GNUC__
	#define GNUC_PACKED    __attribute__ ((__packed__))
#else
	#define GNUC_PACKED
#endif

#ifdef __arm
	#define ARM_PACKED    __packed
#else
	#define ARM_PACKED
#endif

#ifdef WIN32
	#pragma pack(1)
#endif

/* define comand DWORD union type */
typedef ARM_PACKED union
{
	ARM_PACKED struct  
	{
		unsigned char B[4];
	} by;
	unsigned long int u_li;
	         long int s_li;
} CMD_DWORD;

/* define comand WORD union type */
typedef ARM_PACKED union
{
	ARM_PACKED struct  
	{
		unsigned char B[2];
	} by;
	unsigned short int u_s;
	         short int s_s;
} CMD_WORD;

/* define comand BYTE union type */
typedef ARM_PACKED union
{
	ARM_PACKED struct 
	{
		unsigned char B[1];
	} by;
	unsigned char u_c;
	         char s_c;
} CMD_BYTE;


#define PUT_CMD_DWORD(cmd, value) do{\
					  cmd.by.B[0] = (unsigned char)( value      & 0xFF);\
					  cmd.by.B[1] = (unsigned char)((value>>8)  & 0xFF);\
					  cmd.by.B[2] = (unsigned char)((value>>16) & 0xFF);\
					  cmd.by.B[3] = (unsigned char)((value>>24) & 0xFF);\
				  }while(0)

#define PUT_CMD_WORD(cmd, value) do{\
					 cmd.by.B[0] = (unsigned char)( value&0xFF);\
					 cmd.by.B[1] = (unsigned char)((value>>8)&0xFF);\
				  }while(0)		  					

#define PUT_CMD_BYTE(cmd, value)  do{               \
	                 cmd.by.B[0] = (unsigned char)(value & 0xFF);  \
}while(0)

#define PUT_CMD_CHECKSUM(pCmd, cmdLen, checkSum)  do{          \
                         pCmd[cmdLen-2] = (unsigned char)( checkSum       & 0xFF);  \
                         pCmd[cmdLen-1] = (unsigned char)((checkSum >> 8) & 0xFF);  \
}while(0)

#define GET_CMD_DWORD(cmd, value)  do{            \
	value  = ( unsigned long)cmd.by.B[0];         \
	value |= ((unsigned long)cmd.by.B[1])<<8;     \
	value |= ((unsigned long)cmd.by.B[2])<<16;    \
	value |= ((unsigned long)cmd.by.B[3])<<24;    \
}while(0)

#define GET_CMD_WORD(cmd, value)  do{            \
	value  = ( unsigned short)cmd.by.B[0];       \
	value |= ((unsigned short)cmd.by.B[1])<<8;   \
}while(0)

#define GET_CMD_BYTE(cmd, value)  do{     \
	value = cmd.by.B[0];                  \
}while(0)

#define GET_CMD_CHECKSUM(pCmd, cmdLen, checkSum)   do{\
						  checkSum  =  (unsigned short)pCmd[cmdLen-2];\
						  checkSum |= ((unsigned short)pCmd[cmdLen-1])<<8;\
					   }while(0)


/* define checksum length */
#define CMD_CHECKSUM_LEN                     2

/* define command type */
#define CMD_TYPE_LOGIN                    0x01
#define CMD_TYPE_START_UPLOAD_PHOTO       0x02
#define CMD_TYPE_UPLOAD_PHOTO_SEG         0x03
#define CMD_TYPE_END_UPLOAD_PHOTO         0x04
#define CMD_TYPE_LOGOUT                   0x05

#define CMD_TYPE_ACK_LOGIN                0x81
#define CMD_TYPE_ACK_START_UPLOAD_PHOTO   0x82
#define CMD_TYPE_ACK_UPLOAD_PHOTO_SEG     0x83
#define CMD_TYPE_ACK_END_UPLOAD_PHOTO     0x84

					   
/* define command header*/    
typedef ARM_PACKED struct
{
	CMD_WORD  len;                  // Command frame totoal length(from itself to checksum)
	CMD_WORD  cameraID;             // Camera ID which is got by login ack from remote server
	CMD_BYTE  type;                 // Command type
	CMD_WORD  seq;                  // Command sequence (0x0000~0xFFFF)
}cmdHeader;

/* define "login" command body */
typedef ARM_PACKED struct 
{
	CMD_WORD  hwModel;              // Hardware model, 2 bytes
	CMD_WORD  swVersion;            // SW version, 2 bytes
	CMD_BYTE  snLen;                // Serial number: SIM card number or IMEI
#ifdef SUPPORT_ARRAY_ZERO_LENGTH            
	CMD_BYTE  sn[0];
#endif
}cmdBodyLogin;

/* define "login ack" command body */
typedef ARM_PACKED struct 
{
	CMD_WORD  cmdPeerSeq;         // Command sequence got from login command header
	CMD_WORD  cameraID;           // Allocated by sn by remote server
}cmdBodyLoginAck;

/* define "start to transfer photo" command body */
typedef ARM_PACKED struct \
{
	CMD_DWORD  fileOffset;           // The file offset which will be written
	CMD_DWORD  fileLen;              // The file length 
	CMD_BYTE   fileNameLen;           // The file name length 
#ifdef SUPPORT_ARRAY_ZERO_LENGTH        
	CMD_BYTE   fileName[0];
#endif
}cmdBodyTransStart;

/* define "Start to transfer photo ACK" command body */
typedef ARM_PACKED struct 
{
	CMD_WORD   cmdPeerSeq;           // Command sequence got from login command header
	CMD_DWORD  fileOffset;           // The availble file offset which will be written
	CMD_DWORD  fileLen;              // The file curent length 
	CMD_BYTE   fileNameLen;          //  The file name length 
#ifdef SUPPORT_ARRAY_ZERO_LENGTH           
	CMD_BYTE   fileName[0];
#endif
}cmdBodyTransStartAck;

/* define "Transfer 1 segment photo" command body*/
typedef ARM_PACKED struct 
{
	CMD_DWORD  fileOffset;           // Current data file offset
	CMD_WORD   segLen;               // Segment length field len
#ifdef SUPPORT_ARRAY_ZERO_LENGTH           
	CMD_BYTE   segData[0];
#endif
} cmdBodyTrans1Seg;

/* define "Transfer 1 segment photo ACK" command body */
typedef ARM_PACKED struct 
{
	CMD_WORD   cmdPeerSeq;         // Command sequence got from login command header
	CMD_DWORD  fileOffset;         // current file offset
	CMD_DWORD  fileLen;            // The file curent length 
} cmdBodyTrans1SegAck;

/* define "end to Transfer  photo" command body */
typedef ARM_PACKED struct 
{
	CMD_DWORD  fileOffset;         // Current data file offset
	CMD_DWORD  fileLen;            // The file curent length 
} cmdBodyTransEnd;

/* define "End to Transfer  photo ACK" command body */
typedef ARM_PACKED struct 
{
	CMD_WORD   cmdPeerSeq;       // Command sequence got from login command header
	CMD_DWORD  fileOffset;       // Current data file offset
	CMD_DWORD  fileLen;          // The file curent length 
} cmdBodyTransEndAck;


/* login */
typedef ARM_PACKED struct 
{
	cmdHeader     Head;
	cmdBodyLogin  Body;
}cmdLogin;

/* login ACK */
typedef ARM_PACKED struct 
{
	cmdHeader        Head;
	cmdBodyLoginAck  Body;
}cmdLoginAck;

/* logout */
typedef ARM_PACKED struct 
{
	cmdHeader  Head;
}cmdLogout;


/* photo data transfer start */
typedef ARM_PACKED struct 
{
	cmdHeader          Head;
	cmdBodyTransStart  Body;
}cmdTransStart;

/* photo data transfer start ACK */
typedef ARM_PACKED struct 
{
	cmdHeader             Head;
	cmdBodyTransStartAck  Body;
}cmdTransStartAck;


/* photo data transfer 1 segment */
typedef ARM_PACKED struct 
{
	cmdHeader         Head;
	cmdBodyTrans1Seg  Body;
}cmdTrans1Seg;

/* photo data transfer 1 segment ACK */
typedef ARM_PACKED struct 
{
	cmdHeader            Head;
	cmdBodyTrans1SegAck  Body;
}cmdTrans1SegAck;

/* photo data transfer end */
typedef ARM_PACKED struct 
{
	cmdHeader        Head;
	cmdBodyTransEnd  Body;
}cmdTransEnd;

/* photo data transfer end ACK */
typedef ARM_PACKED struct 
{
	cmdHeader           Head;
	cmdBodyTransEndAck  Body;
}cmdTransEndAck;


int    remoteServer_login(int fd);
int    remoteServer_loginAckRes(INT8U *pBuf, int bufLen);
int    remoteServer_logout(int fd);

int    cmdBuild_login(INT8U* pBuf, int bufLen, INT8U* pHwMode, INT8U* pSwVer, char* sn);
int    cmdBuild_head(cmdHeader *pHead, int cmdLen, INT8U cmdType);
INT16U checkSum_calc(INT8U* pBuf, int bufLen);
bool   checkSum_verify(INT8U *pBuf, int cmdLen);
int    cmdHeader_resolve(INT8U *pBuf, int *pCmdLen, INT16U *pID ,INT8U *pCmdType, INT16U *pCmdSeq);

int    cmdBuild_transStart(INT8U* pBuf, int bufLen, int fileOffset, int  fileLen, char* fileName, 
	                       int fileNameLen);
int    cmdBuild_trans1Seg(INT8U* pBuf, int bufLen, int fileOffset, int SegLen, INT8U* segData);
int    cmdBuild_transEnd(INT8U* pBuf, int bufLen, unsigned long fileOffset, unsigned long  fileLen);
int    cmdBuild_logout(INT8U* pBuf, int bufLen);

void   transData_initStruct(void);
int    transData_start(int fd, int fileOffset,int fileLen);
int    transData_startAckRes(INT8U * pBuf, int bufLen, int *pCmdPeerSeq, int* pFileOffset);
int    transData_1Seg(int fd, int fileOffset,int SegLen, INT8U* segData);
int    transData_1SegAckRes(INT8U *pBuf, int bufLen, int *pCmdPeerSeq, int *pFileOffset, int *pFileLen);
int    transData_end(int fd, int fileOffset,int fileLen);
int    transData_endAckRes(INT8U *pBuf, int bufLen, int *pCmdPeerSeq, int* pFileOffset, int* pFileLen);


#endif    


