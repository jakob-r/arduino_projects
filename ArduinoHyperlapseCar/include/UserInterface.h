// User Interface

class UserInterface {
  short int current_value;
  bool current_row; //because it can just be 0 or 1
  short unsigned int current_col;
  bool taking_values = false;
//  LiquidCrystal m_lcd;

  public:
//    UserInterface(LiquidCrystal lcd) {
//      m_lcd = lcd;
//    }
    
    void setup(LiquidCrystal lcd) {
      lcd.clear();
    }

    void set_value(LiquidCrystal lcd, int value) {
      if (taking_values && value != current_value) {
        lcd.setCursor(current_col, current_row);
        lcd.print(F("    "));
        lcd.setCursor(current_col, current_row);
        lcd.print(value);
        current_value = value;
      }
    }

    void show_selection_speed(LiquidCrystal lcd) {
      lcd.print(F("Select speed:"));
      current_row = 1;
      current_col = 0;
      taking_values = true;
    }

    void show_selection_drift(LiquidCrystal lcd) {

    }

    void show_selection_duration(LiquidCrystal lcd) {

    }

    void show_running(LiquidCrystal lcd) {
      lcd.clear();
      lcd.print(F("Running!"));
      taking_values = false;
    }
};