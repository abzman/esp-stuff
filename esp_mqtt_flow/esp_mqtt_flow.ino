/* esp_mqtt_flow - ESP8266 mqtt node with 2 flow sensors as inputs

   Based on DHTServer(ESP8266Webserver, DHTexample, and BlinkWithoutDelay), mqtt_publish_in_callback  (thank you)

   Version 1.0  9/23/2015  Evan Allen
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>



int flowPin1 = 5;
int flowPin2 = 4;
unsigned long flow1count = 0;
unsigned long flow2count = 0;


#define countof(a) (sizeof(a) / sizeof(a[0]))

//char ssid[] = "abz-fi";  //  your network SSID (name)
//char password[] = "we do what we must, because we can";       // your network password
const char* ssid     = "i3";
const char* password = "";


// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 136);

PubSubClient client(server);


String webString = "";   // String to display

// Callback function
void callback(const MQTT::Publish& pub) {
  if(pub.payload_string().equals("flow1"))
  {
    webString = "Flow 2: " + String(flow1count/450.0) + " L"; // Arduino has a hard time with float to string
    client.publish("outTopic",webString);
  } else if(pub.payload_string().equals("flow2"))
  {
    webString = "Flow 2: " + String(flow2count/450.0) + " L";
    client.publish("outTopic",webString);               // send to someones browser when asked

      } else






Serial.print("Flow 1 in Liters: ");
Serial.print(flow1count/450.0);
Serial.print("\tFlow 2 in Liters: ");
Serial.print(flow2count/450.0);
Serial.print("\tFlow 1 in pulses: ");
Serial.print(flow1count);
Serial.print("\tFlow 2 in pulse: ");
Serial.println(flow2count);
delay(500);



      
  {
  }

  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
}

void setup()
{
Serial.begin(115200);

pinMode(flowPin1, INPUT_PULLUP);
attachInterrupt(flowPin1, flow1, CHANGE);
pinMode(flowPin2, INPUT_PULLUP);
attachInterrupt(flowPin2, flow2, CHANGE);



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
    client.publish("outTopic","flow boot up");
    client.subscribe("inTopic");
  }




}

void loop()
{
  client.loop();
}

void flow1()
{
flow1count +=1;
}

void flow2()
{
flow2count +=1;
}
