#include <includes.h>
#include "string.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "sdc.h"

/* Macros for specifying which UART to use. */
#define  SD_UART0            0
#define  SD_UART1            1
#define  SD_UART2            2

#define  SD_UARTS_NUM        1              // 


USART_BUFFER  USART_port[SD_UARTS_NUM];     //

SD_PORT port[SD_UARTS_NUM];
SD_PORT *SDC_PortList[SD_UARTS_NUM];


static char sd_buffer_uart[UART0_BUFFER_SIZE + UART1_BUFFER_SIZE + UART2_BUFFER_SIZE];

static char* sd_buf_ptr[SD_MAX_UARTS]=
{
    sd_buffer_uart,
    &sd_buffer_uart[UART0_BUFFER_SIZE],
    &sd_buffer_uart[UART0_BUFFER_SIZE + UART1_BUFFER_SIZE]
};

static uint32_t sd_buf_size[SD_MAX_UARTS]=
{
    UART0_BUFFER_SIZE,
    UART1_BUFFER_SIZE,
    UART2_BUFFER_SIZE
};


char  num_ports = 0;
SD_PORT         *SDC_Port_List[SD_MAX_UARTS];


void  SDC_Set_BaudRate(uint32_t, SD_PORT*);



volatile INT32S8U  UART0_TxDataBuff[UART0_TxDataBuffSize];  // .
volatile INT32S16U UART0_TxDataNumCnt;
volatile INT32S16U UART0_TxDataNumLen;
volatile INT32S8U  UART0_TxDataOver;

volatile INT32S8U  UART0_RxDataBuff[UART0_RxDataBuffSize];  // .
volatile INT32S16U UART0_RxDataNumCnt;
volatile INT32S16U UART0_RxDataNumLen;

volatile INT32S8U  UART0_ModeIsASCII;       //
volatile INT32S8U  UART0_RcvStrIdChar0;     //
volatile INT32S8U  UART0_RcvStrIdChar1;     //
volatile INT32S8U  UART0_RcvStrIdChar0Flg;  //
volatile INT32S8U  UART0_RcvStrIdFlg;       //

USART_TxRxMode_TypeDef UART0_TxMode;     // 
USART_TxRxMode_TypeDef UART0_RxMode;     // 


void UART0_AppInit(USART_Enable_TypeDef enableTmp, INT32S16U baudrateTmp)
{
//    OS_CPU_SR cpu_sr;
       
//    OS_ENTER_CRITICAL();    /* Disable INT32Serrupts */

    CMU_ClockEnable(cmuClock_UART0, true);	   // .

    USART_InitAsync_TypeDef  UART_init = USART_INITASYNC_DEFAULT;
    UART_init.baudrate = baudrateTmp;

	USART_InitAsync(UART0, &UART_init);	

    UART0->CTRL  |= USART_CTRL_TXBIL_HALFFULL;    // TXBL set when TX buffer is HALF-FULL or empty.

    GPIO_PinModeSet(gpioPortC, 14, gpioModePushPull, 1);   // .
	GPIO_PinModeSet(gpioPortC, 15, gpioModeInput, 1);      // .
	
	/* Module UART0 is configured to location 3 */
	UART0->ROUTE  = (UART0->ROUTE & ~_UART_ROUTE_LOCATION_MASK) | UART_ROUTE_LOCATION_LOC3;
	/* Enable signals TX, RX */
	UART0->ROUTE |= UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
    
/*	
	USART_InitAsync_TypeDef init_tmp =
	{
		.enable = enableTmp,          //
		.refFreq = 0,                 //
		.baudrate = baudrateTmp,      //
		.oversampling = usartOVS16,   // 
		.databits = usartDatabits8,   // UART standard mode.
		.parity = usartNoParity,      // 
		.stopbits = usartStopbits1,   //  
	};		
	USART_InitAsync(UART0, &init_tmp);	
*/


    NVIC_ClearPendingIRQ(UART0_RX_IRQn);
//  USART_IntEnable(UART0, USART_IEN_RXDATAV);  /* Enable overflow interrupt */     
//  NVIC_EnableIRQ(UART0_RX_IRQn);	            /* Enable UART0 interrupt vector in NVIC */
    NVIC_ClearPendingIRQ(UART0_TX_IRQn);
//  USART_IntEnable(UART0, USART_IEN_TXC);      /* Enable overflow interrupt */     
//  NVIC_EnableIRQ(UART0_TX_IRQn);	            /* Enable UART0 interrupt vector in NVIC */


	UART0_init_buffer(USART_BUFFER *uart)

//  OS_EXIT_CRITICAL();	   /* Restore INT32Serrupts to previous level */
}


