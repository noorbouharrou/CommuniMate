#include <TFT_eSPI.h>  // Include the graphics and font library for ST7735 driver chip
#include <SPI.h>        // Include the SPI library to communicate with the display

TFT_eSPI tft = TFT_eSPI();  // Create an instance of the TFT_eSPI class to handle the TFT display
                            // Pins for the display are defined in User_Setup.h (not shown here)

// HC4067 control pins connected to ESP32
const int S0 = 32;  // Channel selection pin S0 (can be any available GPIO on ESP32)
const int S1 = 33;  // Channel selection pin S1
const int S2 = 25;  // Channel selection pin S2
const int S3 = 26;  // Channel selection pin S3
const int muxControlPins[] = {S0, S1, S2, S3};  // Array storing the control pins (S0-S3)

// The multiplexer signal line (connected to TFT displays' CS pins)
const int muxSignalLine = 21;  // The signal line (SIG) from the multiplexer is connected to GPIO 21
const int pullUpLine = 13;     // Pin for controlling pull-up resistor (used to select the displays)

// Function to select the correct multiplexer channel based on the input channel number
void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    // Set S0-S3 pins based on the channel number using bitwise operations
    // (channel >> i) shifts the bits, and & 1 isolates the bit at position i
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
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

  digitalWrite(pullUpLine, HIGH);       // Set the pull-up line to HIGH, ensuring all displays are selected

  // Display text on both displays by selecting them via the multiplexer
  for (int i = 0; i < 2; i++) {  // Loop through the two displays
    selectMuxChannel(i);         // Select the correct multiplexer channel (0 or 1)
    tft.fillScreen(TFT_BLACK);   // Clear the display with a black background
    tft.setTextColor(TFT_RED);   // Set the text color to red
    tft.setCursor(0, 0);         // Reset the cursor to the top-left corner (0, 0) of the screen
    tft.print("Display ");       // Print "Display" text
    tft.println(i);              // Print the display number (0 or 1)
  } 
}

void loop() {
  // The loop is empty, as this code only runs once in the setup function
}
