#include <TFT_eSPI.h>  // TFT display library for ST7735/ST7789
#include <SPI.h>       // SPI communication library
#include <PNGdec.h>    // PNG decoding library
#include "panda.h"     // PNG image stored as a C array

// TFT and PNG decoder instances
TFT_eSPI tft = TFT_eSPI();
PNG png;

// HC4067 control pins (ESP32 GPIOs)
const int S0 = 32;
const int S1 = 33;
const int S2 = 25;
const int S3 = 26;
const int muxControlPins[] = {S0, S1, S2, S3};
const int muxSignalLine = 21; // Signal line connected to TFT CS pins
const int pullUpLine = 13;    // Pull-up line to control display selection

// Constants
#define MAX_IMAGE_WIDTH 240 // Adjust based on the image dimensions
int16_t xpos = 0, ypos = 0;  // Position of the image on the screen

// Function to set the multiplexer channel
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

// PNG rendering function
void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

// Function to display the PNG image on a selected display
void displayImageOnChannel(int channel) {
  selectMuxChannel(channel);   // Select the display channel
  tft.fillScreen(TFT_BLACK);   // Clear the display
  int16_t rc = png.openFLASH((uint8_t *)panda, sizeof(panda), pngDraw);
  if (rc == PNG_SUCCESS) {
    Serial.printf("Displaying on channel %d\n", channel);
    tft.startWrite();
    png.decode(NULL, 0);  // Decode and render the PNG
    tft.endWrite();
  } else {
    Serial.println("Failed to decode PNG");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting PNG Display via Multiplexer");

  // Initialize multiplexer control pins
  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);
    digitalWrite(muxControlPins[i], LOW);
  }

  pinMode(pullUpLine, OUTPUT);
  digitalWrite(pullUpLine, HIGH); // Ensure all displays are deselected initially

  // Initialize TFT display
  tft.init();
  tft.setRotation(0);

  // Display the image on multiple channels (modify based on the number of screens)
  for (int channel = 0; channel < 2; channel++) {
    displayImageOnChannel(channel);
    delay(3000); // Wait before switching to the next display
  }
}

void loop() {
  // Empty loop as all operations are done in setup
}
