#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "private.h"

ESP8266WebServer server ( 80 );

extern const char* const WLAN_SSID;
extern const char* const WLAN_PASS;

IPAddress ip;

StaticJsonDocument<200> doc;


void apiJson(){
  doc["value"] = random(0,255);
  doc["envelope"] = 0;
  doc["beat"] = 0;
  doc["threshold"] = 0;
  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
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

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount failed");
    while(true) yield(); //Stop here
  }

  setupWifi();

  server.on("/api.json", apiJson);
  server.serveStatic("/index.html", SPIFFS, "/index.html");
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop() {
  server.handleClient();
}