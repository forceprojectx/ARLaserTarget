/**
* @brief      Light Sensor Setup and control functions
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
* @details    Uses ADC along with a phototransistor and voltage divider circuit to implement a light sensor circuit.
*               The voltage across the divider is measured by the internal ADC, and ultimately performs "hit" detection.
*
* @link       Datasheets:
*             KDT00030 Phototransistor: https://www.mouser.com/datasheet/2/308/1/KDT00030_D-2314605.pdf
*
**/

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include "LaserTarget.h"
#include "LightSensor.h"

/************************************************************************/
/*                            Using section                             */
/************************************************************************/

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

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
unsigned int LightSensor::ADC_value;

/************************************************************************/
/*                      Implementation (PRIVATE)                        */
/************************************************************************/

/************************************************************************/
/*                      Implementation (PROTECTED)                      */
/************************************************************************/

/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/

void LightSensor::InitGPIO()
{
	// Set P1.4 to input
	P1DIR &= ~IN_LASER_SENSOR;
	P1REN &= ~IN_LASER_SENSOR;
	P1IES &= ~IN_LASER_SENSOR;
    P1SEL0 |= IN_LASER_SENSOR;
    P1SEL1 |= IN_LASER_SENSOR;

	// Set Interrupts
	P1IFG &= ~IN_LASER_SENSOR;
	P1IE |= IN_LASER_SENSOR;

}

void LightSensor::InitADC()
{

    //Turn OFF ADC Module and clear sample settings.
    ADCCTL0 = 0;
    // Disable ADC interrupts
    ADCIE = 0x00;
    // Reset interrupt flags
    ADCIFG = 0x00;


    // Use TB1.1B as sample/hold signal to trigger conversion
    // Generate sample/hold signal to trigger ADC conversion

    // Timer_B clock source select
    TB1CCTL0 |= CCIE;
    TB1CTL = TBSSEL__ACLK;
    // Mode control. = Up mode: Timer counts up to TBxCL0
    TB1CTL |= MC_1;
    // Timer_B clear. Setting this bit clears TBxR, the clock divider logic (the divider setting remains unchanged), and the count direction.
    TB1CTL |= TBCLR;
    // TBxCCRn holds the data for the comparison to the timer value in the Timer_B Register, TBxR
    TB1CCR0 = 650;
    TB1CCR1 = 325;
    // Output mode: Toggle/reset
    TB1CCTL0 = OUTMOD_2;

    // Input divider expansion.
    // Divide by 0;
    TB1EX0 = TBIDEX_0;

    TB1CTL |= TBIE;

    // Make sure the ENC bit is cleared before configuring a Memory Buffer Control
// assert(!(ADCCTL0 & ADCENC));


#ifdef ADCPCTL4
        // In MSP430FR413x devices, the ADC pins are controlled by System Configuration Register 2
        // Enable ADC input pin
    SYSCFG2 |= LASER_SENSOR_ADCPCTL;
#endif

    // Use input pin defined by IN_LASER_SENSOR_ADCINCH (pin A4 in original schematic)
    // Use positive reference of AVcc
    // Use negative reference of AVss
    ADCMCTL0 = IN_LASER_SENSOR_ADCINCH;


    // Set Sample-Hold time to 16 ADCCLK cycles
    ADCCTL0 = ADCON | ADCSHT_2 | ADCMSC_1;

    // USE MODOSC 5MHZ Digital Oscillator as clock source
    // Set the Sample-and-Hold Source
    ADCCTL1 = ADCSHS_2
        // Set Clock Divider to 1
        + ADCDIV_0
        // Setup Clock Source
        + ADCSSEL_2
        // signal is sourced from the sampling timer.
        + ADCSHP_1
        // Enable and Start the conversion in Single-Channel, Single Conversion Mode
        + ADCCONSEQ_2;

    //Set Clock Pre-Divider
    // Use default clock divider of 1
    ADCCTL2 = ADCPDIV_0
        // set resolution to 12 bits
        + ADCRES_2;

    // Configure the ADC Memory Buffer



    // Clear the adc conversion complete interrupt flag
    ADCIFG &= ~ADCIFG0;



    // enable ADC
    ADCCTL0 |= ADCENC | ADCSC;
    ADCCTL0 |= ADCON;
    // Enable the ADC conversion complete Interrupt
    ADCIE |= ADCIE0;

}