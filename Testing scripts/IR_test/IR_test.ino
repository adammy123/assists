#define irLEDPin 9
#define irSensorPin 2
#define redLEDPin 5
#define greenLEDPin 4

int signal;

int irRead(int readPin, int triggerPin)
{
  int halfPeriod = 6; //one period at 38khZ is aproximately 26 microseconds
  int cycles = 200; //26 microseconds * 38 is more or less 1 millisecond
  int i;
  for (i=0; i <=cycles; i++)
  {
    digitalWrite(triggerPin, HIGH); 
    delayMicroseconds(halfPeriod);
    digitalWrite(triggerPin, LOW); 
    delayMicroseconds(halfPeriod - 1);     // - 1 to make up for digitaWrite overhead    
  }
  return digitalRead(readPin);
}

void setup() {
  pinMode(irLEDPin, OUTPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);  
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  signal = irRead(irSensorPin, irLEDPin);
  if(signal == 1){
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);
    
  }
  if(signal == 0){
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, HIGH);
  }
  Serial.println(signal);
}
