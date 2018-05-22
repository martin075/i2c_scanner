// i2c scanner for addr of device
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SCL_CLOCK  100000UL  // I2C bus clocked at 100kHz

#include "I2C.h"             // multi-master, multi-slave, packet switched, single-ended, serial bus

#define UART_BAUD_RATE 2400
#include "uart/uart.h"



int main(){

	char buf[10];

	// Configure All Pins as Inputs
	DDRB = 0x00;  //0 == Input; 1 = Output
	DDRC = 0x00;
	DDRD = 0x00;
	// Enable All Pull-Ups
	PORTB = 0xFF;  //0 = Tri-State; 1 = Pull-Up
	PORTC = 0xFF;
	PORTD = 0xFF;

	// SPI Configuration
	SPCR |= (1<<SPE) | (1<<MSTR);  // SPI enabled as master, Mode0 at 4 MHz
	SPSR |= (1<<SPI2X);  // double the SPI rate: 4-->8 MHz

	
	// Initialize I2C Bus
	TWSR &= ~(1<<TWPS1) & ~(1<<TWPS0);  // set TWI bit rate prescaler value to 1-one
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;    // SCL frequency = CPU Clock Frequency / (16+(2*TWBR*Prescaler))


	//uint8_t i, xPos, yPos, asciiText[3];  // main() scope variables
	uint8_t i;

	//init uart
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
    sei();




	while(1){

		for (i = 1; i < 127; i++){        // iterate over valid 7-bit address range
			 if( (i2cStart((i << 1) | 0))==0 ){  // if i2cStart() returns 0, then no response
				 itoa(i, buf, 10);	// int to char
				 uart_puts(buf);
				 uart_puts("dec ACK=0 no response");
	         	 uart_puts("\r\n");
				 _delay_ms(500);
				}

			else{               // else i2cStart() returns 1, slave ACK received
				itoa(i, buf, 10);
				uart_puts(buf);
				uart_puts("dec - found I2C addr, ACK=1");
				uart_puts("\r\n");                  // convert hex address of slave to ascii
				}
			}	//for

			i2cStop();       // release the bus
			_delay_ms(5000);  // pause for LCD screen readability when cycling through addresses being scanned 100ms
		}	//while
		
	}	//main
 
