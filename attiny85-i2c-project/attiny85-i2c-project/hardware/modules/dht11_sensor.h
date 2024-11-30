/*
* dht11_sensor.h
*
* Created: 11/18/2024 7:28:03 PM
*  Author: Buddhika
*/


#ifndef DHT11_SENSOR_H_
#define DHT11_SENSOR_H_

#ifndef TIMER0_H_
#include "../../hardware/timers/timer0.h"
#endif

#ifndef LED_DEBUGGER_H_
#include "../../hardware/debug/led_debugger.h"
#endif

#ifndef DHT_11_DATA_PIN
#define DHT_11_DATA_PIN
#error "DHT_11_DATA_PIN is required! Please set a proper I/O pin."
#endif

static uint8_t dhtData[5];

static void resetDHTData() {
	for (uint8_t i = 0; i < 5; i++)
	{
		dhtData[i] = 0x00;
	}
}

uint8_t * getDHT11Data() {
	unsigned long timeTracker = 0L;
	unsigned long bitLengths[40];
	resetDHTData();
	
	startTimerZero();
	PORTB |= (1 << DHT_11_DATA_PIN);
	DDRB |= (1 << DHT_11_DATA_PIN);
	timerZeroDelayMicroseconds(1000000);
	PORTB &= ~(1 << DHT_11_DATA_PIN);
	timerZeroDelayMicroseconds(18300);
	PORTB |= (1 << DHT_11_DATA_PIN);
	DDRB &= ~(1 << DHT_11_DATA_PIN);
	while((PINB & (1 << PINB3)));
	timeTracker = getMicroseconds();
	while(!(PINB & (1 << PINB3))); // Rising Edge detector
	while((PINB & (1 << PINB3))); // Falling Edge detector
	dhtData[0] = getMicroseconds() - timeTracker;
	for(uint8_t i = 0; i < 40; i++) {
		timeTracker = getMicroseconds();
		while(!(PINB & (1 << PINB3))); // Rising Edge detector
		while((PINB & (1 << PINB3))); // Falling Edge detector
		bitLengths[i] = getMicroseconds() - timeTracker;
	}
	stopTimerZero();
	
	for(uint8_t i = 0; i < 40; i++) {
		uint8_t dhtIndex = (uint8_t) i / 8;
		if (bitLengths[i] > 60) {
			dhtData[dhtIndex]  = (dhtData[dhtIndex] << 1);
			dhtData[dhtIndex]  |= 0X01;
		}
		if (bitLengths[i] <= 60) {
			dhtData[dhtIndex]  = (dhtData[dhtIndex] << 1);
		}
	}
	//uint8_t checkSumData = dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3];
	//if (checkSumData != dhtData[4])
	//{
		//errorLoop();
	//}
	return dhtData;
}
#endif /* DHT11_SENSOR_H_ */