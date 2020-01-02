#include "config.h"
#include "global.h"

#define OFFSET "offset"

void CFG::load() {

  if(!SPIFFS.begin()) {
    return;
  }
  File configFile = SPIFFS.open("/config.json", "r");
  if(!configFile){
    return;
  }
  size_t size = configFile.size();
  if(size>8192) {
     Serial.println(F("Config file is to large"));
     return;
  }
  //allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  //We don't use String here because ArduinoJson lib req the intput buffer
  // to be mutable. if you don't use ArduionJson, you may as well use
  // configFile.readString instead
  configFile.readBytes(buf.get(), size);
  Serial.println(buf.get());

  DynamicJsonDocument doc(128);
  
  auto error = deserializeJson(doc, buf.get());

  if(error) {
    Serial.println(F("Failed to parse config file"));
  }

  if(doc.containsKey(OFFSET)) {
    
      offset = doc[OFFSET];
      Serial.print("Offset: ");
      Serial.println(offset);
      
  }
}

bool CFG::save() {
  
  if(!SPIFFS.begin()) {
    return false;
  }

  DynamicJsonDocument doc(128);
  
  doc[OFFSET] = offset;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing"));
    return false;
  }
  serializeJson(doc, Serial);
  
  serializeJson(doc, configFile);
  Serial.println(F("\nKonfiguration wurde erfolgreich gespeichert."));

  configFile.flush();
  configFile.close();
  
  return true;
}
