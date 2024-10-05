//LAB 5

//5.1 Printing Numbers on the LCD Display
#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define greenLED BIT7 // Green at P9.7
void Initialize_LCD();

// The array has the shapes of the digits (0 to 9)
// Complete this array...
const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0XE0, 0XFF, 0XF7};
void display_num_lcd(unsigned int n);

int main(void) {
    volatile unsigned int n;
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT |= redLED; // Red on
    P9OUT &= ~greenLED; // Green off

    // Initializes the LCD_C module
    Initialize_LCD();

    // The line below can be used to clear all the segments
    //LCDCMEMCTL = LCDCLRM; // Clears all the segments

    // Display 430 on the rightmost three digits
    display_num_lcd(65535);

    // Flash the red and green LEDs
    for(;;) {
        for(n=0; n<=50000; n++) {} // Delay loop
        P1OUT ^= redLED;
        P9OUT ^= greenLED;
    }

    return 0;
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD() {
PJSEL0 = BIT4 | BIT5; // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT
    do {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM; // Clear LCD memory

    //Turn LCD on
    LCDCCTL0 |= LCDON;

    return;
}

void display_num_lcd(unsigned int n){
    LCDM6 = LCD_Num[((n / 10000) % 10)];    //5th digit, use num / 10,000 % 10 to extract number
    LCDM4 = LCD_Num[((n / 1000) % 10)];    //4th digit, use num / 1,000 % 10 to extract number
    LCDM19 = LCD_Num[((n / 100) % 10)];    //3rd digit, use num / 100 % 10 to extract number
    LCDM15 = LCD_Num[((n / 10) % 10)];     //2nd digit, use num / 10 % 10 to extract number
    LCDM8 = LCD_Num[(n % 10)];             //1st digit, use num % 10 to extract number

    if(LCDM6 == 0xFC) //if 0 then don't need to display, checking for 5th digit
    {//Use nested if statements for checking if a leading 0, don't want to erase a zero if it a required 0 like in 506
        LCDM6 = 0; //turn LED off
        if(LCDM4 == 0xFC) //checking 4th digit
        {
            LCDM4 = 0; //turn LED off
            if(LCDM19 == 0xFC) //checking 3rd digit
            {
                LCDM19 = 0; //turn LED off
                if(LCDM15 == 0xFC) //checking 2nd digit, wont check 1st digit in case we wish to display 0
                {
                    LCDM15 = 0; //turn LED off
                }
            }
        }
    }
}








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//5.2 Implementing a Stopwatch
// Sample code that prints 430 on the LCD monitor
#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define greenLED BIT7 // Green at P9.7
void Initialize_LCD();
void config_ACLK_to_32KHz_crystal();
volatile unsigned int num;

// The array has the shapes of the digits (0 to 9)
// Complete this array...
const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0XE0, 0XFF, 0XF7};
void display_num_lcd(unsigned int n);

int main(void) {
    volatile unsigned int n;
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT |= redLED; // Red on
    P9OUT &= ~greenLED; // Green off


    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Configure Channel 0 for up mode with interrupt
    TA0CCR0 = 32767; // Fill to get 1 second @ 32 KHz (32768 - 1)
    TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
    TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

    // Timer_A: ACLK, div by 1, up mode, clear TAR (leaves TAIE=0)
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
    TA0CTL &= ~TAIFG;

    // Initializes the LCD_C module
    Initialize_LCD();

    // The line below can be used to clear all the segments
    //LCDCMEMCTL = LCDCLRM; // Clears all the segments

    // Enable the global interrupt bit (call an intrinsic function)
       _enable_interrupts();

    num = 65530;
    // Infinite loop... the code waits here between interrupts
    for(;;) {
        for(n=0; n<=50000; n++) {} // Delay loop
        P1OUT ^= redLED;
        P9OUT ^= greenLED;

    }

    return 0;
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD() {
PJSEL0 = BIT4 | BIT5; // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT
    do {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM; // Clear LCD memory

    //Turn LCD on
    LCDCCTL0 |= LCDON;

    return;
}

void display_num_lcd(unsigned int n){
    LCDM6 = LCD_Num[((n / 10000) % 10)];    //5th digit, use num / 10,000 % 10 to extract number
    LCDM4 = LCD_Num[((n / 1000) % 10)];    //4th digit, use num / 1,000 % 10 to extract number
    LCDM19 = LCD_Num[((n / 100) % 10)];    //3rd digit, use num / 100 % 10 to extract number
    LCDM15 = LCD_Num[((n / 10) % 10)];     //2nd digit, use num / 10 % 10 to extract number
    LCDM8 = LCD_Num[(n % 10)];             //1st digit, use num % 10 to extract number

    if(LCDM6 == 0xFC) //if 0 then don't need to display, checking for 5th digit
    {//Use nested if statements for checking if a leading 0, don't want to erase a zero if it a required 0 like in 506
        LCDM6 = 0; //turn LED off
        if(LCDM4 == 0xFC) //checking 4th digit
        {
            LCDM4 = 0; //turn LED off
            if(LCDM19 == 0xFC) //checking 3rd digit
            {
                LCDM19 = 0; //turn LED off
                if(LCDM15 == 0xFC) //checking 2nd digit, wont check 1st digit in case we wish to display 0
                {
                    LCDM15 = 0; //turn LED off
                }
            }
        }
    }
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

//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    display_num_lcd(num);
    // Hardware clears the flag (CCIFG in TA0CCTL0)
    num += 1;
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//5.3 Stopwatch with Halt/Resume and Reset Functions
// Sample code that prints 430 on the LCD monitor
#include <msp430fr6989.h>
#define redLED BIT0 // Red at P1.0
#define greenLED BIT7 // Green at P9.7
#define BUT1 BIT1 // Button S1 at Port 1.1
#define BUT2 BIT2 // Button S2 at Port 1.2
void Initialize_LCD();
void config_ACLK_to_32KHz_crystal();
volatile unsigned int num;

// The array has the shapes of the digits (0 to 9)
// Complete this array...
const unsigned char LCD_Num[10] = {0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0XE0, 0XFF, 0XF7};
void display_num_lcd(unsigned int n);

int main(void) {
    volatile unsigned int n;
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red on
    P9OUT |= greenLED; // Green off

    // Configuring buttons with interrupt
    P1DIR &= ~(BUT1|BUT2); // 0: input
    P1REN |= (BUT1|BUT2); // 1: enable built-in resistors
    P1OUT |= (BUT1|BUT2); // 1: built-in resistor is pulled up to Vcc
    P1IE |= (BUT1|BUT2);// 1: enable interrupts
    P1IES |= (BUT1|BUT2); // 1: interrupt on falling edge
    P1IFG &= ~(BUT1|BUT2); // 0: clear the interrupt flags


    // Configure ACLK to the 32 KHz crystal
    config_ACLK_to_32KHz_crystal();

    // Configure Channel 0 for up mode with interrupt
    TA0CCR0 = 32767; // Fill to get 1 second @ 32 KHz (32768 - 1)
    TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
    TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

    // Timer_A: ACLK, div by 1, up mode, clear TAR (leaves TAIE=0)
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
    TA0CTL &= ~TAIFG;

    // Initializes the LCD_C module
    Initialize_LCD();

    // The line below can be used to clear all the segments
    //LCDCMEMCTL = LCDCLRM; // Clears all the segments

    // Enable the global interrupt bit (call an intrinsic function)
       _enable_interrupts();

    num = 0;
    // Infinite loop... the code waits here between interrupts
    for(;;) {}

    return 0;
}

//**********************************************************
// Initializes the LCD_C module
// *** Source: Function obtained from MSP430FR6989’s Sample Code ***
void Initialize_LCD() {
PJSEL0 = BIT4 | BIT5; // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8; // Unlock CS registers
    CSCTL4 &= ~LFXTOFF; // Enable LFXT
    do {
        CSCTL5 &= ~LFXTOFFG; // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG); // Test oscillator fault flag
    CSCTL0_H = 0; // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC; // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM; // Clear LCD memory

    //Turn LCD on
    LCDCCTL0 |= LCDON;

    return;
}

void display_num_lcd(unsigned int n){
    LCDM6 = LCD_Num[((n / 10000) % 10)];    //5th digit, use num / 10,000 % 10 to extract number
    LCDM4 = LCD_Num[((n / 1000) % 10)];    //4th digit, use num / 1,000 % 10 to extract number
    LCDM19 = LCD_Num[((n / 100) % 10)];    //3rd digit, use num / 100 % 10 to extract number
    LCDM15 = LCD_Num[((n / 10) % 10)];     //2nd digit, use num / 10 % 10 to extract number
    LCDM8 = LCD_Num[(n % 10)];             //1st digit, use num % 10 to extract number

    if(LCDM6 == 0xFC) //if 0 then don't need to display, checking for 5th digit
    {//Use nested if statements for checking if a leading 0, don't want to erase a zero if it a required 0 like in 506
        LCDM6 = 0; //turn LED off
        if(LCDM4 == 0xFC) //checking 4th digit
        {
            LCDM4 = 0; //turn LED off
            if(LCDM19 == 0xFC) //checking 3rd digit
            {
                LCDM19 = 0; //turn LED off
                if(LCDM15 == 0xFC) //checking 2nd digit, wont check 1st digit in case we wish to display 0
                {
                    LCDM15 = 0; //turn LED off
                }
            }
        }
    }
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

//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    display_num_lcd(num);
    // Hardware clears the flag (CCIFG in TA0CCTL0)
    num += 1; //count up, increase digit by 1
}

//******* Writing the ISR *******
#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    // Detect button 1 (BUT1 in P1IFG is 1)
    if ( (P1IFG & BUT1) != 0 ) {
        TA0CTL ^= ~(TASSEL_1 | ID_0 | MC_1 | TACLR); //pause / resume timer
        P1OUT ^= redLED; // Toggle the red LED
        P9OUT ^= greenLED; // Toggle the green LED
        // Clear BUT1 in P1IFG
        P1IFG &= ~BUT1;
    }

    // Detect button 2 (BUT2 in P1IFG is 1)
    if ( (P1IFG & BUT2) != 0) {
        num = 0; //reset digit to 0
        display_num_lcd(num); //use to display 0 when timer is paused
        // Clear BUT2 in P1IFG
        P1IFG &= ~BUT2;
    }
}



