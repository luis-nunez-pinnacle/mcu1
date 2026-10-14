/*
 * stm32f401xx_spi_driver.c
 *
 *  Created on: Oct 13, 2026
 *      Author: lenunez
 */


#include "stm32f401xx_spi_driver.h"


/**************************************************************************************************
 *                                      Helper Function Prototypes
 **************************************************************************************************/

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle);

/**************************************************************************************************
*                                      Function Implementation
**************************************************************************************************/
/**
 * @fn          - SPI_ClockControl
 * 
 * @brief       - Enables or disables the peripheral clock for a SPI port
 * 
 * @param[in]   - pSPIx: Pointer to the SPI peripheral to control (SPI1 through SPI4)
 * @param[in]   - EnOrDi: Nonzero to enable the clock; zero to disable it
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_ClockControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi)
    {
        if(pSPIx == SPI1)
        {
            SPI1_PCLK_EN();
        }
        else if(pSPIx == SPI2)
        {
            SPI2_PCLK_EN();
        }
        else if(pSPIx == SPI3)
        {
            SPI3_PCLK_EN();
        }
        else if(pSPIx == SPI4)
        {
            SPI4_PCLK_EN();
        }
    }   
    else
    {
        if(pSPIx == SPI1)
        {
            SPI1_PCLK_DI();
        }
        else if(pSPIx == SPI2)
        {
            SPI2_PCLK_DI();
        }
        else if(pSPIx == SPI3)
        {
            SPI3_PCLK_DI();
        }
        else if(pSPIx == SPI4)
        {
            SPI4_PCLK_DI();
        }
    }
}

/**
 * @fn          - SPI_Init
 * 
 * @brief       -
 * 
 * @param[in]   - pGPIOHandle: Pointer to a handle containing the SPI configuration
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
    uint32_t tmpreg = 0;

    SPI_ClockControl(pSPIHandle->pSPIx, ENABLE);

    tmpreg |= pSPIHandle->SPIConfig.SPI_DeviceMode  << SPI_CR1_MSTR;

    if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
    {
        tmpreg &= ~(1 << SPI_CR1_BIDI_MODE);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
    {
        tmpreg |= (1 << SPI_CR1_BIDI_MODE);
    }
    else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RX)
    {
        tmpreg &= ~(1 << SPI_CR1_BIDI_MODE);
        tmpreg |= (1 << SPI_CR1_RX_ONLY);
    }

    tmpreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

    tmpreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

    tmpreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

    tmpreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

    tmpreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

    pSPIHandle->pSPIx->CR1 = tmpreg;
}

/**
 * @fn          - SPI_DeInit
 * 
 * @brief       - Resets the registers of SPI using the RCC reset control
 * 
 * @param[in]   - pSPIx
 * 
 * @return      - None
 * 
 * @note        - Uses RCC_APB2RSTR [12,13] for SPI1/4 Uses RCC_APB1RSTR [14,15] for SPI2/3
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
        if(pSPIx == SPI1)
        {
            SPI1_REG_RESET();
        }
        else if(pSPIx == SPI2)
        {
            SPI2_REG_RESET();
        }
        else if(pSPIx == SPI3)
        {
            SPI3_REG_RESET();
        }
        else if(pSPIx == SPI4)
        {
            SPI4_REG_RESET();
        }
}


/**
 * @fn          - SPI_SendData
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx
 * @param[in]   - pTxBuffer
 * @param[in]   - Len
 * 
 * @return      - None
 * 
 * @note        - This is a blocking call
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
    while(Len > 0)
    {
        while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

        if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
        {
            //16-bit DFF
            pSPIx->DR = *((uint16_t *)pTxBuffer);
            Len--;
            Len--;
            (uint16_t*)pTxBuffer++;
        }
        else
        {
            //8-bit DFF
            pSPIx->DR = *pTxBuffer;
            Len--;
            pTxBuffer++;
        }
    }
}

/**
 * @fn          - SPI_ReceiveData
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx
 * @param[in]   - pRxBuffer
 * @param[in]   - Len
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
    while(Len > 0)
    {
        while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

        if((pSPIx->CR1 & ( 1 << SPI_CR1_DFF)))
        {
            //16-bit DFF
            *((uint16_t *)pRxBuffer) = pSPIx->DR;
            Len--;
            Len--;
            (uint16_t*)pRxBuffer++;
        }
        else
        {
            //8-bit DFF
            *(pRxBuffer) = pSPIx->DR;
            Len--;
            pRxBuffer++;
        }
    }
}


/**
 * @fn          - SPI_IRQInterruptConfig
 * 
 * @brief       -
 * 
 * @param[in]   - IRQNumber
 * @param[in]   - EnOrDi
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber,uint8_t EnOrDi)
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
 * @fn          - SPI_IRQPriorityConfig
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
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);

}

/**
 * @fn          - SPI_IRQHandling
 * 
 * @brief       -
 * 
 * @param[in]   - pHandle
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
    uint8_t tmp1, tmp2;
    
    // Check for TXE (and TXEIE)
    tmp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_TXE);
    tmp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_TXEIE);

    if(tmp1 && tmp2)
    {
        // handle TXE
        spi_txe_interrupt_handle(pSPIHandle);
    }
    
    // Check for RXE (and RXNEIE)
    tmp1 = pSPIHandle->pSPIx->SR & ( 1 << SPI_SR_RXNE);
    tmp2 = pSPIHandle->pSPIx->CR2 & ( 1 << SPI_CR2_RXNEIE);

    if(tmp1 && tmp2)
    {
        // handle RXNE
        spi_rxne_interrupt_handle(pSPIHandle);
    }

    // Check for ovr flag (and ERRIE)
    tmp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
    tmp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

    if(tmp1 && tmp2)
    {
        spi_ovr_err_interrupt_handle(pSPIHandle);
    }


}

/**
 * @fn          - SPI_SendDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIHandle
 * @param[in]   - pRxBuffer
 * @param[in]   - Len
 * 
 * @return      - None
 * 
 * @note        - None
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
    uint8_t state = pSPIHandle->TxState;

    if(state != SPI_BUSY_IN_TX)
    {
        // Save the Tx buffer address and Len in global variables
        pSPIHandle->pTxBuffer = pTxBuffer;
        pSPIHandle->TxLen = Len;

        // Mark the SPI state as busy in transmission; then, no one can take over the same SPI peripheral
        pSPIHandle->TxState = SPI_BUSY_IN_TX;

        // Enable TXEIE control bit to get interrupt whenever TXE flag is set in SR
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
    }
    

    return state;
}

/**
 * @fn          - SPI_ReceiveDataIT
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIHandle
 * @param[in]   - pRxBuffer
 * @param[in]   - Len
 * 
 * @return      - None
 * 
 * @note        - None
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
    uint8_t state = pSPIHandle->RxState;

    if(state != SPI_BUSY_IN_RX)
    {
        // Save the Rx buffer address and Len in global variables
        pSPIHandle->pRxBuffer = pRxBuffer;
        pSPIHandle->RxLen = Len;

        // Mark the SPI state as busy in transmission; then, no one can take over the same SPI peripheral
        pSPIHandle->RxState = SPI_BUSY_IN_RX;

        // Enable TXEIE control bit to get interrupt whenever TXE flag is set in SR
        pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
    }

    return state;
}

/**
 * @fn          - SPI_PeripheralControl
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_PeripheralControl(SPI_RegDef_t  *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pSPIx->CR1 |= (1 << SPI_CR1_SPE);
    }
    else
    {
        pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
    }
}

/**
 * @fn          - SPI_SSIConfig
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * @param[in]   - EnOrDi:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_SSIConfig(SPI_RegDef_t  *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pSPIx->CR1 |= (1 << SPI_CR1_SSI);
    }
    else
    {
        pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
    }
}

/**
 * @fn          - SPI_SSOEConfig
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * @param[in]   - EnOrDi:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_SSOEConfig(SPI_RegDef_t  *pSPIx, uint8_t EnOrDi)
{
    if(EnOrDi == ENABLE)
    {
        pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
    }
    else
    {
        pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
    }
}

uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
    if(pSPIx->SR & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

/**
 * @fn          - SPI_ClearOVRFLag
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_ClearOVRFLag(SPI_RegDef_t *pSPIx)
{
    uint8_t tmp;
    tmp = pSPIx->DR;
    tmp = pSPIx->SR;
    (void)tmp;
}

/**
 * @fn          - SPI_CloseTransmission
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{

    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
    pSPIHandle->pTxBuffer = NULL;
    pSPIHandle->TxLen = 0;
    pSPIHandle->TxState = SPI_READY;
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_COMPLETE);
}

/**
 * @fn          - SPI_CloseReception
 * 
 * @brief       -
 * 
 * @param[in]   - pSPIx:
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
    pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
    pSPIHandle->pRxBuffer = NULL;
    pSPIHandle->RxLen = 0;
    pSPIHandle->RxState = SPI_READY;
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_COMPLETE);
}

__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
    // This is a weak implementation. The application may override this function.
}

/**************************************************************************************************
 *                                      Helper Function Implementation
 **************************************************************************************************/

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
    {
        //16-bit DFF
        pSPIHandle->pSPIx->DR = *((uint16_t *)pSPIHandle->pTxBuffer);
        pSPIHandle->TxLen--;
        pSPIHandle->TxLen--;
        (uint16_t *)pSPIHandle->pTxBuffer++;

    }
    else
    {
        //8-bit DFF
        pSPIHandle->pSPIx->DR = *pSPIHandle->pTxBuffer;
        pSPIHandle->TxLen--;
        pSPIHandle->pTxBuffer++;
    }

    if(pSPIHandle->TxLen == 0)
    {
        SPI_CloseTransmission(pSPIHandle);
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_COMPLETE);
    }
}

static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    if((pSPIHandle->pSPIx->CR1 & ( 1 << SPI_CR1_DFF)))
    {
        //16-bit DFF
        *((uint16_t *)pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen--;
        pSPIHandle->RxLen--;
        (uint16_t*)pSPIHandle->pRxBuffer--;
        (uint16_t*)pSPIHandle->pRxBuffer--;
    }
    else
    {
        //8-bit DFF
        *(pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;
        pSPIHandle->RxLen--;
        pSPIHandle->pRxBuffer--;
    }

    if(pSPIHandle->RxLen == 0)
    {
        SPI_CloseReception(pSPIHandle);
        SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_COMPLETE);
    }
}

static void spi_ovr_err_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
    uint8_t tmp;

    if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
    {
        tmp = pSPIHandle->pSPIx->DR;
        tmp = pSPIHandle->pSPIx->SR; 
    }
    (void)tmp;
    SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);

}
