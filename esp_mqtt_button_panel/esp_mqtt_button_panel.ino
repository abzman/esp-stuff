/* esp_mqtt_button_panel - ESP8266 mqtt node with a button panel hooked up to it, it uses a 4021, 74595, and 4051 to multiplex the needed inputs (outputs are pwm and a cds cell can auto dim)

   Based on mqtt_basic, http://www.sinneb.net/?p=421  (thank you)

   Version 1.1  6/20/2016  Evan Allen
*/


//analog definitions
//pin 1 is zoom
//pin 2 is x
//pin 3 is y
//pin 4 is cds cell



#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>

#define PIN_595_1        2              // SPI 74HC595
#define s0               4              //4051 s0
#define s1               5              //4051 s1
#define s2               0              //4051 s2
#define PIN_MISO         12             // SPI data input
#define PIN_MOSI         13             // SPI data output
#define PIN_SCK          14             // SPI clock
#define PIN_SS1          15             // SPI hardware default SS pin, 4021
#define PWM              16             //sets pwm pin for LSD of LEDs



#define CDS_MAX              50        //value for max brightness as seen by the cds cell
#define CDS_MIN              1024       //value for min brightness as seen by the cds cell
#define PWM_MAX              255        //value for max brightness to be written
#define PWM_MIN              5          //value for min brightness to be written (without being off)


#define RST_LED_MASK         0b10000000
#define CRS_LED_MASK         0b01000000
#define FINE_LED_MASK        0b00100000
#define ERROR_LED_MASK       0b00010000
#define RUN_LED_MASK         0b00001000


#define RST_BTN_MASK         0b00001000
#define CRS_BTN_MASK         0b00000100
#define FINE_BTN_MASK        0b00000010
#define ON_SWI_MASK          0b00010000
#define NORM_SWI_MASK        0b00100000
#define DUAL_SWI_MASK        0b01000000

boolean auto_pwm = 1;              //sets pwm to auto set by cds

// result byte for 4021
byte buttons8;
byte trans;

const char *ssid =  "i3";   // cannot be longer than 32 characters!
const char *pass = "";  //

// Update these with values suitable for your network.
IPAddress server(10, 13, 0, 206);

WiFiClient wclient;
PubSubClient client(wclient, server);

String webString = "";


// Callback function
void callback(const MQTT::Publish& pub) {
  if (pub.payload_string().equals("buttons"))
  {
    webString = "buttons: " + String(buttons8);
    client.publish("outTopic", webString);
  } else if (pub.payload_string().equals("x"))
  {
    selectAnalog(1);
    webString = "x: " + String(analogRead(A0));
    client.publish("outTopic", webString);

  } else if (pub.payload_string().equals("zoom"))
  {
    selectAnalog(2);
    webString = "zoom: " + String(analogRead(A0));
    client.publish("outTopic", webString);

  } else if (pub.payload_string().equals("y"))
  {
    selectAnalog(3);
    webString = "y: " + String(analogRead(A0));
    client.publish("outTopic", webString);

  } else if (pub.payload_string().equals("cds"))
  {
    selectAnalog(4);
    webString = "cds: " + String(analogRead(A0));
    client.publish("outTopic", webString);

  } else if (pub.payload_string().equals("auto pwm on"))
  {
    auto_pwm = 1;

  } else if (pub.payload_string().equals("auto pwm off"))
  {
    auto_pwm = 0;

  } else if (pub.payload_string().equals("rst on"))
  {
    trans = trans | RST_LED_MASK;

  } else if (pub.payload_string().equals("crs on"))
  {
    trans = trans | CRS_LED_MASK;

  } else if (pub.payload_string().equals("fine on"))
  {
    trans = trans | FINE_LED_MASK;

  } else if (pub.payload_string().equals("error on"))
  {
    trans = trans | ERROR_LED_MASK;

  } else if (pub.payload_string().equals("run on"))
  {
    trans = trans | RUN_LED_MASK;

  } else if (pub.payload_string().equals("rst off"))
  {
    trans = trans & !RST_LED_MASK;

  } else if (pub.payload_string().equals("crs off"))
  {
    trans = trans & !CRS_LED_MASK;

  } else if (pub.payload_string().equals("fine off"))
  {
    trans = trans & !FINE_LED_MASK;

  } else if (pub.payload_string().equals("error off"))
  {
    trans = trans & !ERROR_LED_MASK;

  } else if (pub.payload_string().equals("run off"))
  {
    trans = trans & !RUN_LED_MASK;

  } else
  {
    writeBrightness(pub.payload_string().toInt());
  }
}


void setup() {

  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  client.set_callback(callback);
  SPI.begin();
  
  // set all IC select pins HIGH
  pinMode(PIN_SS1, OUTPUT);
  digitalWrite(PIN_SS1, HIGH);
  pinMode(PIN_595_1, OUTPUT);
  digitalWrite(PIN_595_1, HIGH);
  pinMode(s0, OUTPUT);
  digitalWrite(s0, HIGH);
  pinMode(s1, OUTPUT);
  digitalWrite(s1, HIGH);
  pinMode(s2, OUTPUT);
  digitalWrite(s2, HIGH);
  pinMode(PWM, OUTPUT);
  digitalWrite(PWM, HIGH);

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("espClient")) {
        client.publish("outTopic", "button panel reconnected");
        client.subscribe("inTopic");
      }
    }

    if (client.connected())
      client.loop();
  }


  if (auto_pwm)
  {
    selectAnalog(4);
    writeBrightness(map(analogRead(A0), CDS_MIN, CDS_MAX, PWM_MIN, PWM_MAX));
  }

  // select 4021
  digitalWrite(PIN_SS1, LOW);
  // select 595
  digitalWrite(PIN_595_1, LOW);
  // read CD4021 IC, output trans to '595
  buttons8 = SPI.transfer(trans);
  buttons8 = buttons8 | 0b10000001;

  if (buttons8 != (buttons8 | RST_BTN_MASK)) //reset pressed
  {
    client.publish("outTopic", "rst");
  }
  if (buttons8 != (buttons8 | CRS_BTN_MASK)) //crs pressed
  {
    client.publish("outTopic", "crs");
  }
  if (buttons8 != (buttons8 | FINE_BTN_MASK)) //fine pressed
  {
    client.publish("outTopic", "fine");
  }
  if (buttons8 != (buttons8 | ON_SWI_MASK)) // on switched
  {
    client.publish("outTopic", "on");
  }
  if (buttons8 != (buttons8 | NORM_SWI_MASK)) // norm switched
  {
    client.publish("outTopic", "norm");
  }
  if (buttons8 != (buttons8 | DUAL_SWI_MASK)) // dual switched
  {
    client.publish("outTopic", "dual");
  }

  delay(100);

  // deselect 4021
  digitalWrite(PIN_SS1, HIGH);
  // deselect 595
  digitalWrite(PIN_595_1, HIGH);
}

void selectAnalog(int addr)
{
  digitalWrite(s0, addr & (1 << 0));
  digitalWrite(s1, addr & (1 << 1));
  digitalWrite(s2, addr & (1 << 2));
}

void writeBrightness(int val)
{
  analogWrite(PWM, val);
}

