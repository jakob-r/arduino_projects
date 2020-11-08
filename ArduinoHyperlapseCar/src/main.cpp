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
const short int pin_A = 0;   //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
const short int pin_B = 1;    //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
const short int pin_button = 12;    //ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
RotaryEncoder rot_enc(pin_A, pin_B, pin_button);

// User input
short int user_speed = 0;
short unsigned int user_duration = 0;
short int user_drift = 0;

// Car Control
CarControl car;

void rot_enc_interrupt() {
  rot_enc.encoderISR();
}

void setup() {
  rot_enc.setup();
  Timer1.initialize();
  Timer1.attachInterrupt(rot_enc_interrupt, 10000); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  ui.setup(lcd);
}

uint16_t display_button_count = 0;
int16_t display_rotary_position = 0;

void loop() {
  short unsigned int state = 0;
  bool state_changed = false;
  
  if (rot_enc.button_pressed()) {
    state++;
    state_changed = true;
  } else {
    state_changed = false;
  }

  ui.set_value(lcd, rot_enc.rotary_position);

  if (state_changed) {
    if (state == 0) {
      ui.show_selection_speed(lcd);
    } else if (state == 1) {
      ui.show_selection_duration(lcd);
    } else if (state == 2) {
      ui.show_selection_drift(lcd);
    } else if (state == 3) {
      ui.show_running(lcd);
    }
  }

  if (state == 0) {
    user_speed = rot_enc.rotary_position;
  } else if (state == 1) {
    user_duration = max(0, rot_enc.rotary_position);
  } else if (state == 2) {
    user_drift = rot_enc.rotary_position;
  } else if (state == 3) {
    car.drive(user_speed, user_duration, user_drift);
  }




  
  // int button_count = digitalRead(pin_button);
  // if (display_button_count != rot_enc.button_count) {
  lcd.setCursor(10, 1);
  lcd.print(digitalRead(pin_button));
  //   display_button_count = button_count;
  // }
  
}