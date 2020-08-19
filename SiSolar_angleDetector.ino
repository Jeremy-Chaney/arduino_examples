/* **************************************
 * UT Design team 976 - "Si Solar"      *
 * Solar Angle Detector Arduino Code    *
 *                                      *
 * By Jeremy Chaney                     *
 * Electical Engineering Undergraduate  *
 * The University of Texas at Dallas    *
 *                                      *
 * Team Members:                        *
 * James Barnes - EE                    *
 * Jeremy Chaney - EE                   *
 * Joseph Holroyd - EE                  *
 * Woosung Jung - EE                    *
 * Tony Le - CE                         *
 * Jeffery Patton - EE                  *
 *                                      *
 * Advisor:                             *
 * Dr. Neal Skinner                     *
 *                                      *
 * Description:                         *
 * The solar angle detector is the      *
 * device which will output two         *
 * spherical readings: φ (phi) and θ    *
 * (theta). These spherical angles      *
 * tell the relative angle of a strong  *
 * light source to the detector.        *
 ****************************************
 ++++++++++++++++++++++++++++++++++++++++
 /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
 ---------Below: for future users--------
 \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
 ++++++++++++++++++++++++++++++++++++++++
 ****************************************
 * Description of angles:               *
 ****************************************
 * φ(phi) is the radial angle:          *
 * range: 0 - 360 degrees               *
 * resolution: 18 degrees               *
 * number of sensing 'rows': 20         *
 * phototransistors per 'row': 7        *
 *                                      *
 * 0 degrees represents the light       *
 * coming from the right side of        *
 * the board                            *
 *                                      *
 * 180 degrees represents the light     *
 * coming from the left side of         *
 * the board                            *
 ****************************************
 * θ(theta) is the drop angle:          *
 * range: 17 - 60 degrees               *
 * resolution: 10 degrees               *
 * number of sensing 'rows': 5          *
 * phototransistors per 'row': 20 for   *
 * 'rows' 1-3, 40 on 'rows' 4 and 5     *
 *                                      *
 * 0 degrees represents the light       *
 * coming from directly above           *
 * the board                            *
 *                                      *
 * 60 degrees represents the light      *
 * coming from the lowest detectable    *
 * angle above the horizon with respect *
 * to the board                         *
 ****************************************/

#include <Wire.h>   //enables I2C communication
#include <Servo.h>
#include <EEPROM.h> //enables storage of data without power

//define pins associated with different readings
#define phi1 24
#define phi2 26
#define phi3 28
#define phi4 30
#define phi5 32
#define phi6 34
#define phi7 36
#define phi8 38
#define phi9 40
#define phi10 42
#define phi11 25
#define phi12 27
#define phi13 29
#define phi14 31
#define phi15 33
#define phi16 35
#define phi17 37
#define phi18 39
#define phi19 41
#define phi20 43
#define theta1 45
#define theta2 47
#define theta3 49
#define theta4 51
#define theta5 53

//if HIGH, will output debugging prompts to serial monitor
bool debugPhi = HIGH;
bool debugTheta = HIGH;

//if HIGH, will spin panel across entire possible area on startup
bool clearAreaStartup = LOW;

//define variables to hold readings from pins
bool phiArray[20];
bool thetaArray[5];
int i;
int phiLast = sizeof(phiArray) / sizeof(bool);
bool phiLastState = LOW;

//define variables which will find average angle detected
int numDetected = 0;
float phiSum = 0;
float thetaSum = 0;

//define the resolutions between rows/columns
float phiRes = 18;
float thetaValues[5] = {17.1,30.8,41.5,51,60};

//holds the values to be outputed to Serial Monitor
float phi = 0;
float theta = 0;

Servo servoPhi, servoTheta;

void setup()
{
  //define all the input pins... ALL 25 of them
  pinMode(phi1, INPUT);
  pinMode(phi2, INPUT);
  pinMode(phi3, INPUT);
  pinMode(phi4, INPUT);
  pinMode(phi5, INPUT);
  pinMode(phi6, INPUT);
  pinMode(phi7, INPUT);
  pinMode(phi8, INPUT);
  pinMode(phi9, INPUT);
  pinMode(phi10, INPUT);
  pinMode(phi11, INPUT);
  pinMode(phi12, INPUT);
  pinMode(phi13, INPUT);
  pinMode(phi14, INPUT);
  pinMode(phi15, INPUT);
  pinMode(phi16, INPUT);
  pinMode(phi17, INPUT);
  pinMode(phi18, INPUT);
  pinMode(phi19, INPUT);
  pinMode(phi20, INPUT);
  pinMode(theta1, INPUT);
  pinMode(theta2, INPUT);
  pinMode(theta3, INPUT);
  pinMode(theta4, INPUT);
  pinMode(theta5, INPUT);
  servoPhi.attach(11);
  servoTheta.attach(12);

  if(clearAreaStartup == HIGH)
  {
    //servo startup test
    servoPhi.write(0);
    servoTheta.write(0);
    delay(500);
    servoPhi.write(180);
    delay(500);
    servoPhi.write(90);
    delay(500);
    servoTheta.write(180);
    delay(500);
    servoTheta.write(90);
    delay(500);
  }
  else
  {
    //regular starting position (straight up)
    servoPhi.write(90);
    delay(250);
    servoTheta.write(90);
    delay(250);
  }

  //start the serial monitor
  Serial.begin(9600);
}

