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

#include "label-button.h"

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <cstdio>

using namespace Dali;
using namespace Dali::Ui;
using namespace Example;

class LabelButtonExampleController : public ConnectionTracker
{
public:
  explicit LabelButtonExampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &LabelButtonExampleController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));
    window.Add(LabelButton::New("LabelButton: 0"));
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  LabelButtonExampleController controller(application);
  application.MainLoop();
  return 0;
}
