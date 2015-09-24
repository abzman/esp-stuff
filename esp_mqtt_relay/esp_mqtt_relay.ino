/* esp_mqtt_relay - ESP8266 mqtt node with a bistable relay

   Based on mqtt_publish_in_callback  (thank you)

   Version 1.0  9/23/2015  Evan Allen
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>


#define ONPIN  14
#define OFFPIN  12
#define countof(a) (sizeof(a) / sizeof(a[0]))
boolean relayState = 0;

const char* ssid     = "i3";
const char* password = "";

String webString = "";   // String to display

// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 136);

PubSubClient client(server);


// Callback function
void callback(const MQTT::Publish& pub) {
  if (pub.payload_string().equals("relay on"))
  {
    if (relayState) {
      webString = "The Relay is already on";
    } else {
      digitalWrite(ONPIN, 1);
      delay(5);
      digitalWrite(ONPIN, 0);
      relayState = 1;
      webString = "The Relay has been turned on";
    }
    client.publish("outTopic", webString);              // send to someones browser when asked


  } else if (pub.payload_string().equals("relay off"))
  {
    if (relayState) {
      digitalWrite(OFFPIN, 1);
      delay(5);
      digitalWrite(OFFPIN, 0);
      relayState = 0;
      webString = "The Relay has been turned off";
    } else {
      webString = "Relay is already off";
    }
    client.publish("outTopic", webString);              // send to someones browser when asked


  } else if (pub.payload_string().equals("relay state"))
  {
    if (relayState) {
      webString = "Relay is on";
    } else {
      webString = "Relay is off";
    }
    client.publish("outTopic", webString);              // send to someones browser when asked


  }
  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
}
void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  pinMode(ONPIN, OUTPUT);
  digitalWrite(ONPIN, 0);
  pinMode(OFFPIN, OUTPUT);
  digitalWrite(OFFPIN, 0);

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
    client.publish("outTopic", "relay boot up");
    client.subscribe("inTopic");
  }

}

void loop(void)
{

  client.loop();

  //send things periodically

}
