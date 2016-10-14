#include <includes.h>

#include "target.h"        // MCU and peripherals on chip.
#include "board.h"         // Peripherals on board.

#include "em_device.h"
#include "em_cmu.h"
#include "em_ebi.h"
#include "em_gpio.h"
#include "em_system.h"
#include "em_usart.h"




const INT8U firmWareVer[] = "\r\nCM.h1v2.2 build on "__TIME__" " __DATE__"\r\n";    //

#define LED_lgtOnhmSecCntMax      10        //

INT8U msg_flag = 0;
// static struct tm sysTime;                //  
// volatile INT8U  photodataBuff[PHOTO_DATA_BUFF_SIZE + DMA_TRANS_BLOCK_SIZE * 2] __attribute__ ((at(0x20009000))); 
//volatile INT8U  photodataBuff[PHOTO_DATA_BUFF_SIZE + DMA_TRANS_BLOCK_SIZE * 2]; 
INT8U* photodataBuff = (INT8U*)0x80000000;
INT8U test = 0;
volatile INT32U DMA_photoDataRecvCnt = 0;
volatile INT8U  cmBuf[cm_dataBuffSize];
	

volatile bool VSYNC_lastLevel = true;
volatile bool VSYNC_currLevel = true;
volatile bool lastPrimary;                       // 	

volatile INT8U  *photoBufPtr = NULL;
volatile INT32U photobufLen = 0;

volatile INT8U  photoGetStat = PHOTOGET_RESET;   // .
volatile INT32U photoGetFraCnt = 0;              // ov5640 frame number.
volatile bool   photoGetFraCntEn = false;        // Vsync count enable.

volatile INT8U  LED_lgthmSecCnt = 0;             // .
volatile INT8U  LED_lgtOnhmSecCmd = 5;           // 5/10 busy.
volatile INT8U  LED_lgtOnFlg;                   // .


volatile INT32U OSTimeGet_ref = 0;

volatile bool   cm_takePhotoEvt = false;
volatile INT8U  cm_ack = 0;
volatile INT32U cm_sendFileOffset;

volatile INT8U  CM_paraSetCmd = 0;          // 0x01: day, 640*480. 


/* Private variables ---------------------------------------------------------*/
static  OS_STK   app_taskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK   app_taskStartStk2[APP_TASK_START_STK_SIZE];

const INT8U firmWareVerInfo[] = "\r\nFW=CM.h1v1.8 build on "__TIME__" "__DATE__"\r\n";	 // 

// OS_EVENT  *xmodem_rslt;
// INT8U     error;



/* Private function prototypes -----------------------------------------------*/
#if (OS_VIEW_MODULE == DEF_ENABLED)
    extern void  App_OSViewTaskCreate (void);
#endif


/*
*********************************************************************************************************
*                                            app_taskCreate()
*
* Description : create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : app_taskStart().
*
* Note(s)     : none.
*********************************************************************************************************
*/

