
#ifndef __OV5640_H
#define __OV5640_H


/*
typedef enum 
{
    false = 0, 
	true  = !false 
} bool;
*/


#define OV5640_I2C_ADDR           0x78
#define OV5460_CHIP_ID            0x5640


// #undef  pr_err
// #define OV5640_DEBUG_INFO_PRINTF            0x5640


#ifdef OV5640_DEBUG_INFO_PRINTF
	#define pr_err    printf
	#define pr_debug  printf
	#define pr_info   printf
#else
	#define pr_err    ob5640_printf
	#define pr_debug  ob5640_printf
	#define pr_info   ob5640_printf
#endif


/* use limited buffer size to capture JPEG */
#define CAMERA_LIMIT_BUFFER_SIZE


// #define PCLK_AND_NOT_GATE_ENABLE    // XXXX = PCLK & REF

#define OV5640_DMA_ENABLE
#define OV5640_JPEG_ENABLE
#define OV5640_SNAPSHOT_ENABLE
// #define OV5640_TEST_PATTERN_ENABLE


// #define PREVIEW_FRAME_RATE    4       // unit: fps
#define PREVIEW_FRAME_RATE    15   // unit: fps


#define CAPTURE_FRAME_RATE       300    // unit: fps*100
// #define CAPTURE_FRAME_RATE    750    // unit: fps*100
// #define CAPTURE_FRAME_RATE    500    // unit: fps*100


// #define PHOTO_DATA_BUFF_ADDR         0x20009000     // .
#define PHOTO_DATA_BUFF_SIZE         500*1024UL     // 80KB,
#define PHOTO_DATA_BUFF_SIZE_LESS    499*1024UL     // 80KB,
// #define PHOTO_DATA_BUFF_SIZE     100*1024UL     // 100KB,
#define DMA_TRANS_BLOCK_SIZE            1024UL     // 1KB. 
#define PHOTO_FRA_VALID_NO                 3       // grab frame number


#define JPEG_SOI_CHAR0    0xFF
#define JPEG_SOI_CHAR1    0xD8
#define JPEG_EOI_CHAR0    0xFF
#define JPEG_EOI_CHAR1    0xD9


//-----------------------------------------------------------------------
//- DVP interface.
//-----------------------------------------------------------------------
// PIN, RST: PD8
#define OV5640_DVP_RESET_PORT          gpioPortD
#define OV5640_DVP_RESET_PIN           8

// PIN, DVP: PD0~PD7
#define OV5640_DVP_DATA_PORT           gpioPortD

// PIN, VSYNC: PC3
#define OV5640_DVP_VSYNC_PORT          gpioPortC
#define OV5640_DVP_VSYNC_PIN           11
#define OV5640_DVP_VSYNC_IRQ_NUM       GPIO_ODD_IRQn
#define OV5640_DVP_VSYNC_PIN_VAL       1 << OV5640_DVP_VSYNC_PIN

// #define OV5640_DVP_VSYNC_RISE_ACTIVE   true            // NOT SURE.
// PIN, PCLK: PC13
#define OV5640_DVP_PCLK_PORT           gpioPortC
#define OV5640_DVP_PCLK_PIN            13

#define OV5640_RESET_OUT_HIGH  GPIO->P[gpioPortD].DOUTSET = (1 << OV5640_DVP_RESET_PIN)  /* GPIOB->BSRR = GPIO_Pin_10  GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define OV5640_RESET_OUT_LOW   GPIO->P[gpioPortD].DOUTCLR = (1 << OV5640_DVP_RESET_PIN)  /* GPIOB->BRR = GPIO_Pin_10   GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

// #define CAMERA_DBG
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifdef CAMERA_DBG
    #define CAMERA_TRACE(x) (printf)x
#else
    #define CAMERA_TRACE(x)
#endif

#define ov5640_VOLTAGE_ANALOG           2800000
#define ov5640_VOLTAGE_DIGITAL_CORE     1500000
#define ov5640_VOLTAGE_DIGITAL_IO       1800000

