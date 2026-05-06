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

using namespace Dali;
using namespace Dali::Ui;

/**
 * AbsoluteLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 25px for easy visual verification.
 *
 * 1. Padding: root AbsoluteLayout has 25px padding (children inset from window edges).
 * 2. Margin: children alternate between no margin and 50px uniform margin.
 *    - Red box: no margin (flush with padding edge).
 *    - Green box: 50px margin all sides.
 *    - Blue box: 50px margin all sides.
 * 3. Nested: a child AbsoluteLayout with 25px padding, containing inner boxes
 *    with no margin and 50px margin respectively.
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

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: AbsoluteLayout with padding (content inset from window edges)
    AbsoluteLayout root = AbsoluteLayout::New()
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(50, 50, 50, 50)); // start, end, top, bottom

    // --- Red box: no margin (positioned at padding edge) ---
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetWidth(100.0f).SetHeight(100.0f));
    root.Add(redBox);

    // --- Green box: 50px margin all sides ---
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetMargin(Extents(50, 50, 50, 50));
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(100.0f).SetWidth(100.0f).SetHeight(50.0f));
    root.Add(greenBox);

    // --- Blue box: 50px margin all sides ---
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetMargin(Extents(50, 50, 50, 50));
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(100.0f).SetY(200.0f).SetWidth(100.0f).SetHeight(50.0f));
    root.Add(blueBox);

    // --- Nested AbsoluteLayout with its own padding ---
    AbsoluteLayout nested = AbsoluteLayout::New()
      .SetBackgroundColor(Color::GRAY)
      .SetPadding(Extents(50, 50, 50, 50))
      .SetMargin(Extents(50, 50, 50, 50))
      .SetLayoutParams(AbsoluteLayoutParams::New()
        .SetY(300.0f).SetWidth(200.0f).SetHeight(200.0f));

    View innerA = View::New();
    innerA.SetBackgroundColor(Color::MAGENTA);
    innerA.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
      .SetFlags(AbsoluteLayoutFlags::ALL));
    nested.Add(innerA);

    View innerB = View::New();
    innerB.SetBackgroundColor(Color::YELLOW);
    innerB.SetMargin(Extents(50, 50, 50, 50));
    innerB.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(50.0f).SetWidth(50.0f).SetHeight(50.0f));
    nested.Add(innerB);

    View innerC = View::New();
    innerC.SetBackgroundColor(Color::CYAN);
    innerC.SetMargin(Extents(50, 50, 50, 50));
    innerC.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(50.0f).SetWidth(50.0f).SetHeight(50.0f));
    nested.Add(innerC);

    root.Add(nested);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AbsoluteLayoutMarginPaddingController::OnKeyEvent);
  }

  void OnKeyEvent(KeyEvent event)
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
