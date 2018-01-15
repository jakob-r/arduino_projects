#include "mytypes.h"

// 8x8 LED Module
#include "LedControl.h"
// DIN, CLK, LOAD (CS), No of Displays
LedControl lc=LedControl(4,2,3,1);

// MPU-6050
// Nano: A4 SDA, A5 SCL
#include<Wire.h> 
const int MPU_addr=0x68;  // I2C-Adresse des MPU-6050
int16_t AcY,AcZ,AcX; //left out: ,AcX,Tmp,GyX,GyY,GyZ;

// for the game
const int delaytime = 100;
boolean gameOver = false;

void setup() {
  Serial.begin(9600);
  setupDisplay();
  setupMpu();
  snakeStart(4,2,5);
  writeDisplay();
  delay(delaytime);
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
  Wire.write(0x3B);  // Beginnend mit register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // Fordert insgesamt 14 Register an
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  //Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  //GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  //GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  //GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

float mapf(int16_t x, int in_min, int in_max, int out_min, int out_max)
{
 return (float)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  readMpu();
//  Serial.print("AcX = "); Serial.print(AcX);
//  Serial.print(" | AcY = "); Serial.print(AcY);
//  Serial.print(" | AcZ = "); Serial.println(AcZ);
//  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //Gleichung für Temperatur in Grad C aus Datenblatt
//  Serial.print(" | GyY = "); Serial.print(GyY);
//  Serial.print(" | GyZ = "); Serial.println(GyZ);
  static float x;
  static float y;
//  AcZ = random(-10000,10000);
//  AcY = random(-10000,10000);
  y = mapf(AcZ, 8000, -8000, 1, 8);
  x = mapf(AcY, 8000, -8000, 1, 8);

  if(!gameOver){
    gameOver = !moveSnakeTo(x, y);
    writeDisplay();
    delay(delaytime);
  }
    
//  Serial.println("Left");
//  moveSnakeTo(2, 3);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(1, 3);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(0, 3);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-1, 3);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 3);
//  writeDisplay();
//  delay(delaytime);
//  Serial.println("Up");
//  moveSnakeTo(-2, 4);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 5);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 6);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 7);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 8);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 9);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(-2, 10);
//  writeDisplay();
//  delay(delaytime);
//  Serial.println("Right");
//  moveSnakeTo(-1, 10);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(0, 10);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(1, 10);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(2, 10);
//  writeDisplay();
//  delay(delaytime);
//  Serial.println("Down");
//  moveSnakeTo(2, 9);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(2, 8);
//  writeDisplay();
//  delay(delaytime);
//  moveSnakeTo(2, 7);
//  writeDisplay();
//  delay(delaytime);
  
}
