/*
 * stm32f401xx_usart_driver.c
 *
 *  Created on: Oct 21, 2026
 *      Author: lenunez
 */
#include "stm32f401xx_usart_driver.h"


/**************************************************************************************************
 *                                      Helper Function Prototypes
 **************************************************************************************************/




/**************************************************************************************************
 *                                      Function Implementation
 **************************************************************************************************/

/**
 * @fn          - USART_ClockControl
 * 
 * @brief       - Enables or disables the peripheral clock for a USART port
 * 
 * @param[in]   - pUSARTx: Pointer to the USART peripheral to control
 * @param[in]   - EnOrDi: Nonzero to enable the clock; zero to disable it
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_ClockControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
if(EnOrDi)
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_EN();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_EN();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_EN();
        }
    }   
    else
    {
        if(pUSARTx == USART1)
        {
            USART1_PCLK_DI();
        }
        else if(pUSARTx == USART2)
        {
            USART2_PCLK_DI();
        }
        else if(pUSARTx == USART6)
        {
            USART6_PCLK_DI();
        }
    }
}

/**
 * @fn          - USART_Init
 * 
 * @brief       - 
 * 
 * @param[in]   - USARTHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_Init(USART_Handle_t *pUSARTHandle)
{
    uint32_t tmpreg = 0;

    /**------------------------------------------------------------------------
     * CR1 configuration 
     *-------------------------------------------------------------------------*/

    //Enable the clock for given USART peripheral
    USART_ClockControl(pUSARTHandle->pUSARTx, ENABLE);

    // Enable USART Tx and Rx engines according to USART_Mode
    if(pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_ONLY_RX)
    {
        tmpreg |= (1 << USART_CR1_RE);
    }
    else if(pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_ONLY_RX)
    {
        tmpreg |= (1 << USART_CR1_TE);
    }
    else if(pUSARTHandle->USARTConfig.USART_Mode == USART_MODE_TXRX)
    {
        tmpreg |= (1 << USART_CR1_RE) | (1 << USART_CR1_TE);
    }

    //Configure Word Length 
    tmpreg |= (pUSARTHandle->USARTConfig.USART_WordLength << USART_CR1_M);

    // Configure parity control
    if(pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_EN_EVEN)
    {
        tmpreg |= (1 << USART_CR1_PCE);
        //Even parity is default
    }
    else if(pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_EN_ODD)
    {
        tmpreg |= (1 << USART_CR1_PCE);
        tmpreg |= (1 << USART_CR1_PS);
    }

    pUSARTHandle->pUSARTx->CR1 = tmpreg;
    
    /**------------------------------------------------------------------------
     * CR2 configuration 
     *-------------------------------------------------------------------------*/
    tmpreg = 0;

    //Configure number of stop bits 
    tmpreg |= pUSARTHandle->USARTConfig.USART_NumOfStopBits << USART_CR2_STOP;

    pUSARTHandle->pUSARTx->CR2 = tmpreg;
    
    /**------------------------------------------------------------------------
     * CR3 configuration 
     *-------------------------------------------------------------------------*/
    tmpreg = 0;

    // Configuration of USART hardware flow control
    if( pUSARTHandle->USARTConfig.USART_HwFlowControl == USART_HW_FLOW_CTRL_CTS)
    {
        tmpreg |= (1 << USART_CR3_CTSE);
    }
    else if(pUSARTHandle->USARTConfig.USART_HwFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
    {
        tmpreg |= (1 << USART_CR3_RTSE);
    }
    else if(pUSARTHandle->USARTConfig.USART_HwFlowControl == USART_HW_FLOW_CTRL_RTS)
    {
        tmpreg |= (1 << USART_CR3_CTSE);
        tmpreg |= (1 << USART_CR3_RTSE);
    }

    pUSARTHandle->pUSARTx->CR3 = tmpreg;


    /**------------------------------------------------------------------------
     * CR3 configuration 
     *-------------------------------------------------------------------------*/

    USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USARTConfig.USART_Baud);
}

/**
 * @fn          - USART_SendData
 * 
 * @brief       - 
 * 
 * @param[in]   - USARTHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint16_t *pdata;

    // Loop over until "Len" number of bytes are transferred
    for(uint32_t i = 0; i < Len; i++)
    {
        // Wait until TXE flag is set in the SR
        while(!USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TXE));

        // Check the USART_WordLength item for 9bit or 8bit in a frame
        if(pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
        {
            // if 9bit load the DR with 2 bytes masking the bites other than first 9 bits
            pdata = (uint16_t *) pTxBuffer;
            pUSARTHandle->pUSARTx->DR = (*pdata & (uint16_t)0x01FF);

            // check for USART_PartityControl
            if(pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
            {
                pTxBuffer++;
                pTxBuffer++;
            }
            else
            {
                pTxBuffer++;
            }
        }
        else /* 8 BITS*/
        {
            pUSARTHandle->pUSARTx->DR = (*pTxBuffer & (uint8_t)0xFF);

            pTxBuffer++;
        }
    }

    // Wait until TC flag is set in the SR
    while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx, USART_FLAG_TC));
}

