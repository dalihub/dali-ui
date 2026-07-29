/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Verifies the Windows backend pointer changes:
//   1. DevelWindow::PointerWarp() now moves the OS cursor      (press 'w')
//   2. Mouse points are tagged with Device::Class::MOUSE       (move/click mouse)
// The results are printed to the console so the behaviour can be checked
// without any GUI automation.

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>

#include <iostream>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
const char* DeviceClassName(Device::Class::Type deviceClass)
{
  switch(deviceClass)
  {
    case Device::Class::USER:     return "USER";
    case Device::Class::KEYBOARD: return "KEYBOARD";
    case Device::Class::MOUSE:    return "MOUSE";
    case Device::Class::TOUCH:    return "TOUCH";
    case Device::Class::PEN:      return "PEN";
    case Device::Class::POINTER:  return "POINTER";
    case Device::Class::GAMEPAD:  return "GAMEPAD";
    case Device::Class::NONE:     // fall through
    default:                      return "NONE";
  }
}
} // unnamed namespace

class PointerDeviceController : public ConnectionTracker
{
public:
  explicit PointerDeviceController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &PointerDeviceController::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::WHITE);

    Layout root = Layout::New();
    root.SetBackgroundColor(UiColor(0x2277CC));
    root.SetRequestedWidth(300_spx);
    root.SetRequestedHeight(300_spx);
    mWindow.Add(root);

    mWindow.KeyEventSignal().Connect(this, &PointerDeviceController::OnKey);
    mWindow.TouchEventSignal().Connect(this, &PointerDeviceController::OnTouch);

    std::cout << "[pointer-device] ready: press 'w' to warp the pointer to client (200,200); "
                 "move or click the mouse to log its device class."
              << std::endl;
  }

  void OnKey(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    const Dali::String& keyName = event.GetKeyName();
    if(keyName == "w" || keyName == "W")
    {
      const bool warped = DevelWindow::PointerWarp(mWindow, 200, 200);
      std::cout << "[pointer-device] PointerWarp(200,200) -> " << (warped ? "true" : "false") << std::endl;
    }
  }

  void OnTouch(Window /*window*/, TouchEvent touch)
  {
    if(touch.GetPointCount() == 0u)
    {
      return;
    }

    std::cout << "[pointer-device] touch state=" << static_cast<int>(touch.GetState(0u))
              << " deviceClass=" << DeviceClassName(touch.GetDeviceClass(0u)) << std::endl;
  }

private:
  Application& mApplication;
  Window       mWindow;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application             application = Application::New(&argc, &argv);
  UiConfig                config      = UiConfig::New();
  config.Apply();
  PointerDeviceController controller(application);
  application.MainLoop();
  return 0;
}
