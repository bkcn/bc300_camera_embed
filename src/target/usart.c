// #include <includes.h>
#include "..\target\target.h"
#include "..\board\board.h"


static struct USART_DATASTRUCT uart0_buffstruct;
struct USART_DATASTRUCT *uart0 = &uart0_buffstruct;

volatile INT8U sd_uart0_rx_buffer[SD_UART0_rxRingBuffSize];
volatile INT8U sd_uart0_tx_buffer[SD_UART0_txRingBuffSize];



/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void UART0_appInit(INT16U baudrate)
{
    OS_CPU_SR cpu_sr = 0u;
       
    OS_ENTER_CRITICAL();                                   /* Disable INT32Serrupts */

    CMU_ClockEnable(cmuClock_UART0, true);	               // .

    GPIO_PinModeSet(gpioPortC, 14, gpioModePushPull, 1);   // .
	GPIO_PinModeSet(gpioPortC, 15, gpioModeInput, 1);      // .

    USART_InitAsync_TypeDef  UART_init = USART_INITASYNC_DEFAULT;
    UART_init.baudrate = baudrate;
	USART_InitAsync(UART0, &UART_init);	
	
	/* Module UART0 is configured to location 3 */
	UART0->ROUTE  = (UART0->ROUTE & ~_UART_ROUTE_LOCATION_MASK) | UART_ROUTE_LOCATION_LOC3;
	/* Enable signals TX, RX */
	UART0->ROUTE |= UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
    
    UART0->CTRL  |= USART_CTRL_TXBIL_HALFFULL;    // TXBL set when TX buffer is HALF-FULL or empty.

    NVIC_ClearPendingIRQ(UART0_TX_IRQn);
    NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	
	UART0_init_dataStruct();

    OS_EXIT_CRITICAL();	   /* Restore INT32Serrupts to previous level */
}


/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void UART0_init_dataStruct(void)
{
//  int  status = OS_ERR_NONE;
//  int  uartindex = 0;
	
/*	
    if ((uart->com_port == SD_UART0)  || 
		(uart->com_port == SD_UART1)  ||
	    (uart->com_port == SD_USART0) || 
		(uart->com_port == SD_USART1) ||
		(uart->com_port == SD_USART2))
    {
		
	}
	
    if(num_ports < SD_UARTS_NUM)
	{
       USART_PORT_List[num_ports++] = uart;
    }	
*/
	
	uart0->base_address = (INT32U)UART0;
	uart0->com_port     = (INT16U)SD_UART0;
	uart0->baud_rate    = (INT16U)USART_BaudrateGet(UART0);
	
	uart0->rx_buffer_size      = (INT16U)SD_UART0_rxRingBuffSize;
	uart0->tx_buffer_size      = (INT16U)SD_UART0_txRingBuffSize;
	
	uart0->rx_buffer = (char *)sd_uart0_rx_buffer;
	uart0->rx_buffer_read = 0;
	uart0->rx_buffer_write = 0;
//	uart0->rx_buffer_status = ringBuffer_EMPTY;
	
	uart0->rx_enable = true;             //  
    uart0->rx_strMode = true;	
	uart0->rx_strIdChar0 = '\r';
	uart0->rx_strIdChar1 = '\n';
	uart0->rx_strIdChar0Match = 0;    // false.
	uart0->rx_strIdChar1Match = 0;    // false.
	
//	uart0->communication_mode = SD_MODE_UART;
	uart0->tx_buffer = (char *)sd_uart0_tx_buffer;
	uart0->tx_buffer_read = 0;
	uart0->tx_buffer_write = 0;
//	uart0->tx_buffer_status = ringBuffer_EMPTY;
}