/* check these values! */
#define FPS_MIN                 15
#define FPS_MAX                 30
#define FPS_DEFAULT             30

#define OV5640_XCLK_MIN         6000000
#define OV5640_XCLK_MAX         24000000

#define OV5640_PWDN_DISABLE     0
#define OV5640_PWDN_ENABLE      1


enum ov5640_mode
{
    ov5640_mode_QVGA_160_120    = 0,
    ov5640_mode_QVGA_320_240    = 1,
    ov5640_mode_VGA_640_480     = 2,
    ov5640_mode_SVGA_800_600    = 3,
    ov5640_mode_XGA_1024_768    = 4,
    ov5640_mode_SXGA_1280_960   = 5,
    ov5640_mode_UXGA_1600_1200  = 6,
    ov5640_mode_QXGA_2048_1536  = 7,
    ov5640_mode_QSXGA_2592_1944 = 8,
    ov5640_mode_MAX             = 9,
};


enum flash_mode
{
    FLASH_MODE_AUTO,
    FLASH_MODE_ON,
    FLASH_MODE_OFF,
};


// + for auto focus
/*
 * Command and Status Registers
 * If the command needs parameters, the main command must be the last register to be sent.
 * The MCU will auto clear CMD_MAIN to zero after the command is receipt,
 * and auto clear CMD_ACK to zero when the command is completed.
 */
#define AF_CMD_MAIN_REG      0x3022    // main command, W
#define AF_CMD_ACK_REG       0x3023    // ACK of command, R/W
#define AF_CMD_PARA0_REG     0x3024    // Parameter: Byte 0, R/W
#define AF_CMD_PARA1_REG     0x3025    // Parameter: Byte 1, R/W
#define AF_CMD_PARA2_REG     0x3026    // Parameter: Byte 2, R/W
#define AF_CMD_PARA3_REG     0x3027    // Parameter: Byte 3, R/W 
#define AF_CMD_PARA4_REG     0x3028    // Parameter: Byte 4, R/W
#define AF_FW_STATUS_REG     0x3029    // Status of focus, R

/* 
 * Running Status of Firmware
 * FW_STATUS is the current focus state.
 */
#define AF_S_FIRWARE       0x7F    // Firmware is downloaded and not run
#define AF_S_STARTUP       0x7E    // Firmware is initializing
#define AF_S_IDLE          0x70    // Idle state, focus is released, lens is located at the furthest position
#define AF_S_FOCUSING      0x00    // Auto Focus is running
#define AF_S_FOCUSED       0x10    // Auto Focus is completed

/* 
 * Commands
 */
#define AF_TRIG_SINGLE_AUTO_FOCUS           0x03
#define AF_CONTINUE_AUTO_FOCUS              0x04
#define AF_PAUSE_AUTO_FOCUS                 0x06
#define AF_GET_FOCUS_RESULT                 0x07    // this command is reserved
#define AF_RELEASE_FOCUS                    0x08
#define AF_RE_LAUNCH_FOCUS_ZONES            0x12
#define AF_WRITE_VCM_DRIVER_IC              0x18    // for ov5640, the command is not supported.
#define AF_READ_VCM_DRIVER_IC               0x19    // for ov5640, the command is not supported.
#define AF_SET_VCM_STEP                     0x1a
#define AF_GET_VCM_STEP                     0x1b
#define AF_RE_LAUNCH_DEFAULT_FOCUS_ZONES    0x80
#define AF_LAUNCH_TOUCH_MODE_FOCUS_ZONES    0x81
#define AF_ENABLE_CUSTOM_FOCUS_ZONE_CONFIG  0x8f
#define AF_CONFIG_FOCUS_ZONE0               0x90
#define AF_CONFIG_FOCUS_ZONE1               0x91
#define AF_CONFIG_FOCUS_ZONE2               0x92
#define AF_CONFIG_FOCUS_ZONE3               0x93
#define AF_CONFIG_FOCUS_ZONE4               0x94
#define AF_LAUNCH_CUSTOM_FOCUS_ZONES        0x9f
#define AF_WEIGHTS_OF_FOCUS_ZONES           0x98

