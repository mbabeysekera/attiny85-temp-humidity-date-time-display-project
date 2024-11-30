/*
* timer0.h
*
* Created: 11/18/2024 7:40:03 PM
*  Author: Buddhika
*/


#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/interrupt.h>

volatile unsigned long microseconds = 0L;
// F_CPU has been set to ~ 8.370000MHz
// Therefore, the counter compare set to run at every 80 cycles => 10us (Found experimentally)
ISR(TIMER0_COMPA_vect) {
	microseconds++;
	//PORTB ^= (1 << PINB1);
}

static void initTimerZero() {
	microseconds = 0L;
	// Start timer sync mode
	GTCCR |= (1 << TSM) | (1 << PSR0);
	// Config TCCR0A to CTC mode.
	TCCR0A |= (1 << WGM01);
	// Config TCCR0B to normal mode and set prescaler to 1.
	TCCR0B |= (1 << CS00);
	// Timer0 interrupt masking (No interrupts)
	TIMSK |= (1 << OCIE0A);
	// Clear Output compare flag A by writing 1
	TIFR |= (1 << OCF0A);
	// Set timer0 to lowest value (0)
	TCNT0 = 0;
	// Experimentally figure out value for 10us
	OCR0A = 80;
	// Exit from sync mode to let the timer start as normal.
	GTCCR = 0;
}

void startTimerZero() {
	cli();
	initTimerZero();
	// Start the timer0
	sei();
}

void stopTimerZero() {
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

unsigned long getMicroseconds() {
	return microseconds * 10;
}

void timerZeroDelayMicroseconds(unsigned long delayInMicroseconds) {
	unsigned long prev = getMicroseconds();
	while(!((getMicroseconds() - prev) > delayInMicroseconds));
}
#endif /* TIMER0_H_ */