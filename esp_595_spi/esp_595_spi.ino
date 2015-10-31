/*
 Example 34.1 - SPI bus demo using a Microchip MCP4162 digital potentiometer [http://bit.ly/iwDmnd]
 http://tronixstuff.com/tutorials > chapter 34 | CC by-sa-nc | John Boxall
*/

#include "SPI.h" // necessary library
int ss=15; // using digital pin 10 for SPI slave select
int del=200; // used for various delays

void setup()
{
  pinMode(ss, OUTPUT); // we use this for SS pin
  SPI.begin(); // wake up the SPI bus.
  SPI.setBitOrder(MSBFIRST);
  // our MCP4162 requires data to be sent MSB (most significant byte) first
}

void loop()
{
  for (int a=0; a<256; a++)
  {
      digitalWrite(ss, LOW);
  SPI.transfer(a); // send value (0~255)
  digitalWrite(ss, HIGH);
    delay(del);
  }
  for (int a=255; a>=0; --a)
  {
    digitalWrite(ss, LOW);
  SPI.transfer(a); // send value (0~255)
  digitalWrite(ss, HIGH);
    delay(del);
  }
}
