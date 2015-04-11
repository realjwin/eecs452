#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "dac_setup.h"
#include "support_setup.h"

int main(void)
{
	uint32_t sample_rate = 160000, freq0 = 40000, freq1 = 40700;
	char string[10] = "hello";
	SystemInit();
	Support_Config();
	DAC1_DDS_Config(sample_rate, freq0, freq1);
	DAC1_Config(sample_rate);


	DAC1_SendData(string);

	/*while(1) {
		if(DAC1_SendData_Done() == 0) {
			int16_t i = 0;
			for(i = 0; i < 100000; i++);
			DAC1_SendData(string);
		}
	}*/

	while(1);
}
