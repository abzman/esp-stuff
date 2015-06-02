
// CONNECTIONS:
// DS3231 SDA --> SDA
// DS3231 SCL --> SCL
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <pgmspace.h>
#include <Wire.h>  // must be incuded here so that Arduino library object file references work
#include <RtcDS1307.h>

RtcDS1307 Rtc;

void setup () 
{
    Serial.begin(115200);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    //--------RTC SETUP ------------
    Rtc.Begin();
    Wire.begin(0, 2);

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
}

void loop () 
{
    if (!Rtc.IsDateTimeValid()) 
    {
		// Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    delay(10000); // ten seconds
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
	String datestring = "";
datestring += dt.Month();
datestring += "/";
datestring += dt.Day();
datestring += "/";
datestring += dt.Year();
datestring += " ";
datestring += dt.Hour();
datestring += ":";
datestring += dt.Minute();
datestring += ":";
datestring += dt.Second();
datestring += "\n";
    Serial.print(datestring);
}

