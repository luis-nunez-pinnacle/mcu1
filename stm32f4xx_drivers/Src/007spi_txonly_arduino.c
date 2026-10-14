/*
 * 007spi_txonly_arduino.c
 *
 *  Created on: Oct 14, 2026
 *      Author: lenunez
 */

 /**------------------------ Project Details-----------------------------
  *
  * GPIO alternate function - SPI2 pin mapping
  *
  * PB12 = SPI2_NSS
  * PB13 = SPI2_SCLK
  * PB14 = SPI2_MISO
  * PB15 = SPI2_MOSI
  * ALT Function mode: 5
  *
  * 
  * SPI Slave demo
  * 
  * SPI pin numbers
  * SS   10  - Arduino SPI pins respond only if SS is pulled LOW by the master
  * MOSI 11
  * MISO 12
  * SCLK 13
  * 
  *----------------------------------------------------------------------*/

#include <string.h>
#include "stm32f401xx.h"

void delay(void)
{
	for(uint32_t i = 0; i < 50000; i++);
}

void SPI2_GPIOInit(void)
{
  GPIO_Handle_t SPIPins;

  SPIPins.pGPIOx = GPIOB;
  SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
  SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
  SPIPins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
  SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
  SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

  // SCLK
  SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_13;
  GPIO_Init(&SPIPins);

  // MOSI
  SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_15;
  GPIO_Init(&SPIPins);

#if 0 /* Not required in this application */
	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_14;
	GPIO_Init(&SPIPins);
#endif

  // NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_12;
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
  SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

  SPI_Init(&SPI2Handle);
}

void GPIO_ButtonInit(void)
{
  GPIO_Handle_t GPIOButton;

  GPIOButton.pGPIOx = GPIOA;
  GPIOButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_0;
  GPIOButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
  GPIOButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
  GPIOButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;

  GPIO_Init(&GPIOButton);
}

int main(void)
{
  char user_data[] = "Hello World";

  GPIO_ButtonInit();

  SPI2_GPIOInit(); /* Used to initialize the GPIO pins to behave as SPI2 pins*/

  SPI2_Init();    /* Used to initialize the SPI2 peripheral */

  /**  
   * Making SSOE 1 does NSS output enable
   * The NSS pin is automatically managed by the HW. 
   * i.e. when SPE=1, NSS will be pulled to LOW and NSS pin will be HIGH when SPE=0
   */
  SPI_SSOEConfig(SPI2, ENABLE);

  while(1)
  {
    uint8_t dataLen = strlen(user_data);
    
    while( !GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NUM_0) ); // wait till button is pressed

    delay(); // Delay to aboid button debouncing issues - aprox 200ms delay

    SPI_PeripheralControl(SPI2, ENABLE); // Enable the SPI2 peripheral

    SPI_SendData(SPI2, &dataLen, 1); // send length information

    SPI_SendData(SPI2, (uint8_t *)user_data, strlen(user_data)); // send data

    while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG)); // wait till SPI is not busy

    SPI_PeripheralControl(SPI2, DISABLE); // Disable the SPI2 peripheral

  }

  return 0;

}