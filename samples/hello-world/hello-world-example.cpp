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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layout.h>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

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

    window.Add(Layout::New() // Parent
      .BackgroundColor(Color::YELLOW)
      .SetSizeWidth(200_spx)
      .SetSizeHeight(200_spx)
      .Contents({
        View::New() // Red child
          .BackgroundColor(Color::RED)
          .SetSizeWidth(100_spx)
          .SetSizeHeight(100_spx)
          .AsClickable(this, [this](View view, const InputEvent& event)
          {
            mSecondChild.SetBackgroundColor(Color::GREEN);
            return true;
          }),
        View::New() // Blue child
          .BackgroundColor(Color::BLUE)
          .SetSizeWidth(100_spx)
          .SetSizeHeight(100_spx)
          .SetPositionX(100_spx)
          .SetPositionY(100_spx)
          .As(mSecondChild),
      }));
  }

private:
  Application& mApplication;
  View mSecondChild;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  HelloWorldController test(application);
  application.MainLoop();
  return 0;
}
