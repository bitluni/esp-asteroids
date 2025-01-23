#include "MyButton.hpp"

MyButton::MyButton(gpio_num_t button_pin)
{
  _button_pin = button_pin;
  gpio_set_direction(button_pin, GPIO_MODE_INPUT);
  gpio_set_pull_mode(button_pin, GPIO_PULLUP_ONLY);
}

bool MyButton::get_button_pushed()
{
  return gpio_get_level(_button_pin) == 0;
}
