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

#include "test-config.h"
#include "test-theme-loader.h"

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

class ColorControlsExample : public ConnectionTracker
{
public:

  ColorControlsExample(Application& application)
    : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &ColorControlsExample::Create);
  }

  ~ColorControlsExample() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ColorControlsExample::OnKeyEvent);

    Layout parent = Layout::New(); // Parent
    parent.SetBackgroundColor(UiColor::BACKGROUND);
    parent.SetRequestedWidth(200_spx);
    parent.SetRequestedHeight(200_spx);

    View redChild = View::New(); // Red child
    redChild.SetBackgroundColor(UiColor::PRIMARY);
    redChild.SetRequestedWidth(100_spx);
    redChild.SetRequestedHeight(100_spx);
    redChild.AsInteractive().ClickedSignal().Connect(this, [this](View view, InputEvent event) -> bool {
      mSecondChild.SetBackgroundColor(UiColor("ThemeColor1"));
      return true;
    });

    mSecondChild = View::New(); // Blue child
    mSecondChild.SetBackgroundColor(UiColor("ThemeColor2"));
    mSecondChild.SetRequestedWidth(100_spx);
    mSecondChild.SetRequestedHeight(100_spx);
    mSecondChild.SetRequestedPositionX(100_spx);
    mSecondChild.SetRequestedPositionY(100_spx);

    parent.AddChildren({redChild, mSecondChild});
    window.Add(parent);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if(event.GetKeyName() == "1")
      {
        if(auto* loader = GetTestThemeLoader())
        {
          loader->ToggleTheme();
        }
        return;
      }

      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
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
  Application application = Application::New(&argc, &argv);
  TestConfig::New().Apply();
  ColorControlsExample test(application);
  application.MainLoop();
  return 0;
}
