/**
* @brief      Laser Target System -- LED Controller Test
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Detect Laser "hits", 16 RGB LED display, mesh network for target game modes like chase, bluetooth for app connectivity
*
*
* @link       Datasheets:
*             TI datasheet (MSP430FR2355): https://www.ti.com/lit/ds/slasec4d/slasec4d.pdf?ts=1650677447571
*             TI User Guide (MSP430FR2355): https://www.ti.com/lit/ug/slau445i/slau445i.pdf
*             TI TLC5957 LED Driver : https://www.ti.com/lit/ds/symlink/tlc5957.pdf
**/

/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include <msp430.h>
#include "LaserTarget.h"
#include "HaloPattern.h"

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

//-------------------------
//    debounce stuff
//-------------------------

//debounced state of the switches
uint8_t debounced_state = 1;
//previous debounced state of the switches
uint8_t debounced_state_prev = 0;

// if true, call debounce
volatile bool doDebounce = false;
//number of times the debounce timer has overflowed
volatile uint8_t debounceTimerCount = 0;



//-------------------------
//    Halo Animations
//-------------------------

// FOR FPS CALCULATIONS
//the number of times the LED timer has overflowed
volatile int LED_OverflowCnt = 0;
// do we need to transition to the next led animation frame
volatile bool LED_State = 0;
// how many frames have been rendered
volatile uint32_t FrameRenderCount = 0; 


//-------------------------
//    EEPROM Vars
//-------------------------

uint16_t crc;
uint16_t crc_msg = 0;
uint32_t eepromStored = 0;
uint32_t eepromPatternArraySize = 0;
uint32_t eepromMsgLen = 0;



uint8_t IdleIndex;

// the current state for the main CPU loop
LEDCommands _state = PLAY_IDLE;



/************************************************************************/
/*                      Implementation (SETUP)                          */
/************************************************************************/

