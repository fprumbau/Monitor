#ifndef MYWIFI_H
#define MYWIFI_H

#include <WiFi.h>
#include <WiFiMulti.h>

class MyWifi {
    private:
      String _ip;
      WiFiMulti wifiMulti;
      IPAddress _localIP;
      long lastReconnectMillis;
      const int myWifiRestartLimit = 60; //in Minuten      
      bool ntpInitialized = false;      
    public:
      bool timeUpdate = false;
      int wifiReconnects = 0; 
      const char* _ssid;
      const char* _password;
      void connect();
      String getIpAddress();
      IPAddress localIP();
      void reconnect();
      bool connected();
      void readTime();
      long strength();
};

#endif
