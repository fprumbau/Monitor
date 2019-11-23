#include "global.h"

bool debug = false;
bool readPegelData = false;

MyWifi myWifi;
AsyncWebServer server(80);

WiFiUDP ntpUdp;
AsyncUDP udp;

NTPClient timeClient(ntpUdp);
OTA updater;
Weather wetter;

//GxIO_Class io(SPI, /*CS=5*/ 15, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
//GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

//WROOM nutze 17 (gruen, DC)/16 (weiss, RST)
//GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ 15, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

//WROVER-B nutze  21 (gruen, DC)/19 (weiss, RST)/2 (violett, busy)
//Ohne die Umstellung von 16->19 und 17->21 gar keine Ansteuerung des Displays moeglich
//Ohne die Umstellung von 4->0 fuer Busy kommen immer Busy Timeout-Meldungen und kein PartialUpdate ist moeglich (mit Pin 2+0 kein Ser.Upd. mehr)
/*
Note:
External connections can be made to any GPIO except for GPIOs in the range 6-11, 16, or 17. GPIOs 6-11 are connected
to the module’s integrated SPI flash and PSRAM. GPIOs 16 and 17 are connected to the module’s integrated PSRAM.
For details, please see Section 6 Schematics.
 */
GxEPD2_BW<GxEPD2_420, GxEPD2_420::HEIGHT> display(GxEPD2_420(/*CS=5*/ 15, /*DC=*/ 21, /*RST=*/ 19, /*BUSY=*/ 22));

Screen screen;

const int smaMeasurementsBeforSwitchoff = 10;
SMA sma;

float bezug=0.0;
float lieferung=0.0;
float netto=0.0;

unsigned long lastTempUpd=0;
unsigned long lastPegelUpdate=0;
unsigned long lastScreenUpdate=0;
unsigned long lastReceivedPegelMillis = -1;
unsigned long lastConnectCheck = 0;
unsigned long lastWeatherMillis = -1;

int pegel=0;
int temperature=0;

int pegelOld=0;
int temperatureOld=0;

String timeStamp;
String timeStampOld;
unsigned int udpResets = 0;

String t1_old[]={ String((char*)0), String((char*)0) };
String t1_new[]={ String((char*)0), String((char*)0) };
String t2_old[]={ String((char*)0), String((char*)0) };
String t2_new[]={ String((char*)0), String((char*)0) };

float offsetTemp = 0.0;
String runningSince = String((char*)0);

float temp;
float tempOld;
int pressure;
int humidity;
float tempMin;
float tempMax;