inline void Setup(void)
{
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;

    // turn off adc
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 &= ~(ADCON_1);
    ADCCTL0 = 0;
    ADCCTL1 &= ~ADCENC;
    ADCCTL1 &= ~(ADCON_1);
    ADCCTL1 = 0;
    ADCCTL2 &= ~ADCENC;
    ADCCTL2 &= ~(ADCON_1);
    ADCCTL2 = 0;

    // CLOCK CONFIG

    // Configure DCO
    // DCORSEL_7 = 24 MHz(Only avaliable in 24MHz clock system)
    // DCOFTRIMEN_0 = Disable frequency trim. DCOFTRIM value is bypassed and the DCO applies default settings from manufacture
    CSCTL1 = DCORSEL_7 + DCOFTRIMEN_0;

    // SELA_2 --->> Set ACLK = VLO (internal 10-kHz clock source); 
    // SELMS_0 --->> Selects DCOCLKDIV as the MCLK and SMCLK source 
    CSCTL4 = SELA_2 + SELMS_0;
    

    // set dividers SMCLK/8; MCLK/1
    CSCTL5 = DIVS_3 + DIVM_0 + VLOAUTOOFF_1;
    // set dividers ACLK/1;
    CSCTL6 = DIVA_0;
    // NOTE: at this point, on the msp430FR2355, The clock values will be:
    // NOTE: VLOCLK: Internal very-low-power low-frequency oscillator with 10-kHz typical frequency
    // NOTE: DCO  -- 24 MHz
    // NOTE: ACLK -- ~10 KHz (~0.01 MHz)
    // NOTE: SMCLK-- 3 MHz
    // NOTE: MCLK -- 24 MHz
    // NOTE: VLO will be shut off if unused.


    // set Port Registers to output
    // turn output off
    P1DIR |= 0xFF;
    P1OUT &= 0x00;
    P2DIR |= 0xFF;
    P2OUT &= 0x00;
    P3DIR |= 0xFF;
    P3OUT &= 0x00;
    P4DIR |= 0xFF;
    P4OUT &= 0x00;
    P5DIR |= 0xFF;
    P5OUT &= 0x00;
    P6DIR |= 0xFF;
    P6OUT &= 0x00;

    PADIR |= 0xFF;
    PAOUT &= 0x00;
    PBDIR |= 0xFF;
    PBOUT &= 0x00;
    PCDIR |= 0xFF;
    PCOUT &= 0x00;
    // PJDIR |= 0xFF;
    //PJOUT &= 0;



    //P4DIR &= ~BIT0; // set pin 4.0 to input
    //P4REN |= BIT0; // turn on internal pull up/down resistor
    //P4OUT |= BIT0; // select pull-up mode for p4.0

    //P4IE = 0; //p4.0 interupt disabled
    //P4IES |= BIT0 | BIT1; // hi/lo edge;
    //P4IFG &= ~BIT0; // clear interrupt flag


    // halo frame rate timer
    FRAME_TIMER = FRAME_TIMER_ON;

    // debounce timer
    // NOTE: debounce timer to be started by edge trigger and stopped by debounce function
    // stop timer / set timer time to 0
    TB1CCR0 = 0;
    //configure timer
    TB1CTL = TBSSEL__SMCLK + ID_3 + MC_3 + TBCLR + TBIE;

    // count up to DEBOUNCE_TRIGGER, then fire interrupt
    TB1CCR0 = 20000;
    TB1CCR1 = 200;
    TB1CCR2 = 100;
    TB1CCTL1 = CCIE;
    TB1CCTL2 = 0;


    TB0CCR1 = PWM_TRIGGER;
    TB0CCTL1 = CCIE;
    TB0CCR2 = PWM_TRIGGER + 1;
    TB0CCTL2 = CCIE;

    DEBUG_3 &= ~DEBUG_2_A;
    DEBUG_2 &= ~DEBUG_2_B;
    DEBUG_2 &= ~DEBUG_2_C;
    DEBUG_3 &= ~BIT2;
    DEBUG_3 &= ~DEBUG_3_C;
    DEBUG_3 &= ~DEBUG_3_B;
    DEBUG_3 &= ~DEBUG_3_A;
    DEBUG_4 &= ~DEBUG_4_A;
    DEBUG_4 &= ~DEBUG_4_B;




    // led "frame" vars
    LED_OverflowCnt = 0;
    LED_State = false;

    //setup debounce vars
    doDebounce = false;
    debounceTimerCount = 0;
    debounced_state_prev = 0;
    debounced_state = 0;
    FrameRenderCount = 0;
    IdleIndex = 0;

    // setup eeprom
    crc_msg = 0;
    _state = LEDCommands::PLAY_IDLE;
    eepromStored = 0;

    
    eepromMsgLen = 0;

    //debounce();
    InitLEDController();

    __enable_interrupt();
    //__bis_SR_register(GIE);       // Enter LPM3, enable interrupts
    //__no_operation();                         // For debugger
}



inline void Loop(void)
{

    if (doDebounce)
    {
        debounce();
        __no_operation();
        doDebounce = false;
    }
    if (LED_State)
    {
        //DEBUG_4 ^= DEBUG_4_B;
        //DEBUG_4 ^= DEBUG_4_B;
        RenderHaloFrame();
        LED_State = false;

        __no_operation();
    }





    __no_operation();                         // For debugger
}

/************************************************************************/
/*                      Implementation (INTERRUPTS)                     */
/************************************************************************/
// TimerB Interrupt Vector (TBIV) handler
// Used for the Pattern Frame Rate
// When this "ticks", the next frame of the light pattern will be drawn
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
        LED_State = true;                  // overflow
        FrameRenderCount++;
        break;
    default: break;
    }
}

// TimerB Interrupt Vector (TBIV) handler
// Used for the digital led bezel
// When this "ticks", the next portion of the bit will be sent
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER1_B1_VECTOR))) TIMER1_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(TB1IV, 0x0E))
    {
    case  0: break;                          // No interrupt
    case  2:
        // CCR1
        DEBUG_4 ^= DEBUG_4_A;
        TB1CCR1 += TB1CCR1;

        break;
    case  4:
        // CCR2

        DEBUG_4 ^= DEBUG_4_B;
        TB1CCR2 += TB1CCR2;

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

// Timer B Interrupt Vector (TAIV) handler
// used for debouncing input
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER3_B1_VECTOR
__interrupt void TIMER3_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER3_B1_VECTOR))) TIMER3_B1_ISR(void)
#else
#error Compiler not supported!
#endif
{
    switch (__even_in_range(TB3IV, 0x0E))
    {
    case  0: break;                          // No interrupt
    case  2:
        // CCR1
        //DEBUG_3 ^= DEBUG_3_A;

        break;
    case  4:
        // CCR2

        DEBUG_3 ^= DEBUG_3_B;

        break;
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14:
        DEBUG_3 ^= DEBUG_3_C;
        break;
    default: break;
    }
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
    // disable pin change interrupt
    //INPUT_IE &= ~(IN_LP_COLLECT | IN_LP_NUDGE | IN_LP_PLAY | IN_SS_COIN_EN | IN_SW_PLAY);
    // clear interrupt flag
    //INPUT_IFG &= ~(IN_LP_COLLECT | IN_LP_NUDGE | IN_LP_PLAY | IN_SS_COIN_EN | IN_SW_PLAY);

    // enable debounce timer
    //DEBOUNCE_TIMER = DEBOUNCE_ON;

    // debug time
    //DEBUG_OUT &= ~DEBUG_7; // set 3.7 low for debugging (pin change interrupt off)


    __no_operation();
}


