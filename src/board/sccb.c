
#include "..\target\target.h"
#include "..\board\board.h"


/*******************************************************************************
* function Name  : I2C_init_GPIO
* description    : I2C�ܽ�����
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
void I2C_init_GPIO(bool enable)
{
	if(enable)
	{
	    GPIO_PinModeSet(gpioPortB, 12, gpioModeWiredAnd, 1);  /* PB12 as I2C_SCLK */	
	    GPIO_PinModeSet(gpioPortB, 11, gpioModeWiredAnd, 1);  /* PB11 as I2C_SDIO */
	}
	else
	{
	    GPIO_PinModeSet(gpioPortB, 12, gpioModeInput, 1);     /* PB12 as I2C_SCLK */	
	    GPIO_PinModeSet(gpioPortB, 11, gpioModeInput, 1);     /* PB11 as I2C_SDIO */
	}
}

/*******************************************************************************
* function Name  : I2C_delay
* description    : �ӳ�ʱ��
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static void I2C_delay(void)
{	
	volatile INT8U i;	

	for(i=3; i>0; i--);
	
//	i = 2;
//	while(i)
	{
//		i--;
	}

//	delay_us(5);      // NOT SURE.	
}

/*******************************************************************************
* function Name  : I2C_start
* description    : None
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static bool I2C_start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if( !SDA_read )
	{
		return FAIL;	    /* SDA��Ϊ�͵�ƽ������æ,�˳� */
	}
	SDA_L;
	I2C_delay();
	if( SDA_read ) 
	{
		return FAIL;	    /* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
	}
	SDA_L;
	I2C_delay();
	return SUCCESS;
}

/*******************************************************************************
* function Name  : I2C_stop
* description    : None
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static void I2C_stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

/*******************************************************************************
* function Name  : I2C_ack
* description    : None
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static void I2C_ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/*******************************************************************************
* function Name  : I2C_noAck
* description    : None
* input          : None
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static void I2C_noAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/*******************************************************************************
* function Name  : I2C_waitAck
* description    : None
* input          : None
* output         : None
* return         : ����Ϊ:=1��ACK, =0��ACK
* attention		 : None
*******************************************************************************/
static bool I2C_waitAck(void) 	
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if( SDA_read )
	{
        SCL_L;
        return FAIL;
	}
	SCL_L;
	return SUCCESS;
}

/*******************************************************************************
* function Name  : I2C_byteSend
* description    : ���ݴӸ�λ����λ
* input          : - data: ���͵�����
* output         : None
* return         : None
* attention		 : None
*******************************************************************************/
static void I2C_byteSend(INT8U data) 
{
    INT8U i=8;
	
    while(i--)
    {
        SCL_L;
        I2C_delay();
		
        if(data & 0x80)  // 
		{
            SDA_H;
		}			
        else
		{			
            SDA_L;
		}			
        data <<= 1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}


/*******************************************************************************
* function Name  : I2C_byteRecv
* description    : ���ݴӸ�λ����λ
* input          : None
* output         : None
* return         : I2C���߷��ص�����
* attention		 : None
*******************************************************************************/
static INT8U I2C_byteRecv(void)  
{ 
    INT8U i = 8;
    INT8U byteRecv = 0;

    SDA_H;				
    while(i--)
    {
        byteRecv <<= 1;      
        SCL_L;
        I2C_delay();
	    SCL_H;
        I2C_delay();	
        if( SDA_read )
        {
            byteRecv |= 0x01;
        }
    }
    SCL_L;
    return byteRecv;
}

/*******************************************************************************
* function Name  : I2C_byteWrite
* description    : дһ�ֽ�����
* input          : - regAddr: ��д���ַ
*           	   - data: ��д������
*                  - deviceAddr: ��������
* output         : None
* return         : ����Ϊ:=1�ɹ�д��,=0ʧ��
* attention		 : None
*******************************************************************************/           
bool I2C_byteWrite(INT16U regAddr, INT8U data, INT8U deviceAddr)
{		
    if( !I2C_start() )
	{
	    return FAIL;
	}
    I2C_byteSend( deviceAddr );                  /* ������ַ */
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}
    I2C_byteSend((INT8U)(regAddr & 0x00ff));     /* ���õ���ʼ��ַ */      
    I2C_waitAck();	
    I2C_byteSend(data);
    I2C_waitAck();   
    I2C_stop(); 
	
	/* �ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms) */
/*
	#ifdef OS_UCOSII_USED			
	OSTimeDlyHMSM(0, 0, 0, 10);     // 10mS.   
	#else
	delay_ms(10); 
	#endif									
*/ 
    return SUCCESS;
}									 

