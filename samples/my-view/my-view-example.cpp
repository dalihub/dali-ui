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
#include "my-view.h"

using namespace Dali;
using namespace Dali::Ui;
using namespace MyViewSample;
using Dali::Ui::View;

// This example shows how to define a new class derived from View
class MyViewController : public ConnectionTracker
{
public:

  MyViewController(Application& application)
    : mApplication(application)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect(this, &MyViewController::Create);
  }

  ~MyViewController() = default; // Nothing to do in destructor

  // The Init signal is received once (only) during the Application lifetime
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect(this, &MyViewController::OnKeyEvent);
    window.SetBackgroundColor(Color::WHITE);

    mMyView = MyView::New();
    mMyView.SetBackgroundColor(UiColor(0x00FFFF));
    mMyView.SetRequestedWidth(200_spx);
    mMyView.SetRequestedHeight(200_spx);
    window.Add(mMyView);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if(event.GetKeyName() == "1")
      {
        mMyView.ChangeBackground();
      }
      else if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
  MyView mMyView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  MyViewController test(application);
  application.MainLoop();
  return 0;
}
