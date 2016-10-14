#ifndef __SETUP_H_
    #define __SETUP_H_

    #define HW_MODEL    "A1"
    #define SW_VERSION  "V0.1"
    //#define CAMERA_SN    "234324345"
    extern char strIMEI[24];

    #define BOOTLOADER_START_ADDR    0x00
    #define BOOTLOADER_SIZE                 0xB000      // 44KB

    #define APPLICATION_START_ADDR    BOOTLOADER_SIZE
    #define APPLICATION_SIZE                 0x3A800    // 234KB

    #define APP_CONSOLE_TIMEOUT          (10*HZ)
    #define APP_CONSOLE_DEBUG_PATTERN_TIMEOUT          (1*HZ)


    #define BOOTLOADER_SRAM_VTOR       0x20000000

    #define EXTERN_SRAM_ADDR           0x80000000
    #define EXTERN_SRAM_SIZE           0x80000

    #define FLASH_ADDR             0x0
    #define FLASH_SIZE             0x80000
    
    #define DOWNLOAD_BUF_ADDR         EXTERN_SRAM_ADDR
    #define DOWNLOAD_BUF_SIZE         EXTERN_SRAM_SIZE
    
    /*Define JPEG resolution, align with ov5640_mode*/
    #define JPEG_640_480    0
    #define JPEG_800_600    1
    #define JPEG_1024_768  2
    #define MAX_JPEG_SIZE  JPEG_1024_768

    /*Define Tansfer type*/
    #define TRANSFER_TCP_TYPE   0
    #define TRANSFER_UDP_TYPE   1
    #define MAX_TRANSFER_TYPE  TRANSFER_UDP_TYPE


    /*Define Upgrade flat*/
    #define UPGRADE_FLAG_OFF    0
    #define UPGRADE_FLAG_ON    1


    #define DEFAULT_JPEG_SIZE   JPEG_640_480
    #define DEFAULT_SERVER_IP   "121.41.25.64"
    #define DEFAULT_SERVER_PORT   8688
    #define DEFAULT_TRANSFER_TYPE   TRANSFER_TCP_TYPE
    #define DEFAULT_GPRS_APN    "CMNET"
    #define DEFAULT_GPRS_USRNAME    ""
    #define DEFAULT_GPRS_PASSWD    ""
    #define DEFAULT_UPGRADE_FLAG    0

    typedef struct
    {
        int  PhotoResolution; 
        char ServerIP[16];
        int ServerPort;
        int TransportType;  
        char GPRSApn[16];
        char GPRSUsrname[16];
        char GPRSUsrpasswd[16];
        int RemoteUpgradeAppFW; /*0:Default, no upgrade, 1:Upgrade fw*/
        char AppFWName[16]; /*App FW name*/
    }SYS_PARM;

    extern SYS_PARM sysParm;
    void vInitSetup(void);
    void vResolveParam(void);

#endif