void loop()
{
  //read the pins if you feel like it...
  phiArray[0] = digitalRead(phi1);
  phiArray[1] = digitalRead(phi2);
  phiArray[2] = digitalRead(phi3);
  phiArray[3] = digitalRead(phi4);
  phiArray[4] = digitalRead(phi5);
  phiArray[5] = digitalRead(phi6);
  phiArray[6] = digitalRead(phi7);
  phiArray[7] = digitalRead(phi8);
  phiArray[8] = digitalRead(phi9);
  phiArray[9] = digitalRead(phi10);
  phiArray[10] = digitalRead(phi11);
  phiArray[11] = digitalRead(phi12);
  phiArray[12] = digitalRead(phi13);
  phiArray[13] = digitalRead(phi14);
  phiArray[14] = digitalRead(phi15);
  phiArray[15] = digitalRead(phi16);
  phiArray[16] = digitalRead(phi17);
  phiArray[17] = digitalRead(phi18);
  phiArray[18] = digitalRead(phi19);
  phiArray[19] = digitalRead(phi20);
  thetaArray[0] = digitalRead(theta1);
  thetaArray[1] = digitalRead(theta2);
  thetaArray[2] = digitalRead(theta3);
  thetaArray[3] = digitalRead(theta4);
  thetaArray[4] = digitalRead(theta5);

  //calculate the φ value
  for(i = 0; i < (sizeof(phiArray)/sizeof(bool)); i = i+1)
  {
    //readings will be "LOW" if light is detected... blame semi-conductor physics for that one
    if(phiArray[i] == LOW)
    {
      if(debugPhi == HIGH)
      {
        Serial.print("Light detected at angle φ");
        Serial.println(i + 1);
      }
      else
        ;
        
      phiSum = (phiSum) + (i*phiRes);
      numDetected = numDetected + 1;
      
      if(i == phiLast - 1)
        phiLastState = HIGH;
      else
        phiLastState = LOW;
    }
    else
    {
      if(numDetected > 0)
        break;
      else
        ;
    }
  }

  if(phiLastState == LOW)
  {
    for(i = (sizeof(phiArray)/sizeof(bool)) - 1 ; i >= 0 ; i = i-1)
    {
      if(phiArray[i] == LOW)
      {
        phiSum = (phiSum) - (phiRes*((sizeof(phiArray)/sizeof(bool)) - 1 - i));
        numDetected = numDetected + 1;
      }
      else
        break;
    }
  }
  else
    ;

  //sets φ to average if at least one φ row detected light, else φ = 0
  if(numDetected > 0)
  {
    phi = phiSum / numDetected - 180;
    phiSum = 0;
    numDetected = 0;
  }
  else
    numDetected = 0;

  if(phi < 0)
    phi = phi + 360;
  else
    ;

  //calculate the θ value
  for(i = 0; i < (sizeof(thetaArray)/sizeof(bool)); i = i+1)
  {
    //readings will be "LOW" if light is detected... blame semi-conductor physics for that one
    if(thetaArray[i] == LOW)
    {
      if(debugTheta == HIGH)
      {
        Serial.print("Light detected at angle θ");
        Serial.println(i + 1);
      }
      else
        ;
        
      thetaSum = thetaSum + thetaValues[i];
      numDetected = numDetected + 1;
    }
    else
    ;
  }

  //sets θ to average if at least one θ row detected light, else θ = 0
  if(numDetected > 0)
  {
    theta = thetaSum / numDetected;
    thetaSum = 0;
  }
  else
    numDetected = 0;

  //move the servos if light is detected
  if(numDetected > 0)
  {
    if(phi < 180)
    {
      servoPhi.write(phi);
      delay(250);
      servoTheta.write(90-theta);
    }
    else
    {
      servoPhi.write(phi - 180);
      delay(250);
      servoTheta.write(90+theta);
    }
    numDetected = 0;
  }
  else
    numDetected = 0;
  
  //print results to serial monitor
  Serial.print("φ = ");
  Serial.print(phi);
  Serial.print(" degrees\n");
  Serial.print("θ = ");
  Serial.print(theta);
  Serial.print(" degrees");
  for(i = 0; i < 3; i = i + 1)
  {
    Serial.println();
  }

  //delays for 2 seconds to view results
  delay(500);
}
