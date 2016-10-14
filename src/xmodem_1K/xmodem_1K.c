/*
    Copyright 2001, 2002 Georges Menie (www.menie.org)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
   INT32U _inbyte(INT16U timeout); // msec timeout
   void _outbyte(INT32U c);
*/
 
#include "..\target\target.h"
#include "..\board\board.h" 


// #include "..\config\datatypes.h"
// #include "usart.h"
// #include "crc16.h"


#define SOH   0x01   // XMODEM数据头.
#define STX   0x02   // XMODEM-1K数据头.
#define EOT   0x04   // 发送结束.end of transmit. 
#define ACK   0x06   // 认可响应.
#define NAK   0x15   // 不认可响应.
#define CAN   0x18   // 撤销传送.cancel.
#define CTRLZ 0x1A   // 填充字符.

// #define TIMEOUT_OS_1S       100L       // time out: 1S.
#define RETRANS_MAX          6        // 


static INT8U xbuff[1030];    /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */


const char testDataStr[]=
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 1.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 2.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 3.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 4.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 5.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 6.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 7.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 8.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 9.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 10.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 11.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 12.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 13.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 14.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 15.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 16.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 17.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 18.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 19.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 20.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 21.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 22.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 23.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 24.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 25.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 26.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 27.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 28.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 29.
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',   // 30.
	'\r', '\n', '\0' 
};


static INT16S _inbyte(INT16U timeOuttmSec)                    // unit(timeout): 10msec.
{
    char  ch;
	INT8U i;
	
	for(i=0; i<timeOuttmSec; i++)
	{
        if(true == USART_getchar(uart0, &ch))
		{
            return (INT8U)ch;
		}	
		else
		{
            #ifdef OS_UCOSII_USED			
		    OSTimeDlyHMSM(0, 0, 0, 10);     // 10mS.   
			#else
			delay_ms(10); 
            #endif									
		}	
    }

	return -1;             // 
}

static void _outbyte(INT8U ch)
{
	INT8U i;
		
	for(i=0; i<10; i++)
	{
		if(true == USART_putchar(uart0, (char)ch))	 // tranemit success.
		{
			break; 
		}
		else                                         // fail.
		{
			delay_us(10); 
		}
	}
}


/*
void outbyte(INT32U c)       // for printable chars only
{
	static char prev = 0;
	if (c < ' ' && 
		c != '\r' && 
	    c != '\n' && 
	    c != '\t' && 
	    c != '\b')
	{
		return;
	}
	if (c == '\n' && prev != '\r') 
	{
		_outbyte('\r');
	}
	_outbyte(c);
	prev = c;
}
*/

static bool crc_check(INT8U crc_enable, const INT8U *buf, INT16U bufLen)
{
	INT16U i;
	INT16U crc_recv;
	INT16U crc_calc;
	
	INT8U  check_sum;           // check type: sum.

    if(crc_enable)                // check type: crc.
	{
        crc_calc = crc16_ccitt(buf, bufLen);
        crc_recv = (buf[bufLen]<<8) + buf[bufLen+1];
        if(crc_calc == crc_recv)
		{
            return true;
		}
    }
    else 
	{
		check_sum = 0;

        for(i=0; i<bufLen; ++i) 
		{
            check_sum += buf[i];
        }
        if(check_sum == buf[bufLen])       // the last byte is check_sum.
		{
            return true;
		}
    }

    return false;
}

static void flush_input(void)              // .
{
    while(_inbyte(TIMEOUT_OS_02S) >= 0);   // until no data and time out.
}

