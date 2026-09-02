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

#include "tv-config.h"

using namespace Dali;
using namespace Dali::Ui;

class TVConfigController : public ConnectionTracker
{
public:
  explicit TVConfigController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TVConfigController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    Layout root = Layout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(32.0f, 32.0f, 32.0f, 32.0f));
    window.Add(root);

    Label title = Label::New("TVConfig");
    title.SetRequestedWidth(420.0f);
    title.SetRequestedHeight(80.0f);
    title.SetFontSize(28.0f);
    root.Add(title);

    View panel = View::New();
    panel.SetRequestedWidth(420.0f);
    panel.SetRequestedHeight(180.0f);
    panel.SetRequestedY(100.0f);
    root.Add(panel);

    Label label = Label::New("Hello World!");
    label.SetRequestedWidth(380.0f);
    label.SetRequestedHeight(100.0f);
    label.SetRequestedX(20.0f);
    label.SetRequestedY(40.0f);
    label.SetFontSize(20.0f);
    label.SetMultiLine(true);
    panel.Add(label);
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  TVConfig config = TVConfig::New();
  config.Apply();

  TVConfigController controller(application);
  application.MainLoop();
  return 0;
}
