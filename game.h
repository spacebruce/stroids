#pragma once
#include "this->h"


#define pi 3.14
#define todeg (180/pi)
#define torad (6.28319)
//#include "constants.h"

#define SCREEN_WIDTH  (WIDTH)
#define SCREEN_HEIGHT (HEIGHT)

#define BTN_U (B00000001)
#define BTN_D (B00000010)
#define BTN_L (B00000100)
#define BTN_R (B00001000)
#define BTN_B (B00100000)
#define BTN_A (B00010000)

class GameCore : public Arduboy {
  private:
  byte    nowInput;
  byte    prevInput;
  
  public:
  char    qx,qy;
    
  void setup() {
    this->setup();
    this->setFrameRate(60);
    nowInput  = 0x00;
    prevInput = 0xff;
  }

  void updateInput() {
    prevInput = nowInput;
    nowInput  = this->getInput();
  }
  
  bool pressed(byte button) const {
    return (nowInput & button) != 0;
  }
  bool pushed(byte button) const {
    return (nowInput & button) != 0 && (prevInput & button) == 0;
  }
  bool released(byte button) const {
    return (nowInput & button) == 0 && (prevInput & button) == 1;
  }

  ///functions nicked from gamemaker. they're handyish.
  float lengthdirX(float len,float dir)
  {
    return(len * cos(dir));
  };
  float lengthdirY(float len,float dir)
  {
    return(len * sin(dir));
  };

  //found at http://xoax.net/cpp/ref/cpp_examples/incl/distance_points_plane/
  float Distance(int dX0, int dY0, int dX1, int dY1)
  {
      return sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0));
  }

  //draws
  inline void drawPixelOffset(int x, int y, byte c) { this->drawPixel(x + qx, y + qy, c); }
  inline void drawBitmapOffset(int x, int y, const byte* bitmap, byte c) {
    this->drawBitmap(x + qx, y + qy, bitmap+2, pgm_read_byte(bitmap), pgm_read_byte(bitmap+1), c);
  }
  inline void drawBitmapSlowOffset(int x,int y,const byte* bitmap, byte c) {
    this->drawSlowXYBitmap(x + qx, y + qy, bitmap+2, pgm_read_byte(bitmap), pgm_read_byte(bitmap+1), c);
  }
  inline void fillRectOffset(int x, int y, int w, int h, byte c) {
    this->fillRect(x + qx, y + qy, w, h, c);
  }
  inline void drawCircleOffset(int x, int y, int r, byte c) {
    this->drawCircle(x + qx, y + qy, r, c);
  }
};

