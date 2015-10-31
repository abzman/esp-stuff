#include <Wire.h> 
#include <PCF8574_HD44780_I2C.h>
 
// First LCD - Address 0x27, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd1(0x27,16,2);
// Second LCD - Address 0x20, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd2(0x20,16,2);
 
 
void setup()
{
  lcd1.init();    // initialize the first lcd 
  lcd2.init();    // initialize the second lcd 
 
  // Print a message on the first LCD.
  lcd1.backlight();
  lcd1.print("Hello, #1 world!");
 
  // Print a message on the second LCD.
  lcd2.backlight();
  lcd2.print("Hello, #2 world!");
 
}
 
void loop()
{
}
