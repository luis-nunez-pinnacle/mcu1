/*
 * stm32f401xx_i2c_driver.h
 *
 *  Created on: Oct 15, 2026
 *      Author: lenunez
 */

#ifndef INC_STM32F401XX_I2C_DRIVER_H_
#define INC_STM32F401XX_I2C_DRIVER_H_

#include "stm32f401xx.h"

/**
 * Configuration structure for I2Cx peripheral
 */
typedef struct{
    uint32_t I2C_SCLSpeed;
    uint8_t I2C_DeviceAddress;
    uint8_t I2C_ACKControl;
    uint16_t I2C_FMDutyCycle;
} I2C_Config_t;

/**
 * Handle structure for I2Cx peripheral
 */
typedef struct 
{
    I2C_RegDef_t *pI2Cx;
    I2C_Config_t I2CConfig;
} I2C_Handle_t;



/**************************************************************************************************
 *                                          I2C Macros
 **************************************************************************************************/
#define I2C_SCL_SPEED_SM            100000
#define I2C_SCL_SPEED_FM            400000
#define I2C_SCL_SPEED_FMPLUS        1000000
#define I2C_SCL_SPEED_HSM           3400000

#define I2C_ACK_DISABLE             0
#define I2C_ACK_ENABLE              1

#define I2C_FM_DUTY_2               0               
#define I2C_FM_DUTY_16_9            1

#define I2C_FLAG_SB                 (0 << I2C_SR1_SB)
#define I2C_FLAG_ADDR               (1 << I2C_SR1_ADDR)
#define I2C_FLAG_BTF                (2 << I2C_SR1_BTF)
//#define I2C_FLAG_ADD10            (3 << I2C_SR1_ADD10)
#define I2C_FLAG_STOPF              (4 << I2C_SR1_STOPF)
#define I2C_FLAG_RXNE               (6 << I2C_SR1_RXNE)
#define I2C_FLAG_TXE                (7 << I2C_SR1_TXE)
#define I2C_FLAG_BERR               (8 << I2C_SR1_BERR)
#define I2C_FLAG_ARLO               (9 << I2C_SR1_ARLO)
#define I2C_FLAG_AF                 (10 << I2C_SR1_AF)
#define I2C_FLAG_OVR                (11 << I2C_SR1_OVR)
//#define I2C_PECERR                 (12 << I2C_SR1_PECERR)
#define I2C_FLAG_TIMEOUT            (14 << I2C_SR1_TIMEOUT)


/**************************************************************************************************
 *                                      Function Prototypes
 **************************************************************************************************/
/**
* Peripheral clock setup
*/
void I2C_ClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);


/**
 * Init and De-Init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);        /* Uses RCC_APB1RSTR [21,22,23] for I2C1/2/3 */

/**
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr);

/**
 * IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnOrDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);

/**
 * Other peripheral control APIs
 */

void I2C_PeripheralControl(I2C_RegDef_t  *pI2Cx, uint8_t EnOrDi);
//uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx , uint32_t FlagName);
void I2C_ManageAcking(I2C_RegDef_t  *pI2Cx, uint8_t EnOrDi);

/**
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);







#endif /* INC_STM32F401XX_I2C_DRIVER_H_ */
