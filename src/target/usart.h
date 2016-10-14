/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               systick.h
** Descriptions:            None
**
**--------------------------------------------------------------------------------------------------------
** Created by:              Aric
** Created date:            2015-02-19
** Version:                 v0.9
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/
#ifndef __USART_APP_H
#define __USART_APP_H 

#include <includes.h>
#include "em_usart.h"

#define OS_UCOSII_USED      1


/* Macros for specifying which UART to use. */
#define SD_UART0            0
#define SD_UART1            1
#define SD_USART0           2
#define SD_USART1           3
#define SD_USART2           4

#define SD_UARTS_NUM        1                 // 


typedef enum 
{
    SD_MODE_UART  = 0x0,
    SD_MODE_USART = 0x1,
    SD_MODE_IR    = 0x2
} enumSD_MODE;


typedef enum 
{
    ringBuffer_EMPTY = 0x0,
    ringBuffer_DATA  = 0x1,
    ringBuffer_FULL  = 0x2
} enumRingBuffer_state;
 

typedef struct USART_DATASTRUCT
{
//  INT16U        data_mode;
    INT32U        base_address;

    /* The following elements should generic accross other platforms. */
    INT16U        com_port;
//  INT16U        data_bits;
//  INT16U        stop_bits;
//  INT16U        parity;
    INT16U        baud_rate;
//  INT16U        rx_vector;
//  INT16U        tx_vector;

	/*------- Begin Port Specific Section --------------*/
//  INT16U        ER_vector;
//  INT16U        RX_vector;
//  INT16U        TX_vector;
//  INT16U        BK_vector;

	/*------- Begin Port Specific Section --------------*/
//  INT16U        driver_options;
    INT16U        rx_buffer_size;       // rx buffer size 
    INT16U        tx_buffer_size;       // tx buffer size 

//  volatile INT16U  parity_errors;
//  volatile INT16U  frame_errors;
//  volatile INT16U  overrun_errors;
//  volatile INT16U  busy_errors;
//  volatile INT16U  general_errors;

	bool             rx_enable;             // .
	bool             rx_strMode;            // .
	volatile INT8U   rx_strIdChar0;
	volatile INT8U   rx_strIdChar1;
	bool             rx_strIdChar0Match;
	bool             rx_strIdChar1Match;

    char             *rx_buffer;
    INT16U           rx_buffer_read;
    volatile INT16U  rx_buffer_write;
//  volatile int     rx_buffer_status; 

    /* All of the following elements are required by PPP, do not modify. */
//  INT16U           communication_mode;
    char             *tx_buffer;
    volatile INT16U  tx_buffer_read;
    INT16U           tx_buffer_write;
//  volatile int     tx_buffer_status; 

} USART_PORT;


extern struct USART_DATASTRUCT *uart0;


extern void   UART0_appInit(INT16U baudrate);
extern void   UART0_init_dataStruct(void);
extern void   USART_tx_bufRst(USART_PORT *uart);

extern bool   USART_putchar(USART_PORT *uart, char ch);
extern void   USART_putstr(USART_PORT *uart, char *str);
extern void   USART_putbinary(USART_PORT *uart, char *str, INT16U dataLen);
extern void   USART_putbinary_long(USART_PORT *uart, char *str, INT32U dataLen);
extern bool   USART_getchar(USART_PORT *uart, char* ch);
extern INT16U USART_getDataNum(USART_PORT *uart);
extern bool   USART_getBufStat(USART_PORT *uart);


extern void   USART_rx_enable(USART_PORT *uart, bool enable);
extern void   USART_rx_strModeSet(USART_PORT *uart, bool enable);
extern void   USART_rx_strIdSet(USART_PORT *uart, char* strID);
extern bool   USART_rx_strIdIsVld(USART_PORT *uart);
extern void   USART_rx_hwClr(USART_PORT *uart);
extern void   USART_rx_bufClr(USART_PORT *uart, INT16U bufLen);
extern void   USART_rx_bufRst(USART_PORT *uart);


/*
#define MAX_BUFF_LEN     1024    // 
typedef struct    // 
{      
    uint   data; 
} DATA_BUFF_T;  

typedef struct    //
{      
    uint         readPos;     
    uint         writePos;      
    DATA_BUFF_T  DATA[MAX_BUFF_LEN]; 
} DATAS_BUFF_T;  

static DATAS_BUFF_T   Buff0;    // 

Buff0.readPos = 0; 
Buff0.writePos = 0;  

BOOL HaveData_Buff(void) 
{      
    if(Buff0.readPos != Buff0.write.Pos)     
    {          
        return 1;     
    }     
    else    
    {          
        return 0;     
    }  
}

BOOL WriteData(uint data) 
{      
	uint nextPos;      
	nextPos = (Buff0.writePos + 1) % MAX_BUFF_LEN;     
	if(nectPos != Buff0.readPos) 
	{
        Buff0.DATA[writePos].data = data;         
		Buff0.writePos = nextPos;         
		return 1;     
	}     
	else     
	{          
		return 0;     
	} 
}

uint ReadData(void)  
{      
	uint dataTemp;      
	if(HaveData_Buff() == 1)     
	{          
		dataTemp = Buff0.DATA[ReadPos].data;         
		ReadPos = (ReadPos + 1) % MAX_BUFF_LEN;     
	}      
	return  dataTemp; 
} 






*/


#endif
/************************************************************************************
 ** END FILE
 ************************************************************************************/






























