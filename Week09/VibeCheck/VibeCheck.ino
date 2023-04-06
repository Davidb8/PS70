#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Button.h>

#define OLED_SDA 16
#define OLED_SCL 17
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDRESS 0x3C

#define BUTTON_1 11
#define BUTTON_2 12
#define BUTTON_3 13
#define BUTTON_4 14

#define LED 37

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Button button1(BUTTON_1);
Button button2(BUTTON_2);
Button button3(BUTTON_3);
Button button4(BUTTON_4);

void setup() {

  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();


  Serial.begin(9600);
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  pinMode(LED, OUTPUT);

  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);


  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Button 1 pressed");
  display.display(); // Update screen with each newly-drawn rectangle
  delay(2000);

}

void loop() {
  button1.read();
  button2.read();
  button3.read();
  button4.read();

  if (button1.pressed()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Button 1 pressed");
    display.display();
  }

  if (button2.pressed()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Button 2 pressed");
    display.display();
  }

  if (button3.pressed()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Button 3 pressed");
    display.display();
  }

  if (button4.pressed()) {
    digitalWrite(LED, HIGH);
    delay(1000);
  } else {
    digitalWrite(LED, LOW);
  }
  
  // digitalWrite(LED, HIGH);
  // delay(1000);
  // digitalWrite(LED, LOW);
  // delay(1000);

}
