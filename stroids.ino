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

int score = 0;
int lives = 4;
float playerx;
float playery;
float playerdir;
int playersize = 4;
int playerBlink = 0;

byte nothing=0; //wow

#define starNumber 32
byte starx[starNumber];
byte stary[starNumber];

#define stroidNumber 32
float stroidX[stroidNumber];
float stroidY[stroidNumber];
byte stroidSize[stroidNumber];
float stroidSpeed[stroidNumber];
byte stroidDir[stroidNumber];
bool stroidActive[stroidNumber];

#define shotNumber 8
#define shotSpeed 4
int shotX[shotNumber];
int shotY[shotNumber];
float shotDir[shotNumber];
bool shotActive[shotNumber];

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
  
  gb.fillScreen(1);
  gb.setCursor(0,0);
  gb.print("'Stroid2s");
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
  const int stroidStartNumber = 3;
  for (byte i=0; i<stroidStartNumber; i+=1)
  {
    stroidX[i] = random(1)*128;
    stroidY[i] = random(1)*64;
    stroidDir[i] = random(2*pi);
    stroidSpeed[i] = 0.5;
    stroidActive[i] = true;
    stroidSize[i] = 6;
  };
};
void asteroidStep()
{
  byte numActive = 0;
  for (byte i=0; i<stroidNumber; i+=1)
  {
    if (stroidActive[i])
    {
      stroidX[i] += gb.lengthdir_x(stroidSpeed[i],stroidDir[i]);
      stroidY[i] += gb.lengthdir_y(stroidSpeed[i],stroidDir[i]);

      if (stroidX[i]>128)  stroidX[i] = 0;
      if (stroidX[i]<0)    stroidX[i] = 128;
      if (stroidY[i]>64)   stroidY[i] = 0;
      if (stroidY[i]<0)    stroidY[i] = 64;
      numActive++;
    }
  };

  if (numActive==0)
  {
    asteroidInit();
  }
};
void asteroidDraw()
{
  for (byte i=0; i<stroidNumber; i+=1)
  {
    if (stroidActive[i])
      gb.drawCircle(stroidX[i],stroidY[i],stroidSize[i],1);
  };
}
////player
void playerInit()
{
  score = 0;
  lives = 5;
  playerReset();
}
void playerReset()
{
  playerx = 64;
  playery = 32;
  playerdir = pi*1.5;
  playerBlink = 120;  //2 seconds of invuln.
}
void playerStep()
{
  
  if (gb.pressed(BTN_L))
  {
    playerdir -= 0.15;
  };
  if (gb.pressed(BTN_R))
  {
    playerdir += 0.15;
  };
  if (gb.pressed(BTN_U))
  {
    int speed = 1.5;
    playerx += gb.lengthdir_x(speed,playerdir);
    playery += gb.lengthdir_y(speed,playerdir);

    //modulo is for losers
    if (playerx>128)  playerx = 0;
    if (playerx<0)    playerx = 128;
    if (playery>64)   playery = 0;
    if (playery<0)    playery = 64;
  }
  if (gb.pushed(BTN_D))
  {
    playerx = random(128);
    playery = random(64);
  }

  //shoots
  if (gb.pushed(BTN_A))
  {
    shotAdd(playerx,playery,playerdir);
  }
  
  if (gb.pushed(BTN_B))
    lives--;
    
  //collision/invincible checks
  if (playerBlink > 0)
  {
    playerBlink--;
  }
  else
  {
    playerCollision();
  }
  ///collisions

  if (lives == 0)
  {
    state = state_menu;
  }
}
void playerCollision()
{
    for(byte i=0; i<stroidNumber; i++)
    {
      if ((stroidActive[i] == true) && (gb.Distance(playerx,playery,stroidX[i],stroidY[i]) < stroidSize[i]))
      {
        lives--;
        playerReset();
        return;
      }
    }
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
  
    gb.drawLine(x1,y1,x2,y2,1);
    gb.drawLine(x1,y1,x3,y3,1);
  
    gb.drawLine(x2,y2,playerx,playery,1);
    gb.drawLine(x3,y3,playerx,playery,1);
  };
};
////shooting
void shotInit()
{
  for(byte i=0; i<shotNumber; i++)
  {
    shotX[i] = 0;
    shotY[i] = 0;
    shotDir[i] = 0;
    shotActive[i] = false;
  }
}
void shotStep()
{
  for (byte i=0; i<shotNumber; i++)
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
  for(byte i=0; i<shotNumber; i++)
  {
    if (shotActive[i])
    {
      gb.drawPixel(shotX[i],shotY[i],1);
    }
  }
}
bool shotAdd(int x,int y,float dir)
{
  for(byte i=0; i<shotNumber; i++)
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

void shotAsteroid()
{
  for(byte s=0; s<shotNumber; s++)
  {
    if (shotActive[s] == true)
    {
      for(byte a=0; a<stroidNumber; a++)
      {
        if ((stroidActive[a] == true) && (gb.Distance(shotX[s],shotY[s],stroidX[a],stroidY[a]) < stroidSize[a]))
        {
          shotActive[s] = false;
          stroidActive[a] = false;

          score += stroidSize[a]*100;

          int msize = stroidSize[a]-2;
          if (msize != 0)
          {
            bool done=false;
            byte num = 2;
            float dir = random(pi);
            byte m=0;
            while(!done)
            {
              if(stroidActive[m] == false)
              {
                stroidActive[m] = true;
                stroidX[m] = stroidX[a]-msize;
                stroidY[m] = stroidY[a];
                stroidSize[m] = msize;
                stroidSpeed[m] = stroidSpeed[a];
                stroidDir[m] = dir+(num*pi);
                num--;
              }
              m++;
              
              if ((num == 0) or (m>32))
              {
                done = true;
              }
            }
            return;
          }
        }
      }
    }
  }
}

void hudDraw()
{
  gb.fillRect(128-(min(lives-1,8)*8),56,128,64,0);
  for(byte i=0; i<=(min(lives-1,8)); i++)
  {
    gb.drawBitmap(128-(i*8),56,sprLives,1);
  };

  /*
  char hud[10];
  gb.printf("%s",hud,score);
  */
  char text[8];
  
  gb.setCursor(0,0);
  sprintf(text, "%03dCPU", gb.cpuLoad());
  gb.print(text);
  
  gb.setCursor(0,56);
  sprintf(text, "%08d", score);
  gb.print(text);
};

/////Game
void stateGame()
{
  shotStep();
  playerStep();
  asteroidStep();
  shotAsteroid();
  
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
