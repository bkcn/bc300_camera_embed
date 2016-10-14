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
#define SD_UART0            0
#define SD_UART1            1
#define SD_UART2            2

#define SD_UARTS_NUM        1              // 
 
 
//#define SD_UART0_BASE       0xFFFD0000      /* Serial A on board */
//#define SD_UART1_BASE       0xFFFCC000      /* Serial B on board */


/* Defines to determine communication mode */  
#define SERIAL_MODE         0
#define SERIAL_MOUSE        3

#define SD_UART0_VECTOR     2
#define SD_UART1_VECTOR     3
#define SD_USART0_VECTOR    16
#define SD_USART1_VECTOR    16
#define SD_USART2_VECTOR    16
 
/* Defines for fields and bits of interrupt control and mask registers. */
#define SD_MODE_NORMAL              0x40    /* 0=Normal -- 1=IR Mode    */
#define SD_IMR_MASTER_MASK         (1 << 26) /* Global IRQ Enable       */
#define SD_IMR_UART1_TX_MASK       (1 << 3)  /* UART 1 Tx IRQ is Bit 3  */
#define SD_IMR_UART2_TX_MASK       (1 << 2)  /* UART 2 Tx IRQ is Bit 2  */
#define SD_IMR_UART1_RX_MASK       (1 << 7)  /* UART 1 Rx IRQ is Bit 7  */
#define SD_IMR_UART2_RX_MASK       (1 << 6)  /* UART 2 Rx IRQ is Bit 6  */


#define UART0_BUFFER_SIZE   (4 * 1024L)     //
#define UART1_BUFFER_SIZE    256            //
#define USART0_BUFFER_SIZE   256            //
#define USART1_BUFFER_SIZE   256            // 
#define USART2_BUFFER_SIZE   256            // 
 
 

typedef struct USART_DATA_STRUCT
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

} USART_DATA;


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






























