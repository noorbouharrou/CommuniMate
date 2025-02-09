/*
===========================================================
Project Name: Communimate
Authors:  
- Niels de Gier  
- Kieran Julien
- Mika Heezen  
- Jeroen de Kreij  
- Noor Bouharrou  
Date: 8/2/2025
Description:
This project uses an ESP32 to display images on TFT-screens with help form a multiplexer.
It also reads button presses and plays an corresponding audio file.  
===========================================================
*/

//import library's
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <PNGdec.h>
#include <TFT_eSPI.h>
#include "driver/dac.h"

//define audio 
#define DAC_CH1 DAC_CHANNEL_1
#define SAMPLE_RATE 8000
#define sdCardOutPin 12
TFT_eSPI tft = TFT_eSPI();

File wavFile;
PNG png;

//multiplexer control pins connected to ESP32
const int S0 = 32;
const int S1 = 33;
const int S2 = 22;
const int S3 = 26;
const int muxControlPins[] = { S0, S1, S2, S3 };
const int pullUpLine = 13;
#define MAX_IMAGE_WIDTH 240
int16_t xpos = 0, ypos = 0;
bool playingAudio = false; 


// Keypad matrix
const int rows[] = {27, 14};
const int cols[] = {15, 16, 17, 21};
const int numRows = sizeof(rows) / sizeof(rows[0]);
const int numCols = sizeof(cols) / sizeof(cols[0]);

bool keyStates[numRows][numCols] = {false};

const char keymap[numRows][numCols] = {
  {'1', '2', '3', '4'}, // Row 0 keys
  {'5', '6', '7', '8'}  // Row 1 keys
};

//Selector function for multiplexer
//takes int channel: multiplexer channel that should be active
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

//custom fileread function for PNG-decoder
int32_t fileReader(PNGFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  File *f = reinterpret_cast<File *>(pFile->fHandle);
  return f->read(pBuf, iLen);
}

//custom fileseek function to search for a specific location in a file
int32_t fileSeek(PNGFILE *pFile, int32_t iPosition) {
  File *f = reinterpret_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  return iPosition;
}

//custom open file fuctions to open a file from the SD card
void *fileOpen(const char *filename, int32_t *size) {
  File *f = new File(SD.open(filename));
  if (f && *f) {
    *size = f->size();
    return reinterpret_cast<void *>(f);
  }
  delete f;
  return nullptr;
}

//custom file close function to close a file
void fileClose(void *handle) {
  File *f = reinterpret_cast<File *>(handle);
  if (f) f->close();
  delete f;
}

//function to render an image to a screen
void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_LITTLE_ENDIAN, 0xFFFFFFFF);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
  
}

//function to load and draw an png image form an sd-card
//takes char *filename: the file name of the png image
//int channel: the multiplexer chanel that needs to be active
void drawSdPNG(const char *filename, int channel) {

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);

  File pngFile = SD.open(filename, FILE_READ);

  if (!pngFile) {
    Serial.print("ERROR: File \"");
    Serial.print(filename);
    Serial.println("\" not found!");
    return;
    }
}

void setup() {
  Serial.begin(115200);
  dac_output_enable(DAC_CH1);
  pinMode(sdCardOutPin, OUTPUT);
  digitalWrite(sdCardOutPin, HIGH);
  pinMode(pullUpLine, OUTPUT);
  //set pullup line low, so all screens are selected
  digitalWrite(pullUpLine, LOW);

  //setup multiplexer control pins
  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);
    digitalWrite(muxControlPins[i], LOW);
  }

  tft.begin();
  tft.setRotation(90);
  tft.fillScreen(TFT_BLUE);

  //set pullup line back to high so that non of the screens are selected
  digitalWrite(pullUpLine, HIGH);

  //start and test SD card module
  if (!SD.begin(12, tft.getSPIinstance())) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  int x = (tft.width() - 240) / 2 - 1;
  int y = (tft.height() - 240) / 2 - 1;

  //draw all 8 starting images on the respective screen
  for (int i = 0; i < 8; i++){
    std::string filename = "/image" + std::to_string(i) + ".png";
    drawSdPNG(filename.c_str(), 15-i);
  }

  //setup keypad
  for (int i = 0; i < numRows; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH); // Default HIGH
  }
 
  for (int j = 0; j < numCols; j++) {
    pinMode(cols[j], INPUT_PULLUP); // Columns as inputs
  }
  
  for (int i = 0; i < numRows; i++) {
    digitalWrite(rows[i], LOW);
  }
}

void loop() {
  //check if audio should be playing
  if(playingAudio){
    //if auio is at its end return to not be playing audio
    if (!wavFile || !wavFile.available()) {
      playingAudio = false; 
    }
    //play audio
    unsigned char sample = wavFile.read();
    dac_output_voltage(DAC_CH1, sample);
    delayMicroseconds(1000000 / SAMPLE_RATE);
    return;
  }
  //check if button is pressed and play corresponding audio
   for (int i = 0; i < numRows; i++) {
    digitalWrite(rows[i], LOW);
      for (int j = 0; j < numCols; j++) {
      if (digitalRead(cols[j]) == HIGH) {
        int key = keymap[i][j]; 
        String audioName = "/" + String(key) + ".wav";
        wavFile = SD.open(audioName);
        wavFile.seek(44);
        playingAudio = true; 
        if (!wavFile) {
          Serial.println("Failed to open WAV file");
          return;
        }
      }
    }
  }
}
