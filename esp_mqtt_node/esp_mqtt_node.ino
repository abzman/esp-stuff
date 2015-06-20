/* esp_mqtt_node - ESP8266 mqtt node with a DHT sensor as an input, an NTP set RTC, a bistable relay, and an LED output

   Based on DHTServer(ESP8266Webserver, DHTexample, and BlinkWithoutDelay), NTPClient, NTP2RTC, DS1307_simple,mqtt_publish_in_callback  (thank you)

   Version 1.0  6/20/2015  Evan Allen
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>


#include <pgmspace.h>
#include <Wire.h>  // must be incuded here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307 Rtc;

#include <DHT.h>
#define DHTTYPE DHT11
#define DHTPIN  5

#define LEDPIN  16
#define ONPIN  14
#define OFFPIN  12
#define countof(a) (sizeof(a) / sizeof(a[0]))
boolean relayState = 0;

//char ssid[] = "abz-fi";  //  your network SSID (name)
//char password[] = "we do what we must, because we can";       // your network password
const char* ssid     = "i3";
const char* password = "";


//rtc timer variables
const unsigned long rtc_time = 600000; //timeout for the knob turn in thousandths of a second (600000 = 10 mins)
unsigned long rtc_start_time = millis(); //used for the count down timer
unsigned long current_time = millis();


// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 134);

PubSubClient client(server);

//ntp stuff
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP

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


void printDateTime(const RtcDateTime& dt)
{
  Serial.print(returnDateTime(dt));
}


String returnDateTime(const RtcDateTime& dt)
{
  String datestring = "";
  if (dt.Month() < 10) {
    datestring += "0";
  }
  datestring += dt.Month();
  datestring += "/";
  if (dt.Day() < 10) {
    datestring += "0";
  }
  datestring += dt.Day();
  datestring += "/";
  datestring += dt.Year();
  datestring += " ";
  if (dt.Hour() < 10) {
    datestring += "0";
  }
  datestring += dt.Hour();
  datestring += ":";
  if (dt.Minute() < 10) {
    datestring += "0";
  }
  datestring += dt.Minute();
  datestring += ":";
  if (dt.Second() < 10) {
    datestring += "0";
  }
  datestring += dt.Second();
  return datestring;
}

// Callback function
void callback(const MQTT::Publish& pub) {
  if(pub.payload_string().equals("RTC"))
  {
    //do rtc stuff
  } else if(pub.payload_string().equals("temp"))
  {
    gettemperature();       // read sensor
    webString = "Temperature: " + String((int)temp_f) + " F"; // Arduino has a hard time with float to string
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);
  } else if(pub.payload_string().equals("hum"))
  {
       gettemperature();           // read sensor
    webString = "Humidity: " + String((int)humidity) + "%";
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked

      } else if(pub.payload_string().equals("relay on"))
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
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked
 
    
      } else if(pub.payload_string().equals("relay off"))
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
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked

    
      } else if(pub.payload_string().equals("relay state"))
  {
       if (relayState) {
      webString = "Relay is on";
    } else {
      webString = "Relay is off";
    }
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked
 
    
      } else if(pub.payload_string().equals("led on"))
  {
        if (!digitalRead(LEDPIN)) {
      webString = "The LED is already on";
    } else {
      digitalWrite(LEDPIN, 0);
      webString = "The LED has been turned on";
    }
    webString += "\nTime: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked
  
      } else if(pub.payload_string().equals("led off"))
  {
       if (!digitalRead(LEDPIN)) {
      digitalWrite(LEDPIN, 1);
      webString = "The LED has been turned off";
    } else {
      webString = "LED is already off";
    }
    webString += "\nTime: ";
    webString += getTime();
   client.publish("outTopic",webString);               // send to someones browser when asked
 
    
          } else if(pub.payload_string().equals("led state"))
  {
    
       if (!digitalRead(LEDPIN)) {
      webString = "LED is on";
    } else {
      webString = "LED is off";
    }
    webString += "\nTime: ";
    webString += getTime();
   client.publish("outTopic",webString);               // send to someones browser when asked
          } else if(pub.payload_string().equals("rtc set"))
  {
       setRTC();
    webString = "RTC Set. Time: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked

              } else if(pub.payload_string().equals("rtc get"))
  {
        webString = "Time: ";
    webString += getTime();
    client.publish("outTopic",webString);               // send to someones browser when asked
 
    
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
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, 1);
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
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  
  
  //--------RTC SETUP ------------
  Rtc.Begin();
#if defined(ESP8266)
  Wire.begin(0, 2);
#endif

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid())
  {
    // Common Cuases:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");

    // following line sets the RTC to the date & time this sketch was compiled
    // it will also reset the valid flag internally unless the Rtc device is
    // having an issue

    Rtc.SetDateTime(compiled);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
  
  
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");
  }

}

void loop(void)
{
    current_time = millis();
  if (current_time - rtc_start_time > rtc_time) //rtc timer loop
  {
    setRTC();
  }
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


String getTime()
{
  if (!Rtc.IsDateTimeValid())
  {
    // Common Cuases:
    //    1) the battery on the device is low or even missing and the power line was disconnected
    return "RTC lost confidence in the DateTime!";
  }

  RtcDateTime now = Rtc.GetDateTime();

  return returnDateTime(now);
}


void setRTC()
{
    Serial.print("RTC before : ");
    printDateTime(Rtc.GetDateTime());    
    Serial.println();
  rtc_start_time = current_time;
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // NTP contains four timestamps with an integer part and a fraction part
    // we only use the integer part here
    unsigned long t1, t2, t3, t4;
    t1 = t2 = t3 = t4 = 0;
    for (int i = 0; i < 4; i++)
    {
      t1 = t1 << 8 | packetBuffer[16 + i];
      t2 = t2 << 8 | packetBuffer[24 + i];
      t3 = t3 << 8 | packetBuffer[32 + i];
      t4 = t4 << 8 | packetBuffer[40 + i];
    }

    // part of the fractional part
    // could be 4 bytes but this is more precise than the 1307 RTC
    // which has a precision of ONE second
    // in fact one byte is sufficient for 1307
    float f1, f2, f3, f4;
    f1 = ((long)packetBuffer[20] * 256 + packetBuffer[21]) / 65536.0;
    f2 = ((long)packetBuffer[28] * 256 + packetBuffer[29]) / 65536.0;
    f3 = ((long)packetBuffer[36] * 256 + packetBuffer[37]) / 65536.0;
    f4 = ((long)packetBuffer[44] * 256 + packetBuffer[45]) / 65536.0;

    // NOTE:
    // one could use the fractional part to set the RTC more precise
    // 1) at the right (calculated) moment to the NEXT second!
    //    t4++;
    //    delay(1000 - f4*1000);
    //    RTC.adjust(DateTime(t4));
    //    keep in mind that the time in the packet was the time at
    //    the NTP server at sending time so one should take into account
    //    the network latency (try ping!) and the processing of the data
    //    ==> delay (850 - f4*1000);
    // 2) simply use it to round up the second
    //    f > 0.5 => add 1 to the second before adjusting the RTC
    //   (or lower threshold eg 0.4 if one keeps network latency etc in mind)
    // 3) a SW RTC might be more precise, => ardomic clock :)


    // convert NTP to UNIX time, differs seventy years = 2208988800 seconds
    // NTP starts Jan 1, 1900
    // Unix time starts on Jan 1 1970.
#define SECONDS_FROM_1970_TO_2000 946684800
    const unsigned long seventyYears = 2208988800UL + 946684800UL; //library differences, it wants seconds since 2000 not 1970
    t1 -= seventyYears;
    t2 -= seventyYears;
    t3 -= seventyYears;
    t4 -= seventyYears;

    Serial.println("T1 .. T4 && fractional parts");
    printDateTime(RtcDateTime(t1)); Serial.println(f1, 4);
    printDateTime(RtcDateTime(t2)); Serial.println(f2, 4);
    printDateTime(RtcDateTime(t3)); Serial.println(f3, 4);

    printDateTime(t4); Serial.println(f4, 4);
    Serial.println();

    // Adjust timezone and DST... in my case substract 4 hours for Chile Time
    // or work in UTC?
    t4 -= (4 * 3600L);     // Notice the L for long calculations!!
    t4 += 1;               // adjust the delay(1000) at begin of loop!
    if (f4 > 0.4) t4++;    // adjust fractional part, see above
    Rtc.SetDateTime(t4);
    Serial.print("RTC after : ");
    printDateTime(Rtc.GetDateTime());    
    Serial.println();
    printDateTime(t4); Serial.println(f4, 4);
    Serial.println();
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
