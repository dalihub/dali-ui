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

#include <dali-ui-foundations/dali-ui-foundations.h>

using namespace Dali;
using namespace Dali::UI;
using Dali::UI::View;

// This example shows how to create and display Hello World! using a simple TextActor
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController(Application& application)
  : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &HelloWorldController::Create);
  }

  ~HelloWorldController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    window.Add(
      View::New() // Parent
        .BackgroundColor(Color::YELLOW)
        .SizeWidth(200_spx)
        .SizeHeight(200_spx)
        .Contents({
          View::New() // Red child
            .BackgroundColor(Color::RED)
            .SizeWidth(100_spx)
            .SizeHeight(100_spx)
            .With([this](View& firstChild)
            {
              firstChild.TouchedSignal().Connect(this, &HelloWorldController::OnTouchRed);
            }),
          View::New() // Blue child
            .BackgroundColor(Color::BLUE)
            .SizeWidth(100_spx)
            .SizeHeight(100_spx)
            .PositionX(100_spx)
            .PositionY(100_spx)
            .As(mSecondChild),
        })
    );

    // Respond to key events
    window.KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);
  }

  bool OnTouchRed(Actor actor, const TouchEvent& touch)
  {
    // quit the application
    mSecondChild.BackgroundColor(Color::GREEN);
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
  View mSecondChild;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  HelloWorldController test(application);
  application.MainLoop();
  return 0;
}
