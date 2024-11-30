/*
 * led_debugger.h
 *
 * Created: 11/23/2024 2:53:23 PM
 *  Author: Buddhika
 */ 


#ifndef LED_DEBUGGER_H_
#define LED_DEBUGGER_H_

#ifndef DEBUG_LED_PIN
#define DEBUG_LED_PIN
#error "DEBUG_LED_PIN is required for the led_debugger.h"
#endif

void initLEDDebugger() {
	PORTB |= (1 << DEBUG_LED_PIN);
	DDRB |= (1 << DEBUG_LED_PIN);
}

void hearbeat() {
	PORTB |= (1 << DEBUG_LED_PIN);
	_delay_ms(100);
	PORTB &= ~(1 << DEBUG_LED_PIN);
}

void errorLoop() {
	while (1)
	{
		PORTB ^= (1 << DEBUG_LED_PIN);
		_delay_ms(50);
	}
}


#endif /* LED_DEBUGGER_H_ */