/*
 * SpiSlave.cpp
 *
 *  Created on: 14.11.2018
 *      Author: F. Geissler
 */

#include <spislave.h>

// Declaration of static member
std::set<bsp::SpiSlave const *> bsp::SpiSlave::instances_;

bsp::SpiSlave::SpiSlave(SPI_TypeDef *spi_instance, GPIO_TypeDef *cs_port,
		uint16_t cs_pin)
{
	// Add instance to set
	bsp::SpiSlave::instances_.insert(this);

	hspi_.Instance = spi_instance;
	cs_port_ = cs_port;
	cs_pin_ = cs_pin;

	// Default state high
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

	// No transfer started
	transfer_started_ = false;

	// non-changing SPI configuration
	hspi_.Init.Mode = SPI_MODE_MASTER;
	hspi_.Init.Direction = SPI_DIRECTION_2LINES;
	hspi_.Init.NSS = SPI_NSS_SOFT;
	hspi_.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi_.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi_.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi_.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi_.Init.CRCPolynomial = 7;
	hspi_.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi_.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
}

bsp::SpiSlave::~SpiSlave()
{
	// Remove instance from set
	bsp::SpiSlave::instances_.erase(this);
}

bool bsp::SpiSlave::SpiStart()
{
	// Check other instances in set
	std::set<SpiSlave const *>::iterator i;

	for (i = bsp::SpiSlave::instances_.begin(); i != bsp::SpiSlave::instances_.end(); ++i) {
		// Resolve iterator
		const SpiSlave *inst = *i;

		// Check SPI peripheral instance and transfer started flag
		if ((inst->hspi_.Instance == this->hspi_.Instance) && inst->transfer_started_)
			return false;
	}

	transfer_started_ = true;

	// Pull CS pin
	HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_RESET);

	return true;
}

bool bsp::SpiSlave::SpiEnd() {
	if (!transfer_started_)
		return true;

	// TODO: With interrupt or DMA driven SPI this method needs to check for tx or rx complete flag.

	// At the moment HAL waits for SPI transfer complete automatically
	HAL_GPIO_WritePin(cs_port_, cs_pin_, GPIO_PIN_SET);

	transfer_started_ = false;

	return true;
}

bool bsp::SpiSlave::SpiWrite(uint8_t *data, size_t size)
{
	if (!transfer_started_)
		return false;

	// write buffer and wait
	HAL_SPI_Transmit(&hspi_, data, size, HAL_MAX_DELAY);

	return true;
}

bool bsp::SpiSlave::SpiRead(uint8_t *data, size_t size)
{
	if (!transfer_started_)
		return false;

	// read to buffer and wait
	HAL_SPI_Receive(&hspi_, data, size, HAL_MAX_DELAY);

	return true;
}

bool bsp::SpiSlave::SpiTransceive(uint8_t* txdata, uint8_t* rxdata, size_t size)
{
	if (!transfer_started_)
		return false;

	// read to buffer and wait
	HAL_SPI_TransmitReceive(&hspi_, txdata, rxdata, size, HAL_MAX_DELAY);

	return true;
}

bool bsp::SpiSlave::SpiConfig(uint32_t data_size, uint32_t clk_polarity,
		uint32_t clk_phase)
{
	/* SPI1 parameter configuration*/
	hspi_.Init.DataSize = data_size;
	hspi_.Init.CLKPolarity = clk_polarity;
	hspi_.Init.CLKPhase = clk_phase;

	if (HAL_SPI_Init(&hspi_) != HAL_OK)
	{
		return false;
	}

	return true;
}
