//The snake moves from righy to left in this array
// - - - HEAD < < < TAIL - - -
// < TAIL - - - - - - HEAD < <
const byte maxSnakeN = 64;
coord snake[maxSnakeN];
const coord nacoord = {0, 0};
byte snakeHeadInd = 0;
byte snakeLength = 0;

const byte squareN = 8;

// which leds to turn on and of
#include <QueueList.h>
QueueList <coord> ledOn;
QueueList <coord> ledOff;

// returns the index position of the i-th snake element
// head is at position 0
byte snakeIndAt(int i) {
  while (i < 0) {
    i = i + maxSnakeN;
  }
  i = (snakeHeadInd + i) % maxSnakeN;
  return i;
}

byte coordsLoop(int x) {
  while (x < 0) {
    x = x + squareN;
  }
  x = x % squareN;
  if (x == 0) {
    x = squareN;  
  }
  return (byte)x;
}

float coordsLoopF(float x) {
  while (round(x) < 0) {
    x = x + squareN;
  }
  int res = (int)x % squareN;
  if (res == 0) {
    res = squareN;
  }
  return ((int)x - x) + res;
}

float distLoop(float a, float b) {
  float fwd = 0;
  float bwd = 0;
  if (b>=a) {
    fwd = b - a;
    bwd = b - squareN - a;
  } else {
    fwd = squareN - a + b;
    bwd = - (a - b);
  }
  if (abs(fwd) <= abs(bwd)) {
    return fwd;
  } else {
    return bwd;
  }
}

void nextLoop() {
  while (!ledOn.isEmpty()) {
    ledOn.pop();
  }
  while (!ledOff.isEmpty()) {
    ledOff.pop();
  }
}

void snakeStart(byte x, byte y, byte len) {
  lc.clearDisplay(0); 
  snakeHeadInd = 0;
  snakeLength = len;
  gameOver = false;
  for (int i = 0; i < maxSnakeN; i++) {
    snake[i] = nacoord;
  }
  for (int i = 0; i < len; i++) {
    coord snakePoint = {coordsLoop(x + i), coordsLoop(y)};
    snake[snakeIndAt(i)] = snakePoint;
    ledOn.push(snakePoint);
  }
}

boolean moveSnakeTo(float x, float y) {
  x = coordsLoopF(x);
  y = coordsLoopF(y);
//  Serial.print("Goal: ");
//  Serial.print(x);
//  Serial.print("'");
//  Serial.print(y);
//  Serial.print(" Head: ");
//  Serial.print(snake[snakeHeadInd].x);
//  Serial.print("'");
//  Serial.println(snake[snakeHeadInd].y);
  float stepsizex = distLoop(snake[snakeHeadInd].x, x);
  byte goalx = coordsLoop(snake[snakeHeadInd].x + max(min(round(stepsizex), 1),-1));
  float stepsizey = distLoop(snake[snakeHeadInd].y, y);
  byte goaly = coordsLoop(snake[snakeHeadInd].y + max(min(round(stepsizey), 1),-1));
//  Serial.print("x steps:");
//  Serial.println(stepsizex);
//  Serial.print("y steps:"); 
//  Serial.println(stepsizey);
  if (goalx == snake[snakeHeadInd].x && goaly == snake[snakeHeadInd].y) {
    Serial.println("Stay at HEAD");
    return true;
  }
  if (abs(stepsizex) > abs(stepsizey)) {
    if (goalx != snake[snakeIndAt(1)].x) {
      goaly = snake[snakeHeadInd].y;
    } else {
      goalx = snake[snakeHeadInd].x;
    }
  } else {
    if (goaly != snake[snakeIndAt(1)].y) {
      goalx = snake[snakeHeadInd].x;
    } else {
      goaly = snake[snakeHeadInd].y;
    }
  }
  coord goal = {goalx, goaly};
  if (collisionWithSnake(goal)) {
    Serial.println("Game Over!");
    gameOver = true;
    return false;
  }
  byte newHeadInd = snakeIndAt(-1);
  snake[newHeadInd] = goal;
  ledOn.push(goal);
  byte snakeTailInd = snakeIndAt(snakeLength-1);
  ledOff.push(snake[snakeTailInd]);
  snake[snakeTailInd] = nacoord;
  snakeHeadInd = newHeadInd;
  return true;
}

boolean collisionWithSnake(coord point) {
  coord snakePoint;
  for(int i = 0; i < snakeLength; i++) {
    snakePoint = snake[snakeIndAt(i)];
    if (snakePoint.x == point.x && snakePoint.y == point.y) {
      return true;
    }
  }
  return false;
}

void writeDisplay() {
//  Serial.print("Snake (");
//  Serial.print(snakeHeadInd);
//  Serial.print("/");
//  Serial.print(snakeLength);
//  Serial.print(") :");
//  coord snakePoint;
//  for (int i = 0; i < snakeLength; i++) {
//    snakePoint = snake[snakeIndAt(i)];
//    Serial.print(snakePoint.x);
//    Serial.print(",");
//    Serial.print(snakePoint.y);
//    Serial.print("|");
//  }
//  Serial.println(";");
  coord point;
  while (!ledOff.isEmpty()) {
    point = ledOff.pop();
    lc.setLed(0, point.x - 1, point.y - 1, false);
  }
  while (!ledOn.isEmpty()) {
    point = ledOn.pop();
    lc.setLed(0, point.x - 1, point.y - 1, true);
  }
//  lc.clearDisplay(0); 
//  for (int i = 0; i < snakeLength; i++) {
//    point = snake[snakeIndAt(i)];
//    lc.setLed(0, point.x - 1, point.y - 1, true);
//  }
  nextLoop();
  
}

