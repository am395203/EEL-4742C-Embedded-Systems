//settings 11.1

#include "msp430fr6989.h"
#include "LcdDriver/middle_driver.h"
#include "Grlib/grlib/grlib.h"
#include <stdint.h>

void HAL_LCD_PortInit(void)
{
    /////////////////////////////////////
    // Configuring the SPI pins
    /////////////////////////////////////
    // Divert UCB0CLK/P1.4 pin to serial clock
    P1SEL0 |= BIT4;

    // Divert UCB0SIMO/P1.6 pin to SIMO
    P1SEL0 |= BIT6;

    // OK to ignore UCB0STE/P1.5 since we'll connect the display's enable bit to low (enabled all the time)
    // OK to ignore UCB0SOMI/P1.7 since the display doesn't give back any data

    ///////////////////////////////////////////////
    // Configuring the display's other pins
    ///////////////////////////////////////////////
    // Set reset pin as output //P9.4
    P9DIR |= BIT4;
    P9SEL1 &= ~BIT4;
    P9SEL0 &= ~BIT4;

    // Set the data/command pin as output
    P2DIR |= BIT3;
    P2SEL1 &= ~BIT3;
    P2SEL0 &= ~BIT3;

    // Set the chip select pin as output
    P2DIR |= BIT5;
    P2SEL1 &= ~BIT5;
    P2SEL0 &= ~BIT5;

    return;
}

void HAL_LCD_SpiInit(void)
{
    //////////////////////////
    // SPI configuration
    //////////////////////////

    // Put eUSCI in reset state while modifying the configuration
    UCB0CTLW0 |= UCSWRST;

    // Set clock phase to "capture on 1st edge, change on following edge"
    UCB0CTLW0 |= UCCKPH;

    // Set clock polarity to "inactive low"
    UCB0CTLW0 &= ~UCCKPL;

    // Set data order to "transmit MSB first"
    UCB0CTLW0 |= UCMSB;

    // Set MCU to "SPI master"
    UCB0CTLW0 |= UCMST;

    // Set SPI to "3 pin SPI" (we won't use eUSCI's chip select)
    UCB0CTLW0 |= UCMODE_0;

    // Set module to synchronous mode
    UCB0CTLW0 |= UCSYNC;

    // Set clock to SMCLK
    UCB0CTLW0 |= UCSSEL_3;

    // Set clock divider to 1 (SMCLK is from DCO at 8 MHz; we'll run SPI at 8 MHz)
    UCB0BRW = 1;

    // Exit the reset state at the end of the configuration
    UCB0CTLW0 &= ~UCSWRST;

    // Set CS' (chip select) bit to 0 (display always enabled) //at P2.5
    P2OUT &= ~BIT5;

    // Set DC' bit to 0 (assume data) //at P2.3
    P2OUT &= ~BIT3;

    return;
}


//*****************************************************************************
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//*****************************************************************************
void HAL_LCD_writeCommand(uint8_t command)
{
    // For command, set the DC' bit to low before transmission
    P2OUT &= ~BIT3;

    // Wait as long as the module is busy
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = command;

    // Set DC' bit back to high
    P2OUT |= BIT3;
}


//*****************************************************************************
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//*****************************************************************************
void HAL_LCD_writeData(uint8_t data)
{
    // Wait as long as the module is busy
    while (UCB0STATW & UCBUSY);

    // Transmit data
    UCB0TXBUF = data;
}




// Code to print to the LCD pixel display on the Educational BoosterPack

#include "msp430fr6989.h"
#include "Grlib/grlib/grlib.h"          // Graphics library (grlib)
#include "LcdDriver/lcd_driver.h"       // LCD driver
#include <stdio.h>

#define redLED BIT0
#define greenLED BIT7
#define button BIT1

void main(void)
{
    volatile unsigned int counter=0;
    char mystring[20];
    unsigned int n;

    WDTCTL = WDTPW | WDTHOLD;     // Stop the Watchdog timer
    PM5CTL0 &= ~LOCKLPM5;         // Disable GPIO power-on default high-impedance mode

    P1DIR |= redLED;    P1OUT &= ~redLED;
    P9DIR |= greenLED;  P9OUT &= ~greenLED;
    P1DIR &= ~button; P1REN|=button; P1OUT|=button; // button, resistor, pullup

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
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    // Set the default font for strings
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    // Clear the screen
    Graphics_clearDisplay(&g_sContext);
    ////////////////////////////////////////////////////////////////////////////////////////////


    Graphics_drawStringCentered(&g_sContext, "Welcome to", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    sprintf(mystring, "EEL 4742 Lab!");
    Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);

    n = 1234;
    sprintf(mystring, "%d", n);
    Graphics_drawStringCentered(&g_sContext, mystring, AUTO_STRING_LENGTH, 64, 80, OPAQUE_TEXT);

    while(1){}


}






///////////////11.2




