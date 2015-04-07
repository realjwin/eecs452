/*
Author: Jacob Winick
Date: 3/22/2015
About: See adc_setup.h
*/

#include "adc_setup.h"

#define NTAPS 51

/*--Initialization Structures--*/
static GPIO_InitTypeDef 		GPIO_InitStructure; 	//GPIO
static TIM_TimeBaseInitTypeDef	TIM_InitStructure;		//Timer
static NVIC_InitTypeDef			NVIC_InitStructure; 	//Interrupt
static ADC_InitTypeDef 			ADC_InitStructure; 		//ADC
static DAC_InitTypeDef			DAC_InitStructure; 		//DAC

//ADC input
uint16_t value=0;
int16_t coef[NTAPS] =      {1182,  -2472,  -1322,     67,    519,   -344,   -789,    155,    863,
							   -1,   -927,   -188,    944,    388,   -914,   -588,    835,    778,
							 -707,   -940,    538,   1066,   -337,  -1146,    114,   1172,    114,
							-1146,   -337,   1066,    538,   -940,   -707,    778,    835,   -588,
							 -914,    388,    944,   -188,   -927,     -1,    863,    155,   -789,
							 -344,    519,     67,  -1322,  -2472,   1182};
							 
int32_t reg[NTAPS + 1] = {0};

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

	//for DAC1 on PA5
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

	//how does this work??




	//what is ADC_Inhibit_Flag??

	value = ADC_GetConversionValue(ADC1);
	GPIOE->BSRRL = GPIO_Pin_10;
	DAC_Output(FIR(value));
	GPIOE->BSRRH = GPIO_Pin_10;
	

	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);

	//then filtering stuff



}

//this returns a 16-bit Q15 value
int16_t FIR(uint16_t value) {
	uint16_t i;
	int16_t temp;

	//convert value (uint16_t) to 16-bit Q15
	temp = (((int16_t)value)<<4)+0x8000;

	//The actual filter work
	for(i=0; i<NTAPS; i++)
	{
		reg[i] = (int32_t)temp * (int32_t)coef[i] + reg[i + 1];
	}

	reg[0] = reg[0] + 0x00004000;		//  rounding
	return (int16_t)(reg[0] >> 15);  	// Conversion from 32 Q30 to 16 Q15.
}

void DAC_Output(int16_t value) {
	uint16_t temp;

	temp = ((uint16_t)value-0x8000)>>4;
	DAC_SetChannel1Data(DAC_Align_12b_R, temp);
}
