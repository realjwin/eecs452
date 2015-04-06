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
#include "stm32f4xx_dac.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#include <math.h>
#include <stdint.h>

void ADC1_Config(uint32_t sample_rate);

void ADC1_GPIO_Config(void);

void ADC1_Timer_Config(uint32_t sample_rate);

void ADC1_Interrupt_Config(void);

void DAC1_Config_All();

/*END CODE*/
#ifdef __cplusplus
}
#endif

#endif /*__ADC_SETUP_H*/
