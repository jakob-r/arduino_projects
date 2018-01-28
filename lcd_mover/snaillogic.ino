//The snake moves from righy to left in this array
// - - - HEAD < < < TAIL - - -
// < TAIL - - - - - - HEAD < <
const byte maxSnakeN = 64;
coord snake[maxSnakeN];
const coord nacoord = {0, 0};
byte snakeHeadInd = 0;
byte snakeLength = 0;

coord powerDot; 

const byte squareN = 8;

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
  int res = round(x) % squareN;
  if (res == 0) {
    res = squareN;
  }
  return (round(x) - x) + (float)res;
}

float distLoop(float a, float b) {
  float fwd = 0;
  float bwd = 0;
  if (b>=a) {
    fwd = b - a;
    bwd = b - (float)squareN - a;
  } else {
    fwd = (float)squareN - a + b;
    bwd = - (a - b);
  }
  if (abs(fwd) <= abs(bwd)) {
    return fwd;
  } else {
    return bwd;
  }
}

void newPowerDot() {
  powerDot = {random(1,8), random(1,8)};
  if (collisionWithSnake(powerDot) != 254) {
    newPowerDot();
  }
  powerDotWorth = powerDotStartWorth;
  ledOn.push(powerDot);
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

/* Returns
 * 0 Collision
 * 1 normal move
 * 2 stay because of backwards collision
 * 3 stay because xy do not change enough
 * 4 hit power dot
 */
byte moveSnakeTo(float x, float y) {
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
  float stepsizey = distLoop(snake[snakeHeadInd].y, y);
  if (abs(stepsizex) < 0.5 && abs(stepsizey) < 0.5) {
    // Move not important
    return 3;
  }
  coord goalx = {coordsLoop(snake[snakeHeadInd].x + max(min(round(stepsizex), 1),-1)), snake[snakeHeadInd].y};
  coord goaly = {snake[snakeHeadInd].x, coordsLoop(snake[snakeHeadInd].y + max(min(round(stepsizey), 1),-1))};
//  Serial.print("x steps:");
//  Serial.println(stepsizex);
//  Serial.print("y steps:"); 
//  Serial.println(stepsizey);
  byte collisionx = collisionWithSnake(goalx);
  byte collisiony = collisionWithSnake(goaly);
  byte newHeadInd = snakeIndAt(-1);

  // Where to move the snake?
  if (collisionx < 2 && collisiony < 2) {
    // We do collide with the HEAD or the part behind the HEAD so we stay
    return 2;
  } else if (collisionx < snakeLength && collisiony < snakeLength) {
    // We collide with another part of the Snake
    return 0;
  } else if ((abs(stepsizex) > abs(stepsizey) && collisionx >= snakeLength) || collisiony <= snakeLength) {
    // We want to move x and x is free, or y was blocked
    snake[newHeadInd] = goalx;
  } else {
    // We can only move y and y should be free
    snake[newHeadInd] = goaly;
  }
  ledOn.push(snake[newHeadInd]);
  // check if we collected a powerDot
  if (snake[newHeadInd].x == powerDot.x && snake[newHeadInd].y == powerDot.y) {
    snakeLength++;
    snakeHeadInd = newHeadInd;
    return 4;
  } else {
    byte snakeTailInd = snakeIndAt(snakeLength-1);
    ledOff.push(snake[snakeTailInd]);
    snake[snakeTailInd] = nacoord;
    snakeHeadInd = newHeadInd;
    return 1;
  }
}

// returns where we collide with snake
// 254 means no collision
// 0 means head...
byte collisionWithSnake(coord point) {
  for(int i = 0; i < snakeLength; i++) {
    if (snake[snakeIndAt(i)].x == point.x && snake[snakeIndAt(i)].y == point.y) {
      return i;
    }
  }
  return 254;
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
}

