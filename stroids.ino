#include <SPI.h>
#include <EEPROM.h>
#include "game.h"
#include "art.h"
GameCore gb;

/////State 
#define state_title 0
#define state_menu 1
#define state_game 2
#define state_pause 3

byte state = 0;

unsigned long highscore = 0;
unsigned long score = 0;
int lives = 4;
float playerX;
float playerY;
float playerdir;
byte playersize = 4;
byte playerBlink = 0;

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
  gb.print("WHAT");
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
  menu.screen = 0;
}
void stateMenu()
{
  //simple placeholder for the moment
  if (gb.pushed(BTN_A))
  {
    state = state_game;
    playerInit();
    backInit();
    asteroidInit();
    shotInit();
  }
  gb.fillScreen(0);
  gb.fillRect(0,0,128,32,1);
  gb.drawBitmapSlow(0,0,sprLogo,0);

  ///top score
  char text[24];
  gb.setCursor(4,56);
  sprintf(text, "TOP SCORE : %08lu", highscore); //sprintf is weird.
  gb.print(text);
  
  gb.display();
};

////background
void backInit()
{
  for(byte i=0; i<starNumber; i++)
  {
    starx[i] = random(128);
    stary[i] = random(64);
  };
};
void backDraw()
{
  for(byte i=0; i<starNumber; i++)
  {
    gb.drawPixel(starx[i],stary[i],1);
  };
};

