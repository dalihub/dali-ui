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
 * FlexLayout flex-shrink sample.
 *
 * Demonstrates the FlexShrink property which controls how items shrink
 * when their total size exceeds the container.
 *
 * Layout: two horizontal FlexLayout rows stacked vertically:
 *
 *   Row 1 (default shrink):
 *     - All items have flex-basis 250px (total > container width)
 *     - Default shrink (1.0) means all items shrink equally
 *
 *   Row 2 (varied shrink):
 *     - All items have flex-basis 250px
 *     - Item 1: shrink 0 (will NOT shrink, keeps 250px)
 *     - Item 2: shrink 1 (shrinks normally)
 *     - Item 3: shrink 3 (shrinks 3x more than item 2)
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutShrinkController : public ConnectionTracker
{
public:
  FlexLayoutShrinkController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutShrinkController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetSpacing(16.0f)
      .SetPadding(Extents(50, 50, 50, 50));

    // Row 1: Default shrink - all items shrink equally
    FlexLayout row1 = FlexLayout::New()
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetDirection(FlexDirection::ROW)
      .SetWrap(FlexWrap::NO_WRAP)
      .SetAlignItems(FlexAlign::STRETCH)
      .SetPadding(Extents(50, 50, 50, 50))
      .SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    // All three items: basis 250, default shrink (1.0)
    View r1box1 = View::New();
    r1box1.SetBackgroundColor(Color::RED);
    r1box1.SetRequestedHeight(100.0f);
    r1box1.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f));
    row1.Add(r1box1);

    View r1box2 = View::New();
    r1box2.SetBackgroundColor(Color::GREEN);
    r1box2.SetRequestedHeight(100.0f);
    r1box2.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f));
    row1.Add(r1box2);

    View r1box3 = View::New();
    r1box3.SetBackgroundColor(Color::BLUE);
    r1box3.SetRequestedHeight(100.0f);
    r1box3.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f));
    row1.Add(r1box3);

    outer.Add(row1);

    // Row 2: Varied shrink values
    FlexLayout row2 = FlexLayout::New()
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetDirection(FlexDirection::ROW)
      .SetWrap(FlexWrap::NO_WRAP)
      .SetAlignItems(FlexAlign::STRETCH)
      .SetPadding(Extents(50, 50, 50, 50))
      .SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    // Item 1: shrink 0 (will not shrink)
    View r2box1 = View::New();
    r2box1.SetBackgroundColor(Color::YELLOW);
    r2box1.SetRequestedHeight(100.0f);
    r2box1.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f).SetFlexShrink(0.0f));
    row2.Add(r2box1);

    // Item 2: shrink 1 (normal shrink)
    View r2box2 = View::New();
    r2box2.SetBackgroundColor(Color::CYAN);
    r2box2.SetRequestedHeight(100.0f);
    r2box2.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f));
    row2.Add(r2box2);

    // Item 3: shrink 3 (shrinks 3x more)
    View r2box3 = View::New();
    r2box3.SetBackgroundColor(Color::MAGENTA);
    r2box3.SetRequestedHeight(100.0f);
    r2box3.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(250.0f).SetFlexShrink(3.0f));
    row2.Add(r2box3);

    outer.Add(row2);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &FlexLayoutShrinkController::OnKeyEvent);
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
  FlexLayoutShrinkController controller(application);
  application.MainLoop();
  return 0;
}
