#include "display.h"

#include "global.h"

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

void Screen::init() {
    display.firstPage();
    do {
      display.setFont(&FreeSans9pt7b);
        
      display.setCursor(191, 115);
      display.print(F("Vorlauf in Celsius"));

      display.setCursor(191, 198);
      display.print(F("Ruecklauf in Celsius"));
    } while (display.nextPage());       
}

void Screen::show() {

  if(stop) {
      if(debugDisplay) { 
        Serial.println(F("stop"));
      }
      return;
  }
  
  //fixme updateCount zurueck auf 100
  if(updateCount ==-1 || updateCount>10) {
    updateCount=0;
    drawContent();
  } else {
    updateCount++;
    drawContent();  
  }
}

void Screen::drawContent() {

  if(debugDisplay) { 
      Serial.println(F("."));
  }

  if(stop) {
      if(debugDisplay) { 
        Serial.println(F("stop"));
      }
      return;
  }

  if(notifyMillis>0 && millis() - notifyMillis > 60000) { //Benachrichtigung nach 5 Minuten ueberschreiben
    redraw = true;
    notifyMillis = -1;
  }

  if(pegel>0 && pegel!=pegelOld || redraw) {

    if(debugDisplay) {
      Serial.println(F("Drawing new pegel box"));
    }
    
    pegelOld=pegel;

    x = 0;
    y = 0;
    w = 400;
    h = 100;
    
    display.setPartialWindow(x, y, w, h);
    
    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&Dialog_bold_125);
      display.setCursor(0, 95);    
      display.print(pegel);   
      display.setFont(&Lato_Bold_80);
      display.print(F("cm"));
    } while (display.nextPage());    
  }

  if(temp>0 && temp != tempOld || redraw) {

    if(debugDisplay) {
      Serial.println(F("Drawing new temp box"));
    }

    tempOld = temp;

    x = 0;
    y = 185;
    w = 185;
    h = 80;

    display.setPartialWindow(x, y, w, h);         
    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&FreeSans9pt7b);
        
      display.setCursor(2, 200);
      String t = F("Temperatur: ");
      t += temp;
      t += "'C";
      display.print(t); 

      display.setCursor(4, 220);
      t = tempMin;
      t += "'C bis ";
      t += tempMax;
      t += "'C";
      display.print(t); 
      
      display.setCursor(4, 240);
      t = F("Luftfeuchte: ");
      t += humidity;
      t += "%";
      display.print(t);       

      display.setCursor(4, 260);
      t = F("Luftdruck: ");
      t += pressure;
      t += " hPa";
      display.print(t);     
    } while (display.nextPage());  
    
  }

  if(temperature>0 && temperature != temperatureOld || redraw) {
    
    if(debugDisplay) {
      Serial.println(F("Drawing new temperature box"));
    }
    
    temperatureOld = temperature;
    
    x = 0;
    y = 102;
    w = 140;
    h = 64;
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&Lato_Bold_80);      
      display.setCursor(0, 162);
      display.print(temperature);   
      display.setFont(&FreeSansBold24pt7b);
      display.print(F("'C"));
    } while (display.nextPage());    
  }

  //DateTime
  if(!timeStamp.equals(timeStampOld)) {

      x = 0;
      y = 165;
      w = 185;
      h = 20;

      timeStampOld = timeStamp;

      if(debugDisplay) {
          Serial.println(F("Drawing new timestamp"));
      }
      
      display.setPartialWindow(x, y, w, h);         
      display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);
        if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
        display.setFont(&FreeSans9pt7b);  
        display.setCursor(2, 180);
        display.print(timeStamp); 
        display.print(F(" Untereschb.")); 
      } while (display.nextPage());  
      
  }

  //Temperatur 1 (bis 1. Nachkommastelle)
  if(!t1_new[0].equals(t1_old[0]) || redraw) {
    
    x = 190;
    y = 119;
    w = 160;
    h = 64;

    t1_old[0] = t1_new[0];

    if(debugDisplay) {
      Serial.println(F("Drawing new pooltemp[0] box"));
    }
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {    
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&Lato_Bold_80);      
      display.setCursor(x, 179);
      display.print(t1_new[0]);        
    } while (display.nextPage());       
  }

  //Temperatur 2
  if(!t1_new[1].equals(t1_old[1]) || redraw) {
    
    x = 190;
    y = 202;
    w = 160;
    h = 64;

    t1_old[1] = t1_new[1];

    if(debugDisplay) {
      Serial.println(F("Drawing new pooltemp[1] box"));
    }
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {    
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&Lato_Bold_80);      
      display.setCursor(x, 262);
      display.print(t1_new[1]);   
    } while (display.nextPage());       
  }

  //Temperatur 1 (2. Nachkommastelle)
  if(!t2_new[0].equals(t2_old[0]) || redraw) {
    
    x = 355;
    y = 158;
    w = 39;
    h = 25;

    t2_old[0] = t2_new[0];

    if(debugDisplay) {
      Serial.println(F("Drawing new pooltemp2 [0] box"));
    }
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {    
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);

      //2. Nachkommastelle klein
      display.setFont(&FreeMonoBold12pt7b);   
      display.setCursor(x+1, y+18);
      display.print(t2_new[0]);
      display.setFont(&FreeSans9pt7b);  
      display.print(F("'C"));
      
    } while (display.nextPage());       
  }

  //Temperatur 2 (2. Nachkommastelle)
  if(!t2_new[1].equals(t2_old[1]) || redraw) {
    
    x = 355;
    y = 241;
    w = 39;
    h = 25;

    t2_old[1] = t2_new[1];

    if(debugDisplay) {
      Serial.println(F("Drawing new pooltemp2 [1] box"));
    }
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {    
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);

      //2. Nachkommastelle klein
      display.setFont(&FreeMonoBold12pt7b);  
      display.setCursor(x+1, y+18);
      display.print(t2_new[1]);      
      display.setFont(&FreeSans9pt7b);  
      display.print(F("'C"));
      
    } while (display.nextPage());       
  }

  //Solarproduktion
  if(sma.hasNewPacket() || redraw) {

    if(debugDisplay) {
      Serial.println(F("Drawing new sma netto box"));
    }
    
    String nettoBetrag = String((char*)0);
    nettoBetrag.reserve(20);
    nettoBetrag+="Solar: ";
    if(netto>=0) {
      nettoBetrag+="+";
    }
    nettoBetrag+=String(netto,0);
    nettoBetrag+=F(" W");
    
    x = 0;
    y = 267;
    w = 232;
    h = 50;
    
    display.setPartialWindow(x, y, w, h);  
    display.setFont(&FreeSans18pt7b);    
    
    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      if(drawRects) display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setCursor(1, 296); //Die Schrift ist ca 18px hoch, die vertikale Mitte der Schrift ist dann bei y=290px
      display.print(nettoBetrag);
    } while (display.nextPage());
    
    sma.resetNewPacket();     
  }
}

