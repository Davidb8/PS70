#include <TFT_eSPI.h>
#include <SPI.h>

#define RELAY_PIN 25 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define LOOP_PERIOD 35 // Display updates every 35 ms

TFT_eSPI tft = TFT_eSPI(); // Initialize TFT display object

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

const int BAR_X = 50;
const int BAR_Y = 120;
const int BAR_W = 220;
const int BAR_H = 20;
const int BAR_BORDER_RADIUS = 5;
const int BAR_COLOR = TFT_WHITE;
const int BAR_BACKGROUND_COLOR = TFT_BLACK;

const int TEXT_X = 50;
const int TEXT_Y = 50;
const int TEXT_SIZE = 2;
const int TEXT_COLOR = TFT_WHITE;

const int TIME_TO_POUR_ONE_SHOT = 1000; // in milliseconds

long animation_start_time = 0;
long previousMillisPour = 0;
float progress = 0;

uint32_t updateTime = 0;       // time for next update

//bool FORCE_UPDATE = TRUE;

void pourAnimationInitial(int shots){
  // Clear screen
  tft.fillScreen(TFT_BLACK);

  // Print "Pouring {shots} Shot" text
  String text = "Pouring " + String(shots) + " Shot";
  int textWidth = tft.textWidth(text);
  int textX = (SCREEN_WIDTH - textWidth) / 2;
  tft.setCursor(textX, TEXT_Y);
  tft.print(text);

  // Draw progress bar background
  tft.fillRoundRect(BAR_X, BAR_Y, BAR_W, BAR_H, BAR_BORDER_RADIUS, BAR_BACKGROUND_COLOR);

}

void pourAnimation(int shots) {
  int TIME_TO_POUR = TIME_TO_POUR_ONE_SHOT * shots;
  
  if ( (int) progress == 100){
    pourAnimationInitial(shots); 
    
  }

  // Draw progress bar
  int currentTime = millis();
  if (currentTime - animation_start_time <= TIME_TO_POUR){
    progress = map(currentTime - animation_start_time, 0, TIME_TO_POUR, 0, 100);
    // Draw progress bar
    int barWidth = (int) (progress / 100 * BAR_W);
    tft.fillRoundRect(BAR_X, BAR_Y, barWidth, BAR_H, BAR_BORDER_RADIUS, BAR_COLOR);

    if ((updateTime <= millis())){
      updateTime = millis() + LOOP_PERIOD;
      // Clear progress percentage text and draw new text
      String percentageText = String((int) progress) + "%";
      int percentageTextWidth = tft.textWidth(percentageText);
      int percentageTextX = (SCREEN_WIDTH - percentageTextWidth) / 2;
      tft.fillRect(percentageTextX, BAR_Y + BAR_H + 10, percentageTextWidth, TEXT_SIZE * 8, TFT_BLACK);
      tft.setCursor(percentageTextX, BAR_Y + BAR_H + 10);
      tft.print(percentageText);
    }
  }
  else{
    progress = 100;
    String percentageText = String((int)progress) + "%";
    int percentageTextWidth = tft.textWidth(percentageText);
    int percentageTextX = (SCREEN_WIDTH - percentageTextWidth) / 2;
    tft.fillRect(percentageTextX, BAR_Y + BAR_H + 10, percentageTextWidth, TEXT_SIZE * 8, TFT_BLACK);
    tft.setCursor(percentageTextX, BAR_Y + BAR_H + 10);
    tft.print("100%");
    animation_start_time = currentTime;
  }


}
  // while (currentTime <= TIME_TO_POUR) {
  //   // Calculate progress percentage
  //   float progress = map(currentTime, 0, TIME_TO_POUR, 0, 100);

  //   // Draw progress bar
  //   int barWidth = (int) (progress / 100 * BAR_W);
  //   tft.fillRoundRect(BAR_X, BAR_Y, barWidth, BAR_H, BAR_BORDER_RADIUS, BAR_COLOR);

  //   // Clear progress percentage text and draw new text
  //   String percentageText = String((int) progress) + "%";
  //   int percentageTextWidth = tft.textWidth(percentageText);
  //   int percentageTextX = (SCREEN_WIDTH - percentageTextWidth) / 2;
  //   tft.fillRect(percentageTextX, BAR_Y + BAR_H + 10, percentageTextWidth, TEXT_SIZE * 8, TFT_BLACK);
  //   tft.setCursor(percentageTextX, BAR_Y + BAR_H + 10);
  //   tft.print(percentageText);

  //   // Update current time
  //   currentTime = millis() - startTime;
  // }
  // String percentageText = String(100) + "%";
  // int percentageTextWidth = tft.textWidth(percentageText);
  // int percentageTextX = (SCREEN_WIDTH - percentageTextWidth) / 2;
  // tft.fillRect(percentageTextX, BAR_Y + BAR_H + 10, percentageTextWidth, TEXT_SIZE * 8, TFT_BLACK);
  // tft.setCursor(percentageTextX, BAR_Y + BAR_H + 10);
  // tft.print("100%");
//}

void pourShot(int shots){
  unsigned long currentMillisPour = millis();
  digitalWrite(RELAY_PIN, LOW); // turn on pump 4 seconds
  if(currentMillisPour - previousMillisPour > TIME_TO_POUR_ONE_SHOT * shots){
    digitalWrite(RELAY_PIN, HIGH);  // turn off pump 4 seconds
    pourAnimation(shots);
    delay(1000);
    progress = 100;

    previousMillisPour = millis();
    animation_start_time = millis();
    Serial.println("Shot Poured");
  }

  
  
}


void setup() {
  Serial.begin(9600); // For debug
  SPI.begin(); // init SPI bus
  tft.init(); // Initialize TFT display
  tft.setRotation(3); // Set display rotation (0 to 3)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 
  tft.fillScreen(TFT_BLACK);
  Serial.println("Black print 1");
  //delay(10000);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Black print 2");
  //delay(10000); 
  // Set font size and color
  tft.setTextSize(TEXT_SIZE);
  tft.setTextColor(TEXT_COLOR);

  updateTime = millis(); // Next update time
  pourAnimationInitial(1); 
  previousMillisPour = millis();
  animation_start_time = millis();

}

void loop() {
  pourAnimation(1);
  pourShot(1);
  
}