enum ov5640_focus_zone_mode 
{
//  Embedded Zone Configuration
    EMBEDDED_FIXED_ZONE_MODE,
    EMBEDDED_TOUCH_ZONE_MODE,
    EMBEDDED_CUSTOM_SINGLE_ZONE_MODE,
    EMBEDDED_CUSTOM_MULTI_ZONES_MODE,
//  External Zone Configuration
    ETERNAL_SINGLE_ZONE_MODE,
    ETERNAL_MULTI_ZONES_MODE,
};

enum sensor_format 
{
    SENSOR_FORMAT_PREVIEW,
    SENSOR_FORMAT_STILL,
};

enum color_effect 
{
    COLOR_EFFECT_NONE,
    COLOR_EFFECT_MONO,
    COLOR_EFFECT_NEGATIVE,
    COLOR_EFFECT_SOLARIZE,
    COLOR_EFFECT_SEPIA,
    COLOR_EFFECT_AQUA,
    COLOR_EFFECT_REDTINT,
    COLOR_EFFECT_BLUETINT,
    COLOR_EFFECT_GREENTINT,
};

enum white_balance 
{
    WHITE_BALANCE_AUTO,
    WHITE_BALANCE_INCANDESCENT,
    WHITE_BALANCE_FLUORESCENT,
    WHITE_BALANCE_WARM_FLUORESCENT,
    WHITE_BALANCE_DAYLIGHT,
    WHITE_BALANCE_CLOUDY_DAYLIGHT,
    WHITE_BALANCE_TWILIGHT,
    WHITE_BALANCE_SHADE,
};

enum image_iso 
{
    IMAGE_ISO_AUTO,
    IMAGE_ISO_100,
    IMAGE_ISO_200,
    IMAGE_ISO_400,
    IMAGE_ISO_800,
};

#define ANTIBANDING_AUTO        0
#define ANTIBANDING_50HZ        1
#define ANTIBANDING_60HZ        2
#define ANTIBANDINT_OFF         3
#define DEFAULT_ANTIBANDING     ANTIBANDING_50HZ

enum mirror_flip 
{
    MIRRORFLIP_NORMAL,
    MIRRORFLIP_MIRROR,
    MIRRORFLIP_FLIP,
    MIRRORFLIP_MIRROR_FLIP,
};

enum exposure_mode 
{
    EXPOSURE_MODE_WEIGHT_CENTER_UPPER,
    EXPOSURE_MODE_FRAME_AVERAGE,
    EXPOSURE_MODE_CENTER_QUATER_IMAGE_AVERAGE,
    EXPOSURE_MODE_SPOT_METERING,
};

enum compressquality 
{
    COMPRESSQUALITY_HIGH,
    COMPRESSQUALITY_NORMAL,     // default
    COMPRESSQUALITY_LOW,
};

enum scene_mode 
{
    SCENE_MODE_AUTO,            // default
    SCENE_MODE_NIGHT,
};

enum focus_mode 
{
    FOCUS_MODE_AUTO,            // default
    FOCUS_MODE_INFINITY,
    FOCUS_MODE_MACRO,
};

enum image_brightness 
{
    IMAGE_BRIGHTNESS_P4,        // +4
    IMAGE_BRIGHTNESS_P3,        // +3
    IMAGE_BRIGHTNESS_P2,        // +2
    IMAGE_BRIGHTNESS_P1,        // +1
    IMAGE_BRIGHTNESS_P0,        // normal
    IMAGE_BRIGHTNESS_N1,        // -1
    IMAGE_BRIGHTNESS_N2,        // -2
    IMAGE_BRIGHTNESS_N3,        // -3
    IMAGE_BRIGHTNESS_N4,        // -4
};

