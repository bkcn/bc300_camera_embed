/****************************************************************
 NAME: xModem.h
 DESC: 
 HISTORY:
 Mar.17.2006 Walt 
 ****************************************************************/
 
#ifndef __XMODEM_1K_H
#define __XMODEM_1K_H

#define XMODEM_1K_TEST_SEND
#define XMODEM_1K_TEST_RECEIVE


extern INT32S xmodemReceive (INT8U *dest, INT32U destsz, bool crc_enable);
extern INT32S xmodemTransmit(INT8U *src,  INT32U srcsz,  bool xmodem1k_enable);
bool xmodem_1K_test_receive(bool crc_enable);
bool xmodem_1K_test_send(bool xmodem1k_enable);


#endif /*__XMODEM_1K_H*/
