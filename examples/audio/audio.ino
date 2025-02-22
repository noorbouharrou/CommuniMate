#include "driver/dac.h" // Inclusief de DAC-driver van ESP-IDF
#include <SD.h>

#define DAC_CH1 DAC_CHANNEL_1 // DAC_CHANNEL_1 is GPIO25
#define SAMPLE_RATE 15000000 // Aantal samples per seconde

// Tabel voor sinusgolf (256 samples)
const uint8_t sineWave[256] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 168, 171, 174,
  177, 180, 183, 186, 189, 192, 195, 198, 201, 203, 206, 209, 212, 215, 217, 220,
  223, 225, 228, 231, 233, 236, 238, 241, 243, 246, 248, 250, 253, 255, 257, 259,
  261, 263, 265, 267, 269, 271, 273, 274, 276, 278, 279, 281, 282, 283, 285, 286,
  287, 288, 289, 290, 291, 292, 292, 293, 294, 294, 295, 295, 295, 296, 296, 296,
  296, 296, 295, 295, 295, 294, 294, 293, 292, 292, 291, 290, 289, 288, 287, 286,
  285, 283, 282, 281, 279, 278, 276, 274, 273, 271, 269, 267, 265, 263, 261, 259,
  257, 255, 253, 250, 248, 246, 243, 241, 238, 236, 233, 231, 228, 225, 223, 220,
  217, 215, 212, 209, 206, 203, 201, 198, 195, 192, 189, 186, 183, 180, 177, 174,
  171, 168, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 125,
  122, 119, 116, 113, 110, 107, 104, 101,  98,  94,  91,  88,  85,  82,  79,  76,
   73,  70,  67,  64,  61,  58,  55,  52,  50,  47,  44,  41,  38,  35,  33,  30,
   27,  25,  22,  19,  17,  14,  12,  10,   7,   5,   3,   1,   0,   0,   0,   0,
    0,   0,   1,   1,   2,   3,   4,   5,   6,   8,   9,  11,  13,  15,  17,  19,
   21,  24,  26,  29,  31,  34,  37,  40,  43,  46,  49,  52,  55,  59,  62,  65,
   69,  72,  76,  79,  83,  86,  90,  94,  97, 101, 105, 109, 113, 117, 121, 125
};

void setup() {
  // DAC output inschakelen
  dac_output_enable(DAC_CH1);
}

void loop() {
  // Genereer een sinusgolf door waarden van de tabel naar de DAC te schrijven
  for (int i = 0; i < 256; i++) {
    dac_output_voltage(DAC_CH1, sineWave[i]); // Schrijf sinuswaarde naar DAC
    delayMicroseconds(1000000 / SAMPLE_RATE); // Wacht voor de volgende sample
  }
}
