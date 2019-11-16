#ifndef OTA_H
#define OTA_H

#include "global.h"
#include <ESPmDNS.h>

class OTA
{
  private:
    String _username;
    String _password;
    bool _authenticated;

    String _title = F("OTA updater");
    String _banner = F("OTA updater");
    String _build = F("Build : ");
    String _branch = F("Branch : ");
    String _deviceInfo = "ChipId : " + String(ESP.getChipRevision());
    String _footer = F("ESP32 OTA Updater");
    void setup(const char *path, String username, String password);
    
  public:
    volatile bool stopForOTA = false;
    void init(const char* host);
    void setUpdaterUi(String title,String banner,String build,String branch,String deviceInfo,String footer);
    void setUpdaterUi(String title,String banner,String build,String branch,String footer);
    bool restartRequired = false; // Set this flag in the callbacks to restart ESP in the main loop
};

#endif
