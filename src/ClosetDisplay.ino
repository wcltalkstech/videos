SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "WCL_WatchDog.h"

#define TEMP_DATA_SIZE 10
#define MQTT_CLIENT_NAME "argonTempDisplay"
#define QUEUE_SIZE 10
#define TRIGGER_WDT_BTN D3
#define MESSAGE_LED A2
#define PETTING_OFF A1
#define TIME_STRING_SIZE 10
#define DATA_IS_OLD 10000 // no update after these number of milliseconds

#define WATCHDOG_TIMEOUT_MS 30000

Adafruit_SSD1306 display(D18);

WCL_WatchDog wd;

unsigned long timeOfLastUpdate;
char timeString[TIME_STRING_SIZE];

os_thread_t thread;
os_thread_t cloudThread;
os_mutex_t mutex;
os_queue_t queue;

void sendToCloud(void *arg) {
  char data[TEMP_DATA_SIZE];
  while (1) {
    os_queue_take(queue, &data, CONCURRENT_WAIT_FOREVER, 0);
    if (Particle.connected()) {
      Particle.publish("temperature", data);
    }
  }
}

void messageLED(void *arg) {
  while (1) {
    os_mutex_lock(mutex);
    digitalWrite(MESSAGE_LED, HIGH);
    delay(2000);
    digitalWrite(MESSAGE_LED, LOW);
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

  pinMode(MESSAGE_LED, OUTPUT);
  pinMode(PETTING_OFF, OUTPUT);
  digitalWrite(MESSAGE_LED, LOW);
  digitalWrite(PETTING_OFF, LOW);

  pinMode(TRIGGER_WDT_BTN, INPUT);

  digitalWrite(ANTSW1, 0);
  digitalWrite(ANTSW2, 1);

  display.clearDisplay();
  if (System.resetReason() == RESET_REASON_WATCHDOG) {
    displayMessage(2, 0, "WDT RST");
  } else {
    displayMessage(2, 0, "RST");
  }
  delay(2000);

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

  wd.initialize(WATCHDOG_TIMEOUT_MS);
}

bool keepPettingWatchDog = true;

void loop() {
  if (digitalRead(TRIGGER_WDT_BTN) == LOW) {
    keepPettingWatchDog = false;
    digitalWrite(PETTING_OFF, HIGH);
  }
  if (keepPettingWatchDog && Particle.connected()) {
    wd.pet();
  }
  unsigned long delta = millis() - timeOfLastUpdate;
  if (delta > DATA_IS_OLD) {
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(90, 0);
      display.println("OLD");
      display.display();
  }
}