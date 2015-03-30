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
#include "stm32f4xx_tim.h"
#include "misc.h"

#include <math.h>
#include <stdint.h>

/*Configures DAC1 without DMA*/
void DAC1_Config(uint32_t sample_rate);

void DAC1_GPIO_Config(void);

void DAC1_Timer_Config(uint32_t sample_rate);

void DAC1_Interrupt_Config(void);

void DAC1_DDS_Config(uint32_t sample_rate, uint32_t target_frequency);

/*Starts DAC1 at upper frequency*/
//void DAC1_Start();
/**/
//void DAC1_SendData();
/**/
//void DAC1_IsDone();
//void Handler();

/*END CODE*/
#ifdef __cplusplus
}
#endif

#endif /*__DAC_SETUP_H*/
