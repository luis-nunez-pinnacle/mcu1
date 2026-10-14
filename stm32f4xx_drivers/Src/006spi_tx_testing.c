/*
 * 006spi_tx_testing.c
 *
 *  Created on: Oct 13, 2026
 *      Author: lenunez
 */

 /**
  * GPIO alternate function - SPI2 pin mapping
  * 
  * PB12 = SPI2_NSS
  * PB13 = SPI2_SCLK
  * PB14 = SPI2_MISO
  * PB15 = SPI2_MOSI
  * ALT Function mode: 5
  * 
  */

#include "string.h"
#include "stm32f401xx.h"


void SPI2_GPIOInit(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

#if 0 /* Not required in this application */
	// NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_12;
	GPIO_Init(&SPIPins);
#endif

	// SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_13;
	GPIO_Init(&SPIPins);

#if 0 /* Not required in this application */
	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_14;
	GPIO_Init(&SPIPins);
#endif

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_15;
	GPIO_Init(&SPIPins);


}

void SPI2_Init(void)
{
	SPI_Handle_t SPI2Handle;

	SPI2Handle.pSPIx = SPI2;
	SPI2Handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_HIGH;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	SPI_Init(&SPI2Handle);

}

int main(void)
{
	char user_data[] = "Hello World";

	SPI2_GPIOInit();

	SPI2_Init();

	SPI_SSIConfig(SPI2, ENABLE); /* make NSS signal internally HIGH and avoid MODF error */

	SPI_PeripheralControl(SPI2, ENABLE);
	
	SPI_SendData(SPI2, (uint8_t *)user_data, strlen(user_data));

	while(1);
}
