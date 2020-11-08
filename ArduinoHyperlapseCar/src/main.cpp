#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <Wire.h>

#include "RotaryEncoder.h"
#include "UserInterface.h"

// PINS

// LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// User Interface
UserInterface ui(lcd);

// Rotary Encoder
const short int pin_A = 0;   //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
const short int pin_B = 1;    //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
const short int pin_button = 12;    //ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
RotaryEncoder rot_enc(pin_A, pin_B, pin_button);

void rot_enc_interrupt() {
  rot_enc.encoderISR();
}

void setup() {
  Timer1.initialize();
  Timer1.attachInterrupt(rot_enc_interrupt, 10000); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(F("Starting..."));                                             //(F()) saves string to flash & keeps dynamic memory free

  delay(500);
  lcd.clear();
  lcd.print(F("POSITION:"));
  lcd.setCursor(0, 1);                                                       //set 1-st column, 2-nd row
  lcd.print(F("BUTTON  :"));
  lcd.setCursor(10, 1);
  lcd.print(rot_enc.button_count);
}

uint16_t display_button_count = 0;
int16_t display_rotary_position = 0;

void loop() {
  if (rot_enc.rotary_position != display_rotary_position) {
    lcd.setCursor(10, 0);
    lcd.print(F("    "));
    lcd.setCursor(10, 0);
    lcd.print(rot_enc.rotary_position);
    display_rotary_position = rot_enc.rotary_position;
  }
  
  int button_count = digitalRead(pin_button);
  if (display_button_count != rot_enc.button_count) {
    lcd.setCursor(10, 1);
    lcd.print(button_count);
    display_button_count = button_count;
  }
  
}