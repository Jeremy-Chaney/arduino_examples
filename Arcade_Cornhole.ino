/*
 * Arcade-Style Cornole for Arduino Mega
 * by Jeremy Chaney and Connor MacLeod
 */
//Header Files to include
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_LEDBackpack.h>
#include "pitches.h"
#include <Wire.h>
#include <EEPROM.h>

//pin definitions
#define buttonLight 22  //Button Light is tied to pin 22
#define IR1 24          //IR sensor 1 will be tied to pin 24
#define IR2 26          //IR sensor 2 will be tied to pin 26
#define IR3 28          //IR sensor 3 will be tied to pin 28
#define button 23       //Start button will be tied to pin 23
/*
 * Speaker is tied to pin 7.
 * list of frequencies can be found at
 * https://pages.mtu.edu/~suits/notefreqs.html
 */

/*
 * Defines I2C 7-segment names
 * Each will have VDD, GND
 * Each SCL will be on pin 21
 * Each SDA will be on pin 20
*/
Adafruit_7segment timerDisp = Adafruit_7segment();
Adafruit_7segment scoreDisp = Adafruit_7segment();
Adafruit_7segment highScoreDisp = Adafruit_7segment();

int highScoreAddr = 0;
int buttonLightState = 0;
int buttonState = 0;
int buttonLastState = 0;
int buttonCount = 0;

int IR1State = 0;
int IR1LastState = 0;
int IR1Count = 0;

int IR2State = 0;
int IR2LastState = 0;
int IR2Count = 0;

int IR3State = 0;
int IR3LastState = 0;
int IR3Count = 0;

//define variables for the timer
float gameTimer = 0;
float gameTimerEnd = 0;
float gameLength = 60000; //input length of game in ms
float timer = 0;
float timeStart = 0;
int timerStart = 0;
int Score = 0;
int highScore = 0;
int Points1 = 10;       //define points for lowest scoring hole
int Points2 = 20;       //define points for middle scoring hole
int Points3 = 50;       //define points for highest scoring hole

//Game will be high when a game is started
int Game = 0;

void setup() 
{
  //Display setup
  timerDisp.begin(0x70);
  scoreDisp.begin(0x72);
  highScoreDisp.begin(0x71);
  
  //Button Setup
  pinMode(buttonLight , OUTPUT);
  pinMode(button , INPUT);
  pinMode(IR1 , INPUT);
  digitalWrite(buttonLight , HIGH);
  digitalWrite(Game , HIGH);
  Serial.begin(9600);
  timerDisp.print(0);
  scoreDisp.print(0);
  EEPROM.get(highScoreAddr , highScore);
  highScoreDisp.print(highScore);
}

