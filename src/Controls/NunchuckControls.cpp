//original source https://github.com/moefh/esp32-wii-nunchuk


#include "NunchuckControls.hpp"
#include <math.h>

#define PIN_SDA  21
#define PIN_SCL  22
#define WII_I2C_PORT 0

#include "wii_i2c.h"

NunchuckControls::NunchuckControls()
{
	if (wii_i2c_init(WII_I2C_PORT, PIN_SDA, PIN_SCL) != 0) {
	    //Serial.printf("ERROR initializing wii i2c controller\n");
    return;
  }
  const unsigned char *ident = wii_i2c_read_ident();
  if (! ident) 
  {
    return;
  }
  int controller_type = wii_i2c_decode_ident(ident);
  wii_i2c_request_state();
}

bool NunchuckControls::is_firing()
{
	data = wii_i2c_read_state();
	wii_i2c_request_state();

	wii_i2c_nunchuk_state state;
  	wii_i2c_decode_nunchuk(data, &state);

  	return state.z;
}

bool NunchuckControls::is_thrusting()
{
	wii_i2c_nunchuk_state state;
  	wii_i2c_decode_nunchuk(data, &state);
  	return sqrt(state.y * state.y + state.x * state.x) > 80;
}

float NunchuckControls::get_direction()
{
	wii_i2c_nunchuk_state state;
  	wii_i2c_decode_nunchuk(data, &state);
	static float angle = 0;
	if(abs(state.x) > 10 || abs(state.y) > 10) 
		angle = atan2(state.x, state.y);
  	return angle;
}
/*
unsigned int controller_type = 0;

void show_nunchuk(const unsigned char *data)
{
  wii_i2c_nunchuk_state state;
  wii_i2c_decode_nunchuk(data, &state);
        
  Serial.printf("a = (%5d,%5d,%5d)\n", state.acc_x, state.acc_y, state.acc_z);
  Serial.printf("d = (%5d,%5d)\n", state.x, state.y);
  Serial.printf("c=%d, z=%d\n", state.c, state.z);
}
*/