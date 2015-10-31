#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <Wire.h> 
#include <PCF8574_HD44780_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#define led 16
String form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>Set up daylight alarm clock </h1>"
  "<form action='msg'><p>time to be awake by (format 0600) <input type='text' name='time' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "<form action='msg'><p>time to spend waking up (in minutes, max 255) <input type='text' name='duration' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "<form action='msg'><p>pwm (testing, 10-1023) <input type='text' name='pwm' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "<p>Snooze <a href=\"msg?snooze=1\"><button>Yes, Please!</button></a></p>"
  "<p>Alarm <a href=\"msg?alarm=on\"><button>ON</button></a>&nbsp;<a href=\"msg?alarm=off\"><button>OFF</button></a></p>"
  "</center>";
// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
long period;
boolean alarmOn = 1;
byte duration = 30; //in minutes (max 255)
int hours = 6; //wake up time in 24 hour time
int minutes = 45; //wake up time in 24 hour time
#define SSID "i3"                                   // insert your SSID
#define PASS ""                                   // insert your password

// Instantiate server class                               // See tab Network for #include
ESP8266WebServer server(80);                              // HTTP server will listen at port 80

// Address 0x27, 16 chars, 2 line display
PCF8574_HD44780_I2C lcd(0x27,16,2);

void setup(void)
{
  //ESP.wdtDisable();                                     // used to debug, disable wachdog timer,
  Serial.begin(115200);                                   // full speed to monitor
  WiFi.begin(SSID, PASS);                                 // Connect to WiFi network
  pinMode(led, OUTPUT);
  analogWrite(led, 0);
  analogWriteFreq(25000);
  EEPROM.begin(512);
  
  alarmOn = EEPROM.read(0);
  duration = EEPROM.read(1);
  hours = EEPROM.read(2);
  minutes = EEPROM.read(3);


  lcd.init();           // LCD Initialization              
  lcd.backlight();      // Backlight ON
  lcd.clear();          // Clear the display
  
  lcd.setCursor(0,0);   // Set the cursor to col 0, row 0
  lcd.print("wifi sunrise");   // Print the first word
  lcd.setCursor(0,1);   // Set the cursor to col 0, row 1
  lcd.print("alarm clock");  // Print the second word


    Serial.println("wifi sunrise alarm clock");
    Serial.println();
  
    Serial.print("alarm set to: ");
    Serial.println(alarmOn);
    Serial.print("duration: ");
    Serial.println(duration);
    Serial.print("wake up time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.println(minutes);
    
  while (WiFi.status() != WL_CONNECTED) {                 // Wait for connection
    delay(500);
    Serial.print(".");
  }

  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", []()
  {
    send_page();
  });

  server.on("/msg", handle_msg);                          // And as regular external functions:
  server.begin();                                         // Start the server

  Serial.print("SSID : ");                                // prints SSID in monitor
  Serial.println(SSID);                                   // to monitor

  char result[16];
  sprintf(result, "%3d.%3d.%3d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  
  lcd.clear();          // Clear the display
  lcd.setCursor(0,0);   // Set the cursor to col 0, row 0
  lcd.print("IP Address");   // Print the first word
  lcd.setCursor(0,1);   // Set the cursor to col 0, row 1
  lcd.print(WiFi.localIP()[0]);  // Print the second word
  lcd.print(".");  // Print the second word
  lcd.print(WiFi.localIP()[1]);  // Print the second word
  lcd.print(".");  // Print the second word
  lcd.print(WiFi.localIP()[2]);  // Print the second word
  lcd.print(".");  // Print the second word
  lcd.print(WiFi.localIP()[3]);  // Print the second word
  
  Serial.println("WebServer ready:   ");
  Serial.println(WiFi.localIP());                         // Serial monitor prints localIP
  Serial.println();
  Serial.println(result);

}


void loop(void) {
  server.handleClient();                                  // checks for incoming messages
}


/*
  handles the messages coming from the webbrowser, restores a few special characters and
  constructs the strings that can be sent to the oled display
*/
void handle_msg()
{
  String msg = server.arg("pwm");
  if (msg.equals(NULL)) {}
  else
  {
    Serial.print("pwm: ");
    Serial.println(msg.toInt());
    analogWrite(led, msg.toInt());
  }




  msg = server.arg("time");
  if (msg.equals(NULL)) {}
  else
  {
    hours = msg.toInt() / 100;
    minutes = msg.toInt() - (hours * 100);
    EEPROM.write(2, hours);
    EEPROM.write(3, minutes);
    EEPROM.commit();
    Serial.print("eeprom set, wake up time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.println(minutes);
  }

  msg = server.arg("duration");
  if (msg.equals(NULL)) {}
  else
  {
    EEPROM.write(1, msg.toInt());
    EEPROM.commit();
    duration = msg.toInt();
    Serial.print("eeprom set, duration: ");
    Serial.println(msg.toInt());
  }

  msg = server.arg("snooze");
  if (msg.equals("1"))
  {
    Serial.println("snoozing");

    //snoozing behavior here

  }

  msg = server.arg("alarm");
  if (msg.equals("on"))
  {
    EEPROM.write(0, 1);
    EEPROM.commit();
    alarmOn = 1;
    Serial.println("eeprom set, alarm on");
  }
  else if (msg.equals("off"))
  {
    EEPROM.write(0, 0);
    EEPROM.commit();
    alarmOn = 0;
    Serial.println("eeprom set, alarm off");
  }
  send_page();
}

void send_page()
{
  String toSend = form;
  toSend += "<p>";
  toSend += "<center>";
  toSend += "<h1>The status quo</h1>";
  toSend += "<p>The alarm is currently ";
  if (alarmOn)
  {
    toSend += "on.</p>";
  }
  else
  {
    toSend += "off.</p>";
  }
  toSend += "<p>The wake up duration is currently ";
  toSend += duration;
  toSend += " minutes from off to full brightness</p>";
  toSend += "<p>The wake up time is currently ";
  toSend += hours;
  toSend += ":";
  toSend += minutes;
  toSend += " </p>";
  toSend += "</center>";
  server.send(200, "text/html", toSend);                    // Send same page so they can send another msg

}




