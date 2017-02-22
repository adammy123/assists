//soccer project v1
//Feb 9 2017
//Adam Muhammad

#include <elapsedMillis.h>
#include<Servo.h>
#define servoPin 9

Servo servo;


//variables to change for setup
int numBalls = 10;
int numGoals = 2;
int distance;
String username;
String choice;
elapsedMillis timeElapsed;
int interval = 5000;        //timeout in milliseconds

//variables for stats
int timeTaken;
int goalsHit;

//initialize variables for sensor readings
int sensorReading;
const int threshold1 = 100;
const int threshold2 = 50;

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

  servo.attach(servoPin);
  servo.write(90);
  
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
  delay(2000);
  int goal;

  //loop for number of balls
  for(int i=1; i<=numBalls; i++){
    Serial.print("\n----Ball number: ");
    Serial.println(i);
    launch();

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
          if (sensorReading >= threshold1){
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
          
            if (sensorReading >= threshold2){
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

  delay(5000);
    
  }//end for for loop

  //print out statistics
  printStats();
  delay(1000);
}

void printStats(){
  Serial.println("\n----End of session----");
  Serial.print("Accuracy: ");
  Serial.print(goalsHit);
  Serial.print(" out of ");
  Serial.print(numBalls);
  Serial.println(" hit.");
  Serial.print("Average time taken: ");
  Serial.print((float)timeTaken/goalsHit/1000);
  Serial.println(" seconds");
  Serial.println("Not bad!");
  Serial.println();
  delay(1000);
}

void launch(){
  Serial.println("----BALL LAUNCHED----");
  servo.write(145);
  delay(100);
  servo.write(90);
  delay(500);
}

