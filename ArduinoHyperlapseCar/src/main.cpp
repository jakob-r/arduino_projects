#include <Arduino.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <Wire.h>

// PINS

// LCD
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Rotary Encoder
const int pin_A = 0;   //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
const int pin_B = 1;    //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
const int pin_button = 12;    //ky-040 sw  pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!

// variables
uint16_t button_count = 0;
int16_t rotary_position = 0;

bool state_both_on = false;
bool state_upwards = false;
bool state_downwards = false;
bool state_button = false;
void encoderISR() {
  // encoder.readAB();
  bool pin_A_val = digitalRead(pin_A);
  bool pin_B_val = digitalRead(pin_B);

  if (pin_A_val == pin_B_val) { // steady position
    state_both_on = pin_A_val;
    state_upwards = state_downwards = false;
  }
  if (!state_both_on) { // if both pins were off
    if (pin_A_val == true && pin_B_val == false && !state_upwards) {
      rotary_position++;
      state_upwards = true;
      state_downwards = false;
    }
    if (pin_A_val == false && pin_B_val == true && !state_downwards) {
      rotary_position--;
      state_upwards = false;
      state_downwards = true;
    }
  } else { // if poth pins were on
    if (pin_A_val == false && pin_B_val == true && !state_upwards) {
      rotary_position++;
      state_upwards = true;
      state_downwards = false;
    }
    if (pin_A_val == true && pin_B_val == false && !state_downwards) {
      rotary_position--;
      state_upwards = false;
      state_downwards = true;
    }
  }
}

void setup() {
  Timer1.initialize();

  Timer1.attachInterrupt(encoderISR, 10000); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(F("Starting..."));                                             //(F()) saves string to flash & keeps dynamic memory free
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  pinMode(pin_button, INPUT_PULLUP);
  delay(500);
  lcd.clear();
  lcd.print(F("POSITION:"));
  lcd.setCursor(0, 1);                                                       //set 1-st column, 2-nd row
  lcd.print(F("BUTTON  :"));
  lcd.setCursor(10, 1);
  lcd.print(button_count);
}

uint16_t display_button_count = 0;
int16_t display_rotary_position = 0;

void loop() {
  if (rotary_position != display_rotary_position) {
    lcd.setCursor(10, 0);
    lcd.print(F("    "));
    lcd.setCursor(10, 0);
    lcd.print(rotary_position);
    display_rotary_position = rotary_position;
  }
  
  button_count = digitalRead(pin_button);
  if (display_button_count != button_count) {
    lcd.setCursor(10, 1);
    lcd.print(button_count);
    display_button_count = button_count;
  }
  
}