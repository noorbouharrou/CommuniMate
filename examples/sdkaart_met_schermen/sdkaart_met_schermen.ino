#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

PNG png;

// HC4067 control pins connected to ESP32
const int S0 = 32;                                // Channel selection pin S0 (can be any available GPIO on ESP32)
const int S1 = 33;                                // Channel selection pin S1
const int S2 = 22;                                // Channel selection pin S2
const int S3 = 26;                                // Channel selection pin S3
const int muxControlPins[] = { S0, S1, S2, S3 };  // Array storing the control pins (S0-S3)
const int pullUpLine = 13;                        // Pin for controlling pull-up resistor (used to select the displays)
#define MAX_IMAGE_WIDTH 240   // Adjust based on the image dimensions
int16_t xpos = 0, ypos = 0;   // Position of the image on the screen

void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

int32_t fileReader(PNGFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  File *f = reinterpret_cast<File *>(pFile->fHandle);
  return f->read(pBuf, iLen);
}

int32_t fileSeek(PNGFILE *pFile, int32_t iPosition) {
  File *f = reinterpret_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  return iPosition;
}

void *fileOpen(const char *filename, int32_t *size) {
  File *f = new File(SD.open(filename));
  if (f && *f) {
    *size = f->size();
    return reinterpret_cast<void *>(f);
  }
  delete f;
  return nullptr;
}

void fileClose(void *handle) {
  File *f = reinterpret_cast<File *>(handle);
  if (f) f->close();
  delete f;
}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_LITTLE_ENDIAN, 0xFFFFFFFF);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

// Function to display PNG image info
void displayPNGInfo(const char *filename) {
  if (png.open(filename, fileOpen, fileClose, fileReader, fileSeek, pngDraw) == PNG_SUCCESS) {
    Serial.println("PNG file opened successfully.");

    // Print image details
    Serial.printf("Image Width: %d\n", png.getWidth());
    Serial.printf("Image Height: %d\n", png.getHeight());
    Serial.printf("Color Type: %d\n", png.getBpp());  // Bits per pixel

    png.close();
  } else {
    Serial.println("Failed to open PNG file.");
  }
}

//####################################################################################################
// Setup
//####################################################################################################
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);    // Set each control pin (S0-S3) as an output
    digitalWrite(muxControlPins[i], LOW);  // Set all control pins to LOW to select channel 0 initially
  }

  digitalWrite(12, HIGH);  // SD card chip select, must use GPIO 5 (ESP32 SS)

  selectMuxChannel(15);

  pinMode(pullUpLine, OUTPUT);  // Set pull-up line as OUTPUT
  digitalWrite(pullUpLine, LOW);

  tft.begin();

  tft.setRotation(0);  // Set the rotation of the display (0 degrees)
  tft.fillScreen(TFT_BLUE);

  digitalWrite(pullUpLine, HIGH);

  if (!SD.begin(12, tft.getSPIinstance())) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println("Initialization done.");  // Set the pull-up line to HIGH, ensuring all displays are deselected

  int x = (tft.width() - 240) / 2 - 1;
  int y = (tft.height() - 240) / 2 - 1;

  drawSdPNG("/jonathan.png",0);
  drawSdPNG("/dikkeN.png",1);
  drawSdPNG("/nibba.png",2);

}

void drawSdPNG(const char *filename, int channel) {

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);

  // Open the named file (the Jpeg decoder library will close it)
  File pngFile = SD.open(filename, FILE_READ);  // or, file handle reference for SD library

  if (!pngFile) {
    Serial.print("ERROR: File \"");
    Serial.print(filename);
    Serial.println("\" not found!");
    return;
  }

  Serial.println("===========================");
  Serial.print("Drawing file: ");
  Serial.println(filename);
  Serial.println("===========================");

  displayPNGInfo(filename);

  int16_t openResult = png.open(filename, fileOpen, fileClose, fileReader, fileSeek, pngDraw);

  if (openResult == PNG_SUCCESS) {
    Serial.println("Decoding success.");

    selectMuxChannel(channel);
    tft.fillScreen(TFT_BLACK);          // Clear the display
    png.decode(NULL, 0);                // Decode and render the PNG
    png.close();                        // Close the PNG file
  } else {
    Serial.println("Failed to decode PNG image.");
  }

  tft.setSwapBytes(swapBytes);
}


//####################################################################################################
// Main loop
//####################################################################################################
void loop() {
}