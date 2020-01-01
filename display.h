#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

class Screen {

  private:
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;  
    int updateCount=-1;
    void drawContent();
    bool redraw = false;
    bool drawRects = false;
    long notifyMillis=-1; 
  public:
    bool refreshed = false;
    void redrawScreen(bool rects);
    bool debugDisplay = false;  
    bool stop = false;
    void show();
    void init();
    void notify(String msg);
    void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial);
    void refresh();
};

#endif
