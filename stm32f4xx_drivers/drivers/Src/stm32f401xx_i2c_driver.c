/*
 * stm32f401xx_i2c_driver.c
 *
 *  Created on: Oct 15, 2026
 *      Author: lenunez
 */


#include "stm32f401xx_i2c_driver.h"

/**************************************************************************************************
 *                                      Helper Functions
 **************************************************************************************************/
static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2C);
static uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);
static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);



/**************************************************************************************************
*                                      Function Implementation
**************************************************************************************************/ 

/**
 * @fn          - I2C_ClockControl
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * @param[in]   - EnOrDi
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
 * @param[in]   - pI2CHandle:
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

    // TRISE Configuration
    if(pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        tmpreg = (RCC_GetPCLK1Value() / 10000000U + 1u);
    }
    else /* Fast Mode*/
    {
        tmpreg = ((RCC_GetPCLK1Value() * 300) / 1000000000U) + 1u; 
    }

    pI2CHandle->pI2Cx->TRISE = (tmpreg & 0x3F);
    
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
 * @param[in]   - Sr
 * 
 * @return      - None
 * 
 * @note        - 
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    // wait until SB is cleared SCL will be stretch
    while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

    // send address of the slave 
    I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

    // confirm address phase is completed by checking the ADDR flag in SR1
    while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

    // clear ADDR flag. Until ADDR is clear, SCL will be stretched (pulled to LOW)
    I2C_ClearADDRFlag(pI2CHandle);

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
    if(Sr == I2C_DISABLE_SR)
    {
        I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
    }
}

/**
 * @fn          - I2C_MasterReceiveData
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * @param[in]   - pTxBuffer:
 * @param[in]   - Len:
 * @param[in]   - SlaveAddr:
 * @param[in]   - Sr
 * 
 * @return      - None
 * 
 * @note        -
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    // Generate Start Condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    // Check SB flag in SR1 to confirm start generation is completed
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

    // Send the address of the Slave with rw bit
    I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

    // Wait until address phased is completed (by checking the ADDR flag in the SR1)
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

    if(Len == 1)
    {
        // Disable ACK
        I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

        // Clear the ADDR flag
        I2C_ClearADDRFlag(pI2CHandle);

        // Wait Until RXNE becomes 1
        while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

        if(Sr == I2C_DISABLE_SR)
        {
            I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
        }

        // Read data in to the buffer
        *pRxBuffer = pI2CHandle->pI2Cx->DR;
    }

    if(Len > 2)
    {
        // Clear the ADDR flag
        I2C_ClearADDRFlag(pI2CHandle);

        // read the data until Len becomes 0
        for(uint32_t i = Len; i > 0; i--)
        {
            // Wait until RXNE becomes 1
            while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

            if(i == 2) // 2 bytes remaining?
            {
                // Clear the ACK bit
                I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
                
                // Generate Stop Condition
                if(Sr == I2C_DISABLE_SR)
                {
                    I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
                }
            }

            // Read the data from data register in to buffer
            *pRxBuffer = pI2CHandle->pI2Cx->DR;

            // Increment the buffer address
            pRxBuffer++;
        }
    }

    // Re-enable ACKing
    if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
    {
        I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);   
    }
}

/**
 * @fn          - I2C_SlaveSendDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx
 * 
 * @return      - None
 * 
 * @note        - 
 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data)
{
    pI2Cx->DR = data;
}

/**
 * @fn          - I2C_SlaveReceiveDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx
 * 
 * @return      - None
 * 
 * @note        - 
 */
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
    return (uint8_t)pI2Cx->DR;
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
 * @fn          - I2C_MasterSendDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * @param[in]   - pTxBuffer:
 * @param[in]   - Len:
 * @param[in]   - SlaveAddr:
 * @param[in]   - Sr:
 * 
 * @return      - None
 * 
 * @note        - 
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    uint8_t busystate = pI2CHandle->TxRxState;

    if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
    {
        pI2CHandle->pTxBuffer   = pTxBuffer;
        pI2CHandle->TxLen       = Len;
        pI2CHandle->TxRxState   = I2C_BUSY_IN_TX; // pending
        pI2CHandle->DevAddr     = SlaveAddr;
        pI2CHandle->Sr          = Sr;

        // Generate Start Condition
        I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

        // Enable ITBUFEN Control bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

        // Implement code to enable ITEVFEN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVFEN);

        // Enable ITERREN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
    }

    return busystate;
}

