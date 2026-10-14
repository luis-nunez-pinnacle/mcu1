/*
 * stm32f401xx_spi_driver.c
 *
 *  Created on: Oct 13, 2026
 *      Author: lenunez
 */


#include "stm32f401xx_spi_driver.h"

/**************************************************************************************************
 *                                      Local Prototypes
 **************************************************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
    if(pSPIx->SR & FlagName)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}


/**************************************************************************************************
*                                      Function Prototypes
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
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t pRxBuffer, uint32_t Len)
{
    
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

}

/**
 * @fn          - SPI_IRQHandling
 * 
 * @brief       -
 * 
 * @param[in]   - PinNumber
 * 
 * @return      - None
 * 
 * @note        - None
 */
void SPI_IRQHandling(uint8_t PinNumber)
{

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

