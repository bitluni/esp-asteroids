#ifndef _dma_dac_renderer_h_
#define _dma_dac_renderer_h_

#include "Renderer.h"

class RenderBuffer;
class Font;

class DMADACRenderer : public Renderer
{
private:
  volatile int draw_position;
  void IRAM_ATTR draw();

public:
  DMADACRenderer(float world_size, Font *font);
  void start();
  friend void dma_dac_draw_timer(void *para);
};

#endif