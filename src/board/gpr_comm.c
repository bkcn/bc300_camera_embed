#include "..\target\target.h"
#include "..\board\board.h"

//---------------------------------------------------
//- add to the buffer's tail, low byte first.
//---------------------------------------------------
void checksum_add(char *buf, INT32U bufLen)
{
    INT32U i;
	INT16U checksum;

    checksum = 0;

    for(i=0; i<bufLen; i++)	        // 15
	{
        checksum += buf[i];
	}  
	buf[bufLen]   =  checksum & 0xff;
	buf[bufLen+1] = (checksum >> 8) & 0xff;
}

//---------------------------------------------------
//- just check, low byte first.
//---------------------------------------------------
INT8U checksum_verify(char *buf, INT32U bufLen)
{
    INT32U i;
	INT16U checksumCalc;
	INT16U checksumRecv;

	if(bufLen >= 4)
	{
	    checksumRecv = buf[bufLen-2] + (buf[bufLen-1] << 8);   // 
        checksumCalc = 0;

	    for(i=0; i<bufLen-2; i++)	       // 15
		{
	        checksumCalc += buf[i];	   // 
		} 
		
		if(checksumCalc == checksumRecv) 
		{
		    return true;
		}
	}

	return false;
}


/* remote server login */
void cm_takePhotoAck_fraBulid(char *fraBuf)
{
	struct cm_fraTakePhotoAck *pBuf;

	pBuf = (struct cm_fraTakePhotoAck *)fraBuf;	
		
	pBuf->fraHead[0] = CM_ACK_FRAHEAD_0;                         // fraType=0x01; 
	pBuf->fraHead[1] = CM_ACK_FRAHEAD_1;                         // fraType=0x01; 
	pBuf->fraType    = CM_ACK_FRATYPE_TAKE_PHOTO;                // fraType=0x01; 
//  pBuf->dataLen.word_u = bigToLittle16((INT16U)1);
    pBuf->dataLen.word_u = 1;
	pBuf->ack = cm_ack;                                      // fraType=0x01; 
	checksum_add(fraBuf, sizeof(struct cm_fraTakePhotoAck)-4);   // 
	pBuf->fraTail[0] = CM_ACK_FRATAIL_0;                         // fraType=0x01; 
	pBuf->fraTail[1] = CM_ACK_FRATAIL_1;                         // fraType=0x01; 
}
 
/* remote server logout */
void cm_getPhotoLenAck_fraBulid(char *fraBuf, INT32U fileLen)
{
	struct cm_fraGetPhotoLenAck *pBuf;

	pBuf = (struct cm_fraGetPhotoLenAck *)fraBuf;	
		
	pBuf->fraHead[0] = CM_ACK_FRAHEAD_0;                         // fraType=0x01; 
	pBuf->fraHead[1] = CM_ACK_FRAHEAD_1;                         // fraType=0x01; 
	pBuf->fraType    = CM_ACK_FRATYPE_PHOTO_LEN;                 // fraType=0x01; 
	pBuf->dataLen.word_u  = 4;  
//	pBuf->fileLen.dword_u = bigToLittle32(fileLen);              // fraType=0x01; 
	pBuf->fileLen.dword_u = fileLen;                             // fraType=0x01; 
	checksum_add(fraBuf, sizeof(struct cm_fraGetPhotoLenAck)-4);
	pBuf->fraTail[0] = CM_ACK_FRATAIL_0;                         // fraType=0x01; 
	pBuf->fraTail[1] = CM_ACK_FRATAIL_1;                         // fraType=0x01; 
}

/* file transmit start request */
void cm_getPhotoDataAck_fraBulid(char *fraBuf, INT16U fileOffset, INT16U *fraLen)
{
	struct cm_fraGetPhotoDataAck *pBuf;
	INT16U i;
	INT16U uiTmpVal;

	pBuf = (struct cm_fraGetPhotoDataAck *)fraBuf;	
		
	pBuf->fraHead[0] = CM_ACK_FRAHEAD_0;                   // fraType=0x01; 
	pBuf->fraHead[1] = CM_ACK_FRAHEAD_1;                   // fraType=0x01; 
	
	if(fileOffset < (photobufLen-FILE_DATA_LEN_BLOCK))	   // 1024 < (2048-1024)
	{
		pBuf->fraType = CM_ACK_FRATYPE_PHOTO_DATA;               // fraType=0x01; 
//		pBuf->dataLen.word_u = bigToLittle16(FILE_DATA_LEN_BLOCK);  // 
		pBuf->dataLen.word_u = FILE_DATA_LEN_BLOCK;                 // 
		
//		pBuf->fileOffset.dword_u = bigToLittle32(fileOffset);       // fraType=0x01; 
		pBuf->fileOffset.dword_u = fileOffset;                      // fraType=0x01; 
	    for(i=0; i<FILE_DATA_LEN_BLOCK; i++)
		{
			pBuf->dataBuf[i] = *(photoBufPtr + fileOffset + i);
		}
		uiTmpVal = sizeof(struct cm_fraGetPhotoDataAck);
        *fraLen = uiTmpVal;		
	    checksum_add(fraBuf, uiTmpVal-4);		
	}
	else
	{
		uiTmpVal = photobufLen - fileOffset;
		
		pBuf->fraType = CM_ACK_FRATYPE_PHOTO_DATA_LAST;         // fraType=0x01; 
//		pBuf->dataLen.word_u = bigToLittle16(uiTmpVal);         // 
		pBuf->dataLen.word_u = uiTmpVal;                        // 
//		pBuf->fileOffset.dword_u = bigToLittle32(fileOffset);   // fraType=0x01; 
		pBuf->fileOffset.dword_u = fileOffset;                  // fraType=0x01; 
	    for(i=0; i<uiTmpVal; i++)
		{
			pBuf->dataBuf[i] = *(photoBufPtr + fileOffset + i);
		}		
		
		uiTmpVal = sizeof(struct cm_fraGetPhotoDataAck)-FILE_DATA_LEN_BLOCK+uiTmpVal;	
        *fraLen = uiTmpVal;		
	    checksum_add(fraBuf, uiTmpVal-4);
	}
	
	pBuf->fraTail[0] = CM_ACK_FRATAIL_0;         // fraType=0x01; 
	pBuf->fraTail[1] = CM_ACK_FRATAIL_1;         // fraType=0x01; 
}
      



