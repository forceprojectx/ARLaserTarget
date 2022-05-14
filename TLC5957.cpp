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

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include "TLC5957.h"

/************************************************************************/
/*                            Using section                             */
/************************************************************************/

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

// for trimming only the lower 9 bits of a uint16_t
#define MASK_9_BIT_MAX 0x1FF

#define MASK_LODVTH 0x03
#define MASK_SEL_TD0 0x0C
#define MASK_SEL_GDLY 0x10
#define MASK_XREFRESH 0x20
#define MASK_SEL_GCK_EDGE 0x40
#define MASK_SEL_PCHG 0x80
#define MASK_ESPWM 0x100
#define MASK_LGSE3 0x200
#define MASK_SEL_SCK_EDGE 0x400
#define MASK_LGSE1 0x3800
#define MASK_CCB_LOW 0xC000
#define MASK_CCB_MID 0x7F
#define MASK_CCG_MID 0xFF80 
#define MASK_CCR_HIGH 0x1FF
#define MASK_BC_HIGH 0xE00
#define MASK_POKER_TRANS_MODE 0x1000
#define MASK_LGSE2 0xE000

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/

/************************************************************************/
/*                        Variables declarations                        */
/************************************************************************/


/************************************************************************/
/*                      Implementation (PRIVATE)                        */
/************************************************************************/


/************************************************************************/
/*                      Implementation (PROTECTED)                      */
/************************************************************************/


/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/


void FCRegister::SetBits_default()
{
	SetBit_LODVTH(LODVTH::VLOD1);
	SetBit_SEL_TD0(SEL_TD0::TD0_21NS);
	SetBit_SEL_GDLY(true);
	SetBit_XREFRESH(false);
	SetBit_SEL_GCK_EDGE(false);
	SetBit_SEL_PCHG(false);
	SetBit_ESPWM(false);
	SetBit_LGSE3(false);
	SetBit_SEL_SCK_EDGE(false);
	SetBit_LGSE1(LGSE::None);
	SetBit_CCB(0xFFFF);
	SetBit_CCG(0xFFFF);
	SetBit_CCR(0xFFFF);
	SetBit_BC(BC::Gain_100_0);
	SetBit_POKER_TRANS_MODE(0);
	SetBit_LGSE2(LGSE::None);
}

void FCRegister::SetBit_LODVTH(LODVTH value)
{

	Register_low &= ~MASK_LODVTH;
	switch (value)
	{
	case LODVTH::VLOD0:
		Register_low |= 0x00;
		break;
	case LODVTH::VLOD1:
		Register_low |= 0x01;
		break;
	case LODVTH::VLOD2:
		Register_low |= 0x02;
		break;
	case LODVTH::VLOD3:
		Register_low |= 0x03;
		break;
	default:
		Register_low |= 0x01;
		break;
	}

}

void FCRegister::SetBit_SEL_TD0(SEL_TD0 value)
{

	Register_low &= ~MASK_SEL_TD0;
	switch (value)
	{
	case SEL_TD0::TD0_18NS:
		Register_low |= 0x00;
		break;
	case SEL_TD0::TD0_21NS:
		Register_low |= 0x04;
		break;
	case SEL_TD0::TD0_24NS:
		Register_low |= 0x08;
		break;
	case SEL_TD0::TD0_12NS:
		Register_low |= 0x0C;
		break;
	default:
		Register_low |= 0x04;
		break;
	}
}

void FCRegister::SetBit_SEL_GDLY(bool value)
{
	if (value)
	{
		Register_low |= MASK_SEL_GDLY;
	}
	else
	{
		Register_low &= ~MASK_SEL_GDLY;
	}
}

void FCRegister::SetBit_XREFRESH(bool value)
{
	if (value)
	{
		Register_low |= MASK_XREFRESH;
	}
	else
	{
		Register_low &= ~MASK_XREFRESH;
	}
}

void FCRegister::SetBit_SEL_GCK_EDGE(bool value)
{
	if (value)
	{
		Register_low |= MASK_SEL_GCK_EDGE;
	}
	else
	{
		Register_low &= ~MASK_SEL_SCK_EDGE;
	}
}

