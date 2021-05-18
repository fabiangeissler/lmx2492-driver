/*
 * app.cpp
 *
 *  Created on: Jul 21, 2020
 *      Author: F. Geissler
 */

#include "string.h"
#include "stdio.h"

#include "lmx2492_driver.h"

// Hardware device instance, GPIO defines by CubeMX
bsp::LMX2492Driver pll(SPI1, PLL_nCS_GPIO_Port, PLL_nCS_Pin);

// Initialize the PLL
void _init_lmx2492()
{
	// Issue a soft reset
	pll.Reset();

	// Pll configuration
	bsp::LMX2492_Config_TypeDef pll_config;
	bsp::LMX2492_GPIO_Config_TypeDef gpio_config;
	bsp::LMX2492_Ramp_Config_TypeDef ramp_config;
	bsp::LMX2492_Ramp_TypeDef ramp;

	bsp::LMX2492Driver::SimpleConfig(&pll_config,
			1.6e9, 					// < Output Frequency 1.6 GHz
			32e6,  					// < Reference Frequency 32 MHz
			LMX2492_CPPOL_POSITIVE,	// < Charge pump polarity positive
			LMX2492_CPG_1600UA, 	// < Charge pump gain 1600 uA
			1, 						// < R divider = 1
			LMX2492_OSC_2X_DISABLED	// < Frequency doubler disabled
	);

	bsp::LMX2492Driver::SimpleGPIOConfig(&gpio_config,
			LMX2492_MUX_IN_MOD, LMX2492_PIN_INPUT,	// < TRIG1 pin set to input connected to internal MOD bus
			LMX2492_MUX_IN_TRIG1, LMX2492_PIN_INPUT	// < TRIG2 pin set to input connected to internal TRIG 1 bus
	); // Other GPIOs set to default (tristate)

	bsp::LMX2492Driver::SimpleRampConfig(&ramp_config,
			LMX2492_RAMP_EN_ENABLE, 		// < Enable ramping functions
			LMX2492_RAMP_CLK_PD, 			// < Ramp increment clock from Phase Detector
			LMX2492_RAMP_TRIG_TRIG1_RISING,	// < Ramping trigger A from TRIG1 bus rising signal
			0								// < Ramp counter not used
	);

	uint16_t RAMP_LEN;
	uint32_t RAMP_INC;

	bsp::LMX2492Driver::RampFromFrequency(
			1.6e9, 					// < Frequency delta (3.2 GHz - 1.6 GHz = 1.6 GHz)
			32e6, 					// < Reference frequency 32 MHz
			1e-3,  					// < Ramp duration 1 ms
			RAMP_INC, 				// < Output ramp increment
			RAMP_LEN, 				// < Output ramp length
			0, 						// < Ramp increment frequency (0 => fPFD)
			1, 						// < R divider = 1
			LMX2492_OSC_2X_DISABLED // < Reference doubler disabled
	);

	// Ramp 2: Continuous frequency segment at end of ramp
	bsp::LMX2492Driver::SimpleRamp(&ramp,
			0, 								// < Ramp increment
			1000, 							// < Ramp length
			0, 								// < Next ramp: Ramp 0
			LMX2492_RAMPx_RST_DISABLE, 		// < Reset counter after this ramp
			LMX2492_RAMPx_NEXT_TRIG_NONE	// < No trigger
	);

	pll.WriteRamp(&ramp, 2);

	// Ramp 1: Frequency ramp segment
	bsp::LMX2492Driver::SimpleRamp(&ramp,
			RAMP_INC, 						// < Ramp increment
			RAMP_LEN, 						// < Ramp length
			2, 								// < Next ramp: Ramp 2
			LMX2492_RAMPx_RST_DISABLE, 		// < Reset counter after this ramp
			LMX2492_RAMPx_NEXT_TRIG_NONE	// < No trigger
	);

	pll.WriteRamp(&ramp, 1);

	// Ramp 0: Continuous frequency segment at start of ramp
	bsp::LMX2492Driver::SimpleRamp(&ramp,
			0, 								// < Ramp increment
			2, 								// < Ramp length (For some reason this must be at least 2 for the trigger to work properly)
			1, 								// < Next ramp: Ramp 1
			LMX2492_RAMPx_RST_ENABLE, 		// < Reset counter after this ramp
			LMX2492_RAMPx_NEXT_TRIG_TRIG_A	// < Wait for TRIGA before next ramp
	);

	pll.WriteRamp(&ramp, 0);

	uint8_t data[sizeof(ramp)];
	memcpy(data, &ramp, sizeof(ramp));

 	// Write all in reverse order (important)
	pll.WriteRampConfig(&ramp_config);
	pll.WriteGPIOConfig(&gpio_config);
	pll.WriteConfig(&pll_config);
	pll.WritePowerConfig(LMX2492_POWERDOWN_POWER_UP);
}

void main()
{
	// Init hardware
	_init_lmx2492();

	uint32_t tick;
	uint32_t trigtick = 0;
	int16_t val = 0;

	while (1) {
		tick = HAL_GetTick();

		// Trigger PLL ramp every 100 ms
		if ((tick - trigtick) > 100) {
			trigtick = tick;

			// Trigger PLL ramp, GPIO defines by CubeMX
			HAL_GPIO_WritePin(PLL_TRIG2_GPIO_Port, PLL_TRIG2_Pin, GPIO_PIN_SET);
			HAL_Delay(1);
			HAL_GPIO_WritePin(PLL_TRIG2_GPIO_Port, PLL_TRIG2_Pin, GPIO_PIN_RESET);
		}
	}
}
