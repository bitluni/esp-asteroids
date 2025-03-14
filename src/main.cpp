#include <stdio.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "WiFi.h"
#include "Rendering/DACRenderer.h"
#include "Rendering/HeltecOLEDRenderer.hpp"
#include "Rendering/SPIRenderer.h"
#include "Game/GameLoop.h"
#include "Game/Game.hpp"
#include "Controls/MechanicalRotaryEncoder.hpp"
// #include "Controls/MagneticRotaryEncoder.hpp"
#include "Controls/Button.hpp"
#include "Controls/ESP32Controls.hpp"
#include "Controls/NunchuckControls.hpp"
#include "Audio/I2SOutput.h"
#include "Audio/WAVFile.h"
#include "Audio/SoundFX.h"
// #include "Fonts/HersheyFont.hpp"
#include "Fonts/SimpleFont.hpp"

#define WORLD_SIZE 30
#define ROTARY_ENCODER_CLK_GPIO GPIO_NUM_19
#define ROTARY_ENCODER_DI_GPIO GPIO_NUM_18

#define MAGNETIC_ROTARY_ENCODER_CS_GPIO GPIO_NUM_2
#define MAGNETIC_ROTARY_ENCODER_CLK_GPIO GPIO_NUM_13
#define MAGNETIC_ROTARY_ENCODER_MISO_GPIO GPIO_NUM_12
#define MAGNETIC_ROTARY_ENCODER_MOSI_GPIO GPIO_NUM_14

// WARNING - this pin conflicts with the HELTEC OLED displat
// if using the heltec renderer switch to another pin.
#define FIRE_GPIO GPIO_NUM_15
#define THRUST_GPIO GPIO_NUM_4

extern "C"
{
  void app_main(void);
}

static const char *TAG = "APP";

void app_main()
{
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  ESP_LOGI(TAG, "Started UP");

  esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true};

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    }
    else
    {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
  }

  uint32_t free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram before setup %d", free_ram);

  ESP_LOGI(TAG, "Starting audio");
  I2SOutput *i2sOutput = new I2SOutput();
  i2sOutput->start();

  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after I2SOutput %d", free_ram);

  SoundFX *sound_fx = new SoundFX(i2sOutput);
  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after SoundFX %d", free_ram);

  ESP_LOGI(TAG, "Creating controls");
  // MagneticRotaryEncoder *rotary_encoder = new MagneticRotaryEncoder(
  //     MAGNETIC_ROTARY_ENCODER_CS_GPIO,
  //     MAGNETIC_ROTARY_ENCODER_CLK_GPIO,
  //     MAGNETIC_ROTARY_ENCODER_MISO_GPIO,
  //     MAGNETIC_ROTARY_ENCODER_MOSI_GPIO);
  //RotaryEncoder *rotary_encoder = new MechanicalRotaryEncoder(ROTARY_ENCODER_CLK_GPIO, ROTARY_ENCODER_DI_GPIO);

  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after RotaryEncoder %d", free_ram);

  //Button *fire_button = new Button(FIRE_GPIO);
  //Button *thrust_button = new Button(THRUST_GPIO);

  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after Button %d", free_ram);

  //ESP32Controls *controls = new ESP32Controls(rotary_encoder, fire_button, thrust_button);
  NunchuckControls *controls = new NunchuckControls();
  ESP_LOGI(TAG, "Starting world");
  Game *game = new Game(WORLD_SIZE, controls, sound_fx);

  ESP_LOGI(TAG, "Loading font");
  //HersheyFont *font = new HersheyFont();
  //font->read_from_file("/spiffs/futural.jhf");
  SimpleFont *font = new SimpleFont();

  ESP_LOGI(TAG, "Starting renderer");
  Renderer *renderer = new DACRenderer(WORLD_SIZE, font);
  // Renderer *renderer = new HeltecOLEDRenderer(WORLD_SIZE, font);
  // Renderer *renderer = new SPIRenderer(WORLD_SIZE, font);
  renderer->start();

  GameLoop *game_loop = new GameLoop(game, renderer->get_render_buffer());
  game_loop->start();

  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after world %d", free_ram);

  free_ram = esp_get_free_heap_size();
  ESP_LOGI(TAG, "Free ram after renderer %d", free_ram);

  float start = esp_timer_get_time() / 1E6f;
  while (true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    float end = esp_timer_get_time() / 1E6f;
    ESP_LOGI(TAG, "World steps %d, FPS %.0f, Transactions %d, Free RAM %d",
             game_loop->steps,
             renderer->rendered_frames / (end - start),
             renderer->transactions,
             esp_get_free_heap_size());
  }
}
