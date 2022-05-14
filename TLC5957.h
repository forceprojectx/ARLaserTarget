/**
* @brief      Standard register and bit definitions for the Texas Instruments TLC5957 LED driver
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Standard register and bit definitions for the Texas Instruments TLC5957 LED driver
*				Includes routines and data structures for easier communication with the chip.
*
*
* @link       Datasheets:
*             TI datasheet (TLC5957): https://www.ti.com/lit/gpn/tlc5957
*             TI User Guide (TLC5957): https://www.ti.com/lit/pdf/slvuaf0
*             TI TLC5957 LED Driver : https://www.ti.com/lit/ds/symlink/tlc5957.pdf
**/

#pragma once
#ifndef TI_TLC5957_H
#define TI_TLC5957_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

// LOD detection threshold voltage.
enum class LODVTH
{
	// 0.09 V typ
	VLOD0,
	// 0.19 V typ
	VLOD1,
	// 0.35 V typ
	VLOD2,
	// 0.5 V typ
	VLOD3
};

// TD0 is the Time from SCLK Rising edge to SOUT first edge
enum class SEL_TD0
{
	TD0_18NS,
	TD0_21NS,
	TD0_24NS,
	TD0_12NS,

};

// Low Gray Scale Enhancement for Red/Green/Blue color
enum class LGSE
{
	None,
	Weak,
	Medium,
	Strong
};

// Global brightness control data for all output
enum class BC
{
	Gain_20_0,
	Gain_39_5,
	Gain_58_6,
	Gain_80_9,
	Gain_100_0,
	Gain_113_3,
	Gain_141_6,
	Gain_154_5
};


/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/
class FCRegister
{
public:
	uint16_t Register_high;
	uint16_t Register_mid;
	uint16_t Register_low;


	// Set the FC register to the default values as specified in the user guide.
	void SetBits_default();

	// 1 – 0 LODVTH
	// Default Value: 01b
	// LOD detection threshold voltage.
	//
	// These two bits select the detection threshold voltage for the LED open detection (LOD).
	// Table 5 shows the detect voltage truth table.
	// mask LODVTH 0x03
	void SetBit_LODVTH(LODVTH value);



	// 3 – 2 SEL_TD0
	// Default Value: 01b
	// TD0 select.
	// SOUT hold time is decided by TD0 definition and selection.
	// Table 6 shows the detail.
	// mask SEL_TD0 0x0C
	void SetBit_SEL_TD0(SEL_TD0 value);



	// 4 SEL_GDLY
	// Default Value: 1b
	//  Group delay select.
	//
	// When this bit is ‘0’, no delay between channels. All channels turn on at same time.
	//
	// When this bit is ‘1’, channels turn on with different delay time, thus the inrush
	// current is minimized. See Table 6 for more detail.
	// mask SEL_GDLY 0x10
	void SetBit_SEL_GDLY(bool value);



	// 5 XREFRESH
	// Default Value: 0b
	//  Auto data refresh mode bit. (0 = enabled, 1 = disabled)
	//
	// If LATGS/LINERESET command is input while this bit is ‘0’, auto data refresh
	// mode is enabled. All data in the 1st GS DATA latch are copied to the second GS
	// DATA latch until GS counter reaches the maximum count value of 65,536.
	// No OUTn are forced off and GS counter continues counting.
	//
	// If LATGS/LINERESET command is input while this bit is ‘1’, all data in the 1st GS
	// DATA latch are copied to the second GS DATA latch.
	// All OUTn are forced off and GS counter is reset to “0”.
	// Refer to Figure 10 for detailed timing.
	// mask XREFRESH 0x20
	void SetBit_XREFRESH(bool value);



	// 6 SEL_GCK_EDGE
	// Default Value: 0b
	//  GCLK edge select.
	//
	// When this bit is ‘0’, OUTn only turns on/off at the rising edge of GCLK
	// this is the default setting;
	//
	// When this bit is ‘1’, OUTn turn on/off at both edges (rising and falling) of GCLK.
	// At this condition, the maximum input GCLK is 16.5 MHz.
	// mask SEL_GCK_EDGE 0x40
	void SetBit_SEL_GCK_EDGE(bool value);



	// 7 SEL_PCHG
	// Default Value: 0b
	//  Pre-charge working mode select.
	//
	// When this bit is ‘0’:
	// After power on, Pre-charge FET is enabled.
	// When the 1st GCLK input, the precharge FET state depends on the GS data.
	// If GS = 0, then the pre-charge FET will always keep on.
	// If GS > 0, the pre-charge FET is turned off, and will be turned on again once the output channel is turned off.
	// This means the Pre-charge FET will only remain off during the period in which the channel is on.
	//
	// When this bit is ‘1’ :
	// After power on, Pre-charge FET is enabled.
	// When the 1st GCLK input, Pre-charge FET is turn off and remains off until the GS counter is reset to 0.
	// Once GS counter = ‘0’, pre-charge FET is turned on again.
	// This means the pre-charge FET is kept off during the whole segment period, and remains on during the dead-time
	// (the time between two adjacent segments).
	//
	// See Figure 27 for more detail.
	// mask SEL_PCHG 0x80
	void SetBit_SEL_PCHG(bool value);



