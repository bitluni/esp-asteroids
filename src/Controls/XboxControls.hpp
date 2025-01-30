#ifndef Xbox_Controls_hpp
#define Xbox_Controls_hpp

#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "Controls.hpp"

//class XboxSeriesXControllerESP32_asukiaaa::Core;

class XboxControls : public Controls
{
private:
  XboxSeriesXControllerESP32_asukiaaa::Core *xboxController;

public:
  XboxControls(XboxSeriesXControllerESP32_asukiaaa::Core *xboxController);
  bool is_firing();
  float is_thrusting();
  float get_direction();
};

#endif