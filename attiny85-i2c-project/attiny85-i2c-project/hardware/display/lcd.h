/*
* lcd.h
*
* Created: 11/16/2024 1:12:19 PM
* Author: Buddhika
* PCF8574 to LCD Pin map.
P7	P6	P5	P4	P3	P2	P1	P0
D7	D6	D5	D4	LED	E	R/W	RS

RS	:	Selects registers
0: Instruction register (for write) Busy flag:address counter (for read))
1: Data register (for write and read)
R/W	:	Selects read or write.
0: Write
1: Read
E	:	Starts data read/write.
*/


#ifndef LCD_H_
#define LCD_H_

// LCD mode selection
#define COMMAND_MODE					0B00000000
#define DATA_MODE						0B00000001
#define ENABLE_OPERATION				0B00000100
#define DISABLE_OPERATION				0B00000000
// LCD clear command
#define CLEAER_LCD						0B00000001
// LCD return home mode
#define RETURN_HOME						0B00000010
// LCD entry mode selection
#define CURSOR_INCREMENT				0B00000110
#define CURSOR_DECREMENT				0B00000100
#define SHIFT_DISPLAY					0B00000001
// LCD display controller commands
#define DISPLAY_ON						0B00001100
#define CURSOR_ON						0B00000010
#define CURSOR_BLINK					0B00000001
// LCD cursor of display shift
#define	CURSOR_SHIFT_LEFT				0B00010000
#define	CURSOR_SHIFT_RIGHT				0B00010100
#define	DISPLAY_SHIFT_LEFT				0B00011000
#define	DISPLAY_SHIFT_RIGHT				0B00011100
// LCD Mode selection commands
#define SET_4BIT_MODE					0B00000010
#define SET_4BIT_AND_2LINE_MODE			0B00101000
// LCD set display row/column number
#define CG_RAM_ADDR_TEMPLATE			0B01000000
#define DD_RAM_ADDR_TEMPLATE			0B10000000
#define FIRST_LINE_START_AT_1602		0B10000000
#define SECOND_LINE_START_AT_1602		0B11000000
// LCD back light config
#define LCD_BACKLIGHT_ON				0X08
#define LCD_BACKLIGHT_OFF				0XF7

#include <util/delay.h>

#ifndef TWI_H_
#error "LCD library require wire.h. Please include!"
#endif

#ifndef LCD_ADDRESS
#define LCD_ADDRESS	0X4E
#error "LCD_ADDRESS must be defined!"
#endif

unsigned char lcdBacklightStatus;

void lcdBacklight(unsigned char status) {
	lcdBacklightStatus = status;
}

static void lcdEnablePulse() {
	_delay_ms(1);
	twiStart();
	twiWrite(LCD_ADDRESS | WIRE_WRITE_MODE);
	twiWrite(DISABLE_OPERATION | lcdBacklightStatus);
	twiStop();
	_delay_ms(1);
}

void lcdCommand(unsigned char command, unsigned char instruction) {
	command |= lcdBacklightStatus;
	twiStart();
	twiWrite(LCD_ADDRESS | WIRE_WRITE_MODE);
	//twiWrite((instruction & 0XF0) | command);
	twiWrite((instruction & 0XF0) | command | ENABLE_OPERATION);
	twiStop();
	lcdEnablePulse();
	twiStart();
	twiWrite(LCD_ADDRESS | WIRE_WRITE_MODE);
	//twiWrite((instruction << 4) | command);
	twiWrite((instruction << 4) | command | ENABLE_OPERATION);
	twiStop();
	lcdEnablePulse();
}

void lcdInit() {
	lcdBacklight(LCD_BACKLIGHT_ON);
	lcdCommand(COMMAND_MODE, SET_4BIT_MODE);
	lcdCommand(COMMAND_MODE, SET_4BIT_AND_2LINE_MODE);
	lcdCommand(COMMAND_MODE, CLEAER_LCD);
	lcdCommand(COMMAND_MODE, RETURN_HOME);
	lcdCommand(COMMAND_MODE, CURSOR_INCREMENT);
	//lcdCommand(COMMAND_MODE, DISPLAY_ON | CURSOR_ON | CURSOR_BLINK);
	lcdCommand(COMMAND_MODE, DISPLAY_ON);
	//lcdCommand(COMMAND_MODE, DD_RAM_ADDR_TEMPLATE | SECOND_LINE_START_AT_1602);
	//lcdCommand(DATA_MODE, 'a');
}

void lcdSetCursorAt(unsigned char row, unsigned char col) {
	if (row == 0)
	{
		lcdCommand(COMMAND_MODE, DD_RAM_ADDR_TEMPLATE | col);
	} else {
		col += SECOND_LINE_START_AT_1602;
		lcdCommand(COMMAND_MODE, DD_RAM_ADDR_TEMPLATE | col);
	}
}

void lcdWrite(unsigned char lcdWrite) {
	lcdCommand(DATA_MODE, lcdWrite);
}

void lcdPrint(const char * stream) {
	while(*stream) {
		lcdCommand(DATA_MODE, *stream);
		stream++;
	}
}

void lcdShiftDisplay(unsigned char direction) {
	lcdCommand(COMMAND_MODE, direction);
}

#endif /* LCD_H_ */