/**
 * @fn          - I2C_MasterReceiveDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * @param[in]   - pTxBuffer:
 * @param[in]   - Len:
 * @param[in]   - SlaveAddr:
 * @param[in]   - Sr:
 * 
 * @return      - None
 * 
 * @note        -
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
    uint8_t busystate = pI2CHandle->TxRxState;

    if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
    {
        pI2CHandle->pRxBuffer   = pRxBuffer;
        pI2CHandle->RxLen       = Len;
        pI2CHandle->TxRxState   = I2C_BUSY_IN_RX;
        pI2CHandle->RxSize      = Len;
        pI2CHandle->DevAddr     = SlaveAddr;
        pI2CHandle->Sr          = Sr;

        // Generate Start Condition
        I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

        // Enable ITBUFEN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITBUFEN);

        // Enable ITEVFEN Control Bit 
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITEVFEN);

        // Enable ITERREN Control Bit
        pI2CHandle->pI2Cx->CR2 |= (1 << I2C_CR2_ITERREN);
    }

    return busystate;
}

/**
 * @fn          - I2C_EV_IRQHandling
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    // Interrupt handling for both master and slave mode of a device
    uint32_t tmp1, tmp2, tmp3;

    tmp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVFEN);
    tmp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

    // Handle for interrupt generated by SB event (SB flag is only applicable in master mode)
    tmp3 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_SR1_SB);
    if(tmp1 && tmp3)
    {
        // SB event
        if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
        {
            I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
        }
        else if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
        {
            I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
        }
        else
        {
            // Do nothing
        }

    }

    //Handle for interrupt generated by ADDR event. Master mode - address is sent. Slave mode - address matched with own address
    tmp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
    if(tmp1 && tmp3)
    {
        // ADDR event
        I2C_ClearADDRFlag(pI2CHandle);

    }
    
    // Handle for interrupt generated by BTF(Byte Transfer Finished) event 
    tmp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
    if(tmp1 && tmp3)
    {
        // BTF event
        // SB event
        if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
        {
            if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE))
            {
                // Do nothing
            }
        }
        else if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
        {
            if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE))
            {
                // BTF and TXE = 1
                // BTF and RXNE = 1
                if(pI2CHandle->TxLen == 0)
                {
                    if(pI2CHandle->Sr == I2C_DISABLE_SR)
                    {
                        I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
                    }

                    I2C_CloseSendData(pI2CHandle);

                    I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_COMPLETE);
                }
            }
        }
        else
        {
            // Do nothing
        }


    }

    // Handle for interrupt generated by STOPF event (Stop detection flag is only applicable in slave mode)
    tmp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
    if(tmp1 && tmp3)
    {
        // STOPF event
        pI2CHandle->pI2Cx->CR1 |= 0x0000;

        I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
    }

    // Handle for interrupt generated by TXE event
    tmp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);
    if(tmp1 && tmp2 && tmp3)
    {
        // check for device mode
        if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
        {
            // TXE event
            if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
            {
                I2C_MasterHandleTXEInterrupt(pI2CHandle);
            }
        }
        else /* Slave */
        {
            if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
            {
                I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
            }
        }
    }

    // Handle for interrupt generated by RXNE event
    tmp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);
    if(tmp1 && tmp2 &&tmp3)
    {
        if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
        {
            // RXNE flag is set 
            if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
            {
                I2C_MasterHandleRXNEInterrupt(pI2CHandle);
            }
        }
        else /* Slave */
        {
            if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
            {
                I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
            }
        }
    }
}

