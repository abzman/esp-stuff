/*
  Shift Register Example
  Turning on the outputs of a 74HC595 using an array

 Hardware:
 * 74HC595 shift register 
 * LEDs attached to each of the outputs of the shift register

 */
int MR = 16;
int STCP = 15;
int PL = 2;
int SHCP = 14;
int Q = 12;
int DO = 13;

//holders for infromation you're going to pass to shifting function
byte data;
byte dataIn;
byte dataArray[10];
byte errorLED = 0x01;
byte runLED = 0x02;
byte rstLED = 0x04;
byte crsLED = 0x08;
byte fineLED = 0x10;
#include "SPI.h"

void setup() {
SPI.begin();
SPI.setBitOrder(MSBFIRST);             
  pinMode(MR, OUTPUT);     
  pinMode(STCP, OUTPUT);     
  pinMode(PL, OUTPUT);     
  pinMode(SHCP, OUTPUT); 
  pinMode(Q, INPUT); 
  pinMode(DO, OUTPUT); 
  digitalWrite(MR, 1);   
  digitalWrite(STCP, 0);   
  digitalWrite(PL, 1);   
  digitalWrite(SHCP, 0);   
  digitalWrite(DO, 0);   
  Serial.begin(115200);

  //Binary notation as comment
  dataArray[0] = errorLED | runLED;
  dataArray[1] = rstLED | crsLED;
  dataArray[2] = fineLED | errorLED;
  dataArray[3] = crsLED;
  dataArray[4] = rstLED;
}




void loop() {

  for (int j = 0; j < 5; j++) {
    data = dataArray[j];
    dataIn = 0x00;
    digitalWrite(STCP, LOW);
 /* digitalWrite(PL, 0);
  digitalWrite(PL, 1);
  dataIn = dataIn | digitalRead(Q);
    digitalWrite(DO, data & 0b00000001);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>1;
    digitalWrite(DO, data & 0b00000010);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>2;
    digitalWrite(DO, data & 0b00000100);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>3;
    digitalWrite(DO, data & 0b00001000);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>4;
    digitalWrite(DO, data & 0b00010000);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>5;
    digitalWrite(DO, data & 0b00100000);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>6;
    digitalWrite(DO, data & 0b01000000);
  digitalWrite(SHCP, 1);
  digitalWrite(SHCP, 0);
  dataIn = dataIn | digitalRead(Q)>>7;
    digitalWrite(DO, data & 0b10000000);*/
    dataIn = SPI.transfer(data);
    digitalWrite(STCP, HIGH);
    Serial.print("spi transfer in: ");
    Serial.println (dataIn,HEX);
    delay(1000);
  }
}