/**
 * @fn          - USART_ReceiveData
 * 
 * @brief       - 
 * 
 * @param[in]   - USARTHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void  USART_ReceiveData(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Len)
{
    // Loop over until "Len" number of bytes are transferred
    for(uint32_t i = 0; i < Len; i++)
    {
        //Wait until RXNE flag is set in the SR
        while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx, I2C_FLAG_RXNE));

        // Check the USART_WordLength
        if(pUSARTHandle->USARTConfig.USART_WordLength == USART_WORDLEN_9BITS)
        {
            // Parity control check
            if(pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
            {
                // read only first 9 bits so mask the DR with 0x1FF
                *((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint16_t)0x01FF);      
                pRxBuffer++;
                pRxBuffer++;      
            } 
            else /* PARITY ENABLE*/
            {
                *pRxBuffer = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
                pRxBuffer++;
            }
        }
        else /* 8 BITS*/
        {
            // Parity control check
            if(pUSARTHandle->USARTConfig.USART_ParityControl == USART_PARITY_DISABLE)
            {
                // read only first 8 bits so mask the DR with 0x1FF
                *((uint16_t*)pRxBuffer) = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);      
            } 
            else /* PARITY ENABLE*/
            {
                *pRxBuffer = (pUSARTHandle->pUSARTx->DR & (uint8_t)0xFF);
            } 
            pRxBuffer++;
        }
    }
}

/**
 * @fn          - USART_SendDataIT
 * 
 * @brief       - 
 * 
 * @param[in]   - USARTHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
uint8_t USART_SendDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	return 0;
}

/**
 * @fn          - USART_ReceiveDataIT
 * 
 * @brief       - 
 * 
 * @param[in]   - USARTHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
uint8_t USART_ReceiveDataIT(USART_Handle_t *pUSARTHandle,uint8_t *pRxBuffer, uint32_t Len)
{
	return 0;
}

/**
 * @fn          - USART_IRQInterruptConfig
 * 
 * @brief       - 
 * 
 * @param[in]   - IRQNumber
 * @param[in]   - EnOrDi:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnOrDi)
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
 * @fn          - USART_IRQPriorityConfig
 * 
 * @brief       - 
 * 
 * @param[in]   - IRQNumber
 * @param[in]   - IRQPriority
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/**
 * @fn          - USART_PeripheralControl
 * 
 * @brief       - 
 * 
 * @param[in]   - pUSARTx: Pointer to the USART peripheral to control
 * @param[in]   - EnOrDi: 
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_PeripheralControl(USART_RegDef_t  *pUSARTx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pUSARTx->CR1 |= (1 << USART_CR1_UE);
    }
    else
    {
        pUSARTx->CR1 &= ~(1 << USART_CR1_UE);
    }
}

/**
 * @fn          - USART_GetFlagStatus
 * 
 * @brief       - 
 * 
 * @param[in]   - pUSARTx: Pointer to the USART peripheral to control
 * @param[in]   - EnOrDi:
 * 
 * @return      - None
 * 
 * @note        - None
 */
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t FlagName)
{
    if(pUSARTx->SR & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

/**
 * @fn          - USART_ClearFlag
 * 
 * @brief       - 
 * 
 * @param[in]   - pUSARTx: 
 * @param[in]   - EnOrDi: 
 * 
 * @return      - None
 * 
 * @note        - None
 */
void USART_ClearFlag(USART_RegDef_t *pUSARTx, uint16_t FlagName)
{

}

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{
    uint32_t PCLKx;
    uint32_t usartdiv;
    uint32_t mantissa, fraction;

    uint32_t tmpreg = 0;

    if(pUSARTx == USART1 || pUSARTx == USART6)
    {
        PCLKx = RCC_GetPCLK2Value();
    }
    else
    {
        PCLKx = RCC_GetPCLK1Value();
    }

    // Check for OVER8 configuration bit
    if(pUSARTx->CR1 & (1 << USART_CR1_OVER8))
    {
        usartdiv = ((25 * PCLKx) / (2 * BaudRate));
    }
    else /* OVER8 = 0 -- oversampling by 16*/
    {
        usartdiv = ((25 * PCLKx) / (4 * BaudRate));
    }

    // Calculate the Mantissa part
    mantissa = usartdiv/100;

    // Place the mantissa part in the appropiate bit position. Refer to USART_BRR
    tmpreg |= (mantissa << USART_BRR_DIVMANT);

    // Extract the fraction part
    fraction = (usartdiv - (mantissa*100));

    // Calculate the final fractional
    if(USART1->CR1 & (1 << USART_CR1_OVER8))
    {
        fraction = ((( fraction * 8) +50) /100) & ((uint8_t)0x07);
    }
    else
    {
        fraction = (((fraction * 16)+50)/100) & ((uint8_t)0x0F);
    }

    tmpreg |= fraction;

    pUSARTx->BRR = tmpreg;
}
