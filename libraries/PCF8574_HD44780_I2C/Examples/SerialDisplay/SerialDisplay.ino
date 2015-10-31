/*
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 */
#include <Wire.h> 
#include <PCF8574_HD44780_I2C.h>


// Address 0x27, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd(0x27,16,2);

void setup()
{
  lcd.init();           // LCD Initialization              
  lcd.backlight();      // Backlight ON
  lcd.clear();          // Clear the display
  Serial.begin(115200); // You need to set this baudrate on serial monitor 
}

void loop()
{
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      lcd.write(Serial.read());
    }
  }
}
