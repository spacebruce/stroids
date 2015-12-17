#include <SPI.h>
#include <EEPROM.h>
#include "game.h"
#include "art.h"
GameCore gb;

/////State 
#define state_title 0
#define state_menu 1
#define state_game 2
#define state_paused 3

byte state = 0;

int score = 5000;
int lives = 4;
float playerx;
float playery;
float playerdir;
int playersize = 4;
int playerBlink = 0;

byte starx[32];
byte stary[32];

byte stroidNumber = 4;
float stroidx[32];
float stroidy[32];
byte stroidSpeed[32];
byte stroidDir[32];

#define maxShots 16
#define shotSpeed 4
int shotX[16];
int shotY[16];
float shotDir[16];
bool shotActive[16];

/////Intro
void stateIntro()
{
  gb.clearDisplay();
  gb.setCursor(46,28);
  gb.print("ARDUBOY");
  gb.display();
//  gb.tunes.tone(987, 160);
  delay(160);
//  gb.tunes.tone(1318, 400);
  delay(2000);
  
  initMenu(); state = state_menu;
}

/////Menu
struct MenuType {
  byte selected = 0;
  byte screen = 0;
}; MenuType menu;

void initMenu()
{
  menu.selected = 0;
}

void stateMenu()
{
  if (gb.pushed(BTN_A))
  {
    state = state_game;
    playerInit();
    backInit();
    asteroidInit();
    shotInit();
  }
  
  gb.fillScreen(0);
  gb.setCursor(0,0);
  gb.print("'ojhgjfhgf");
  gb.display();
};

////background
void backInit()
{
  for(byte i=0; i<32; i++)
  {
    starx[i] = random(128);
    stary[i] = random(64);
  };
};
void backDraw()
{
  for(byte i=0; i<32; i++)
  {
    gb.drawPixel(starx[i],stary[i],1);
  };
};
////'Stroids
void asteroidInit()
{
  stroidNumber = 4;
  for (byte i=0; i<stroidNumber; i+=1)
  {
    stroidx[i] = random(128);
    stroidy[i] = random(64);
    stroidDir[i] = random(2*pi);
    stroidSpeed[i] = 1;
  };
};
void asteroidStep()
{
  for (byte i=0; i<stroidNumber; i+=1)
  {
    stroidx[i] += gb.lengthdir_x(stroidSpeed[i],stroidDir[i]);
    stroidy[i] += gb.lengthdir_y(stroidSpeed[i],stroidDir[i]);
  
    if (stroidx[i]>128)  stroidx[i] = 0;
    if (stroidx[i]<0)    stroidx[i] = 128;
    if (stroidy[i]>64)   stroidy[i] = 0;
    if (stroidy[i]<0)    stroidy[i] = 64;
  };
};

void asteroidDraw()
{
  for (byte i=0; i<stroidNumber; i+=1)
  {
    gb.drawCircle(stroidx[i],stroidy[i],8,1);
  };

}
////player
void playerInit()
{
  score = 12345;
  lives = 4;
  playerReset();
}
void playerReset()
{
  playerx = 64;
  playery = 32;
  playerdir = pi*1.5;
  playerBlink = 120;
}
void playerStep()
{
  if (gb.pressed(BTN_L))
  {
    playerdir -= 0.3;
  };
  if (gb.pressed(BTN_R))
  {
    playerdir += 0.3;
  };
  if (gb.pressed(BTN_U))
  {
    int speed = 3;
    playerx += gb.lengthdir_x(speed,playerdir);
    playery += gb.lengthdir_y(speed,playerdir);
  }
  
  //modulo is for losers
  if (playerx>128)  playerx = 0;
  if (playerx<0)    playerx = 128;
  if (playery>64)   playery = 0;
  if (playery<0)    playery = 64;

  //shoots
  if (gb.pushed(BTN_A))
  {
    shotAdd(playerx,playery,playerdir);
  }
  //other stuff
  if (playerBlink > 0)
    playerBlink--;
}
void playerDraw()
{
  if (((playerBlink/4)%2) == false)
  {
    int x1,y1,x2,y2,x3,y3;
    float pd = playerdir;
    x1 = playerx + gb.lengthdir_x(playersize,pd);
    y1 = playery + gb.lengthdir_y(playersize,pd);
    x2 = playerx + gb.lengthdir_x(playersize,(pd+pi)+(0.3*pi));
    y2 = playery + gb.lengthdir_y(playersize,(pd+pi)+(0.3*pi));
    x3 = playerx + gb.lengthdir_x(playersize,(pd+pi)-(0.3*pi));
    y3 = playery + gb.lengthdir_y(playersize,(pd+pi)-(0.3*pi));
  
    //if(graphics.filled)
    if (false)
    {
      gb.drawTriangle(x1,y1,x2,y2,playerx,playery,0);
      gb.drawTriangle(x1,y1,x3,y3,playerx,playery,0);
    };
    
    gb.drawLine(x1,y1,x2,y2,1);
    gb.drawLine(x1,y1,x3,y3,1);
  
    gb.drawLine(x2,y2,playerx,playery,1);
    gb.drawLine(x3,y3,playerx,playery,1);
  };
};
////shooting
void shotInit()
{
  for(byte i=0; i<maxShots; i++)
  {
    shotX[i] = 0;
    shotY[i] = 0;
    shotDir[i] = 0;
    shotActive[i] = false;
  }
}
void shotStep()
{
  for (byte i=0; i<maxShots; i++)
  {
    if (shotActive[i])
    {
      shotX[i] += gb.lengthdir_x(shotSpeed,shotDir[i]);
      shotY[i] += gb.lengthdir_y(shotSpeed,shotDir[i]);
      
      if ((shotX[i] < 0) or (shotY[i] < 0) or (shotX[i] > 128) or (shotY[i] > 64))
      {
        shotActive[i] = false;
      }
    }
};

}
void shotDraw()
{
  for(byte i=0; i<maxShots; i++)
  {
    if (shotActive[i])
    {
      gb.drawPixel(shotX[i],shotY[i],1);
    }
  }
}
bool shotAdd(int x,int y,float dir)
{
  for(byte i=0; i<maxShots; i++)
  {
    if (shotActive[i] == false)
    {
      shotX[i] = x;
      shotY[i] = y;
      shotDir[i] = dir;
      shotActive[i] = true;
      return true;
    }
  }
  return false;
}

void hudDraw()
{
  gb.fillRect(128-(min(lives,8)*8),56,128,64,0);
  for(byte i=0; i<=(min(lives,8)); i++)
  {
    gb.drawBitmap(128-(i*8),56,sprLives,1);
  };

  /*
  char hud[10];
  gb.printf("%s",hud,score);
  */
  gb.setCursor(0,56);
  char text[12];
  sprintf(text, "%08d", score);
  gb.print(text);
};

/////Game
void stateGame()
{
  shotStep();
  playerStep();
  asteroidStep();
  
  gb.fillScreen(0);

  backDraw();
  
  asteroidDraw();
  playerDraw();
  shotDraw();
  
  hudDraw();
  gb.display();
}

void setup()
{
  gb.setup();
  backInit();
}

void loop()
{
  if(!gb.nextFrame()) { return; };
  gb.updateInput();
  
  switch(state)
  {
    case state_title: { stateIntro();}; break;
    case state_menu:  { stateMenu(); }; break;
    case state_game:  { stateGame(); }; break;
    //case state_pause: { statepause(); }; break;
  }
}
