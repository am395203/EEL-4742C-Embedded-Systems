/////LAB 10///////////
//10.1
#include <msp430fr6989.h>
#include <stdint.h>
#define redLED BIT0 // Red LED at P1.0
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

void Initialize_ADC();
void Initialize_UART(void);
void uart_write_char(unsigned char ch);
void uart_write_uint16(unsigned int n);


int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off t

    Initialize_ADC(); //initialize ADC
    Initialize_UART(); //initialize UART

    char carriage = '\r';
    char newline = '\n';
    unsigned int i;

    for( ; ; ){
        ADC12CTL0 |= ADC12SC; //start conversion
        while((ADC12CTL0 & ADC12BUSY) != 0){} //wait while ADC12BUSY is on

        uart_write_uint16(ADC12MEM0); //transmit ADC12MEM0 value to terminal
        uart_write_char(newline); //print newline
        uart_write_char(carriage); // print carriage
        for(i = 0; i < 60000; i++){} //delay loop
        P1OUT ^= redLED; // toggle red LED

    }

}




void Initialize_ADC() {
    // Divert the pins to analog functionality
    // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;

    // Turn on the ADC module
    ADC12CTL0 |= ADC12ON;

    // Turn off ENC (Enable Conversion) bit while modifying the configuration
    ADC12CTL0 &= ~ADC12ENC;

    //*************** ADC12CTL0 ***************
    // Set ADC12SHT0 (select the number of cycles that you determined)
    ADC12CTL0 |= ADC12SHT0_3; //SHT + conversions time = 30 ~ from table go up to next number which is 32

    //*************** ADC12CTL1 ***************
    // Set ADC12SHS (select ADC12SC bit as the trigger)
    // Set ADC12SHP bit
    // Set ADC12DIV (select the divider you determined)
    // Set ADC12SSEL (select MODOSC)
    ADC12CTL1 |= ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0;

    //*************** ADC12CTL2 ***************
    // Set ADC12RES (select 12-bit resolution)
    // Set ADC12DF (select unsigned binary format)
    ADC12CTL2 |= ADC12RES_2;
    ADC12CTL2 &= ~ADC12DF;
    //*************** ADC12CTL3 ***************
    // Leave all fields at default values

    //*************** ADC12MCTL0 ***************
    // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
    // Set ADC12INCH (select channel A10)
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_10;

    // Turn on ENC (Enable Conversion) bit at the end of the configuration
    ADC12CTL0 |= ADC12ENC;

    return;
}

// Configure UART to the popular configuration
void Initialize_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_2;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=13, UCBRS=0x22, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5|UCBRS1|UCBRF3|UCBRF2|UCBRF0|UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}


void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}



void uart_write_uint16(unsigned int n){
    int number;

    if(n >= 10000){ //extract 5th digit
        number = (n / 10000) % 10; //divide by 10,000, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number); //then write to screen
    }
    if(n >= 1000){ //extract 4th digit
        number = (n/1000) %10;//divide by 1000, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
    }
    if(n >= 100){ //extract 3rd digit
        number = (n/100) %10;//divide by 100, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
        }
    if(n >= 10){ //extract 2nd digit
        number = (n/10) %10;//divide by 10, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
        }

    number = n % 10; //1st digit
    number += '0'; //change back to char by adding '0' to int
    uart_write_char(number);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//10.2
#include <msp430fr6989.h>
#include <stdint.h>
#define redLED BIT0 // Red LED at P1.0
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

void Initialize_ADC();
void Initialize_UART(void);
void uart_write_char(unsigned char ch);
void uart_write_uint16(unsigned int n);


int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Direct pin as output
    P1OUT &= ~redLED; // Turn LED Off t

    Initialize_ADC(); //initialize ADC
    Initialize_UART(); //initialize UART

    char carriage = '\r';
    char newline = '\n';
    unsigned int i;

    for( ; ; ){
        ADC12CTL0 |= ADC12SC; //start conversion
        while((ADC12CTL0 & ADC12BUSY) != 0){} //wait while ADC12BUSY is on

        uart_write_uint16(ADC12MEM0); //transmit ADC12MEM0 value to terminal
        uart_write_char(','); //print newline
        uart_write_uint16(ADC12MEM1); //transmit ADC12MEM1 value to terminal
        uart_write_char(newline); //print newline
        uart_write_char(carriage); // print carriage
        for(i = 0; i < 60000; i++){} //delay loop
        P1OUT ^= redLED; // toggle red LED

    }

}




