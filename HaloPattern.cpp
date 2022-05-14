/**
* @brief      LED Halo pattern controls
*
* @copyright  Copyright (c) 2022 Bebop Arms. All rights reserved.
*
*
* @details    Controls for the LED ring (halo) around the target zone which serves as an annunciator for the target status.
*
* @link       TODO: Link to the article that describe your module in the
*                   WIKI.
**/


/************************************************************************/
/*                           Include section                            */
/************************************************************************/

#include <stdint.h>

#include "HaloPattern.h"
#include "LaserTarget.h"
#include "TLC5957.h"

/************************************************************************/
/*                         #define declarations                         */
/************************************************************************/

#define RGB_LED_COUNT 16

/************************************************************************/
/*                         Forward declarations                         */
/************************************************************************/
uint8_t Bezel_RGB = BEZEL_BLUE;

/************************************************************************/
/*                     Data structures declarations                     */
/************************************************************************/

/************************************************************************/
/*                         Classes declarations                         */
/************************************************************************/

/************************************************************************/
/*                         Routine declarations                         */
/************************************************************************/

/// <summary>
///  send data most significant bit first
/// </summary>
/// <param name="data"></param>
/// <param name="latchBytes">number of end bits to hold latch high for</param>
void send16bits(uint16_t data, uint8_t latchBits=0)
{
    for (int i = 16; i-- > 0;)
    {
        if (0 > latchBits)
        {
            if (i < latchBits)
            {
                // set latch data
                HALO_DATA_OUT |= HALO_LATCH_LED;
            }
        }
        bool bitSet = (data & (1 << i)) >> (i);
        if (bitSet)
        {
            // Set HALO_DATA_LED signal HIGH
            HALO_DATA_OUT |= HALO_DATA_LED;
        }
        else
        {
            // Set HALO_DATA_LED signal LOW
            HALO_DATA_OUT &= ~HALO_DATA_LED;
        }

        //output clock high
        HALO_DATA_OUT |= HALO_CLK_SIG;
        //output clock low
        HALO_DATA_OUT &= ~HALO_CLK_SIG;
    }


    // latch data
    HALO_DATA_OUT &= ~HALO_LATCH_LED;
}


// Draws a frame for the pattern
// @param currentFrame: The current animation frame. Zero indexed. (First frame is frame 0)
// @return bool: true if the pattern has a next frame.
bool RedCCW(uint16_t currentFrame)
{

    uint8_t latch_count = 0;
    bool retval=true;
    if (currentFrame >= RGB_LED_COUNT)
    {
        currentFrame = currentFrame % RGB_LED_COUNT;
        retval=false;
    }
    // Turn off halo
    P6OUT &= ~BIT0;
    //HALO_DATA_OUT |= (1 << HALO_OUTPUT_EN);

    // for each RGB LED, output a 1 for the active frame red LED and 0 for all others
    // LED data should be sent Blue, green, red.


    // halo position loop
    for (uint8_t k = RGB_LED_COUNT; k-- > 0;)
    {
        // RGB loop
        //The sending sequence is from MSB to LSB, from Blue color to Green color, and finally, Red color.
        for (uint8_t j = 3; j-- > 0;)
        {
            // last color of the RGB cluster?
            if (0 == j)
            {
                if (0 != k)
                {
                    // WRTGS need 1 clock of latch to write data to GS register
                    latch_count = 1;
                }
                else if (0 == k)
                {
                    // LATGS need 3 clock of latch to write data to GS register
                    latch_count = 3;
                }
            }
            else
            {
                latch_count = 0;
            }
            

            if (k == currentFrame && 0 == j)
            {
                send16bits(0xFFFF, latch_count);
            }
            else
            {
                send16bits(0x0000, latch_count);
            }

        }

    }


    // turn on halo
    P6OUT |= BIT0;
    return retval;
}

