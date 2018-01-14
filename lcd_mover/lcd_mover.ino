#include "mytypes.h"

// 8x8 LED Module
#include "LedControl.h"
// DIN, CLK, LOAD (CS), No of Displays
LedControl lc=LedControl(4,2,3,1);

// MPU-6050
// Nano: A4 SDA, A5 SCL
#include<Wire.h> 
const int MPU_addr=0x68;  // I2C-Adresse des MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

// for the game
const int delaytime = 25;
// include queue library header.
#include <QueueArray.h>
QueueArray <int> queueX;
QueueArray <int> queueY;
const int queueN = 5;
int queueNnow = 0;

void setup() {
  Serial.begin(9600);
  setupDisplay();
  setupMpu();
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
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}
/* 
 This function will light up every Led on the matrix.
 The led will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void single() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      delay(delaytime);
      lc.setLed(0,row,col,true);
      delay(delaytime);
      for(int i=0;i<col;i++) {
        lc.setLed(0,row,col,false);
        delay(delaytime);
        lc.setLed(0,row,col,true);
        delay(delaytime);
      }
    }
  }
}

void loop() {
  readMpu();
//  Serial.print("AcX = "); Serial.print(AcX);
//  Serial.print(" | AcY = "); Serial.print(AcY);
//  Serial.print(" | AcZ = "); Serial.print(AcZ);
//  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //Gleichung für Temperatur in Grad C aus Datenblatt
//  Serial.print(" | GyY = "); Serial.print(GyY);
//  Serial.print(" | GyZ = "); Serial.println(GyZ);
  byte static x;
  byte static y;
  byte y_new = map(AcZ, 5000, -5000, 0, 7);
  byte x_new = map(AcY, 5000, -5000, 0, 7);
  if (x_new != x || y_new != y) {
    x = x_new;
    y = y_new;
    lc.setLed(0,x,y,true);
    queueX.push(x);
    queueY.push(y);
    if (queueNnow < queueN) {
      queueNnow++;
    } else {
      x_new = queueX.pop();
      y_new = queueY.pop();
      lc.setLed(0,x_new,y_new,false);
    }  
  }
  
  delay(delaytime);
}
