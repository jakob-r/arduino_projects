// Rotary Encoder

class RotaryEncoder {
  bool state_both_on = false;
  bool state_upwards = false;
  bool state_downwards = false;
  bool state_button = true;

  // Rotary Encoder
  uint8_t m_pin_A; // cll pin
  uint8_t m_pin_B; // dt pin
  uint8_t m_pin_button; // sw pin
  
  
  public:
    uint8_t button_count = 0;
    int16_t rotary_position = 0;

    RotaryEncoder(uint8_t pin_A, uint8_t pin_B, uint8_t pin_button) {
      m_pin_A = pin_A;
      m_pin_B = pin_B;
      m_pin_button = pin_button;
    }
    

    void setup() {
      pinMode(m_pin_A, INPUT);
      pinMode(m_pin_B, INPUT);
      pinMode(m_pin_button, INPUT_PULLUP);
    }

    void encoderISR() {
      bool pin_A_val = digitalRead(m_pin_A);
      bool pin_B_val = digitalRead(m_pin_B);

      if (pin_A_val == pin_B_val) { // steady position
        state_both_on = pin_A_val;
        state_upwards = state_downwards = false;
      }
      if (!state_both_on) { // if both pins were off
        if (pin_A_val == true && pin_B_val == false && !state_upwards) {
          rotary_position++;
          state_upwards = true;
          state_downwards = false;
        }
        if (pin_A_val == false && pin_B_val == true && !state_downwards) {
          rotary_position--;
          state_upwards = false;
          state_downwards = true;
        }
      } else { // if poth pins were on
        if (pin_A_val == false && pin_B_val == true && !state_upwards) {
          rotary_position++;
          state_upwards = true;
          state_downwards = false;
        }
        if (pin_A_val == true && pin_B_val == false && !state_downwards) {
          rotary_position--;
          state_upwards = false;
          state_downwards = true;
        }
      }
    }

    bool button_pressed() { // 0 is button pushed
      bool pin_button_val = digitalRead(m_pin_button);
      if (state_button != pin_button_val) {
        state_button = pin_button_val;
        if (state_button == 0) {
          return(true);
        }
      }
      return(false);
    }
  
};