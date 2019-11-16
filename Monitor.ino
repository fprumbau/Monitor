#include "global.h"


TaskHandle_t Task0;

#define uS_TO_mS_FACTOR 1000 // Umwandlungsfaktor Mikrosekunden zu Sekunden

//Temperatursensoren
#include <OneWire.h>
#include <DallasTemperature.h>
#define DS18B20_Aufloesung 12 //12 +/- 0.01, 10 +/- 0.05, 8 +/- 0.5
#define ONE_WIRE_BUS 25
#define Anzahl_Sensoren_DS18B20  2 // Anzahl der angeschlossenen Sensoren - Mögliche Werte: '0','1','2'

DeviceAddress DS18B20_Adressen;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature myDS18B20(&oneWire);

const float No_Val = 999.99;
float Temperatur[2] = {No_Val,No_Val};
unsigned long now;

//selbstgemachter Filter
float TempLast[2] = {0.0,0.0};

bool debugTemperature = false;

HTTPClient http;

#include <esp_task_wdt.h>
#include <esp_log.h>
   
/**
 * Kommandos von Serial lesen, wie z.B.
 * Neustart von WiFi
 */
void commandLine();
/**
 * Gibt verschiedene Werte aus
 */
void print();
/**
 * String tokeninzer
 */
uint16_t split(String s, char parser, int index);

void setup() {

  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_task_wdt_init(999,false); //0.14
  
  Serial.begin(115200); 

  Serial.println(F("Starting..."));

  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);

  if ((Anzahl_Sensoren_DS18B20 > 0) and (Anzahl_Sensoren_DS18B20 < 3)) {
      myDS18B20.begin();
      Serial.print(F("Anzahl angeschlossener Sensoren: "));
      Serial.println(myDS18B20.getDeviceCount(), DEC);
      for(byte i=0 ;i < myDS18B20.getDeviceCount(); i++) {
          if(myDS18B20.getAddress(DS18B20_Adressen, i)) {
            myDS18B20.setResolution(DS18B20_Adressen, DS18B20_Aufloesung);
          }
      }
  }

  myWifi._ssid="Perry";
  myWifi._password="5785340536886787";

  // etabliere Wifi Verbindung
  myWifi.connect();
  
  Serial.println(F("setup"));
  
  display.init(115200);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", F("Pegeldisplay, rufe /update zur Aktualisierung oder /restart zum Neustart!"));
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", F("Restarte ESP in 3s..."));
    delay(3000);
    ESP.restart();
  });

  server.begin();

  // initialize other the air updates
  updater.init("pegeldisplay");

  //neuer Taskcode Multicore
  xTaskCreatePinnedToCore(loop0, "Task0", 5000, NULL, 0, &Task0, 0);

  //Allg. Bildschirmeinstellungen
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);

  lastPegelUpdate = millis() + 10000; //Erstes Lesen der Daten nach 20s (s.u.)

  screen.init();            

  // allow reuse (if server supports it)
  http.setReuse(true);
}

/**********************************************************************/
/*                                                                    */
/*                 Loop0 (Core0)                                      */
/*                                                                    */
/**********************************************************************/
void loop0(void * parameter) {
  //Der normale Arduino loop laeuft automatisch in einer Schleife; die Schleife DARF nicht leer sein!!!
  for(;;) {
      commandLine();    //Pruefen, ob eine Kommandozeileneingabe vorhanden ist
      vTaskDelay(10);   //if not: Task watchdog got triggered. The following tasks did not feed the watchdog in time
      long now = millis();
      if((now - lastConnectCheck) > 60000 && !updater.stopForOTA) {
        lastConnectCheck = now;
        if(!myWifi.connected()) {
          Serial.println(F("myWifi ist nicht verbunden, versuche einen Reconnect"));    
          myWifi.reconnect();
        }
      }
  }
}

void loop() {

  if(!updater.stopForOTA) {

      now = millis();
    
      if(now - lastTempUpd > 1500) {
          readTemperatures();
          lastTempUpd = now;
      }

    
    //Nun Pegel und Wassertemp. holen
    if(now - lastPegelUpdate > 30000 && myWifi.connected()) {
        if(readPegelData) {  
          yield();
          readPegel(); 
        }
        lastPegelUpdate = now;       
        if(!myWifi.timeUpdate) {
          Serial.println(F("NTP noch nicht initialisiert, starte myWifi.readTime()"));
          myWifi.readTime();   
        }
    }
    //Ist das letzte Pegelupdate mehr als 10Min her, versuche eine
    //Re-Initialisierung des HTTPClients über einen forcierten NtpClient-Aufruf
    //analog 'read time reset'
    if(now - lastReceivedPegelMillis > 600000 && myWifi.connected()) {
        
        Serial.print(F("now: "));
        Serial.println(now);
        Serial.print(F("lastReceivedPegelMillis: "));
        Serial.print(lastReceivedPegelMillis);
        
        lastReceivedPegelMillis = now; //Reset, damit dieser Versuch nur maximal alle 10 Minuten wiederholt wird
             
    }
    if(now - lastScreenUpdate > 1000) {
        screen.show();
        lastScreenUpdate = now;
    }
  }

  //Restart erforderlich, wird durch updater-Objekt nach Upload einer neuen Firmware geregelt
  if(updater.restartRequired) {
    delay(2000);
    ESP.restart();
  }
 
}

