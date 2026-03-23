/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/layouts/absolute-layout.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * AbsoluteLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 25px for easy visual verification.
 *
 * 1. Padding: root AbsoluteLayout has 25px padding (children inset from window edges).
 * 2. Margin: children alternate between no margin and 25px uniform margin.
 *    - Red box: no margin (flush with padding edge).
 *    - Green box: 25px margin all sides.
 *    - Blue box: 25px margin all sides.
 * 3. Nested: a child AbsoluteLayout with 25px padding, containing inner boxes
 *    with no margin and 25px margin respectively.
 *
 * Press Escape or Back to quit.
 */
class AbsoluteLayoutMarginPaddingController : public ConnectionTracker
{
public:
  AbsoluteLayoutMarginPaddingController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AbsoluteLayoutMarginPaddingController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: AbsoluteLayout with padding (content inset from window edges)
    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetViewPadding(Extents(25, 25, 25, 25)); // start, end, top, bottom

    // --- Red box: no margin (positioned at padding edge) ---
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetWidth(150.0f).SetHeight(80.0f));
    root.AddView(redBox);

    // --- Green box: 25px margin all sides ---
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetViewMargin(Extents(25, 25, 25, 25));
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(100.0f).SetWidth(150.0f).SetHeight(80.0f));
    root.AddView(greenBox);

    // --- Blue box: 25px margin all sides ---
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetViewMargin(Extents(25, 25, 25, 25));
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(210.0f).SetWidth(150.0f).SetHeight(80.0f));
    root.AddView(blueBox);

    // --- Nested AbsoluteLayout with its own padding ---
    AbsoluteLayout nested = AbsoluteLayout::New();
    nested.SetBackgroundColor(Color::GRAY);
    nested.SetViewPadding(Extents(25, 25, 25, 25));
    nested.SetViewMargin(Extents(25, 25, 25, 25));
    nested.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(320.0f).SetWidth(350.0f).SetHeight(200.0f));

    View innerA = View::New();
    innerA.SetBackgroundColor(Color::MAGENTA);
    innerA.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetWidth(100.0f).SetHeight(60.0f));
    nested.AddView(innerA);

    View innerB = View::New();
    innerB.SetBackgroundColor(Color::YELLOW);
    innerB.SetViewMargin(Extents(25, 25, 25, 25));
    innerB.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(120.0f).SetWidth(100.0f).SetHeight(60.0f));
    nested.AddView(innerB);

    View innerC = View::New();
    innerC.SetBackgroundColor(Color::CYAN);
    innerC.SetViewMargin(Extents(25, 25, 25, 25));
    innerC.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(80.0f).SetWidth(220.0f).SetHeight(60.0f));
    nested.AddView(innerC);

    root.AddView(nested);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutMarginPaddingController::OnKeyEvent);
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  AbsoluteLayoutMarginPaddingController controller(application);
  application.MainLoop();
  return 0;
}