void Screen::showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial) {
  if(stop) {
      if(debugDisplay) { 
        Serial.println(F("stop"));
      }
      return;
  }
  display.setRotation(0);
  if (partial) {
    display.setPartialWindow(x, y, w, h);
  }
  else {
    display.setFullWindow();
  }
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(x, y, w, h, GxEPD_BLACK);
  }
  while (display.nextPage());  
}

void Screen::redrawScreen(bool rects) {  
  if(stop) {
      if(debugDisplay) { 
        Serial.println(F("stop"));
      }
      return;
  }  
  drawRects = rects;
  init();
  redraw=true;
}

void Screen::notify(String msg) {
    if(stop) {
        if(debugDisplay) { 
          Serial.println(F("stop"));
        }
        return;
    }
    
    x = 180;
    y = 0;
    w = 220;
    h = 25;

    if(debugDisplay) {
      Serial.println(F("Drawing notification message"));
    }
    notifyMillis = millis(); 
    
    display.setPartialWindow(x, y, w, h);
    display.firstPage();
    do {    
      display.fillScreen(GxEPD_WHITE);
      display.drawRect(x, y, w, h, GxEPD_BLACK);
      display.setFont(&FreeMonoBold9pt7b);  
      display.setCursor(x+3, y+18);
      display.print(msg);      
    } while (display.nextPage());         
}

void Screen::refresh() {
  stop = true;
  delay(2000);
  for(int i=0; i<3; i++) {
    display.setFullWindow();
    display.firstPage();
    do {
      display.fillScreen(GxEPD_BLACK);
    }
    while (display.nextPage());
    
    delay(500);
    
    display.setFullWindow();
    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
    }
    while (display.nextPage());
    stop = false;    
  }
}
