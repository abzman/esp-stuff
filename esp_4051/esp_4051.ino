int s0 = 4;
int s1 = 5;
void setup(){
pinMode(s0, OUTPUT);    // s0
pinMode(s1, OUTPUT);    // s1
Serial.begin(115200);
}

 //pin 1 is zoom
 //pin 2 is x
 //pin 0 is y

void loop () {

 

  for (int count=1; count<=3; count++) {   

    digitalWrite(s0, bitRead(count,0));

    digitalWrite(s1, bitRead(count,1));

 
Serial.print("pin :");
Serial.print(count,HEX);
Serial.print(" ");
    Serial.println(analogRead(A0));
 delay(300);

  }  

}

