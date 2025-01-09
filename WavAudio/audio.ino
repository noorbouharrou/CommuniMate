#include "driver/dac.h"
#include "music.h"  // Include the header file containing your WAV file raw data

#define DAC_CH1 DAC_CHANNEL_1 // DAC_CHANNEL_1 is GPIO25
#define SAMPLE_RATE 16000 // Sample rate of the WAV file (16kHz)

// Function to get the next sample from the WAV file data
unsigned char getSample(int index) {
    return rawData[index];
}

void setup() {
  // DAC output inschakelen
  dac_output_enable(DAC_CH1);
}

void loop() {
  static int sampleIndex = 0; // Keep track of the current sample in the WAV data

  // Get the next sample from the raw WAV data
  unsigned char sample = getSample(sampleIndex);

  // Output the sample value to the DAC
  dac_output_voltage(DAC_CH1, sample );  // Directly output the 8-bit sample

  // Increment sample index and wrap around when we reach the end of the data
  sampleIndex++;  // Each sample is 1 byte (8-bit)
  if (sampleIndex >= sizeof(rawData)) {
    sampleIndex = 0;  // Loop back to the beginning of the WAV data
  }

  // Wait for the next sample based on the sample rate (16kHz)
  delayMicroseconds(1000000 / SAMPLE_RATE); // Adjust the delay for 16kHz sample rate
}