INT32S SDC_init_port(SD_PORT *uart)
{
    INT32S status = OS_ERR_NONE;
    INT32U uartindex = 0;

    INT32U i;
	
    USART_InitAsync_TypeDef  USART_InitStructure = USART_INITASYNC_DEFAULT;

    if ((uart->com_port == SD_UART0) ||
		(uart->com_port == SD_UART1) ||
	    (uart->com_port == SD_UART2))
    {
         /* Handle UART0 */
         if (uart->com_port == SD_UART0)
         {             
             uart->vector = UART0_RX_IRQn;        /* Set the vector inside this structure */            
             uart->base_address = (INT32U)UART0;  /* Set the base address for this UART. */
            
//           uartindex = (uart->vector - UART0_RX_IRQn)/2;
         }
         else if (uart->com_port == SD_UART1)   /* Otherwise handle UART1. */
         {

         }
         else if (uart->com_port == SD_UART2)   /* Otherwise handle UART2. */
         {

         }       
         else
         {
             status = OS_ERR_ID_INVALID;
         }
         /* Make sure all the above was completed. Then store off this
            UART stucture and initialize the chip. */

		 if (status == OS_ERR_NONE)
		 {        
             if(num_ports < SD_UARTS_NUM)
			 {
                 SDC_Port_List[num_ports++] = uart;
			 }
        }
 
		if (status == OS_ERR_NONE)
		{
			memset(sd_buf_ptr[uartindex], 0, sd_buf_size[uartindex]);

			/*Support repeating initialize the UART*/
			uart->tx_buffer = sd_buf_ptr[uartindex];
			uart->sd_tx_buffer_size = sd_buf_size[uartindex]/2;

			uart->rx_buffer = sd_buf_ptr[uartindex] + uart->sd_tx_buffer_size;
			uart->sd_buffer_size = sd_buf_size[uartindex]/2;			

			if (status == OS_ERR_NONE)
			{
				/* Setup the RX SD buffer */
				uart->rx_buffer_read = 0; 
				uart->rx_buffer_write = 0;

				for(i=0; i<uart->sd_buffer_size; i++)  
				{
					uart->rx_buffer[i]=0;
				}
				
				uart->rx_buffer_status = UART_RingBuffer_EMPTY;

				/* Setup the TX SD buffer */
				uart->tx_buffer_read = 0;  
				uart->tx_buffer_write = 0;
				uart->tx_buffer_status = UART_RingBuffer_EMPTY;
			}
    }
  
    return (status);
}





void UART0_SendStrASCII(char* SndStr)             // ASCII mode; 
{ 
    OS_CPU_SR cpu_sr;
       
	if(SndStr != UART0_TxDataBuff)
    {
	    strcpy(UART0_TxDataBuff, SndStr);       // 
    }

    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

	UART0_TxMode = IsASCII; 
	UART0_TxDataOver = 0;
    UART0_TxDataNumCnt = 0;

    OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
}

void UART0_SendStrBinary(char* SndStr, INT32S8U StrLen)    // Binary Mode. 
{ 
    OS_CPU_SR cpu_sr;

	if(SndStr != UART0_TxDataBuff)
	{
        memcpy(UART0_TxDataBuff, SndStr, StrLen);   // 
	}

    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

	UART0_TxMode = IsBINARY; 
	UART0_TxDataOver = 0;
    UART0_TxDataNumCnt = 0;
    UART0_TxDataNumLen = StrLen;                    // .

    OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
}

USART_TxRxMode_TypeDef UART0_SendStrStat(void)    //  
{
	return UART0_TxMode;
}


void UART0_RcvEn (USART_TxRxMode_TypeDef RcvMode, bool RcvEnable)
{
    OS_CPU_SR cpu_sr;

    if(RcvEnable)
	{
		OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

		UART0_RxMode = RcvMode; 
		UART0_RcvStrIdFlg = 0;

		OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
    }
	else
	{
		OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

		UART0_RxMode = IsIdle; 
	    UART0_RxDataNumLen = UART0_RxDataNumCnt;

		OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
    }
}


INT32S8U UART0_RcvStrWithID (USART_TxRxMode_TypeDef RcvMode, char* StrID, INT32S8U TimOut)
{
    OS_CPU_SR cpu_sr;
    INT32S8U  i;	
	
    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

	UART0_RxMode = IsIdle;              // 
    UART0_RxDataNumCnt = 0;	            // .
	UART0_RcvStrIdChar0 = StrID[0];
	UART0_RcvStrIdChar1 = StrID[1];

	OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
	
	UART0_RcvStrIdChar0Flg = 0;
	UART0_RcvStrIdFlg = 0;
	UART0_RxMode = RcvMode;             // 

	for(i=0; i<TimOut; i++)
	{
		if(UART0_RcvStrIdFlg)
		{
			return 0;
		}
		else
		{
			OSTimeDlyHMSM(0, 0, 0, 100);   /* Delay 100mS */
		}
	}
	
	return 1;                              // .
}
	
void UART0_RcvBuffClr(INT32S8U BuffLen)
{
    OS_CPU_SR cpu_sr;
	
    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

//	UART0_RxMode = IsIdle;       // 
    UART0_RxDataNumCnt = 0;	     // 
	UART0_RcvStrIdChar0Flg = 0;
	UART0_RcvStrIdFlg = 0;

	OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */
	
	if(BuffLen == 0)
	{															     
	    memset(UART0_RxDataBuff, '\0', sizeof(UART0_RxDataBuff)); 
	}
	else
	{
	    memset(UART0_RxDataBuff, '\0', BuffLen); 
	}
}

void UART0_RcvBuffRst(void)
{
    OS_CPU_SR cpu_sr;
	
    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

//	UART0_RxMode = IsIdle;       // 
    UART0_RxDataNumCnt = 0;	     // 
	UART0_RcvStrIdChar0Flg = 0;
	UART0_RcvStrIdFlg = 0;

	OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */	
}

void UART0_RcvStrIdSet(char* StrID)
{
    OS_CPU_SR cpu_sr;
	
    OS_ENTER_CRITICAL();      /* Disable INT32Serrupts */

	UART0_RcvStrIdChar0 = StrID[0];
	UART0_RcvStrIdChar1 = StrID[1];

	OS_EXIT_CRITICAL();	      /* Restore INT32Serrupts to previous level */	
}









