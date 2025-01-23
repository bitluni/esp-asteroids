#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <driver/gpio.h>

class MyButton
{
private:
  gpio_num_t _button_pin;

public:
  MyButton(gpio_num_t button_pin);
  bool get_button_pushed();
};

#endif