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

// for the game
const int delaytime = 1000;
boolean gameOver = false;
byte counter = 0;
Timer timer; // craete a timer object
int8_t timer_ingame;
int8_t timer_blink;
uint16_t score;
const uint8_t powerDotStartWorth = 10;
uint8_t powerDotWorth;


// Display writing queue
// which leds to turn on and of
#include <QueueList.h>
QueueList <coord> ledOn;
QueueList <coord> ledOff;

void setup() {
  Serial.begin(9600);
  setupDisplay();
  setupMpu();
  startGame();
  timer_blink = timer.every(100, blinkGoal);
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

void setupMpu() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Auf Null setzen (weckt die MPU-6050 auf)
  Wire.endTransmission(true);
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

void blinkGoal(byte x, byte y) {
  Serial.print("Score: ");
  Serial.print(score);
  Serial.print(" PowerDot: ");
  Serial.print(" Speed: ");
  Serial.println(1000 / pow(2, counter));
  
  static boolean on;
  static coord last;
  coord now = {coordsLoop(x), coordsLoop(y)};
  if ((last.x == now.x && last.y == now.y) || !on && (last.x != now.x || last.y != now.y)) {
    if (on && collisionWithSnake(now) == 254) {
      ledOff.push(now);
    } else {
      ledOn.push(now);
    }
  } else if ((last.x != now.x || last.y != now.y) && collisionWithSnake(last) == 254) {
    ledOff.push(last);
  }
  last = now;
  on = !on;
}

void startGame(){
  gameOver = false;
  snakeStart(random(1,8), random(1,8), 1);
  delay(delaytime);
  newPowerDot();
  timer_ingame = timer.every(1000, moveSnake);
  powerDotWorth = powerDotStartWorth;
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
