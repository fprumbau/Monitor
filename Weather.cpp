#include "global.h"

void Weather::update() {


   if(myWifi.connected()) {
    
         Serial.println(F("Updating weather info..."));

         {
      
         int httpCode = 200;

         HTTPClient http;
         
         if(!debugWeather) {            
             http.begin(url);
             yield();          
             httpCode= http.GET();
         }
         
         vTaskDelay(8);
         if (httpCode > 0) { //Check for the returning code
          
              String payload;
              
              if(debugWeather) {
                payload = String(weatherdata);
                Serial.println(payload);
              } else {
                payload = http.getString();
              }
              
  
              DynamicJsonDocument doc(1024);
              deserializeJson(doc, payload);
      
              yield();
              if(debugWeather) {
                Serial.println(F("SerializeJsonPretty to Serial"));
                serializeJsonPretty(doc, Serial);
              }

              String base = doc["base"];

              temp = doc["main"]["temp"];
              pressure = doc["main"]["pressure"];
              humidity = doc["main"]["humidity"];
              tempMin = doc["main"]["temp_min"];
              tempMax = doc["main"]["temp_max"];

              //"main":{"temp":9.98,"pressure":997,"humidity":58,"temp_min":7.78,"temp_max":12.78}       

              lastWeatherMillis = millis();
                  
          } else {
            
            Serial.print(F("Error sending POST: "));
            Serial.println(http.errorToString(httpCode));
            String err = F("Err2 ");
            err+=timeClient.getFormattedTime();
            screen.notify(err);
            
          }    
          if(!debugWeather) {
            http.end();
            yield();
          }
        }
   } else {
        Serial.println(F("wetter.update() nicht moeglich, weil myWifi nicht verbunden"));
   }


  
}
