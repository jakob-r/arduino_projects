#include <IFTTTMaker.h>
#include <Timer.h>

/*******************************************************************
 *  Trigger event at end of the setup                              *
 *                                                                 *
 *  adapted by Brian Lough                                         *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

extern const char* WLAN_SSID;
extern const char* WLAN_PASS;
extern const char* IFTTT_KEY;

const char* EVENT_BOOT = "boot_device";
const char* EVENT_MOTION = "motion_detected";
const uint32_t RETENTION_TIME = 30000;
uint8_t timer_motion;

IPAddress ip;

WiFiClientSecure client;
IFTTTMaker ifttt(IFTTT_KEY, client);

Timer timer;

// Led Connected
const byte PIN_LED_G = D8;
const byte PIN_LED_R = D7;
const byte PIN_IR = D6;

void setup() {

  Serial.begin(115200);
  setupWifi();
  sendDeviceStarted();

  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_IR, INPUT);

  timer.every(250, checkConnected);
  startMotionDetection();
}

void setupWifi() {
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  ip = WiFi.localIP();
  Serial.println(ip);
}

void sendDeviceStarted() {
  //triggerEvent takes an Event Name and then you can optional pass in up to 3 extra Strings
  if(false || ifttt.triggerEvent(EVENT_BOOT, WLAN_SSID, ip.toString())){
    Serial.println("Successfully sent");
  } else {
    Serial.println("Failed!");
  }
}

void sendMotionDetected() {
  //triggerEvent takes an Event Name and then you can optional pass in up to 3 extra Strings
  if(false || ifttt.triggerEvent(EVENT_MOTION)){
    Serial.println("Motion Detection Successfully sent");
  } else {
    Serial.println("Failed!");
  }
}

void checkMotion() {
  static bool irFired = false;
  bool irSignal = digitalRead(PIN_IR);
  if (irSignal && !irFired) {
    digitalWrite(PIN_LED_R, LOW);
    irFired = true;
    sendMotionDetected();
    timer.stop(timer_motion);
    timer.after(RETENTION_TIME, startMotionDetection);
  } else if (!irSignal && irFired) {
    irFired = false;
  }
}

void startMotionDetection() {
  Serial.println("Start motion detection...");
  digitalWrite(PIN_LED_R, HIGH);
  timer_motion = timer.every(1, checkMotion);
}

void checkConnected() {
  static uint8_t timer_offline;
  if (WiFi.status() != WL_CONNECTED) {
    timer_offline = timer.oscillate(PIN_LED_G, 500, HIGH);
  } else {
    timer.stop(timer_offline);
    digitalWrite(PIN_LED_G, HIGH);
  }
}

void loop() {
  timer.update();
}
