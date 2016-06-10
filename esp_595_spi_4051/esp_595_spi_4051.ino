#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>






#include <SPI.h>

#define PIN_595_1        2              // SPI 74HC595
#define s0               4              //4051 s0
#define s1               5              //4051 s1
#define PIN_MISO         12             // SPI data input
#define PIN_MOSI         13             // SPI data output
#define PIN_SCK          14             // SPI clock
#define PIN_SS1          15             // SPI hardware default SS pin, 4021

// result byte for 4021
byte buttons8;

byte trans;

// global vars for button timeout and debounce
long button1timeout = 0;
long button2timeout = 0;
long debounce = 200;



#define countof(a) (sizeof(a) / sizeof(a[0]))

//char ssid[] = "abz-fi";  //  your network SSID (name)
//char password[] = "we do what we must, because we can";       // your network password
const char* ssid     = "i3";
const char* password = "";


// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 136);

PubSubClient client(server);

// Callback function
void callback(const MQTT::Publish& pub) {
  if(pub.payload_string().equals("button"))
  {
    client.publish("outTopic",buttons8);
    //client.publish("displayTopic","TEMPERATURE " + String((int)temp_f) + " F");
  } else if(pub.payload_string().equals("a0"))
  {
    digitalWrite(s0, 0);
    digitalWrite(s1, 0);// read sensor
    webString = "a0: " + String(analogRead(A0));
    client.publish("outTopic",webString);
    //client.publish("displayTopic","HUMIDITY " + String((int)humidity));

      } else if(pub.payload_string().equals("a1"))
  {
    digitalWrite(s0, 1);
    digitalWrite(s1, 0);// read sensor
    webString = "a1: " + String(analogRead(A0));
    client.publish("outTopic",webString);
    //client.publish("displayTopic","HUMIDITY " + String((int)humidity));

      } else if(pub.payload_string().equals("a2"))
  {
    digitalWrite(s0, 0);
    digitalWrite(s1, 1);// read sensor
    webString = "a2: " + String(analogRead(A0));
    client.publish("outTopic",webString);
    //client.publish("displayTopic","HUMIDITY " + String((int)humidity));

      } else
  {
    trans = pub.payload_string();
  }
/*
  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
  */
}


void setup() {

//Wire.begin(0, 2);
  
  Serial.begin(115200);
  SPI.begin();
  // set all IC select pins HIGH
  pinMode(PIN_SS1, OUTPUT);
  digitalWrite(PIN_SS1, HIGH);
  pinMode(PIN_595_1, OUTPUT);
  digitalWrite(PIN_595_1, HIGH);
pinMode(s0, OUTPUT);    // s0
  digitalWrite(s0, HIGH);
pinMode(s1, OUTPUT);    // s1
  digitalWrite(s1, HIGH);



  client.set_callback(callback);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Esp General Purpose Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  

  
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","button panel boot up");
    client.subscribe("inTopic");
  }




  
}

void loop() {

  client.loop();
  // SS1 = HIGH -> 4021 is gathering data from parallel inputs

  // select 595
  digitalWrite(PIN_595_1, LOW);

  // send BIN number to 595 to light 1 LED (not necessarily the 1 example LED on the schematic)
  SPI.transfer(trans);
  /*
  if (trans == 0xff)
  {
    trans = 0x00;
  } else {
    trans++;
  }*/
  // deselect 595
  digitalWrite(PIN_595_1, HIGH);

  // select 4021
  digitalWrite(PIN_SS1, LOW);
  // read CD4021 IC
  buttons8 = SPI.transfer(0x00);
  buttons8 = buttons8 | 0b10000001;

  Serial.println(buttons8, BIN);
  delay(100);

  /*
    // button functions and debounces
    // needs refactoring for smaller footprint
    if((B10000000 & buttons8) && (millis() - button1timeout > debounce)) {
    Serial.println("but1");
    button1timeout = millis();
    }
    if((B01000000 & buttons8) && (millis() - button2timeout > debounce)) {
    Serial.println("but2");
    button2timeout = millis();
    }
  */
  // deselect 4021
  digitalWrite(PIN_SS1, HIGH);

 //pin 1 is zoom
 //pin 2 is x
 //pin 0 is y
/*
  for (int count = 1; count <= 3; count++) {

    digitalWrite(s0, bitRead(count, 0));

    digitalWrite(s1, bitRead(count, 1));


    Serial.print("pin :");
    Serial.print(count, HEX);
    Serial.print(" ");
    Serial.print(analogRead(A0));
    Serial.print(" ");
  }
  Serial.println();
*/

}
