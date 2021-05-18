/*
 * LMX2492_RegDef.h
 *
 *  Created on: 22.11.2018
 *      Author: F. Geissler
 */

#ifndef APPLICATION_LMX2492_REGDEF_H_
#define APPLICATION_LMX2492_REGDEF_H_

#include "stdint.h"

namespace bsp {

////////////////////////////////////////////////////////////////////////////
// LMX2492 revision ID register, POR value = 0x18
#define LMX2492_ID_ADDR			0x00

// POWERDOWN register
#define LMX2492_POWERDOWN_ADDR		0x02
// POWERDOWN[1:0]	State
// 0				Power Down, ignore CE
// 1				Power Up, ignore CE
// 2				use CE
// 3				reserved
#define LMX2492_POWERDOWN_POWER_DOWN	0x00
#define LMX2492_POWERDOWN_POWER_UP		0x01
#define LMX2492_POWERDOWN_POWER_CE		0x02

// SWRST software reset register
#define LMX2492_SWRST_ADDR		0x02
// (1 << 2)	reset all registers
#define LMX2492_SWRST_RESET		(1 << 2)

////////////////////////////////////////////////////////////////////////////
// General configuration registers and definitions
typedef struct {
	// begins at address 0x10 of LMX memory space
	uint8_t PLL_N_7_0;
	uint8_t PLL_N_15_8;
	uint8_t PLL_N_17_16 : 2;

	uint8_t FRAC_DITHER : 2;
	uint8_t FRAC_ORDER : 3;
	uint8_t _0x12b7 : 1; // unused bit

	uint8_t FRAC_NUM_7_0;
	uint8_t FRAC_NUM_15_8;
	uint8_t FRAC_NUM_23_16;

	uint8_t FRAC_DEN_7_0;
	uint8_t FRAC_DEN_15_8;
	uint8_t FRAC_DEN_23_16;

	uint8_t PLL_R_7_0;
	uint8_t PLL_R_15_8;

	uint8_t OSC_2X : 1;
	uint8_t _0x1Bb1 : 1; // unused bit
	uint8_t PLL_R_DIFF : 1;
	uint8_t PFD_DLY : 2;
	uint8_t FL_CSR : 2;
	uint8_t _0x1Bb7 : 1; // unused bit

	uint8_t CPG : 5;
	uint8_t CPPOL : 1;
	uint8_t _0x1Cb7_6 : 2; // unused bits

	uint8_t FL_CPG : 5;
	uint8_t FL_TOC_10_8 : 3;

	uint8_t CMP_THR_LOW : 6;
	uint8_t CMP_FLAGL : 1;
	uint8_t _0x1Eb7 : 1; // unused bit

	uint8_t CMP_THR_HIGH : 6;
	uint8_t CMP_FLAGH : 1;
	uint8_t _0x1Fb7 : 1; // unused bit

	uint8_t FL_TOC_7_0;

	uint8_t DLD_PASS_CNT;
	uint8_t DLD_ERR_CNTR : 5;
	uint8_t DLD_TOL : 3;
} LMX2492_Config_TypeDef;

#define LMX2492_CONFIG_ADDRESS	0x10
#define LMX2492_CONFIG_LAST_ADDRESS	(LMX2492_CONFIG_ADDRESS + sizeof(LMX2492_Config_TypeDef) - 1)

// FRAC_ORDER register
// Values
#define LMX2492_FRAC_ORDER_INTEGER 	0
#define LMX2492_FRAC_ORDER_1		1
#define LMX2492_FRAC_ORDER_2		2
#define LMX2492_FRAC_ORDER_3		3
#define LMX2492_FRAC_ORDER_4		4

// FRAC_DITHER register values
#define LMX2492_FRAC_DITHER_WEAK		0
#define LMX2492_FRAC_DITHER_MEDIUM		1
#define LMX2492_FRAC_DITHER_STRONG		2
#define LMX2492_FRAC_DITHER_DISABLED	3

// CPG register values
// 0 tri state
#define LMX2492_CPG_TRISTATE	0
// 1 ... 31 charge pump gain in x*100uA
#define LMX2492_CPG_100UA		1
#define LMX2492_CPG_200UA		2
#define LMX2492_CPG_300UA		3
#define LMX2492_CPG_400UA		4
#define LMX2492_CPG_500UA		5
#define LMX2492_CPG_600UA		6
#define LMX2492_CPG_700UA		7
#define LMX2492_CPG_800UA		8
#define LMX2492_CPG_900UA		9
#define LMX2492_CPG_1000UA		10
#define LMX2492_CPG_1100UA		11
#define LMX2492_CPG_1200UA		12
#define LMX2492_CPG_1300UA		13
#define LMX2492_CPG_1400UA		14
#define LMX2492_CPG_1500UA		15
#define LMX2492_CPG_1600UA		16
#define LMX2492_CPG_1700UA		17
#define LMX2492_CPG_1800UA		18
#define LMX2492_CPG_1900UA		19
#define LMX2492_CPG_2000UA		20
#define LMX2492_CPG_2100UA		21
#define LMX2492_CPG_2200UA		22
#define LMX2492_CPG_2300UA		23
#define LMX2492_CPG_2400UA		24
#define LMX2492_CPG_2500UA		25
#define LMX2492_CPG_2600UA		26
#define LMX2492_CPG_2700UA		27
#define LMX2492_CPG_2800UA		28
#define LMX2492_CPG_2900UA		29
#define LMX2492_CPG_3000UA		30
#define LMX2492_CPG_3100UA		31

// CPPOL register values
// 0 negative going VCO slope
#define LMX2492_CPPOL_NEGATIVE	0
// 1 positive going VCO slope
#define LMX2492_CPPOL_POSITIVE	1

// OSC_X2 register values
#define LMX2492_OSC_2X_DISABLED	0
#define LMX2492_OSC_2X_ENABLED	1

////////////////////////////////////////////////////////////////////////////
// GPIO configuration registers and definitions
typedef struct {
	// begins at address 0x23 of LMX memory space
	uint8_t _0x23b2_0 : 3; // unused bits
	uint8_t TRIG1_MUX_5 : 1;
	uint8_t TRIG2_MUX_5 : 1;
	uint8_t MUXout_MUX_5 : 1;
	uint8_t _0x23b6 : 1; // unused bit
	uint8_t MOD_MUX_5 : 1;

	uint8_t TRIG1_PIN : 3;
	uint8_t TRIG1_MUX_4_0 : 5;

	uint8_t TRIG2_PIN : 3;
	uint8_t TRIG2_MUX_4_0 : 5;

	uint8_t MOD_PIN : 3;
	uint8_t MOD_MUX_4_0 : 5;

	uint8_t MUXout_PIN : 3;
	uint8_t MUXout_MUX_4_0 : 5;
} LMX2492_GPIO_Config_TypeDef;

#define LMX2492_GPIO_CONFIG_ADDRESS			0x23
#define LMX2492_GPIO_CONFIG_LAST_ADDRESS	(LMX2492_GPIO_CONFIG_ADDRESS + sizeof(LMX2492_GPIO_Config_TypeDef) - 1)

// Pin drive definitions
#define LMX2492_PIN_TRISTATE		0
#define LMX2492_PIN_OPENDRAIN		1
#define LMX2492_PIN_PUSHPULL		2
#define LMX2492_PIN_GND				4
#define LMX2492_PIN_INVOPENDRAIN	5
#define LMX2492_PIN_INVPUSHPULL		6
#define LMX2492_PIN_INPUT			7

// Multi purpose multiplexer definitions
#define LMX2492_MUX_GND					0
#define LMX2492_MUX_IN_TRIG1			1
#define LMX2492_MUX_IN_TRIG2			2
#define LMX2492_MUX_IN_MOD				3
#define LMX2492_MUX_OUT_TRIG1			4
#define LMX2492_MUX_OUT_TRIG2			5
#define LMX2492_MUX_OUT_MOD				6
#define LMX2492_MUX_OUT_READBACK		7
#define LMX2492_MUX_OUT_CMP0			8
#define LMX2492_MUX_OUT_CMP1			9
#define LMX2492_MUX_OUT_LD				10
#define LMX2492_MUX_OUT_DLD				11
#define LMX2492_MUX_OUT_CPMONGOOD		12
#define LMX2492_MUX_OUT_CPMONHIGH		13
#define LMX2492_MUX_OUT_CPMONLOW		14
#define LMX2492_MUX_OUT_RAMPLIMEXC		15
#define LMX2492_MUX_OUT_RDIV2			16
#define LMX2492_MUX_OUT_RDIV4			17
#define LMX2492_MUX_OUT_NDIV2			18
#define LMX2492_MUX_OUT_NDIV4			19
#define LMX2492_MUX_OUT_CMP0RAMP		22
#define LMX2492_MUX_OUT_CMP1RAMP		23
#define LMX2492_MUX_OUT_FASTLOCK		28
#define LMX2492_MUX_OUT_CPG				29
#define LMX2492_MUX_OUT_FLAG0			30
#define LMX2492_MUX_OUT_FLAG1			31
#define LMX2492_MUX_OUT_TRIGA			32
#define LMX2492_MUX_OUT_TRIGB			33
#define LMX2492_MUX_OUT_TRIGC			34
#define LMX2492_MUX_OUT_RDIVIDE			35
#define LMX2492_MUX_OUT_CPUP			36
#define LMX2492_MUX_OUT_CPDN			37
#define LMX2492_MUX_OUT_RAMPCNTFIN		38

////////////////////////////////////////////////////////////////////////////
// Ramp configuration registers and definitions
typedef struct {
	// begins at address 0x3A of LMX memory space
	uint8_t RAMP_EN : 1;
	uint8_t RAMP_CLK : 1;
	uint8_t RAMP_PM_EN : 1;
	uint8_t _0x3Ab3 : 1; // unused bit
	uint8_t RAMP_TRIG_A : 4;
	uint8_t RAMP_TRIG_B : 4;
	uint8_t RAMP_TRIG_C : 4;

	uint8_t RAMP_CMP0_7_0;
	uint8_t RAMP_CMP0_15_8;
	uint8_t RAMP_CMP0_23_16;
	uint8_t RAMP_CMP0_31_24;
	uint8_t RAMP_CMP0_EN;

	uint8_t RAMP_CMP1_7_0;
	uint8_t RAMP_CMP1_15_8;
	uint8_t RAMP_CMP1_23_16;
	uint8_t RAMP_CMP1_31_24;
	uint8_t RAMP_CMP1_EN;

	uint8_t RAMP_CMP0_32 : 1;
	uint8_t RAMP_CMP1_32 : 1;
	uint8_t FSK_DEV_32 : 1;
	uint8_t RAMP_LIMIT_LOW_32 : 1;
	uint8_t RAMP_LIMIT_HIGH_32 : 1;
	uint8_t FSK_TRIG : 2;
	uint8_t _0x46b7 : 1; // unused bit

	uint8_t FSK_DEV_7_0;
	uint8_t FSK_DEV_15_8;
	uint8_t FSK_DEV_23_16;
	uint8_t FSK_DEV_31_24;

	uint8_t RAMP_LIMIT_LOW_7_0;
	uint8_t RAMP_LIMIT_LOW_15_8;
	uint8_t RAMP_LIMIT_LOW_23_16;
	uint8_t RAMP_LIMIT_LOW_31_24;

	uint8_t RAMP_LIMIT_HIGH_7_0;
	uint8_t RAMP_LIMIT_HIGH_15_8;
	uint8_t RAMP_LIMIT_HIGH_23_16;
	uint8_t RAMP_LIMIT_HIGH_31_24;

	uint8_t RAMP_COUNT_7_0;
	uint8_t RAMP_COUNT_12_8 : 5;
	uint8_t RAMP_AUTO : 1;
	uint8_t RAMP_TRIG_INC : 2;
} LMX2492_Ramp_Config_TypeDef;

#define LMX2492_RAMP_CONFIG_ADDRESS			0x3A
#define LMX2492_RAMP_CONFIG_LAST_ADDRESS	(LMX2492_RAMP_CONFIG_ADDRESS + sizeof(LMX2492_Ramp_Config_TypeDef) - 1)

// RAMP_EN register
// State defines:
#define LMX2492_RAMP_EN_DISABLE		0
#define LMX2492_RAMP_EN_ENABLE		1

// Ramp trigger defines
#define LMX2492_RAMP_TRIG_NEVER 0
#define LMX2492_RAMP_TRIG_TRIG1_RISING 1
#define LMX2492_RAMP_TRIG_TRIG2_RISING 2
#define LMX2492_RAMP_TRIG_MOD_RISING 3
#define LMX2492_RAMP_TRIG_DLD_RISING 4
#define LMX2492_RAMP_TRIG_CMP0 5
#define LMX2492_RAMP_TRIG_CPG_RISING 6
#define LMX2492_RAMP_TRIG_FLAG0_RISING 7
#define LMX2492_RAMP_TRIG_ALWAYS 8
#define LMX2492_RAMP_TRIG_TRIG1_FALLING 9
#define LMX2492_RAMP_TRIG_TRIG2_FALLING 10
#define LMX2492_RAMP_TRIG_MOD_FALLING 11
#define LMX2492_RAMP_TRIG_DLD_FALLING 12
#define LMX2492_RAMP_TRIG_CMP1 13
#define LMX2492_RAMP_TRIG_CPG_FALLING 14
#define LMX2492_RAMP_TRIG_FLAG0_FALLING 15

// RAMP_CLK register
// State defines:
#define LMX2492_RAMP_CLK_PD			0
#define LMX2492_RAMP_CLK_MOD		1

// RAMP_PM_EN register
// State defines:
#define LMX2492_RAMP_PM_EN_FM		0
#define LMX2492_RAMP_PM_EN_PM		1

////////////////////////////////////////////////////////////////////////////
// Single ramp registers and definitions
typedef struct {
	// begins at address 0x56 + x*7 of LMX memory space
	uint8_t RAMPx_INC_7_0 : 8;
	uint8_t RAMPx_INC_15_8 : 8;
	uint8_t RAMPx_INC_23_16 : 8;
	uint8_t RAMPx_INC_29_24 : 6;
	uint8_t RAMPx_FL : 1;
	uint8_t RAMPx_DLY : 1;

	uint8_t RAMPx_LEN_7_0;
	uint8_t RAMPx_LEN_15_8;

	uint8_t RAMPx_FLAG : 2;
	uint8_t RAMPx_RST : 1;
	uint8_t RAMPx_NEXT_TRIG : 2;
	uint8_t RAMPx_NEXT : 3;
} LMX2492_Ramp_TypeDef;

#define LMX2492_RAMP_ADDRESS(x) 		(0x56 + (sizeof(LMX2492_Ramp_TypeDef) * (x)))
#define LMX2492_RAMP_LAST_ADDRESS(x) 	(LMX2492_RAMP_ADDRESS(x) + sizeof(LMX2492_Ramp_TypeDef) - 1)

// RAMP0_FL register
// State defines:
#define LMX2492_RAMPx_FL_DISABLE	0
#define LMX2492_RAMPx_FL_ENABLE		1

// RAMP0_DLY register
// State defines:
#define LMX2492_RAMPx_DLY_1PFD		0
#define LMX2492_RAMPx_DLY_2PFD		1

// RAMP0_RST register
#define LMX2492_RAMPx_RST_OFFSET	2
// State defines:
#define LMX2492_RAMPx_RST_DISABLE	0
#define LMX2492_RAMPx_RST_ENABLE	1

// RAMP0_NEXT_TRIG register
// State defines:
#define LMX2492_RAMPx_NEXT_TRIG_NONE	0
#define LMX2492_RAMPx_NEXT_TRIG_TRIG_A	1
#define LMX2492_RAMPx_NEXT_TRIG_TRIG_B	2
#define LMX2492_RAMPx_NEXT_TRIG_TRIG_C	3

}

#endif /* APPLICATION_LMX2492_REGDEF_H_ */