/*
static void app_taskCreate (void)
{
#if (OS_VIEW_MODULE == DEF_ENABLED)
    App_OSViewTaskCreate();
#endif    
}
*/
static void EbiConfigure(void)
{
  EBI_Init_TypeDef ebiConfig = EBI_INIT_DEFAULT;

  /* Run time check if we have EBI on-chip capability on this device */
  switch ( SYSTEM_GetPartNumber() )
  {
	/* Only device types EFM32G 280/290/880 and 890 have EBI capability */
	case 280:
	case 290:
	case 880:
	case 890:
	  break;
	default:
	  /* This device do not have EBI capability - use SPI to interface DK */
	  /* With high probability your project has been configured for an */
	  /* incorrect part number. */
	  while (1) ;
  }
  /* Enable clocks */
  CMU_ClockEnable(cmuClock_EBI, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* Configure mode - disable SPI, enable EBI */
  GPIO_PinModeSet(gpioPortC, 13, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortC, 12, gpioModePushPull, 0);
  /* Configure GPIO pins as push pull */
  /* EBI AD9..15 */
  GPIO_PinModeSet(gpioPortA, 12, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 13, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 14, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 10, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 7, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 1, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 4, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 5, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 6, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortE, 7, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 0, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 1, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 2, gpioModePushPull, 0);
  
  /* EBI CS0-CS3 */
  GPIO_PinModeSet(gpioPortD, 9, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 11, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 12, gpioModePushPull, 0);
  /* EBI AD0..7 */
  GPIO_PinModeSet(gpioPortE, 8, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 9, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 11, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 12, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 13, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 14, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortE, 15, gpioModePushPull, 1);
  /* EBI ARDY/ALEN/Wen/Ren */
  //GPIO_PinModeSet(gpioPortF, 2, gpioModePushPull, 1);
  //GPIO_PinModeSet(gpioPortF, 3, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0);

  /* Configure EBI controller, changing default values */
  ebiConfig.mode = ebiModeD8A24ALE;//ebiModeD8A24ALE;//ebiModeD8A8;//ebiModeD16;//;
  ebiConfig.aLow = ebiALowA0;
  ebiConfig.aHigh = ebiAHighA19;
  ebiConfig.location = ebiLocation0;
  //printf("=======%d\n\r", ebiModeD8A24ALE);
  
  /* Enable bank 0 address map 0x80000000, FPGA Flash */
  /* Enable bank 1 address map 0x84000000, FPGA SRAM */
  /* Enable bank 2 address map 0x88000000, FPGA TFT Display (SSD2119) */
  /* Enable bank 3 address map 0x8c000000, FPGA Board Control Registers */
  ebiConfig.banks	= EBI_BANK0;// | EBI_BANK1 | EBI_BANK2 | EBI_BANK3;
  ebiConfig.csLines = EBI_CS0;// | EBI_CS1 | EBI_CS2 | EBI_CS3; /* SRAM 接CS0上，CS0控制Bank0*/

  /* Address Setup and hold time */
  ebiConfig.addrHoldCycles	= 0;
  ebiConfig.addrSetupCycles = 0;

  /* Read cycle times */
  ebiConfig.readStrobeCycles = 4;
  ebiConfig.readHoldCycles	 = 1;
  ebiConfig.readSetupCycles  = 1;
  
  /* Write cycle times */
  ebiConfig.writeStrobeCycles = 3;
  ebiConfig.writeHoldCycles   = 0;
  ebiConfig.writeSetupCycles  = 0;

  //ebiConfig.alePolarity = ebiActiveHigh;
  /* Configure EBI */
  //EBI->ROUTE |= (3 << 6);
  //EBI->CTRL |= (1<<31);
  EBI_Init(&ebiConfig);
}


/*
*********************************************************************************************************
*                                          app_taskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
* Argument(s) : p_arg       Argument passed to 'app_taskStart()' by 'OSTaskCreate()'.
* Return(s)   : none.
* Caller(s)   : This is a task.
* Note(s)     : none.
*********************************************************************************************************
*/    
static void app_taskStart (void *p_arg)
{ 
	OS_CPU_SR cpu_sr;
	
    INT8U  i;
//	INT8U  j;
	INT8U  photoGetTimeOuthmSecCnt;
	INT8U  photoGetSimuhmSecCnt;
  
	bool   retval;	
	
    (void)p_arg; 
    cpu_sr = cpu_sr;	
	
	//---------------------------------------------
    //- Init hardware.
	//---------------------------------------------
    OS_CPU_SysTickInit();                                    /* Initialize the SysTick.                              */

    /* Calibrate the delay */
//  delay_init();

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                          /* Determine CPU capacity.                              */
#endif

//  app_taskCreate();                                      /* Create application tasks.                            */
//  ResetSystem();   
//  ov5640_photodataBuff_clr(0);             // .

	EbiConfigure();



    while(1)
    {
		photoGetStat = PHOTOGET_RESET;	
		photoGetFraCnt = 0;
		photoGetFraCntEn = 0;
		
		LED_lgtOnhmSecCmd = 5;	
		
        OSTimeGet_ref = OSTimeGet(); 
   		printf("%s", firmWareVer);		
//		printf("\r\n");
        printf("prepare to take picture, %d msec, para=%d\r\n", (INT16U)0, (INT16U)CM_paraSetCmd);  

		ov5640_init_hw_xclkOn(true);
		ov5640_init_hw_DVP(true);  		
//      ov5640_init_hw_reset(true);		                           //

		memset((void*)photodataBuff, 0, sizeof(photodataBuff));    // as delay.

		for(i=0; i<3; i++)
		{		
            ov5640_init_hw_reset(true);		                       //
			
			switch(CM_paraSetCmd)
			{
				case 0: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_VGA_640_480);  // .
					    break;
				case 1: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_XGA_1024_768);   // .
					    break;
//				case 2: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_SVGA_800_600);  // .
				case 2: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_UXGA_1600_1200);   // .
					    break;
//				case 3: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_SVGA_800_600);  // .
				case 3: retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_VGA_640_480);   // .
					    break;
				case 4: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_VGA_640_480);  // .
					    break;
				case 5: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_XGA_1024_768);   // .
					    break;
