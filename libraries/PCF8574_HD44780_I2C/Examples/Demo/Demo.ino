/*
 * Demonstration of most of the available functions 
 * in the PCF8574_HD44780_I2C library
 */
#include <Wire.h> 
#include <PCF8574_HD44780_I2C.h>

//  Arrays with user generated symbols
uint8_t bell[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t note[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t duck[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t retarrow[8] = {	0x1,0x1,0x5,0x9,0x1f,0x8,0x4};

// Address 0x27, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd(0x27,16,2);


void setup(){
  lcd.init();                   
  lcd.backlight();    

  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, retarrow);

  lcd.clear(); 
  lcd.setCursor(3,0);
  lcd.print("Hello");
  lcd.setCursor(7,1);
  lcd.print("World!"); delay(2000);

  lcd.clear(); 
  lcd.setCursor(1,0);
  lcd.print("Starting Demo"); delay(2000);



}
void loop(){

// Cursor control
  lcd.clear();
  lcd.setCursor(0,0);  lcd.print("Cursor");  
  lcd.setCursor(0,1);  lcd.print("Control");  delay(1500);

  lcd.clear();
  lcd.setCursor(0,0);  lcd.print("Cursor");  
  lcd.setCursor(0,1);  lcd.print("off");  delay(1500);

  lcd.clear();
  lcd.cursor();
  lcd.setCursor(0,0);  lcd.print("Cursor");  
  lcd.setCursor(0,1);  lcd.print("on");  delay(1500);
  lcd.noCursor();
  
  lcd.clear();
  lcd.blink();
  lcd.setCursor(0,0);  lcd.print("Cursor");  
  lcd.setCursor(0,1);  lcd.print("blink");  delay(1500);
  lcd.noBlink();
  
// Display control
  lcd.clear();         
  lcd.setCursor(0,0);  lcd.print("Display");  
  lcd.setCursor(0,1);  lcd.print("off");  delay(1500);
  lcd.noDisplay();     delay(1500);
  
  lcd.display();
  lcd.setCursor(0,0);  lcd.print("Display");  
  lcd.setCursor(0,1);  lcd.print("on ");  delay(3000);

// Scroll left
  lcd.clear();         
  lcd.setCursor(5,0);  lcd.print("Scroll");  
  lcd.setCursor(6,1);  lcd.print("left");  delay(1500);
  for(int k=0;k<16;k++){
    lcd.scrollDisplayLeft();
    delay(200);
  }
  delay(1000);
  
// Scroll right
  lcd.clear();
  lcd.home();  
  lcd.setCursor(5,0);  lcd.print("Scroll");  
  lcd.setCursor(6,1);  lcd.print("right");  delay(1500);
  for(int k=0;k<16;k++){
    lcd.scrollDisplayRight();
    delay(200);
  }
  delay(1000);

// Autoscroll
  lcd.clear();
  lcd.home();  
  lcd.setCursor(0,0);  lcd.print("Auto");  
  lcd.setCursor(0,1);  lcd.print("scroll");  delay(1500);
  lcd.clear();
  lcd.autoscroll();
  lcd.setCursor(13,1);
  char msg1[]="AutoScroll";
  for(int h=0;h<10;h++){
  lcd.print(msg1[h]);
  delay(200);
  }
  delay(1000);
  lcd.noAutoscroll();


// No autoscroll
  lcd.clear();
  lcd.home();  
  lcd.setCursor(0,0);  lcd.print("No Auto");  
  lcd.setCursor(0,1);  lcd.print("scroll");  delay(1500);
  lcd.clear();
  lcd.setCursor(1,1);
  char msg2[]="No_AutoScroll";
  for(int h=0;h<13;h++){
  lcd.print(msg2[h]);
  delay(200);
  }
  delay(1000);

  
// User chars
  lcd.clear();         
  lcd.setCursor(0,0);  lcd.print("User");  
  lcd.setCursor(0,1);  lcd.print("chars");  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);  
  for(uint8_t cr = 0; cr < 8; cr++){
    lcd.write(cr);
  }
  delay(4000);

  
// Display all keycodes
  uint8_t i = 0;
  while (1) {
    lcd.clear();
    lcd.print("Codes 0x"); lcd.print(i, HEX);
    lcd.print("-0x"); lcd.print(i+16, HEX);
    lcd.setCursor(0, 1);
    for (int j=0; j<16; j++) {
      lcd.write(i+j);
    }
    i+=16;
    
    delay(4000);
  }
}

