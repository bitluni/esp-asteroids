#include <cstring>
#include "driver/timer.h"
#include "DMADACRenderer.h"
//#include "driver/dac.h"
//#include "driver/dac_continuous.h"
//#include "driver/i2s_std.h"
#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint8_t wav[32]; // calloc()?

void IRAM_ATTR dma_dac_draw_timer(void *para)
{
  timer_spinlock_take(TIMER_GROUP_0);
  DMADACRenderer *renderer = static_cast<DMADACRenderer *>(para);
  renderer->draw();
}

int ramp=0;
uint8_t output_y_old;
uint8_t output_x_old;
volatile bool changed=false;
void IRAM_ATTR DMADACRenderer::draw()
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
    uint8_t output_y = instruction.x;
    uint8_t output_x = instruction.y;
    hold = instruction.hold;
    if(output_x != output_x_old || output_y != output_y_old) {
      output_x_old=output_x;
      output_y_old=output_y;
//    output_x=output_y=ramp++%256;
/*      wav[0]=0;
      wav[1]=output_x;
      wav[2]=0;
      wav[3]=output_y;
      wav[4]=0;
      wav[5]=output_x;
      wav[6]=0;
      wav[7]=output_y;
*/for(uint8_t i=0; i<sizeof(wav); i+=2 ) {
  wav[i  ]=output_x;
  wav[i+1]=output_y;
}
      changed=true;
    }
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

void /*IRAM_ATTR*/ dma_dac_timer_setup(void *param)
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
  timer_isr_register(TIMER_GROUP_0, TIMER_0, dma_dac_draw_timer,
                     param, ESP_INTR_FLAG_IRAM, NULL);

  timer_start(TIMER_GROUP_0, TIMER_0);
  while (true)
  {

     if(changed) {
      changed=false;
      size_t bytes_written;
/*      ESP_ERROR_CHECK(i2s_write(I2S_NUM_0, wav, sizeof(wav), &bytes_written, portMAX_DELAY));
      ESP_ERROR_CHECK(i2s_write(I2S_NUM_0, wav, sizeof(wav), &bytes_written, portMAX_DELAY));*/
      ESP_ERROR_CHECK(i2s_write_expand(I2S_NUM_0, wav, sizeof(wav), I2S_BITS_PER_SAMPLE_8BIT, I2S_BITS_PER_SAMPLE_16BIT, &bytes_written, portMAX_DELAY));
     }
    vTaskDelay(1/*0000000*/);
  }
}

void DMADACRenderer::start()
{
  draw_position = 0;
  // setup I²S output
/*  i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
		.sample_rate = 11025,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,		
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,	
//		.communication_format = I2S_COMM_FORMAT_PCM,
		.communication_format = I2S_COMM_FORMAT_STAND_PCM_SHORT,
		.intr_alloc_flags = 0,
		.dma_buf_count = 2,												
		.dma_buf_len = 8,											
		.use_apll = false,
    .tx_desc_auto_clear = false,
    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
	};*/
static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 44100,
    .bits_per_sample = /*16*/I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 2,
    .dma_buf_len = 8,
    .use_apll = false,
    .tx_desc_auto_clear = false
};

     //install and start i2s driver
     ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
     ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, NULL)); //for internal DAC, this will enable both of the internal channels
     //init DAC pad
     ESP_ERROR_CHECK(i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN));
     //ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, 11025, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO));
//     ESP_ERROR_CHECK(i2s_set_sample_rates(I2S_NUM_0, /*22050*/768000)); //set sample rates
     //ESP_ERROR_CHECK(i2s_channel_preload_data());
      wav[0]=0;
      wav[1]=63;//l
      wav[2]=1;
      wav[3]=127;//r
      wav[4]=2;
      wav[5]=195;
      wav[6]=3;
      wav[7]=255;
      wav[8]=4;
      wav[9]=63;//l
      wav[10]=5;
      wav[11]=127;//r
      wav[12]=6;
      wav[13]=195;
      wav[14]=7;
      wav[15]=255;
      wav[16]=8;
      wav[17]=63;//l
      wav[18]=9;
      wav[19]=127;//r
      wav[20]=10;
      wav[21]=195;
      wav[22]=11;
      wav[23]=255;
      wav[24]=12;
      wav[25]=63;//l
      wav[26]=13;
      wav[27]=127;//r
      wav[28]=14;
      wav[29]=195;
      wav[30]=15;
      wav[31]=255;
      size_t bytes_written;
      //ESP_ERROR_CHECK(i2s_write(I2S_NUM_0, wav, sizeof(wav), &bytes_written, portMAX_DELAY));
      ESP_ERROR_CHECK(i2s_write_expand(I2S_NUM_0, wav, sizeof(wav), I2S_BITS_PER_SAMPLE_8BIT, I2S_BITS_PER_SAMPLE_16BIT, &bytes_written, portMAX_DELAY));



  // make sure to start the task on CPU 1
  TaskHandle_t timer_setup_handle;
  xTaskCreatePinnedToCore(dma_dac_timer_setup, "Draw Task", 4096, this, 0, &timer_setup_handle, 1);

  Renderer::start();
}

DMADACRenderer::DMADACRenderer(float world_size, Font *font)
{
  render_buffer = new RenderBuffer(
      0, 255,
      0, 255,
      128,
      128,
      128.0f / world_size,
      font);
}
