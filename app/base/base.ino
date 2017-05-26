#include <Wire.h>
const int totalBalls = 5;
int target[totalBalls];
int hit[totalBalls];
unsigned long timeTaken[totalBalls];
bool state = false;
int sessionNumber = 0;
int temp;

void setup() {
  // put your setup code here, to run once:
   Wire.begin(7);
   Wire.onReceive(receiveEvent);
   Wire.onRequest(requestEvent);
   Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  if(state){
    Serial.println("true");
  }
  else{
    Serial.println("false");
  }
}

void receiveEvent(byte command){
  if (command == 1){
    // begin session
    for (int i = 0; i < totalBalls; i++){
      target[i] = i;
      hit[i] = 1;
      timeTaken[i] = 200;
    }
    state = true;
  }
}

void requestEvent(){
  sessionNumber += 1;
  Wire.write(sessionNumber);
  
  for(int i=0; i<totalBalls; i++){
    Wire.write(target[i]);
  }
  for(int i=0; i<totalBalls; i++){
    Wire.write(hit[i]);
  }
  for(int i=0; i<totalBalls; i++){
    temp = int(timeTaken[i])+sessionNumber;
    Wire.write(temp);
  }
}