//				case 6: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_SVGA_800_600);  // .
				case 6: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_UXGA_1600_1200);   // .
                        break;					
//				case 7: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_SVGA_800_600);  // .
				case 7: retval = ov5640_init_soft(SCENE_MODE_NIGHT,ov5640_mode_VGA_640_480);   // .
					    break;
				default:retval = ov5640_init_soft(SCENE_MODE_AUTO, ov5640_mode_QVGA_320_240);  // .
					    break;
			}			
            
			if(retval != SUCCESS)
			{
				printf("ov5640 init failed,  %d, %d msec\r\n", (INT16U)i, (INT16U)(OSTimeGet()-OSTimeGet_ref)*10);
                break; 
			}
			else
			{
				photoGetStat = PHOTOGET_INIT_OK;
				printf("ov5640 init success, %d, %d msec\r\n", (INT16U)i, (INT16U)(OSTimeGet()-OSTimeGet_ref)*10);

				photoGetFraCntEn = false;       // 
				photoGetFraCnt = 0;
				photoGetTimeOuthmSecCnt = 0;

				DMA_ov5640_config();	        // ping-pong mode.	
				TIMER1_appInit();               // CC0 trigged DMA. 				
				VSYNC_lastLevel = true;
				VSYNC_currLevel = true;
				photoGetFraCntEn = true;  
				
				while(photoGetStat == PHOTOGET_INIT_OK)                    // .
				{				
					OSTimeDlyHMSM(0, 0, 0, 100);

					photoGetTimeOuthmSecCnt++;
					if(photoGetTimeOuthmSecCnt >= 50)       // .
					{
						photoGetTimeOuthmSecCnt = 0;
				        printf("cm data get failed, %d, %d msec\r\n", (INT16U)i, (INT16U)(OSTimeGet()-OSTimeGet_ref)*10);
                        ov5640_init_hw_reset(false);
						OSTimeDlyHMSM(0, 0, 0, 500);
						OSTimeDlyHMSM(0, 0, 0, 500);
						
						photoGetStat = PHOTOGET_RESET;
						continue;
					}
				}
				ov5640_free_running();
				
				ov5640_init_hw_reset(false);	 // .
				ov5640_init_hw_xclkOn(false);    // .
				ov5640_init_hw_DVP(false);  	

				if(photoGetStat == PHOTOGET_SUCCESS)
				{	
					LED_lgtOnhmSecCmd = 1;	     // idle.	
					CM_paraSetCmd = 1;
		            printf("cm data check success, %d bytes, %d msec\r\n", photobufLen, (INT16U)(OSTimeGet()-OSTimeGet_ref)*10);
				}
				else
				{
					CM_paraSetCmd = 0;
			        printf("cm data check failed, retry again\r\n");
					
					photoGetStat = PHOTOGET_RESET;
					continue;
				}
				
				photoGetSimuhmSecCnt = 0;
				while(photoGetStat == PHOTOGET_SUCCESS)              // wait for data transmit.
				{
					OSTimeDlyHMSM(0, 0, 0, 100);                     // delay 100ms.
					
					photoGetSimuhmSecCnt++;
					if(photoGetSimuhmSecCnt >= 150)	
					{
						photoGetSimuhmSecCnt = 0;
					    photoGetStat = PHOTOGET_RESET;
					}

					if (msg_flag == 1)
					{
						photoGetSimuhmSecCnt = 0;
						msg_flag = 0;
					}
				}				
		        printf("cm data send success, %d msec\r\n", (INT16U)(OSTimeGet()-OSTimeGet_ref)*10);

				break;				
			}					
		}
		OSTimeDly(100);
    }   
}

