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

namespace
{
View CreatePanel(const Vector3& position, const Vector2& size, const UiColor& color)
{
  View panel = View::New();
  panel.SetParentOrigin(ParentOrigin::TOP_LEFT);
  panel.SetPivot(Vector3(0.0f, 0.0f, 0.5f));
  panel.SetRequestedX(position.x);
  panel.SetRequestedY(position.y);
  panel.SetRequestedWidth(size.width);
  panel.SetRequestedHeight(size.height);
  panel.SetBackgroundColor(color);
  return panel;
}
} // unnamed namespace

class TransparentWindowController : public ConnectionTracker
{
public:
  explicit TransparentWindowController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TransparentWindowController::Create);
  }

private:
  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::TRANSPARENT);

    // The initial Windows backend support is color-key based. Red and green
    // should remain visible while the transparent-black window background
    // reveals the desktop. The black panel demonstrates the known limitation:
    // it is also removed by the color key.
    window.Add(CreatePanel(Vector3(60.0f, 80.0f, 0.0f), Vector2(180.0f, 180.0f), UiColor(0xE53935)));
    window.Add(CreatePanel(Vector3(270.0f, 80.0f, 0.0f), Vector2(180.0f, 180.0f), UiColor(0x43A047)));
    window.Add(CreatePanel(Vector3(480.0f, 80.0f, 0.0f), Vector2(180.0f, 180.0f), UiColor(0x000000)));
    // Keep the renderer active long enough to evaluate window dragging and
    // input responsiveness while transparent content is being presented.
    window.KeepRendering(60.0f);
  }

  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  WindowData   windowData;
  PositionSize positionSize(100, 100, 720, 360);
  windowData.SetPositionSize(positionSize);
  windowData.SetTransparency(true);

  Application application = Application::New(&argc, &argv, "", false, windowData);
  TransparentWindowController controller(application);
  application.MainLoop();
  return 0;
}