////'Stroids
void asteroidInit()
{
  const int stroidStartNumber = 3;  //how many to start each round with
  byte i;
  for (i=0; i<stroidStartNumber; i+=1)
  {
    stroidX[i] = random(1)*128;
    stroidY[i] = random(1)*64;
    stroidDir[i] = random(2*pi);
    stroidSpeed[i] = 0.5;
    stroidActive[i] = true;
    stroidSize[i] = 6;
  };
  for(i; i<stroidNumber; i++) //deactivates rest
  {
    stroidActive[i] = false;
  }
};
void asteroidStep()
{
  //loops 'stroids and adds speed/direction to position
  byte numActive = 0; //counts how many are active
  for (byte i=0; i<stroidNumber; i+=1)
  {
    if (stroidActive[i])
    {
      stroidX[i] += gb.lengthdirX(stroidSpeed[i],stroidDir[i]);
      stroidY[i] += gb.lengthdirY(stroidSpeed[i],stroidDir[i]);

      //loop offscreen
      if (stroidX[i]>128)  stroidX[i] = 0;
      if (stroidX[i]<0)    stroidX[i] = 128;
      if (stroidY[i]>64)   stroidY[i] = 0;
      if (stroidY[i]<0)    stroidY[i] = 64;
      
      numActive++;
    }
  };

  if (numActive==0) //if all asteroids are gone, start next round
  {
    asteroidInit();
  }
};
void asteroidDraw()
{
  for (byte i=0; i<stroidNumber; i+=1)
  {
    if (stroidActive[i])
      gb.drawCircle(stroidX[i],stroidY[i],stroidSize[i],1); //todo : make actual asteroid graphics
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
  playerX = 64;
  playerY = 32;
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
    int speed = 1.5;  //todo : velocity
    playerX += gb.lengthdirX(speed,playerdir);
    playerY += gb.lengthdirY(speed,playerdir);

    //modulo is for losers
    if (playerX>128)  playerX = 0;
    if (playerX<0)    playerX = 128;
    if (playerY>64)   playerY = 0;
    if (playerY<0)    playerY = 64;
  }
  if (gb.pushed(BTN_D))
  {
    //teleport to random position. 
    playerX = random(128);
    playerY = random(64);
  }
  
  //shoots
  if (gb.pushed(BTN_B))
  {
    shotAdd(playerX,playerY,playerdir);
  }
  
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
    if (score>highscore)
    {
      highscore = score;
    }
  }
}
void playerCollision()
{
  //Collision is based on a single pixel at the players midpoint. Should make up for the difficulty from the small playfield.
  for(byte i=0; i<stroidNumber; i++)
  {
    if ((stroidActive[i] == true) && (gb.Distance(playerX,playerY,stroidX[i],stroidY[i]) < stroidSize[i]))
    {
      lives--;
      playerReset();
      return;
    }
  }
}
void playerDraw()
{
  if (((playerBlink/4)%2) == false) //if visible on this frame
  {
    int x1,y1,x2,y2,x3,y3;
    //reminder that the arduino library uses radians! Not degrees! You idiot! You wasted HOURS working that out!
    x1 = playerX + gb.lengthdirX(playersize,playerdir);     //tip
    y1 = playerY + gb.lengthdirY(playersize,playerdir);
    x2 = playerX + gb.lengthdirX(playersize,(playerdir+pi)+(0.3*pi));   //left(?) wing
    y2 = playerY + gb.lengthdirY(playersize,(playerdir+pi)+(0.3*pi));
    x3 = playerX + gb.lengthdirX(playersize,(playerdir+pi)-(0.3*pi));   //right(?) wing
    y3 = playerY + gb.lengthdirY(playersize,(playerdir+pi)-(0.3*pi));
  
    gb.drawLine(x1,y1,x2,y2,1); //tip to wings
    gb.drawLine(x1,y1,x3,y3,1);
    gb.drawLine(x2,y2,playerX,playerY,1); //tips to center
    gb.drawLine(x3,y3,playerX,playerY,1);
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
  //loops shots, updates positions based on speed and direction.
  for (byte i=0; i<shotNumber; i++)
  {
    if (shotActive[i])
    {
      shotX[i] += gb.lengthdirX(shotSpeed,shotDir[i]);
      shotY[i] += gb.lengthdirY(shotSpeed,shotDir[i]);

      //delete if out of bounds
      if ((shotX[i] < 0) or (shotY[i] < 0) or (shotX[i] > 128) or (shotY[i] > 64))
      {
        shotActive[i] = false;
      }
    }
  }
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
  //loops shots and inserts into the first free space it can find.
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
  //loops every active shot and every active asteroid and checks for incircle collisions
  for(byte s=0; s<shotNumber; s++)
  {
    if (shotActive[s] == true)
    {
      for(byte a=0; a<stroidNumber; a++)
      {
        if ((stroidActive[a] == true) && ((gb.Distance(shotX[s],shotY[s],stroidX[a],stroidY[a])) <= stroidSize[a]))
        {
          shotActive[s] = false;
          stroidActive[a] = false;

          score += stroidSize[a]*100; //todo : better score calculation

          int msize = stroidSize[a]-2;  //chunk sizes
          if (msize != 0)
          {
            bool done=false;
            byte num = 2; //makes 2 chunks
            float dir = random(pi); //fly in random directions
            byte m=0;
            while(!done)  //loops searching for free asteroid slots
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
              
              if ((num == 0) or (m>32)) //break if satisfied or cannot continue
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
  
  gb.fillRect(128-(min(lives-1,8)*8),56,128,64,0);  //background for lives counter
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
  sprintf(text, "%08d", score); //sprintf is weird.
  gb.print(text);
};

/////Game
void stateGame()
{
  if (gb.pushed(BTN_A))
  {
    state = state_pause;
  }
  shotStep();
  playerStep();
  asteroidStep();
  shotAsteroid();
  
  stateGameDraw();
  gb.display();
}
void stateGameDraw()
{
  gb.fillScreen(0);
  backDraw();
  asteroidDraw();
  playerDraw();
  shotDraw();
  hudDraw();
}
////pause
void statePause()
{
  if (gb.pushed(BTN_A))
  {
    state = state_game;
  }
  stateGameDraw();
  
  if((gb.frameCount()/12)%2)  //flicker the text
  {
    gb.setCursor(44,28);  //seems to be about the middle of the screen
    gb.print("PAUSED!");
  }
  gb.display();
}


///the good bits
void setup()
{
  gb.setup();
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
    case state_pause: { statePause(); }; break;
  }
}
