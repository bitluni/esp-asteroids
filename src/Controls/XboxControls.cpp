#include "XboxControls.hpp"
#include "RotaryEncoder.hpp"
#include "MyButton.hpp"

#include <cmath>

static const char *TAG = "APP";

XboxControls::XboxControls(XboxSeriesXControllerESP32_asukiaaa::Core *xboxController) : xboxController(xboxController)
{
}

bool XboxControls::is_firing()
{
//  Serial.print(xboxController->xboxNotif.btnA);
  return xboxController->xboxNotif.btnA;
}

float XboxControls::get_thrust()
{
  return (float)xboxController->xboxNotif.trigRT / XboxControllerNotificationParser::maxTrig;
}

float XboxControls::get_direction()
{
float x=(float)xboxController->xboxNotif.joyLHori / XboxControllerNotificationParser::maxJoy*2.0f-1.0f;
float y=(float)xboxController->xboxNotif.joyLVert / XboxControllerNotificationParser::maxJoy*2.0f-1.0f;
//  ESP_LOGI(TAG, "%f,%f", x, y);
// https://chatgpt.com/share/67999632-7a44-8002-9df6-3d2d1958b8b9
    float angleRad = std::atan2(x, -y);  // Get angle in radians
    return angleRad;
}
