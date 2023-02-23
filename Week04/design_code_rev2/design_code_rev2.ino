const int potPinAPower = 2;
const int potPinBPower = 3;
const int potPinAData = 2;
const int potPinBData = 2;
const int switchPin = 5;
int motorA = 18;
int motorB = 19;

int switchValue;
int potValueA;
int potValueB;
int percentA;
int percentB;

void setup() {
  pinMode(motorA, OUTPUT); // specify these pins as outputs
  pinMode(motorB, OUTPUT); // specify these pins as outputs
  pinMode(potPinAPower, OUTPUT); // specify these pins as outputs
  pinMode(potPinBPower, OUTPUT); // specify these pins as outputs
  pinMode(switchPin, INPUT); // specify these pins as inputs
  pinMode(potPinAData, INPUT); // specify these pins as inputs
  pinMode(potPinBData, INPUT); // specify these pins as inputs

}

void loop() {
  switchValue = digitalRead(switchPin)

  // if switch closed --> power potentiometers
  if (switchValue > 0){
    digitalWrite(potPinAPower,HIGH)
    digitalWrite(potPinBPower,HIGH)
  }
  else {
    digitalWrite(potPinAPower,LOW)
    digitalWrite(potPinBPower,LOW)
  }

  // read potentiometers
  potValueA = analogRead(potPinAData);
  potValueB = analogRead(potPinBData);

  // map to values
  percentA = map(potValueA,0,1023,-50,50);
  percentB = map(potValueB,0,1023,-50,50);

  // Analog write values to power motors
  analogWrite(motorA,percentA)
  analogWrite(motorB,percentB)
  
}
