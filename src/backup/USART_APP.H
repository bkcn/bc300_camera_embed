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


/* Macros for specifying which UART to use. */
#define  SD_UART0            0
#define  SD_UART1            1
#define  SD_UART2            2

#define  SD_UARTS_NUM        1              // 
 
 

#define UART0_TxDataBuffSize   (2*1024L)    // 2KB.
#define UART0_RxDataBuffSize   (2*1024L)    // 2KB.


typedef struct SD_INIT_STRUCT
{
    INT32U        data_mode;
    INT32U        base_address;

    /* The following elements should generic accross other platforms. */
    /* NU_SEMAPHORE  *sd_semaphore; */
    INT32U        com_port;
    INT32U        data_bits;
    INT32U        stop_bits;
    INT32U        parity;
    INT32U        baud_rate;
    INT32U        vector;

	/*------- Begin Port Specific Section --------------*/
//  INT32U        ER_vector;
//  INT32U        RX_vector;
//  INT32U        TX_vector;
//  INT32U        BK_vector;

	/*------- Begin Port Specific Section --------------*/
    INT32U        driver_options;
    INT32U        rx_buffer_size;     // rx buffer size 
    INT32U        tx_buffer_size;     // tx buffer size 

//  volatile INT32U  parity_errors;
//  volatile INT32U  frame_errors;
//  volatile INT32U  overrun_errors;
//  volatile INT32U  busy_errors;
//  volatile INT32U  general_errors;

    char             *rx_buffer;
    INT32U           rx_buffer_read;
    volatile INT32U  rx_buffer_write;
    volatile int     rx_buffer_status; 

    /* All of the following elements are required by PPP, do not modify. */
    INT32U           communication_mode;
    char             *tx_buffer;
    volatile INT32U  tx_buffer_read;
    INT32U           tx_buffer_write;
    volatile int     tx_buffer_status; 

} SD_PORT;


typedef enum
{
    IsIdle = 0,     // . 
    IsASCII = 1,    // . 
    IsBINARY = 2,	// . 
    IsBusy = 3,     // .   	
} USART_TxRxMode_TypeDef; 


extern volatile INT8U  UART0_TxDataBuff[UART0_TxDataBuffSize];  // .
extern volatile INT8U  UART0_RxDataBuff[UART0_RxDataBuffSize];  // .
extern volatile INT16U UART0_TxDataNumCnt;
extern volatile INT16U UART0_TxDataNumLen;
extern volatile INT8U  UART0_TxDataOver;
extern volatile INT16U UART0_RxDataNumCnt;
extern volatile INT16U UART0_RxDataNumLen;

extern volatile INT8U  UART0_ModeIsASCII;       //
extern volatile INT8U  UART0_RcvStrIdChar0;     //
extern volatile INT8U  UART0_RcvStrIdChar1;     //
extern volatile INT8U  UART0_RcvStrIdChar0Flg;  //
extern volatile INT8U  UART0_RcvStrIdFlg;       // 

extern USART_TxRxMode_TypeDef UART0_TxMode;     // 
extern USART_TxRxMode_TypeDef UART0_RxMode;     // 


extern void UART0_AppInit(USART_Enable_TypeDef enableTmp, INT16U baudrateTmp);
extern void UART0_SendStrASCII(char* SndStr);
extern void UART0_SendStrBinary(char* SndStr, INT8U StrLen) ;
extern USART_TxRxMode_TypeDef UART0_SendStrStat(void);
extern void UART0_RcvEn (USART_TxRxMode_TypeDef RcvMode, bool RcvEnable);
extern INT8U UART0_RcvStrWithID (USART_TxRxMode_TypeDef RcvMode, char* StrID, INT8U TimOut);
extern void UART0_RcvBuffClr(INT8U BuffLen);
extern void UART0_RcvBuffRst(void);
extern void UART0_RcvStrIdSet(char* StrID);



#endif
/************************************************************************************
 ** END FILE
 ************************************************************************************/






























