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
  void Create(Application application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    Layout parent = Layout::New();
    parent.SetBackgroundColor(UiColor(0xFFFF00));
    parent.SetRequestedWidth(200_spx);
    parent.SetRequestedHeight(200_spx);

    InteractiveView redChild = InteractiveView::New();
    redChild.SetBackgroundColor(UiColor(0xFF0000));
    redChild.SetRequestedWidth(100_spx);
    redChild.SetRequestedHeight(100_spx);
    redChild.ConnectClickedSignal(this, [this](View view, InputEvent event) -> bool {
      mSecondChild.SetBackgroundColor(UiColor(0x00FF00));
      return true;
    });

    mSecondChild = View::New();
    mSecondChild.SetBackgroundColor(UiColor(0x0000FF));
    mSecondChild.SetRequestedWidth(100_spx);
    mSecondChild.SetRequestedHeight(100_spx);
    mSecondChild.SetRequestedPositionX(100_spx);
    mSecondChild.SetRequestedPositionY(100_spx);

    parent.AddChildren({redChild, mSecondChild});
    window.Add(parent);
  }

private:
  Application& mApplication;
  View mSecondChild;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.Apply();
  HelloWorldController test(application);
  application.MainLoop();
  return 0;
}
