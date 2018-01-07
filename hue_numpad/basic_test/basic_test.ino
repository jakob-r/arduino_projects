#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Keypad.h>
#include <ArduinoJson.h>

// HUE SETTINGS
const char* HUE_IP = "192.168.0.4";
const char* HUE_USER = "fFNkeCHkeWr-w9eO-s68qLSwgoKaMHZRI85ksI6z";
const int HUE_LIGHT = 3;
const int NLIGHTS = 9;
const int MAPLIGHTS[NLIGHTS] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; //defines the mapping 1 will be the first of this array and so on.


// WIFI SETTINGS
const char* WLAN_SSID = "das soziale Netzwerk";
const char* WLAN_PASS = "WLAN_passwort";

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

WiFiClient client;

// Global Variables we use to output functions
int state_light;
bool state_on;
int state_bri;
int state_ct;
//String state_colormode;
bool state_reachable;

void setup() {
  Serial.begin(115200);
  delay(50);

  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  connectWifi();
  delay(1000);
  getHue(3);
  Serial.println(state_bri);
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

void doHttp(String action, String &url, String &command, String &result) {
  if ((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    http.begin(url);

    int httpCode;
    if (action == "GET") {
      httpCode = http.GET();  
    } else if (action == "PUT") {
      httpCode = http.PUT(command);  
    }

    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] %s... code: %d\n", action.c_str(), httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        result = http.getString();
        Serial.print(result.substring(0,63));
        Serial.println("...");
      }
    } else {
      Serial.printf("[HTTP] %s... failed, error: %s\n", action.c_str(), http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void setHue(int lightNum, boolean ison, int bri, int transitiontime) {
  String url = String("http://") + HUE_IP + "/api/" + HUE_USER + "/lights/" + lightNum + "/state";
  String command;
  serializeHueJson(ison, bri, transitiontime, command);
  Serial.println(command);
  String result = "Fuck this!";
  doHttp("PUT", url, command, result);
  Serial.println(url);
  Serial.println(result);
}

void getHue(int lightNum) {
  String url = String("http://") + HUE_IP + "/api/" + HUE_USER + "/lights/" + lightNum;
  String result;
  String command;
  doHttp("GET", url, command, result);
  parseHueJson(result);
  state_light = lightNum; 
}

void parseHueJson(String &json) {
  // Thanks to awesome service: http://arduinojson.org/assistant/
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(11) + 440;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(json);

  JsonObject& state = root["state"];
  state_on = state["on"]; // false
  state_bri = state["bri"]; // 254
  state_ct = state["ct"]; // 443
  //state_colormode = state["colormode"]; // "ct"
  state_reachable = state["reachable"]; // true
}

void serializeHueJson(boolean ison, int bri, int transitiontime, String &result) {
  // Thanks to awesome service: http://arduinojson.org/assistant/
  const size_t bufferSize = JSON_OBJECT_SIZE(3);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.createObject();
  //if (ison != NULL) {
    root["on"] = (bool)ison;  
  //}
  //if (transitiontime != NULL) {
    root["transitiontime"] = transitiontime;  
  //}
  //if (bri != NULL) {
    root["bri"] = bri;
  //}
  root.printTo(result);
} 

void loop() {
  char customKey = customKeypad.getKey();
  
  if (customKey != NULL) {
    Serial.print("Key pressed: ");
    Serial.println(customKey);
    int lamp = customKey - '0';
    getHue(lamp);
    if (state_on == true) {
      setHue(lamp, false, 200, 2);
    } else {
      setHue(lamp, true, 200, 2);
    }
    state_on = !state_on;
  }
}
