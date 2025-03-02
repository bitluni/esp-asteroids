#include "XboxControls.hpp"

static const char *TAG = "XboxControls";

XboxControls::XboxControls()
{
ESP_LOGI(TAG, "Starting NimBLE Client");
xboxController.begin();
while(!(xboxController.isConnected())) {
//while(xboxController.isWaitingForFirstNotification()) {

  xboxController.onLoop();
  if (xboxController.isConnected()) {
    if (xboxController.isWaitingForFirstNotification()) {
      ESP_LOGI(TAG, "waiting for first notification");
    } else {
      /*Serial.println("Address: " + xboxController.buildDeviceAddressStr());
      Serial.print(xboxController.xboxNotif.toString());
      unsigned long receivedAt = xboxController.getReceiveNotificationAt();
      uint16_t joystickMax = XboxControllerNotificationParser::maxJoy;
      Serial.print("joyLHori rate: ");
      Serial.println((float)xboxController.xboxNotif.joyLHori / joystickMax);
      Serial.print("joyLVert rate: ");
      Serial.println((float)xboxController.xboxNotif.joyLVert / joystickMax);
      Serial.println("battery " + String(xboxController.battery) + "%");
      Serial.println("received at " + String(receivedAt));*/
    }
  } else {
    ESP_LOGI(TAG, "not connected");
    if (xboxController.getCountFailedConnection() > 2) {
      ESP.restart();
    }
  }

  vTaskDelay(500 / portTICK_PERIOD_MS);
}
}

void XboxControls::update() {
  xboxController.onLoop();
}

bool XboxControls::is_firing()
{
  update();
//  Serial.print(xboxController->xboxNotif.btnA);
  return xboxController.xboxNotif.btnA;
}

float XboxControls::get_thrust()
{
  update();
  return (float)xboxController.xboxNotif.trigRT / XboxControllerNotificationParser::maxTrig;
}

float XboxControls::get_direction()
{
  update();
  float x=(float)xboxController.xboxNotif.joyLHori / XboxControllerNotificationParser::maxJoy*2.0f-1.0f;
  float y=(float)xboxController.xboxNotif.joyLVert / XboxControllerNotificationParser::maxJoy*2.0f-1.0f;
//  ESP_LOGI(TAG, "%f,%f", x, y);
// https://chatgpt.com/share/67999632-7a44-8002-9df6-3d2d1958b8b9
    float angleRad = atan2(x, -y);  // Get angle in radians
    return angleRad;
}

void XboxControls::shake()
{
  //ESP_LOGI(TAG, "shake()");
  update();
  XboxSeriesXHIDReportBuilder_asukiaaa::ReportBase repo;
  repo.setAllOff();
  repo.v.select.shake = true;
  repo.v.power.shake = 30;
  repo.v.timeActive = 50;    // 0.5 second
  xboxController.writeHIDReport(repo);
}

int XboxControls::get_function_key() {
  //ESP_LOGI(TAG, "get_function_key()");
  if(xboxController.xboxNotif.btnXbox) return 1;
  else if(xboxController.xboxNotif.btnSelect) return 2;
  else if(xboxController.xboxNotif.btnStart) return 3;
  else if(xboxController.xboxNotif.btnRB) return 4;
  else return 0;
}