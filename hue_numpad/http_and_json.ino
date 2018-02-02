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

    if (httpCode > 0 || true) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] %s... code: %d\n", action.c_str(), httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        result = http.getString();
        Serial.print("[HTTP] ");
        Serial.print(result.substring(0,63));
        Serial.println("...");
      }
    } else {
      Serial.printf("[HTTP] %s... failed, error: %s\n", action.c_str(), http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Not connected to WiFi anymore");
  }
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
  
  root["on"] = (bool)ison;  
  if (transitiontime != NULL) {
    root["transitiontime"] = transitiontime;  
  }
  if (bri != NULL) {
    root["bri"] = bri;
  }
  root.printTo(result);
}
