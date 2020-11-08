// Car Control

class CarControl {

  //right wheels
  const int8_t rw_sw = A0;
  const int8_t rw_fwd = A1;
  const int8_t rw_rwd = A2;
  //left wheels
  const int8_t lw_fwd = A3;
  const int8_t lw_rwd = A4;
  const int8_t lw_sw = A5;
  //avoidance 
  const int8_t av_bl = 8; //bottom
  const int8_t av_br = 9;
  const int8_t av_fl = 10; //front
  const int8_t av_fr = 11;

  uint8_t pwm_speed = 0;
  uint8_t pwm_count = 0;
  uint16_t duration_max = 0;
  uint16_t duration_count = 0;


  void forward() { // sets forward "gear"
    digitalWrite(rw_fwd, HIGH);
    digitalWrite(rw_rwd, LOW);
    
    digitalWrite(lw_fwd, HIGH);
    digitalWrite(lw_rwd, LOW);
  }

  void backward() { // sets backwards "gear"
    digitalWrite(rw_fwd, LOW);
    digitalWrite(rw_rwd, HIGH);
    
    digitalWrite(lw_fwd, LOW);
    digitalWrite(lw_rwd, HIGH);
  }

  void halt() { // turns motors off 
    digitalWrite(rw_sw, LOW);
    digitalWrite(lw_sw, LOW);
  }

  void move() { // turns motors on
    digitalWrite(rw_sw, HIGH);
    digitalWrite(lw_sw, HIGH);
  }

  public:
    boolean driving = false;
    boolean stopped = false;

    void setup() {
      pinMode(rw_sw, OUTPUT);
      pinMode(rw_fwd, OUTPUT);
      pinMode(rw_rwd, OUTPUT);
      pinMode(lw_fwd, OUTPUT);
      pinMode(lw_rwd, OUTPUT);
      pinMode(lw_sw, OUTPUT);
      pinMode(av_bl, INPUT_PULLUP);
      pinMode(av_br, INPUT_PULLUP);
      pinMode(av_fl, INPUT_PULLUP);
      pinMode(av_fr, INPUT_PULLUP);
    }

    void interrupt() {
      if (driving) {
        if (pwm_count <= pwm_speed) {
          move();
        } else {
          halt();
        }
      } else {
        halt();
      }
      pwm_count++;
      if (pwm_count == 10) {
        pwm_count = 0;
        duration_count++;
        if (duration_count == duration_max) {
          driving = false;
          stopped = true;
        }
      }
    }

    void drive(int16_t speed, uint16_t duration, int8_t drift) {
      if (speed > 0) {
        forward();
      } else {
        backward();
      }
      pwm_speed = 7;
      duration_max = 20;
      driving = true;
      stopped = false;
    }
};