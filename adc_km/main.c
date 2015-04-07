#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "adc_setup.h"
#include "support_setup.h"

int main(void)
{
	SystemInit();
	Support_Config();
	DAC1_Config_All();
	ADC1_Config(160000);
	
	while(1)
	{
	}
}
