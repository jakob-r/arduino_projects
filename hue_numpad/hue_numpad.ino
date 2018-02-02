#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Keypad.h>
#include <ArduinoJson.h>

// HUE SETTINGS
const char* HUE_IP = "192.168.0.4";
//const char* HUE_USER = "..."; // --> define in private.ino
extern const char* HUE_USER;
const int HUE_LIGHT = 3;
const int NLIGHTS = 9;
const int MAPLIGHTS[NLIGHTS] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; //position in array = numpad
const int NTRUELIGHTS = 7;
const int TRUELIGHTS[7] = {1, 3, 4, 5, 6, 7, 8}; //things that are not lights.

// WIFI SETTINGS
//const char* WLAN_SSID = "..."; // --> define in private.ino
//const char* WLAN_PASS = "..."; // --> define in private.ino
extern const char* WLAN_SSID;
extern const char* WLAN_PASS;

// IFTT SETTINGS
WiFiClientSecure ifttt_client;
const char* ifttt_url = "https://maker.ifttt.com/trigger/arduino_motionhome/with/key/";
extern const char* IFTTT_KEY;
const char* ifttt_sha1_fingerprint = "C0 5D 08 5E E1 3E E0 66 F3 79 27 1A CA 1F FC 09 24 11 61 62";

// Keypad Settings
const byte ROWS = 4; //4 Reihen
const byte COLS = 4; //4 Zeilen
//Definieren der Symbole auf den Tasten der Tastenfelder
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {D0, D1, D2, D3}; //Verbindung zu den Zeilenzugaben des Tastenfeldes
byte colPins[COLS] = {D4, D5, D6, D7}; //An die Spaltenbelegungen der Tastatur anschließen

//Initialisierung einer Instanz der Klasse NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//IR Motion
const byte PIN_IR = D8;
bool irFired = true;

// Light Sensor  (Photoresistor)
const byte PIN_PR = A0;

// Global Variables we use to output functions
int state_light;
bool state_on;
int state_bri;
int state_ct;
//String state_colormode;
bool state_reachable;

// Helper globals
bool partyModeOn = false;
unsigned int partyModeMills = 0;
bool partyModeLightsOn = false;

void setup() {
  Serial.begin(115200);
  delay(50);

  pinMode(PIN_IR, INPUT);

  connectWifi();
  delay(1000);
}

void connectWifi() {
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected with ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect (true);
}

void setHue(int lightNum, boolean ison, int bri, int transitiontime) {
  String url = String("http://") + HUE_IP + "/api/" + HUE_USER + "/lights/" + lightNum + "/state";
  String command;
  serializeHueJson(ison, bri, transitiontime, command);
  String result;
  doHttp("PUT", url, command, result);
}

void getHue(int lightNum) {
  String url = String("http://") + HUE_IP + "/api/" + HUE_USER + "/lights/" + lightNum;
  String result;
  String command;
  doHttp("GET", url, command, result);
  parseHueJson(result);
  state_light = lightNum; 
}

// Lamp Helpers
void lightsAllOff() {
  for(int i=0; i < NLIGHTS; i++) {
    setHue(MAPLIGHTS[i], false, NULL, NULL);
  }
}

void lightsAllOn() {
  for(int i=0; i < NLIGHTS; i++) {
    setHue(MAPLIGHTS[i], true, NULL, NULL);
  }
}

bool anyLightOn() {
  for (int i = 0; i < NTRUELIGHTS; i++) {
    Serial.print(i);
    getHue(TRUELIGHTS[i]);
    if (state_on == true) {
      return true;
      Serial.println("<-");
    }
  }
  return false;
}

// Decoupled Stuff
void partyMode() {
  static int previousMillis = millis();
  if (partyModeOn) {
    if (millis() - previousMillis >= 200) {
      if (partyModeLightsOn) {
        for(int i=0; i < NTRUELIGHTS; i++) {
          setHue(TRUELIGHTS[i], false, NULL, 2);
        }  
      } else {
        for(int i=0; i < NTRUELIGHTS; i++) {
          setHue(TRUELIGHTS[i], true, 150, 1);
        }  
      }
      partyModeLightsOn = !partyModeLightsOn;
      previousMillis = millis();
    }
  }
}

String connectionStatus ( int which )
{
    switch ( which )
    {
        case WL_CONNECTED:
            return "Connected";
            break;

        case WL_NO_SSID_AVAIL:
            return "Network not availible";
            break;

        case WL_CONNECT_FAILED:
            return "Wrong password";
            break;

        case WL_IDLE_STATUS:
            return "Idle status";
            break;

        case WL_DISCONNECTED:
            return "Disconnected";
            break;

        default:
            return "Unknown";
            break;
    }
}

void loop() {
  // Handle Stuff that always happens
  partyMode();

  // Motion Detector
  Serial.println(analogRead(PIN_PR));
  delay(500);
//  if (analogRead(PIN_PR) < 100) {
    bool irSignal = digitalRead(PIN_IR);
    if (irSignal && !irFired) { 
      Serial.print("Motion detected... ");
      irFired = true;
      bool anyLight = anyLightOn();
      if (anyLight == false) {
        Serial.print("and all lights off... ");
        lightsAllOn();
      }
      Serial.println("!");
    } else if (!irSignal && irFired) {
      irFired = false;
    }  
//  }
  
  // Handle Keypad
  char customKey = customKeypad.getKey();
  if (customKey != NULL) {
    Serial.print("Key pressed: ");
    Serial.println(customKey);
    if (customKey == '0') {
      lightsAllOff();
    } else if (customKey == 'A') {
      lightsAllOn();
    } else if (customKey == '*') {
      partyModeOn = !partyModeOn;
    } else {
      int lamp = customKey - '0';
      lamp = MAPLIGHTS[lamp - 1];
      getHue(lamp);
      if (state_on == true) {
        setHue(lamp, false, NULL, NULL);
      } else {
        setHue(lamp, true, NULL, NULL);
      }  
    }
  }

  // Reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Problem :");
    Serial.println(connectionStatus( WiFi.status() ).c_str());
    WiFi.disconnect();
    delay(500);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    Serial.print("Reconnect to ");
    Serial.println(WLAN_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }  
  }
  
}
