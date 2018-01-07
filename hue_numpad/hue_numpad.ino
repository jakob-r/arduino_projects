#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Keypad.h>
#include <ArduinoJson.h>

// HUE SETTINGS
const char* HUE_IP = "192.168.0.4";
//const char* HUE_USER = "fFNkeCHkeWr-w9eO-s68qLSwgoKaMHZRI85ksI6z"; // --> define in private.ino
const int HUE_LIGHT = 3;
const int NLIGHTS = 9;
const int MAPLIGHTS[NLIGHTS] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; //position in array = numpad
const int NTRUELIGHTS = 7;
const int TRUELIGHTS[7] = {1, 3, 4, 5, 6, 7, 8}; //things that are not lights.

// WIFI SETTINGS
//const char* WLAN_SSID = "..."; // --> define in private.ino
//const char* WLAN_PASS = "..."; // --> define in private.ino

WiFiClient client;

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

  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  connectWifi();
  delay(1000);
}

void connectWifi() {
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
  Serial.print("WiFi connected with");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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
    getHue(TRUELIGHTS[i]);
    if (state_on == true) {
      return true;
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

void loop() {
  // Handle Stuff that always happens
  partyMode();

  // Motion Detector
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
}
