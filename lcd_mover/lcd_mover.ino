#include "mytypes.h"
//timer library
#include "Timer.h"
// 8x8 LED Module
#include "LedControl.h"
#include <binary.h>
// DIN, CLK, LOAD (CS), No of Displays
LedControl lc=LedControl(4,2,3,1);

// MPU-6050
// Nano: A4 SDA, A5 SCL
#include<Wire.h> 
const int MPU_addr=0x68;  // I2C-Adresse des MPU-6050
const int sampleN = 250;
int16_t samplesY = 0;
int16_t samplesZ = 0;

// 4digit 7segment display
const byte CA[4] = {12, 11, 10, 9};
const byte clk = 7;
const byte latch = 6;
const byte data = 5;
byte digits[4];
const byte numbers[12] = {B11111100, B01100000, B11011010, B11110010, B01100110, // 0 - 4
                          B10110110, B10111110, B11100000, B11111110, B11110110, // 5 - 9
                          B00000000, // 10 = off
                          B00000010}; //  11 = -
const byte digit_off = 10;
const byte digit_minus = 11;
uint8_t timer_7seg;

// for the game
const int delaytime = 1000;
boolean gameOver = false;
byte counter = 0;
Timer timer; // craete a timer object
uint8_t timer_ingame;
uint8_t timer_blink;
uint16_t score;
const uint8_t powerDotStartWorth = 20;
uint8_t powerDotWorth;


// Display writing queue
// which leds to turn on and of
#include <QueueList.h>
QueueList <coord> ledOn;
QueueList <coord> ledOff;

void setup() {
  Serial.begin(9600);
  setupDigitDisplay();
  setupDisplay();
  setupMpu();
  startGame();
  timer_blink = timer.every(1000, blinkGoal);
  timer_7seg = timer.every(1, displayDigit);
}

void setupDisplay() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0); 
}

void setupDigitDisplay() {
  pinMode(CA[0], OUTPUT);
  pinMode(CA[1], OUTPUT);
  pinMode(CA[2], OUTPUT);
  pinMode(CA[3], OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(data, OUTPUT);
  resetDigitDisplay();
}

void setupMpu() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Auf Null setzen (weckt die MPU-6050 auf)
  Wire.endTransmission(true);
}

void resetDigitDisplay() {
  digitalWrite(CA[0], HIGH);
  digitalWrite(CA[1], HIGH);
  digitalWrite(CA[2], HIGH);
  digitalWrite(CA[3], HIGH);
}

void displayDigit() { //scanning
  static byte count = 0;
  resetDigitDisplay(); //black screen
  digitalWrite(latch, LOW); //put the shift register to read
  shiftOut(data, clk, LSBFIRST, numbers[digits[count]]); //send the data
  digitalWrite(CA[count], LOW); //turn on the relevent digit
  digitalWrite(latch, HIGH); //put the shift register to write mode
  count++; //count up the digit
  if (count == 4) { // keep the count between 0-3
    count = 0;
  }
}

void setDigitNumber(uint16_t num) { // seperate the input number into 4 single digits
  digits[0] = num / 1000;
  uint16_t first_left = num - (digits[0] * 1000);
  digits[1] = first_left / 100;
  uint16_t second_left = first_left - (digits[1] * 100);
  digits[2] = second_left / 10;
  digits[3] = second_left - (digits[2] * 10);
  if (num < 1000) digits[0] = digit_off;
  if (num < 100) digits[1] = digit_off;
  if (num < 10) digits[2] = digit_off;
}

void readMpu() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3D);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,4,true);  // Fordert insgesamt 14 Register an    
  samplesY = setSample(samplesY, Wire.read()<<8|Wire.read());  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  samplesZ = setSample(samplesZ, Wire.read()<<8|Wire.read());  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}

float mapf(long x) {
  const long in_min = 8000;
  const long in_max = -8000;
  const long out_min = 1;
  const long out_max = 8;
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

int16_t setSample(int16_t a, int16_t x) {
  return (float)a * ((float)(sampleN-1)/(float)sampleN) + ((float)x / (float)sampleN);
}

void blinkGoal() {
  static boolean showScore= true;
  if (showScore) {
    setDigitNumber(score);
  } else {
    setDigitNumber(powerDotWorth * 10);
    digits[0] = digit_off;
    digits[3] = digit_off;
  }
  showScore = !showScore;
//  Serial.print("Score: ");
//  Serial.print(score);
//  Serial.print(" PowerDot: ");
//  Serial.print(powerDotWorth);
//  Serial.print(" Speed: ");
//  Serial.println(1000 / pow(2, counter));
//  float y = mapf(samplesZ);
//  float x = mapf(samplesY);
//  static boolean on;
//  static coord last;
//  coord now = {coordsLoop(x), coordsLoop(y)};
//  if ((last.x == now.x && last.y == now.y) || !on && (last.x != now.x || last.y != now.y)) {
//    if (on && collisionWithSnake(now) == 254) {
//      ledOff.push(now);
//    } else {
//      ledOn.push(now);
//    }
//  } else if ((last.x != now.x || last.y != now.y) && collisionWithSnake(last) == 254) {
//    ledOff.push(last);
//  }
//  last = now;
//  on = !on;
}

void startGame(){
  gameOver = false;
  snakeStart(random(1,8), random(1,8), 1);
  delay(delaytime);
  newPowerDot();
  timer_ingame = timer.every(1000, moveSnake);
  powerDotWorth = powerDotStartWorth;
  score = 0;
}

void moveSnake() {
  float y = mapf(samplesZ);
  float x = mapf(samplesY);
  byte state = moveSnakeTo(x, y);
  if (state == 0) {
    // Collision
    gameOver = true;
  } else if (state == 1) {
    // Move
    if (powerDotWorth>1) {
      powerDotWorth--;
    }
  } else if (state == 2) {
    // Stay at Position
  } else if (state == 3) {
    // Hit powerDot
    counter++;
    timer.stop(timer_ingame);
    score += powerDotWorth;
    newPowerDot();
    timer_ingame = timer.every(1000 / pow(2, counter), moveSnake);
  }
}

void loop() {
  readMpu();
  
  timer.update();

  if (gameOver){
    timer.stop(timer_ingame);
    for (int i=0; i < 8; i++) {
      lc.setColumn(0,i,B11111111);
      delay(100); 
    }
    gameOver = !gameOver;
    lc.clearDisplay(0);
    startGame();
  }
  writeDisplay();
}
