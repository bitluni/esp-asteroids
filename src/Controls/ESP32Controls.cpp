#include "ESP32Controls.hpp"
#include "RotaryEncoder.hpp"
#include "Button.hpp"

ESP32Controls::ESP32Controls(RotaryEncoder *rotary_encoder, Button *fire_button, Button *thrust_button) : rotary_encoder(rotary_encoder), fire_button(fire_button), thrust_button(thrust_button)
{
}

bool ESP32Controls::is_firing()
{
  return true; //fire_button->get_button_pushed();
}

bool ESP32Controls::is_thrusting()
{
  return false; //thrust_button->get_button_pushed();
}

float ESP32Controls::get_direction()
{
  return rotary_encoder->get_angle();
}
