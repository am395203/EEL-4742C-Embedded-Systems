//LAB02 CODES

//Section 2.1 Reading the Push Buttons
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= ~BUT1; // Direct pin as input
    P1REN |= BUT1; // Enable built-in resistor
    P1OUT |= BUT1; // Set resistor as pull-up

    // Polling the button in an infinite loop
    for(;;)
    {
        if ( (P1IN & BUT1) == 0)
		{
            P1OUT |= redLED; // Turn red LED on
        }
        else P1OUT &= ~redLED; // Turn red LED off
    }
}


//Section 2.2 Using Two Push Buttons
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1
#define BUT2 BIT2 // Button S2 at P1.2

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= (~BUT1 & ~BUT2); // Direct pin as input
    P1REN |= (BUT1|BUT2); // Enable built-in resistor
    P1OUT |= (BUT1|BUT2); // Set resistor as pull-up

    // Polling the button in an infinite loop
        for(;;)
        {
            if( (P1IN & BUT1) == 0)
            {
                P1OUT |= redLED; // Turn red LED on when button 1 is pushed
            }
            else P1OUT &= ~redLED; // Keep red LED off if button 1 not pushed

            if ( (P1IN & BUT2) == 0)
            {
                P9OUT |= greenLED; // Turn green LED on when button 2 is pushed
            }
            else P9OUT &= ~greenLED; // Keep green LED off if button 1 not pushed
        }
}



//Section 2.3 Using Two Buttons with Exclusive Access
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1
#define BUT2 BIT2 // Button S2 at P1.2

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= (~BUT1 & ~BUT2); // Direct pin as input
    P1REN |= (BUT1|BUT2); // Enable built-in resistor
    P1OUT |= (BUT1|BUT2); // Set resistor as pull-up

    // Polling the button in an infinite loop
        for(;;)
        {
            if(((P1IN & BUT1) == 0) && ((P1IN & BUT2) != 0)) //When button 1 is pushed and button 2 is not
            {
                P1OUT |= redLED; // Turn red LED on when button 1 is pushed

                while((P1IN & BUT1) == 0) //while button 1 is pushed, keep green LED off
                {
                    P9OUT &= ~greenLED; // keep green LED off
                }
            }

            if (((P1IN & BUT2) == 0) && ((P1IN & BUT1) != 0)) //When button 2 is pushed and button 1 is not
            {
                P9OUT |= greenLED; // Turn green LED on when button 2 is pushed

                while((P1IN & BUT2) == 0) //while button 2 is pushed, keep red LED off
                {
                    P1OUT &= ~redLED; // keep red LED off
                }
            }
            else //else keep both LEDS off
            {
                P1OUT &= ~redLED; // turn red LED off
                P9OUT &= ~greenLED; // turn green LED off
            }
        }
}





//Section 2.4 Your Own Design
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at P1.1
#define BUT2 BIT2

void main(void)
{
    volatile unsigned int i;
    volatile unsigned int j;
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    // Configure and initialize LEDs
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure buttons
    P1DIR &= (~BUT1 & ~BUT2); // Direct pin as input
    P1REN |= (BUT1|BUT2); // Enable built-in resistor
    P1OUT |= (BUT1|BUT2); // Set resistor as pull-up

    // Polling the button in an infinite loop
        for(;;)
        {
            if(((P1IN & BUT1) == 0) && ((P1IN & BUT2) != 0)) //When button 1 is pushed and button 2 is not
            {
                P1OUT |= redLED; // Turn red LED on when button 1 is pushed

                while((P1IN & BUT1) == 0) //while button 1 is pushed, keep green LED off
                {
                    P9OUT &= ~greenLED; // keep green LED off

                    for(i = 0; i < 10000; i++){} //while button 1 is held
                    P1OUT ^= redLED; //flash the red LED
                }
            }

            if (((P1IN & BUT2) == 0) && ((P1IN & BUT1) != 0)) //When button 2 is pushed and button 1 is not
            {
                P9OUT |= greenLED; // Turn green LED on when button 2 is pushed

                while((P1IN & BUT2) == 0) //while button 2 is pushed, keep red LED off
                {
                    P1OUT &= ~redLED; // keep red LED off
                    for(j = 0; j < 30000; j++){} //while button 2 is held
                    P9OUT ^= greenLED; //flash the green LED
                }
            }
            else //else keep both LEDS off
            {
                P1OUT &= ~redLED; // turn red LED off
                P9OUT &= ~greenLED; // turn green LED off
            }
        }
}