void readPegel() {

   if(myWifi.connected()) {

       // Send HTTP request
       http.begin(F("http://www.prumbaum.com/data"));
       yield();
    
       int httpCode = http.GET();
       if (httpCode > 0) { //Check for the returning code
            yield();
            String payload = http.getString();

            DynamicJsonDocument doc(256);
            deserializeJson(doc, payload); //TODO from website???
    
            yield();
            if(debug) {
              Serial.println(F("SerializeJsonPretty to Serial"));
              serializeJsonPretty(doc, Serial);
            }
            
            pegel = doc["p"];
            temperature = doc["t"];

            if(debug) {
              Serial.print(F("Pegel: "));
              Serial.println(pegel);
            }
            timeStamp = timeClient.getFormattedDate();

            lastReceivedPegelMillis = now; //Diese Messung ist 'später' als die Vergleichsmessung in loop()
                
        } else {
            Serial.println(httpCode);
            Serial.println(F("Error on HTTP request"));
        }          
        yield();
        http.end();
   } else {
        Serial.println(F("readPegel nicht moeglich, weil myWifi nicht verbunden"));
   }
}



void commandLine() {
  if(Serial.available()) {
      String cmd = Serial.readString();
      Serial.print(F("Echo: "));
      Serial.println(cmd);
      String msg = String((char*)0);
      msg.reserve(32);
      if(cmd.startsWith(F("reconnect sma"))) {      
        sma.reset();
      } else if(cmd.startsWith(F("restart wifi"))) {      
        myWifi.reconnect();
      } else if(cmd.startsWith(F("debug on"))) {      
        debug = true;
      } else if(cmd.startsWith(F("debug off"))) {      
        debug = false;
      } else if(cmd.startsWith(F("print"))) {      
        print();
      } else if(cmd.startsWith(F("debug sma on"))) {      
        sma.debugSma=true;
      } else if(cmd.startsWith(F("debug sma off"))) {      
        sma.debugSma=false;
      } else if(cmd.startsWith(F("debug display on"))) {      
        screen.debugDisplay=true;
      } else if(cmd.startsWith(F("debug display off"))) {      
        screen.debugDisplay=false;
      } else if(cmd.startsWith(F("debug temp on"))) {      
        debugTemperature=true;
      } else if(cmd.startsWith(F("debug temp off"))) {      
        debugTemperature=false;
      } else if(cmd.startsWith(F("read pegel on"))) {      
        readPegelData=true;
      } else if(cmd.startsWith(F("read pegel off"))) {      
        readPegelData=false;
      } else if(cmd.startsWith(F("read pegel"))) {      
        readPegel();
      } else if(cmd.startsWith(F("read time"))) {    
        if(cmd.startsWith(F("read time reset"))) {
            myWifi.timeUpdate = false;
        }
        myWifi.readTime();
      } else if(cmd.startsWith(F("draw rects on"))) {      
        screen.redrawScreen(true);
      } else if(cmd.startsWith(F("draw rects off"))) {      
        screen.redrawScreen(false);
      } else if(cmd.startsWith(F("set pegel"))) {      
        String val = cmd.substring(10);
        pegel = atoi(val.c_str());
        Serial.print(val);   
        Serial.print(F(" / "));   
        Serial.println(pegel);       
      } else if(cmd.startsWith(F("set temp"))) {      
        String val = cmd.substring(9);
        temperature = atoi(val.c_str()); 
        Serial.print(val);   
        Serial.print(F(" / "));   
        Serial.println(temperature);
      } else if(cmd.startsWith(F("set offsetTemp"))) {        
        String ost = cmd.substring(15);
        ost.trim();
        offsetTemp = ost.toFloat();     
        Serial.println(offsetTemp);
      } else if(cmd.startsWith(F("rect"))) {      
        String val = cmd.substring(5);
        Serial.println(val);        
        uint16_t x = split(val, ',', 0);
        uint16_t y = split(val, ',', 1);
        uint16_t w = split(val, ',', 2);
        uint16_t h = split(val, ',', 3);
        screen.stop=true;
        screen.showBox(x,y,w,h,true);
      } else if(cmd.startsWith(F("display stop"))) {      
        screen.stop=true;
      } else if(cmd.startsWith(F("display start"))) {      
        screen.stop=false;
      } else {
        Serial.println(F("Available commands:"));
        Serial.println(F(" - debug on|off"));
        Serial.println(F(" - read pegel :: httpclient read pegel data"));
        Serial.println(F(" - set pegel|temp VAL :: setzen von Testwerten fuer Temperatur und Pegel"));
        Serial.println(F(" - reconnect sma :: reinit udp"));
        Serial.println(F(" - restart wifi :: reinit wifi conn"));
        Serial.println(F(" - read time [reset]:: NTP-Client lesen (mit reset lese NTP neu)"));
        Serial.println(F(" - read pegel on|off :: Pegel lesen"));
        Serial.println(F(" - draw rects on|off :: Felder nicht mehr optisch begrenzen"));
        Serial.println(F(" - debug sma on|off :: SMA-UdP-Debug on/off"));
        Serial.println(F(" - debug temp on|off :: TemperaturSensor-Debug on/off"));
        Serial.println(F(" - debug display on|off :: Debugausgaben fuers Display on/off"));
        Serial.println(F(" - rect x,y,w,h, :: zeichne ein Rechteck"));
        Serial.println(F(" - set offsetTemp TEMP :: zur Angleichung der Temperatursensoren"));        
        Serial.println(F(" - display start|stop :: Ausgaben stoppen"));
        Serial.println(F(" - print :: Schreibe einige abgeleitete Werte auf den Bildschirm"));
        return;
      }
      Serial.println(msg);;
    }  
}

