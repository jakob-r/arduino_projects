//The snake moves from righy to left in this array
// - - - HEAD < < < TAIL - - -
// < TAIL - - - - - - HEAD < <
const byte maxSnailN = 64;
coord snake[maxSnailN];
const coord nacoord = {0, 0};
byte snakeHeadInd = 0;
byte snakeLength = 0;

const byte squareN = 8;

// which leds to turn on and of
#include <QueueArray.h>
QueueArray <coord> ledOn;
QueueArray <coord> ledOff;
boolean gameOver = false;

// returns the index position of the i-th snake element
// head is at position 0
byte snakeIndAt(int i) {
  i = (snakeHeadInd + i) % maxSnailN;
  return i;
}

byte coordsLoop(int x) {
  // 0 stays 0, but 9 is 1, -1 is 8  
  x = (x % squareN) + 1;
  return x;
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
  snakeHeadInd = 0;
  snakeLength = len;
  gameOver = false;
  memset(snake,0,sizeof(snake));
  for (int i = 0; i < len; i++) {
    x = coordsLoop(x + i);
    y = coordsLoop(y);
    coord snakePoint = {x, y};
    snake[snakeIndAt(i)] = snakePoint;
    ledOn.push(snakePoint);
  }
}

boolean moveSnakeTo(float x, float y) {
  // check if goal is next point
  coord goal = {round(x), round(y)};
  if ((goal.x == snake[snakeHeadInd].x && goal.y == snake[snakeHeadInd].y) || (goal.x == snake[snakeIndAt(1)].x && goal.y == snake[snakeIndAt(1)].y)) {
    return true;
  } else if (collisionWithSnake(goal)){
    return false;  
  } else {
    snakeHeadInd = snakeIndAt(-1);  
    snake[snakeHeadInd] = goal;
    ledOn.push(goal);
    byte snakeTailInd = snakeIndAt(snakeHeadInd + snakeLength);
    ledOff.push(snake[snakeTailInd]);
    snake[snakeTailInd] = nacoord;
    return true;
  }
}

boolean collisionWithSnake(coord point) {
  boolean collision = false;
  coord snakePoint;
  for(int i = 0; i < snakeLength; i++) {
    snakePoint = snake[snakeIndAt(i)];
    if (snakePoint.x == point.x && snakePoint.y == point.y) {
      return true;
    }
  }
}

