/** example_pwm app:
 *
 * This app shows how to generate two PWM signals with the Wixel on pins P1_3
 * and P1_4 using Timer 3.
 *
 * To understand how this app works, you will need to read Section 12 of the
 * CC2511 datasheet, which documents Timer 3 and Timer 4.
 *
 * We do not currently have a general-purpose PWM library, so this example
 * directly accesses the timer registers in order to set up PWM.
 * The servo library is not appropriate for most PWM applications because it
 * is designed for RC servos and cannot produce a full range of duty cycles.
 *
 * The PWM frequency used by this app is 23.4 kHz, which works well for most
 * DC motor control applications because it is super-sonic.
 * If you need a different frequency, you can change the prescaler configuration
 * bits in the T3CTL register to achieve frequencies as high as 93.8 kHz and as
 * low as 0.73 kHz.
 *
 * If you need three or four PWM outputs, you can adapt this code to use both
 * Timer 3 and Timer 4.
 *
 * If you need PWM outputs on different pins, you can change the code to use
 * the other location for Timer 3 or you can change it to use Timer 4.
 */

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>

void timer3Init()
{
    // Start the timer in free-running mode with a prescaler of 1:4.
    // The global tick frequency is 24 MHz and the timer will overflow after
    // counting to 255, so this results in a Timer 3 PWM frequency of
    // (24000 kHz)/4/256 = 23.4 kHz.
    T3CTL = 0b01010000;

    // Set the duty cycles to zero.
    T3CC0 = T3CC1 = 0;

    // Enable PWM on both channels.  We choose the mode where the channel
    // goes high when the timer is at 0 and goes low when the timer value
    // is equal to T3CCn.
    T3CCTL0 = T3CCTL1 = 0b00100100;

    // Configure Timer 1 to use Alternative 1 location, which is the default.
    PERCFG &= ~(1<<6);  // PERCFG.T1CFG = 0;

    // Configure P1_3 and P1_4 to be controlled by a peripheral function (Timer 3)
    // instead of being general purpose I/O.
    P1SEL |= (1<<3) | (1<<4);

    // After running this function, you can set the duty cycles by simply writing
    // to T3CC0 and T3CC1.  A value of 255 results in a 100% duty cycle, and a
    // value of N < 255 results in a duty cycle of N/256.
}

void updatePwm()
{
    uint16 x;

    // Set the duty cycle on channel 0 (P1_3) to 210/256 (82.0%).
    T3CC0 = 210;

    // Make the duty cycle on channel 1 (P1_4) vary smoothly up and down.
    x = getMs() >> 3;
    T3CC1 = (x >> 8 & 1) ? ~x : x;
}

void main()
{
    systemInit();
    usbInit();
    timer3Init();

    while(1)
    {
        boardService();
        usbShowStatusWithGreenLed();
        updatePwm();
        usbComService();
    }
}