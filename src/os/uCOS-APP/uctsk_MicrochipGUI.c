/*----------------------------------------------------------------------------
 *      
 *----------------------------------------------------------------------------
 *      Name:    uctsk_MicrochipGUI.C
 *      Purpose: 
 *----------------------------------------------------------------------------
 *      
 *      Copyright (c) 2004-2009          All rights reserved.
 *---------------------------------------------------------------------------*/
#include <includes.h> 
#include <menu.h>


static  OS_STK  AppMicrochipGUITaskStk[APP_TASK_MICROCHIP_GUI_STK_SIZE];
static  void    uctsk_MicrochipGUI(void *pdata);


void  App_MicrochipGUITaskCreate (void)
{
	CPU_INT08U  os_err;

	os_err = os_err; /* prevent warning... */
    
    os_err = OSTaskCreateExt((void (*)(void *)) uctsk_MicrochipGUI,
                             (void          * ) 0,
                             (OS_STK        * )&AppMicrochipGUITaskStk[APP_TASK_MICROCHIP_GUI_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_MICROCHIP_GUI_PRIO,
                             (INT16U          ) APP_TASK_MICROCHIP_GUI_PRIO,
                             (OS_STK        * )&AppMicrochipGUITaskStk[0],
                             (INT32U          ) APP_TASK_MICROCHIP_GUI_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_MICROCHIP_GUI_PRIO, "Task microchipGUI", &os_err);
	#endif

}

static  void    uctsk_MicrochipGUI(void *pdata)
{
	GOL_MSG msg;        				// GOL message structure to interact with GOL

	(void)pdata;	/* Prevent compiler warning                 */

	StartMenu();
	
	for(;;)
   	{
      	OSTimeDlyHMSM(0, 0, 0, 100);
		
		if ( GOLDraw() )
      	{				// Draw GOL object
         	TouchGetMsg(&msg);   		// Get message from touch screen
         	GOLMsg(&msg);        		// Process message
		} 
    }
}