void loop() 
{
  buttonCount = 0;
  //ensure that the game starts with IR Last states = IR current states
  IR1State = digitalRead(IR1);
  IR1LastState = IR1State;
  IR2State = digitalRead(IR2);
  IR2LastState = IR2State;
  IR3State = digitalRead(IR3);
  IR3LastState = IR3State;
  
  //fetch the highest score from EEPROM
  EEPROM.get(highScoreAddr , highScore);
  highScoreDisp.print(highScore);

  //waiting Jeopardy :)
/*
 * while(digitalRead(button) == LOW)
  {
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 554 , 125);
    delay(250);
    tone(7 , 587 , 125);
    delay(250);
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 659 , 250);
    delay(500);
    
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 1109 , 500);
    delay(750);
    tone(7 , 988 , 125);
    delay(125);
    tone(7 , 880 , 125);
    delay(125);
    tone(7 , 831 , 125);
    delay(125);
    tone(7 , 740 , 125);
    delay(125);
    tone(7 , 698 , 125);
    delay(125);
    
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 554 , 125);
    delay(250);
    tone(7 , 587 , 125);
    delay(250);
    tone(7 , 659 , 250);
    delay(500);
    tone(7 , 880 , 250);
    delay(500);
    tone(7 , 659 , 250);
    delay(500);

    tone(7 , 880 , 500);
    delay(750);
    tone(7 , 740 , 125);
    delay(250);
    tone(7 , 659 , 500);
    delay(500);
    tone(7 , 587 , 500);
    delay(500);
    tone(7 , 554 , 500);
    delay(500);
    tone(7 , 494 , 500);
    delay(500);
    tone(7 , 440 , 500);
    delay(1000);
  }
 */
  
  //detects button pushes
  buttonState = digitalRead(button);
  if(buttonState != buttonLastState)
  {
    Serial.println("Button was pushed");
    buttonCount = buttonCount + 1;
    Game = LOW;
  }
  else
  {
    ;
  }
  buttonLastState = buttonState;

  //detects if button was pushed and allows game to start
  delay(50); //protects "bouncing input"
  if(buttonCount % 3 == 1)
  {
    //start button was pushed
    buttonCount = buttonCount + 1;
    Serial.print("Start button was pushed\n");
    Serial.print("Number of button pushes: ");
    Serial.println(buttonCount);
    Game = LOW;
  }
  else
  {
    Game = HIGH;
  }
  
  //Game code
  if(Game && buttonState == LOW)
  {
    Serial.print("Game is Running\n");
    gameTimerEnd = millis();
    gameTimerEnd = gameTimerEnd + gameLength;
    gameTimer = millis();
    timeStart = millis();
    timeStart = timeStart + 3000;

    //Mario Cart style starting code
    while(millis() < timeStart)
    {
      timerStart = (timerStart - millis());
      timer = (timeStart - millis())/1000;
      timerDisp.print(timer);
      if((timer >= .95)&&(timer <= 1.05))
      {
        tone(7 , 440 , 250);
      }
      else if((timer >= 1.95)&&(timer <= 2.05))
      {
        tone(7 , 440 , 250);
      }
      else if((timer >= 2.95)&&(timer <= 3.05))
      {
        tone(7 , 440 , 250);
      }
      else
      {
        ;
      }
    }

    tone(7 , 880 , 500);
    
    while(gameTimer <= gameTimerEnd)          //this code will run for the duration of the length of game
    {
      timer = (gameTimerEnd - gameTimer)/1000;//timer value will be normalized to show only "seconds"
      timerDisp.print(timer);
      Serial.println(timer);
      scoreDisp.print(Score);
      IR1State = digitalRead(IR1);
      IR2State = digitalRead(IR2);
      IR3State = digitalRead(IR3);
      if(IR1State != IR1LastState)
      {
        Serial.print("IR1 detects score!\n");
        Score = Score + Points1/2;  //divides by two as sensors will detect beam breaking and un-breaking
        Serial.println(Score);
        IR1LastState = IR1State;
        tone(7,Note_A4,200);
        delay(50);
      }
      else
      {
        ;
      }
      if(IR2State != IR2LastState)
      {
        Serial.print("IR2 detects score!\n");
        Score = Score + Points2/2;  //divides by two as sensors will detect beam breaking and un-breaking
        Serial.println(Score);
        IR2LastState = IR2State;
        tone(7,Note_CS5,200);
        delay(50);
      }
      else
      {
        ;
      }
      if(IR3State != IR3LastState)
      {
        Serial.print("IR3 detects score!\n");
        Score = Score + Points3/2;  //divides by two as sensors will detect beam breaking and un-breaking
        Serial.println(Score);
        IR3LastState = IR3State;
        tone(7,Note_E5,125);
        delay(125);
        tone(7,Note_A4,125);
        delay(125);
        tone(7,Note_CS5,125);
        delay(125);
        tone(7,880,250);
        delay(50);
      }
      else
      {
        ;
      }
      timerDisp.writeDisplay();
      scoreDisp.writeDisplay();
      gameTimer = millis();
    }
    
    //checks if current score is new highest score
    if(Score >= highScore)
    {
      highScore = Score;
      tone(7 , 523 , 500);
      delay(500);
      tone(7 , 392 , 250);
      delay(375);
      tone(7 , 392 , 125);
      delay(125);
      tone(7 , 440 , 500);
      delay(500);
      tone(7 , 392 , 500);
      delay(1000);
      tone(7 , 494 , 250);
      delay(375);
      tone(7 , 523 , 250);      
    }
    else
    {
      tone(7 , 440 , 1000);
      delay(500);
      tone(7 , 414 , 1000);
      delay(500);
      tone(7 , 392 , 2000);
    }
    
    //case to reset high score
    if(Score == 6900) //my program my numbers
    {
      highScore = 0;
    }
    else
    {
      ;
    }
    //update the highest score value
    EEPROM.put(highScoreAddr,highScore);
    highScoreDisp.print(highScore);
    highScoreDisp.writeDisplay();
    
    Serial.print("Game is done\n");
    Score = 0; //reset the current score
    Game = HIGH;
  }
  else
  {
    ;
  }
}
