#include "application.h"
#line 1 "/Users/kevinmcquown/Dropbox/WCL/wcltalkstech/spiInDepth/src/spiInDepth.ino"
void setup();
void SPITransferComplete();
void loop();
#line 1 "/Users/kevinmcquown/Dropbox/WCL/wcltalkstech/spiInDepth/src/spiInDepth.ino"
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#define LED A1
#define SS A5 // default for Xenon, we can use any GPIO!

byte tx_buffer[8] = {0x0B, 0x0E, 0x0E, 0x0F, 0x0C, 0x0A, 0x0F, 0x0E};
byte rx_buffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

volatile bool transferComplete = false;
volatile bool ledIsHigh = false;

void setup() {
  SPI.begin(SS);
  SPI.setClockSpeed(100, MHZ);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledIsHigh);
}

void SPITransferComplete() {
  transferComplete = true;
}

void loop() {

  transferComplete = false;

  digitalWrite(SS, LOW);
  SPI.transfer(tx_buffer, rx_buffer, sizeof(tx_buffer), SPITransferComplete);
  while (!transferComplete) {};
  digitalWrite(SS, HIGH);

  delay(1000);
  ledIsHigh = !ledIsHigh;
  digitalWrite(LED, ledIsHigh);

}