#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


void setup(void) {
  tft.init();
  tft.setRotation(0);
  tft.setTextSize(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED);
  tft.println(" Red text");
  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");
  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");
}

void loop(){

}