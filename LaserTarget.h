/**
* @brief      Definitions and declarations for the msp430 based
*               Laser Target System
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    TODO: More comments to quickly describe functionalities and
*                   the file content.
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/

#pragma once
#ifndef LASER_TARGET_H
#define LASER_TARGET_H

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include <msp430.h>
#include <stdint.h>

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

//-------------------------
//    DEBOUNCE TIMER
//-------------------------
#define DEBOUNCE_TRIGGER    30
#define DEBOUNCE_TIMER      TB1CTL

// SMCLK/8, up/down moode, clear TBR
#define DEBOUNCE_ON         TBSSEL__SMCLK+ID_3+MC_3+TBCLR+TBIE 
#define DEBOUNCE_OFF        TBSSEL__SMCLK+ID_3+MC_3+TBCLR 

//-------------------------
//    FRAME TIMER
//-------------------------
#define FRAME_TIMER         TB0CTL
// SMCLK/2, continuous mode, clear TBR
#define FRAME_TIMER_ON      TBSSEL__SMCLK+ID_1+MC_2+TBCLR+TBIE 
#define FRAME_TIMER_OFF     TBSSEL__SMCLK+ID_1+MC_2+TBCLR
// 30KHz is a reasonable minimum to keep switching out of the audible frequency
#define PWM_30_KHZ          40                                      
/* a value of 40 here produces a 3KHz pwm frequeny,
which is about as much as my test circuit will suffer due to slow switching transistors*/
#define PWM_MAX             7
#define PWM_TRIGGER         PWM_30_KHZ*PWM_MAX


//-------------------------
//    Output pins
//-------------------------
//these values are for building bitmasks
//so should be between 0 and 7 inclusive
// NOTE: Output is disabled when OUTPUT_EN is HIGH, and enabled when OUTPUT_EN shifts to LOW
#define HALO_DATA_OUT   P3OUT
#define HALO_CLK_SIG     BIT6               //P3.6
#define HALO_DATA_LED    BIT5            //P3.5
#define HALO_OUTPUT_EN   BIT1            //P6.0?
#define HALO_LATCH_LED   BIT7            //P3.7

#define BEZEL_OUT       P2OUT
#define BEZEL_RED       BIT1            //P2.1
#define BEZEL_GREEN     BIT2            //P2.2
#define BEZEL_BLUE      BIT3            //P2.3

#define USB_CHRG_OUT    P2OUT
#define USB_CHRG_PIN    BIT0            //P2.0

//-------------------------
//    Comms pins
//-------------------------

// bluetooth communication pin assignments
#define BT_SEL0			P1SEL0
#define BT_SEL1			P1SEL1
#define BT_SELC			P1SELC
#define BT_DIR			P1DIR
#define BT_REN			P1REN
#define BT_IES			P1IES
#define BT_IFG			UCA0IFG
#define BT_IE			UCA0IE
// eUSCI_A0 Control Word Register 0
#define BT_CTLW0		UCA0CTLW0
// eUSCI_Ax Modulation Control Word Register
#define BT_MCTLW		UCA0MCTLW
// Clock prescaler setting of the Baud rate generato
#define BT_BRW			UCA0BRW

// bluetooth Tx pin (uC Rx)
#define BT_TX			BIT6
// bluetooth Rx pin (uC Tx)
#define BT_RX			BIT7

//-------------------------
//    Input pins
//-------------------------
#define IN_LASER_SENSOR BIT4
#define LASER_SENSOR_ADCPCTL    ADCPCTL4
// Use input A4
#define IN_LASER_SENSOR_ADCINCH ADCINCH_4


//-------------------------
//    debugging pins
//-------------------------
// spare IO might as well be used for debugging

// J15 pin 1-5
#define DEBUG_2         P2OUT
#define DEBUG_2_START   4
#define DEBUG_2_A       BIT4
#define DEBUG_2_B       BIT5
#define DEBUG_2_C       BIT6
#define DEBUG_2_D       BIT7

// J7 Pins 4,5,6
#define DEBUG_3         P6OUT
#define DEBUG_3_A       BIT5
#define DEBUG_3_B       BIT6
#define DEBUG_3_C       BIT7

// J7 Pins 2,3
#define DEBUG_4         P4OUT
#define DEBUG_4_A       BIT0
#define DEBUG_4_B       BIT1

#define DEBUG_6         P6OUT
#define DEBUG_6_A       BIT3


//-------------------------
//    Input values
//-------------------------
#define INPUT_PORT  P1IN
#define INPUT_DIR   P1DIR
#define INPUT_REN   P1REN
#define INPUT_OUT   P1OUT
#define INPUT_IE    P1IE
#define INPUT_IES   P1IES
#define INPUT_IFG   P1IFG
#define INPUT_VECTOR    PORT1_VECTOR
#define INPUT_MASK  0x1F


//-------------------------
// Game Modes
//-------------------------

// Standard : All targets begin active. When targets are hit, they become inactive for a short time then reactivate.
// Clear : All targets begin active. When targets are hit, they become inactive. When all targets have been hit, they re-activate.
// Chase : active target moves randomly between targets. user must find and shoot active target before time runs out.
// Shoot/No-Shoot : Same as clear, but some targets must NOT be shot.


//==============
//-------------------------
// Game State Patterns
//-------------------------
//==============

// LED halo Pattern IDs go here for different game modes
#define STANDARD_PATTERN 0x01
#define NO_SHOOT_PATTERN 0x02
#define TIME_TICK_PATTERN 0x03


//-------------------------
// Time Constants
//-------------------------

#define IDLE_TIME_COUNT 500 //500
#define LOCKOUT_TIME_COUNT 25
#define TARGET_HIT_FRAME_COUNT 21


#define EEPROM_SIZE     8192

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/
enum LEDCommands { STORE_PATTERN = 1, PLAY_PATTERN, PLAY_IDLE, RETRIEVE_EEPROM };

/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/

void Setup(void);
void Loop(void);

void RenderHaloFrame(void);
void debounce(void);
void doIdle(void);
uint8_t readkeys(void);

void DigitalBezel(void);
void Reset_ISR(void);

#endif // !LASER_TARGET_H