static void app_taskStart2 (void *p_arg)
{
	INT8U  i;
//	INT16U j;
	INT16U fraLen;
	INT32U fileOffset;
	
	char   *pStr;
//	char   strBuf[32];
//	INT8U  ucTmpVal = 0;
	
	struct cm_fraTakePhoto    *pBuf1;
	struct cm_fraGetPhotoLen  *pBuf2;
	struct cm_fraGetPhotoData *pBuf3;	
	struct cm_fraShutdown     *pBuf4;	
	
    USART_rx_enable(uart0, false);          // .
	USART_rx_strModeSet(uart0, true);       // ASCII mode, support frame check and rx_auto_stop. 
    USART_rx_strIdSet(uart0, "\r\n");       // .
    USART_rx_bufClr(uart0, 32);             // clear all to 0.
    USART_rx_enable(uart0, true);           // .	
	
    while(1)
    {				
		if( USART_rx_strIdIsVld(uart0) )           // .
		{	
			i = 0;
	        for(;;)                                // 将所有的‘0’用0xff进行填充.
            {
				if(uart0->rx_buffer[i] == 0)
				{
					uart0->rx_buffer[i] = 0xff;
					i++;
				}
				else
				{
					break;
				}
            }	
			
			pStr = strstr((char*)(uart0->rx_buffer), "##");
			//11111111111111111111111test
			//printf("%Test:%s\n", (char*)(uart0->rx_buffer));
			msg_flag = 1;
			//11111111111111111111111
			if(pStr)
			{
				if(pStr[2] == CM_FRATYPE_TAKE_PHOTO)
				{
					pBuf1 = (struct cm_fraTakePhoto *)pStr;
					if(checksum_verify((char *)pBuf1, sizeof(struct cm_fraTakePhoto)-2))
					{
						CM_paraSetCmd = pBuf1->para; 
			
						//--------------------------------------------------
						//- ack to MB.
						//--------------------------------------------------
						cm_ack = CM_ACK_OK; 
						cm_takePhotoAck_fraBulid((char *)cmBuf); 
						USART_putbinary(uart0, (char *)cmBuf, sizeof(struct cm_fraTakePhotoAck));
						USART_putbinary(uart0, "\r\n", 2);

						photoGetStat = PHOTOGET_RESET;
					}
				}
				else if(pStr[2] == CM_FRATYPE_GET_PHOTO_LEN) 
				{
					pBuf2 = (struct cm_fraGetPhotoLen *)pStr;
					if(checksum_verify((char *)pBuf2, sizeof(struct cm_fraGetPhotoLen)-2))
					{
						if(photoGetStat == PHOTOGET_SUCCESS)
						{
							cm_getPhotoLenAck_fraBulid((char *)cmBuf, photobufLen); 
							USART_putbinary(uart0, (char *)cmBuf, sizeof(struct cm_fraGetPhotoLenAck));
						}
						else
						{
							cm_getPhotoLenAck_fraBulid((char *)cmBuf, 0); 
							USART_putbinary(uart0, (char *)cmBuf, sizeof(struct cm_fraGetPhotoLenAck));
						}
					    USART_putbinary(uart0, "\r\n", 2);
					}
				}
				else if(pStr[2] == CM_FRATYPE_GET_PHOTO_DATA) 
				{
					pBuf3 = (struct cm_fraGetPhotoData *)pStr;										
					if(checksum_verify((char *)pBuf3, sizeof(struct cm_fraGetPhotoData)-2))
					{						
						if(photoGetStat == PHOTOGET_SUCCESS)
						{
							fileOffset = pBuf3->fileOffset.dword_u;
							cm_getPhotoDataAck_fraBulid((char *)cmBuf, fileOffset, &fraLen); 
							USART_putbinary(uart0, (char *)cmBuf, fraLen);
							USART_putbinary(uart0, "\r\n", 2);	
						}							
					}					
				}
				else if(pStr[2] == CM_FRATYPE_SHUT_DOWN) 
				{
					pBuf4 = (struct cm_fraShutdown *)pStr;
					if(checksum_verify((char *)pBuf4, sizeof(struct cm_fraShutdown)-2))
					{

					}
				}
				else
				{
					                                // do nothing.
				}
			}
			USART_rx_bufClr(uart0, 16);              // clear all to 0.
			USART_rx_enable(uart0, true);            // .
		}
		
		LED_lgthmSecCnt++;
		if(LED_lgthmSecCnt >= LED_lgtOnhmSecCntMax)  // 10.
		{
			LED_lgthmSecCnt = 0;
			LED_lgtOnFlg = 0;
		}
		else
		{
			if(LED_lgthmSecCnt < LED_lgtOnhmSecCmd)
			{
				if(LED_lgtOnFlg)
				{
					LED_lgtOnFlg = 0; 
					LED_RUN_LIGHT_OFF;
				}
				else
				{
					LED_lgtOnFlg = 1; 
					LED_RUN_LIGHT_ON;					
				}
			}
			else
			{
				LED_RUN_LIGHT_OFF;
			}
		}		
		//printf("test = %d\n", test);
		OSTimeDlyHMSM(0, 0, 0, 100);                 // delay 100ms.
//		OSTimeDly(1);
		WDOG_Feed();
    }
}


