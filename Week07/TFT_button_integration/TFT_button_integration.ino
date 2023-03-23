
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions


// Display Setup
#define USE_SD_CARD

// TFT display and SD card share the hardware SPI interface, using
// 'select' pins for each to identify the active device on the bus.

#define SD_CS   4 // SD card select pin
#define TFT_CS 10 // TFT select pin
#define TFT_DC  9 // TFT display/command pin

#if defined(USE_SD_CARD)
  SdFat                SD;         // SD card filesystem
  Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
#else
  // SPI or QSPI flash filesystem (i.e. CIRCUITPY drive)
  #if defined(__SAMD51__) || defined(NRF52840_XXAA)
    Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
      PIN_QSPI_IO0, PIN_QSPI_IO1, PIN_QSPI_IO2, PIN_QSPI_IO3);
  #else
    #if (SPI_INTERFACES_COUNT == 1)
      Adafruit_FlashTransport_SPI flashTransport(SS, &SPI);
    #else
      Adafruit_FlashTransport_SPI flashTransport(SS1, &SPI1);
    #endif
  #endif
  Adafruit_SPIFlash    flash(&flashTransport);
  FatVolume        filesys;
  Adafruit_ImageReader reader(filesys); // Image-reader, pass in flash filesys
#endif

//Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC,11,13,8,12);
Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC,5,6,8,7);

Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;




                     
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 3;    // the number of the LED pin

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int ScreenImage = 0;
int screen = 0;
long previousMillis = 0; 
long interval = 500;


void setup() {
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);

  ImageReturnCode stat; // Status from image-reading functions

  #if !defined(ESP32)
    while(!Serial);       // Wait for Serial Monitor before continuing
  #endif

  tft.begin();          // Initialize screen

  // The Adafruit_ImageReader constructor call (above, before setup())
  // accepts an uninitialized SdFat or FatVolume object. This MUST
  // BE INITIALIZED before using any of the image reader functions!
  Serial.print(F("Initializing filesystem..."));
  #if defined(USE_SD_CARD)
  // SD card is pretty straightforward, a single call...
  if(!SD.begin(SD_CS)) { // ESP32 requires 25 MHz limit
    Serial.println(F("SD begin() failed"));
    for(;;); // Fatal error, do not continue
  }
  #else
  // SPI or QSPI flash requires two steps, one to access the bare flash
  // memory itself, then the second to access the filesystem within...
  if(!flash.begin()) {
    Serial.println(F("flash begin() failed"));
    for(;;);
  }
  if(!filesys.begin(&flash)) {
    Serial.println(F("filesys begin() failed"));
    for(;;);
  }
  #endif
  Serial.println(F("OK!"));

  // Fill screen blue. Not a required step, this just shows that we're
  // successfully communicating with the screen.
  tft.fillScreen(ILI9341_BLUE);

  // Load full-screen BMP file 'purple.bmp' at position (0,0) (top left).
  // Notice the 'reader' object performs this, with 'tft' as an argument.
  tft.setRotation(3);    // Set rotation
  Serial.print(F("Loading nfc.bmp to screen..."));
  stat = reader.drawBMP("/nfc.bmp", tft, 0, 0);
  reader.printStatus(stat);   // How'd we do?
  delay(1000);


}
void readButton()
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval)
  {
    buttonState = digitalRead(buttonPin);

    if (buttonState == HIGH) {

    //  do nothing
    } 
    else {
      // turn LED off:
      previousMillis = currentMillis;

      screen += 1;
      if (screen > 3)
      {
        screen = 1;
      }
      Serial.println(screen);

    }

  }

}

void loop() {
  readButton();
  updateScreen();
}


void updateScreen()
{
  // make sure screen not already displaying correct info
  
  if (!(screen == ScreenImage))
  {
  
    // update Screen
    ScreenImage = screen;

    if (screen == 0)
    {
      reader.drawBMP("/nfc.bmp", tft, 0, 0);
    }
    if (screen == 1)
    {
      reader.drawBMP("/mojito.bmp", tft, 0, 0);
    }
    if (screen == 2)
    {
      reader.drawBMP("/g&t.bmp", tft, 0, 0);
    }
    if (screen == 3)
    {
      reader.drawBMP("/whiskey sour.bmp", tft, 0, 0);
    }
  }  
  
}
