#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "dac_setup.h"

int main(void)
{
	SystemInit();
	DAC1_Config(100000);
	DAC1_DDS_Config(100000, 200);
	while(1);
}
