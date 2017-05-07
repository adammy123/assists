#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

bool isGoal = false;

//int ledPin = 2;
char resp;
int choice = 22;
int buttonPin = 2;
int count = 0;
bool writing;
bool timeout;
int started_waiting_at;

void setup() {

  pinMode(buttonPin, INPUT);
  Serial.begin(115200);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, n_pipes[0]);
  radio.openReadingPipe(2, n_pipes[1]);
  radio.startListening();

  delay(1000);

  Serial.println("Ready to go");

}

void loop() {
    choice = digitalRead(buttonPin);
    resp = 'x';
    Serial.println(choice);

    radio.openWritingPipe(b_pipes[choice]);
    radio.stopListening();
    radio.write( &choice, sizeof(choice) );
    radio.startListening();
    
  
    started_waiting_at = millis();
    timeout = false;
    while(!radio.available() && !timeout){
      if(millis() - started_waiting_at > 250){
        timeout = true;
      }  
    }

    if(timeout){
      //Serial.println("Timeout");
    }
    else{
      radio.read( &resp, sizeof(resp) );
  }

  Serial.println(resp);

  delay(100);

  
}
      


