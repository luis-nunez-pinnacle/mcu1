/*
 * stm32f401xx_gpio_driver.c
 *
 *  Created on: Oct 7, 2026
 *      Author: lenunez
 */


#include "stm32f401xx_gpio_driver.h"

/**
 * @fn          - GPIO_ClockControl
 * 
 * @brief       - Enables or disables the peripheral clock for a GPIO port
 * 
 * @param[in]   - pGPIOx: Pointer to the GPIO peripheral to control (GPIOA through GPIOE)
 * @param[in]   - EnOrDi: Nonzero to enable the clock; zero to disable it
 * 
 * @return      - None
 * 
 * @note        - This function handles GPIOA, GPIOB, GPIOC, GPIOD, and GPIOE. Other
 *                peripheral addresses are ignored.
 */
void GPIO_ClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnOrDi)
{
    if(EnOrDi)
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_EN();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_EN();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_EN();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_EN();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_EN();
        }
    }   
    else
    {
        if(pGPIOx == GPIOA)
        {
            GPIOA_PCLK_DI();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_PCLK_DI();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_PCLK_DI();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_PCLK_DI();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_PCLK_DI();
        }
    }
}

/**
 * @fn          - GPIO_Init
 * 
 * @brief       - Applies the specified pin configuration to a GPIO port
 * 
 * @param[in]   - pGPIOHandle: Pointer to a handle containing the GPIO port and pin
 *                             configuration (pin number, mode, speed, pull-up/pull-down,
 *                             output type, and alternate-function selection)
 * 
 * @return      - None
 * 
 * @note        - The GPIO peripheral clock must be enabled separately before calling this
 *                function. The current implementation does not validate the handle or its
 *                configuration values. 
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
    uint32_t tmp;

    GPIO_ClockControl(pGPIOHandle->pGPIOx, ENABLE);

    /* Configure mode */
    if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
    {
        tmp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
        pGPIOHandle->pGPIOx->MODER &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); /* clearing reg */
        pGPIOHandle->pGPIOx->MODER |= tmp;
    }
    else
    {
        if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
        {
            EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

            EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        }
        else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
        {
            EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        
            EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        }
        else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
        {
            EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        
            EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
        }

        /* Configure the GPIO port selection in SYSCFG_EXTICR */
        uint8_t tmp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
        uint8_t tmp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
        uint8_t portcode = GPIO_BASSEADDR_TO_PORT_CODE(pGPIOHandle->pGPIOx);
        SYSCFG_PCLK_EN();

        SYSCFG->EXTICR[tmp1] = portcode << (tmp2 * 4);
        

        /* Enable the EXTI interrupt delivery using IMR */
        EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

         
    }
        tmp = 0;

    /* Configure speed */
    tmp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed <<  (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); /* clearing reg*/
    pGPIOHandle->pGPIOx->OSPEEDR |= tmp;
    tmp = 0;

    /* configure pupd settings */
    tmp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
    pGPIOHandle->pGPIOx->PUPDR &= ~(0x3U << (2U * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); /* clearing reg*/
    pGPIOHandle->pGPIOx->PUPDR |= tmp;
    tmp = 0;

    /* configure optype */
    tmp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOpType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandle->pGPIOx->OTYPER &= ~(0x1U << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); /* clearing reg*/
    pGPIOHandle->pGPIOx->OTYPER |= tmp;
    tmp = 0;

    /* configure alt functionality*/
    if((pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN))
    {
        uint8_t tmp1, tmp2;
        tmp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8U;
        tmp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8U;

        pGPIOHandle->pGPIOx->AFR[tmp1] &= ~(0xF << (tmp2 * 4U));
        pGPIOHandle->pGPIOx->AFR[tmp1] |= pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (tmp2 * 4U);
    }
}

