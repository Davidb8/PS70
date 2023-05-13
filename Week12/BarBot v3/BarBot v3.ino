#include <TFT_eSPI.h>
#include <Button.h>
#include <stdio.h>

TFT_eSPI tft = TFT_eSPI();  // Create a TFT_eSPI object

const int buttonPowerPin = 32;   // Power button pin  // BLUE WIRE
const int buttonUpPin = 33;      // Up button pin  // YELLOW WIRE
const int buttonDownPin = 13;    // Down button pin // WHITE WIRE
const int buttonOKPin = 16;      // OK button pin // BROWN WIRE

Button buttonPower(buttonPowerPin);   // Power button object 
Button buttonUp(buttonUpPin);         // Up button object
Button buttonDown(buttonDownPin);     // Down button object
Button buttonOK(buttonOKPin);         // OK button object

const int ledPin = 17;     // LED pin
bool poweredOn = false;    // Power on state flag

int currentMenu = 0;       // Current selected menu
int currentOption = 0;     // Current selected menu option



// Menu options
const char* optionsInitial[] = {"Shot","Cocktail","Juices"};// Menu 0 

const char* optionsShot[] = {"Vodka","Rum","Gin","Whiskey","Soda Water","Lime Juice","Back"}; // Menu 1

const char* optionsCocktail[] = {"Mojito","Gin and Tonic","Moscow Mule","Back"}; // Menu 2

const char* optionsJuice[] = {"Lemonade","Pink Lemonade","Limeade","COMBO!!!","Back"}; // Menu 3

const int menuSizes[] = {3,7,4,5};

// Define Pumps
#define VODKA_PIN 5 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define GIN_PIN 26 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define RUM_PIN 21 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define WHISKEY_PIN 22 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define LIME_PIN 1 // ESP32 pin GIOP11, which connects to the IN pin of relay
#define SODA_PIN 25 // ESP32 pin GIOP11, which connects to the IN pin of relay


void setup() {
  Serial.begin(9600); // For debug

  pinMode(ledPin, OUTPUT);    // Set the LED pin as output
  digitalWrite(ledPin, LOW);  // Turn off the LED initially

  pinMode(VODKA_PIN, OUTPUT);
  pinMode(GIN_PIN, OUTPUT);
  pinMode(RUM_PIN, OUTPUT);
  pinMode(WHISKEY_PIN, OUTPUT);
  pinMode(LIME_PIN, OUTPUT);
  pinMode(SODA_PIN, OUTPUT);
  digitalWrite(VODKA_PIN, HIGH);  // Turn off the LED initially
  digitalWrite(GIN_PIN, HIGH);  // Turn off the LED initially
  digitalWrite(RUM_PIN, HIGH);  // Turn off the LED initially
  digitalWrite(WHISKEY_PIN, HIGH);  // Turn off the LED initially
  digitalWrite(LIME_PIN, HIGH);  // Turn off the LED initially
  digitalWrite(SODA_PIN, HIGH);  // Turn off the LED initially


  tft.begin();                // Initialize the TFT display
  tft.setRotation(2);         // Set the display rotation (if needed)
  tft.fillScreen(TFT_BLACK);  // Set the background color

  buttonPower.begin();   // Initialize the power button
  buttonUp.begin();      // Initialize the up button
  buttonDown.begin();    // Initialize the down button
  buttonOK.begin();      // Initialize the OK button
}

void drawMenu() {
  tft.fillScreen(TFT_BLACK);  // Clear the screen

  // Draw the menu options
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);

  if (currentMenu == 0) {
    tft.setCursor(10, 20);
    tft.println("Select Mode");
  }
  else if (currentMenu == 1) {
    tft.setCursor(10, 20);
    tft.println("Select Shot");
  }
  else if (currentMenu == 2) {
    tft.setCursor(10, 20);
    tft.println("Select Cocktail");
  }
  else if (currentMenu == 3) {
    tft.setCursor(10, 20);
    tft.println("Select Juice");
  }
  
  if (currentMenu == 0) {
    for (int i = 0; i < getNumOptions(currentMenu); i++) {
      if (i == currentOption) {
        tft.setTextColor(TFT_YELLOW);  // Highlight the selected option
      } else {
        tft.setTextColor(TFT_WHITE);
      }
      tft.setCursor(10, 70 + i * 30);
      tft.println(optionsInitial[i]);
    }
  }
  
  else if (currentMenu == 1) {
    for (int i = 0; i < getNumOptions(currentMenu); i++) {
      if (i == currentOption) {
        tft.setTextColor(TFT_YELLOW);  // Highlight the selected option
      } else {
        tft.setTextColor(TFT_WHITE);
      }
      tft.setCursor(10, 70 + i * 30);
      tft.println(optionsShot[i]);
    }
  }

  else if (currentMenu == 2) {
    for (int i = 0; i < getNumOptions(currentMenu); i++) {
      if (i == currentOption) {
        tft.setTextColor(TFT_YELLOW);  // Highlight the selected option
      } else {
        tft.setTextColor(TFT_WHITE);
      }
      tft.setCursor(10, 70 + i * 30);
      tft.println(optionsCocktail[i]);
    }
  }

  else if (currentMenu == 3) {
    for (int i = 0; i < getNumOptions(currentMenu); i++) {
      if (i == currentOption) {
        tft.setTextColor(TFT_YELLOW);  // Highlight the selected option
      } else {
        tft.setTextColor(TFT_WHITE);
      }
      tft.setCursor(10, 70 + i * 30);
      tft.println(optionsJuice[i]);
    }
  }

}


