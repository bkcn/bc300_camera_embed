/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               systick.c
** Descriptions:            使用SysTick的普通计数模式对延迟进行管理
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-10-30
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <includes.h>
#include "systick.h"
#include "Delay.h"

/* Private variables ---------------------------------------------------------*/     
static uint8_t  delay_fac_us = 0;  /* us延时倍乘数 */
static uint16_t delay_fac_ms = 0;  /* ms延时倍乘数 */



/* This is the number of bits of precision for the loops_per_second.  Each
   bit takes on average 1.5/HZ seconds.  This (like the original) is a little
   better than 1% */
#define LPS_PREC 8
//unsigned long loops_per_sec=0;
unsigned long loops_per_sec=0x64000;  /*Set default value, Get by test with 44MHZ /60MHZ */
#define jiffies OSTime
void calibrate_delay(void)
{
    unsigned long  ticks;
    unsigned long loopbit;
    unsigned long lps_precision = LPS_PREC;


    loops_per_sec = (1<<12);
/*  printk("Calibrating delay loop.. "); */
    while (loops_per_sec <<= 1) {
        /* wait for "start of" clock tick */
        ticks = jiffies;
        while (ticks == jiffies)
            /* nothing */;
        /* Go .. */
        ticks = jiffies;
        __delay(loops_per_sec);
        ticks = jiffies - ticks;
        if (ticks)
            break;
        }



/* Do a binary approximation to get loops_per_second set to equal one clock
   (up to lps_precision bits) */
    loops_per_sec >>= 1;
    loopbit = loops_per_sec;
    while ( lps_precision-- && (loopbit >>= 1) ) {
        loops_per_sec |= loopbit;
        ticks = jiffies;
        while (ticks == jiffies);
        ticks = jiffies;
        __delay(loops_per_sec);
        if (jiffies != ticks)   /* longer than 1 tick */
            loops_per_sec &= ~loopbit;
    }

/* finally, adjust loops per second in terms of seconds instead of clocks */    
    loops_per_sec *= OS_TICKS_PER_SEC;
/* Round the value and print it */
#if 0
    WriteStr("\r\nok - %lu.%02lu BogoMIPS\r\n",
        loops_per_sec/500000,
        (loops_per_sec/5000) % 100);
#endif

}



/*******************************************************************************
* Function Name  : delay_init
* Description    : 初始化延迟函数 
* Input          : None
* Output         : None
* Return         : None
* Attention      : SYSTICK的时钟固定为HCLK时钟的1/8
*******************************************************************************/
void delay_init(void)
{
#if 1
    calibrate_delay();
#else
    RCC_ClocksTypeDef RCC_ClocksStatus;

    

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);  /*选择外部时钟  HCLK/8 */
    SysTick_ITConfig(DISABLE);
    delay_fac_us = RCC_ClocksStatus.HCLK_Frequency / 8000000;
    delay_fac_ms = RCC_ClocksStatus.HCLK_Frequency / 8000;
#endif    
}
                    
/*******************************************************************************
* Function Name  : delay_us
* Description    : 初始化延迟函数 
* Input          : - Nus: 延时us
* Output         : None
* Return         : None
* Attention      : 参数最大值为 0xffffff / (HCLK / 8000000)
*******************************************************************************/            
void delay_us(unsigned long Nus)
{ 
#if 1
    udelay(Nus);
#else
    SysTick_SetReload(delay_fac_us * Nus);          /* 时间加载 */
    SysTick_CounterCmd(SysTick_Counter_Clear);      /* 清空计数器 */
    SysTick_CounterCmd(SysTick_Counter_Enable);     /* 开始倒数 */  
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);                         /* 等待时间到达 */
    SysTick_CounterCmd(SysTick_Counter_Disable);    /* 关闭计数器 */
    SysTick_CounterCmd(SysTick_Counter_Clear);      /* 清空计数器 */   
#endif
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : 初始化延迟函数 
* Input          : - nms: 延时ms
* Output         : None
* Return         : None
* Attention      : 参数最大值 nms<=0xffffff*8*1000/SYSCLK 对72M条件下,nms<=1864 
*******************************************************************************/  
void delay_ms(uint16_t nms)
{    
#if 1
    unsigned long delaytime=nms;
    udelay(delaytime*1000);
#else
    uint32_t temp = delay_fac_ms * nms;

    if (temp > 0x00ffffff)
    {
        temp = 0x00ffffff;
    }
    SysTick_SetReload(temp);                     /* 时间加载 */
    SysTick_CounterCmd(SysTick_Counter_Clear);   /* 清空计数器 */
    SysTick_CounterCmd(SysTick_Counter_Enable);  /* 开始倒数 */ 
    do
    {
        Status = SysTick_GetFlagStatus(SysTick_FLAG_COUNT);
    }while (Status != SET);                      /* 等待时间到达 */
    SysTick_CounterCmd(SysTick_Counter_Disable); /* 关闭计数器 */
    SysTick_CounterCmd(SysTick_Counter_Clear);   /* 清空计数器 */
#endif
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
















