#include <SPI.h> 
#include <EEPROM.h> 
#include "Sprites.h"
#include "Arduboy.h"

Arduboy ardboy;

// Declare Functions
void setup(void);
void loop(void);

// The good bits
void setup(void)
{
  arduboy.setup();
  arduboy.setFrameRate(60);
}

void loop(void)
{
  arduboy.updateInput();
  if(!arduboy.nextFrame())
  { 
    return;
  }
}