void FCRegister::SetBit_SEL_PCHG(bool value)
{
	if (value)
	{
		Register_low |= MASK_SEL_PCHG;
	}
	else
	{
		Register_low &= ~MASK_SEL_PCHG;
	}
}

void FCRegister::SetBit_ESPWM(bool value)
{
	if (value)
	{
		Register_low |= MASK_ESPWM;
	}
	else
	{
		Register_low &= ~MASK_ESPWM;
	}
}

void FCRegister::SetBit_LGSE3(bool value)
{
	if (value)
	{
		Register_low |= MASK_LGSE3;
	}
	else
	{
		Register_low &= ~MASK_LGSE3;
	}
}

void FCRegister::SetBit_SEL_SCK_EDGE(bool value)
{
	if (value)
	{
		Register_low |= MASK_SEL_SCK_EDGE;
	}
	else
	{
		Register_low &= ~MASK_SEL_SCK_EDGE;
	}
}

void FCRegister::SetBit_LGSE1(LGSE value)
{
	Register_low &= ~MASK_LGSE1;
	switch (value)
	{
	case LGSE::None:
		Register_low |= 0x00;
		break;
	case LGSE::Weak:
		Register_low |= 0x0800;
		break;
	case LGSE::Medium:
		Register_low |= 0x2000;
		break;
	case LGSE::Strong:
		Register_low |= 0x3800;
		break;
	default:
		Register_low |= 0x00;
		break;
	}
}

void FCRegister::SetBit_CCB(uint16_t value)
{
	Register_low &= ~MASK_CCB_LOW;
	Register_mid &= ~MASK_CCB_MID;

	// keep only the lowest 9 bits
	value &= MASK_9_BIT_MAX;

	// move the lowest 2 bits from value to bits 14 and 15 of the low order byte
	Register_low |= ((value & 0x03) << 14);
	// move the top 7 bits from value to bits 0-6 of middle byte
	Register_mid |= value >> 2;
}

void FCRegister::SetBit_CCG(uint16_t value)
{
	Register_mid &= ~MASK_CCG_MID;

	// keep only the lowest 9 bits
	value &= MASK_9_BIT_MAX;

	// move the 9 bits from value to bits 7-15 of middle byte
	Register_mid |= value << 7;
}

void FCRegister::SetBit_CCR(uint16_t value)
{
	Register_high &= ~MASK_CCR_HIGH;

	// keep only the lowest 9 bits
	value &= MASK_9_BIT_MAX;

	// move the 9 bits from value to bits 0-8 of high byte
	Register_high |= value;
}

void FCRegister::SetBit_BC(BC value)
{
	Register_high &= ~MASK_BC_HIGH;
	switch (value)
	{
	case BC::Gain_20_0:
		Register_high |= 0x00;
		break;
	case BC::Gain_39_5:
		Register_high |= 0x200;
		break;
	case BC::Gain_58_6:
		Register_high |= 0x400;
		break;
	case BC::Gain_80_9:
		Register_high |= 0x600;
		break;
	case BC::Gain_100_0:
		Register_high |= 0x800;
		break;
	case BC::Gain_113_3:
		Register_high |= 0xA00;
		break;
	case BC::Gain_141_6:
		Register_high |= 0xC00;
		break;
	case BC::Gain_154_5:
		Register_high |= 0xE00;
		break;
	default:
		Register_high |= 0x800;
		break;
	}
}


void FCRegister::SetBit_POKER_TRANS_MODE(bool value)
{
	if (value)
	{
		Register_high |= MASK_POKER_TRANS_MODE;
	}
	else
	{
		Register_high &= ~MASK_POKER_TRANS_MODE;
	}
}

void FCRegister::SetBit_LGSE2(LGSE value)
{
	Register_high &= ~MASK_LGSE2;
	switch (value)
	{
	case LGSE::None:
		Register_high |= 0x00;
		break;
	case LGSE::Weak:
		Register_high |= 0x2000;
		break;
	case LGSE::Medium:
		Register_high |= 0xA000;
		break;
	case LGSE::Strong:
		Register_high |= 0xE000;
		break;
	default:
		Register_high |= 0x00;
		break;
	}
}


/************************************************************************/
/*                      Implementation (INTERRUPTS)                     */
/************************************************************************/
