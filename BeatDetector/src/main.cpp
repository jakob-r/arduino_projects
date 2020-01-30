#include <Arduino.h>
#include <Plotter.h>

#include "BeatDetector.h"

Plotter p; // create plotter

BeatDetector beat_detector;
  
void setup() {
    delay(1000); // setup guard

    //The pin with the LED
    pinMode(LED_BUILTIN, OUTPUT);

    beat_detector.setup();

    p.Begin(); // start plotter
    //p.AddTimeGraph("sample", 1000, "sample", sample);
    p.AddTimeGraph("value", 1000, "value", beat_detector.value);
    p.AddTimeGraph("envelope", 1000, "envelope", beat_detector.envelope);
    p.AddTimeGraph("beat", 1000, "beat", beat_detector.beat);
    p.AddTimeGraph("thresh", 1000, "thresh", beat_detector.thresh);

    
}



void loop() {
    beat_detector.loop();
    p.Plot();
}