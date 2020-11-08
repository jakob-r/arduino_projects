// Car Control

class CarControl {

  //right wheels
  const int rw_sw = A0;
  const int rw_fwd = A1;
  const int rw_rwd = A2;
  //left wheels
  const int lw_fwd = A3;
  const int lw_rwd = A4;
  const int lw_sw = A5;
  //avoidance 
  const int av_bl = 8; //bottom
  const int av_br = 9;
  const int av_fl = 10; //front
  const int av_fr = 11;

  void forward() {
    digitalWrite(rw_sw, HIGH);
    digitalWrite(rw_fwd, HIGH);
    digitalWrite(rw_rwd, LOW);
    
    digitalWrite(lw_sw, HIGH);
    digitalWrite(lw_fwd, HIGH);
    digitalWrite(lw_rwd, LOW);
  }

  void halt() {
    digitalWrite(rw_sw, LOW);
    digitalWrite(lw_sw, LOW);
  }

  public:
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

    void drive(short int speed, short unsigned int duration, short int drift) {
      forward();
      delay(duration * 100);
      halt();
    }
};