void handleButtonPress() {
  if (buttonPower.pressed()) {
    poweredOn = !poweredOn;  // Toggle the powered on state

    if (poweredOn) {
      digitalWrite(ledPin, HIGH);  // Turn on the LED
      currentMenu = 0;             // Reset the current menu
      currentOption = 0;           // Reset the current option
      drawMenu();                  // Draw the initial menu
    } else {
      digitalWrite(ledPin, LOW);   // Turn off the LED
      tft.fillScreen(TFT_BLACK);   // Clear the screen
    }
    Serial.println("POWER BUTTON");

  }

  if (poweredOn) {
    if (buttonUp.pressed()) {
      if (currentOption > 0) {
        currentOption--;
      } else {
        currentOption = getNumOptions(currentMenu) - 1;
      }
      drawMenu();
      Serial.println("UP BUTTON");
    }

    if (buttonDown.pressed()) {
      if (currentOption < getNumOptions(currentMenu) - 1) {
        currentOption++;
      } else {
        currentOption = 0;
      }
      drawMenu();
      Serial.println("DOWN BUTTON");
    }

    if (buttonOK.pressed()) {
      if (currentMenu != 0 && currentOption == getNumOptions(currentMenu) - 1) {
        // Navigate back to the previous menu
        currentMenu = 0;
        currentOption = 0;
      } 
      
      else {
        // goes to next menu if in main
        if (currentMenu == 0) {
          currentMenu = currentOption + 1;
          currentOption = 0;
        } 
        // Print the selected option to the serial monitor
        
        else if (currentMenu == 1) {

          const char* selectedDrink = optionsShot[currentOption];
      
          // Turn on the corresponding drink pin for 1 second
          if (strcmp(selectedDrink, "Vodka") == 0) {
            digitalWrite(VODKA_PIN, LOW);
            delay(10000);
            digitalWrite(VODKA_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Gin") == 0) {
            digitalWrite(GIN_PIN, LOW);
            delay(10000);
            digitalWrite(GIN_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Rum") == 0) {
            digitalWrite(RUM_PIN, LOW);
            delay(10000);
            digitalWrite(RUM_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Whiskey") == 0) {
            digitalWrite(WHISKEY_PIN, LOW);
            delay(10000);
            digitalWrite(WHISKEY_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Soda Water") == 0) {
            digitalWrite(SODA_PIN, LOW);
            delay(5500);
            digitalWrite(SODA_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Lime Juice") == 0) {
            digitalWrite(LIME_PIN, LOW);
            delay(10000);
            digitalWrite(LIME_PIN, HIGH);
          }
          
          // Print the selected drink
          Serial.print("Selected drink: ");
          Serial.println(selectedDrink);
        }
        
        else if (currentMenu == 2) {
          Serial.println(optionsCocktail[currentOption]);
        }

        else if (currentMenu == 3) {

          const char* selectedDrink = optionsJuice[currentOption];
      
          // Turn on the corresponding drink pin for 1 second
          if (strcmp(selectedDrink, "Lemonade") == 0) {
            digitalWrite(GIN_PIN, LOW);
            digitalWrite(WHISKEY_PIN, LOW);
            delay(5500);
            digitalWrite(WHISKEY_PIN, HIGH);
            digitalWrite(GIN_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Limeade") == 0) {
            digitalWrite(RUM_PIN, LOW);
            digitalWrite(LIME_PIN, LOW);
            delay(5500);
            digitalWrite(LIME_PIN, HIGH);
            digitalWrite(RUM_PIN, HIGH);
          } else if (strcmp(selectedDrink, "Pink Lemonade") == 0) {
            digitalWrite(SODA_PIN, LOW);
            delay(5500);
            digitalWrite(SODA_PIN, HIGH);
          } else if (strcmp(selectedDrink, "COMBO!!!") == 0) {
            digitalWrite(GIN_PIN, LOW);
            digitalWrite(WHISKEY_PIN, LOW);
            delay(1500);
            digitalWrite(WHISKEY_PIN, HIGH);
            digitalWrite(GIN_PIN, HIGH);

            digitalWrite(RUM_PIN, LOW);
            digitalWrite(LIME_PIN, LOW);
            delay(1500);
            digitalWrite(LIME_PIN, HIGH);
            digitalWrite(RUM_PIN, HIGH);

            
            digitalWrite(SODA_PIN, LOW);
            delay(1500);
            digitalWrite(SODA_PIN, HIGH);
          }
          
          
          // Print the selected drink
          Serial.print("Selected drink: ");
          Serial.println(selectedDrink);
        }
      }      
      drawMenu();
      Serial.println("OKAY BUTTON");
    }
  }
}

int getNumOptions(int menu) {
  return (int) menuSizes[menu];
}

void loop() {
  buttonPower.read();
  buttonUp.read();
  buttonDown.read();
  buttonOK.read();

  handleButtonPress();

  

}
