#pragma once

class I2SOutput;
class WAVFile;

class SoundFX
{
  WAVFile *bang_large_wav;
  WAVFile *bang_medium_wav;
  WAVFile *bang_small_wav;
  WAVFile *fire_wav;
  WAVFile *thrust_wav;
  I2SOutput *output;

public:
  SoundFX(I2SOutput *output);
#ifdef USE_XBOX_CONTROLLER
  void fire(float volume_adjust = 1.0f);
#else
  void fire(float volume_adjust = 0.0f);
#endif
  void thrust(float volume_adjust = 1.0f);
  void bang_large(float volume_adjust = 1.0f);
  void bang_medium(float volume_adjust = 1.0f);
  void bang_small(float volume_adjust = 1.0f);
};