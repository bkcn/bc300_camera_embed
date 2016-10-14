

#ifndef __TARGET_H
#define __TARGET_H 


#include "..\config\datatypes.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>          // .

#include "em_cmu.h"
#include "em_wdog.h"
#include "em_timer.h"
#include "em_gpio.h"
#include "em_dma.h"
#include "em_usart.h"
#include "em_msc.h"

#include "usart.h"


#include "board.h"



//#include "efm32gg280f512.h"

#ifndef _EFM32_GIANT_FAMILY
#define _EFM32_GIANT_FAMILY
#endif

#ifndef EFM32GG280F512
#define EFM32GG280F512
#endif


#define __at(_addr) __attribute__ ((at(_addr)))


#define UART0_ENABLE      1                   // enable the UART0. 

// #define DELAY_OS_1S       OS_TICKS_PER_SEC    // 100UL.


#define LED_RUN_LIGHT_ON   GPIO_PinOutClear(gpioPortF, 8)
#define LED_RUN_LIGHT_OFF  GPIO_PinOutSet(gpioPortF, 8)
#define CM_STAT            GPIO_PinInGet(gpioPortF, 8)
#define CM_STAT_SET        GPIO_PinOutSet(gpioPortF, 8)

#define SD_UART0_rxRingBuffSize    2048UL     // 2kB.
#define SD_UART0_txRingBuffSize    2048UL     // 2kB.
#define cm_dataBuffSize            1048UL     // 2kB.


extern volatile INT8U  sd_uart0_rx_buffer[SD_UART0_rxRingBuffSize];
extern volatile INT8U  sd_uart0_tx_buffer[SD_UART0_txRingBuffSize];
extern volatile INT8U  cmBuf[cm_dataBuffSize];



//---------------------------------------------------------------------
//- setting the TIMER0
//---------------------------------------------------------------------
// 48MHz (clock frequency) / 16 (prescaler) = 3MHz.
// setting TIMER0_CNTMAX = 600, results in an overflow each 200 milliseconds.
#define TIMER0_CNTMAX            600L
#define DMA_CHAN_CM_DATA_READ      0      // use DMA channal 0.

extern  INT8U*  photodataBuff;
extern  INT8U  test;


//extern volatile INT8U  photodataBuff[PHOTO_DATA_BUFF_SIZE + DMA_TRANS_BLOCK_SIZE * 2];
extern volatile INT8U  *photoBufPtr;
extern volatile INT32U photobufLen;

extern volatile INT32U photoGetFraCnt;
extern volatile bool   photoGetFraCntEn;

extern volatile INT8U  photoGetStat;


extern volatile INT32U cm_sendFileOffset;
extern volatile INT8U  cm_ack;

extern volatile INT32U DMA_photoDataRecvCnt;

extern volatile bool lastPrimary;            // 
extern volatile bool VSYNC_lastLevel;
extern volatile bool VSYNC_currLevel;


/*
#define HW_MODEL    "A1"
#define SW_VERSION  "V0.1"
//#define CAMERA_SN    "234324345"
*/

/* define parameter between bootloader and application */
/*
#define BOOT_PARM_MAGIC         0x1E4C7A8B
#define BOOT_PARM_BUF_ADDR      0x2001ff00 
#define BOOT_PARM_BUF_SIZE      0x100 

#define BOOTLOADER_START_ADDR               0x00
#define BOOTLOADER_SIZE                     0xB000            // 44KB

#define APPLICATION_START_ADDR              BOOTLOADER_SIZE
#define APPLICATION_SIZE                    0x3A800           // 234KB

#define APP_CONSOLE_TIMEOUT                 (10*HZ)
#define APP_CONSOLE_DEBUG_PATTERN_TIMEOUT   ( 1*HZ)

#define BOOTLOADER_SRAM_VTOR       0x20000000
*/

#define EXTERN_SRAM_ADDR           0x80000000
#define EXTERN_SRAM_SIZE           0x80000

#define FLASH_ADDR                 0x0
//#define FLASH_SIZE               0x80000

#define PHOTO_FILELEN_MIN         ( 2*1024UL)
#define PHOTO_FILELEN_MAX         (80*1024UL)


#define TIMEOUT_OS_01S      10	   //   10 * 10mS = 0.1S 
#define TIMEOUT_OS_02S      20	   //   20 * 10mS = 0.2S 
#define TIMEOUT_OS_05S      50	   //   50 * 10mS = 0.5S 
#define TIMEOUT_OS_1S      100	   //  100 * 10mS = 1.0S 
#define TIMEOUT_OS_2S      200	   //  200 * 10mS = 2.0S
#define TIMEOUT_OS_5S      500	   //  500 * 10mS = 5.0S 
#define TIMEOUT_OS_10S    1000     // 1000 * 10mS = 10.0S 


enum photoGet_stat
{
	PHOTOGET_RESET    = 0,
	PHOTOGET_INIT_OK  = 1,
	PHOTOGET_SUCCESS  = 2	
};


extern struct tm sysTime;          //  


extern void sysReset(void);
extern void CMU_CLKOUT0_init(bool enable);
extern void GPIO_appInit(void);
extern void WDOG_appInit(void);
extern void TIMER0_appInit(void);
extern void TIMER1_appInit(void);

extern void EBI_init_GPIO(bool enable);
extern void CM_init_GPIO(bool enable);

// void delay_init(void);
extern void delay_us(INT16U val);
extern void delay_ms(INT16U val);
extern void delay_1s(INT16U val);


extern void DMA_photoDataGet_callBack(INT32U channel, bool primary, void *user);



typedef struct
{
	int  PhotoResolution; 
	char ServerIP[16];
	int  ServerPort;
	int  TransportType;  
	char GPRSApn[16];
	char GPRSUsrname[16];
	char GPRSUsrpasswd[16];
	int  RemoteUpgradeAppFW;   /* 0:Default, no upgrade, 1:Upgrade fw */
	char AppFWName[16];        /* App FW name */
} SYS_PARM;

extern SYS_PARM sysParm;

//extern void vInitSetup(void);
//extern void vResolveParam(void);


#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/






























