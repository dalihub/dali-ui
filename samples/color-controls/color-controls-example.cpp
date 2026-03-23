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
  void Create(Application& application)
  {
    // Get a handle to the window
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &ColorControlsExample::OnKeyEvent);

    window.Add(Layout::New() // Parent
      .SetBackgroundColor(UiColor::BACKGROUND)
      .SetSizeWidth(200_spx)
      .SetSizeHeight(200_spx)
      .Children({
        View::New() // Red child
          .SetBackgroundColor(UiColor::PRIMARY)
          .SetSizeWidth(100_spx)
          .SetSizeHeight(100_spx)
          .AsInteractive([this](InteractiveTrait& trait) {
            trait.ClickedSignal().Connect(this, [this](View view, const InputEvent& event) -> bool {
              mSecondChild.SetBackgroundColor(UiColor("ThemeColor1"));
              return true;
            });
          }),
        View::New() // Blue child
          .SetBackgroundColor(UiColor("ThemeColor2"))
          .SetSizeWidth(100_spx)
          .SetSizeHeight(100_spx)
          .SetPositionX(100_spx)
          .SetPositionY(100_spx)
          .As(mSecondChild),
      }));
  }

  void OnKeyEvent(const KeyEvent& event)
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