/**
 * @fn          - I2C_ER_IRQHandling
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    uint32_t tmp1, tmp2;

    tmp2 = (pI2CHandle->pI2Cx->CR2) & (1 << I2C_CR2_ITERREN);

    // Bus Error
    tmp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_BERR);
    if(tmp1 && tmp2)
    {
        // Clear bus error flag and notify app
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_BERR);
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
    }

    // Arbitration loss error
    tmp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_ARLO);
    if(tmp1 && tmp2)
    {
        // Clear the arbitration loss error flag and notify app
        pI2CHandle->pI2Cx->SR1 &= ~(I2C_SR1_ARLO);
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
    }

    // ACK failure error
    tmp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_AF);
    if(tmp1 && tmp2)
    {
        // Clear the ack failure flag and notify app
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_AF);
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
    }

    // Overrun/underrun error
    tmp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_OVR);
    if(tmp1 && tmp2)
    {
        // Clear the overrun and underrun error flag and notify the app
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_OVR);
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
    }
    
    // Time Out Error
    tmp1 = (pI2CHandle->pI2Cx->SR1) & (1 << I2C_SR1_TIMEOUT);
    if(tmp1 && tmp2)
    {
        // Clear the time out error flag and notify the app
        pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_TIMEOUT);
        I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
    }
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
 * @param[in]   - pI2CHandle:
 * @param[in]   - AppEv
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{

}

/**
 * @fn          - I2C_ApplicationEventCallback
 * 
 * @brief       -
 * 
 * @param[in]   - pI2Cx:
 * @param[in]   - EnOrDi:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_ManageAcking(I2C_RegDef_t  *pI2Cx, uint8_t EnOrDi)
{
    if(EnOrDi == I2C_ACK_ENABLE)
    {
        pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
    }
    else
    {
        pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
    }
}

/**
 * @fn          - I2C_CloseReceiveData
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
    // Disable ITBUFEN Control bit
    pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

    // Disable ITEVFEN Control bit
    pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVFEN);

    pI2CHandle->TxRxState = I2C_READY;
    pI2CHandle->pRxBuffer = NULL;
    pI2CHandle->RxLen = 0;
    pI2CHandle->RxSize = 0;

    if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
    {
        I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
    }
}

/**
 * @fn          - I2C_CloseSendData
 * 
 * @brief       -
 * 
 * @param[in]   - pI2CHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
    // Disable ITBUFEN Control bit
    pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

    // Disable ITEVFEN Control bit
    pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVFEN);

    pI2CHandle->TxRxState = I2C_READY;
    pI2CHandle->pTxBuffer = NULL;
    pI2CHandle->TxLen = 0;
}

/**
 * @fn          - I2C_GenerateStopCondition
 * 
 * @brief       -
 * 
 * @param[in]   - pI2C
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_GenerateStopCondition(I2C_RegDef_t *pI2C)
{
    pI2C->CR1 |= (1 << I2C_CR1_STOP);
}

/**
 * @fn          - I2C_SlaveEnableDisableCallbackEvents
 * 
 * @brief       -
 * 
 * @param[in]   - pI2C
 * 
 * @return      - None
 * 
 * @note        - None
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2C, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pI2C->CR2 |= (1 << I2C_CR2_ITEVFEN);
        pI2C->CR2 |= (1 << I2C_CR2_ITBUFEN);
        pI2C->CR2 |= (1 << I2C_CR2_ITERREN);
    }
    else /* DISABLE */
    {
        pI2C->CR2 &= ~(1 << I2C_CR2_ITEVFEN);
        pI2C->CR2 &= ~(1 << I2C_CR2_ITBUFEN);
        pI2C->CR2 &= ~(1 << I2C_CR2_ITERREN);
    }
}


/**************************************************************************************************
 *                             Helper Function Implementation
 **************************************************************************************************/

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2C)
{
    pI2C->CR1 |= (1 << I2C_CR1_START);
}

static uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
    if(pI2Cx->SR1 & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
    SlaveAddr = SlaveAddr << 1; // Make space for r/w bit
    SlaveAddr &= ~(1);          // SlaveAddr is slave address + r/w bit=0
    pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
    SlaveAddr = SlaveAddr << 1; // Make space for r/w bit
    SlaveAddr |= 1;          // SlaveAddr is slave address + r/w bit=1
    pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
    uint32_t dummy_read;

    if(pI2CHandle->pI2Cx->SR2 << (I2C_SR2_MSL)) /* master mode*/
    {
        if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
        {
            if (pI2CHandle->RxSize == 1)
            {
                I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
            }
            // Clear ADDR flag
            dummy_read = pI2CHandle->pI2Cx->SR1;
            dummy_read = pI2CHandle->pI2Cx->SR2;
            (void)dummy_read;
        }
        else
        {
            // Clear ADDR flag
            dummy_read = pI2CHandle->pI2Cx->SR1;
            dummy_read = pI2CHandle->pI2Cx->SR2;
            (void)dummy_read;
        }
    }
    else /* Slave mode*/
    {
        dummy_read = pI2CHandle->pI2Cx->SR1;
        dummy_read = pI2CHandle->pI2Cx->SR2;
        (void)dummy_read; 
    }
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
    if(pI2CHandle->RxSize == 1)
    {
        *pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
        pI2CHandle->RxLen--;
    }
    
    if(pI2CHandle->RxLen > 1)
    {
        if(pI2CHandle->RxLen == 2)
        {
            I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);
        }
        *pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
        pI2CHandle->pRxBuffer++;
        pI2CHandle->RxLen--;
    }

    if(pI2CHandle->RxLen == 0)
    {
        if(pI2CHandle->Sr == I2C_DISABLE_SR)
        {
            // Close reception
            I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
        }
        I2C_CloseReceiveData(pI2CHandle); 

        I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_COMPLETE);
    }
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
    if(pI2CHandle->TxLen > 0)
    {
        // Load the data in to DR
        pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

        // Decrement the TxLen
        pI2CHandle->TxLen--; 

        // Increment the buffer address
        pI2CHandle->pTxBuffer++;
    }
}


