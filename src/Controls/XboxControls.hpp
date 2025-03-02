#ifndef Xbox_Controls_hpp
#define Xbox_Controls_hpp

#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include "Controls.hpp"

class XboxControls : public Controls
{
private:
// Required to replace with your xbox address
//XboxSeriesXControllerESP32_asukiaaa::Core xboxController("40:8e:2c:b8:34:d5");
// any xbox controller
  XboxSeriesXControllerESP32_asukiaaa::Core xboxController;
//  XboxSeriesXHIDReportBuilder_asukiaaa::ReportBase *repo;
  void update();

public:
  XboxControls();
  bool is_firing();
  float get_thrust();
  float get_direction();
  void shake();
  int get_function_key();
};

#endif