int main(void)
{
	CPU_INT08U  os_err;
	INT8U i;
	INT8U j;
	
    INT8U  CM_paraSetTmp[2];
//	INT16U CM_paraGetTimeOutCnt = 0;
	INT16U CM_statHighLevlCnt = 0;

//	bool CM_nightStat = 0;           // day.
//	bool CM_picSizeMode = 0;         // 320*240.	
	
	/* chip errata */
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);	  // Enable HFXO (CMOS), 48MHz.	
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);  // HFCLK = HFXO.	
	CMU_ClockDivSet(cmuClock_CORE,  cmuClkDiv_1);     // the core clock = HFCLK/1 (div-1).	
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_1);     // the peripheral clock = HFCLK/1 (div-1).
    CMU_ClockEnable(cmuClock_GPIO, true);             // 
    GPIO_appInit();	

//	while(1);
/*	
	for(;;)
	{
		LED_RUN_LIGHT_ON;
//		delay_us(100);
		delay_ms(1);
		LED_RUN_LIGHT_OFF;
//		delay_us(100);
		delay_ms(1);
	}	
*/
	
	//---------------------------------------------------------------
	//- TIMER0 init, 200uS, interrupt enabled.
	//---------------------------------------------------------------
    TIMER0_appInit();

	//---------------------------------------------------------------
	//- UART0 init, @T0 ints.
	//---------------------------------------------------------------
    UART0_appInit(57600);                         // 
    WDOG_appInit();
//	while(1);

    CM_paraSetTmp[0] = 0;
    CM_paraSetTmp[1] = 0;

