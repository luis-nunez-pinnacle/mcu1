/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: Oct 19, 2026
 *      Author: lenunez
 */


/**
 * PB6-> SCL
 * PB9-> SDA
 * 
 */


#include <stdio.h>
#include <string.h>
#include "stm32f401xx.h"

#define APP_MY_ADDR     0x61
#define SLAVE_ADDR      0x68 // retrieved from Arduino

I2C_Handle_t I2C1Handle;
uint8_t receive_buffer[32];

void delay(void)
{
    for(uint32_t i = 0; i < 500000/2; i++);
}

void I2C1_GPIOInit(void)
{
    GPIO_Handle_t I2CPins;

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;

    // SCL
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_6;
    GPIO_Init(&I2CPins);

    // SDA 
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_7;
    GPIO_Init(&I2CPins);

}

void I2C1_Init(void)
{
    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2CConfig.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2CConfig.I2C_DeviceAddress = APP_MY_ADDR; // dummy address
    I2C1Handle.I2CConfig.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2CConfig.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

    I2C_Init(&I2C1Handle);
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
    uint8_t commandcode, len;

    GPIO_ButtonInit();

    I2C1_GPIOInit();
    
    I2C1_Init();
    
    I2C_PeripheralControl(I2C1, ENABLE);

    I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

    while(1)
    {
        while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NUM_0));

        delay();

        commandcode = 0x51;
        I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
        I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

        commandcode = 0x52;
        I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR, I2C_ENABLE_SR);
        I2C_MasterReceiveData(&I2C1Handle, receive_buffer, len, SLAVE_ADDR, I2C_DISABLE_SR);
    }
}