void InitLEDController()
{
    // SIN data doesnt matter. Set it high just cuz;
    HALO_DATA_OUT |= HALO_DATA_LED;

    // Unlock FC register
    // 15 SCLK rising edge must be input while LAT is high.
    HALO_DATA_OUT |= HALO_LATCH_LED;
    for (int i = 0; i < 15; i++)
    {
        //output clock high
        HALO_DATA_OUT |= HALO_CLK_SIG;
        //output clock low
        HALO_DATA_OUT &= ~HALO_CLK_SIG;
    }
    HALO_DATA_OUT &= ~HALO_LATCH_LED;


    // The TI chip requires atleast 80ns before we can write the data.
    // So we will take the time to create the FC Register buffers here.
    FCRegister fc_register;
    // We don't need anything fancy. The defaults will do, for the most part.
    fc_register.SetBits_default();
    // Tune color settings for our hardware. 
    // NOTE: These numbers are magic as hell.
    fc_register.SetBit_CCB(102);
    fc_register.SetBit_CCG(204);
    // hardware limit for Max current is based on Red's current requirement, so set Red limiter to max
    fc_register.SetBit_CCR(0xFFFF);

    send16bits(fc_register.Register_high);
    send16bits(fc_register.Register_mid);
    // 5 SCLK rising edge must be input while LAT is high.
    // The 48 - bit of common shift register is copied to FC register at the falling edge of LAT
    send16bits(fc_register.Register_low, 5);

}


/// <summary>
///  pwm on the msp430FR5739
/// </summary>
void PWM_Example()
{
    // alpha 0-7 : 7 == 100%
    uint8_t Bezel_Alpha = 3;
    uint8_t Bezel_RGB = BEZEL_BLUE;
    Bezel_Alpha *= PWM_30_KHZ;

    // reset pwm timers
    TB0CCTL1 = 0;
    TB0CCTL2 = 0;
    if (Bezel_Alpha > 0)
    {
        // alpha value >0, so we need to turn the lights on
        TB0CCR1 = PWM_TRIGGER;
        TB0CCR2 = TB0CCR1 + Bezel_Alpha;
        if (Bezel_Alpha < (PWM_TRIGGER))
        {
            // enable pwm on and off timers
            TB0CCTL1 = CCIE;
            TB0CCTL2 = CCIE;
        }
        else
        {
            // bezel alpha is max pwm, so we never need to shut it off
            // so just enable the 'pwm on' timer
            TB0CCTL1 = CCIE;

            // clear output values
            BEZEL_OUT &= ~((BEZEL_RED | BEZEL_GREEN | BEZEL_BLUE) ^ Bezel_RGB);
        }
    }
    else
    {
        // clear output values
        BEZEL_OUT &= ~(BEZEL_RED | BEZEL_GREEN | BEZEL_BLUE);
    }
}

/************************************************************************/
/*                     PWM_Example Implementation (INTERRUPTS)           */
/************************************************************************/
// TimerB Interrupt Vector (TBIV) handler
// Used for the Pattern Frame Rate
// When this "ticks", the next frame of the light pattern will be drawn
//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=TIMER0_B1_VECTOR
//__interrupt void TIMER0_B1_ISR(void)
//#elif defined(__GNUC__)
//void __attribute__((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR(void)
//#else
//#error Compiler not supported!
//#endif
//{
//    switch (__even_in_range(TB0IV, 14))
//    {
//    case  0: break;                          // No interrupt
//    case  2:
//        // CCR1
//
//        //DEBUG_3 ^= DEBUG_3_A;
//        //DEBUG_3 ^= DEBUG_3_A;
//        TB0CCR1 += PWM_TRIGGER;
//        // turn on bezel outputs
//        BEZEL_OUT |= Bezel_RGB;
//        break;
//    case  4:
//        // CCR2
//        /*
//        DEBUG_3 ^= DEBUG_3_B;
//        DEBUG_3 ^= DEBUG_3_B;*/
//        TB0CCR2 += PWM_TRIGGER;
//
//        // turn off bezel outputs
//        BEZEL_OUT &= ~(BEZEL_RED | BEZEL_GREEN | BEZEL_BLUE);
//
//
//        break;
//
//    default: break;
//    }
//}
