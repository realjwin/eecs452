/*
Author: Jacob Winick
Date: 3/22/2015
About:
*/

#ifndef __DAC_SETUP_H
#define __DAC_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif
/*BEGIN CODE*/

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"

/*Configures clocks for GPIOA and DAC1 (PA4)*/
void RCC_Config_DAC1();

/*Configures GPIOA PA4 for DAC1*/
void GPIO_Config_DAC1();

/*Configures DAC1 without DMA*/
void DAC1_Config();

/*END CODE*/
#ifdef __cplusplus
}
#endif

#endif /*__ADC_SETUP_H*/