void print() {
  Serial.print(F("MyWifi.connected() :: "));
  Serial.println(myWifi.connected());
  Serial.print(F("sma.hasNewPacket() :: "));
  Serial.println(sma.hasNewPacket());
  Serial.print(F("Pegel :: "));
  Serial.println(pegel);
  Serial.print(F("Temperatur :: "));
  Serial.println(temperature);
  Serial.print(F("MyWifi.wifiReconnects :: "));
  Serial.println(myWifi.wifiReconnects);    
  Serial.print(F("Lokale IP :: "));
  Serial.println(myWifi.localIP()); 
  Serial.print(F("Debug Display :: "));
  Serial.println(screen.debugDisplay); 
  Serial.print(F("Debug SMA :: "));
  Serial.println(sma.debugSma);    
  Serial.print(F("Read PegelData Flag :: "));
  Serial.println(readPegelData);     
  Serial.print(F("Zeit :: "));  
  Serial.println(timeClient.getFormattedDate());
  Serial.print(F("Timestamp :: "));  
  Serial.println(timeStamp);  
  Serial.print(F("Timestamp old :: "));  
  Serial.println(timeStampOld);
}

uint16_t split(String s, char parser, int index) {
  uint16_t rs=0;
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return 0;
      String tk = s.substring(rFromIndex,rToIndex);
      Serial.println(tk);
      return atoi(tk.c_str());
    } else parserCnt++;
  }
  return rs;
}

void readTemperatures() {
      //if ((Anzahl_Sensoren_DS18B20 > 0) and (Anzahl_Sensoren_DS18B20 < 3)) {
        myDS18B20.requestTemperatures();
        for(byte i=0 ;i < 2; i++) {
          //if (i < myDS18B20.getDeviceCount()) {         
        
            Temperatur[i] = myDS18B20.getTempCByIndex(i);
            if (Temperatur[i] == DEVICE_DISCONNECTED_C) {
                Temperatur[i] = No_Val;
                if(debug) Serial.println(F("Fehler"));
            } else {

                if(i==0) {
                  Temperatur[i]+=offsetTemp; //Korrekturwert
                }
                                                          
                if(TempLast[i] == 0) {
                  TempLast[i] = Temperatur[i];
                } else {
                  TempLast[i] = TempLast[i]  - 0.035 * (TempLast[i] -Temperatur[i]);
                }

                String t = String(TempLast[i]);                
                
                t1_new[i]=t.substring(0,4);
                t2_new[i]=t.substring(4);                
            }
          //}
        }
        if(debugTemperature) {
          Serial.print(F("Sensor 1 (raw/flt): "));
          Serial.print(Temperatur[0]);
          Serial.print(F("'C / "));
          Serial.print(TempLast[0]);
          Serial.print(F("'C"));

          Serial.print(F(" ;Sensor 2 (raw/flt): "));
          Serial.print(Temperatur[1]);
          Serial.print(F("'C / "));
          Serial.print(TempLast[1]);
          Serial.println(F("'C"));                      
        }
    //}  
}