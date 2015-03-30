//Includes
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "adc_setup.h"
#include "dac_setup.h"

//Private Variables
uint16_t ADCResult;
uint16_t i = 0;
uint16_t adc_holder[100];

int main(void){

	SystemInit();


	RCC_Config_ADC1();
	GPIO_Config_ADC1();
	ADC1_Config();

	RCC_Config_DAC1();
	GPIO_Config_DAC1();
	DAC1_Config();

	//Print out current clock speed
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	printf("PCLK2 Speed: %d\r\n", RCC_Clocks.PCLK2_Frequency);

	while(1)
	{

		//In this code the ADC only samples every time it gets here
		//So this is not continuous sampling
	  ADC_SoftwareStartConv(ADC1);
	  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	  ADCResult= ADC_GetConversionValue(ADC1);

	  if(ADCResult > 4095) ADCResult = 4095;

	  /*if(i == 100) goto theend;
	  adc_holder[i] = ADCResult;
	  ++i;*/

	  DAC_SetChannel1Data(DAC_Align_12b_R, ADCResult);
	}

	/*theend:
	i=0;
	return 0;*/
}
