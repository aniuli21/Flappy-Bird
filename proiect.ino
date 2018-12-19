#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>

#define V0_PIN 9

LedControl lc = LedControl(12, 11, 10, 1); 

//DIN, CLK, LOAD, No. DRIVER
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
 
int col = 7;
int opt;
int wallPos1 = 0;
int wallPos2 = 1;
int wallPos3 = 6;
int wallPos4 = 7;
int birdBodyX = 0;
int birdBodyY = 4;
int birdHeadX = 1;
int birdHeadY = 3;
int birdHeadTopX = 1;
int birdHeadTopY = 0;
int birdBodyTopX = 0;
int birdBodyTopY = 1;
int score = 0;
int gameState = 0;
int periodFall = 50;
int periodJump = 50;
int periodWalls = 100;
int periodEnd = 1000;
int buttonState;

unsigned long currentTimeFall = 0;
unsigned long currentTimeJump = 0;
unsigned long currentTimeWallsShow = 0;
unsigned long currentTimeEnd = 0;

const int buttonPin = 8;

bool sadFace[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {0, 1, 1, 0, 0, 1, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 1, 1, 1, 1, 0, 0}, 
    {0, 1, 0, 0, 0, 0, 1, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0}
};

void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  lcd.begin(16, 2);
  lcd.clear();
  //bird initial position
  lc.setLed(0,birdHeadY,birdHeadX,true);
  lc.setLed(0,birdBodyY,birdBodyX,true);
  pinMode(V0_PIN, OUTPUT); 
  analogWrite(V0_PIN, 90); 
  pinMode(buttonPin, INPUT);
}

void loop()
{ 
  play();   
}

void play()
{
  buttonState = digitalRead(buttonPin);
 
  if (buttonState == 1)
  {
    gameState = 1;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Current score:");
    lcd.print(score);
  }
  if (gameState == 0)
  {
    mainMenu();
  }
  else
  {
    if (birdBodyY < 8 && (birdHeadY != wallPos1 || birdHeadX != col) && (birdHeadY != wallPos2 || birdHeadX != col) && (birdHeadY != wallPos3 || birdHeadX != col) && (birdHeadY != wallPos4 || birdHeadX != col) && (birdBodyY != wallPos1 || birdBodyX != col) && (birdBodyY != wallPos2 || birdBodyX != col) && (birdBodyY != wallPos3 || birdBodyX != col) && (birdBodyY != wallPos4 || birdBodyX != col) )
    {
      continueGame();  
    }
    else
    {
      endGame();
    }
  }
}

void mainMenu()
{
  lc.setLed(0,birdHeadY,birdHeadX,true);
  lc.setLed(0,birdBodyY,birdBodyX,true);
  lcd.setCursor(0,0);
  lcd.print("---START GAME---");
}

void continueGame()
{
  showWalls();
  col--;
  if (buttonState == 0)
  {
    birdFall();
  }
  hideWalls();
  if (col==-2)
  { 
    randomWalls();
    col = 8;
    raiseScore();
  }
  if (buttonState == 1)
  {  
    jump(); 
  }
}

void printScore()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Your score is:");
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("Press to restart");
}

void raiseScore()
{
  score++;
  lcd.clear();
  lcd.print(score);
  if (score % 5 == 0 && score != 0)
  {
    setDifficulty();
  }
}

void setDifficulty()
{
  periodWalls = periodWalls - 20;
}
void showWalls()
{     
 currentTimeWallsShow = millis();
  
 lc.setLed(0,wallPos1,col,true);
 lc.setLed(0,wallPos2,col,true);
 lc.setLed(0,wallPos3,col,true);
 lc.setLed(0,wallPos4,col,true);
   
 while (millis() < currentTimeWallsShow + periodWalls)
 {
         
 }
}

void hideWalls()
{
  lc.setLed(0,wallPos1,col+1,false);
  lc.setLed(0,wallPos2,col+1,false);
  lc.setLed(0,wallPos3,col+1,false);
  lc.setLed(0,wallPos4,col+1,false);    
}

void randomWalls()
{ 
  opt = random(5);
  
  switch(opt)
  {
    case 0:
    {
      wallPos1=0;
      wallPos2=5;
      wallPos3=6;
      wallPos4=7;
    }
    break;
    case 1:
    {
      wallPos1=0;
      wallPos2=1;
      wallPos3=6;
      wallPos4=7;
    }
    break;
    case 2:
    {
      wallPos1=0;
      wallPos2=1;
      wallPos3=2;
      wallPos4=7;
    }
    break;
    case 3:
    {
      wallPos1=4;
      wallPos2=5;
      wallPos3=6;
      wallPos4=7;
    }
    break;
    case 4:
    {
      wallPos1=0;
      wallPos2=1;
      wallPos3=2;
      wallPos4=3;
    }
    break;
  } 
}

void birdFall()
{ 
  currentTimeFall = millis();
  
  lc.setLed(0,birdHeadY,birdHeadX,false);
  lc.setLed(0,birdBodyY,birdBodyX,false);
      
  birdBodyY++;
  birdHeadY++;
  
  lc.setLed(0,birdHeadY,birdHeadX,true);
  lc.setLed(0,birdBodyY,birdBodyX,true);
  
  while(millis() < currentTimeFall + periodFall)
  {
    
  }
}

void jump()
{ 
  currentTimeJump = millis();
  if (birdHeadY > 0)
  {
   lc.setLed(0,birdHeadY,birdHeadX,false);
   lc.setLed(0,birdBodyY,birdBodyX,false);
    
   birdBodyY--;
   birdHeadY--;
    
   lc.setLed(0,birdHeadY,birdHeadX,true);
   lc.setLed(0,birdBodyY,birdBodyX,true);
  }
  else
  { 
   lc.setLed(0,birdHeadY,birdHeadX,false);
   lc.setLed(0,birdBodyY,birdBodyX,false);
    
   lc.setLed(0,birdHeadTopY,birdHeadTopX,true);
   lc.setLed(0,birdBodyTopY,birdBodyTopX,true);
  }
  while(millis() < currentTimeJump + periodJump)
  {
    
  }
}

void endGame()
{
  printScore();
  currentTimeEnd = millis();
  displaySadFace();
  while(millis() < currentTimeEnd + periodEnd)
  {
    
  }
  if (buttonState == 1)
  {
    restart();  
  }
}

void displaySadFace()
{ 
  int i, j;
  for(i = 0; i < 8; i++)
    for(j = 0; j < 8; j++)
      lc.setLed(0,i,j,sadFace[i][j]);
}

void restart()
{
  gameState = 0;
  lcd.clear();
  int i, j;
  for(i = 0; i < 8; i++)
    for(j = 0; j < 8; j++)
      lc.setLed(0,i,j,0);
  col = 7;
  birdBodyX = 0;
  birdBodyY = 4;
  birdHeadX = 1;
  birdHeadY = 3;
  wallPos1 = 0;
  wallPos2 = 1;
  wallPos3 = 6;
  wallPos4 = 7;
  score = 0;
  periodWalls = 100;
}