void Initialize_ADC() {
    // Divert the pins to analog functionality
    // X-axis: A10/P9.2, for A10 (P9DIR=x, P9SEL1=1, P9SEL0=1)
    P9SEL1 |= BIT2;
    P9SEL0 |= BIT2;

    // Divert the vertical signal’s pin to analog functionality
    //Y-axis: J3.26, A4/P8.7, for A4 (P8DIR = x, P8SEL1 = 1, P8SEL0 = 1)
    P8SEL1 |= BIT7;
    P8SEL0 |= BIT7;

    // Turn on the ADC module
    ADC12CTL0 |= ADC12ON;

    // Turn off ENC (Enable Conversion) bit while modifying the configuration
    ADC12CTL0 &= ~ADC12ENC;

    //*************** ADC12CTL0 ***************
    // Set ADC12SHT0 (select the number of cycles that you determined)
    // Set the bit ADC12MSC (Multiple Sample and Conversion)
    ADC12CTL0 |= ADC12SHT0_3 | ADC12MSC; //SHT + conversions time = 30 ~ from table go up to next number which is 32

    //*************** ADC12CTL1 ***************
    // Set ADC12SHS (select ADC12SC bit as the trigger)
    // Set ADC12SHP bit
    // Set ADC12DIV (select the divider you determined)
    // Set ADC12SSEL (select MODOSC)
    // Set ADC12CONSEQ (select sequence-of-channels)
    ADC12CTL1 |= ADC12SHS_0 | ADC12SHP | ADC12DIV_0 | ADC12SSEL_0 | ADC12CONSEQ_1;

    //*************** ADC12CTL2 ***************
    // Set ADC12RES (select 12-bit resolution)
    // Set ADC12DF (select unsigned binary format)
    ADC12CTL2 |= ADC12RES_2;
    ADC12CTL2 &= ~ADC12DF;

    //*************** ADC12CTL3 ***************
    // Set ADC12CSTARTADD to 0 (first conversion in ADC12MEM0)
    ADC12CTL3 |= ADC12CSTARTADD_0;

    //*************** ADC12MCTL0 ***************
    // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
    // Set ADC12INCH (select channel A10)
    ADC12MCTL0 |= ADC12VRSEL_0 | ADC12INCH_10;

    //*************** ADC12MCTL1 ***************
    // Set ADC12VRSEL (select VR+=AVCC, VR-=AVSS)
    // Set ADC12INCH (select the analog channel that you found)
    // Set ADC12EOS (last conversion in ADC12MEM1)
    ADC12MCTL1 |= ADC12VRSEL_0 | ADC12EOS | ADC12INCH_4;

    // Turn on ENC (Enable Conversion) bit at the end of the configuration
    ADC12CTL0 |= ADC12ENC;

    return;
}

// Configure UART to the popular configuration
void Initialize_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_2;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=13, UCBRS=0x22, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5|UCBRS1|UCBRF3|UCBRF2|UCBRF0|UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}


void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}



void uart_write_uint16(unsigned int n){
    int number;

    if(n >= 10000){ //extract 5th digit
        number = (n / 10000) % 10; //divide by 10,000, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number); //then write to screen
    }
    if(n >= 1000){ //extract 4th digit
        number = (n/1000) %10;//divide by 1000, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
    }
    if(n >= 100){ //extract 3rd digit
        number = (n/100) %10;//divide by 100, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
        }
    if(n >= 10){ //extract 2nd digit
        number = (n/10) %10;//divide by 10, then modulo 10
        number += '0'; //change back to char by adding '0' to int
        uart_write_char(number);
        }

    number = n % 10; //1st digit
    number += '0'; //change back to char by adding '0' to int
    uart_write_char(number);

}


























