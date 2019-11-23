#include "MyWifi.h"
#include "global.h"

void MyWifi::connect() {

  //This is here to force the ESP32 to reset the WiFi and initialize correctly.
  WiFi.disconnect(true);
  WiFi.setSleep(false);
  WiFi.enableSTA(true);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  WiFi.mode(WIFI_STA);
  //Ende silly mode 
 
  WiFi.begin(_ssid, _password); 

  int ct = 0;
  while(WiFi.status() != WL_CONNECTED) {
      ct++;
      if(ct>5) {
        Serial.print(F("Next attempt..."));
        
        Serial.println(ct);
        ct=0;
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _password);
      }
      Serial.print(".");
      delay(3000); 
  }

  IPAddress ip = WiFi.localIP();
  _localIP = ip;
  _ip=ip.toString();
  
  Serial.print(F("\nIp Address: "));
  Serial.println(ip);

  Serial.println(F("Initializing sma.init"));
  sma.init();

  if(!ntpInitialized) {
    Serial.println(F("Reading NTP-Time"));
    readTime();
  }
}

String MyWifi::getIpAddress() {
  return _ip;
}

IPAddress MyWifi::localIP() {
  return _localIP;
}

void MyWifi::reconnect() {
  long now = millis();
  if((now - lastReconnectMillis) > 60000) { //Ein Reconnect max jede Minute
    wifiReconnects++;
    if(wifiReconnects >= myWifiRestartLimit) {
      wifiReconnects=0;
    } else {    
      Serial.println(F("Restarting WiFi..."));
      connect();
      lastReconnectMillis = now;
    }
  }
}

bool MyWifi::connected() {
  return WiFi.status() == WL_CONNECTED;
}

void MyWifi::readTime() {
    
    readPegelData = false;
    yield();
    //nicht EWIG warten (10 Versuche)
    int ct = 0;
    Serial.println(F("Reading time start"));
    if(!timeUpdate) {
      while(!(timeUpdate = timeClient.update())) {
        yield();
        Serial.print(F("o"));
        if(ct++>10) { 
          break;
        }
        timeClient.forceUpdate();        
      }    
      if(!timeUpdate) {
        Serial.println(F("Fehler beim Lesen der Zeit mit NTPClient!"));
        readPegelData = true;
        delay(1000);
        yield;
        return;
      }
      runningSince = timeClient.getFormattedTime();
      readPegelData = true; // Erst JETZT mit HTTPClient Pegel lesen, sonst Crash!
      ntpInitialized = true; //im connect()-Fall nur beim Start ausfuehren
      timeClient.setTimeOffset(3600); //UTC Korrektur 
    }
    Serial.println(runningSince);
    delay(1000);
    yield();
}
