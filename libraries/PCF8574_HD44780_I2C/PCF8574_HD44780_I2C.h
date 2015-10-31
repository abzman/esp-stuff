/*********************************
PCF8574_HD44780_I2C
Library for PCF8574 Breakout Board
by Testato on ArduinoForum

based on code from:
- Mario H.
**********************************/


#ifndef PCF8574_HD44780_I2C_h
#define PCF8574_HD44780_I2C_h

#include <inttypes.h>
#include "Print.h" 
#include <Wire.h>

// Expander I/O port
#define P7	7
#define P6	6
#define P5	5
#define P4	4
#define P3	3
#define P2	2
#define P1	1
#define P0	0

// LCD signals
#define LCD_BACKLIGHT _BV(P3)		// Led ON
#define LCD_NOBACKLIGHT 0x00		// Led OFF
#define En _BV(P2)					// Enable
#define Rw _BV(P1)					// Read/Write
#define Rs _BV(P0)					// Register select

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYLEFT 0x02			// I/D Increment
#define LCD_ENTRYRIGHT 0x00			// I/D Decrement
#define LCD_ENTRYSHIFTINCREMENT 0x01// S Accompanies display shift
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04			// D
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02			// C
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01			// B
#define LCD_BLINKOFF 0x00

// flags for cursor/display shift
#define LCD_DISPLAYMOVE 0x08		// S/C Display shift
#define LCD_CURSORMOVE 0x00			// S/C Cursor move
#define LCD_MOVERIGHT 0x04			// R/L Shift to the right
#define LCD_MOVELEFT 0x00			// R/L Shift to the left

// flags for function set
#define LCD_8BITMODE 0x10			// DL
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08				// N
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04			// F
#define LCD_5x8DOTS 0x00




class PCF8574_HD44780_I2C : public Print {
public:
  PCF8574_HD44780_I2C(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS );
  void clear();
  void home();
  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void printLeft();
  void printRight();
  void leftToRight();
  void rightToLeft();
  void shiftIncrement();
  void shiftDecrement();
  void noBacklight();
  void backlight();
  void autoscroll();
  void noAutoscroll(); 
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  virtual size_t write(uint8_t);
  void command(uint8_t);
  void init();

////compatibility API function aliases
void blink_on();						// alias for blink()
void blink_off();       					// alias for noBlink()
void cursor_on();      	 					// alias for cursor()
void cursor_off();      					// alias for noCursor()
void setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
void printstr(const char[]);

////Unsupported API functions (not implemented in this library)
uint8_t status();
void setContrast(uint8_t new_val);
uint8_t keypad();
void setDelay(int,int);
void on();
void off();
uint8_t init_bargraph(uint8_t graphtype);
void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
	 

private:
  void init_priv();
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void expanderWrite(uint8_t);
  void pulseEnable(uint8_t);
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;
};

#endif
