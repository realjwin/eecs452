/*
Author: Jacob Winick
Date: 3/22/2015
About: See adc_setup.h
*/

#include "adc_setup.h"
#include "filters.h"

/*--Initialization Structures--*/
static GPIO_InitTypeDef 		GPIO_InitStructure; 	//GPIO
static TIM_TimeBaseInitTypeDef	TIM_InitStructure;		//Timer
static NVIC_InitTypeDef			NVIC_InitStructure; 	//Interrupt
static ADC_InitTypeDef 			ADC_InitStructure; 		//ADC
static DAC_InitTypeDef			DAC_InitStructure; 		//DAC

//ADC variables
uint16_t value=0;
int32_t reg_bp0[NTAPS_BP0 + 1] = {0};
int32_t reg_bp1[NTAPS_BP1 + 1] = {0};
int32_t reg_lp_1[NTAPS_LP + 1] = {0};
int32_t reg_lp_2[NTAPS_LP + 1] = {0};
int16_t temp1 = 0, temp2 = 0, temp3 = 0; //temp value before going to FIR

/**/
void ADC1_Config(uint32_t sample_rate) {

	ADC_DeInit();

	ADC1_GPIO_Config();

	ADC1_Timer_Config(sample_rate);

	ADC1_Interrupt_Config();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
}

/**/
void ADC1_GPIO_Config(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	//for ADC1 on PC0 using IN10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**/
void ADC1_Timer_Config(uint32_t sample_rate) {
	//NOTE: Kurt also created a 1 MHz clock with TIM4 to measure accuracy
	//Based on: http://visualgdb.com/tutorials/arm/stm32/pwm

	RCC_ClocksTypeDef RCC_Clocks; //to get current APB1 (PCLK1 speed)
	uint32_t timer_period; //to set TIM3 counter

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //enable clock

	//determine TIM3 max counter
	RCC_GetClocksFreq(&RCC_Clocks);
	timer_period = RCC_Clocks.PCLK1_Frequency*2; //look @ clock diag for the x2
	timer_period = timer_period/sample_rate;
	timer_period = timer_period - 1;

	TIM_InitStructure.TIM_Period = timer_period;
	TIM_InitStructure.TIM_Prescaler = 0;
	TIM_InitStructure.TIM_ClockDivision = 0;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_InitStructure);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_Cmd(TIM3, ENABLE);
}

/**/
void ADC1_Interrupt_Config(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel=ADC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void DAC1_Config_All(void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);

	//for DAC1 on PA4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void ADC_IRQHandler(void) {
	GPIOE->BSRRL = GPIO_Pin_10; //set timing pin
	//get ADC value
	value = ADC_GetConversionValue(ADC1);
	//convert value (uint16_t) to 16-bit Q15
	temp1 = (((int16_t)value)<<4)^0x8000;

	temp2 = FIR(temp1, bandpass0, NTAPS_BP0, reg_bp0); //filter for freq0
	temp3 = FIR(temp1, bandpass1, NTAPS_BP1, reg_bp1); //filter for freq1

	//square it
	temp2 = (int16_t)(((int32_t)(temp2*temp2))>>15);
	temp3 = (int16_t)(((int32_t)(temp3*temp3))>>15);

	//second filters
	temp2 = FIR(temp2, lowpass, NTAPS_LP, reg_lp_1);
	temp3 = FIR(temp3, lowpass, NTAPS_LP, reg_lp_2);

	//difference
	temp1 = temp3 - temp2;

	//hard clip
	temp1 = temp1 > 0 ? 0x7FFF : 0x8000;
	DAC_Output(temp1);
	
	//now demodulate

	//clear pending bit
	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);

	GPIOE->BSRRH = GPIO_Pin_10; //release timing pin
}

//this returns a 16-bit Q15 value
int16_t FIR(int16_t value, int16_t coef[], uint16_t numtaps, int32_t reg[]) {
	uint16_t i;

	//The actual filter work
	for(i=0; i<numtaps; i++)
	{
		reg[i] = (int32_t)value * (int32_t)coef[i] + reg[i + 1];
	}

	reg[0] = (reg[0] + 0x00004000) >> 15;		//  rounding and conversion
	reg[0] = reg[0] > 32767 ? 32767:
			reg[0] < -32768 ? -32768 : reg[0]; //overflow

	return (int16_t)reg[0];
}

void DAC_Output(int16_t value) {
	uint16_t temp;

	temp = ((uint16_t)value^0x8000)>>4;

	DAC_SetChannel1Data(DAC_Align_12b_R, temp);
}
