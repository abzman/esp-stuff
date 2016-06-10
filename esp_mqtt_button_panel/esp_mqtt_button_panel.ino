/* esp_mqtt_button_panel - ESP8266 mqtt node with a button panel hooked up to it, it uses a 4021, 74595, and 4051 to multiplex the needed inputs

   Based on mqtt_basic, http://www.sinneb.net/?p=421  (thank you)

   Version 1.0  2/22/2016  Evan Allen
*/






#include <ESP8266WiFi.h>
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
boolean rst;
boolean crs;
boolean fine;

const char *ssid =  "i3";   // cannot be longer than 32 characters!
const char *pass ="";   //

// Update these with values suitable for your network.
IPAddress server(10,13,0,136);

WiFiClient wclient;
PubSubClient client(wclient, server);

String webString = "";
// Callback function
void callback(const MQTT::Publish& pub) {
  if(pub.payload_string().equals("buttons"))
  {
    webString = "buttons: " + String((int)buttons8);
    client.publish("outTopic",webString);
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
    trans = pub.payload_string().toInt();
    Serial.println(int(trans));
  }
/*
  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
  */
}


void setup() {

//Wire.begin(0, 2);
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  client.set_callback(callback);
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

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("arduinoClient")) {
  client.publish("outTopic","button panel reconnected");
  client.subscribe("inTopic");
      }
    }

    if (client.connected())
      client.loop();
  }


  // SS1 = HIGH -> 4021 is gathering data from parallel inputs

  // select 595
  //digitalWrite(PIN_595_1, LOW);

  // send BIN number to 595 to light 1 LED (not necessarily the 1 example LED on the schematic)
  //SPI.transfer(trans);
  /*
  if (trans == 0xff)
  {
    trans = 0x00;
  } else {
    trans++;
  }*/
  // deselect 595
  //digitalWrite(PIN_595_1, HIGH);

  // select 4021
  digitalWrite(PIN_SS1, LOW);
  // select 595
  digitalWrite(PIN_595_1, LOW);
  // read CD4021 IC
  buttons8 = SPI.transfer(trans);
  buttons8 = buttons8 | 0b10000001;

if(buttons8 != (buttons8 | 0b00001000))//reset pressed
{
    client.publish("outTopic","rst");
  Serial.println("rst");
} 
if(buttons8 != (buttons8 | 0b00000100))//crs pressed
{
    client.publish("outTopic","crs");
  Serial.println("crs");
}
if(buttons8 != (buttons8 | 0b00000010))
{
 client.publish("outTopic","fine");
  Serial.println("fine");
}

  Serial.println(buttons8, BIN);
  delay(100);
  
  // deselect 4021
  digitalWrite(PIN_SS1, HIGH);
  // deselect 595
  digitalWrite(PIN_595_1, HIGH);
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
