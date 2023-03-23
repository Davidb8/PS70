#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>

#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

// NFC Card Setup
SoftwareSerial SWSerial( 3, 2 ); // RX, TX
 
PN532_SWHSU pn532swhsu( SWSerial );
PN532 nfc( pn532swhsu );
String tagId = "None", dispTag = "None";
byte nuidPICC[4];
String drink;
String ScreenImage;
long previousMillis = 0; 
long interval = 500;

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




void setup(void)
{
  Serial.begin(9600);
  delay(100);
  Serial.println("Hello Maker, I am BarBot!");
  //  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata)
  {
    Serial.println("Didn't Find PN53x Module");
    while (1); // Halt
  }

  // Got valid data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  // Configure board to read RFID tags
  nfc.SAMConfig();
  drink = "";
  //Serial.println("Waiting for an ISO14443A Card ...");

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
  tft.setRotation(1);    // Set rotation
  Serial.print(F("Loading nfc.bmp to screen..."));
  stat = reader.drawBMP("/nfc.bmp", tft, 0, 0);
  reader.printStatus(stat);   // How'd we do?


  delay(10000);

  delay(2000); // Pause 2 seconds before moving on to loop()
  
}
 
 
void loop()
{
  readNFC();
  updateScreen();
}
 
String readTag(String tagId)
{
  // Takes in a tag string and Identifies what it should be recognized as for drinks
  if (tagId.equals("50.40.36.33"))
  {
    return "Mojito";
  }
  if (tagId.equals("34.126.220.33"))
  {
    return "Gin and Tonic";
  }
  if (tagId.equals("17.136.236.38"))
  {
    return "Wiskey Sour";
  }
  return "Invalid Tag";
} 

String tagToString(byte id[4])
{
  String tagId = "";
  for (byte i = 0; i < 4; i++)
  {
    if (i < 3) tagId += String(id[i]) + ".";
    else tagId += String(id[i]);
  }
  return tagId;
}

void readNFC()
{ 

  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval)
  { 
    

    boolean success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    if (success)
    {
      // save the last time you read NFC 
      previousMillis = currentMillis;
      //Serial.print("UID Length: ");
      //Serial.print(uidLength, DEC);
      //Serial.println(" bytes");
      //Serial.print("UID Value: ");
      for (uint8_t i = 0; i < uidLength; i++)
      {
        nuidPICC[i] = uid[i];
        //Serial.print(" "); Serial.print(uid[i], DEC);
      }
      //Serial.println();
      tagId = tagToString(nuidPICC);
      dispTag = tagId;
      //Serial.print(F("tagId is : "));
      //Serial.println(tagId);
      //Serial.println("");
      
      //print out the drink
      String tempDrink = readTag(tagId);
      if (!drink.equals(tempDrink))
      {
        if(!tempDrink.equals("Invalid Tag"))
        {
          Serial.print(F("Cocktail Selected: "));
        }
        Serial.print(tempDrink);
        Serial.println(F(""));
        drink = tempDrink;
        
      }  
      
    }
    else
    {
      // PN532 probably timed out waiting for a card
      //Serial.println("Timed out! Waiting for a card...");
    }
  }
}

void updateScreen()
{
  // make sure screen not already displaying correct info

  if (!drink.equals(ScreenImage))
  {
    if(!drink.equals("Invalid Tag"))
    {
      // update Screen
      ScreenImage = drink;

      if (ScreenImage.equals(""))
      {
        reader.drawBMP("/nfc.bmp", tft, 0, 0);
      }
      if (ScreenImage.equals("Mojito"))
      {
        reader.drawBMP("/mojito.bmp", tft, 0, 0);
      }
      if (ScreenImage.equals("Gin and Tonic"))
      {
        reader.drawBMP("/g&t.bmp", tft, 0, 0);
      }
      if (ScreenImage.equals("Whiskey Sour"))
      {
        reader.drawBMP("/whiskey sour.bmp", tft, 0, 0);
      }
    }
    else
    {
      reader.drawBMP("/nfc.bmp", tft, 0, 0);
    }    
  }
}