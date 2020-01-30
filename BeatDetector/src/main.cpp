#include <Arduino.h>
#include <Plotter.h>
#include <FastLED.h>

#include "BeatDetector.h"

#define PIN_LED 9

Plotter p; // create plotter

BeatDetector beat_detector;

const uint8_t NUM_LEDS = 24;
CRGB leds[NUM_LEDS];
  
void setup() {
    delay(1000); // setup guard

    beat_detector.setup();

    FastLED.addLeds<WS2812, PIN_LED, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(120);

    p.Begin(); // start plotter
    //p.AddTimeGraph("sample", 1000, "sample", sample);
    p.AddTimeGraph("value", 1000, "value", beat_detector.value);
    p.AddTimeGraph("envelope", 1000, "envelope", beat_detector.envelope);
    p.AddTimeGraph("beat", 1000, "beat", beat_detector.beat);
    p.AddTimeGraph("thresh", 1000, "thresh", beat_detector.thresh);

    
}



void loop() {
    static uint8_t nled_lights_old;
    static uint8_t nled_lights;
    static uint8_t brightness;
    static uint8_t i;

    beat_detector.loop();

    memset8(leds, 0, NUM_LEDS * sizeof(CRGB));
    // smooth leds
    nled_lights = min((int)(abs(beat_detector.value) * 4 / beat_detector.thresh), NUM_LEDS);
    if (nled_lights < nled_lights_old && nled_lights_old >= 2) {
        nled_lights = nled_lights_old - 2;
    }
    brightness = (uint8_t)(min(max(0.25, beat_detector.envelope) * 80 / beat_detector.thresh, 255));
    nled_lights_old = nled_lights;
    fill_solid(leds, nled_lights, CHSV(i, 255, brightness) );
    FastLED.show();
    // if the volume is higher we faster cycle
    i = (i + nled_lights) % 255;
    
    p.Plot();
}