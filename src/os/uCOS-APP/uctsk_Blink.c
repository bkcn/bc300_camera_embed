/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               uctsk_Blink.c
** Descriptions:            The uctsk_Blink application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-9
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


/* Private variables ---------------------------------------------------------*/
static  OS_STK         App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE];

/* Private function prototypes -----------------------------------------------*/
static void uctsk_Blink (void);


void  App_BlinkTaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */

	os_err = OSTaskCreate((void (*)(void *)) uctsk_Blink,				
                          (void          * ) 0,							
                          (OS_STK        * )&App_TaskBlinkStk[APP_TASK_BLINK_STK_SIZE - 1],		
                          (INT8U           ) APP_TASK_BLINK_PRIO  );							

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_BLINK_PRIO, "Task LED Blink", &os_err);
	#endif

}
/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


static void uctsk_Blink (void) {                 
  
   
   	for(;;)
   	{   
	    Delay( rand()%0xfffff ); 	

		OSTimeDlyHMSM(0, 0, 1, 0);	 /* 1 seconds  */
	  	/*====LED-ON=======*/
		//GPIO_SetBits(GPIOB , GPIO_Pin_1);

	    Delay( rand()%0xfffff ); 

      	OSTimeDlyHMSM(0, 0, 1, 0);
	  	 /*====LED-OFF=======*/ 
		GPIO_ResetBits(GPIOB , GPIO_Pin_1);
   }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