INT32S xmodemReceive(INT8U *dest, INT32U destsz, bool crc_enable)
{
//  INT8U xbuff[1030];        /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */

    INT8U *pStr;
    INT8U trychar;            // . 
	
    INT16S ch;

//  register INT16U count;    // 1024/128.
    INT32S count;

	INT16U i;                 // retry times: 16.
	INT32U j, recvLen;
    INT16U bufsz;
    INT8U  packetno;          // .

    INT16U retrans;           // 25.

	packetno = 1;             // .
	recvLen = 0;               // . 
	retrans = RETRANS_MAX;    // 25.
	
	if(crc_enable)
	{
	    trychar = 'C';        // crc. 
	}
	else
	{
	    trychar = NAK;        // cheak sum. 
	}
	
    for(;;) 
	{                                      // step 0: tranemit 'C'.
        for(i=0; i<16; i++)                // retry num = 16.
		{
            if(trychar)                    // send 'C' or NAK.
			{
				_outbyte(trychar);
			}
			
			ch = _inbyte(TIMEOUT_OS_2S);         // first data from first frame.
            if(ch >= 0)                          // step 1: -1 or data (char).
			{
                switch(ch) 
				{
					case SOH: bufsz = 128;            // XMODEM.
						      goto start_recv;
					case STX: bufsz = 1024;           // XMODEM-1K.
						      goto start_recv;
					case EOT: flush_input();          //     
						      _outbyte(ACK);          // 
						      return recvLen;          /* normal end */
					case CAN: // if((ch = _inbyte(TIMEOUT_OS_1S)) == CAN)      // confirm.
							  {
								  flush_input();
								  _outbyte(ACK);
								  return -1;          /* canceled by remote */
							  }
//							  break;
					default:  break;
                }
            }
        }

        if(trychar == 'C') 
        { 
			trychar = NAK;     // cheak sum, retry again. 
			continue; 
		}

        flush_input();
        _outbyte(CAN);
        _outbyte(CAN);
        _outbyte(CAN);
        return -2;                         /* sync error */

	start_recv:                            // step 2: before receive data.
        trychar = 0;
        pStr = xbuff;                      // .
        *pStr++ = ch;                      // SOH/STX.

		// step 3: receive data.
        for(j=0; j<(bufsz+3+(crc_enable?1:0)); j++)     // 1+2+1024/128+1/2.
		{    
			ch = _inbyte(TIMEOUT_OS_1S);
            if(ch < 0) 
			{
				goto reject;
			}
            *pStr++ = ch;
        }
		
        // step 4: check data.
        if(xbuff[1] == (INT8U)(~xbuff[2]) &&          
          ((xbuff[1] == packetno) || (xbuff[1] == (INT8U)packetno-1)) &&
            crc_check(crc_enable, &xbuff[3], bufsz))    // data and cheak.
		{
            if(xbuff[1] == packetno)    
			{
                count = destsz - recvLen;
                if(count >= bufsz) 
				{
					count = bufsz;
				}
                if(count > 0) 
				{
                    memcpy (&dest[recvLen], &xbuff[3], count);
                    recvLen += count;

					packetno++;                    // receaived one packet.
					retrans = RETRANS_MAX+1;       // retrans plus  1.
                }
				else                               // receaived enough btyes.
				{
					_outbyte(CAN);
					_outbyte(CAN);
					_outbyte(CAN);
					flush_input();
					return recvLen;
				}
            }
            if((retrans--) <= 0)               // retrans minus 1.
			{
                flush_input();
                _outbyte(CAN);
                _outbyte(CAN);
                _outbyte(CAN);
                return -3;          /* too many retry error */
            }
            _outbyte(ACK);
            continue;
        }
    reject:                     // receive error.
        flush_input();
        _outbyte(NAK);
    }
}

INT32S xmodemTransmit(INT8U *src, INT32U srcsz, bool xmodem1k_enable)
{
// INT8U xbuff[1030];         /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */

    bool  crc_enable;         // .

    INT16S ch;
    INT16U bufsz;

	INT16U i;                 // retry times: 16.
	INT32U j, sendLen;        // . 
    INT8U  packetno;          // packetno=1-->255-->0-->255.
    INT32S count;             // data number of one packet.
	
	INT16U crc_calc;          // 
	INT8U  cheak_sum;         // 

	packetno = 1;             // .
	sendLen = 0;              // . 
	crc_enable = false;       // .
	
    for(;;) 
	{
//      for(i=0; i<16; i++) 
        for(i=64; i>0; i--) 
		{
//			ch = _inbyte(TIMEOUT_OS_2S); 
			ch = _inbyte(TIMEOUT_OS_1S); 
            if(ch >= 0) 
			{
                switch(ch) 
				{
					case 'C': crc_enable = true;
//					          _outbyte(ch);          // ack.
					          _outbyte(NAK);         // ack.					
						      goto start_trans;
					case NAK: crc_enable = false;
					          _outbyte(ch);          // ack.
						      goto start_trans;
					case CAN: // ch = _inbyte(TIMEOUT_OS_1S);
						      // if(ch == CAN) 
						      {
							      _outbyte(ACK);
								  flush_input();
								  return -1;           /* canceled by remote */
							  }
//							  break;
					default:  break;
                }
            }
        }
        _outbyte(CAN);
        _outbyte(CAN);
        _outbyte(CAN);
        flush_input();
        return -2;                 /* no sync */

        for(;;) 
		{
        start_trans:		
			if(xmodem1k_enable)
			{
				xbuff[0] = STX;
				bufsz = 1024;
			}
			else
			{
				xbuff[0] = SOH;
				bufsz = 128;
			}
			
//          xbuff[0] = SOH; bufsz = 128;
            xbuff[1] = packetno;
            xbuff[2] = ~packetno;
            count = srcsz - sendLen;
			
            if(count > bufsz) 
			{
				count = bufsz;
			}
            if(count >= 0) 
			{
                memset(&xbuff[3], 0, bufsz);
                if(count == 0) 
				{
                    xbuff[3] = CTRLZ;            // add CTRLZ at the end of data steam.
                }
                else 
				{
                    memcpy(&xbuff[3], &src[sendLen], count);
                    if(count < bufsz) 
					{
						xbuff[3+count] = CTRLZ;  // add CTRLZ at the end of data steam.
					}
                }
				
                if(crc_enable) 
				{
                    crc_calc = crc16_ccitt(&xbuff[3], bufsz);
                    xbuff[bufsz+3] = (crc_calc>>8) & 0xFF;
                    xbuff[bufsz+4] =  crc_calc     & 0xFF;
                }
                else 
				{	
					cheak_sum = 0;
					
                    for(i=3; i<(bufsz+3); i++)   // 3..3+128-1=130.
					{
                        cheak_sum += xbuff[i];
                    }
                    xbuff[bufsz+3] = cheak_sum;
                }
								
                for(i=0; i<RETRANS_MAX; i++) 
				{
//					flush_input();                                // add by ARIC, 20150312.
					
                    for(j=0; j<(bufsz+3+(crc_enable?2:1)); j++)   // 3+128/1024+2/1. 
					{
                        _outbyte(xbuff[j]);
                    }
					
					ch = _inbyte(TIMEOUT_OS_2S);
                    if(ch >= 0 ) 
					{
                        switch(ch) 
						{
							case ACK: packetno++;
								      sendLen += bufsz;
								      goto start_trans;
							case CAN: // ch = _inbyte(TIMEOUT_OS_1S);
								      // if(ch == CAN) 
								      {
									      _outbyte(ACK);
										  flush_input();
										  return -1;          /* canceled by remote */
									  }
//									  break;
							case NAK:
							default:  break;
                        }
                    }
                }
                _outbyte(CAN);
                _outbyte(CAN);
                _outbyte(CAN);
                flush_input();
                return -4;             /* xmit error */
            }
            else 
			{
                for(i=0; i<16; i++) 
			    {
                    _outbyte(EOT);
					
					ch = _inbyte(TIMEOUT_OS_1S);
                    if(ch == ACK) 
					{
						flush_input();
						return sendLen;
					}
                }
                flush_input();
                return -5;
            }
        }
    }
}


