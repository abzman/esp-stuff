
#include <EEPROM.h>

char ssid[32] = "i3detroit";
char password[32];
char mqtt_server[32] = "10.13.0.155";
char mqtt_topic[50] = "abzmanHome/House/Basement/North/Light/Control";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  delay(500);
  //saveCredentials();
  loadCredentials();
}

void loop() {
  // put your main code here, to run repeatedly:

}

/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  EEPROM.get(0+sizeof(ssid)+sizeof(password), mqtt_server);
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(mqtt_server), mqtt_topic);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password)+sizeof(mqtt_server)+sizeof(mqtt_topic), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(password);
  Serial.println(mqtt_server);
  Serial.println(mqtt_topic);
  
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  EEPROM.put(0+sizeof(ssid)+sizeof(password), mqtt_server);
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(mqtt_server), mqtt_topic);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password)+sizeof(mqtt_server)+sizeof(mqtt_topic), ok);
  EEPROM.commit();
  EEPROM.end();
  
}
