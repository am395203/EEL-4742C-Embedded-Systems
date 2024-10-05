////LAB 07//////////////////////////////////////////////////////////////

#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define BUT1 BIT1 // Button S1 at Port 1.1

void config_ACLK_to_32KHz_crystal();

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    //Configure red LED
    P1DIR |= redLED;
    P1OUT &= ~redLED;

    // Configuring button with interrupt
    P1DIR &= ~BUT1; // 0: input
    P1REN |= BUT1; // 1: enable built-in resistors
    P1OUT |= BUT1; // 1: built-in resistor is pulled up to Vcc
    P1IE |= BUT1;// 1: enable interrupts
    P1IES |= BUT1; // 1: interrupt on falling edge
    P1IFG &= ~BUT1; // 0: clear the interrupt flags

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Engage low-power mode
    _low_power_mode_3();

    return;
}

#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    if((P1IFG & BUT1) != 0){
        P1IFG &= ~BUT1; //Clear the flag
        P1IE &= ~BUT1; //Disable button interrupt

        P1OUT ^= redLED; //toggle the red LED

        // Configure Channel 1
        TA0CCR1 = 24576 - 1; // @ 8 KHz --> 3 seconds (32 KHz with divider ID_2(4))
        TA0CCTL1 |= CCIE; //enable interrupt
        TA0CCTL1 &= ~CCIFG; //clear flag

        // Configure timer (ACLK) (divide by 4) (continuous mode)
        TA0CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR() {
    P1OUT ^= redLED; //turn the red LED off after 3 seconds
    P1IE |= BUT1; //reenable the button's interrupt
    P1IFG &= ~BUT1; //Clear the buttons flag
    TA0CCTL1 &= ~CCIFG; //clear the timers flag
    TA0CCTL1 &= ~CCIE; //disable the timer's interrupt

}


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




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define BUT1 BIT1 // Button S1 at Port 1.1

void config_ACLK_to_32KHz_crystal();

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    //Configure red LED
    P1DIR |= redLED;
    P1OUT &= ~redLED;

    // Configuring button with interrupt
    P1DIR &= ~BUT1; // 0: input
    P1REN |= BUT1; // 1: enable built-in resistors
    P1OUT |= BUT1; // 1: built-in resistor is pulled up to Vcc
    P1IE |= BUT1;// 1: enable interrupts
    P1IES |= BUT1; // 1: interrupt on falling edge
    P1IFG &= ~BUT1; // 0: clear the interrupt flags

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Engage low-power mode
    _low_power_mode_3();

    return;
}

#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    if((P1IFG & BUT1) != 0){
        P1IFG &= ~BUT1; //Clear the flag
        P1OUT |= redLED; //toggle the red LED
        //wont disable the button interrupt, in order to reset the timer and increase by 3 seconds each time it is pushed

        // Configure Channel 1
        TA0CCR1 = 12288 - 1; // @ 8 KHz --> 3 seconds (32 KHz with divider ID_3(8))
        TA0CCTL1 |= CCIE; //enable interrupt
        TA0CCTL1 &= ~CCIFG; //clear flag

        // Configure timer (ACLK) (divide by 4) (continuous mode)
        TA0CTL = TASSEL_1 | ID_3 | MC_2 | TACLR;
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR() {
    P1OUT ^= redLED; //turn the red LED off after 3 seconds
    P1IE |= BUT1; //reenable the button's interrupt
    P1IFG &= ~BUT1; //Clear the flag
	TA0CCTL1 &= ~CCIFG; //clear the flag
    TA0CCTL1 &= ~CCIE; //disable the timer's interrupt

}


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







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define BUT1 BIT1 // Button S1 at Port 1.1

void config_ACLK_to_32KHz_crystal();

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    //Configure red LED
    P1DIR |= redLED;
    P1OUT &= ~redLED;

    // Configuring button with interrupt
    P1DIR &= ~BUT1; // 0: input
    P1REN |= BUT1; // 1: enable built-in resistors
    P1OUT |= BUT1; // 1: built-in resistor is pulled up to Vcc
    P1IE |= BUT1;// 1: enable interrupts
    P1IES |= BUT1; // 1: interrupt on falling edge
    P1IFG &= ~BUT1; // 0: clear the interrupt flags

    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Engage low-power mode
    _low_power_mode_3();

    return;
}

#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    if((P1IFG & BUT1) != 0){
        P1IFG &= ~BUT1; //Clear the flag
        // Configure Channel 1
        TA0CCR1 =  983 - 1; // @ 8 KHz --> 3 seconds (32 KHz with divider ID_3(8))
        TA0CCTL1 |= CCIE; //enable interrupt
        TA0CCTL1 &= ~CCIFG; //clear flag

        // Configure timer (ACLK) (divide by 4) (continuous mode)
        TA0CTL = TASSEL_1 | ID_2 | MC_2 | TACLR;
    }
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void T0A1_ISR() {
    while((P1IFG & BUT1) == 0){}

    TA0CCTL1 &= ~CCIFG; //clear the flag
    P1OUT ^= redLED;
    P1IFG &= ~BUT1; //Clear the flag
    TA0CCTL1 &= ~CCIE; //disable the timer's interrupt

}


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







//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////