	// 8 ESPWM
	// Default Value: 0b
	// ESPWM mode enable bit. (0 = enabled, 1 = disabled)
	//
	// When this bit is ‘0’, the conventional PWM control mode is selected.
	// When this bit is ‘1’, the ES-PWM control mode is selected. 
	//
	// If using poker transmode, this bit has to be set to ‘1’.
	// See Section 3.5.3 “Multiplexed Enhanced Spectrum(ES) PWM Control” for more detail.
	// mask ESPWM 0x100
	void SetBit_ESPWM(bool value);



	// 9 LGSE3
	// Default Value: 0b
	//  Compensation for Blue LED. (0 = disabled, 1 = enabled)
	//
	// When this bit is ‘0’, no compensation for Blue LED.
	// When this bit is ‘1’, internal circuit is enabled for Blue LED compensation.
	// mask LGSE3 0x200
	void SetBit_LGSE3(bool value);



	// 10 SEL_SCK_EDGE
	// Default Value: 0b
	//  SCLK edge select.
	//
	// When this bit is ‘0’, the GS data at the SIN pin shifts to the common shift register
	// only at the rising edge of SCLK, this is the default setting;
	//
	// When this bit is ‘1’, the GS data at the SIN pin shifts to the common shift register
	// at both edges (rising and falling) of SCLK. At this condition, the maximum input
	// SCLK is 18 MHz.
	//
	// FC (function control) data can only shift at the rising edge of SCLK.
	// See Section 3.13 “Double Edge for data transmission” for more detail.
	// mask SEL_SCK_EDGE 0x400
	void SetBit_SEL_SCK_EDGE(bool value);



	// 13 – 11 LGSE1
	// Default Value: 000b
	// Low Gray Scale Enhancement for Red/Green/Blue color
	//
	// Can be used to solve the 1st line issue commonly happens in high density, multiplexing panel
	// also is helpful for the white balance at low grayscale condition.
	//
	//  It functions as shown in the following:
	//   000b — no enhancement
	//   001b — weak enhancement
	//   100b — medium enhancement
	//   111b — strong enhancement
	// mask LGSE1 0x3800
	void SetBit_LGSE1(LGSE value);



	// 22 – 14 CCB
	// Default Value: 1 0000 0000b 
	// Color brightness control data for Blue color group (Data = 000h-1FFh. See Table 1)
	// mask CCB 0x7FC000
	void SetBit_CCB(uint16_t value);



	// 31 – 23 CCG
	// Default Value: 1 0000 0000b 
	// Color brightness control data for Green color group (Data = 000h-1FFh. See Table 1)
	// mask CCG 0xFF800000
	void SetBit_CCG(uint16_t value);



	// 40 – 32 CCR
	// Default Value: 1 0000 0000b
	// Color brightness control data for Red color group (Data = 000h-1FFh. See Table 1)
	// mask CCR 0x1FF00000000
	void SetBit_CCR(uint16_t value);



	// 43 – 41 BC
	// Default Value: 100b
	// Global brightness control data for all output (Data = 0h- 7h. See Table 2)
	// mask BC 0xE0000000000
	void SetBit_BC(BC value);



	// 44 POKER_TRANS_MODE
	// Default Value: 0b
	//  Poker trans mode enable bit. (0 = disabled, 1 = enabled)
	//
	// When this bit is ‘0’
	// traditional GS trans mode is enabled
	// PWM bit is 16
	// need to transfer 16-bit GS data for each OUTn.
	//
	// When this bit is ‘1’
	// poker GS trans mode enabled
	// PWM can be configured from 9 bit to 16 bit.
	// Transfer corresponding GS data for each PWM mode.
	//
	// See Section 3.4.3 “Poker trans mode for GS data” for more detail.
	// mask POKER_TRANS_MODE 0x100000000000
	void SetBit_POKER_TRANS_MODE(bool value);



	// 47 – 45 LGSE2
	// Default Value: 000b
	//  Besides LGSE1, these three bits are designed for the first line performance improvement at low grayscale condition.
	//  000b — no improvement
	//  001b — weak improvement
	//  101b — medium improvement
	//  111b — strong improvement
	// mask LGSE2 0xE00000000000
	void SetBit_LGSE2(LGSE value);


};

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/



#endif // !TI_TLC5957_H

