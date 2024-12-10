#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// HC4067 control pins connected to ESP32
const int S0 = 32;  // Channel selection pins (can be any available GPIOs)
const int S1 = 33;
const int S2 = 25;
const int S3 = 26;
const int muxControlPins[] = {S0, S1, S2, S3};

// The multiplexer signal line (connected to TFT displays' CS pins)
const int muxSignalLine = 21;  // Assuming Signal (SIG) is connected to GPIO 21
const int pullUpLine = 13;

// Function to select the correct multiplexer channel
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxControlPins[i], (channel >> i) & 1);  // Set S0-S3 to select channel
  }
}

void setup(void) {
  // Set multiplexer control pins as OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);
    digitalWrite(muxControlPins[i], LOW);  // Default to channel 0
  }
  pinMode(pullUpLine, OUTPUT);
  digitalWrite(pullUpLine, LOW); // All displays are selected
  //Configure displays
  tft.init();
  tft.setRotation(0);
  tft.setTextSize(3);
  digitalWrite(pullUpLine, HIGH); // All displays are selected

  for (int i = 0; i < 2; i++){
    selectMuxChannel(i);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED);
    tft.print("Display ");
    tft.println(i);
  } 
}

void loop(){

}