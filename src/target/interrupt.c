
#include "..\target\target.h"
#include "..\board\board.h"


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
    CPU_SR cpu_sr;

    OS_ENTER_CRITICAL();       /* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();              /* Call uC/OS-II's OSTimeTick() */
    OSIntExit();               /* Tell uC/OS-II that we are leaving the ISR */
}

void TIMER0_IRQHandler(void)
{    
//	CPU_SR cpu_sr;
	INT8U  ucTmpVal;
	INT16U next_write;

//  CPU_CRITICAL_ENTER();                  /* Tell uC/OS-II that we are starting an ISR            */
//  OSIntNesting++;
//  CPU_CRITICAL_EXIT();
	
    TIMER_IntClear(TIMER0, TIMER_IF_OF);   // Clear flag for TIMER0 overflow interrupt.
	
//*	
    if(GPIO_PortInGet(gpioPortC) & OV5640_DVP_VSYNC_PIN_VAL)   // 
	{
        VSYNC_currLevel = true;      // level high.
	}		
	else
	{
        VSYNC_currLevel = false;     // level low.
	}
/*
    // active at rising edge.  
    if(VSYNC_currLevel)              // level high.
	{
		if( !VSYNC_lastLevel )       // level low. 
		{
			if(photoGetFraCntEn)
			{
			    photoGetFraCnt++;
			}		
		}
	}
*/
	// active at falling edge.
    if( !VSYNC_currLevel)              // level low.
	{
		if( VSYNC_lastLevel )          // level high. 
		{
			if(photoGetFraCntEn)
			{
			    photoGetFraCnt++;
			}		
		}
	}

	VSYNC_lastLevel = VSYNC_currLevel;
//*/	
	
	#ifdef UART0_ENABLE
    //----------------------------------------------------------	
    //- 查询UART0的发送状态.
    //----------------------------------------------------------
	if(uart0->tx_buffer_read != uart0->tx_buffer_write)
	{
//		while(UART0->STATUS & USART_STATUS_TXBL)
		if(UART0->STATUS & USART_STATUS_TXBL)
		{
			if(uart0->tx_buffer_read != uart0->tx_buffer_write)
			{
				UART0->TXDATA = (INT32U)uart0->tx_buffer[uart0->tx_buffer_read];
				
				uart0->tx_buffer_read++;
				if(uart0->tx_buffer_read >= uart0->tx_buffer_size)
				{
					uart0->tx_buffer_read = 0;
				}
			}
			else
			{
//				break;
			}
		}
	}


	//----------------------------------------------------------	
	//- UART0's receive, check state of FIFO.
	//----------------------------------------------------------	
/*
	if(UART0->STATUS & USART_STATUS_RXFULL)         // FIFO 溢出.
	{
		while(UART0->STATUS & USART_STATUS_RXDATAV) 
		{
			ucTmpVal = (INT8U)UART0->RXDATA;		
		}
	}
*/
	//----------------------------------------------------------	
	//- UART0's receive, check the receive buffer.
	//----------------------------------------------------------	
	while(UART0->STATUS & USART_STATUS_RXDATAV)    // can receive 3 bytes at one time.
	{
		ucTmpVal = (INT8U)UART0->RXDATA;           // 
		
		if(uart0->rx_enable)
		{
			next_write = uart0->rx_buffer_write + 1;     // 2043 + 1 = 2048/2048.
			if(next_write >= uart0->rx_buffer_size)
			{
				next_write = 0;
			}
			
			if(next_write != uart0->rx_buffer_read)
			{
				uart0->rx_buffer[uart0->rx_buffer_write] = ucTmpVal;
				uart0->rx_buffer_write = next_write;
			
				if(uart0->rx_strMode)                            //  
				{				
					if(ucTmpVal == uart0->rx_strIdChar1)	     // K.
		//	        if(ucTmpVal == '\n')
					{
						if(uart0->rx_strIdChar0Match)            //
						{
							uart0->rx_strIdChar1Match = 1;
							uart0->rx_buffer[uart0->rx_buffer_write] = '\0';  // .
							uart0->rx_enable = 0;						      // . 
						}	
					}
					else if(ucTmpVal == uart0->rx_strIdChar0)	 // O.
		//	        else if(ucTmpVal == '\r')
					{
						uart0->rx_strIdChar0Match = 1;
					}
					else
					{
						uart0->rx_strIdChar0Match = 0;
					}					
				}
			}
			else
			{
				break;           // rx buffer is full.
			}
		}
	}
    #endif

}


