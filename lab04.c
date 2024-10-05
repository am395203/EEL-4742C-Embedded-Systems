//LAB 4 
//SECTION 4.1 TIMER'S CONTINUOUS MODE WITH INTERRUPT
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

// Timer_A continuous mode, with interrupt, flashes LEDs
void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Timer_A configuration (fill the line below)
    // Use ACLK, divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero event
    TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR | TAIE;

    // Ensure the flag is cleared at the start
    TA0CTL &= ~TAIFG;

    // Enable the global interrupt bit (call an intrinsic function)
    _enable_interrupts();

    // Infinite loop... the code waits here between interrupts
    for(;;) {}
}

//******* Writing the ISR *******
#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
__interrupt void T0A1_ISR() {
    // Toggle both LEDs
    P1OUT ^= redLED;
    P9OUT ^= greenLED;

    // Clear the TAIFG flag
    TA0CTL &= ~TAIFG;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//SECTION 4.2 TIMER'S UP MODE WITH INTERRUPT
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

// Timer_A up mode, with interrupt, flashes LEDs
void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT |= greenLED; // Turn LED On (alternate flashing)

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Configure Channel 0 for up mode with interrupt
    TA0CCR0 = 32767; // Fill to get 1 second @ 32 KHz (32768 - 1)
    TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
    TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

    // Timer_A: ACLK, div by 1, up mode, clear TAR (leaves TAIE=0)
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

    // Enable the global interrupt bit (call an intrinsic function)
    _enable_interrupts();

    // Infinite loop... the code waits here between interrupts
    for(;;) {}
}


//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    // Toggle both LEDs
    P1OUT ^= redLED;
    P9OUT ^= greenLED;
    // Hardware clears the flag (CCIFG in TA0CCTL0)

}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//SECTION 4.3 Push Button with Interrupt
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at Port 1.1
#define BUT2 BIT2 // Button S2 at Port 1.2

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configuring buttons with interrupt
    P1DIR &= ~(BUT1|BUT2); // 0: input
    P1REN |= (BUT1|BUT2); // 1: enable built-in resistors
    P1OUT |= (BUT1|BUT2); // 1: built-in resistor is pulled up to Vcc
    P1IE |= (BUT1|BUT2);// 1: enable interrupts
    P1IES |= (BUT1|BUT2); // 1: interrupt on falling edge
    P1IFG &= ~(BUT1|BUT2); // 0: clear the interrupt flags

    _enable_interrupts();

    // Infinite loop... the code waits here between interrupts
    for(;;) {}
}


//******* Writing the ISR *******
#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    // Detect button 1 (BUT1 in P1IFG is 1)
    if ( (P1IFG & BUT1) != 0 ) {
        // Toggle the red LED
        P1OUT ^= redLED;
        // Clear BUT1 in P1IFG
        P1IFG &= ~BUT1;
    }

    // Detect button 2 (BUT2 in P1IFG is 1)
    if ( (P1IFG & BUT2) != 0) {
        // Toggle the green LED
        P9OUT ^= greenLED;
        // Clear BUT2 in P1IFG
        P1IFG &= ~BUT2;
    }
}








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SECTION 4.4 Low-Power Modes (CONTINUOUS MODE)
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

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins
    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Timer_A configuration (fill the line below)
    // Use ACLK, divide by 1, continuous mode, TAR cleared, enable interrupt for rollback-to-zero event
    TA0CTL = TASSEL_1 | ID_0 | MC_2 | TACLR | TAIE;

    // Ensure the flag is cleared at the start
    TA0CTL &= ~TAIFG;

    //Using LPM3 to keep ACLK on as it is needed
    _low_power_mode_3();
}

//******* Writing the ISR *******
#pragma vector = TIMER0_A1_VECTOR // Link the ISR to the vector
__interrupt void T0A1_ISR() {
    // Toggle both LEDs
    P1OUT ^= redLED;
    P9OUT ^= greenLED;

    // Clear the TAIFG flag
    TA0CTL &= ~TAIFG;
}






//SECTION 4.4 Low-Power Modes (UP MODE)
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


// Timer_A up mode, with interrupt, flashes LEDs
void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT |= greenLED; // Turn LED On (alternate flashing)

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Configure Channel 0 for up mode with interrupt
    TA0CCR0 = 32767; // Fill to get 1 second @ 32 KHz (32768 - 1)
    TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
    TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit


    // Timer_A: ACLK, div by 1, up mode, clear TAR (leaves TAIE=0)
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;

    //Using LPM3 to keep ACLK on as it is needed
    _low_power_mode_3();
}


//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    // Toggle both LEDs
    P1OUT ^= redLED;
    P9OUT ^= greenLED;
    // Hardware clears the flag (CCIFG in TA0CCTL0)

}






//SECTION 4.4 Low-Power Modes (BUTTONS)
#include <msp430fr6989.h>
#define redLED BIT0 // Red LED at P1.0
#define greenLED BIT7 // Green LED at P9.7
#define BUT1 BIT1 // Button S1 at Port 1.1
#define BUT2 BIT2 // Button S2 at Port 1.2

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Enable the GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P9DIR |= greenLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off
    P9OUT &= ~greenLED; // Turn LED Off

    // Configuring buttons with interrupt
    P1DIR &= ~(BUT1|BUT2); // 0: input
    P1REN |= (BUT1|BUT2); // 1: enable built-in resistors
    P1OUT |= (BUT1|BUT2); // 1: built-in resistor is pulled up to Vcc
    P1IE |= (BUT1|BUT2);// 1: enable interrupts
    P1IES |= (BUT1|BUT2); // 1: interrupt on falling edge
    P1IFG &= ~(BUT1|BUT2); // 0: clear the interrupt flags

    //Using LPM4
    _low_power_mode_4();
}


//******* Writing the ISR *******
#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    // Detect button 1 (BUT1 in P1IFG is 1)
    if ( (P1IFG & BUT1) != 0 ) {
        // Toggle the red LED
        P1OUT ^= redLED;
        // Clear BUT1 in P1IFG
        P1IFG &= ~BUT1;
    }

    // Detect button 2 (BUT2 in P1IFG is 1)
    if ( (P1IFG & BUT2) != 0) {
        // Toggle the green LED
        P9OUT ^= greenLED;
        // Clear BUT2 in P1IFG
        P1IFG &= ~BUT2;
    }
}







