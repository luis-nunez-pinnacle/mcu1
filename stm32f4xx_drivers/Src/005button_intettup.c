/*
 * 005button_intettup.c
 *
 *  Created on: Oct 12, 2026
 *      Author: lenunez
 */


#include <string.h>
#include "stm32f401xx_gpio_driver.h"

#define HIGH           1
#define LOW            0

#define USER_BUTTON_PRESSED         LOW     /* PC13 is active-low on this board: it reads high when released and low when pressed. */
#define EXTERNAL_BUTTON_PRESSED     HIGH

#define DELAY_MULTIPLIER            2000

void delay(uint32_t ms)
{
	for(volatile uint32_t i = 0; i < ms*DELAY_MULTIPLIER; i++);
}

int main(void)
{
	GPIO_Handle_t GpioLed, GpioButton;
    memset(&GpioLed, 0, sizeof(GpioLed));
    memset(&GpioButton, 0, sizeof(GpioButton));


    // LED configuration
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_ClockControl(GPIOD, ENABLE);
    GPIO_Init(&GpioLed);
    
    // Button configuration
    GpioButton.pGPIOx = GPIOD;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_5;
	GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	GPIO_ClockControl(GPIOD, ENABLE);
    GPIO_Init(&GpioButton);

    // IRQ Configuration
    GPIO_IRQInterruptConfig(IRQ_NUM_EXTI9_5, ENABLE);
    GPIO_IRQPriorityConfig(IRQ_NUM_EXTI9_5, NVIC_IRQ_PRIORITY_15);

    while(1)
;
}

void EXTI9_5_IRQHandler(void)
{
    delay(200);
    GPIO_IRQHandling(GPIO_PIN_NUM_5);
    GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NUM_12);
}