/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_tx_bufRst(USART_PORT *uart)
{
	uart->tx_buffer_read = 0;
	uart->tx_buffer_write = 0;
//	uart->tx_buffer_status = ringBuffer_EMPTY;	
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
/*
bool USART_putchar(USART_PORT *uart, char ch)
{
	if(uart->tx_buffer_status != ringBuffer_FULL)
	{				
		uart->tx_buffer[uart->tx_buffer_write] = ch;
		uart->tx_buffer_status = ringBuffer_DATA; 
		
		uart->tx_buffer_write++;
		if(uart->tx_buffer_write >= uart->tx_buffer_size)
		{
			uart->tx_buffer_status = ringBuffer_FULL;      // .
		}
				
        return true;	// success. 	
	}	
	
	return false;       // fail.
}
*/

bool USART_putchar(USART_PORT *uart, char ch)
{
	INT16U next_write;
	
	next_write = uart->tx_buffer_write + 1;   // 2043 + 1 = 2048/2048.
	if(next_write >= uart->tx_buffer_size)
	{
		next_write = 0;
	}
	
	if(next_write != uart->tx_buffer_read)
	{
		uart->tx_buffer[uart->tx_buffer_write] = ch;	
		uart->tx_buffer_write = next_write;

//		delay_us(100); 
//		OSTimeDly(1);

		return true;
	}
	else
	{
		return false;
	}
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
bool USART_getchar(USART_PORT *uart, char *ch)
{
	if(uart->rx_buffer_read != uart->rx_buffer_write)
	{
		*ch = (char)uart->rx_buffer[uart->rx_buffer_read];       //

		uart->rx_buffer_read++;
		if(uart->rx_buffer_read >= uart->rx_buffer_size)
		{
		    uart->rx_buffer_read = uart->rx_buffer_read - uart->rx_buffer_size;
		}
		return true;
	}
	else
	{
		return false;
	}
}


/****************************************************************
 ** fuction:      USART_putstr()
 ** description:            
 ** input:        USART_PORT *uart, 
 **               char *str,
 ** output:       null.
 ** input globe:  null.
 ** call:         delay_ms.      
 ** other:        . 
 *****************************************************************/
void USART_putstr(USART_PORT *uart, char *str)
{        
    for (; *str != '\0'; )
	{               
        if(true == USART_putchar(uart, *str))
		{
			str++;
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
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_putbinary(USART_PORT *uart, char *str, INT16U datalen)
{       
    INT16U i;
	
    for(i=0; i<datalen; )
	{               
        if(true == USART_putchar(uart, str[i]))
		{
			i++;
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
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_putbinary_long(USART_PORT *uart, char *str, INT32U datalen)
{       
    INT32U i;
	
    for(i=0; i<datalen; )
	{               
        if(true == USART_putchar(uart, str[i]))
		{
			i++;
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
}

bool USART_getBufStat(USART_PORT *uart)
{
    /* check the status */
    if(uart->rx_buffer_read != uart->rx_buffer_write)
	{
		return true;
	}
    else
	{
        return false;
	}
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
INT16U USART_getDataNum(USART_PORT *uart)
{
	INT16U num;
	
    /* check the status. */
    if(uart->rx_buffer_read != uart->rx_buffer_write)
	{
		if(uart->rx_buffer_write > uart->rx_buffer_read)
		{		
            num = uart->rx_buffer_write - uart->rx_buffer_read;
		}
        else
		{
            num = uart->rx_buffer_write + uart->rx_buffer_size - uart->rx_buffer_read;
		}
		return num;
	}
    else
	{
        return 0;
	}
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_rx_enable(USART_PORT *uart, bool enable)
{
    OS_CPU_SR cpu_sr = 0u;

	OS_ENTER_CRITICAL();     
	
	uart->rx_enable = enable;		          // .
	
	OS_EXIT_CRITICAL();	      
}

void USART_rx_hwClr(USART_PORT *uart)
{
	INT8U ucTmpVal = 0;
	
	while(UART0->STATUS & USART_STATUS_RXDATAV) 
	{
		ucTmpVal = (INT8U)UART0->RXDATA;		
	}
	ucTmpVal = ucTmpVal;                     // get off the warning.
}	

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_rx_strModeSet(USART_PORT *uart, bool enable)
{
    OS_CPU_SR cpu_sr = 0;

	OS_ENTER_CRITICAL();     	
	uart->rx_strMode = enable;		// .	
	OS_EXIT_CRITICAL();	      
}	

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_rx_bufClr(USART_PORT *uart, INT16U bufLen)
{
    OS_CPU_SR cpu_sr = 0;
	
    OS_ENTER_CRITICAL();  
    
	uart->rx_buffer_read = 0;
	uart->rx_buffer_write = 0;
//	uart->rx_buffer_status = ringBuffer_EMPTY;

    uart->rx_strIdChar0Match = 0;
	uart->rx_strIdChar1Match = 0;
	
	OS_EXIT_CRITICAL();	      
	
	if(bufLen == 0)
	{															     
	    memset(uart->rx_buffer, '\0', sizeof(uart->rx_buffer)); 
	}
	else
	{
	    memset(uart->rx_buffer, '\0', bufLen); 
	}
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_rx_bufRst(USART_PORT *uart)
{
    OS_CPU_SR cpu_sr = 0;
	
    OS_ENTER_CRITICAL();      
	
	uart->rx_buffer_read = 0;
	uart->rx_buffer_write = 0;
//	uart->rx_buffer_status = ringBuffer_EMPTY;

    uart->rx_strIdChar0Match = 0;
	uart->rx_strIdChar1Match = 0;
	
	OS_EXIT_CRITICAL();	      
}

/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
void USART_rx_strIdSet(USART_PORT *uart, char* strID)
{
    OS_CPU_SR cpu_sr = 0;
	
    OS_ENTER_CRITICAL();      

	uart->rx_strIdChar0 = strID[0];
	uart->rx_strIdChar1 = strID[1];
	uart->rx_strIdChar0Match = 0;
	uart->rx_strIdChar1Match = 0;

	OS_EXIT_CRITICAL();	      
}


/****************************************************************
 ** fuction:      
 ** description:            
 ** input:        , 
 **               ,
 ** output:       null.
 ** input globe:  null.
 ** call:         .      
 ** other:        . 
 *****************************************************************/
bool USART_rx_strIdIsVld(USART_PORT *uart)
{
    OS_CPU_SR cpu_sr = 0;
	
    if(uart->rx_strIdChar1Match) 	    // .
	{
        OS_ENTER_CRITICAL();  		
        uart->rx_strIdChar0Match = 0;	// .
        uart->rx_strIdChar1Match = 0;	// .
        OS_EXIT_CRITICAL();	      

	    return true;                  
	} 
	else				                // 
	{
	    return false;        
    }
} 















