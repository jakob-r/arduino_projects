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

  public:
    void drive(short int speed, short unsigned int duration, short int drift) {

    }
};