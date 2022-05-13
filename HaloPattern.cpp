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
void send9bitByte(uint16_t data)
{

    for (int i = 9; i-- > 0;)
    {
        bool bitSet = (data & (1 << i)) >> (i);
        if (bitSet)
        {
            // Set HALO_DATA_LED signal HIGH
            HALO_DATA_OUT |= 1 << HALO_DATA_LED;
        }
        else
        {
            // Set HALO_DATA_LED signal LOW
            HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
        }

        //output clock high
        HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
        //output clock low
        HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    }
}


// Draws a frame for the pattern
// @param currentFrame: The current animation frame. Zero indexed. (First frame is frame 0)
// @return bool: true if the pattern has a next frame.
bool RedCCW(uint16_t currentFrame)
{
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

    // frame loop
    for (uint8_t i = RGB_LED_COUNT; i-- > 0;)
    {

        // RGB loop
        //The sending sequence is from MSB to LSB, from Blue color to Green color, and finally, Red color.
        for (uint8_t j = 3; j-- > 0;)
        {
            // halo position loop
            for (uint8_t k = RGB_LED_COUNT; k-- > 0;)
            {

                if (k == i && 0 == j)
                {
                    // Set HALO_DATA_LED signal HIGH
                    HALO_DATA_OUT |= 1 << HALO_DATA_LED;
                }
                else
                {
                    // Set HALO_DATA_LED signal LOW
                    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
                }

                // need 1 clock of latch to write data to GS register
                if (i!=0 && j == 0 && k == 0)
                {
                    HALO_DATA_OUT |= (1 << HALO_LATCH_LED);
                }
                // need 1 clock of latch to write data to GS register
                if (i==0 && j == 0 && k == 2)
                {
                    HALO_DATA_OUT |= (1 << HALO_LATCH_LED);
                }


                //output clock high
                HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
                //output clock low
                HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
            }

            // latch data
            HALO_DATA_OUT &= ~(1 << HALO_LATCH_LED);
        }
    }

    // latch data
    HALO_DATA_OUT &= ~(1 << HALO_LATCH_LED);

    // turn on halo
    P6OUT |= BIT0;
    return retval;
}

void InitLEDController()
{
    // SIN data doesnt matter. Set it high just cuz;
    HALO_DATA_OUT |= 1 << HALO_DATA_LED;

    // Unlock FC register
    // 15 SCLK rising edge must be input while LAT is high.
    HALO_DATA_OUT |= (1 << HALO_LATCH_LED);
    for (int i = 0; i < 15; i++)
    {
        //output clock high
        HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
        //output clock low
        HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    }
    HALO_DATA_OUT &= ~(1 << HALO_LATCH_LED);


    // add a few cpu cycles of delay as required by hardware.
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();

    // 47 – 45 LGSE2
    // 000b — no improvement
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    for (int i = 0; i < 15; i++)
    {
        //output clock high
        HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
        //output clock low
        HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    }

    // 44 Poker trans mode
    // Poker trans mode enable bit. (0 = disabled
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 43 – 41 BC
    // Global brightness control data for all output(Data = 0h - 7h) 
    // 1
    HALO_DATA_OUT &= (1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    // 00
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);



    // 40 – 32 CCR
    // 511d
    send9bitByte(511);

    // 31 – 23 CCG
    // 204d
    send9bitByte(204);

    // 22 – 14 CCB
    // 102d
    send9bitByte(102);

    // 13 – 11 LGSE1 -- Low Gray Scale Enhancement
    //000
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    
    // 10 SEL_SCK_EDGE
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);
    
    // 9 LGSE3
    // Compensation for Blue LED. (0 = disabled
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 8 ESPWM
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 7 SEL_PCHG 
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 6 SEL_GCK_EDGE 
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 5 XREFRESH
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 5 SCLK rising edge must be input while LAT is high.
    // The 48 - bit of common shift register is copied to FC register at the falling edge of LAT
    HALO_DATA_OUT |= (1 << HALO_LATCH_LED);


    // 4 SEL_GDLY
    // Group delay select.
    // When this bit is ‘0’, no delay between channels. All channels turn on at same time.
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 3 – 2 SEL_TD0
    // TD0 is the time from SCLK to SOUT, typical value is 21 ns. Once received, SCLK, with 21 - ns delay, SOUT begins change.(default value when power up)
    // 01
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    HALO_DATA_OUT &= (1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);


    // 1 – 0 LODVTH -- LOD detection threshold voltage.
    // 01
    HALO_DATA_OUT &= ~(1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    HALO_DATA_OUT &= (1 << HALO_DATA_LED);
    HALO_DATA_OUT |= (1 << HALO_CLK_SIG);
    HALO_DATA_OUT &= ~(1 << HALO_CLK_SIG);

    // 5 SCLK rising edge must be input while LAT is high.
    // The 48 - bit of common shift register is copied to FC register at the falling edge of LAT
    HALO_DATA_OUT &= ~(1 << HALO_LATCH_LED);


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
