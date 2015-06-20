/* esp_mqtt_display_01 - ESP8266 connected to a 4800bps serial led display (16 digit) for the esp-01 (no debug output, using the serial line to talk to the display)zzzz

   Based on ESP8266Webserver,mqtt_publish_in_callback   (thank you)

   Version 1.0  6/20/2015  Evan Allen
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

//char ssid[] = "abz-fi";  //  your network SSID (name)
//char password[] = "we do what we must, because we can";       // your network password
const char* ssid     = "i3";
const char* password = "";

// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 202);

PubSubClient client(server);

String webString = "";   // String to display
String tempString = "";   // String to temporarily store stuff
int character = 0;
int d = 500;
// Callback function
void callback(const MQTT::Publish& pub) {
  if (pub.topic().equals("delayTopic"))
  {
    char buffer[] = ""; 
    pub.payload_string().toCharArray(buffer,pub.payload_string().length()+1);
    d = atoi(buffer);
  }
  if (pub.topic().equals("displayTopic"))
  {
    tempString = "";
    tempString += pub.payload_string();

    if (tempString.length() < 16)
    {
      webString = "";
      for (int i = 0; i < 16 - tempString.length(); i++)
      {
        webString += " ";
      }
      webString += tempString;
    } else
    {
      webString = "";
      webString += tempString;

    }
    for (int i = 0; i < 16; i++)
    {
      Serial.print(webString.charAt(i));
    }
    character = 16;
  }
}

void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(4800);  // Serial connection to display
  client.set_callback(callback);
  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("IP ADDR ");
  Serial.print(WiFi.localIP());

  if (client.connect("arduinoClient")) {
    client.publish("outTopic", "hello world");
    client.subscribe("displayTopic");
    client.subscribe("delayTopic");
  }

}

void loop(void)
{
  client.loop();
  if (tempString.length() > 16)
  {

    Serial.print(webString.charAt(character));
      character++;
    if (webString.length() < character+1)
    {
      character = 0;
    }
  }
  delay(d);
}
