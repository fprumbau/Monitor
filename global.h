#ifndef GLOBAL_H
#define GLOBAL_H

//#include <GxEPD.h>
//#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w
//#include <GxIO/GxIO_SPI/GxIO_SPI.h>
//#include <GxIO/GxIO.h>

#include <GxEPD2_BW.h>

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h> 

#include "html.h"
#include "fonts.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <NTPClient.h>

#include <WiFiUdp.h>
#include "AsyncUDP.h""


#include <Update.h>
#include <time.h>

#include "MyWifi.h"
#include "OTA.h"
#include "SMA.h"
#include "display.h"

extern bool debug;
extern bool readPegelData;

extern MyWifi myWifi;
extern NTPClient timeClient;

extern GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display;

extern Screen screen;

extern AsyncWebServer server;   
extern OTA updater; //Over-the-air updater

extern const int smaMeasurementsBeforSwitchoff;
extern SMA sma;  //read SMA energy meter broadcast messages 

extern AsyncUDP udp;
extern WiFiUDP ntpUdp;

extern int pegel;
extern int temperature;

extern int pegelOld;
extern int temperatureOld;

extern float bezug;
extern float lieferung;
extern float netto;

extern unsigned long lastTempUpd;
extern unsigned long lastPegelUpdate;
extern unsigned long lastScreenUpdate;
extern unsigned long lastReceivedPegelMillis;
extern unsigned long lastConnectCheck;

extern unsigned int udpResets;
extern String timeStamp;
extern String timeStampOld;

extern String t1_old[2];
extern String t1_new[2];
extern String t2_old[2];
extern String t2_new[2];

extern float offsetTemp;
#endif
