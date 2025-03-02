#ifndef ESP32_Controls_hpp
#define ESP32_Controls_hpp

#include "Controls.hpp"

class RotaryEncoder;
class MyButton;

class ESP32Controls : public Controls
{
private:
  RotaryEncoder *rotary_encoder;
  MyButton *fire_button;
  MyButton *thrust_button;

public:
  ESP32Controls(RotaryEncoder *rotary_encoder, MyButton *fire_button, MyButton *thrust_button);
  bool is_firing();
  float get_thrust();
  float get_direction();
};

#endif