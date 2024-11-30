/*
* rtc_ds1307.h
*
* Created: 11/23/2024 10:42:10 PM
*  Author: Buddhika
*/


#ifndef RTC_DS1307_H_
#define RTC_DS1307_H_

#ifndef DS1307_RTC_ADDRESS
#define DS1307_RTC_ADDRESS
#error "DS1307_RTC_ADDRESS is reaquired to use rtc_ds1307.h"
#endif

#ifndef TWI_H_
#error "LCD library require wire.h. Please include!"
#endif

#define MINUTES						0X01
#define HOURS						0X02
#define DATE						0X04
#define MONTH						0X05
#define YEAR						0X06
#define CONTROL_REGISTER			0X07
#define DATE_TIME_SET_SRAM_FLAG		0X08
#define OSCILLATOR_REGISTRER		0X00
#define START_OSCILLATOR			0X00
#define CONTROL_REGISTER_CONFIG		0X00

static void rtcWriteData(unsigned char memoryLocation, unsigned char data) {
	twiStart();
	twiWrite(DS1307_RTC_ADDRESS | WIRE_WRITE_MODE);
	twiWrite(memoryLocation);
	twiWrite(data);
	twiStop();
}

void rtcReadData(uint8_t memoryAddress, uint8_t numberOfBytes, uint8_t * byteBuffer) {
	unsigned char nack = 0X00; // Continue for number of bytes mode
	twiStart();
	twiWrite(DS1307_RTC_ADDRESS);
	twiWrite(memoryAddress);
	twiRestart();
	twiWrite(DS1307_RTC_ADDRESS | WIRE_READ_MODE);
	
	for (uint8_t i = 0; i < numberOfBytes; i++)
	{
		if (i == (numberOfBytes - 1))
		{
			nack = 0XFF;
		}
		byteBuffer[i] = (uint8_t) twiRead(nack);
	}
	twiStop();
}

void rtcInit() {
	rtcWriteData(CONTROL_REGISTER, CONTROL_REGISTER_CONFIG);
	rtcWriteData(OSCILLATOR_REGISTRER, START_OSCILLATOR);
}

static unsigned char stringToBCD(const char * stringVal) {
	unsigned char bcd = 0;
	for (uint8_t i = 0; i < 2; i++)
	{
		unsigned char val = (uint8_t) (*stringVal) - 48; // This step is to get integer value of string number.
		if (i == 0)
		{
			bcd = val;
			bcd = (bcd << 4);
			} else {
			bcd |= val;
		}
		stringVal++;
	}
	return bcd;
}

static void generateBCDYear(const char * stringlet) {
	stringlet = stringlet + 2; // Advance the address to get last two digits of the year part.
	rtcWriteData(YEAR, stringToBCD(stringlet));
}

static void generateBCDMonth(const char * stringlet) {
	stringlet = stringlet + 5; // Advance the address to get month digits of the month part.
	rtcWriteData(MONTH, stringToBCD(stringlet));
}

static void generateBCDDate(const char * stringlet) {
	stringlet = stringlet + 8; // Advance the address to get date digits of the date part.
	rtcWriteData(DATE, stringToBCD(stringlet));
}

static void generateBCDHours(const char * stringlet) {
	stringlet = stringlet + 11; // Advance the address to get last two digits of the year part.
	rtcWriteData(HOURS, stringToBCD(stringlet));
}

static void generateBCDMinutes(const char * stringlet) {
	stringlet = stringlet + 14; // Advance the address to get last two digits of the year part.
	rtcWriteData(MINUTES, stringToBCD(stringlet));
}

static void updateSRAMToAvoidDateTimeUpdateAtNextRestart() {
	rtcWriteData(DATE_TIME_SET_SRAM_FLAG, 0XF0);
}

void rtcSetDateTimeOnce(const char * dateTimeString) {
	// DateTime String format: 2024-07-27T14:17:20
	generateBCDYear(dateTimeString);
	generateBCDMonth(dateTimeString);
	generateBCDDate(dateTimeString);
	generateBCDHours(dateTimeString);
	generateBCDMinutes(dateTimeString);
	updateSRAMToAvoidDateTimeUpdateAtNextRestart();
}

void rtcProcessDateTime(char * dateBuffer, char * timeBuffer) {
	// Read Buffer data sequence -> {MM, HH, DAY, dd, mm, yy}
	unsigned char readBuffer[6];
	rtcReadData(MINUTES, 6, readBuffer);
	// Format Time
	timeBuffer[0] = (char)((readBuffer[1] >> 4) + 48);
	timeBuffer[1] = (char)((readBuffer[1] & 0X0F) + 48);
	timeBuffer[3] = (char)((readBuffer[0] >> 4) + 48);
	timeBuffer[4] = (char)((readBuffer[0] & 0X0F) + 48);
	// Format Date
	dateBuffer[0] = (char)((readBuffer[3] >> 4) + 48);
	dateBuffer[1] = (char)((readBuffer[3] & 0X0F) + 48);
	dateBuffer[3] = (char)((readBuffer[4] >> 4) + 48);
	dateBuffer[4] = (char)((readBuffer[4] & 0X0F) + 48);
	dateBuffer[8] = (char)((readBuffer[5] >> 4) + 48);
	dateBuffer[9] = (char)((readBuffer[5] & 0X0F) + 48);
}

#endif /* RTC_DS1307_H_ */