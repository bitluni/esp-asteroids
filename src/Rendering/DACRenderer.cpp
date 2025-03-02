#ifndef ARDUINO_M5STACK_CORES3

#include <cstring>
#include "driver/timer.h"
#include "DACRenderer.h"
#include "driver/dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void IRAM_ATTR dac_draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  DACRenderer *renderer = static_cast<DACRenderer *>(para);
  renderer->draw();
}

void IRAM_ATTR DACRenderer::draw()
{
  static int hold = 0;
  // Clear the interrupt
  timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
  // After the alarm has been triggered we need enable it again, so it is triggered the next time
  timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
  // do the actual drawing
if (hold > 0)
{
  hold--;
}
else
{
  // do we still have things to draw?
  if (draw_position < render_buffer->display_frame->size())
  {
    const DrawInstruction_t &instruction = render_buffer->display_frame->at(draw_position);
    set_laser(instruction.laser);
    uint8_t output_x = instruction.x;
    uint8_t output_y = instruction.y;
    dac_output_voltage(DAC_CHANNEL_1, output_x);
    dac_output_voltage(DAC_CHANNEL_2, output_y);
    hold = instruction.hold;
    draw_position++;
    transactions++;
  }
  else
  {
    // trigger a re-render
    rendered_frames++;
    render_buffer->swapBuffers();
    draw_position = 0;
  }
}
  timer_spinlock_give(TIMER_GROUP_0);
}

void /*IRAM_ATTR*/ dac_timer_setup(void *param)
{
  // set up the renderer timer
  timer_config_t config = {
      .alarm_en = TIMER_ALARM_EN,
      .counter_en = TIMER_PAUSE,
      .intr_type = TIMER_INTR_LEVEL,
      .counter_dir = TIMER_COUNT_UP,
      .auto_reload = TIMER_AUTORELOAD_EN,
      .divider = 4000}; // default clock source is APB
  timer_init(TIMER_GROUP_0, TIMER_0, &config);

  // Timer's counter will initially start from value below.
  //   Also, if auto_reload is set, this value will be automatically reload on alarm
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 0x00000001ULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, dac_draw_timer,
                     param, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
  while (true)
  {
    vTaskDelay(10000000);
  }
}

void DACRenderer::start()
{
  draw_position = 0;
  // enable the DAC channels for X and Y
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);

  // make sure to start the task on CPU 1
  TaskHandle_t timer_setup_handle;
  xTaskCreatePinnedToCore(dac_timer_setup, "Draw Task", 4096, this, 0, &timer_setup_handle, 1);

  Renderer::start();
}

DACRenderer::DACRenderer(float world_size, Font *font)
{
  render_buffer = new RenderBuffer(
      0, 255,
      0, 255,
      128,
      128,
      128.0f / world_size,
      font);
}
#endif