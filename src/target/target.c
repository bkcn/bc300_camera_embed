
#include <includes.h>
#include <stdio.h>
#include "..\target\target.h"
#include "..\board\board.h"


void CMU_CLKOUT0_init(bool enable)
{
	if(enable)
	{		
		GPIO_PinModeSet(gpioPortC, 12, gpioModePushPull, 1);
		
		/* Module CMU is configured to location 1, PC12 */
		CMU->ROUTE  = (CMU->ROUTE & ~_CMU_ROUTE_LOCATION_MASK) | CMU_ROUTE_LOCATION_LOC1;	
		CMU->ROUTE |=  CMU_ROUTE_CLKOUT0PEN;     
		
		/* enable signal CLKOUT0 */
		CMU->CTRL = (CMU->CTRL & ~(0x7<<20))|(0x2<<20);
//		CMU->CTRL = (CMU->CTRL & ~(0x3<<_CMU_CTRL_CLKOUTSEL0_SHIFT)) |(0x2<<_CMU_CTRL_CLKOUTSEL0_SHIFT);		
//		CMU->CTRL = (CMU->CTRL & ~(0x3<<_CMU_CTRL_CLKOUTSEL0_SHIFT)) | CMU_CTRL_CLKOUTSEL0_HFCLK2;		
	}
	else
	{
		GPIO_PinModeSet(gpioPortC, 12, gpioModeInput, 1);

		CMU->ROUTE &= ~CMU_ROUTE_CLKOUT0PEN;     /* disable signal CLKOUT0 */
	}
}

void GPIO_appInit(void)
{
//  GPIO_PinModeSet(gpioPortF,  8, gpioModeWiredAnd, 1);         // LED_RUN#, 同时作为CM_STAT输出.
//  GPIO_PinModeSet(gpioPortF,  8, gpioModeWiredAndPullUp, 1);         // LED_RUN#, 同时作为CM_STAT输出.
    GPIO_PinModeSet(gpioPortC,  1, gpioModeWiredAndPullUp, 1);         // LED_RUN#, 同时作为CM_STAT输出. // 20161013

	
    GPIO_PinModeSet(gpioPortC, 14, gpioModePushPull, 1);         // .
	GPIO_PinModeSet(gpioPortC, 15, gpioModeInputPullFilter, 1);  // .
}


void WDOG_appInit(void)
{
    CMU_ClockEnable(cmuClock_CORELE, true);	   // 看门狗为低频外设，访问其寄存器需要开启LE时钟.
	
	WDOG_Init_TypeDef init_tmp =
	{
		.enable     = true,               /* Start watchdog when init done */
		.debugRun   = false,              /* WDOG not counting during debug halt */
		.em2Run     = true,               /* WDOG counting when in EM2 */
		.em3Run     = true,               /* WDOG counting when in EM3 */
		.em4Block   = false,              /* EM4 can be entered */
		.swoscBlock = false,              /* Do not block disabling LFRCO/LFXO in CMU */
		.lock       = false,              /* Do not lock WDOG configuration (if locked, reset needed to unlock) */
		.clkSel     = wdogClkSelULFRCO,   /* Select 1kHZ WDOG oscillator */
		.perSel     = wdogPeriod_2k,      /* Set the watchdog period to 2048 clock periods (ie ~2 seconds)*/
//		.perSel     = wdogPeriod_8k,      /* Set the watchdog period to 8192 clock periods (ie ~8 seconds)*/
	};
    WDOG_Init(&init_tmp);

	WDOG_Enable(true);
	while (WDOG->SYNCBUSY & WDOG_SYNCBUSY_CTRL);  // must do it before the next WDOG_Enable().
//	WDOG_Feed();
//	if(ulResetCause == RMU_RSTCAUSE_WDOGRST) { }  // .
	
}	

/*
void EBI_init_GPIO(bool enable)
{
	if(enable)
	{
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
		GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);   // Push-pull.
	}
	else
	{
		
	}
}
*/

