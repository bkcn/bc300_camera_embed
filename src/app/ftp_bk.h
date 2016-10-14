#ifndef __FTP_BK_H_
#define __FTP_BK_H_


int FtpPutFile_BK(char* apnName, char* userName, char* password, char* serverName, 
				  int  ftpPort, unsigned char* buf, int bufLen, int* curPos );

#endif


