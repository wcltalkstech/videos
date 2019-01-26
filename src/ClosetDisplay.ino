#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

void showTemp(const char *event, const char *data)
{
  display.clearDisplay();
  display.setCursor(0,5);
  display.print(data);
  display.display();
}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,5);
  display.println("WAITING");
  display.display();

  Particle.subscribe("newTemp", showTemp);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.

}