enum image_contrast 
{
    IMAGE_CONTRAST_P4,          // +4
    IMAGE_CONTRAST_P3,          // +3
    IMAGE_CONTRAST_P2,          // +2
    IMAGE_CONTRAST_P1,          // +1
    IMAGE_CONTRAST_P0,          // normal
    IMAGE_CONTRAST_N1,          // -1
    IMAGE_CONTRAST_N2,          // -2
    IMAGE_CONTRAST_N3,          // -3
    IMAGE_CONTRAST_N4,          // -4
};

enum image_saturation 
{
    IMAGE_SATURATION_P4,        // +4
    IMAGE_SATURATION_P3,        // +3
    IMAGE_SATURATION_P2,        // +2
    IMAGE_SATURATION_P1,        // +1
    IMAGE_SATURATION_P0,        // normal
    IMAGE_SATURATION_N1,        // -1
    IMAGE_SATURATION_N2,        // -2
    IMAGE_SATURATION_N3,        // -3
    IMAGE_SATURATION_N4,        // -4
};

enum image_hue 
{
    IMAGE_HUE_N180_DEGREE,      // -180 degree
    IMAGE_HUE_N150_DEGREE,      // -150 degree
    IMAGE_HUE_N120_DEGREE,      // -120 degree
    IMAGE_HUE_N90_DEGREE,       // -90 degree
    IMAGE_HUE_N60_DEGREE,       // -60 degree
    IMAGE_HUE_N30_DEGREE,       // -30 degree
    IMAGE_HUE_P0_DEGREE,        // +0 degree
    IMAGE_HUE_P30_DEGREE,       // +30 degree
    IMAGE_HUE_P60_DEGREE,       // +60 degree
    IMAGE_HUE_P90_DEGREE,       // +90 degree
    IMAGE_HUE_P120_DEGREE,      // +120 degree
    IMAGE_HUE_P150_DEGREE,      // +150 degree
};

enum exposurelevel 
{
    EXPOSURELEVEL_N1_7EV,       // -1.7EV
    EXPOSURELEVEL_N1_3EV,       // -1.3EV
    EXPOSURELEVEL_N1_0EV,       // -1.0EV
    EXPOSURELEVEL_N0_7EV,       // -0.7EV
    EXPOSURELEVEL_N0_3EV,       // -0.3EV
    EXPOSURELEVEL_P0_0EV,       // +0.0EV (default)
    EXPOSURELEVEL_P0_3EV,       // +0.3EV
    EXPOSURELEVEL_P0_7EV,       // +0.7EV
    EXPOSURELEVEL_P1_0EV,       // +1.0EV
    EXPOSURELEVEL_P1_3EV,       // +1.3EV
    EXPOSURELEVEL_P1_7EV,       // +1.7EV
};

enum image_sharpness 
{
    IMAGE_SHARPNESS_AUTO,       // default
    IMAGE_SHARPNESS_MANUAL_OFF, // manual off
    IMAGE_SHARPNESS_MANUAL_P1,  // manual +1
    IMAGE_SHARPNESS_MANUAL_P2,  // manual +2
    IMAGE_SHARPNESS_MANUAL_P3,  // manual +3
    IMAGE_SHARPNESS_MANUAL_P4,  // manual +4
    IMAGE_SHARPNESS_MANUAL_P5,  // manual +5
    IMAGE_SHARPNESS_MANUAL_P6,  // manual +3
    IMAGE_SHARPNESS_MANUAL_P7,  // manual +4
    IMAGE_SHARPNESS_MANUAL_P8,  // manual +5
};

enum test_pattern 
{
    TEST_PATTERN_OFF,
    TEST_PATTERN_COLOR_BAR,
    TEST_PATTERN_RANDOM_DATA,
    TEST_PATTERN_SQUARE_DATA,
    TEST_PATTERN_BLACK_IMAGE,
};

struct reg_value 
{
    INT16U reg;
    INT8U val;
};

struct ov5640_mode_info 
{
    enum ov5640_mode mode;
    INT32U width;
    INT32U height;
    struct reg_value *init_data_ptr;
    INT32U init_data_size;
};