/*******************************************************************************
* function Name  : I2C_byteRead
* description    : ��ȡһ������
* input          : - pBuff: ��Ŷ�������
*           	   - dataLen: ����������
*                  - regAddr: ��������ַ
*                  - deviceAddr: ��������
* output         : None
* return         : ����Ϊ:=1�ɹ�����, =0ʧ��
* attention		 : None
*******************************************************************************/          
bool I2C_byteRead(INT8U *pBuff, INT16U dataLen, INT8U regAddr, INT8U deviceAddr)
{	
    if( !I2C_start() )
	{
	    return FAIL;
	}
    I2C_byteSend( deviceAddr );          /* ������ַ */
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}
    I2C_byteSend( regAddr );             /* ���õ���ʼ��ַ */      
    I2C_waitAck();	
    I2C_stop(); 
	
    if( !I2C_start() )
	{
		return FAIL;
	}
    I2C_byteSend( deviceAddr + 1 );      /* ������ַ */ 
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}
    while(dataLen)
    {
		*pBuff = I2C_byteRecv();
		if(dataLen == 1)
		{
		  I2C_noAck();
		}
		else
		{
		  I2C_ack(); 
		}
		pBuff++;
		dataLen--;
    }
    I2C_stop();
    return SUCCESS;
}

bool I2C_byteRead_4Phase(INT8U *pBuff, INT16U dataLen, INT16U regAddr, INT8U deviceAddr)
{
	INT8U readAddr;
	
    if( !I2C_start() )
	{
	    return FAIL;
	}
    I2C_byteSend( deviceAddr );         /* ������ַ */
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}

    readAddr = (INT8U)(regAddr >> 8);   /* ���ø���ʼ��ַ */ 
    I2C_byteSend( readAddr );                
    I2C_waitAck();	

    readAddr = (INT8U)(regAddr);        /* ���õ���ʼ��ַ */  
    I2C_byteSend( readAddr );              
    I2C_waitAck();	
	
    I2C_stop(); 
	
    if( !I2C_start() )
	{
		return FAIL;
	}
    I2C_byteSend( deviceAddr+1 );       /* ������ַ */ 
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}
    while(dataLen)
    {
        *pBuff = I2C_byteRecv();
		if(dataLen == 1)
		{
		    I2C_noAck();
		}
		else
		{
		    I2C_ack(); 
		}
		pBuff++;
		dataLen--;
    }
    I2C_stop();
    return SUCCESS;
}

bool I2C_byteWrite_4Phase(INT16U regAddr, INT8U data, INT8U deviceAddr)
{
	INT8U writeAddr;

	if( !I2C_start() )
	{
	    return FAIL;
	}
    I2C_byteSend( deviceAddr );           /* ������ַ */
    if( !I2C_waitAck() )
	{
		I2C_stop(); 
		return FAIL;
	}
    writeAddr = (INT8U)(regAddr >> 8);    /* ���ø���ʼ��ַ */   
    I2C_byteSend(writeAddr);                 
    I2C_waitAck();	
	
    writeAddr = (INT8U)(regAddr & 0xff);  /* ���õ���ʼ��ַ */ 
    I2C_byteSend(writeAddr);                   
    I2C_waitAck();	

    I2C_byteSend(data);
    I2C_waitAck();   
    I2C_stop(); 
	
	/* �ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms) */
/*
	#ifdef OS_UCOSII_USED			
	OSTimeDlyHMSM(0, 0, 0, 10);     // 10mS.   
	#else
	delay_ms(10); 
	#endif	
*/	
    return SUCCESS;
}


/***************************************************************************
 ** END FILE
 ***************************************************************************/

