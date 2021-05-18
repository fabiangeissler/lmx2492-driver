/*
 * lmx2492_driver.h
 *
 *  Created on: Mar 17, 2021
 *      Author: F. Geissler
 */

#ifndef LMX2492_DRIVER_H_
#define LMX2492_DRIVER_H_

#include <lmx2492_regdef.h>
#include <spislave.h>

#define USE_RICHARDS_FRACTION

namespace bsp
{

	class LMX2492Driver: public SpiSlave
	{
	public:
		LMX2492Driver(SPI_TypeDef * spi_instance, GPIO_TypeDef * cs_port, uint16_t cs_pin);

		virtual ~LMX2492Driver();

		// Perform a soft reset.
		bool Reset();

		// Write Power configuration.
		bool WritePowerConfig(uint8_t power_config);

		// Write PLL Config
		bool WriteConfig(LMX2492_Config_TypeDef* config);

		// Write PLL GPIO Config
		bool WriteGPIOConfig(LMX2492_GPIO_Config_TypeDef* gpio_config);

		// Write PLL Ramp Config
		bool WriteRampConfig(LMX2492_Ramp_Config_TypeDef* ramp_config);

		// Write PLL Ramp
		bool WriteRamp(LMX2492_Ramp_TypeDef* ramp, uint8_t ramp_idx);

		// Generate simple PLL configuration with a limited feature set from divider values
		static void SimpleConfig(LMX2492_Config_TypeDef* config, uint32_t N, uint8_t CPPOL, uint8_t CPG, uint32_t FRAC_NUM, uint32_t FRAC_DEN, uint16_t R, uint8_t OSC_2X);

		// Generate simple PLL configuration with a limited feature set from frequency values
		static void SimpleConfig(LMX2492_Config_TypeDef* config, float fout, float fref, uint8_t CPPOL, uint8_t CPG, uint16_t R, uint8_t OSC_2X);

		// Calculate pll divider values from frequencies
		static void DividerFromFrequency(float fout, float fref, uint32_t& N, uint32_t& FRAC_NUM, uint32_t& FRAC_DEN, uint16_t R = 1, uint8_t OSC_2X = 0);

		// Generate simple PLL GPIO configuration
		static void SimpleGPIOConfig(LMX2492_GPIO_Config_TypeDef* gpio_config,
				uint8_t TRIG1_MUX = LMX2492_MUX_IN_TRIG1, uint8_t TRIG1_PIN = LMX2492_PIN_TRISTATE,
				uint8_t TRIG2_MUX = LMX2492_MUX_IN_TRIG2, uint8_t TRIG2_PIN = LMX2492_PIN_TRISTATE,
				uint8_t MOD_MUX = LMX2492_MUX_IN_MOD, uint8_t MOD_PIN = LMX2492_PIN_TRISTATE,
				uint8_t MUXout_MUX = LMX2492_MUX_OUT_LD, uint8_t MUXout_PIN = LMX2492_PIN_TRISTATE);

		// Generate simple ramp config with a limited feature set
		static void SimpleRampConfig(LMX2492_Ramp_Config_TypeDef* ramp_config, uint8_t RAMP_EN = 1, uint8_t RAMP_CLK = 0, uint8_t RAMP_TRIGA = 0, uint16_t RAMP_COUNT = 0);

		// Generate simple ramp with a limited feature set
		static void SimpleRamp(LMX2492_Ramp_TypeDef* ramp, uint32_t RAMP_INC, uint16_t RAMP_LEN, uint8_t RAMP_NEXT = 0, uint8_t RAMP_RST = 0, uint8_t RAMP_NEXT_TRIG = 0, uint8_t RAMP_DLY = 0);

		// Calculate ramp INC and LEN from frequencies
		// df .. Frequency delta of the ramp in Hz
		// duration .. the ramp duration in seconds
		// finc .. ramp increment frequency in Hz (set to zero if fPFD is used)
		static void RampFromFrequency(float df, float fref, float duration, uint32_t& INC, uint16_t& LEN, float finc = 0, uint16_t R = 1, uint8_t OSC_2X = 0);

	private:
		// Write data to PLL register in reverse order
		bool WriteMemory(uint16_t last_byte_address, uint8_t *reversed_data, size_t size);
	};

}; /* namespace bsp */

#endif /* LMX2492_DRIVER_H_ */
