/*
 * stm32f401xx_gpio_driver.h
 *
 *  Created on: Oct 7, 2026
 *      Author: lenunez
 */

#ifndef INC_STM32F401XX_GPIO_DRIVER_H_
#define INC_STM32F401XX_GPIO_DRIVER_H_

#include "stm32f401xx.h"

/**
 * Configuration structure for a GPIO pin
 */
typedef struct
{
    uint8_t GPIO_PinNumber;
    uint8_t GPIO_PinMode;
    uint8_t GPIO_PinSpeed;
    uint8_t GPIO_PinPuPdControl;
    uint8_t GPIO_PinOpType;
    uint8_t GPIO_PinAltFunMode;
} GPIO_PinConfig_t;

/**
 * Handle structure for a GPIO pin
 */
typedef struct
{
    GPIO_RegDef_t *pGPIOx;              /* Base address of the GPIO port to which the pin belongs */
    GPIO_PinConfig_t GPIO_PinConfig;    /* GPIO pin configuration settings */
} GPIO_Handle_t;


/**************************************************************************************************
 *                                          GPIO Macros
 **************************************************************************************************/

#define GPIO_PIN_NUM_0              0
#define GPIO_PIN_NUM_1              1
#define GPIO_PIN_NUM_2              2 
#define GPIO_PIN_NUM_3              3 
#define GPIO_PIN_NUM_4              4 
#define GPIO_PIN_NUM_5              5
#define GPIO_PIN_NUM_6              6
#define GPIO_PIN_NUM_7              7
#define GPIO_PIN_NUM_8              8 
#define GPIO_PIN_NUM_9              9 
#define GPIO_PIN_NUM_10             10 
#define GPIO_PIN_NUM_11             11
#define GPIO_PIN_NUM_12             12
#define GPIO_PIN_NUM_13             13
#define GPIO_PIN_NUM_14             14 
#define GPIO_PIN_NUM_15             15 

#define GPIO_MODE_IN                0
#define GPIO_MODE_OUT               1
#define GPIO_MODE_ALTFN             2
#define GPIO_MODE_ANALOG            3
#define GPIO_MODE_IT_FT             4
#define GPIO_MODE_IT_RT             5
#define GPIO_MODE_IT_RFT            6

#define GPIO_OP_TYPE_PP             0
#define GPIO_OP_TYPE_OD             1

#define GPIO_SPEED_LOW              0
#define GPIO_SPEED_MED              1
#define GPIO_SPEED_HIGH             2
#define GPIO_SPEED_VERY_HIGH        3

#define GPIO_PIN_NO_PUPD            0
#define GPIO_PIN_PU                 1
#define GPIO_PIN_PD                 2




/**************************************************************************************************
 *                                      Function Prototypes
 **************************************************************************************************/

/**
 * Peripheral Clock setup
 */
void GPIO_ClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi);

/**
 * Init and De-Init
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);        /* Uses RCC_AHB1RSTR [0-4]*/

/**
 * Data Read and Write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/**
 * IRQ Configuration and ISR handling
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnOrDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);

/**************************************************************************************************
 *                                      Declarations
 **************************************************************************************************/



#endif /* INC_STM32F401XX_GPIO_DRIVER_H_ */
