#include <TFT_eSPI.h>  // Include the graphics and font library for ST7735 driver chip
#include <SPI.h>        // Include the SPI library to communicate with the display
#include <PNGdec.h>    // PNG decoding library
#include "john.h"     // PNG image stored as a C array
#include "decline.h"
#include "accept.h"
#include "shhhhh.h"
#include "shhhhh2.h"
#include "shhhhh15.h"

TFT_eSPI tft = TFT_eSPI();  // Create an instance of the TFT_eSPI class to handle the TFT display
                            // Pins for the display are defined in User_Setup.h (not shown here)
PNG png;

// HC4067 control pins connected to ESP32
const int S0 = 32;  // Channel selection pin S0 (can be any available GPIO on ESP32)
const int S1 = 33;  // Channel selection pin S1
const int S2 = 25;  // Channel selection pin S2
const int S3 = 26;  // Channel selection pin S3
const int muxControlPins[] = {S0, S1, S2, S3};  // Array storing the control pins (S0-S3)

// The multiplexer signal line (connected to TFT displays' CS pins)
const int muxSignalLine = 5;  // The signal line (SIG) from the multiplexer is connected to GPIO 21
const int pullUpLine = 13;     // Pin for controlling pull-up resistor (used to select the displays)
#define MAX_IMAGE_WIDTH 240 // Adjust based on the image dimensions
int16_t xpos = 0, ypos = 0;  // Position of the image on the screen

// Function to select the correct multiplexer channel based on the input channel number
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    // Set S0-S3 pins based on the channel number using bitwise operations
    // (channel >> i) shifts the bits, and & 1 isolates the bit at position i
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

void setup(void) {
  // Set multiplexer control pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);  // Set each control pin (S0-S3) as an output
    digitalWrite(muxControlPins[i], LOW);  // Set all control pins to LOW to select channel 0 initially
  }

  pinMode(pullUpLine, OUTPUT);           // Set pull-up line as OUTPUT
  digitalWrite(pullUpLine, LOW);         // Initially set pull-up line to LOW, selecting all displays

  // Configure the TFT display
  tft.init();                           // Initialize the TFT display
  tft.setRotation(0);                   // Set the rotation of the display (0 degrees)
  tft.setTextSize(3);                   // Set the text size on the display

  digitalWrite(pullUpLine, HIGH);       // Set the pull-up line to HIGH, ensuring all displays are deselected

  // Display text on both displays by selecting them via the multiplexer
  for (int i = 0; i <= 2; i++) {  // Loop through the two displays
    selectMuxChannel(i);         // Select the correct multiplexer channel (0 or 1)
    tft.fillScreen(TFT_BLACK);   // Clear the display

    switch (i) {
      case 0: {
        int16_t openJohn = png.openFLASH((uint8_t *)shhhhh, sizeof(shhhhh), pngDraw);
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
      case 1: {
        int16_t openDecline = png.openFLASH((uint8_t *)shhhhh15, sizeof(shhhhh15), pngDraw);
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
      case 2: {
        int16_t openAccept = png.openFLASH((uint8_t *)shhhhh2, sizeof(shhhhh2), pngDraw);
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
  } 
}

void loop() {
  // The loop is empty, as this code only runs once in the setup function
}