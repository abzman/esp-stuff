/* esp_mqtt_dht - ESP8266 mqtt node with a DHT sensor as an input

   Based on DHTServer(ESP8266Webserver, DHTexample, and BlinkWithoutDelay), mqtt_publish_in_callback  (thank you)

   Version 1.0  9/23/2015  Evan Allen
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>


#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN  5

#define countof(a) (sizeof(a) / sizeof(a[0]))

//char ssid[] = "abz-fi";  //  your network SSID (name)
//char password[] = "we do what we must, because we can";       // your network password
const char* ssid     = "i3";
const char* password = "";


// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 136);

PubSubClient client(server);

// Initialize DHT sensor
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float humidity, temp_f;  // Values read from sensor
String webString = "";   // String to display
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor



// Callback function
void callback(const MQTT::Publish& pub) {
  if(pub.payload_string().equals("temp"))
  {
    gettemperature();       // read sensor
    webString = "Temperature: " + String((int)temp_f) + " F"; // Arduino has a hard time with float to string
    client.publish("outTopic",webString);
    client.publish("displayTopic","TEMPERATURE " + String((int)temp_f) + " F");
  } else if(pub.payload_string().equals("hum"))
  {
       gettemperature();           // read sensor
    webString = "Humidity: " + String((int)humidity) + "%";
    client.publish("outTopic",webString);               // send to someones browser when asked
    client.publish("displayTopic","HUMIDITY " + String((int)humidity));               // send to someones browser when asked

      } else
  {
  }

  Serial.print(pub.topic());
  Serial.print(" => ");
  Serial.println(pub.payload_string());
}
void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  dht.begin();           // initialize temperature sensor
  
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
    client.publish("outTopic","dht boot up");
    client.subscribe("inTopic");
  }

}

void loop(void)
{
  client.loop();
  
  //send things periodically
  
}


void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_f = dht.readTemperature(true);     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}
