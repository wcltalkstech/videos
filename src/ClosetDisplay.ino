SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET D4
#define TIME_STRING_SIZE 10
#define DATA_IS_OLD 10000 // no update after these number of milliseconds

Adafruit_SSD1306 display(OLED_RESET);

unsigned long timeOfLastUpdate;
char timeString[TIME_STRING_SIZE];

void displayMessage(uint8_t textSize, uint8_t lineNumber, const char *data) {
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, lineNumber*18);
  display.println(data);
  display.display();
}
void showTemp(const char *event, const char *data)
{
  unsigned long delta = millis() - timeOfLastUpdate;
  timeOfLastUpdate = millis();
  display.clearDisplay();
  displayMessage(2,0,data);
  snprintf(timeString, sizeof(timeString), "%d", delta);
  displayMessage(2, 1, timeString);
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  digitalWrite(ANTSW1, 0);
  digitalWrite(ANTSW2, 1);

  display.clearDisplay();
  displayMessage(2, 0, "Mesh...");

  Mesh.on();
  Mesh.connect();
  waitUntil(Mesh.ready);

  Mesh.subscribe("newTemp", showTemp);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  unsigned long delta = millis() - timeOfLastUpdate;
  if (delta > DATA_IS_OLD) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(90, 0);
      display.println("OLD");
      display.display();
  }
}