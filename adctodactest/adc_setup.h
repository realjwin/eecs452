/*
Author: Jacob Winick
Date: 3/22/2015
About:
*/

#ifndef __ADC_SETUP_H
#define __ADC_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif
/*BEGIN CODE*/

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"

/*Configures clocks for GPIOC and ADC1*/
void RCC_Config_ADC1();

/*Configures GPIOC PC0 for ADC1 on IN10*/
void GPIO_Config_ADC1();

/*Configures ADC1*/
void ADC1_Config();

/*END CODE*/
#ifdef __cplusplus
}
#endif

#endif /*__ADC_SETUP_H*/
