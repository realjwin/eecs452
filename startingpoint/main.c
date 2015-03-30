#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

int main(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef GPIO_InitDef;

	GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	//Initialize pins
	GPIO_Init(GPIOD, &GPIO_InitDef);

	volatile int i;
    while (1) {
    	// Toggle leds
    	GPIO_ToggleBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_15);
    	// Waste some time
    	for (i = 0; i < 500000; i++);
    }
}
