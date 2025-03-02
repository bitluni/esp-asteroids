#ifndef _renderer_h_
#define _renderer_h_

#include "driver/gpio.h"
#include <vector>
#include "RenderBuffer.hpp"

#define PIN_NUM_LASER GPIO_NUM_32

//class Game;

class Renderer
{
protected:
  RenderBuffer *render_buffer;
  int mode=0;

public:
  Renderer();

  virtual void start() = 0;
  void IRAM_ATTR set_laser(bool on);
  void set_mode(int mode);
  RenderBuffer *get_render_buffer() { return render_buffer; }

  volatile int rendered_frames;
  volatile int transactions;
  volatile int delays;
};

#endif