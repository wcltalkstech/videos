SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define TEMP_DATA_SIZE 10
#define QUEUE_SIZE 10
#define OLED_RESET D4
#define TIME_STRING_SIZE 10
#define DATA_IS_OLD 10000 // no update after these number of milliseconds

Adafruit_SSD1306 display(OLED_RESET);

unsigned long timeOfLastUpdate;
char timeString[TIME_STRING_SIZE];

os_thread_t thread;
os_thread_t cloudThread;
os_mutex_t mutex;
os_queue_t queue;

void sendToCloud(void *arg) {
  char data[TEMP_DATA_SIZE];
  while (1) {
    os_queue_take(queue, &data, 100000, 0);
    if (Particle.connected()) {
      Particle.publish("temperature", data);
    }
  }
}

void messageLED(void *arg) {
  while (1) {
    os_mutex_lock(mutex);
    digitalWrite(D5, HIGH);
    delay(2000);
    digitalWrite(D5, LOW);
  }
}

void displayMessage(uint8_t textSize, uint8_t lineNumber, const char *data) {
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(0, lineNumber*18);
  display.println(data);
  display.display();
}
void showTemp(const char *event, const char *data)
{
  char localCopy[TEMP_DATA_SIZE];
  memcpy((void*)&localCopy, (void *)data, TEMP_DATA_SIZE);
  os_queue_put(queue, localCopy, 0, 0);
  unsigned long delta = millis() - timeOfLastUpdate;
  timeOfLastUpdate = millis();
  display.clearDisplay();
  displayMessage(2,0,localCopy);
  snprintf(timeString, sizeof(timeString), "%d", delta);
  displayMessage(2, 1, timeString);
  os_mutex_unlock(mutex);

}

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);

  digitalWrite(ANTSW1, 0);
  digitalWrite(ANTSW2, 1);

  display.clearDisplay();
  displayMessage(2, 0, "Mesh...");

  Mesh.on();
  Mesh.connect();
  waitUntil(Mesh.ready);

  display.clearDisplay();
  displayMessage(2, 0, "WiFi...");
  WiFi.on();
  WiFi.connect();
  waitUntil(WiFi.ready);

  display.clearDisplay();
  displayMessage(2, 0, "Prtcl...");
  Particle.connect();
  waitUntil(Particle.connected);

  display.clearDisplay();
  displayMessage(2, 0, "Temp...");

  os_mutex_create(&mutex);
  os_queue_create(&queue, TEMP_DATA_SIZE, QUEUE_SIZE, NULL);

  os_thread_create(&thread, NULL, OS_THREAD_PRIORITY_DEFAULT, messageLED, NULL, 2048);
  os_thread_create(&cloudThread, NULL, OS_THREAD_PRIORITY_DEFAULT, sendToCloud, NULL, 2048);

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