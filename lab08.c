//LAB 08

//SECTION 8.1
#include <msp430fr6989.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define redLED BIT0 //Red LED at P1.0
#define greenLED BIT7 //Green LED at P9.7

void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);

int main(void){
    volatile unsigned int i;
    unsigned char ch;
    unsigned char stored;
    char carriage = '\r';
    char newline = '\n';

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red on
    P9OUT &= ~greenLED; // Green off

    Initialize_UART();
    for( ; ; ){
        for(ch = '0'; ch <= '9'; ch++){
            stored = uart_read_char(); //stored read number
            if(stored == '1'){  //if 1 then turn green LED on
                P9OUT |= greenLED;
            }
            if(stored == '2'){ //if 2 is read then turn green LED off
                P9OUT &= ~greenLED;
             }

            uart_write_char(ch); //print digit
            uart_write_char(newline); //print newline
            uart_write_char(carriage); //print carriage
            P1OUT ^= redLED; //toggle red LED
            for(i=0; i < 45000; i++){} //create a delay loop
        }

    }
}

// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control
// Initial clock: SMCLK @ 1.048 MHz with oversampling
void Initialize_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_2;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=8, UCBRS=0x20, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5|UCBRF3|UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}


// The function returns the byte; if none received, returns NULL
unsigned char uart_read_char(void){
    unsigned char temp;
    // Return NULL if no byte received
    if( (FLAGS & RXFLAG) == 0){
        return '\0';
    }
    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    return temp;
}






//SECTION 8.2//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <msp430fr6989.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define redLED BIT0 //Red LED at P1.0
#define greenLED BIT7 //Green LED at P9.7

void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);
void uart_write_uint16(unsigned int n);


int main(void){
    volatile unsigned int i;
    int j;
    unsigned char ch;
    unsigned char stored;
    char carriage = '\r';
    char newline = '\n';

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red on
    P9OUT &= ~greenLED; // Green off

    Initialize_UART();

    for( ; ; ){
        for(j = 65530; j <= 65535; j++){
            stored = uart_read_char(); //stored read number
            if(stored == '1'){  //if 1 then turn green LED on
                P9OUT |= greenLED;
            }
            if(stored == '2'){ //if 2 is read then turn green LED off
                P9OUT &= ~greenLED;
             }

            uart_write_uint16(j); //accepts an int so subtract '0' to get an int
            uart_write_char(newline); //print newline
            uart_write_char(carriage); //print carriage
            P1OUT ^= redLED; //toggle red LED
            for(i=0; i < 45000; i++){} //create a delay loop
        }

    }
}

// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control
// Initial clock: SMCLK @ 1.048 MHz with oversampling
void Initialize_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_2;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=8, UCBRS=0x20, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5|UCBRF3|UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}


// The function returns the byte; if none received, returns NULL
unsigned char uart_read_char(void){
    unsigned char temp;
    // Return NULL if no byte received
    if( (FLAGS & RXFLAG) == 0){
        return '\0';
    }
    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    return temp;
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


////SECTION 8.3//////////////////////////////////////////////////////////////////////////////////////////////////
#include <msp430fr6989.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define redLED BIT0 //Red LED at P1.0
#define greenLED BIT7 //Green LED at P9.7

void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);
void uart_write_uint16(unsigned int n);
void uart_write_string(char * str);

int main(void){
    volatile unsigned int i;
    int j;
    unsigned char ch;
    unsigned char stored;
    char carriage = '\r';
    char newline = '\n';
    char mystring[] = "UART Transmission Begins...";

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red on
    P9OUT &= ~greenLED; // Green off

    Initialize_UART();

    uart_write_string(mystring);
    uart_write_char(newline); //print newline
    uart_write_char(carriage); //print carriage

    for( ; ; ){
        P1OUT ^= redLED; //toggle red LED
        for(i=0; i < 45000; i++){} //create a delay loop
    }
}


// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control
// Initial clock: SMCLK @ 1.048 MHz with oversampling
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


// The function returns the byte; if none received, returns NULL
unsigned char uart_read_char(void){
    unsigned char temp;
    // Return NULL if no byte received
    if( (FLAGS & RXFLAG) == 0){
        return '\0';
    }
    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    return temp;
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

void uart_write_string(char * str){
   unsigned int i = 0;

   while(str[i] != '\0'){
       uart_write_char(str[i]);
       i++;
   }

}






//SECTION 8.4////////////////////////////////////////////////////////////////////
#include <msp430fr6989.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer
#define redLED BIT0 //Red LED at P1.0
#define greenLED BIT7 //Green LED at P9.7

void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);
void uart_write_uint16(unsigned int n);
void uart_write_string(char * str);
void Initialize_UART_2(void);
void config_ACLK_to_32KHz_crystal();

int main(void){
    volatile unsigned int i;
    int j;
    unsigned char ch;
    unsigned char stored;
    char carriage = '\r';
    char newline = '\n';
    char mystring[] = "UART Transmission Begins...";

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    P1DIR |= redLED; // Pins as output
    P9DIR |= greenLED;
    P1OUT &= ~redLED; // Red on
    P9OUT &= ~greenLED; // Green off

    config_ACLK_to_32KHz_crystal();
    Initialize_UART_2();

    for( ; ; ){
        for(j = 0; j <= 65535; j++){
            uart_write_uint16(j); //accepts an int so subtract '0' to get an int
            uart_write_char(newline); //print newline
            uart_write_char(carriage); //print carriage
            P1OUT ^= redLED; //toggle red LED
            for(i=0; i < 45000; i++){} //create a delay loop
        }

    }
}


// Configure UART to the popular configuration
// 9600 baud, 8-bit data, LSB first, no parity bits, 1 stop bit
// no flow control
// Initial clock: SMCLK @ 1.048 MHz with oversampling
void Initialize_UART(void){
    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use SMCLK clock; leave other settings default
    UCA1CTLW0 |= UCSSEL_2;

    // Configure the clock dividers and modulators
    // UCBR=6, UCBRF=8, UCBRS=0x20, UCOS16=1 (oversampling)
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS5|UCBRF3|UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
}

void uart_write_char(unsigned char ch){
    // Wait for any ongoing transmission to complete
    while ( (FLAGS & TXFLAG)==0 ) {}

    // Write the byte to the transmit buffer
    TXBUFFER = ch;
}


// The function returns the byte; if none received, returns NULL
unsigned char uart_read_char(void){
    unsigned char temp;
    // Return NULL if no byte received
    if( (FLAGS & RXFLAG) == 0){
        return '\0';
    }
    // Otherwise, copy the received byte (clears the flag) and return it
    temp = RXBUFFER;
    return temp;
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

void uart_write_string(char * str){
   unsigned int i = 0;

   while(str[i] != '\0'){
       uart_write_char(str[i]);
       i++;
   }

}

void Initialize_UART_2(void){

    // Divert pins to UART functionality
    P3SEL1 &= ~(BIT4|BIT5);
    P3SEL0 |= (BIT4|BIT5);

    // Use ACLK clock
    UCA1CTLW0 |= UCSSEL_1;

    // Configure the clock dividers and modulators
    //UCBR=6,UCBRF=x,UCBRS=0xEE,UCOS16 =0
    UCA1BRW = 6;
    UCA1MCTLW = UCBRS7|UCBRS6|UCBRS5|UCBRS3|UCBRS2|UCBRS1|~UCOS16;

    // Exit the reset state (so transmission/reception can begin)
    UCA1CTLW0 &= ~UCSWRST;
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

















