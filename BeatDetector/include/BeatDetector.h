// Arduino Beat Detector By Damian Peckett 2015
// Adapted by Jakob Richter for Mic KY-037 
// License: Public Domain.

// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


class BeatDetector {

    // 20 - 200hz Single Pole Bandpass IIR Filter
    float bassFilter(float sample) {
        static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
        xv[0] = xv[1]; xv[1] = xv[2]; 
        xv[2] = sample / 9.1f;
        yv[0] = yv[1]; yv[1] = yv[2]; 
        yv[2] = (xv[2] - xv[0])
            + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
        return yv[2];
    }

    // 10hz Single Pole Lowpass IIR Filter
    float envelopeFilter(float sample) { //10hz low pass
        static float xv[2] = {0,0}, yv[2] = {0,0};
        xv[0] = xv[1]; 
        xv[1] = sample / 160.f;
        yv[0] = yv[1]; 
        yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
        return yv[1];
    }

    // 1.7 - 3.0hz Single Pole Bandpass IIR Filter
    float beatFilter(float sample) {
        static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
        xv[0] = xv[1]; xv[1] = xv[2]; 
        xv[2] = sample / 7.015f;
        yv[0] = yv[1]; yv[1] = yv[2]; 
        yv[2] = (xv[2] - xv[0]) + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
        return yv[2];
    }

    void detectBeat() {
        unsigned long endtime = micros()+SAMPLEPERIODUS;
        for (uint8_t i = 0; i < 200; i++) {
            // Read ADC and center so +-512
            sample = (float)analogRead(0)-503.f;
        
            // Filter only bass component
            value = bassFilter(sample);
        
            // Take signal amplitude and filter
            envelope = envelopeFilter(abs(value));
            
            // Consume excess clock cycles, to keep at 5000 hz
            while (micros() < endtime) {}
        }
        // Filter out repeating bass sounds 100 - 180bpm
        beat = beatFilter(envelope);

        // Threshold it based on potentiometer on AN1
        thresh = 0.004f * (float)analogRead(1);

        // If we are above threshold, light up LED
        if(beat > thresh) digitalWrite(LED_BUILTIN, HIGH);
        else digitalWrite(LED_BUILTIN, LOW);
    }

    public:
        // thresh in (0,4)
        // sample in (-255,255) should be 0 if no sound. loud music (home) makes it go up/down 10 
        // value in (-2.2, 2.5)
        // envelope in (0, 0.6)
        // beat  in (-0.05, 0.06)
        float sample, value, envelope, beat, thresh;
        
        void setup() {   
            // Set ADC to 77khz, max for 10bit
            sbi(ADCSRA,ADPS2);
            cbi(ADCSRA,ADPS1);
            cbi(ADCSRA,ADPS0);
        }

        void loop() {
            detectBeat();
        }
};
