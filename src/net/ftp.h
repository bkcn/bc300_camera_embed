#ifndef __FTP_H_
#define __FTP_H_

// int ftp_open(char* apnName, char* serverName, INT16U ftpPort, char* userName, char* password);
int GSM_ftp_putFile(char* apnName, char* serverName, INT16S ftpPort,char* userName,
	                char* password, char* filePath, char* fileName, INT8U* buf, int bufLen );


int GSM_ftp_getFile(char* apnName, char* serverName, INT16S ftpPort, char* userName, 
	                char* password, char* filePath, char* fileName, INT8U* buf, int bufLen);

	
#endif