/**
 * @fn          - GPIO_Deinit
 * 
 * @brief       - Resets the registers of a GPIO port using the RCC reset control
 * 
 * @param[in]   - pGPIOx: Pointer to the GPIO peripheral to reset (GPIOA through GPIOE)
 * 
 * @return      - None
 * 
 * @note        - The reset is performed through the corresponding RCC_AHB1RSTR bit.
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
        if(pGPIOx == GPIOA)
        {
            GPIOA_REG_RESET();
        }
        else if(pGPIOx == GPIOB)
        {
            GPIOB_REG_RESET();
        }
        else if(pGPIOx == GPIOC)
        {
            GPIOC_REG_RESET();
        }
        else if(pGPIOx == GPIOD)
        {
            GPIOD_REG_RESET();
        }
        else if(pGPIOx == GPIOE)
        {
            GPIOE_REG_RESET();
        }
}

/**
 * @fn          - GPIO_ReadFromInputPin
 * 
 * @brief       - Reads the logic level of one GPIO input pin
 * 
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: GPIO pin index to read (0-15)
 * 
 * @return      - The selected IDR bit as 0 or 1
 * 
 * @note        - The pin number is used as a bit shift and is not range-checked.
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
    uint8_t value;
    value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);

    return value;
}

/**
 * @fn          - GPIO_ReadFromInputPort
 * 
 * @brief       - Reads the input data register for an entire GPIO port
 * 
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * 
 * @return      - The 16-bit value of the port's input data register (IDR)
 * 
 * @note        - The returned value contains the sampled logic levels of pins 0-15.
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
    uint16_t value;
    value = (uint16_t)pGPIOx->IDR;

    return value;
}

/**
 * @fn          - GPIO_WriteToOutputPin
 * 
 * @brief       - Sets or clears one GPIO output pin
 * 
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: GPIO pin index to update (0-15)
 * @param[in]   - Value: Nonzero to set the output bit; zero to clear it
 * 
 * @return      - None
 * 
 * @note        - This function modifies the corresponding ODR bit using a
 *                read-modify-write operation. The pin number is not range-checked.
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
    if(Value) /* GPIO_PIN_SET */
    {
        pGPIOx->ODR |= (1 << PinNumber);
    }
    else /* GPIO_PIN_RESET */
    {
        pGPIOx->ODR &= ~(1 << PinNumber);
    }
}

/**
 * @fn          - GPIO_WriteToOutputPort
 * 
 * @brief       - Writes a 16-bit value to a GPIO port's output data register
 * 
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - Value: 16-bit value to write to the port's ODR
 * 
 * @return      - None
 * 
 * @note        - This replaces the entire ODR value and therefore updates all port pins
 *                at once.
 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
    pGPIOx->ODR = Value;
}

/**
 * @fn          - GPIO_ToggleOutputPin
 * 
 * @brief       - Inverts the output state of one GPIO pin
 * 
 * @param[in]   - pGPIOx: Base address of the GPIO peripheral
 * @param[in]   - PinNumber: GPIO pin index to toggle (0-15)
 * 
 * @return      - None
 * 
 * @note        - The pin is toggled by XORing its ODR bit; the pin number is not
 *                range-checked.
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
    pGPIOx->ODR ^= (1 << PinNumber);
}

/**
 * @fn          - GIO_IRQConfig
 * 
 * @brief       - Placeholder for configuring or disabling an NVIC interrupt
 * 
 * @param[in]   - IRQNumber: NVIC interrupt number to configure (currently unused)
 * @param[in]   - EnOrDi: Nonzero to enable or zero to disable (currently unused)
 * 
 * @return      - None
 * 
 * @note        - This function is currently empty and does not configure the NVIC.
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            // ISER0 register
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            // ISER1 register
            *NVIC_ISER1 |= (1 << IRQNumber % 32);

        }
        else if(IRQNumber <= 64 && IRQNumber < 96)
        {
            // ISER2 register
            *NVIC_ISER1 |= (1 << IRQNumber % 64);
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            // ICER0 register
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            // ICER1 register
            *NVIC_ICER1 |= (1 << IRQNumber % 32);

        }
        else if(IRQNumber <= 64 && IRQNumber < 96)
        {
            // ICER2 register
            *NVIC_ICER1 |= (1 << IRQNumber % 64);
        }
    }
}

/**
 * @fn          - GPIO_IRQPriorityConfig
 * 
 * @brief
 * 
 * @param[in]   - IRQNumber: NVIC interrupt number to configure (currently unused)
 * @param[in]   - IRQPriority: Priority to assign when enabling (currently unused)
 * 
 * @return      - None
 * 
 * @note        - 
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/**
 * @fn          - GPIO_IRQHandling
 * 
 * @brief       - Placeholder for handling a pending GPIO pin interrupt
 * 
 * @param[in]   - PinNumber: GPIO pin index whose interrupt is to be handled (currently
 *                           unused)
 * 
 * @return      - None
 * 
 * @note        - This function is currently empty; it does not inspect or clear any
 *                pending interrupt flag.
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
    // Cleae the EXTI PR register corresponding to the pin number   
    if(EXTI->PR & (1 << PinNumber))
    {
        EXTI->PR |= (1 << PinNumber);
    }
}
