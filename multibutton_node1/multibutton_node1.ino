#include <SPI.h>
#include "RF24.h"

RF24 radio(7,8);

const uint64_t b_pipes[6] = {0x0F0F0F0F11LL, 0x0F0F0F0F22LL};  
const uint64_t n_pipes[6] = {0x1F1F1F1F11LL, 0x1F1F1F1F22LL};

//int buttonPin = 2;
int choice = 33;
char resp = 'a';
int count = 0;
bool writing = false;

void setup() {

//  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  delay(1000);

  radio.begin();
  radio.setChannel(108);

  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, b_pipes[0]);
  radio.startListening();
  delay(1000);
  Serial.println("Ready to go");

}

void loop() {

  while(!radio.available()){
    }

  while(radio.available()){
    radio.read( &choice, sizeof(choice) );
    Serial.println(choice);
  }

  radio.stopListening();
  radio.openWritingPipe(n_pipes[0]);

  radio.write( &resp, sizeof(resp) );

  radio.startListening();
}