void CM_init_GPIO(bool enable)
{
    if(enable)
	{
		GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 1);           // CM_D0, Input.
		GPIO_PinModeSet(gpioPortD, 1, gpioModeInput, 1);           // CM_D1, Input.
		GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 1);           // CM_D2, Input.
		GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 1);           // CM_D3, Input.
		GPIO_PinModeSet(gpioPortD, 4, gpioModeInput, 1);           // CM_D4, Input.
		GPIO_PinModeSet(gpioPortD, 5, gpioModeInput, 1);           // CM_D5, Input.
		GPIO_PinModeSet(gpioPortD, 6, gpioModeInput, 1);           // CM_D6, Input.
		GPIO_PinModeSet(gpioPortD, 7, gpioModeInput, 1);           // CM_D7, Input.

		GPIO_PinModeSet(gpioPortC,11, gpioModeInputPullFilter, 1); // CM_VSYNC, Input with filter..
		GPIO_PinModeSet(gpioPortC,12, gpioModePushPull, 0);        // CM_XCLK.
		GPIO_PinModeSet(gpioPortC,13, gpioModeInput, 0);           // CM_PCLK.

		GPIO_PinModeSet(gpioPortB,11, gpioModeWiredAnd, 1);        // CM_I2C_SDIO, OC.
		GPIO_PinModeSet(gpioPortB,12, gpioModeWiredAnd, 1);        // CM_I2C_SCLK, OC.
	}
	else
	{
		GPIO_PinModeSet(gpioPortC,11, gpioModeInput, 1);     // CM_VSYNC, Input with filter..
		GPIO_PinModeSet(gpioPortC,12, gpioModeInput, 1);     // CM_XCLK.
		GPIO_PinModeSet(gpioPortC,13, gpioModeInput, 1);     // CM_PCLK.

		GPIO_PinModeSet(gpioPortB,11, gpioModeInput, 1);     // CM_I2C_SDIO, OC.
		GPIO_PinModeSet(gpioPortB,12, gpioModeInput, 1);     // CM_I2C_SCLK, OC.
		
	}
}

void TIMER0_appInit(void)
{
    /* enable clock for TIMER0 module */
    CMU_ClockEnable(cmuClock_TIMER0, true);
    
    /* select TIMER0 parameters */  
    TIMER_Init_TypeDef timerInit =
    {
        .enable     = true, 
        .debugRun   = true, 
        .prescale   = timerPrescale16,       // 48/16=3MHz.
        .clkSel     = timerClkSelHFPerClk, 
        .fallAction = timerInputActionNone, 
        .riseAction = timerInputActionNone, 
        .mode       = timerModeUp, 
        .dmaClrAct  = false,
        .quadModeX4 = false, 
        .oneShot    = false, 
        .sync       = false, 
    };
    
    /* 1, configure TIMER */
    TIMER_Init(TIMER0, &timerInit);

    /* 2, set TIMER Top value */
    TIMER_TopSet(TIMER0, TIMER0_CNTMAX);

    /* 3, enable overflow interrupt */
    TIMER_IntEnable(TIMER0, TIMER_IF_OF);
    
    /* 4, enable TIMER0 interrupt vector in NVIC */
    NVIC_EnableIRQ(TIMER0_IRQn);        
}


/**************************************************************************//**
 * @brief  configure TIMER1
 * TIMER is set up to start running on a falling edge of the CC0 input.
 * As the TOP value is set to 0 this will give an immediate overflow that
 * generates a DMA request. As the TIMER is set up in One-Shot Mode, it will
 * stop immediately on the overflow, requiring another falling edge to start
 * again. 
 *****************************************************************************/
