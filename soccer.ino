//soccer project v1
//Feb 9 2017
//Adam Muhammad

#include <elapsedMillis.h>

//variables to change for setup
int numBalls = 10;
int numGoals = 2;
int timeout = 5;  //seconds
int distance;
String username;
String choice;
elapsedMillis timeElapsed;
int interval = 5000;

//variables for stats
int timeTaken;
int goalsHit;

//initialize variables for sensor readings
int sensorReading;
const int threshold = 200;

//initialize pins for sensors and LEDs
const int sensor1   = A0;
const int sensor2   = A1;
const int greenPin1 = 5;
const int redPin1   = 6;
const int greenPin2 = 7;
const int redPin2   = 8;

void setup() {
  // put your setup code here, to run once:
  pinMode(greenPin1, OUTPUT);
  pinMode(redPin1, OUTPUT);
  pinMode(greenPin2, OUTPUT);
  pinMode(redPin2, OUTPUT);

  //turn off LEDs at the beginning of setup;
  digitalWrite(greenPin1, LOW);
  digitalWrite(redPin1, LOW);
  digitalWrite(greenPin2, LOW);
  digitalWrite(redPin2, LOW);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //user input credentials
  Serial.print("Please enter your name: ");
  while(Serial.available() == 0){}
  username = Serial.readString();
  Serial.println(username);
  Serial.print("Welcome, ");
  Serial.println(username); 

  Serial.print("Choose mode: A | B : ");
  while(Serial.available() == 0){}
  choice = Serial.readString();
  Serial.println(choice);
  Serial.print("You have chosen mode ");
  Serial.println(choice);

  Serial.println("Initializing...");
  int goal;

  //loop for number of balls
  for(int i=1; i<=numBalls; i++){
    Serial.print("Ball number: ");
    Serial.println(i);
    Serial.println("----BALL LAUNCHED----");
    delay(500);

    //choose a goal randomly
    goal = random(numGoals)+1;
    
    timeElapsed = 0;
    bool hit = false;
    
    switch(goal){
      case 1:
        digitalWrite(greenPin1, HIGH);
        
        while (timeElapsed < interval){
          
          sensorReading = analogRead(sensor1);

          //if sensor registers a HIT
          if (sensorReading >= threshold){
            timeTaken += timeElapsed;
            goalsHit += 1;
            Serial.println("HIT!!!");
            hit = true;
            break;
          }
        }
        digitalWrite(greenPin1, LOW);

        //if fail to hit before timeout
        if(!hit){
          digitalWrite(redPin1, HIGH);
          Serial.println("miss...");
          delay(1000);
          digitalWrite(redPin1, LOW);
        }
        break;

        case 2:
          digitalWrite(greenPin2, HIGH);
          while (timeElapsed < interval){
          
            sensorReading = analogRead(sensor2);
          
            if (sensorReading >= threshold){
              timeTaken += timeElapsed;
              goalsHit += 1;
              Serial.println("HIT!!!");
              hit = true;
              break;
            }
          }
          digitalWrite(greenPin2, LOW);
          if(!hit){
            digitalWrite(redPin2, HIGH);
            Serial.println("miss...");
            delay(1000);
            digitalWrite(redPin2, LOW);
          }
          break;
    }//end of switch

  delay(2000);
    
  }//end for for loop

  //print out statistics
  Serial.println("----End of session----");
  Serial.print("Accuracy: ");
  Serial.println((float)goalsHit/numBalls);
  Serial.print("Average time taken: ");
  Serial.println(timeTaken/goalsHit);
  delay(1000);
}
