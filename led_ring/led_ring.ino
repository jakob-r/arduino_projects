#include <FastLED.h>
#include "my_types.h"
#define DATA_PIN 9
const uint8_t NUM_LEDS = 24;
CRGB leds[NUM_LEDS];

const uint8_t nparts = 3;
particle parts[3];

const uint8_t pause = 50;


void setup() {
  Serial.begin(9600);
  delay(1000); // setup guard
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(120);
  parts[0].x = 0;
  parts[0].l = 40;
  parts[0].color = CHSV(0, 200, 50);
  parts[0].v = 1;
  parts[1].x = 100;
  parts[1].l = 25;
  parts[1].color = CHSV(80, 200, 100);
  parts[1].v = -3;
  parts[2].x = 160;
  parts[2].l = 12;
  parts[2].color = CHSV(150, 200, 200);
  parts[2].v = 5;
}

void clearLeds() {
  memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
}

void renderParticlesToLeds() {
  for (uint8_t i=0; i < nparts; i++) {
    float pixelstartpos = ((float)parts[i].x / 255) * NUM_LEDS;
    float pixelendpos = ((float)(parts[i].x + parts[i].l) / 255) * NUM_LEDS;
    uint8_t startpixel = floor(pixelstartpos);
    uint8_t endpixel = ceil(pixelendpos);
    uint8_t widthpixel = endpixel - startpixel;
    float weightstart = 1 - (pixelstartpos - startpixel);
    float weightend = 1 - (endpixel - pixelendpos);
    leds[startpixel] |= blend(leds[startpixel], parts[i].color, weightstart * 255);
    uint8_t paintpixel = startpixel+1;
    if (paintpixel == NUM_LEDS) paintpixel = 0;
    for (uint8_t j = 1; j < widthpixel; j++) {
      leds[paintpixel] |= parts[i].color;
      paintpixel++;
      if (paintpixel == NUM_LEDS) paintpixel = 0;
    }
    leds[paintpixel] |= blend(leds[paintpixel], parts[i].color, weightend * 255);
  }
}

void moveParticles() {
  for (uint8_t i=0; i < nparts; i++) {
    if (parts[i].v > 0 && parts[i].x > 254 - parts[i].v) {
      parts[i].x = parts[i].x - (254 - parts[i].v);
    } else if (parts[i].v < 0 && parts[i].x < (-1) * parts[i].v) {
      parts[i].x = 254 + parts[i].v + parts[i].x;
    } else {
      parts[i].x += parts[i].v;  
    }
  }
}

void loop() {
  //parts[0].color = CHSV(parts[0].x, 255, 255);
  clearLeds();
  moveParticles();
  renderParticlesToLeds();
  FastLED.show();
  delay(pause);
}