/************************************************************************/
/*                      Implementation (PUBLIC)                         */
/************************************************************************/

int main(void)
{


    Reset_ISR();
    Setup();

    while (1)
    {
        Loop(); Reset_ISR();
        __no_operation();

    }
    __no_operation();

}

void RenderHaloFrame(void)
{
    if (FrameRenderCount > IDLE_TIME_COUNT)
    {
        doIdle();
    }

    // Render the next animation frame.
    // If the animation returns false, the last frame has been rendered
    if (!RedCCW(FrameRenderCount))
    {
        FrameRenderCount = 0;
    }

    LED_State = false;
    __no_operation();
}

int16_t findHaloPattern(void)
{
    return 0;
    __no_operation();
}

void debounce(void)
{
    debounced_state = readkeys();

    //mirror inputs on an output
    //DEBUG_2 &= 0x0F;
    //DEBUG_2 |= (debounced_state << DEBUG_2_START);

    /*DEBUG_4 ^= DEBUG_4_A;
    DEBUG_4 ^= DEBUG_4_A;*/

    // Input port interupt enabled
    //INPUT_IE |= (IN_LP_COLLECT | IN_LP_NUDGE | IN_LP_PLAY | IN_SS_COIN_EN | IN_SW_PLAY);
    //DEBUG_OUT |= DEBUG_7; // set 3.7 high for debugging (pin change interrupt on)



    doDebounce = false;
    debounced_state_prev = debounced_state;
    __no_operation();
}

//-------------------------
//    read the keys into an byte
//-------------------------
uint8_t readkeys(void)
{
    uint8_t ret = 0x7F;   //3F (0b00111111)

    uint8_t input = INPUT_PORT;

    // set the interrupt direction
    // BIT value of 1 triggers on falling edge. (hi->lo)
    // BIT value of 0 triggers on rising edge. (lo->hi)
    // if a signal is low (0), the next edge will be rising
    INPUT_IES = input;

    //inverted because we are using active low
    ret &= ~(input);

    return ret;
}


void doIdle(void)
{
    FrameRenderCount = 0;
    IdleIndex++;
    __no_operation();
}


void Reset_ISR(void)
{
    int var = _even_in_range(SYSRSTIV, SYSRSTIV__FLLUL);
    if (var != 0)
    {
        switch (var)
        {
        case  SYSRSTIV_NONE:        /* No Interrupt pending */
            __no_operation();
            break;

        case  SYSRSTIV_BOR:        /* SYSRSTIV : BOR */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_RSTNMI:        /* SYSRSTIV : RST/NMI */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_DOBOR:        /* SYSRSTIV : Do BOR */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_LPM5WU:        /* SYSRSTIV : Port LPM5 Wake Up */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_SECYV:        /* SYSRSTIV : Security violation */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_SVSHIFG:        /* SYSRSTIV : SVSHIFG */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_DOPOR:        /* SYSRSTIV : Do POR */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_WDTTO:        /* SYSRSTIV : WDT Time out */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_WDTKEY:        /* SYSRSTIV : WDTKEY violation */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        case  SYSRSTIV_FRCTLPW:        /* SYSRSTIV : FRAM Key violation */
            __no_operation();
            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            DEBUG_3 ^= DEBUG_3_C;
            DEBUG_3 ^= DEBUG_3_C;

            break;

        default:
            __no_operation();
            break;
        }
    }

}