#ifdef XMODEM_1K_TEST_RECEIVE
bool xmodem_1K_test_receive(bool crc_enable)
{
    INT32S st;

    printf ("prepare to receive data with xmodem protocol now...\r\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
	USART_rx_strModeSet(uart0, false);
	USART_rx_hwClr(uart0);
	USART_rx_bufRst(uart0);
	USART_rx_enable(uart0, true);
    st = xmodemReceive((INT8U *)photodataBuff, (PHOTO_DATA_BUFF_SIZE/4+1), crc_enable);
    if (st < 0) 
	{
		printf("\r\n");
        printf("xmodem receive error: %d\r\n", st);
		return false;
    }
    else  
	{
		printf("\r\n");
        printf("xmodem received(bytes): %d/%d\r\n", st, (PHOTO_DATA_BUFF_SIZE/4+1));
		return true;
    }

//  return false;
}
#endif


#ifdef XMODEM_1K_TEST_SEND
bool xmodem_1K_test_send(bool xmodem1k_enable)
{
    INT32S st;

    printf("prepare to send data now...\r\n");

    /* the following should be changed for your environment:
       0x30000 is the download address,
       12000 is the maximum size to be send from this address
     */
//	USART_tx_bufRst(uart0);
	USART_rx_strModeSet(uart0, false);
	USART_rx_hwClr(uart0);
	USART_rx_bufRst(uart0);
	USART_rx_enable(uart0, true);
    st = xmodemTransmit((INT8U *)photodataBuff, (PHOTO_DATA_BUFF_SIZE/4+1), xmodem1k_enable);   // . 
    if(st < 0) 
	{
		printf("\r\n");
        printf("xmodem transmit error: %d\r\n", st);
		return false;
    }
    else  
	{
		printf("\r\n");
        printf("xmodem transmitted(bytes): %d/%d\r\n", st, (PHOTO_DATA_BUFF_SIZE/4+1));
		return true;
    }

//  return false;
}
#endif


/*
INT32S  XMain(void)
{
   INT32S retlen ;
   //UART0_Printf("\r\nXModem-1KCRC\r\n");
    #ifdef  CONFIG_WATCHDOG
           vResetWD();
    #endif     
    //retlen =xmodemReceive((INT8U *)ADDR_KERNEL_DOWNLOAD, 1024*512);
    retlen = xmodemTransmit((INT8U *)PhotoBufPtr, (INT32S) PhotoBufLen);

    #ifdef  CONFIG_WATCHDOG
           vResetWD();
    #endif    
   //UART0_Printf("\r\nReceive File Size=%d\r\n", retlen);
  return retlen;
}
*/


