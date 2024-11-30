/*
* main.cpp
*
* Created: 11/16/2024 11:42:21 AM
*  Author: Buddhika
* The Programmer board has an LED connected to PINB1
*/

#define F_CPU								8370000UL
#define DEBUG_LED_PIN						PINB1

#define DHT_11_DATA_PIN						PINB3
#define DS1307_RTC_ADDRESS					0XD0
#define LCD_ADDRESS							0X4E

#define HUMID_DISPLAY_ROW_CORDINATE			0
#define HUMID_INTEGRAL_DISPLAY_LOCATION		10
#define HUMID_DECIMAL_DISPLAY_LOCATION		13
#define TEMP_DISPLAY_ROW_CORDINATE			1
#define TEMP_INTEGRAL_DISPLAY_LOCATION		10
#define TEMP_DECIMAL_DISPLAY_LOCATION		13

#define DATE_DISPLAY_ROW_CORDINATE			0
#define DATE_DISPLAY_LOCATION				29
#define TIME_DISPLAY_ROW_CORDINATE			1
#define TIME_DISPLAY_LOCATION				33

#include <xc.h>
#include <util/delay.h>
#include "hardware/usi/twi.h"
#include "hardware/display/lcd.h"
#include "hardware/modules/dht11_sensor.h"
#include "hardware/modules/rtc_ds1307.h"
#include "hardware/debug/led_debugger.h"
#include "other/converters/itoa.h"

void displayInitialTemplate();
void displayAnimator(uint8_t * direction);
void updateTempAndHumidDataOnDisplay();
void updateDateAndTimeDataOnDisplay();

int main(void)
{
	initLEDDebugger();
	twiInit();
	lcdInit();
	rtcInit();
	displayInitialTemplate();
	
	// This method should be only called for the time setting process. Then this should be commented
	// to avoid resetting the time after a power loss to the microcontroller.
	//rtcSetDateTimeOnce("2024-11-30T12:40");

	uint8_t displayLDCAnimatorDirection = 0; // Initial direction (0 => left, 1 => right)
	while(1)
	{
		updateTempAndHumidDataOnDisplay();
		updateDateAndTimeDataOnDisplay();
		displayAnimator(&displayLDCAnimatorDirection);
		hearbeat();
	}
}

void displayInitialTemplate() {
	lcdSetCursorAt(0, 0);
	lcdPrint("Humidity  00.0 %");
	lcdSetCursorAt(0, 23);
	lcdPrint("Date: 00-00-0000");
	lcdSetCursorAt(1, 0);
	lcdPrint("Temp      00.0ßC");
	lcdSetCursorAt(1, 23);
	lcdPrint("Time:     00:00H");
}

void updateTempAndHumidDataOnDisplay() {
	uint8_t * dat = getDHT11Data();
	char buffer[5];
	lcdSetCursorAt(HUMID_DISPLAY_ROW_CORDINATE, HUMID_INTEGRAL_DISPLAY_LOCATION);
	lcdPrint(citoa(*dat, buffer, 10));
	lcdSetCursorAt(HUMID_DISPLAY_ROW_CORDINATE, HUMID_DECIMAL_DISPLAY_LOCATION);
	lcdPrint(citoa(*(dat + 1), buffer, 10));
	lcdSetCursorAt(TEMP_DISPLAY_ROW_CORDINATE, TEMP_INTEGRAL_DISPLAY_LOCATION);
	lcdPrint(citoa(*(dat + 2), buffer, 10));
	lcdSetCursorAt(TEMP_DISPLAY_ROW_CORDINATE, TEMP_DECIMAL_DISPLAY_LOCATION);
	lcdPrint(citoa(*(dat + 3), buffer, 10));
}

void updateDateAndTimeDataOnDisplay() {
	char dateBuffer[] = "00-00-2000";
	char timeBuffer[] = "00:00";
	rtcProcessDateTime(dateBuffer, timeBuffer);
	lcdSetCursorAt(DATE_DISPLAY_ROW_CORDINATE, DATE_DISPLAY_LOCATION);
	lcdPrint(dateBuffer);
	lcdSetCursorAt(TIME_DISPLAY_ROW_CORDINATE, TIME_DISPLAY_LOCATION);
	lcdPrint(timeBuffer);
}

void displayAnimator(uint8_t * direction) {
	for (unsigned char i = 0; i < 23; i++)
	{
		_delay_ms(20);
		if (*direction == 0)
		{
			lcdShiftDisplay(DISPLAY_SHIFT_LEFT);
			} else {
			lcdShiftDisplay(DISPLAY_SHIFT_RIGHT);
		}
	}
	*direction ^= (1 << 1);
	_delay_ms(2000);
}