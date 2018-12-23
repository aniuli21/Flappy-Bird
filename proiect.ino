#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define dataInPin 12
#define clkPin 11
#define loadPin 10
#define deviceNumbers 1
#define V0_PIN 9 
#define RS 2
#define EN 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7 
LedControl lc = LedControl(dataInPin, clkPin, loadPin, deviceNumbers); 

//DIN, CLK, LOAD, No. DRIVER
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
 
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
int periodFall = 100;
int periodJump = 100;
int periodWalls = 150;
int periodEnd = 500;
int buttonState;
int highScore;
int addr = 0;

unsigned long currentTimeFall = 0;
unsigned long currentTimeJump = 0;
unsigned long currentTimeWallsShow = 0;
unsigned long currentTimeEnd = 0;
unsigned long currentTimeWallsHide = 0;

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
  analogWrite(V0_PIN, 45); 
  pinMode(buttonPin, INPUT);
  highScore=(int)EEPROM.read(addr);
  Serial.begin(9600);
}

void loop()
{ 
  play();
  Serial.println(buttonState);
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
    if (birdBodyY < 8 && (birdHeadY != wallPos1 || birdHeadX != col) 
        && (birdHeadY != wallPos2 || birdHeadX != col) 
        && (birdHeadY != wallPos3 || birdHeadX != col) 
        && (birdHeadY != wallPos4 || birdHeadX != col) 
        && (birdBodyY != wallPos1 || birdBodyX != col) 
        && (birdBodyY != wallPos2 || birdBodyX != col) 
        && (birdBodyY != wallPos3 || birdBodyX != col) 
        && (birdBodyY != wallPos4 || birdBodyX != col))
    {
      continueGame();  
    }
    else
    {
      endGame();
    }
  }
}
//used to display the option to start game when 
void mainMenu()
{
  lc.setLed(0,birdHeadY,birdHeadX,true);
  lc.setLed(0,birdBodyY,birdBodyX,true);
  lcd.setCursor(0,0);
  lcd.print("---START GAME---");
}
//used to run the game while not losing
void continueGame()
{ 
  if (millis() > currentTimeWallsShow + periodWalls)
  {
    currentTimeWallsShow = millis();
    col--;
    hideWalls();
  }
  showWalls();
  if (buttonState == 0)
  {
    birdFall();
  }
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
//used to display stats on lcd after losing
void printScore()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Your score is:");
  lcd.print(score);
  lcd.setCursor(0,1);
  lcd.print("High score: ");
  lcd.print(highScore);                  
}
//used to update the score
void raiseScore()
{
  score++;
  lcd.clear();
  //lcd.print(score);
  if (score % 5 == 0 && score != 0 && periodWalls != 0)
  {
    setDifficulty();
  }
}
//used to increase difficulty every 5 points
void setDifficulty()
{
  periodWalls = periodWalls - 20;
}
//used to turn on the wall
void showWalls()
{ 
  lc.setLed(0,wallPos1,col,true);
  lc.setLed(0,wallPos2,col,true);
  lc.setLed(0,wallPos3,col,true);
  lc.setLed(0,wallPos4,col,true);
}
//used to turn off the wall from one position behind
void hideWalls()
{ 
  lc.setLed(0,wallPos1,col+1,false);
  lc.setLed(0,wallPos2,col+1,false);
  lc.setLed(0,wallPos3,col+1,false);
  lc.setLed(0,wallPos4,col+1,false);  
}
//used to generate random walls
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
  if (currentTimeFall + periodFall < millis())
  {
    currentTimeFall = millis();
  
    lc.setLed(0,birdHeadY,birdHeadX,false);
    lc.setLed(0,birdBodyY,birdBodyX,false);
      
    birdBodyY++;
    birdHeadY++;
  
    lc.setLed(0,birdHeadY,birdHeadX,true);
    lc.setLed(0,birdBodyY,birdBodyX,true);
  }
}

void jump()
{ 
  if (currentTimeJump + periodJump < millis())
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
  }
}

void endGame()
{
  printScore();
  updateHighScore();
  currentTimeEnd = millis();
  displaySadFace();
  while(millis() < currentTimeEnd + periodEnd)
  {
    //wait periodEnd
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
  periodWalls = 150;
}

void updateHighScore()
{
  if (score > highScore)
     {
       highScore = score;
     }
  EEPROM.write(addr, (byte)highScore);
}