void TIMER1_appInit(void)
{
    /* enable clock for TIMER1 module */
    CMU_ClockEnable(cmuClock_TIMER1, true);
		
	TIMER_InitCC_TypeDef timerInit_cc_struct = TIMER_INITCC_DEFAULT;
/*	
	TIMER_InitCC_TypeDef timerInit_cc_struct =
	{
		.eventCtrl = timerEdgeRising,
		.edge      = timerEdgeRising,
		.prsSel    = timerPRSSELCh0,          // useless.
		.cufoa     = timerOutputActionNone,   // . 
		.cofoa     = timerOutputActionNone,   // .
		.cmoa      = timerOutputActionNone,   // .
		.mode      = timerCCModeOff,
		.filter    = false,
		.prsInput  = false,
		.coist     = false,
		.outInvert = false,
	};	
*/	
    TIMER_InitCC(TIMER1, 0, &timerInit_cc_struct);
	
    // select TIMER1 parameters. 
    // configure TIMER1 to set a DMA request on rising edge for CC0 input.
	// set CC0's rising edge (input) to start TIMER1.
	// set 0 to TIMER1's TOP value.
    // TIMER1 underflow when started by CC0's rising edge.	
	// DMA trigged by TIMER1 underflow.
	// set TIMER1 one-shot mode, stopped and reload when underflow/overflow.
	
//	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    TIMER_Init_TypeDef timerInit =
    {
        .enable     = false, 
        .debugRun   = true, 
        .prescale   = timerPrescale1,              // 48/1=48MHz..
        .clkSel     = timerClkSelHFPerClk,         // timerClkSelCC1.
        .fallAction = timerInputActionNone,        // Start TIMER on falling edge on CC0 pin.
//      .riseAction = timerInputActionReloadStart, // PCLK_AND_NOT_GATE_ENABLE
        .riseAction = timerInputActionStart,       // PCLK_AND_NOT_GATE_ENABLE
        .mode       = timerModeUp,                 // +1.
        .dmaClrAct  = true,
        .quadModeX4 = false, 
        .oneShot    = true, 
        .sync       = false, 
    };

    // 1, configure TIMER.
    TIMER_Init(TIMER1, &timerInit);	
	
    // 2, set TIMER Top value.
	// TOP value = 0: generate overflow immediately once TIMER starts.
	TIMER_TopSet(TIMER1, 0);
//	TIMER_TopSet(TIMER1, 0xFFFF);   // 48MHz (input) = 20ns (delay). 

	// 3, enable routing of TIMER1 CC0 pin to GPIO LOCATION 0 (PC13).
	TIMER1->ROUTE = TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC0;	
//	TIMER1->ROUTE  = TIMER_ROUTE_LOCATION_LOC0;	
//	TIMER1->ROUTE |= TIMER_ROUTE_CC0PEN;	
}



int fputc(int ch, FILE *f)
{
    while(false == USART_putchar(uart0, (unsigned char) ch))
		;
    return ch;
}


// @48MHz, delay_1us() = 1.2us;
// @48MHz, delay_us(1) = 1.2us;
// @48MHz, delay_us(2) = 1.2us;
// @48MHz, delay_us(3) = 1.2us;

// @24MHz, delay_1us() = 1.2us;
// @24MHz, delay_us(1) = 1.2us;
// @24MHz, delay_us(2) = 1.2us;
// @24MHz, delay_us(3) = 1.2us;

/*******************************************************************************
* Function Name  : delay_us
* Description    : 初始化延迟函数 
* Input          : - Nus: 延时us
* Output         : None
* Return         : None
* Attention      : 参数最大值为 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void delay_us(INT16U val)
{
	INT8U  i;
    INT16U j;	
	for(j=val; j>0; j--)
	{
		for(i=7; i>0; i--);
	}
}

/*******************************************************************************
* Function Name  : delay_us
* Description    : 初始化延迟函数 
* Input          : - Nus: 延时us
* Output         : None
* Return         : None
* Attention      : 参数最大值为 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void delay_1us(void)
{
    INT16U i;	

	for(i=1; i>0; i--);
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : 初始化延迟函数 
* Input          : nms: 延时ms
* Output         : None
* Return         : None
* Attention      : 参数最大值 nms<=0xffffff*8*1000/SYSCLK 对72M条件下,nms<=1864 
*******************************************************************************/  
void delay_ms(INT16U val)
{    
	INT16U i;
	
	for(i=0; i<val; i++)
	{
        delay_us(1068);
	}
}

void delay_1s(INT16U val)
{    
	INT16U i;
	
	for(i=0; i<val; i++)
	{
        delay_ms(1000);
	}
}




/*********************************************************************************************************
 ** END FILE
 *********************************************************************************************************/



