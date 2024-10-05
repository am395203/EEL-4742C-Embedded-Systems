//LAB 03 CODES

//SECTION 3.1 The Continuous Mode
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
    } while((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

// Flashing the LED with Timer_A, continuous mode, via polling
void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal (function call)
    config_ACLK_to_32KHz_crystal();

    // Configure Timer_A
    // Use ACLK, divide by 1, continuous mode, clear TAR
    TA0CTL = TASSEL_1 | MC_2 | TACLR;

    // Ensure flag is cleared at the start
    TA0CTL &= ~TAIFG;

    // Infinite loop
    for(;;) {
        // Empty while loop; waits here until TAIFG is raised
        while((TA0CTL&TAIFG) == 0) {}
        P1OUT ^= redLED; // Toggle the red LED
        TA0CTL &= ~TAIFG; // Clear the flag
    }
}







//SECTION 3.2 The Up Mode
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
    } while((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

// Flashing the LED with Timer_A, up mode, via polling
void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal (function call)
    config_ACLK_to_32KHz_crystal();

    // Set timer period
    TA0CCR0 = 32767; // cycles = Time * frequency = 1 * 2^16 = 32768, includes 0, so subtract 1 = 32767

    // Timer_A: ACLK, div by 1, up mode, clear TAR
    TA0CTL = TASSEL_1 | MC_1 | TACLR;

    // Ensure flag is cleared at the start
    TA0CTL &= ~TAIFG;

    // Infinite loop
    for(;;) {
        // Empty while loop; waits here until TAIFG is raised
        while((TA0CTL&TAIFG) == 0) {}
        P1OUT ^= redLED; // Toggle the red LED
        TA0CTL &= ~TAIFG; // Clear the flag
    }
}










//SECTION 3.3 Your Own Design
// continuous mode design ----> flashes red, both, then green
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
    } while((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

// Flashing the LED with Timer_A, continuous mode, via polling
void main(void) {
    volatile unsigned int i=0;
    volatile unsigned int innerFlag=0;
    volatile unsigned int outerFlag = 0;
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal (function call)
    config_ACLK_to_32KHz_crystal();

    // Configure Timer_A
    // Use ACLK, divide by 1, continuous mode, clear TAR
    TA0CTL = TASSEL_1 | MC_2 | TACLR;

    // Ensure flag is cleared at the start
    TA0CTL &= ~TAIFG;

    // Infinite loop
    for(;;) {

        if((outerFlag % 2) == 0) //flag used to choose between flashing one or both LEDs, flag == 0 will flash one LED
        {
            while((TA0CTL&TAIFG) == 0 && (innerFlag % 2 ==0)) //flag j is used to select which LED to flash, j == 0 will flash the red LED
            {
                for(i = 0; i < 20000; i++){} //delay of 20000 will continue as long as the flag has not been raised
                P1OUT ^= redLED; // Toggle the red LED
            }

            while((TA0CTL&TAIFG) == 0 && (innerFlag % 2 == 1)) //flag j == 1 will flash the green LED
            {
                for(i = 0; i < 20000; i++){} //delay of 20000 will continue as long as the flag has not been raised
                P9OUT ^= greenLED; // Toggle the green LED
            }
            P1OUT &= ~redLED; //keep LEDs off otherwise
            P9OUT &= ~greenLED;
            outerFlag++; //increase flag by 1
            innerFlag++; //increase flag j by 1
            TA0CTL &= ~TAIFG; // Clear the timer flag
        }

        if((outerFlag % 2) == 1) //if flag == 1 then flash both LEDS
        {
            while((TA0CTL&TAIFG) == 0) //as long as the flag is not raised
            {
                for(i = 0; i < 20000; i++){} //will flash both LEDS at a delay of 20000 while flag has not been raised
                P1OUT ^= redLED; //toggle red LEG
                P9OUT ^= greenLED; //toggle green LED
            }
            P1OUT &= ~redLED; //keep LEDs off otherwise
            P9OUT &= ~greenLED;
            outerFlag++; //increase flag by 1
            TA0CTL &= ~TAIFG; // Clear the timer flag

        }

    }
}







//SECTION 3.3 Your Own Design
// UP mode design ----> frequency increases and flashes both LEDs
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7

//**********************************
// Configures ACLK to 32 KHz crystal
void config_ACLK_to_32KHz_crystal() {
    // By default, ACLK runs on LFMODCLK at 5MHz/128 = 39 KHz
    // Reroute pins to LFXIN/LFXOUT functionality
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;

    // Wait until the oscillator fault flags remain cleared
    CSCTL0 = CSKEY; // Unlock CS registers
    do {
        CSCTL5 &= ~LFXTOFFG; // Local fault flag
        SFRIFG1 &= ~OFIFG; // Global fault flag
    } while((CSCTL5 & LFXTOFFG) != 0);
    CSCTL0_H = 0; // Lock CS registers
    return;
}

// Flashing the LED with Timer_A, up mode, via polling
void main(void) {
    volatile unsigned int i;
    volatile unsigned int flag = 0;
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal (function call)
    config_ACLK_to_32KHz_crystal();

    // Set timer period
    TA0CCR0 = 2000; //want 2000 cycles -> 2001 - 1 = 2000

    // Timer_A: ACLK, div by 1, up mode, clear TAR
    TA0CTL = TASSEL_1 | MC_1 | TACLR;

    // Ensure flag is cleared at the start
    TA0CTL &= ~TAIFG;

    // Infinite loop
    for(;;) {
        for(TA0CCR0 = 2000; TA0CCR0 < 16000; TA0CCR0+=500) //goes from 2000 - 16000 and increments by +500
        {
            while(((TA0CTL&TAIFG) == 0) && (flag %2 == 0)) // while flag has not been raised, if int flag is 0 then flash the red LED
            {
                P1OUT ^= redLED; // Toggle the red LED
            }

            while(((TA0CTL&TAIFG) == 0) && (flag %2 == 1)) // while flag has not been raised, if int flag is 1 then flash the green LED
            {
                P9OUT ^= greenLED; // Toggle the green LED
            }
            P1OUT &= ~redLED; //keep LEDs off otherwise
            P9OUT &= ~greenLED;

            flag++; //increase flag by 1
            TA0CTL &= ~TAIFG; // Clear the flag
        }

        for(TA0CCR0 = 16000; TA0CCR0 > 2000; TA0CCR0-=500) //goes from 16000 - 2000-1 and decrements by -500
        {
            while(((TA0CTL&TAIFG) == 0) && (flag %2 == 0)) // while flag has not been raised, if int flag is 0 then flash the red LED
            {
            P1OUT ^= redLED; // Toggle the red LED
            }

            while(((TA0CTL&TAIFG) == 0) && (flag %2 == 1)) // while flag has not been raised, if int flag is 1 then flash the green LED
            {
                P9OUT ^= greenLED; // Toggle the green LED
            }
            P1OUT &= ~redLED; //keep LEDs off otherwise
            P9OUT &= ~greenLED;

            flag++; //increase flag by 1
            TA0CTL &= ~TAIFG; // Clear the flag
        }

    }
}








