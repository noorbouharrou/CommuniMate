#include <TFT_eSPI.h>  // Include the graphics and font library for ST7735 driver chip
#include <SPI.h>        // Include the SPI library to communicate with the display
#include <PNGdec.h>    // PNG decoding library
#include "driver/dac.h"

// Function to get the next sample from the WAV file data

//images
#include "image1.h"
#include "image2.h"
#include "image3.h"
#include "image4.h"
#include "image5.h"
#include "image6.h"
#include "image7.h"
#include "image8.h"

//audio
#include "1.h"
#include "2.h"
#include "3.h"
#include "4.h"
#include "5.h"
#include "6.h"
#include "7.h"
#include "8.h"

#define DAC_CH1 DAC_CHANNEL_1 // DAC_CHANNEL_1 is GPIO25
#define SAMPLE_RATE 8000 // Sample rate of the WAV file (16kHz)
#define MAX_IMAGE_WIDTH 240 // Adjust based on the image dimensions

TFT_eSPI tft = TFT_eSPI();  // Create an instance of the TFT_eSPI class to handle the TFT display
                            // Pins for the display are defined in User_Setup.h (not shown here)
PNG png;

const int rows[] = {27, 14};          // Row pins
const int cols[] = {15, 16, 17, 21};  // Column pins
const int numRows = sizeof(rows) / sizeof(rows[0]);
const int numCols = sizeof(cols) / sizeof(cols[0]);
bool keyStates[numRows][numCols] = {false};

const char keymap[numRows][numCols] = {
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

// HC4067 control pins connected to ESP32
const int S0 = 32;  // Channel selection pin S0 (can be any available GPIO on ESP32)
const int S1 = 33;  // Channel selection pin S1
const int S2 = 22;  // Channel selection pin S2
const int S3 = 26;  // Channel selection pin S3
const int muxControlPins[] = {S0, S1, S2, S3};  // Array storing the control pins (S0-S3)

// The multiplexer signal line (connected to TFT displays' CS pins)
const int pullUpLine = 13;     // Pin for controlling pull-up resistor (used to select the displays)
int16_t xpos = 0, ypos = 0;  // Position of the image on the screen

// Function to select the correct multiplexer channel based on the input channel number
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    // Set S0-S3 pins based on the channel number using bitwise operations
    // (channel >> i) shifts the bits, and & 1 isolates the bit at position i
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

const unsigned char* audioFiles[] = {rawData1, rawData2, rawData3, rawData4, rawData5, rawData6, rawData7};
const size_t audioSizes[] = {sizeof(rawData1), sizeof(rawData2), sizeof(rawData3), sizeof(rawData4), sizeof(rawData5), sizeof(rawData6), sizeof(rawData7)};

void playAudio(const unsigned char* audioData, size_t dataSize) {
    for (size_t i = 44; i < dataSize; i++) { // Skip first 44 bytes (WAV header)
        dac_output_voltage(DAC_CH1, audioData[i]);
        delayMicroseconds(1000000 / SAMPLE_RATE);
    }
}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void setup(void) {
  Serial.begin(115200);

  dac_output_enable(DAC_CH1);

  // Set multiplexer control pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);  // Set each control pin (S0-S3) as an output
    digitalWrite(muxControlPins[i], LOW);  // Set all control pins to LOW to select channel 0 initially
  }

  for (int i = 0; i < numRows; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH); // Default HIGH
  }

    // Set up column pins as INPUT with pull-ups (if required)
  for (int j = 0; j < numCols; j++) {
    pinMode(cols[j], INPUT_PULLUP); // Columns as inputs
  }

  pinMode(pullUpLine, OUTPUT);           // Set pull-up line as OUTPUT
  digitalWrite(pullUpLine, LOW);         // Initially set pull-up line to LOW, selecting all displays

  // Configure the TFT display
  tft.init();                           // Initialize the TFT display
  tft.setRotation(3);                   // Set the rotation of the display (0 degrees)
  tft.setTextSize(3);                   // Set the text size on the display
  tft.fillScreen(TFT_BLACK);

  digitalWrite(pullUpLine, HIGH);       // Set the pull-up line to HIGH, ensuring all displays are deselected

  // Display text on both displays by selecting them via the multiplexer
  for (int i = 15; i >= 8; i--) {  // Loop through the two displays
    selectMuxChannel(i);         // Select the correct multiplexer channel (0 or 1)
    tft.fillScreen(TFT_RED);   // Clear the display
    Serial.println(i);
    switch (i) {
      case 15: {
        int16_t openJohn = png.openFLASH((uint8_t *)image1, sizeof(image1), pngDraw);
        if (openJohn == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 14: {
        int16_t openDecline = png.openFLASH((uint8_t *)image2, sizeof(image2), pngDraw);
        if (openDecline == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 13: {
        int16_t openAccept = png.openFLASH((uint8_t *)image3, sizeof(image3), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 12: {
        int16_t openAccept = png.openFLASH((uint8_t *)image4, sizeof(image4), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 11: {
        int16_t openAccept = png.openFLASH((uint8_t *)image5, sizeof(image5), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 10: {
        int16_t openAccept = png.openFLASH((uint8_t *)image6, sizeof(image6), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 9: {
        int16_t openAccept = png.openFLASH((uint8_t *)image7, sizeof(image7), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
      case 8: {
        int16_t openAccept = png.openFLASH((uint8_t *)image8, sizeof(image8), pngDraw);
        if (openAccept == PNG_SUCCESS) {
          Serial.printf("Displaying on channel %d\n", i);
          tft.startWrite();
          png.decode(NULL, 0);  // Decode and render the PNG
          tft.endWrite();
        } else {
          Serial.println("Failed to decode PNG");
        }
        break;
      }
    }
    delay(100);
  } 
}

void loop() {
    for (int i = 0; i < numRows; i++) {
        digitalWrite(rows[i], LOW);
        for (int j = 0; j < numCols; j++) {
            if (digitalRead(cols[j]) == LOW) {
                if (!keyStates[i][j]) {
                    keyStates[i][j] = true;
                    Serial.print("Key pressed: ");
                    Serial.println(keymap[i][j]);
                    int keyIndex = (i * numCols) + j;
                    playAudio(audioFiles[keyIndex], audioSizes[keyIndex]);
                }
            } else {
                keyStates[i][j] = false;
            }
        }
        digitalWrite(rows[i], HIGH);
    }
    delay(10);
}