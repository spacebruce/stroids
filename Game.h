#pragma once
#include "core.h"
#include "Arduboy.h"
#include "Maths.h"

#define SCREEN_WIDTH  (WIDTH)
#define SCREEN_HEIGHT (HEIGHT)

#define BTN_U (UP_BUTTON)
#define BTN_D (DOWN_BUTTON)
#define BTN_L (LEFT_BUTTON)
#define BTN_R (RIGHT_BUTTON)
#define BTN_B (B_BUTTON)
#define BTN_A (A_BUTTON)

struct GameCore {
  Arduboy arduboy;
  byte    nowInput;
  byte    prevInput;
  char    qx, qy;
    
  void setup(void) {
    arduboy.setup();
    arduboy.setFrameRate(60);
    nowInput = 0x00;
    prevInput = 0xff;
  }

  void updateInput(void) {
    prevInput = nowInput;
    nowInput = arduboy.getInput();
  }
  
  bool pressed(byte button) const {
    return (nowInput & button) != 0;
  }
  bool pushed(byte button) const {
    return (nowInput & button) != 0 && (prevInput & button) == 0;
  }
  bool released(byte button) const {
    return (nowInput & button) == 0 && (prevInput & button) != 0;
  }

  // functions nicked from gamemaker.they're handyish.
  float lengthdirX(float len, float dir)
  {
    return(len * cos(dir));
  };
  float lengthdirY(float len, float dir)
  {
    return(len * sin(dir));
  };

  // found at http:// xoax.net / cpp / ref / cpp_examples / incl / distance_points_plane/
  float Distance(int dX0, int dY0, int dX1, int dY1)
  {
      return sqrt((dX1 - dX0) * (dX1 - dX0) + (dY1 - dY0) * (dY1 - dY0));
  }

  // delegation time!
  inline int cpuLoad(void)  { return arduboy.cpuLoad(); }
  inline bool nextFrame(void) { return arduboy.nextFrame(); }
  inline void clearDisplay(void)  { arduboy.clearDisplay(); }
  inline void fillScreen(byte c)  { arduboy.fillScreen(c); }
  inline void display(void) { arduboy.display(); }
  inline int frameCount(void) { return arduboy.frameCount; }
  // draws
  inline void drawPixel(int x, int y, byte c) { arduboy.drawPixel(x + qx, y + qy, c); }
  inline void drawLine(int x0, int y0, int x1, int y1, uint8_t c)  { arduboy.drawLine(x0, y0, x1, y1, c); }
  inline void drawBitmap(int x, int y, const byte * bitmap, byte c) {
    arduboy.drawBitmap(x + qx, y + qy, bitmap + 2, pgm_read_byte(bitmap), pgm_read_byte(bitmap + 1), c);
  }
  inline void drawBitmapSlow(int x, int y, const byte * bitmap, byte c) {
    arduboy.drawSlowXYBitmap(x + qx, y + qy, bitmap + 2, pgm_read_byte(bitmap), pgm_read_byte(bitmap + 1), c);
  }
  inline void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color) { arduboy.fillTriangle (x0, y0, x1, y1, x2, y2, color); }
  inline void fillRect(int x, int y, int w, int h, byte c) {
    arduboy.fillRect(x + qx, y + qy, w, h, c);
  }
  inline void drawCircle(int x, int y, int r, byte c) {
    arduboy.drawCircle(x + qx, y + qy, r, c);
  }
  inline void setCursor(int x, int y) { arduboy.setCursor(x, y); }
  inline void print(const char * text) { arduboy.print(text); }
};

