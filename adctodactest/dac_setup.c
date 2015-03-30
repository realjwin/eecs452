/*
Author: Jacob Winick
Date: 3/22/2015
About: See adc_setup.h
*/

#include "dac_setup.h"

void RCC_Config_DAC1(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
}

void GPIO_Config_DAC1(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_StructInit(&GPIO_InitStructure);

  //for DAC1 on PA4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC1_Config(void)
{
  DAC_InitTypeDef DAC_InitStructure;

  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  DAC_Cmd(DAC_Channel_1, ENABLE);
}
