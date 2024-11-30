/*
* twi.h
*
* Created: 11/16/2024 12:09:50 PM
*  Author: Buddhika
*/


#ifndef TWI_H_
#define TWI_H_

#define SCL_PIN				PINB2
#define SDA_PIN				PINB0
#define EIGHT_CLOCK_CYCLE	0XF0
#define SINGLE_CLOCK_CYCLE	0XFE
#define WIRE_WRITE_MODE		0X00
#define WIRE_READ_MODE		0X01
#define MASTER_SEND_ACK		0X00
#define MASTER_SEND_NACK	0XFF

#ifndef F_CPU
	#error "F_CPU is reaquired to be defined when using twi.h."
#endif
#ifndef DEBUG_LED_PIN 
	#error "DEBUG_LED_PIN is reaquired to be defined when using twi.h."
#endif

#include <xc.h>
#include <util/delay.h>

void twiInit(){
	USIDR = 0xFF;
	// Enable software clock strobe (USITC)
	USICR |= (1 << USIWM1) | (1 << USICS1) | (1 << USICLK);
	// Clear the overflow interrupt flag
	// Reset clock counter
	USISR &= 0XF0;//~((1 << USICNT3) | (1 << USICNT2) | (1 << USICNT1) | (1 << USICNT0));
	_delay_ms(5); // Allow few mills to stabilize the micro-controller.
}

void twiStart(){// Initially make both the SCA and SDA pins outputs
	DDRB |= (1 << SCL_PIN) | (1 << SDA_PIN);
	// Pull-up both pins to HIGH
	PORTB |= (1 << SCL_PIN) | (1 << SDA_PIN);
	// Wait until SCL is set to HIGH
	while(!(PINB & (1 << SCL_PIN)));
	PORTB &= ~(1 << SDA_PIN); // Pull-down SDA pin to generate the start condition
	_delay_us(5);
	PORTB &= ~(1 << SCL_PIN); // Pull-down SCL pin to generate the start condition
	PORTB |= (1 << SDA_PIN); // SDA_PIN high (release to start transmitting)
}

void twiStop() {
	// Initially make both the SCA and SDA pins outputs
	DDRB |= (1 << SCL_PIN) | (1 << SDA_PIN);
	PORTB &= ~(1 << SDA_PIN);
	PORTB |= (1 << SCL_PIN); // Set SCL pin to HIGH
	// Wait until SCL is set to HIGH
	while(!(PINB & (1 << SCL_PIN)));
	_delay_us(5);
	PORTB |= (1 << SDA_PIN); // Set SDA pin to HIGH
	_delay_us(5);
}

void twiRestart() {
	twiStop();
	twiStart();
}

unsigned char twiTransaction(unsigned char cycles) {
	PORTB &= ~(1 << SCL_PIN);
	USISR = cycles;
	do
	{
		_delay_us(5);
		USICR |= (1<<USITC); // Set SCL to toggle
		while(!(PINB & (1 << SCL_PIN))); // wait until SCL pin is HIGH
		_delay_us(5);
		USICR |= (1<<USITC); // Set SCL to toggle
	} while (!(USISR & (1 << USIOIF)));
	unsigned char temp = USIDR;
	USIDR = 0xFF;
	return temp;
}

void twiSlaveAck() {
	DDRB &= ~(1 << SDA_PIN); // Make the SDA pin to input to be able to detect Acknowledge
	twiTransaction(SINGLE_CLOCK_CYCLE); // Wait until clock cycles are done
	DDRB |= (1 << SDA_PIN); // Make the SDA pin to output
}

void twiMasterAck(unsigned char mode) {
	DDRB |= (1 << SDA_PIN); // Make the SDA pin to output
	USIDR = mode; // if mode = 1, it will read another byte. if it is 0, TWI interface will send the NACK to slave (stop receive).
	twiTransaction(SINGLE_CLOCK_CYCLE); // Wait until clock cycles are done
}

void twiWrite(unsigned char data) {
	USIDR = data;
	twiTransaction(EIGHT_CLOCK_CYCLE); // Wait until clock cycles are done
	twiSlaveAck();
}

unsigned char twiRead(unsigned char mode) {
	DDRB &= ~(1 << SDA_PIN); // Make the SDA pin to input to be able to read
	unsigned char receive =  twiTransaction(EIGHT_CLOCK_CYCLE);
	twiMasterAck(mode);
	return receive;
}
#endif /* TWI_H_ */