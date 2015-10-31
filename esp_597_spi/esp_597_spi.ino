/*
  Set DS to 0 (output) can be hardwired
Set MR to 1 (output) can be hardwired
Set STCP to 0 (output) needs to be toggled
Set PL to 1 (output) needs to be toggled
Set SHCP to 0 (output) needs to be toggled

to load data
Set STCP to 1
delay 1us
Set STCP to 0 (load flip flop)
delay 1us
Set PL to 0
delay 1us
Set PL to 1 (parallel load)

now bit 7 is already available for read
toggle clock SHCP (to 1 and then again to 0)
now bit 6 is available for read
toggle clock SHCP (to 1 and then again to 0)
 */
int MR = 16;
int STCP = 15;
int PL = 2;
int SHCP = 14;
int Q = 12;

#include "SPI.h"
// the setup routine runs once when you press reset:
void setup() {     
  
SPI.begin();
SPI.setBitOrder(MSBFIRST);          
  pinMode(MR, OUTPUT);     
  pinMode(STCP, OUTPUT);     
  pinMode(PL, OUTPUT);     
  pinMode(SHCP, OUTPUT); 
  pinMode(Q, INPUT); 
  digitalWrite(MR, 1);   
  digitalWrite(STCP, 0);   
  digitalWrite(PL, 1);   
  digitalWrite(SHCP, 0);   
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(STCP, 1);
  delayMicroseconds(1);
  digitalWrite(STCP, 0);
  delayMicroseconds(1);
  digitalWrite(PL, 0);
  delayMicroseconds(1);
  digitalWrite(PL, 1);
  delayMicroseconds(1);
  Serial.print("bit pattern: ");
  Serial.println(SPI.transfer(0x01), BIN);
}
