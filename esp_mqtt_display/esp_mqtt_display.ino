/* esp_mqtt_display - ESP8266 connected to a 4800bps serial led display (16 digit)

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
IPAddress server(10, 13, 0, 136);

PubSubClient client(server);

String webString = "";   // String to display
String tempString = "";   // String to temporarily store stuff


// Callback function
void callback(const MQTT::Publish& pub) {

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

  Serial1.print(webString);
  }

  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
}

void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  Serial1.begin(4800);  // Serial connection to display
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
  Serial1.print("IP ADDR ");
  Serial1.print(WiFi.localIP());
  Serial.println();

  if (client.connect("arduinoClient")) {
    client.publish("outTopic", "hello world");
    client.subscribe("displayTopic");
  }

}

void loop(void)
{
  client.loop();
}
