
#ifndef __SCCB_H
#define __SCCB_H


/* Includes ------------------------------------------------------------------*/
// #include "em_device.h"

#define GPIO_pin_SCL  (1 << 12)   /* EFM32: PB12 */
#define GPIO_pin_SDA  (1 << 11)   /* EFM32: PB11 */

/* Private define ------------------------------------------------------------*/
#define SCL_H      GPIO->P[1].DOUTSET = GPIO_pin_SCL   /* GPIOB->BSRR = GPIO_Pin_10  GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define SCL_L      GPIO->P[1].DOUTCLR = GPIO_pin_SCL   /* GPIOB->BRR = GPIO_Pin_10   GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

#define SDA_H      GPIO->P[1].DOUTSET = GPIO_pin_SDA   /* GPIOB->BSRR = GPIO_Pin_11  GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA_L      GPIO->P[1].DOUTCLR = GPIO_pin_SDA   /* GPIOB->BRR  = GPIO_Pin_11  GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

#define SCL_read  (GPIO->P[1].DIN & GPIO_pin_SCL)      /* GPIOB->IDR  & GPIO_Pin_10  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_10) */
#define SDA_read  (GPIO->P[1].DIN & GPIO_pin_SDA)      /* GPIOB->IDR  & GPIO_Pin_11  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */


typedef enum 
{
    DISABLE = 0, 
	ENABLE = !DISABLE
} FunctionalState;


/*---------------- private function prototypes ---------------------------*/
void  I2C_init_GPIO(bool enable);
bool  I2C_byteWrite(INT16U addr, INT8U data, INT8U deviceAddr);
bool  I2C_byteRead(INT8U *pBuff, INT16U dataLen, INT8U readAddr, INT8U deviceAddr);
bool  I2C_byteWrite_4Phase(INT16U regAddr, INT8U data, INT8U deviceAddr);
bool  I2C_byteRead_4Phase(INT8U *pBuff, INT16U dataLen, INT16U regAddr, INT8U deviceAddr);


#endif 




/*********************************************************************************
 ** END FILE
 *********************************************************************************/


