#include <SPI.h>
#include <EEPROM.h>
#include "game.h"
#include "art.h"
GameCore gb;

/////State 
enum class State : char
{
  Title = 0,
  Menu = 1,
  Game = 2,
  Pause = 3
};
State state = State::Title;

bool debugShow = false;
unsigned long highscore = 0;
unsigned long score = 0;
int lives = 4;
float playerX;
float playerY;
float playerdir;
byte playersize = 4;
byte playerBlink = 0;

#define starNumber 32
byte starx[starNumber];
byte stary[starNumber];

#define stroidNumber 32
float stroidSpeed = 0.5;
float stroidX[stroidNumber];
float stroidY[stroidNumber];
byte stroidSize[stroidNumber];
float stroidDir[stroidNumber];
bool stroidActive[stroidNumber];

#define shotNumber 8
#define shotSpeed 4
int shotX[shotNumber];
int shotY[shotNumber];
float shotDir[shotNumber];
bool shotActive[shotNumber];

// Declare Functions
void stateIntro();
void initMenu();
void stateMenu();
void backInit();
void backDraw();
void asteroidInit();
void asteroidStep();
void asteroidDraw();
void playerInit();
void playerReset();
void playerStep();
void playerCollision();
void playerDraw();
void shotInit();
void shotDraw();
bool shotAdd(int x,int y,float dir);
void shotAsteroid();
void hudDraw();
void stateGame();
void stateGameDraw();
void statePause();
void setup();
void loop();

/////Intro
void stateIntro()
{
  gb.clearDisplay();
  gb.setCursor(46,28);
  gb.print(strArduboy);
  gb.display();
//  gb.tunes.tone(987, 160);
  delay(160);
//  gb.tunes.tone(1318, 400);
  delay(2000);
  initMenu(); state = State::Menu;
}

/////Menu
struct MenuType {
  byte selected;
  byte screen;
}; MenuType menu;

void initMenu()
{
  menu.selected = 0;
  menu.screen = 0;
  backInit();
  asteroidInit();
}
void stateMenu()
{
  //simple placeholder for the moment
  if (gb.pushed(BTN_A))
  {
    state = State::Game;
    playerInit();
    asteroidInit();
    shotInit();
  }
  if (gb.pushed(BTN_B))
  {
    debugShow = !debugShow;
  }
  gb.fillScreen(0);

  asteroidStep();
  backDraw();
  asteroidDraw();

  gb.drawBitmapSlow(0,0,sprLogoMask,0);
  gb.drawBitmapSlow(0,0,sprLogo,1);

  ///top score
  char text[24];
  gb.setCursor(4,56);
  sprintf(text, strHighscore , highscore); //sprintf is weird.
  gb.print(text);

  if (debugShow)
  {
    gb.setCursor(0,48);
    gb.print(strDebug);
  }
  
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
  for (byte i = 0; i < stroidStartNumber; ++i)
  {
    stroidX[i] = random(1)*128;
    stroidY[i] = random(1)*64;
    stroidDir[i] = random(2*pi);
    //stroidSpeed[i] = 0.5;
    stroidActive[i] = true;
    stroidSize[i] = 6;
  };
  for(byte i = stroidStartNumber; i<stroidNumber; ++i) //deactivates rest
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
      stroidX[i] += gb.lengthdirX(stroidSpeed,stroidDir[i]);
      stroidY[i] += gb.lengthdirY(stroidSpeed,stroidDir[i]);

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
    playerX += gb.lengthdirX(speed,(float)playerdir);
    playerY += gb.lengthdirY(speed,(float)playerdir);

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
    state = State::Menu;  //to-do : go to gameover screen
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
            float dir = shotDir[s] + (0.5*pi);
            byte m=0;
            while(!done)  //loops searching for free asteroid slots
            {
              if(stroidActive[m] == false)
              {
                int newDir = dir+(num*pi);
                stroidActive[m] = true;
                stroidX[m] = stroidX[a]+gb.lengthdirX(msize,newDir);
                stroidY[m] = stroidY[a]+gb.lengthdirY(msize,newDir);
                stroidSize[m] = msize;
                //stroidSpeed[m] = stroidSpeed[a];
                stroidDir[m] = newDir;
                num--;
              }
              m++;
              
              if ((num == 0) or (m>32)) //break if satisfied or cannot continue
              {
                done = true;
              }
            }
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

  if(debugShow == true)
  {
    gb.setCursor(0,0);
    sprintf(text, strCPU, gb.cpuLoad());
    gb.print(text);
  }
  
  gb.setCursor(0,56);
  sprintf(text, strScore , score); //sprintf is weird.
  gb.print(text);
};

/////Game
void stateGame()
{
  if (gb.pushed(BTN_A))
  {
    state = State::Pause;
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
    state = State::Game;
  }
  stateGameDraw();
  
  if((gb.frameCount()/12)%2)  //flicker the text
  {
    gb.setCursor(44,28);  //seems to be about the middle of the screen
    gb.print(strPaused);
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
    case State::Title: { stateIntro();}; break;
    case State::Menu:  { stateMenu(); }; break;
    case State::Game:  { stateGame(); }; break;
    case State::Pause: { statePause(); }; break;
  }
}
