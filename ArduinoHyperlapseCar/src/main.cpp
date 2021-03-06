#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <Wire.h>

#include "RotaryEncoder.h"
#include "UserInterface.h"
#include "CarControl.h"

// PINS

// LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// User Interface
UserInterface ui;

// Rotary Encoder
const int8_t pin_A = 0;   //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
const int8_t pin_B = 1;    //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
const int8_t pin_button = 12;    //ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
RotaryEncoder rot_enc(pin_A, pin_B, pin_button);

// User input
int8_t user_speed = 0;
uint16_t user_duration = 0;
int8_t user_drift = 0;

// Car Control
CarControl car;

// Menu States
uint8_t state = 0;
bool state_changed = true;

void interrupt() {
  rot_enc.encoderISR();
}

void setup() {
  rot_enc.setup();
  Timer1.initialize();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  ui.setup(lcd);
  car.setup();
}

void loop() {

  if (state_changed) {
    if (state == 0) {
      Timer1.attachInterrupt(interrupt, 10000);
      ui.show_selection_speed(lcd);
    } else if (state == 1) {
      user_speed = rot_enc.rotary_position;
      ui.show_selection_duration(lcd);
    } else if (state == 2) {
      user_duration = max(1, rot_enc.rotary_position);
      ui.show_selection_drift(lcd);
    } else if (state == 3) {
      //user_drift = rot_enc.rotary_position;
      ui.show_start(lcd);
    } else if (state == 4) {
      Timer1.detachInterrupt();
      ui.show_running(lcd);
      car.drive(user_speed, user_duration, user_drift);
    }
    rot_enc.rotary_position = 0;
  }

  if (state == 4) {
    if (car.stopped) {
      state_changed = true;
      state = 0;
    }
  } else {
    ui.set_value(lcd, rot_enc.rotary_position);
    state_changed = rot_enc.button_pressed();
    state = state + state_changed;
  }
  
  
  // int button_count = digitalRead(pin_button);
  // if (display_button_count != rot_enc.button_count) {
  lcd.setCursor(10, 1);
  lcd.print(state);
  //   display_button_count = button_count;
  // }
  
}