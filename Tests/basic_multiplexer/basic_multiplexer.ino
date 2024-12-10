// HC4067 control pins connected to ESP32
const int S0 = 32;  // Channel selection pins (can be any available GPIOs)
const int S1 = 33;
const int S2 = 25;
const int S3 = 26;
const int muxControlPins[] = {S0, S1, S2, S3};

// The multiplexer signal line (connected to TFT displays' CS pins)
const int muxSignalLine = 21;  // Assuming Signal (SIG) is connected to GPIO 21

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

  // Set the multiplexer signal line as OUTPUT
  pinMode(muxSignalLine, OUTPUT);
  digitalWrite(muxSignalLine, LOW);  // Initially not selecting any display

  selectMuxChannel(0);
}

void loop(){
  selectMuxChannel(0);
  digitalWrite(muxSignalLine, HIGH);
  delay(1000);
  digitalWrite(muxSignalLine, LOW);
  delay(1000);
  selectMuxChannel(1);
  digitalWrite(muxSignalLine, HIGH);
  delay(1000);
  digitalWrite(muxSignalLine, LOW);
  delay(1000);
}