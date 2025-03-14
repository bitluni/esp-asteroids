#ifndef NunchuckControls_hpp
#define NunchuckControls_hpp

#include "Controls.hpp"

class NunchuckControls : public Controls
{
private:
  const unsigned char *data;

public:
  NunchuckControls();
  bool is_firing();
  bool is_thrusting();
  float get_direction();
};

#endif