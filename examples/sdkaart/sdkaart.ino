#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <PNGdec.h>

PNG png;

// HC4067 control pins connected to ESP32
const int S0 = 32;                                // Channel selection pin S0 (can be any available GPIO on ESP32)
const int S1 = 33;                                // Channel selection pin S1
const int S2 = 25;                                // Channel selection pin S2
const int S3 = 26;                                // Channel selection pin S3
const int muxControlPins[] = { S0, S1, S2, S3 };  // Array storing the control pins (S0-S3)

void selectMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxControlPins[i], (channel >> i) & 1);
  }
}

int32_t fileReader(PNGFILE *pFile, uint8_t *pBuf, int32_t iLen) {
  File* f = reinterpret_cast<File*>(pFile->fHandle);
  return f->read(pBuf, iLen);
}

int32_t fileSeek(PNGFILE *pFile, int32_t iPosition) {
  File* f = reinterpret_cast<File*>(pFile->fHandle);
  f->seek(iPosition);
  return iPosition;
}

void *fileOpen(const char *filename, int32_t *size) {
  File *f = new File(SD.open(filename));
  if (f && *f) {
    *size = f->size();
    return reinterpret_cast<void*>(f);
  }
  delete f;
  return nullptr;
}

void fileClose(void *handle) {
  File* f = reinterpret_cast<File*>(handle);
  if (f) f->close();
  delete f;
}

// Dummy draw callback (required by PNG library but not used here)
void drawPNG(PNGDRAW *pDraw) {
  // No drawing is done in this example
}

// Function to display PNG image info
void displayPNGInfo(const char* filename) {
  if (png.open(filename, fileOpen, fileClose, fileReader, fileSeek, drawPNG) == PNG_SUCCESS) {
    Serial.println("PNG file opened successfully.");

    // Print image details
    Serial.printf("Image Width: %d\n", png.getWidth());
    Serial.printf("Image Height: %d\n", png.getHeight());
    Serial.printf("Color Type: %d\n", png.getBpp()); // Bits per pixel

    png.close();
  } else {
    Serial.println("Failed to open PNG file.");
  }
}

//####################################################################################################
// Setup
//####################################################################################################
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(muxControlPins[i], OUTPUT);    // Set each control pin (S0-S3) as an output
    digitalWrite(muxControlPins[i], LOW);  // Set all control pins to LOW to select channel 0 initially
  }

  digitalWrite(5, HIGH);  // SD card chip select, must use GPIO 5 (ESP32 SS)

  selectMuxChannel(15);

  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println("Initialization done.");

  const char* pngFilename = "/jonathan.png";
  File pngFile = SD.open(pngFilename);
  if (!pngFile) {
    Serial.println("Failed to open image file");
    return;
  }
  pngFile.close();

  // Display PNG information
  displayPNGInfo(pngFilename);
}

//####################################################################################################
// Main loop
//####################################################################################################
void loop() {
}
