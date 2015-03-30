/*
Author: Jacob Winick
Date: 3/22/2015
About: See dac_setup.h

NOTE: Actual DAC rate and given sample rate aren't exact because of divider
Clock works by taking TIM6 (operating nominally at 84 MHz) and counting up so it
is really 84 MHz/max_counter

NOTE: This does not use DMA, but rather you update DAC on a trigger event everytime
a timer counts to a specified value
*/

#include "dac_setup.h"

//NOTE: These should really go in .h file
/*--Defines--*/
#define PI 	3.14159265
#define N 	256

/*--Initialization Structures--*/
static GPIO_InitTypeDef 		GPIO_InitStructure; //GPIO
static TIM_TimeBaseInitTypeDef	TIM_InitStructure;	//Timer
static NVIC_InitTypeDef			NVIC_InitStructure; //Interrupt
static DAC_InitTypeDef 			DAC_InitStructure; 	//DAC

/*--DDS Variables--*/
//NOTE: Sine table is only positive values because of DAC (also dac is 12 bits)
uint16_t sine_table[N];
uint16_t dds_counter1 = 0, dds_counter2 = 0, ftv = 1;


/*--Configure DAC1--*/
void DAC1_Config(uint32_t sample_rate) {

	/*--Reset any DACs (this is safety check)--*/
	DAC_DeInit();

	/*--Configure GPIO for DAC1 (PA4)--*/
	DAC1_GPIO_Config();

	//NOTE: This may have to go after Configure DAC1?
	/*--Configure DAC1 timer (TIM6)--*/
	DAC1_Timer_Config(sample_rate);

	//NOTE: Figure out exactly how NVIC works
	/*--Configure DAC1 TIM6 Interrupt--*/
	DAC1_Interrupt_Config();

	/*--Configure DAC1--*/
	//Enable DAC clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	//Trigger on TIM6
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	//This buffer lets you drive external loads easier? See RM0090 (reference manual)
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	//Initialize and enable DAC1
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

/*--Configure GPIO for DAC1 (PA4)--*/
void DAC1_GPIO_Config(void) {
	//Enable GPIOA clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//Set structure to default values
	GPIO_StructInit(&GPIO_InitStructure);

	//Change default values
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Initialize PA4
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*--Configure DAC1 timer (TIM6)--*/
void DAC1_Timer_Config(uint32_t sample_rate) {
	//Local variables for computing clock dividers
	RCC_ClocksTypeDef RCC_Clocks; //to get current APB1 (PCLK1 speed)
	uint32_t timer_period; //to set TIM6 counter


	//Enable TIM6 clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	//Compute TIM6 max counter
	//NOTE: PCLK1 should be 42 MHz -- TIM6 operates at 2xPCLK1
	//NOTE: The timer starts at 0 so period is: actual period - 1
	RCC_GetClocksFreq(&RCC_Clocks);
	timer_period = RCC_Clocks.PCLK1_Frequency*2;
	timer_period = timer_period/sample_rate;
	timer_period = timer_period - 1;

	//Set max counter (period) for TIM6
	TIM_InitStructure.TIM_Period = timer_period;
	//NOTE: The actual prescaler is: TIM_Prescaler + 1
	TIM_InitStructure.TIM_Prescaler = 0;
	//NOTE: Not sure what this does
	TIM_InitStructure.TIM_ClockDivision = 0;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;

	//Initialize TIM6
	TIM_TimeBaseInit(TIM6, &TIM_InitStructure);

	//Set the trigger on update (when the counter gets to max counter)
	//This is for the DAC to transfer the next value to the output register
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	//Enables interrupt on update
	//This is so we can update the DAC at the right time
	//Which is different from the trigger
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

	//Enable the timer
	TIM_Cmd(TIM6, ENABLE);
}

/*--Configure DAC1 TIM6 Interrupt--*/
void DAC1_Interrupt_Config(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // three bits priority, one bit sub priority
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void DAC1_DDS_Config(uint32_t sample_rate, uint32_t target_frequency) {
	//Create sine table
	int16_t i = 0;
	for(i = 0; i < N; i++) {
		sine_table[i] = (4095/2)*sin(2*PI*i/(float)N)+(4095/2);
	}

	//f0 = FTV * fs/(2^Bftv)
	ftv = (1<<16)*(float)target_frequency/(float)sample_rate;

}

void TIM6_DAC_IRQHandler(void) {
	
	//how exactly does this BSRRL work
	GPIOE->BSRRL = GPIO_Pin_9; //used to time interrupt duration

	if(dds_counter1 == N) {
		dds_counter1 = 0;
	}

	uint16_t value = sine_table[dds_counter1];

	dds_counter2++;

	if(dds_counter2 == ftv) {
		dds_counter1++;
		dds_counter2 = 0;
	}

	DAC_SetChannel1Data(DAC_Align_12b_R, value); //dac1 flag = 1?

	//what is the point of doing this??
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);

	GPIOE->BSRRH = GPIO_Pin_9;
}
