#include "ESP32Controls.hpp"
#include "RotaryEncoder.hpp"
#include "MyButton.hpp"

ESP32Controls::ESP32Controls(RotaryEncoder *rotary_encoder, MyButton *fire_button, MyButton *thrust_button) : rotary_encoder(rotary_encoder), fire_button(fire_button), thrust_button(thrust_button)
{
}

bool ESP32Controls::is_firing()
{
//  return fire_button->get_button_pushed();
  return true;
//  return false;
}

bool ESP32Controls::is_thrusting()
{
//  return thrust_button->get_button_pushed();
  return false;
}

float ESP32Controls::get_direction()
{
  return rotary_encoder->get_angle();
}
