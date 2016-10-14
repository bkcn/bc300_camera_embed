/******************************************************************
    The socket is based on SIMCOM9000A internal TCP/IP stack
    
******************************************************************/
#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <includes.h>
#include <stdint.h>
#include "usart.h"
#include "modem.h"
#include "sim900a.h"


extern  volatile INT8U  GSM_dataTransEvt;        //

#define SOCKET_SUCCESS          0  
#define SOCKET_TCPCLOSED       -1
#define SOCKET_ERROR           -3  
#define SOCKET_NO_DATA       -269     /* None of the specified sockets were data ready */
#define SOCKET_DESCRIPTION      1     /* Record the socket number */


/* A generic catch-all for unused parameters. */
#define NU_NONE         0

/* Address family equates */
#define SK_FAM_UNSPEC   0               /* unspecified */
#define SK_FAM_LOCAL    1
#define SK_FAM_UNIX     SK_FAM_LOCAL
#define SK_FAM_IP       2               /* Internet:  UDP, TCP, etc. */
#define SK_FAM_ROUTE    17              /* Internal routing protocol */
#define SK_FAM_LINK     18              /* Link layer interface.     */


#define DELAY_OS_02S   (OS_TICKS_PER_SEC/5)    // 0.2S.       
#define DELAY_OS_05S   (OS_TICKS_PER_SEC/2)    // 0.5S.
#define DELAY_OS_1S    (OS_TICKS_PER_SEC)      // 1S.
#define DELAY_OS_2S    (OS_TICKS_PER_SEC*2)    // 2S.
#define DELAY_OS_5S    (OS_TICKS_PER_SEC*5)    // 5S.
#define DELAY_OS_10S   (OS_TICKS_PER_SEC*10)   // 10S.	
#define DELAY_OS_20S   (OS_TICKS_PER_SEC*20)   // 20S.	



/* These equates are for backwards compatability */
#define OS_FAMILY_UNIX    SK_FAM_UNIX        /* Unix */
#define OS_FAMILY_IP      SK_FAM_IP          /* Internet       - valid entry */

/* TYPE equates */
#define OS_TYPE_STREAM     0     /* stream Socket             - valid entry */
#define OS_TYPE_DGRAM      1     /* datagram Socket           - valid entry */
#define OS_TYPE_RAW        2     /* raw Socket                - valid entry */
#define OS_TYPE_SEQPACKET  3     /* sequenced packet Socket */
#define OS_TYPE_RDM        4     /* reliably delivered msg Socket */

/* PROTOCOL equates */
#define OS_PROTO_INVALID   0
#define OS_PROTO_TCP       1
#define OS_PROTO_UDP       2
#define OS_PROTO_ICMP      3

/***************************  SOCKET OPTIONS  *****************************/
/* SOCKET OPTION control flags */
#define OS_SETFLAG         1
#define OS_BLOCK           1

/* PROTOCOL LEVELS */
#define OS_SOCKET_LEVEL    0

/* Levels used in the call to NU_Setsockopt */
#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_IGMP    2
#define IPPROTO_GGP     3
#define IPPROTO_TCP     6
#define IPPROTO_EGP     8
#define IPPROTO_PUP     12
#define IPPROTO_UDP     17
#define SOL_SOCKET      100

/* Protocol used INT16S call to NU_Socket with a Raw IP socket */
#define IPPROTO_HELLO   63
#define IPPROTO_RAW     255
#define IPPROTO_OSPF    89


/* 32-bit structure containing 4-digit ip number */
struct id_struct
{
	INT8U is_ip_addrs[4];       /* IP address number */
};

struct addr_struct
{
	INT16S  family;             /* family = INTERNET */
	INT16U  port;               /* machine's port number */
	struct  id_struct id;       /* contains the 4-digit ip number for the host machine */
	char    serverName[64];     /* points to remote server */ 
	char    *name;              /* points to machine's name */
};

INT16S socket_open(INT16S family, INT16S type, INT16S protocol);
//INT16S socket_connect(INT16S socketd, struct addr_struct *servaddr, INT16S addrlen);
INT16S socket_connect(INT16S socketd);
bool   socket_established(void);
INT16U socket_availableData(void);
INT16S socket_close(INT16S socketd);

//INT16S socket_send_to(INT16S socketd, INT8U *buff, INT16S nbytes, INT16S flags, 
//                      struct addr_struct *to, INT16S addrlen);

INT16S socket_send_to(INT16S socketd, INT8U *buff, INT16S nbytes, INT16U retryNum, INT32U timeout);

//INT16U socket_recv_from(INT16S socketd, INT8U *buff, INT16U nbytes, INT16S flags, 
//                        struct addr_struct *from, INT16S *addrlen, INT32U timeout);

INT16U socket_recv_from(INT16S socketd, INT8U *buff, INT16U nbytes, INT32U timeout);

INT16S GPRS_changeParam(char* apn, char* userName, char* password);
void   socket_comm_test(INT16S family, INT16S type, INT16S protocol);
void   socket_comm_test2(INT16S socketfd);


#endif



