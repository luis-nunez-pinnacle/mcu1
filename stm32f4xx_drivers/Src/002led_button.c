/*
 * 002led_button.c
 *
 *  Created on: Oct 12, 2026
 *      Author: lenunez
 */

#include "stm32f401xx_gpio_driver.h"

#define HIGH           1
#define LOW            0

#define BUTTON_PRESSED LOW     /* PC13 is active-low on this board: it reads high when released and low when pressed. */

#define DELAY_MULTIPLIER            2000

void delay(uint32_t ms)
{
	for(volatile uint32_t i = 0; i < ms*DELAY_MULTIPLIER; i++);
}


int main(void)
{
	GPIO_Handle_t GpioLed, GpioButton;

    // LED configuration
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_ClockControl(GPIOA, ENABLE);
    GPIO_Init(&GpioLed);
    
    // Button configuration
    GpioButton.pGPIOx = GPIOC;
	GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NUM_13;
	GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
    GpioButton.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_NO_PUPD;
	GPIO_ClockControl(GPIOC, ENABLE);
    GPIO_Init(&GpioButton);


	while(1)
	{
        if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NUM_13) == BUTTON_PRESSED)
        {
		    delay(1000);
            GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NUM_5);
        }

	}

	return 0;

}
