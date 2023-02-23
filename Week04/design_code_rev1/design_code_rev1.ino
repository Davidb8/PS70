
const int potPin = 2;  // define pin 3 for A-1A (PWM Speed)
int motor1 = 18;
int motor2 = 19;
int potValue;
int percent;

void setup() {
  pinMode(motor1, OUTPUT); // specify these pins as outputs
  pinMode(motor2, OUTPUT); // specify these pins as outputs
  pinMode(potPin, INPUT); // specify these pins as outputs
  
}

void loop() {
  potValue = analogRead(potPin);
  percent = map(potValue,0,1023,0,100);
  digitalWrite(motor1,HIGH);
  
  if (percent > 50){
    digitalWrite(motor1,HIGH);
    digitalWrite(motor2,HIGH);
  }
  else{
    digitalWrite(motor1,LOW);
    digitalWrite(motor2,LOW);
  }
}