/* GPIO even IRQ */
void GPIO_EVEN_IRQHandler(void)
{

}

/* GPIO odd IRQ */
/*
void GPIO_ODD_IRQHandler(void)
{
    GPIO->IFC = 1 << OV5640_DVP_VSYNC_PIN;    // PC3 interrupt chear.
    photoGetFraCnt++;
}
*/


void DMA_photoDataGet_callBack(INT32U channel, bool primary, void *user)
{	
    INT32U i;
    INT32U *pPriBuf;
	INT32U *pAltBuf;
    INT32U ulTmpVal;
	
    bool   retval;
    INT8U  *pSOI;
    INT32U fileLen;    
    INT32U *pDataCnt;
	
	pDataCnt = (INT32U *)user;
	
    if(photoGetFraCnt < (PHOTO_FRA_VALID_NO-1))       // frame no < 3-1=2.
    {
        /* re-activate the DMA 
         * over-write header buffer DMA_TRANS_BLOCK_SIZE*2
         */
        DMA_RefreshPingPong(channel,
                            primary,
                            false,
                            (void*)((true==primary)?(photodataBuff):(photodataBuff+DMA_TRANS_BLOCK_SIZE)),
                            NULL,
                            DMA_TRANS_BLOCK_SIZE-1,
                            false);
		test= 1;
        lastPrimary = primary;
    }
	else if(photoGetFraCnt <= PHOTO_FRA_VALID_NO)    // frame no = 2/3.
	{                 
		if(*pDataCnt <= PHOTO_DATA_BUFF_SIZE_LESS)   // 80k-1k=79kB.
		{
			/* re-activate the DMA */
			DMA_RefreshPingPong(channel,                    // from base+0x800. 
								primary,
								false,
								(void*)(photodataBuff+(*pDataCnt)),  // next block.
								NULL,
								DMA_TRANS_BLOCK_SIZE-1,
								false);
			  
			*pDataCnt = *pDataCnt + DMA_TRANS_BLOCK_SIZE;   // base+0x800+0x400.
			test= 2;
		}
		else
		{
			test= 3;
			goto __JPEG_CHECK_FORMAT;
		}
	}
	else                                             // frame no = 4.
    {        
__JPEG_CHECK_FORMAT:		
		/* exchange 0x0000-0x3FF VS 0x400-0x7FF */		
		if(true == lastPrimary)
		{				
			pPriBuf = (INT32U *)photodataBuff;
			pAltBuf = &pPriBuf[DMA_TRANS_BLOCK_SIZE/sizeof(INT32U)];
			for(i=0; i<DMA_TRANS_BLOCK_SIZE/sizeof(INT32U); i++)
			{
				ulTmpVal   = pPriBuf[i];
				pPriBuf[i] = pAltBuf[i];
				pAltBuf[i] = ulTmpVal;
			}
		}
		test= 4;
		/* only track one frame */
		retval = jpegFileCheck((INT8U *)photodataBuff, *pDataCnt, 0, &pSOI, &fileLen);	
		if((retval == SUCCESS) && (fileLen > PHOTO_FILELEN_MIN))// && (fileLen < PHOTO_FILELEN_MAX))
		{
			photoBufPtr = (INT8U *)pSOI;
			photobufLen = fileLen;
			printf("Image Start Addr = 0x%x\n", photoBufPtr);
			printf("File Len Addr = 0x%x\n", fileLen);
			photoGetStat = PHOTOGET_SUCCESS;        // error.
		}
		else
		{
			photoBufPtr = (INT8U *)photodataBuff;
			photobufLen = 0;                        // .
			photoGetStat = PHOTOGET_RESET;          // error, take photo again.
		}

		photoGetFraCntEn = false;                   // stop trigger.
		photoGetFraCnt = 0;                         // . 
    }
}










