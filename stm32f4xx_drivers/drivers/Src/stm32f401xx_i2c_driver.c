/*
 * stm32f401xx_i2c_driver.c
 *
 *  Created on: Oct 15, 2026
 *      Author: lenunez
 */


#include "stm32f401xx_i2c_driver.h"

/**************************************************************************************************
 *                                      RCC Helper Code
 **************************************************************************************************/
uint16_t AHB_PreScaler[8] = {2,4,8,16,64,128,256,512};
uint16_t APB1_PreScaler[4] = {2,4,8,16};


static uint32_t RCC_GetPCLK1Value(void);
static uint32_t RCC_GetPLLOutputClock(void);

static uint32_t RCC_GetPCLK1Value(void)
{
    uint32_t pclk1, SystemClk;
    uint8_t clksrc, tmp, ahbp, apb1;

    clksrc = (RCC->CFGR >> 2) * 0x3;

    if(clksrc == 0)
    {
        SystemClk = 16000000;
    }
    else if (clksrc == 1)
    {
        SystemClk = 8000000;
    }
    else if( clksrc == 2)
    {
        SystemClk = RCC_GetPLLOutputClock();
    }

    // AHB
    tmp = ((RCC->CFGR >> 4) & 0xF);

    if(tmp < 8)
    {
        ahbp = 1;
    }
    else
    {
        ahbp = AHB_PreScaler[tmp-8];
    }

    // APB1
    tmp = ((RCC->CFGR >> 10) & 0x7);

    if(tmp < 4)
    {
        apb1 = 1;
    }
    else
    {
        apb1 = APB1_PreScaler[tmp-4];
    }

    pclk1 = (SystemClk / ahbp) / apb1;

    return pclk1;
}

static uint32_t RCC_GetPLLOutputClock(void)
{
    // Placeholder
    return 0;
}

/**************************************************************************************************
 *                                      Helper Functions
 **************************************************************************************************/
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2C);
static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2C);
static uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx);


/**************************************************************************************************
*                                      Function Implementation
**************************************************************************************************/ 

/**
 * @fn          - I2C_ClockControl
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_ClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
    if(EnOrDi)
    {
        if(pI2Cx == I2C1)
        {
            I2C1_PCLK_EN();
        }
        else if(pI2Cx == I2C2)
        {
            I2C2_PCLK_EN();
        }
        else if(pI2Cx == I2C3)
        {
            I2C3_PCLK_EN();
        }
    }   
    else
    {
#if TODO
        if(pI2Cx == I2C1)
        {
            I2C1_PCLK_DI();
        }
        else if(pI2Cx == I2C2)
        {
            I2C2_PCLK_DI();
        }
        else if(pI2Cx == I2C3)
        {
            I2C3_PCLK_DI();
        }
#endif
    }
}


/**
 * @fn          - I2C_Init
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
    uint32_t tmpreg = 0;

    // Enable the clock for the I2Cx peripheral
    I2C_ClockControl(pI2CHandle->pI2Cx, ENABLE);

    // ACK control bit
    tmpreg |= (pI2CHandle->I2CConfig.I2C_ACKControl  << 10);
    pI2CHandle->pI2Cx->CR1 = tmpreg;

    // Configure the FREQ field of CR2
    tmpreg = 0;
    tmpreg |= RCC_GetPCLK1Value() / 1000000U;
    pI2CHandle->pI2Cx->CR2 = (tmpreg & 0x3F);

    // program the device own address
    tmpreg = 0;
    tmpreg |= pI2CHandle->I2CConfig.I2C_DeviceAddress << 1;
    tmpreg |= (1 << 14); // as per the reference manual
    pI2CHandle->pI2Cx->OAR1 = tmpreg;

    // CCR calculations 
    uint16_t ccr_value = 0;
    tmpreg = 0;
    if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        // Standard mode
        ccr_value = (RCC_GetPCLK1Value() / (2 * pI2CHandle->I2CConfig.I2C_SCLSpeed));
        tmpreg |= (ccr_value & 0xFFF);
    }
    else
    {
        // Fast mode
        tmpreg |= (1 << 15);
        tmpreg |= (pI2CHandle->I2CConfig.I2C_FMDutyCycle << 14);
        if(pI2CHandle->I2CConfig.I2C_FMDutyCycle == I2C_FM_DUTY_2)
        {
            ccr_value = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2CConfig.I2C_SCLSpeed));
        }
        else
        {
            ccr_value = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2CConfig.I2C_SCLSpeed));
        }
        tmpreg |= (ccr_value & 0xFFF);
    }
    pI2CHandle->pI2Cx->CCR = tmpreg;
    
}

/**
 * @fn          - I2C_DeInit
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - Uses RCC_APB1RSTR [21,22,23] for I2C1/2/3
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
        if(pI2Cx == I2C1)
        {
            I2C1_REG_RESET();
        }
        else if(pI2Cx == I2C2)
        {
            I2C2_REG_RESET();
        }
        else if(pI2Cx == I2C3)
        {
            I2C3_REG_RESET();
        }
}

/**
 * @fn          - I2C_MasterSendData
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * @param[in]   - pTxBuffer:
 * @param[in]   - Len:
 * @param[in]   - SlaveAddr:
 * 
 * @return      - None
 * 
 * @note        - Uses RCC_APB1RSTR [21,22,23] for I2C1/2/3
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr)
{
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    // wait until SB is cleared SCL will be stretch
    while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

    // send address of the slave 
    I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr);

    // confirm address phase is completed by checking the ADDR flag in SR1
    while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

    // clear ADDR flag. Until ADDR is clear, SCL will be stretched (pulled to LOW)
    I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

    // Send data until Len becomes 0
    while (Len > 0)
    {
        while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
        pI2CHandle->pI2Cx->DR = *pTxBuffer;
        pTxBuffer++;
        Len--;       
    }

    // When Len becomes 0, wait for TXE=1 and BTF=1 before generating stop condition.
    while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
    while (! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

    // Generate Stop condition
    I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

}

/**
 * @fn          - I2C_IRQInterruptConfig
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnOrDi)
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
 * @fn          - I2C_IRQPriorityConfig
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/**
 * @fn          - I2C_PeripheralControl
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_PeripheralControl(I2C_RegDef_t  *pI2Cx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_PE);
    }
    else
    {
        pI2Cx->CR1 &= ~(1 << 0);
    }
}

/**
 * @fn          - I2C_ApplicationEventCallback
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{

}


/**************************************************************************************************
 *                             Helper Function Implementation
 **************************************************************************************************/

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2C)
{
    pI2C->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2C)
{
    pI2C->CR1 |= (1 << I2C_CR1_STOP);
}


static uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
    if(pI2Cx->SR1 & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
    SlaveAddr = SlaveAddr << 1; // Make space for r/w bit
    SlaveAddr &= ~(1);          // SlaveAddr is slave address + r/w bit
    pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx)
{
    uint32_t dummyRead = pI2Cx->SR1;
    dummyRead = pI2Cx->SR2;

    (void)dummyRead;
}
