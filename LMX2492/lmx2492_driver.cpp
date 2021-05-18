/*
 * lmx2492driver.cpp
 *
 *  Created on: Mar 17, 2021
 *      Author: F. Geissler
 */

#include <lmx2492_driver.h>

#include <assert.h>
#include "richards_fraction.h"
#include "string.h"

namespace bsp {

LMX2492Driver::LMX2492Driver(SPI_TypeDef *spi_instance, GPIO_TypeDef *cs_port, uint16_t cs_pin)
 : SpiSlave(spi_instance, cs_port, cs_pin)
{ }

LMX2492Driver::~LMX2492Driver() { }

bool LMX2492Driver::Reset()
{
	uint8_t rst = LMX2492_SWRST_RESET;

	// execute soft reset
	if(!WriteMemory(LMX2492_SWRST_ADDR, &rst, 1)) return false;

	// TODO: Delay required?

	return true;
}

bool LMX2492Driver::WritePowerConfig(uint8_t power_config)
{
	assert(power_config <= 2);

	return WriteMemory(LMX2492_POWERDOWN_ADDR, &power_config, 1);
}

// Write PLL Config
bool LMX2492Driver::WriteConfig(LMX2492_Config_TypeDef* config)
{
	return WriteMemory(LMX2492_CONFIG_ADDRESS, (uint8_t*)config, sizeof(LMX2492_Config_TypeDef));
}

// Write PLL GPIO Config
bool LMX2492Driver::WriteGPIOConfig(LMX2492_GPIO_Config_TypeDef* gpio_config)
{
	return WriteMemory(LMX2492_GPIO_CONFIG_ADDRESS, (uint8_t*)gpio_config, sizeof(LMX2492_GPIO_Config_TypeDef));
}

// Write PLL Ramp Config
bool LMX2492Driver::WriteRampConfig(LMX2492_Ramp_Config_TypeDef* ramp_config)
{
	return WriteMemory(LMX2492_RAMP_CONFIG_ADDRESS, (uint8_t*)ramp_config, sizeof(LMX2492_Ramp_Config_TypeDef));
}

// Write PLL Ramp
bool LMX2492Driver::WriteRamp(LMX2492_Ramp_TypeDef* ramp, uint8_t ramp_idx)
{
	assert(ramp_idx <= 7);

	return WriteMemory(LMX2492_RAMP_ADDRESS(ramp_idx), (uint8_t*)ramp, sizeof(LMX2492_Ramp_TypeDef));
}

bool LMX2492Driver::WriteMemory(uint16_t address, uint8_t *data, size_t size)
{
	// assert parameters
	assert(address <= 0x8D); // Max PLL address space
	assert(data > 0);
	assert(size > 0);

	// Point to last byte of data
	data += (size - 1);
	address += (size - 1);

	// Create transmit address array (1 bit R/~W, 15 bit address)
	uint8_t txaddr[2] = { (uint8_t)(((uint8_t*)&address)[1] & 0x7F), ((uint8_t*)&address)[0] };

	// Configure bus
	if (!SpiConfig(SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE)) return false;
	// Begin SPI transfer
	if (!SpiStart()) return false;
	// Write txaddr to SPI bus
	if (!SpiWrite(txaddr, 2)) return false;

	// Write data to SPI bus in reverse byte order
	while(size-- > 0)
	{
		if (!SpiWrite(data--, 1)) return false;
	}

	// End SPI transfer
	if (!SpiEnd()) return false;

	return true;
}

void LMX2492Driver::SimpleConfig(LMX2492_Config_TypeDef* config, uint32_t N, uint8_t CPPOL, uint8_t CPG, uint32_t FRAC_NUM, uint32_t FRAC_DEN, uint16_t R, uint8_t OSC_2X)
{
	// assert parameters
	assert(N <= 0x3FFFF);
	assert(CPPOL <= 0x1);
	assert(CPG <= 31);
	assert(FRAC_NUM <= 0xFFFFFF);
	assert(FRAC_DEN <= 0xFFFFFF);
	assert(R <= 0xFFFF);
	assert(OSC_2X <= 0x1);

	// Reset struct
	memset(config, 0, sizeof(LMX2492_Config_TypeDef));

	// Fill struct
	config->PLL_N_7_0 = N & 0xFF;
	config->PLL_N_15_8 = (N >> 8) & 0xFF;
	config->PLL_N_17_16 = (N >> 16) & 0x03;

	config->CPPOL = CPPOL;
	config->CPG = CPG;

	config->PLL_R_7_0 = R & 0xFF;
	config->PLL_R_15_8 = (R >> 8) & 0xFF;

	config->OSC_2X = OSC_2X;

	config->FRAC_NUM_7_0 = FRAC_NUM & 0xFF;
	config->FRAC_NUM_15_8 = (FRAC_NUM >> 8) & 0xFF;
	config->FRAC_NUM_23_16 = (FRAC_NUM >> 16) & 0xFF;

	config->FRAC_DEN_7_0 = FRAC_DEN & 0xFF;
	config->FRAC_DEN_15_8 = (FRAC_DEN >> 8) & 0xFF;
	config->FRAC_DEN_23_16 = (FRAC_DEN >> 16) & 0xFF;

	//if(FRAC_NUM != 0)
	//{
		config->FRAC_ORDER = LMX2492_FRAC_ORDER_2;
	//}

	// POR values
	config->PFD_DLY = 1;
	config->DLD_PASS_CNT = 0x20;
	config->DLD_ERR_CNTR = 0x04;
	config->FRAC_DITHER = LMX2492_FRAC_DITHER_DISABLED;
	config->CMP_THR_HIGH = 0x32;
	config->CMP_THR_LOW = 0x0A;
}

void LMX2492Driver::SimpleConfig(LMX2492_Config_TypeDef* config, float fout, float fref, uint8_t CPPOL, uint8_t CPG, uint16_t R, uint8_t OSC_2X)
{
	uint32_t N, FRAC_NUM, FRAC_DEN;

	// Calculate dividers
	LMX2492Driver::DividerFromFrequency(fout, fref, N, FRAC_NUM, FRAC_DEN, R, OSC_2X);

	// Configure
	LMX2492Driver::SimpleConfig(config, N, CPPOL, CPG, FRAC_NUM, FRAC_DEN, R, OSC_2X);
}

void LMX2492Driver::DividerFromFrequency(float fout, float fref, uint32_t& N, uint32_t& FRAC_NUM, uint32_t& FRAC_DEN, uint16_t R, uint8_t OSC_2X)
{
	assert(fref > 0);
	assert(fout >= fref);
	assert(OSC_2X <= 0x1);

	// Calculate the dividers
	float rmul = (float)(OSC_2X + 1) / (float)R;
	float fPFD = fref * rmul;
	float Ndiv = fout / fPFD;

	N = (uint32_t)Ndiv;

#ifdef USE_RICHARDS_FRACTION
	richards_fraction(Ndiv - N, FRAC_NUM, FRAC_DEN);
#else
	FRAC_DEN = 0xFFFFFF;
	FRAC_NUM = (uint32_t)(FRAC_DEN * (Ndiv - N))
#endif
}

// Generate simple PLL GPIO configuration
void LMX2492Driver::SimpleGPIOConfig(LMX2492_GPIO_Config_TypeDef* gpio_config, uint8_t TRIG1_MUX, uint8_t TRIG1_PIN, uint8_t TRIG2_MUX, uint8_t TRIG2_PIN, uint8_t MOD_MUX, uint8_t MOD_PIN, uint8_t MUXout_MUX, uint8_t MUXout_PIN)
{
	assert(TRIG1_MUX <= 38);
	assert(TRIG1_PIN <= 7);

	assert(TRIG2_MUX <= 38);
	assert(TRIG2_PIN <= 7);

	assert(MOD_MUX <= 38);
	assert(MOD_PIN <= 7);

	assert(MUXout_MUX <= 38);
	assert(MUXout_PIN <= 7);

	// Reset struct
	memset(gpio_config, 0, sizeof(LMX2492_GPIO_Config_TypeDef));
	gpio_config->_0x23b2_0 = 1;
	gpio_config->_0x23b6 = 1;


	gpio_config->TRIG1_MUX_5 = (TRIG1_MUX >> 5) & 0x01;
	gpio_config->TRIG1_MUX_4_0 = TRIG1_MUX & 0x1F;
	gpio_config->TRIG1_PIN = TRIG1_PIN;

	gpio_config->TRIG2_MUX_5 = (TRIG2_MUX >> 5) & 0x01;
	gpio_config->TRIG2_MUX_4_0 = TRIG2_MUX & 0x1F;
	gpio_config->TRIG2_PIN = TRIG2_PIN;

	gpio_config->MOD_MUX_5 = (MOD_MUX >> 5) & 0x01;
	gpio_config->MOD_MUX_4_0 = MOD_MUX & 0x1F;
	gpio_config->MOD_PIN = MOD_PIN;

	gpio_config->MUXout_MUX_5 = (MUXout_MUX >> 5) & 0x01;
	gpio_config->MUXout_MUX_4_0 = MUXout_MUX & 0x1F;
	gpio_config->MUXout_PIN = MUXout_PIN;
}

void LMX2492Driver::SimpleRampConfig(LMX2492_Ramp_Config_TypeDef* ramp_config, uint8_t RAMP_EN, uint8_t RAMP_CLK, uint8_t RAMP_TRIGA, uint16_t RAMP_COUNT)
{
	assert(RAMP_EN <= 1);
	assert(RAMP_CLK <= 1);
	assert(RAMP_TRIGA <= 15);
	assert(RAMP_COUNT <= 0x1FFF);

	// Reset struct
	memset(ramp_config, 0, sizeof(LMX2492_Ramp_Config_TypeDef));

	ramp_config->RAMP_EN = RAMP_EN;
	ramp_config->RAMP_CLK = RAMP_CLK;
	ramp_config->RAMP_TRIG_A = RAMP_TRIGA;

	ramp_config->RAMP_COUNT_7_0 = RAMP_COUNT & 0xFF;
	ramp_config->RAMP_COUNT_12_8 = (RAMP_COUNT >> 8) & 0x1F;

	if(RAMP_COUNT != 0)
		ramp_config->RAMP_AUTO = 1;

	// Max. high limit (twos complement 33 bit)
	ramp_config->RAMP_LIMIT_HIGH_7_0 = 0xFF;
	ramp_config->RAMP_LIMIT_HIGH_15_8 = 0xFF;
	ramp_config->RAMP_LIMIT_HIGH_23_16 = 0xFF;
	ramp_config->RAMP_LIMIT_HIGH_31_24 = 0xFF;
	// Min. low limit (twos complement 33 bit)
	ramp_config->RAMP_LIMIT_LOW_32 = 0x1;
}

// TODO: finc as output parameter for optimum ramp accuracy to set the increment timer accordingly. For now it remains an input parameter
void LMX2492Driver::RampFromFrequency(float df, float fref, float duration, uint32_t& INC, uint16_t& LEN, float finc, uint16_t R, uint8_t OSC_2X)
{
	assert(OSC_2X <= 0x1);

	// Calculate the dividers
	float rmul = (float)(OSC_2X + 1) / (float)R;
	float fPFD = fref * rmul;
	float Ndiv = df / fPFD;

	// Check if alternate finc is given
	if (finc == 0)
		finc = fPFD;

	float lenf = duration * finc;

	// Check if value fits in uint16
	assert(lenf <= UINT16_MAX);

	// Round to closest int
	LEN = (uint16_t)(lenf + 0.5f);

	float incf = Ndiv * 16777216.0f / (float)LEN;

	// Check if value fits in 30 bit register
	assert(incf <= 0x3FFFFFFF);

	// Round to closest int
	INC = (uint32_t)(incf + 0.5f);
}

void LMX2492Driver::SimpleRamp(LMX2492_Ramp_TypeDef* ramp, uint32_t RAMP_INC, uint16_t RAMP_LEN, uint8_t RAMP_NEXT, uint8_t RAMP_RST, uint8_t RAMP_NEXT_TRIG, uint8_t RAMP_DLY)
{
	assert(RAMP_INC <= 0x3FFFFFFF);
	assert(RAMP_NEXT <= 7);
	assert(RAMP_RST <= 1);
	assert(RAMP_NEXT_TRIG <= 4);
	assert(RAMP_DLY <= 1);

	// Reset struct
	memset(ramp, 0, sizeof(LMX2492_Ramp_TypeDef));

	ramp->RAMPx_INC_29_24 = (RAMP_INC >> 24) & 0x3F;
	ramp->RAMPx_INC_23_16 = (RAMP_INC >> 16) & 0xFF;
	ramp->RAMPx_INC_15_8 = (RAMP_INC >> 8) & 0xFF;
	ramp->RAMPx_INC_7_0 = RAMP_INC & 0xFF;

	ramp->RAMPx_LEN_15_8 = (RAMP_LEN >> 8) & 0xFF;
	ramp->RAMPx_LEN_7_0 = RAMP_LEN & 0xFF;

	ramp->RAMPx_NEXT = RAMP_NEXT;
	ramp->RAMPx_RST = RAMP_RST;
	ramp->RAMPx_NEXT_TRIG = RAMP_NEXT_TRIG;
	ramp->RAMPx_DLY = RAMP_DLY;
}

} /* namespace bsp */
