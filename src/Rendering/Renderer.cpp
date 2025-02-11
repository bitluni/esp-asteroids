#include "Renderer.h"
#include "../Game/Game.hpp"
// for APCD-520-02-C3
#define PWM
#ifdef PWM
#include "driver/ledc.h"
#define LEDC_DUTY               (  0)
//#define LEDC_DUTY               (127) // fuzzy (?)
//#define LEDC_DUTY               (13) // fuzzy (?)
#define LEDC_DUTY_OFF           (255) // Set duty to 100%. (2 ** 8 = 256) * 100% = 256 - 1 for PWM
//#define LEDC_DUTY_OFF           (15) // Set duty to 100%. (2 ** 4 = 16) * 100% = 16 - 1 for PWM
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#endif

Renderer::Renderer() : render_buffer(NULL)
{
  rendered_frames = 0;
  transactions = 0;
  delays = 0;
}

void Renderer::start()
{
#ifndef PWM
  // setup the laser output
  gpio_set_direction(PIN_NUM_LASER, GPIO_MODE_OUTPUT);
#else
  // Prepare and then apply the LEDC PWM timer configuration
  ledc_timer_config_t ledc_timer = {
      .speed_mode       = LEDC_MODE,
      .duty_resolution  = LEDC_TIMER_8_BIT,
//      .duty_resolution  = LEDC_TIMER_4_BIT,
      .timer_num        = LEDC_TIMER,
      .freq_hz          = (100*1000),
//      .freq_hz          = (1000*1000),
      .clk_cfg          = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Prepare and then apply the LEDC PWM channel configuration
  ledc_channel_config_t ledc_channel = {
      .gpio_num       = PIN_NUM_LASER,
      .speed_mode     = LEDC_MODE,
      .channel        = LEDC_CHANNEL,
      .intr_type      = LEDC_INTR_DISABLE,
      .timer_sel      = LEDC_TIMER,
      .duty           = 0,
      .hpoint         = 0
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
#endif
}

void IRAM_ATTR Renderer::set_laser(bool on)
{
#ifndef PWM
  gpio_set_level(PIN_NUM_LASER, on ? 1 : 0);
#else
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, on ? LEDC_DUTY : LEDC_DUTY_OFF));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
#endif
}
