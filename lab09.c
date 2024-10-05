
//SECTION 9.1
//LAB 9

//SECTION 9.1
#include <msp430fr6989.h>
#include <stdint.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data);
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data);
void Initialize_I2C(void);
void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);
void uart_write_uint16(unsigned int n);
void uart_write_string(char * str);

int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    Initialize_I2C(); //initialize I2C
    Initialize_UART(); //initialize UART

    unsigned int data;
    unsigned int i = 0;
    unsigned int j;

    char carriage = '\r';
    char newline = '\n';

    for(; ;){
        uart_write_string("Counter: ");
        uart_write_uint16(i);
        uart_write_string(" ");

        //I2C light sensor address: 0b1000100 = 0x44
        //Manufacturer ID Address: 7Eh
        i2c_read_word(0x44, 0x7E, &data);
        uart_write_string("Manufacturer ID:");
        uart_write_uint16(data);
        uart_write_string(" ");

        //Device ID 7Fh
        i2c_read_word(0x44, 0x7F, &data);
        uart_write_string("Device ID:");
        uart_write_uint16(data);

        uart_write_char(newline); //print newline
        uart_write_char(carriage); //print carriage
        i++;
        for(j = 0; j < 60000; j++){ }

    }
}



// Read a word (2 bytes) from I2C (address, register)
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data){
    unsigned char byte1, byte2;
    // Initialize the bytes to make sure data is received every time
    byte1 = 111;
    byte2 = 111;

    //********** Write Frame #1 ***************************
    UCB1I2CSA = i2c_address; // Set I2C address
    UCB1IFG &= ~UCTXIFG0;
    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    if(( UCB1IFG & UCNACKIFG )!=0) return -1;

    UCB1CTLW0 &= ~UCTR; // Master reads (R/W bit = Read)
    UCB1CTLW0 |= UCTXSTT; // Initiate a repeated Start Signal
    //****************************************************

    //********** Read Frame #1 ***************************
    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte1 = UCB1RXBUF;

    //****************************************************

    //********** Read Frame #2 ***************************
    while((UCB1CTLW0 & UCTXSTT)!=0) {}
    UCB1CTLW0 |= UCTXSTP; // Setup the Stop Signal

    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte2 = UCB1RXBUF;

    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
    //****************************************************

    // Merge the two received bytes
    *data = ( (byte1 << 8) | (byte2 & 0xFF) );
    return 0;
}


// Write a word (2 bytes) to I2C (address, register)
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data){
    unsigned char byte1, byte2;

    byte1 = (data >> 8) & 0xFF; // MSByte
    byte2 = data & 0xFF; // LSByte

    UCB1I2CSA = i2c_address; // Set I2C address

    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    //********** Write Byte #1 ***************************
    UCB1TXBUF = byte1;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}

    //********** Write Byte #2 ***************************
    UCB1TXBUF = byte2;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}

    UCB1CTLW0 |= UCTXSTP;
    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}

    return 0;
}


// Configure eUSCI in I2C master mode
void Initialize_I2C(void) {
    // Enter reset state before the configuration starts...
    UCB1CTLW0 |= UCSWRST;

    // Divert pins to I2C functionality
    P4SEL1 |= (BIT1|BIT0);
    P4SEL0 &= ~(BIT1|BIT0);

    // Keep all the default values except the fields below...
    // (UCMode 3:I2C) (Master Mode) (UCSSEL 1:ACLK, 2,3:SMCLK)
    UCB1CTLW0 |= UCMODE_3 | UCMST | UCSSEL_3;

    // Clock divider = 8 (SMCLK @ 1.048 MHz / 8 = 131 KHz)
    UCB1BRW = 8;

    // Exit the reset mode
    UCB1CTLW0 &= ~UCSWRST;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//SECTION 9.2
#include <msp430fr6989.h>
#include <stdint.h>
#define FLAGS UCA1IFG // Contains the transmit & receive flags
#define RXFLAG UCRXIFG // Receive flag
#define TXFLAG UCTXIFG // Transmit flag
#define TXBUFFER UCA1TXBUF // Transmit buffer
#define RXBUFFER UCA1RXBUF // Receive buffer

int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data);
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data);
void Initialize_I2C(void);
void Initialize_UART(void);
void uart_write_char(unsigned char ch);
unsigned char uart_read_char(void);
void uart_write_uint16(unsigned int n);
void uart_write_string(char * str);

