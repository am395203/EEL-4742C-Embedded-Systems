#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#define redLED BIT0
#define greenLED BIT7
#define button BIT1

void secondScreen(void);
void config_ACLK_to_32KHz_crystal(void);

char mystring[20];
unsigned int n;
volatile uint8_t counter = 240;
Graphics_Context g_sContext;
Graphics_Context g_sContext2;


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;     // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // Disable GPIO power-on default high-impedance mode

    P1DIR |= redLED;    P1OUT &= ~redLED;
    P9DIR |= greenLED;  P9OUT &= ~greenLED;
    P1DIR &= ~button; P1REN|=button; P1OUT|=button; // button, resistor, pullup
    P1IE |= button;// 1: enable interrupts
    P1IES |= button; // 1: interrupt on falling edge
    P1IFG &= button; // 0: clear the interrupt flags

    // Configure SMCLK to 8 MHz (used as SPI clock)
    CSCTL0 = CSKEY;                 // Unlock CS registers
    CSCTL3 &= ~(BIT4|BIT5|BIT6);    // DIVS=0
    CSCTL0_H = 0;                   // Relock the CS registers

    ////////////////////////////////////////////////////////////////////////////////////////////
    Graphics_Context g_sContext;        // Declare a graphic library context
    Crystalfontz128x128_Init();         // Initialize the display

    // Set the screen orientation
    Crystalfontz128x128_SetOrientation(0);

    // Initialize the context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);

    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_CHARTREUSE);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    // Clear the screen
   // Graphics_clearDisplay(&g_sContext);

    /////////////////////////////////// Context 2 for frame 2 /////////////////////////////////////////////////////////
    // Initialize the context
    Graphics_initContext(&g_sContext2, &g_sCrystalfontz128x128);

    // Set background and foreground colors
    Graphics_setBackgroundColor(&g_sContext2, GRAPHICS_COLOR_CHOCOLATE);
    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_WHITE);

    // Set the default font for strings
    GrContextFontSet(&g_sContext2, &g_sFontFixed6x8);

    /////// TIMER CONFIGURATION ////////////////////////////////////////////////////////////
    // Configure Channel 0 for up mode with interrupt
    TA0CCR0 = 32767; // Fill to get 1 second @ 32 KHz (32768 - 1)
    TA0CCTL0 |= CCIE; // Enable Channel 0 CCIE bit
    TA0CCTL0 &= ~CCIFG; // Clear Channel 0 CCIFG bit

    // Timer_A: ACLK, div by 1, up mode, clear TAR (leaves TAIE=0)
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
    ////////////////////////////////////////////////////////////////////////////////////////

    // Enable the global interrupt bit (call an intrinsic function)
    _enable_interrupts();
    secondScreen();

}

void printTimer(){
    if(counter == 0){
        sprintf(mystring, "  0  ", counter); //once 0, print 0 with multiple spaces to erase remaining numbers
        Graphics_drawStringCentered(&g_sContext2, mystring, AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);
    }
    sprintf(mystring, " %d ", counter);
    Graphics_drawStringCentered(&g_sContext2, mystring, AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

}






//////////////////  Second Screen ////////////////////////////////////////////
void secondScreen(){
    Graphics_clearDisplay(&g_sContext2);
    Graphics_Rectangle g_Rectangle1; //rectangle struct
    g_Rectangle1.xMin = 60;
    g_Rectangle1.yMin = 85;
    g_Rectangle1.xMax = 80;
    g_Rectangle1.yMax = 95;

    Graphics_Rectangle g_Rectangle2; //rectangle struct
    g_Rectangle2.xMin = 90;
    g_Rectangle2.yMin = 85;
    g_Rectangle2.xMax = 110;
    g_Rectangle2.yMax = 95;

    sprintf(mystring, "EEL 4742 Lab");
    Graphics_drawStringCentered(&g_sContext2, mystring, AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    GrContextFontSet(&g_sContext2, &g_sFontCm12);
    sprintf(mystring, "Graphics Demo");
    Graphics_drawStringCentered(&g_sContext2, mystring, AUTO_STRING_LENGTH, 64, 40, OPAQUE_TEXT);

    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_WHITE);
    Graphics_drawLineH(&g_sContext2, 20, 100, 75); //draw line

    Graphics_drawCircle(&g_sContext2, 20, 90, 8); //Circle 1 w/ outline
    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_DARK_CYAN);
    Graphics_fillCircle(&g_sContext2, 20, 90, 7);

    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_MAROON); //circle 2, no outline
    Graphics_fillCircle(&g_sContext2, 45, 90, 8);

    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_DARK_CYAN); //rectangle 1 w/ outline
    Graphics_fillRectangle(&g_sContext2, &g_Rectangle1);
    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_WHITE);
    Graphics_drawRectangle(&g_sContext2, &g_Rectangle1);

    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_MAROON); //rectangle 1 w/ outline
    Graphics_fillRectangle(&g_sContext2, &g_Rectangle2);

    Graphics_setForegroundColor(&g_sContext2, GRAPHICS_COLOR_WHITE); //draw line
    Graphics_drawLineH(&g_sContext2, 20, 100, 110);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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

//******* Writing the ISR *******
#pragma vector = TIMER0_A0_VECTOR // Link the ISR to the vector
__interrupt void T0A0_ISR() {
    printTimer();
    counter++;
    // Hardware clears the flag (CCIFG in TA0CCTL0)
}

#pragma vector = PORT1_VECTOR // Write the vector name
__interrupt void Port1_ISR() {
    // Detect button 1 (BUT1 in P1IFG is 1)
    if ( (P1IFG & button) != 0 ) {

    }
    P1IFG &= ~button;
}

