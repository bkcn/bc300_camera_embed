#ifndef __CAMERA_COMM_H
#define __CAMERA_COMM_H

#include "..\config\datatypes.h"


// #define SIMU_CM_SEND_DATA                  1

#define FRA_CHECKSUM_LEN                   2
#define FILE_DATA_LEN_BLOCK             1024


#define CM_FRAHEAD_0                    0x23
#define CM_FRAHEAD_1                    0x23
#define CM_FRATYPE_TAKE_PHOTO           0x01
#define CM_FRATYPE_GET_PHOTO_LEN        0x02
#define CM_FRATYPE_GET_PHOTO_DATA       0x03
#define CM_FRATYPE_SHUT_DOWN            0x04
#define CM_FRATAIL_0                    0x0d
#define CM_FRATAIL_1                    0x0a

#define CM_ACK_FRAHEAD_0                0x23
#define CM_ACK_FRAHEAD_1                0x23
#define CM_ACK_FRATYPE_TAKE_PHOTO       0x01
#define CM_ACK_FRATYPE_PHOTO_LEN        0x02
#define CM_ACK_FRATYPE_PHOTO_DATA       0x03
#define CM_ACK_FRATYPE_PHOTO_DATA_LAST  0x13
#define CM_ACK_FRATAIL_0                0x0d
#define CM_ACK_FRATAIL_1                0x0a


enum cm_stat
{
    CM_ERROR      = 0,    // 
	CM_SUCCESS    = 1     //	 	
};

enum cm_ack
{
    CM_ACK_ERROR  = 0,    // 
	CM_ACK_OK     = 1,    //	 	
	CM_ACK_BUSY   = 2  
};



#pragma pack(push)   //
#pragma pack(1)      // 1 bytes

/* define comand DWORD union type */
/* high byte first */
union DWORD_32
{
    struct 
	{
        INT8U  byte[4];
	}byte4;

    INT32U dword_u;
    INT32S dword_s;
};

/* define comand WORD union type */
/* high byte first */
union WORD_16 
{
    struct 
	{
        INT8U  byte[2];
	}byte2;

    INT16U word_u;
    INT16S word_s;
};

/* define comand WORD union type */
union BYTE_8 
{
    struct 
	{
        INT8U  byte[1];
	}byte1;

    INT8U byte_u;
    INT8S byte_s;
};

/*
#define bigToLittle16(A)              \   
    (( (INT16U)(A) & 0xff00) >> 8) |  \ 
    (( (INT16U)(A) & 0x00ff) << 8)    \


#define bigToLittle32(A)\  
    (( (INT32U)(A) & 0xff000000) >> 24) | \  
    (( (INT32U)(A) & 0x00ff0000) >> 8)  | \  
    (( (INT32U)(A) & 0x0000ff00) << 8)  | \  
    (( (INT32U)(A) & 0x000000ff) << 24)   


#define littleToBig16(A)\   
    (( (INT16U)(A) & 0xff00) >> 8) |\ 
    (( (INT16U)(A) & 0x00ff) << 8)

#define littleToBig32(A)\   
    (( (INT32U)(A) & 0xff000000) >> 24) | \  
    (( (INT32U)(A) & 0x00ff0000) >> 8)  | \  
    (( (INT32U)(A) & 0x0000ff00) << 8)  | \  
    (( (INT32U)(A) & 0x000000ff) << 24)   
*/

struct cm_fraTakePhoto
{
    INT8U fraHead[2];                  // command type
    INT8U fraType;         	           // command type
	union WORD_16 dataLen;             //
	INT8U para;
	union WORD_16 checksum;            //
    INT8U fraTail[2];                  // command type
};

struct cm_fraTakePhotoAck
{
    INT8U fraHead[2];                  // command type
    INT8U fraType;         	           // command type
	union WORD_16 dataLen;             //
	INT8U ack;
	union WORD_16 checksum;            //
    INT8U fraTail[2];                  // command type
};

struct cm_fraGetPhotoLen
{
    INT8U fraHead[2];                  // command type
    INT8U fraType;         	           // command type
	union WORD_16 dataLen;             //
	union WORD_16 checksum;            //
    INT8U fraTail[2];                  // command type
};

struct cm_fraGetPhotoLenAck
{
    INT8U fraHead[2];                  // command type
    INT8U fraType;         	           // command type
	union WORD_16  dataLen;             //
	union DWORD_32 fileLen;            //	 
	union WORD_16  checksum;            //
    INT8U fraTail[2];                  // command type
};

struct cm_fraGetPhotoData
{
    INT8U fraHead[2];                   // command type
    INT8U fraType;         	            // command type
	union WORD_16  dataLen;             //
	union DWORD_32 fileOffset;          //	 
	union WORD_16  checksum;            //
    INT8U fraTail[2];                   // command type
};

struct cm_fraGetPhotoDataAck
{
    INT8U fraHead[2];                   // command type
    INT8U fraType;         	            // command type
	union WORD_16  dataLen;             //
	union DWORD_32 fileOffset;          //
	INT8U dataBuf[FILE_DATA_LEN_BLOCK];	// 2+1+2+4+1024+2+2=1037; 
	union WORD_16  checksum;            //
    INT8U fraTail[2];                   // command type
};
				  
struct cm_fraShutdown
{
    INT8U fraHead[2];                   // command type
    INT8U fraType;         	            // command type
	union WORD_16 dataLen;             //
	union WORD_16  checksum;            //
    INT8U fraTail[2];                   // command type
};
#pragma pack(pop)


void  checksum_add(char *buf, INT32U bufLen);
INT8U checksum_verify(char *buf, INT32U bufLen);

void cm_takePhotoAck_fraBulid(char *fraBuf);
void cm_getPhotoLenAck_fraBulid(char *fraBuf, INT32U fileLen);
void cm_getPhotoDataAck_fraBulid(char *fraBuf, INT16U fileOffset, INT16U *fraLen);



#endif