//	GPIO_PinOutSet(gpioPortF, 8);
/*

	CM_STAT_SET;
	while(CM_STAT == 0);
	while(CM_STAT == 1);
	while(CM_STAT == 0);
	while(CM_STAT == 1);
	
//	while((GPIO->P[gpioPortF].DIN & 0x0100) == 0); // 等待出现高电平.
//	while(GPIO->P[gpioPortF].DIN & 0x0100);        // 等待出现低电平.
//	while((GPIO->P[gpioPortF].DIN & 0x0100) == 0); // 等待出现高电平.
//	while(GPIO->P[gpioPortF].DIN & 0x0100);        // 等待出现低电平.
	
	for(i=0; i<10; i++)
	{
		CM_statHighLevlCnt = 0;                        // 
		
		for(j=0; j<25; j++)                            // period: 25*1=25ms.
		{
//			if(GPIO->P[gpioPortF].DIN & 0x0100)
			if(CM_STAT == 1)  
			{
				CM_statHighLevlCnt++;                  // 
			}
			delay_ms(1);
		}
		CM_paraSetTmp[i & 0x1] = CM_statHighLevlCnt;   // .

		if(i > 2)
		{
			if(CM_paraSetTmp[0] == CM_paraSetTmp[1])
			{
				switch(CM_paraSetTmp[0])
				{
					case 1:
					case 2:
					case 3: //CM_nightStat = 0;           // day.
							//CM_picSizeMode = 0;         // 320*240.	
                            CM_paraSetCmd = 0;					
							break;
					case 4:
					case 5:
					case 6: //CM_nightStat = 0;           // day.
							//CM_picSizeMode = 1;         // 640*480.					
                            CM_paraSetCmd = 1;					
							break;
					case 7:
					case 8:
					case 9: //CM_nightStat = 0;           // day.
							//CM_picSizeMode = 1;         // 800*600.					
                            CM_paraSetCmd = 2;					
							break;
					case 10:
					case 11:
					case 12://CM_nightStat = 0;           // day.
							//CM_picSizeMode = 1;         // 1024*768.					
                            CM_paraSetCmd = 3;					
							break;		
					case 13:
					case 14:
					case 15://CM_nightStat = 1;           // night.
							//CM_picSizeMode = 0;         // 320*240.					
                            CM_paraSetCmd = 4;					
							break;
					case 16:
					case 17:
					case 18://CM_nightStat = 1;           // night.
							//CM_picSizeMode = 1;         // 640*480.					
                            CM_paraSetCmd = 5;					
							break;
					case 19:
					case 20:
					case 21://CM_nightStat = 1;           // night.
							//CM_picSizeMode = 1;         // 800*600.					
                            CM_paraSetCmd = 6;					
							break;
					case 22:
					case 23:
					case 24://CM_nightStat = 1;           // night.
							//CM_picSizeMode = 1;         // 1024*768.					
                            CM_paraSetCmd = 7;					
							break;						
					default:break;
				}
				
				break;
			}
		}		
	}
*/
	
	
//  SCB->CCR |= 0x31B;       /* Open fault */	
//  __disable_fault_irq();
    os_err = os_err;         /* prevent warning... */

    /* note: */
    CPU_IntDis();            /* disable all ints */
    OSInit();                /* initialize "uC/OS-II, The Real-Time Kernel" */
	
//*	
    os_err = OSTaskCreateExt((void (*)(void *)) app_taskStart,   // Create the start task. 
                             (void          * ) 0,
                             (OS_STK        * )&app_taskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO3,
                             (INT16U          ) APP_TASK_START_PRIO3,
                             (OS_STK        * )&app_taskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
//*/
							 
//*    
    os_err = OSTaskCreateExt((void (*)(void *)) app_taskStart2,  // Create the start task. 
                             (void          * ) 0,
                             (OS_STK        * )&app_taskStartStk2[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO4,
                             (INT16U          ) APP_TASK_START_PRIO4,
                             (OS_STK        * )&app_taskStartStk2[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
//*/
							 
#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_START_PRIO3, (CPU_INT08U *)"Start Task", &os_err);
#endif

    OSStart();               /* Start multitasking (i.e. give control to uC/OS-II).  */
    return (0);  
}