int main(void){
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO pins

    // Divert pins to I2C functionality
    // (UCB1SDA same as P4.0) (UCB1SCL same as P4.1)
    // (P4SEL1=11, P4SEL0=00) (P4DIR=xx)
    P4SEL1 |= (BIT1|BIT0);
    P4SEL0 &= ~(BIT1|BIT0);

    Initialize_I2C();
    Initialize_UART();

    //RN=0111b=7 The LSB bit is worth 1.28
    //CT=0 Result produced in 100 ms
    //M=11b=3 Continuous readings
    //ME=1 Mask (hide) the Exponent from the result
    unsigned int config = 0x7604;

     //I2C light sensor address: 0b1000100 = 0x44
     //Configuration Register: 01h
     i2c_write_word(0x44, 0x01, config);

    unsigned int data;
    unsigned int i = 0;
    volatile int k;
    volatile unsigned int j;

    char carriage = '\r';
    char newline = '\n';


    for(; ;){
        uart_write_string("Counter: ");
        uart_write_uint16(i);
        uart_write_string(" ");

        //I2C light sensor address: 0b1000100 = 0x44
        //Configuration address: 0x01
        i2c_read_word(0x44, 0x00, &data);
        uart_write_string("Result: ");
        uart_write_uint16(data);
        uart_write_string(" ");

        uart_write_char(newline); //print newline
        uart_write_char(carriage); //print carriage
        i++;
        for(j = 0; j < 60000; j++){
            //for(k =0; k<40000; k++){}
        }

    }

}



// Read a word (2 bytes) from I2C (address, register)
int i2c_read_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int * data){
    unsigned char byte1, byte2;
    // Initialize the bytes to make sure data is received every time
    byte1 = 111;
    byte2 = 111;

    //********** Write Frame #1 ***************************
    UCB1I2CSA = i2c_address; // Set I2C address
    UCB1IFG &= ~UCTXIFG0;
    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    if(( UCB1IFG & UCNACKIFG )!=0) return -1;

    UCB1CTLW0 &= ~UCTR; // Master reads (R/W bit = Read)
    UCB1CTLW0 |= UCTXSTT; // Initiate a repeated Start Signal
    //****************************************************

    //********** Read Frame #1 ***************************
    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte1 = UCB1RXBUF;

    //****************************************************

    //********** Read Frame #2 ***************************
    while((UCB1CTLW0 & UCTXSTT)!=0) {}
    UCB1CTLW0 |= UCTXSTP; // Setup the Stop Signal

    while ( (UCB1IFG & UCRXIFG0) == 0) {}
    byte2 = UCB1RXBUF;

    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}
    //****************************************************

    // Merge the two received bytes
    *data = ( (byte1 << 8) | (byte2 & 0xFF) );
    return 0;
}


// Write a word (2 bytes) to I2C (address, register)
int i2c_write_word(unsigned char i2c_address, unsigned char i2c_reg, unsigned int data){
    unsigned char byte1, byte2;

    byte1 = (data >> 8) & 0xFF; // MSByte
    byte2 = data & 0xFF; // LSByte

    UCB1I2CSA = i2c_address; // Set I2C address

    UCB1CTLW0 |= UCTR; // Master writes (R/W bit = Write)
    UCB1CTLW0 |= UCTXSTT; // Initiate the Start Signal

    while ((UCB1IFG & UCTXIFG0) ==0) {}

    UCB1TXBUF = i2c_reg; // Byte = register address

    while((UCB1CTLW0 & UCTXSTT)!=0) {}

    //********** Write Byte #1 ***************************
    UCB1TXBUF = byte1;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}

    //********** Write Byte #2 ***************************
    UCB1TXBUF = byte2;
    while ( (UCB1IFG & UCTXIFG0) == 0) {}

    UCB1CTLW0 |= UCTXSTP;
    while ( (UCB1CTLW0 & UCTXSTP) != 0) {}

    return 0;
}


// Configure eUSCI in I2C master mode
void Initialize_I2C(void) {
    // Enter reset state before the configuration starts...
    UCB1CTLW0 |= UCSWRST;

    // Divert pins to I2C functionality
    P4SEL1 |= (BIT1|BIT0);
    P4SEL0 &= ~(BIT1|BIT0);

    // Keep all the default values except the fields below...
    // (UCMode 3:I2C) (Master Mode) (UCSSEL 1:ACLK, 2,3:SMCLK)
    UCB1CTLW0 |= UCMODE_3 | UCMST | UCSSEL_3;

    // Clock divider = 8 (SMCLK @ 1.048 MHz / 8 = 131 KHz)
    UCB1BRW = 8;

    // Exit the reset mode
    UCB1CTLW0 &= ~UCSWRST;
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







