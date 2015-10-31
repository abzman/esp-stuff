#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define led 14
String form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>simple pwm test </h1>"
  "<form action='msg'><p>pwm (10-1023, 0 for off) <input type='text' name='pwm' size=20 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";

long period;
#define SSID "i3"                                   // insert your SSID
#define PASS ""                                   // insert your password

// Instantiate server class                               // See tab Network for #include
ESP8266WebServer server(80);                              // HTTP server will listen at port 80


void setup(void)
{
  //ESP.wdtDisable();                                     // used to debug, disable wachdog timer,
  Serial.begin(115200);                                   // full speed to monitor
  WiFi.begin(SSID, PASS);                                 // Connect to WiFi network
  pinMode(led, OUTPUT);
  analogWrite(led, 0);
  analogWriteFreq(20000);
  while (WiFi.status() != WL_CONNECTED) {                 // Wait for connection
    delay(500);
    Serial.print(".");
  }

  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", []()
  {
    server.send(200, "text/html", form);
  });

  server.on("/msg", handle_msg);                          // And as regular external functions:
  server.begin();                                         // Start the server

  Serial.print("SSID : ");                                // prints SSID in monitor
  Serial.println(SSID);                                   // to monitor

  char result[16];
  sprintf(result, "%3d.%3d.%3d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);

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
  server.send(200, "text/html", form);                    // Send same page so they can send another msg

  String msg = server.arg("pwm");
  Serial.println(msg.toInt());
    analogWrite(led, msg.toInt());
}




