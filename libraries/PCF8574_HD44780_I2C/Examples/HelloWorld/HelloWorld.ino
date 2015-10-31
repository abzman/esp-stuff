#include <Wire.h> 
#include <PCF8574_HD44780_I2C.h>


// Address 0x27, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd(0x27,16,2);


void setup()
{
  lcd.init();           // LCD Initialization              
  lcd.backlight();      // Backlight ON
  lcd.clear();          // Clear the display
  
  lcd.setCursor(0,0);   // Set the cursor to col 0, row 0
  lcd.print("Hello");   // Print the first word
  lcd.setCursor(0,1);   // Set the cursor to col 0, row 1
  lcd.print("World!");  // Print the second word
}

void loop()
{
}
