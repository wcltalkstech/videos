#include "application.h"
#line 1 "/Users/kevinmcquown/Dropbox/WCL/wcltalkstech/spiInDepth/src/spiInDepth.ino"
void setup();
void SPITransferComplete();
void loop();
#line 1 "/Users/kevinmcquown/Dropbox/WCL/wcltalkstech/spiInDepth/src/spiInDepth.ino"
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
#define SS A5 // default for Xenon, we can use any GPIO!
#define OE D6 // Output Enable for Shift Register, drives parallel output pins when low, otherwise floating
#define RCLK D5 // Latches in on rising edge
#define SRCLR D4 // Clear parallel outputs when LOW
byte tx_buffer[1] = {0xF0};
byte rx_buffer[1] = {0x00};
volatile bool transferComplete = false;
void setup() {
  SPI.begin(SS);
  SPI.setClockSpeed(40, MHZ);
  SPI.setBitOrder(LSBFIRST);  // default is MSBFIRST
  pinMode(OE, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLR, OUTPUT);
  digitalWrite(OE, HIGH);
  digitalWrite(RCLK, LOW);
  digitalWrite(SRCLR, HIGH);
}
void SPITransferComplete() {
  transferComplete = true;
}
void loop() {
  transferComplete = false;
  digitalWrite(SS, LOW);
  digitalWrite(OE, HIGH); //outputs floating
  digitalWrite(SRCLR, LOW); // clear the data
  delayMicroseconds(20);
  digitalWrite(SRCLR, HIGH); // turn off the clearing
  digitalWrite(RCLK, LOW);
  SPI.transfer(tx_buffer, rx_buffer, sizeof(tx_buffer), SPITransferComplete);
  while (!transferComplete) {};
  digitalWrite(RCLK, HIGH); // latch the serial input to the parallel lines
  digitalWrite(OE, LOW); // outputs are pulled high or low as appropriate
  digitalWrite(SS, HIGH);
  delay(1000);
}
