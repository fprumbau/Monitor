#ifndef WEATHER_H
#define WEATHER_H

#include "global.h"

/**
 * User: fprumbau, A...
 * 
 * https://openweathermap.org/find?q=Untereschbach
 * 
 * Untereschbach
 * https://api.openweathermap.org/data/2.5/forecast/hourly?q={city name},{country code}
 * oder
 * https://openweathermap.org/city/2886242
 * 
 * { 
   "coord":{ 
      "lon":6.95,
      "lat":50.93
   },
   "weather":[ 
      { 
         "id":804,
         "main":"Clouds",
         "description":"overcast clouds",
         "icon":"04d"
      }
   ],
   "base":"stations",
   "main":{ 
      "temp":6.74,
      "pressure":999,
      "humidity":70,
      "temp_min":4.44,
      "temp_max":8.89
   },
   "visibility":10000,
   "wind":{ 
      "speed":2.6,
      "deg":110
   },
   "clouds":{ 
      "all":100
   },
   "dt":1574502410,
   "sys":{ 
      "type":1,
      "id":1271,
      "country":"DE",
      "sunrise":1574492441,
      "sunset":1574523399
   },
   "timezone":3600,
   "id":2886242,
   "name":"Koeln",
   "cod":200
}
{"coord":{"lon":6.95,"lat":50.93},"weather":[{"id":804,"main":"Clouds","description":"overcast clouds","icon":"04d"}],
"base":"stations","main":{"temp":9.98,"pressure":997,"humidity":58,"temp_min":7.78,"temp_max":12.78},"visibility":10000,"wind":{"speed":4.1,"deg":110},"clouds":{"all":100},
"dt":1574513789,"sys":{"type":1,"id":1271,"country":"DE","sunrise":1574492441,"sunset":1574523399},"timezone":3600,"id":2886242,"name":"Koeln","cod":200}
 */
 
class Weather {
  private:
        const char* apiKey = "d136638f4f28ae9d558b38b65b43fbd5";
        const char* url = "https://api.openweathermap.org/data/2.5/weather?id=2886242&appid=d136638f4f28ae9d558b38b65b43fbd5&units=metric";
  public:
        bool debugWeather = false;
        void update();
};



#endif
