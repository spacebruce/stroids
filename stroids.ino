#include <SPI.h>
#include <EEPROM.h>
#include "game.h"
#include "art.h"
GameCore gb;

// State
enum class State : char
{
  Title = 0,
  Menu = 1,
  Game = 2,
  Pause = 3,
  End = 4
};
State state = State::Title;

bool debugShow = false;
unsigned long highscore = 0;
unsigned long score = 0;
int lives = 4;
#define SPACEFRICTION 0.975
#define playerAcceleration 0.1
#define playerSpeedCap 2
#define playerSize 4
float playerX,playerY
float playerXSpeed,playerYSpeed; //vec2d :D
float playerDir;
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

// Intro
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

// Menu
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
  // Simple placeholder for the moment
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

  // Top score
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

// Background
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
  const int stroidStartNumber = 3;  // How many to start each round with
  byte i;

  for(i = 0; i<stroidNumber; i++) // 
  {
    stroidActive[i] = false;
  }
  for (i = 0; i < stroidStartNumber; i++)
  {
    stroidActive[i] = true;
    stroidX[i] = random(1)*127;
    stroidY[i] = random(1)*63;
    stroidDir[i] = random(2*pi);
    //stroidSpeed[i] = 0.5;
    stroidSize[i] = 6;
  };
};
void asteroidStep()
{
  // Loops 'stroids and adds speed/direction to position
  byte numActive = 0; // Counts how many are active
  for (byte i=0; i<stroidNumber; i++)
  {
    if (stroidActive[i])
    {
      stroidX[i] += gb.lengthdirX(stroidSpeed,stroidDir[i]);
      stroidY[i] += gb.lengthdirY(stroidSpeed,stroidDir[i]);

      // Loop offscreen
      if (stroidX[i]>127)  stroidX[i] = 0;
      if (stroidX[i]<0)    stroidX[i] = 127;
      if (stroidY[i]>63)   stroidY[i] = 0;
      if (stroidY[i]<0)    stroidY[i] = 63;
      
      numActive++;
    }
  };

  if (numActive==0) // If all asteroids are gone, start next round
  {
    asteroidInit();
  }
};
void asteroidDraw()
{
  for (byte i=0; i<stroidNumber; i++)
  {
    if (stroidActive[i])
      gb.drawCircle(stroidX[i],stroidY[i],stroidSize[i],1); // Todo : make actual asteroid graphics
  };
}
// Player
void playerInit()
{
  score = 0;
  lives = 5;
  playerReset();
}
void playerReset()
{
  playerX = 63;
  playerY = 31;
  playerXSpeed = 0;
  playerYSpeed = 0;
  playerDir = pi*1.5;
  playerBlink = 120;  //2 seconds of invuln.
}
void playerStep()
{
  if (gb.pressed(BTN_L))
  {
    playerDir -= 0.15;
  };
  if (gb.pressed(BTN_R))
  {
    playerDir += 0.15;
  };
  if (gb.pressed(BTN_U))
  {
    playerXSpeed += gb.lengthdirX(playerAcceleration,playerDir);
    playerYSpeed += gb.lengthdirY(playerAcceleration,playerDir);

    if (playerXSpeed > 0)
      playerXSpeed = min(playerXSpeed,playerSpeedCap);
    if (playerYSpeed > 0)
      playerYSpeed = min(playerYSpeed,playerSpeedCap);
    if (playerXSpeed < 0)
      playerXSpeed = max(playerXSpeed,-playerSpeedCap);
    if (playerYSpeed < 0)
      playerYSpeed = max(playerYSpeed,-playerSpeedCap);
  }
  else
  {
    if (abs(playerXSpeed) > 0)  { playerXSpeed *= SPACEFRICTION; }
    if (abs(playerYSpeed) > 0)  { playerYSpeed *= SPACEFRICTION; }
  }
  if (gb.pushed(BTN_D))
  {
    //teleport to random position. 
    playerX = random(128);
    playerY = random(64);
  }
  
  playerX += playerXSpeed;
  playerY += playerYSpeed;

  // Modulo is for losers
  if (playerX>127)  playerX = 0;
  if (playerX<0)    playerX = 127;
  if (playerY>63)   playerY = 0;
  if (playerY<0)    playerY = 63;

  // Shoots
  if (gb.pushed(BTN_B))
  {
    shotAdd(playerX,playerY,playerDir);
  }
  
  // Collision/invincible checks
  if (playerBlink > 0)
  {
    playerBlink--;
  }
  else
  {
    playerCollision();
  }
  // Collisions

  if (lives == 0)
  {
    state = State::Menu;  // To-do : go to gameover screen
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
  if(((playerBlink / 12) & 1) == 0) // If visible on this frame
  {
    int x1,y1,x2,y2,x3,y3;
    //reminder that the arduino library uses radians! Not degrees! You idiot! You wasted HOURS working that out!
    x1 = playerX + gb.lengthdirX(playerSize,playerDir);     // Tip
    y1 = playerY + gb.lengthdirY(playerSize,playerDir);
    x2 = playerX + gb.lengthdirX(playerSize,(playerDir+pi)+(0.3*pi));   // Left(?) wing
    y2 = playerY + gb.lengthdirY(playerSize,(playerDir+pi)+(0.3*pi));
    x3 = playerX + gb.lengthdirX(playerSize,(playerDir+pi)-(0.3*pi));   // Right(?) wing
    y3 = playerY + gb.lengthdirY(playerSize,(playerDir+pi)-(0.3*pi));
  
    gb.drawLine(x1,y1,x2,y2,1); // Tip to wings
    gb.drawLine(x1,y1,x3,y3,1);
    gb.drawLine(x2,y2,playerX,playerY,1); // Tips to center
    gb.drawLine(x3,y3,playerX,playerY,1);
  };
};
// Shooting
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

      // Delete if out of bounds
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
  // Loops every active shot and every active asteroid and checks for incircle collisions
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

          score += stroidSize[a]*100; // Todo : better score calculation

          int msize = stroidSize[a]-2;  // Chunk sizes
          if (msize != 0)
          {
            bool done=false;
            byte num = 2; // Makes 2 chunks
            float dir = shotDir[s] + (0.5*pi);
            byte m=0;
            while(!done)  // Loops searching for free asteroid slots
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
              
              if ((num == 0) or (m>32)) // Break if satisfied or cannot continue
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
  
  gb.fillRect(128-(min(lives-1,8)*8),56,128,64,0);  // Background for lives counter
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

// Game
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
// Pause
void statePause()
{
  if (gb.pushed(BTN_A))
  {
    state = State::Game;
  }
  stateGameDraw();
  
  if((gb.frameCount()/12)%2)  // Flicker the text
  {
    gb.setCursor(44,28);  // Seems to be about the middle of the screen
    gb.print(strPaused);
  }
  gb.display();
}
// KillScreen
void stateEnd()
{
  gb.fillScreen(0);

  asteroidStep();
  backDraw();
  asteroidDraw();

  gb.fillRect(8,8,120,56,0);
  gb.drawLine(8,8,120,8,1);
  gb.drawLine(8,56,120,56,1);
  gb.drawLine(8,8,8,56,1);
  gb.drawLine(120,8,120,56,1);

  gb.display();
}
void endInit()
{

}

// The good bits
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
    case State::Title: { stateIntro(); }; break;
    case State::Menu:  { stateMenu();  }; break;
    case State::Game:  { stateGame();  }; break;
    case State::Pause: { statePause(); }; break;
    case State::End:   { stateEnd();   }; break;
  }
}