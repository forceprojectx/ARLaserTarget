/**
* @brief      Interrupt Functions and variables
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
* @details    define interrupts here in order to reduce clutter in main.cpp
*
**/

/************************************************************************/
/*                           Include section                            */
/************************************************************************/
#include "LaserTarget.h"
#include "Interrupts.h"
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
volatile uint32_t Interrupts::FrameInterruptCount = 0;
volatile bool Interrupts::LED_State = 0;
volatile int Interrupts::calibratedADC = 0;


/************************************************************************/
/*                      Implementation (PRIVATE)                        */
/************************************************************************/

/************************************************************************/
/*                      Implementation (PROTECTED)                      */
/************************************************************************/

/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/

/************************************************************************/
/*                      Implementation (INTERRUPTS)                     */
/************************************************************************/

// Timer 0 Interrupt Vector handler
// used for 
// INTERRUPT FLAG: TB0CCR0 CCIFG0
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_B0_VECTOR))) TIMER0_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{

}

// Timer0 Interrupt Vector (TB0IV) handler
// Used for the Pattern Frame Rate
// When this "ticks", the next frame of the light pattern will be drawn
// INTERRUPT FLAG: TB0CCR1 CCIFG1, TB0CCR2 CCIFG2, TB0IFG(TB0IV)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{


    switch (__even_in_range(TB0IV, 14))
    {
    case  0: break;                          // No interrupt
    case  2:
        // CCR1

        //DEBUG_3 ^= DEBUG_3_A;
        //DEBUG_3 ^= DEBUG_3_A;
        break;
    case  4:
        // CCR2
        // DEBUG_3 ^= DEBUG_3_B;
        // DEBUG_3 ^= DEBUG_3_B;


        break;
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14:
        //DEBUG_OUT ^= DEBUG_7;
        //DEBUG_OUT ^= DEBUG_7;
        Interrupts::LED_State = true;                  // overflow
        Interrupts::FrameInterruptCount++;
        break;
    default: break;
    }
}

// Timer 1 Interrupt Vector handler
// used for 
// INTERRUPT FLAG: TB1CCR0 CCIFG0 
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B0_VECTOR
__interrupt void TIMER1_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER1_B0_VECTOR))) TIMER1_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{

}

// Timer1 Interrupt Vector (TB1IV) handler
// Used for ADC sample clock
// When this "ticks", the next portion of the bit will be sent
// INTERRUPT FLAG: TB1CCR1 CCIFG1, TB1CCR2 CCIFG2, TB1IFG(TB1IV)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER1_B1_VECTOR))) TIMER1_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    DEBUG_6 ^= DEBUG_6_A;
    DEBUG_6 ^= DEBUG_6_A;

    switch (__even_in_range(TB1IV, 0x0E))
    {
    case  0: break;                          // No interrupt
    case  2:
        // CCR1
        //DEBUG_4 ^= DEBUG_4_A;

        break;
    case  4:
        // CCR2


        break;
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14:
        //DEBUG_3 ^= DEBUG_3_C;

        break;
    default: break;
    }
}


// Timer 2 Interrupt Vector handler
// used for 
// INTERRUPT FLAG: TB2CCR0 CCIFG0  
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_B0_VECTOR
__interrupt void TIMER2_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER2_B0_VECTOR))) TIMER2_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{

}

// Timer2 Interrupt Vector (TB2IV) handler
// Used for
// When this "ticks", the next portion of the bit will be sent
// INTERRUPT FLAG: TB2CCR1 CCIFG1, TB2CCR2 CCIFG2, TB2IFG (TB2IV)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER2_B1_VECTOR
__interrupt void TIMER2_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER2_B1_VECTOR))) TIMER2_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{

}



// Timer 3 Interrupt Vector handler
// used for 
// INTERRUPT FLAG: TB3CCR0 CCIFG0 
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER3_B0_VECTOR
__interrupt void TIMER3_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER3_B0_VECTOR))) TIMER3_B0_ISR(void)
#else
#error Compiler not supported!
#endif
{

}

// Timer3 Interrupt Vector (TB3IV) handler
// Used for
// When this "ticks", the next portion of the bit will be sent
// INTERRUPT FLAG: TB3CCR1 CCIFG1, TB3CCR2 CCIFG2, TB3CCR3 CCIFG3, TB3CCR4 CCIFG4, TB3CCR5 CCIFG5, TB3CCR6 CCIFG6, TB3IFG (TB3IV)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER3_B1_VECTOR
__interrupt void TIMER3_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER3_B1_VECTOR))) TIMER3_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{

}




// pin change interrupt
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=INPUT_VECTOR
__interrupt void inputInt(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(INPUT_VECTOR))) inputInt(void)
#else
#error Compiler not supported!
#endif
{
    DEBUG_4 ^= DEBUG_4_A;
    // disable pin change interrupt
    //INPUT_IE &= ~(IN_LP_COLLECT | IN_LP_NUDGE | IN_LP_PLAY | IN_SS_COIN_EN | IN_SW_PLAY);
    // clear interrupt flag
    //INPUT_IFG &= ~(IN_LP_COLLECT | IN_LP_NUDGE | IN_LP_PLAY | IN_SS_COIN_EN | IN_SW_PLAY);

    // enable debounce timer
    //DEBOUNCE_TIMER = DEBOUNCE_ON;

    // debug time
    //DEBUG_OUT &= ~DEBUG_7; // set 3.7 low for debugging (pin change interrupt off)

    P1IFG &= ~IN_LASER_SENSOR;                         // Clear P4.1 IFG
    Interrupts::calibratedADC = LightSensor::ADC_value;

    __no_operation();
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(ADC_VECTOR))) ADC_ISR(void)
#else
#error Compiler not supported!
#endif
{
    DEBUG_4 ^= DEBUG_4_A;
    DEBUG_4 ^= DEBUG_4_A;


    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    case ADCIV_NONE:
        break;
    case ADCIV_ADCOVIFG:
        break;
    case ADCIV_ADCTOVIFG:
        break;
    case ADCIV_ADCHIIFG:
        break;
    case ADCIV_ADCLOIFG:
        break;
    case ADCIV_ADCINIFG:
        break;
    case ADCIV_ADCIFG:
        LightSensor::ADC_value = ADCMEM0;
        // Sleep Timer Exits LPM3
        //__bic_SR_register_on_exit(LPM3_bits);

    // Clear the adc conversion complete interrupt flag
        ADCIFG &= ~ADCIFG0;
        break;
    default:
        break;
    }
}