enum zoom_format 
{
    ZOOM_FORMAT_SXGA,
    ZOOM_FORMAT_QSXGA,
};


struct v4l2_pix_format
{
    INT16U width;
    INT16U height;
};

struct mxc_camera_platform_data
{
    INT8U i;
    INT8U j;
};

struct i2c_client
{
    INT8U i;
    INT8U j;
};

struct v4l2_captureparm
{
    INT8U capturemode;
};

struct v4l2_int_device
{
    INT8U i;
    INT8U j;
};


/*!
 * Maintains the information on the current state of the sesor.
 */
struct sensor 
{
    const struct mxc_camera_platform_data *platform_data;
    struct v4l2_int_device *v4l2_int_device;
    struct i2c_client *i2c_client;
    struct v4l2_pix_format pix;
    struct v4l2_captureparm streamcap;
    INT8U  on;

    /* control settings */
    INT16U brightness;
    INT16U hue;
    INT16U contrast;
    INT16U saturation;
    INT16U red;
    INT16U green;
    INT16U blue;
    INT16U ae_mode;
    INT16U flash_mode;
    INT16U af_mode;
    INT16U zoom_level;
    INT16U scene_mode;
    INT8U  preview_brightness;   // for flashlight
};


void ov5640_init_hw_xclkOn(bool enable);
void ov5640_init_hw_reset(bool enable);
void ov5640_init_hw_DVP(bool enable);
void ov5640_init_hw_DMA(INT16U skipFrame);
void ov5640_init_hw_VSYNC(void);
void ov5640_photodataBuff_clr(INT32U bufsz);


bool ov5640_init_soft(INT8U scene_mode, INT8U capture_mode);
bool ov5640_free_running(void);
void ov5640_set_jpeg_2592_1944(void);
void ov5640_set_jpeg_320_240(void);


void DMA_ov5640_config(void);
void DMA_ov5640_activatePingPong(void);
void DMA_ov5640_refreshPingPong(bool enable);
void DMA_photoDataGet_callBack(INT32U channel, bool primary, void *user);



bool jpegFileCheck(INT8U *pBuf, INT32U bufLen, INT8U jpegFraStart, INT8U* *pSOI, INT32U *fileLen);
/*
bool jpegFileCheckAtRingBuffer(INT8U *pBuf, INT32U BufLen, INT8U jpegFraStart, void* *pSOI, 
	                           void* *pEOI, INT32U *FileLen, INT32U CurPos);
*/




/*
    step1: RESET is applied to ov5640 camera module. PWDN is pulled high.

    if( ov5640_data.platform_data->reset ) 
	{
      ov5640_data.platform_data->reset(0);
	}
    if (ov5640_data.platform_data->pwdn)
	{
      ov5640_data.platform_data->pwdn(1);
	}

    step2: DOVDD and AVDD powers are applied. 
    The 2 powers could be applied simultaneously.
    If applied separately, the power on sequence should be DOVDD first, and AVDD last.

    step3: after 5ms of AVDD reaching stable, pull PWDN to low.
    delay_os_ms(10);
    if (ov5640_data.platform_data->pwdn)
	{
      ov5640_data.platform_data->pwdn(0);
    }

    step4: after 1ms of PWDN go low, pull high RESET.
    delay_os_ms(10);
    if( ov5640_data.platform_data->reset )
	{
      ov5640_data.platform_data->reset(1);
	}

    step5: after 20ms, initialize ov5640 by SCCB initialization.
    delay_os_ms(20);
    retval = ov5640_init_soft_powerOn();
	
    if(retval == SUCCESS) 
	{
        delay_os_ms(10);
        retval = ov5640_init_embedded_firmware();
    }

    step6: pull high PWDN, set ov5640 to power down mode.
    delay_os_ms(10);
    if (ov5640_data.platform_data->pwdn)
	{
      ov5640_data.platform_data->pwdn(1);
	}
    step7: pull XCLK low. 	
*/	


#endif         /* __OV5640_H */
