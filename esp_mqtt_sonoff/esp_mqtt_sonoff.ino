/*
  Basic ESP8266 MQTT example

  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.

  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#define BUTTON_PIN 0
#define LED_PIN 13
#define RELAY_PIN 12
#include <Bounce2.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

//needed for wfimanager
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

int ledState = LOW;
int relayState = LOW;

// Instantiate a Bounce object :
Bounce debouncer = Bounce();
Bounce debouncer2 = Bounce();

// Update these with values suitable for your network.

char ssid[32] = "i3detroit-wpa";
char password[32];
char mqtt_server[32] = "10.13.0.155";
char mqtt_topic[50] = "abzmanHome/House/Basement/North/Light/Control";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


void setup() {

  pinMode(RELAY_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  delay(500);
  //saveCredentials();
  loadCredentials();
  setup_wifi();


  byte mqtt_ip[4];
  parseBytes(mqtt_server, '.', mqtt_ip, 4, 10);



  client.setServer(mqtt_ip, 1883);
  client.setCallback(callback);

  // Setup the button with an internal pull-up :
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5000);

  // After setting up the button, setup the Bounce instance :
  debouncer2.attach(BUTTON_PIN);
  debouncer2.interval(5); // interval in ms

  // Setup the LED :
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (i > 15) {
      captivePortal();
    }
    delay(500);
    Serial.print(".");
    i++;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //Serial.println(strlen(topic));
  //Serial.println(topic[strlen(topic)-1]);
  //if ((char)payload[1] == 'f' && topic[strlen(topic)-1] == 'l') {//off
  if ((char)payload[1] == 'f') { //off
    relayState = LOW;
  } else if ((char)payload[1] == 'n') { //on
    relayState = HIGH;
  } else if ((char)payload[1] == 't') { //state



    client.publish(mqtt_topic, relayState ? "Lights are on" : "Lights are off");
    //client.publish("abzmanHome/House/Basement/North/Light", relayState ? "on" : "off");
  } else {
    client.publish("outTopic", "command not recognised");
  }

}

void reconnect() {
  // Loop until we're reconnected
  int i = 0;
  while (!client.connected()) {
    if (i > 5) {
      captivePortal();
    }
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "sonoff boot up");
      // ... and resubscribe
      client.subscribe(mqtt_topic);
      Serial.print("subscribed to ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      i++;
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Update the Bounce instance :
  debouncer.update();
  debouncer2.update();
  // Call code if Bounce fell (transition from HIGH to LOW) :
  if ( debouncer2.fell() ) {
    // Toggle Relay state :
    relayState = !relayState;
  }
  if ( debouncer.fell() ) {
    captivePortal();
  }
  digitalWrite(RELAY_PIN, relayState);

}

/** Load WLAN credentials from EEPROM */
void loadCredentials() {

  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0 + sizeof(ssid), password);
  EEPROM.get(0 + sizeof(ssid) + sizeof(password), mqtt_server);
  EEPROM.get(0 + sizeof(ssid) + sizeof(password) + sizeof(mqtt_server), mqtt_topic);
  char ok[2 + 1];
  EEPROM.get(0 + sizeof(ssid) + sizeof(password) + sizeof(mqtt_server) + sizeof(mqtt_topic), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password) > 0 ? "********" : "<no password>");
  Serial.println(mqtt_server);
  Serial.println(mqtt_topic);

}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0 + sizeof(ssid), password);
  EEPROM.put(0 + sizeof(ssid) + sizeof(password), mqtt_server);
  EEPROM.put(0 + sizeof(ssid) + sizeof(password) + sizeof(mqtt_server), mqtt_topic);
  char ok[2 + 1] = "OK";
  EEPROM.put(0 + sizeof(ssid) + sizeof(password) + sizeof(mqtt_server) + sizeof(mqtt_topic), ok);
  EEPROM.commit();
  EEPROM.end();

}


void captivePortal() {
  //WiFiManager

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic", mqtt_topic, 50);


  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();


  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);


  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_topic);

  //it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration

  //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
  //WiFi.mode(WIFI_STA);

  if (!wifiManager.startConfigPortal("sonoff config")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  client.subscribe(mqtt_topic);
      Serial.print("subscribed to ");
      Serial.println(mqtt_topic);

//move ssid and password out of current memory and into eeprom and local variables
//Serial.println(WiFi.SSID());
//Serial.println(WiFi.psk());
WiFi.SSID().toCharArray(ssid, 32);
WiFi.psk().toCharArray(password, 32);
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    saveCredentials();
    //end save
  }



}


void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);  // Convert byte
    str = strchr(str, sep);               // Find next separator
    if (str == NULL || *str == '\0') {
      break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
}

