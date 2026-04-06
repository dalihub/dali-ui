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
 * FlexLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 25px for easy visual verification.
 *
 * 1. Padding: root FlexLayout has 25px padding (content inset from window edges).
 * 2. Margin: children alternate between no margin and 50px uniform margin.
 *    - Red box: no margin (flush with padding edge).
 *    - Green box: 50px margin all sides.
 *    - Blue box: no margin.
 * 3. Nested: a horizontal FlexLayout row with 50px padding and 50px margin,
 *    containing children that alternate no margin / 50px margin.
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutMarginPaddingController : public ConnectionTracker
{
public:
  FlexLayoutMarginPaddingController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutMarginPaddingController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: vertical flex layout with padding
    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::COLUMN);
    root.SetViewPadding(Extents(50, 50, 50, 50)); // start, end, top, bottom

    // --- Red box: no margin (flush with padding edge) ---
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedHeight(50.0f);
    root.Add(redBox);

    // --- Green box: 50px margin all sides ---
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedHeight(50.0f);
    greenBox.SetViewMargin(Extents(50, 50, 50, 50));
    root.Add(greenBox);

    // --- Blue box: no margin ---
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedHeight(50.0f);
    root.Add(blueBox);

    // --- Nested: horizontal FlexLayout with padding + children with margins ---
    FlexLayout nestedRow = FlexLayout::New();
    nestedRow.SetBackgroundColor(Color::GRAY);
    nestedRow.SetRequestedHeight(400.0f);
    nestedRow.SetDirection(FlexDirection::ROW);
    nestedRow.SetAlignItems(FlexAlign::STRETCH);
    nestedRow.SetViewPadding(Extents(50, 50, 50, 50));
    nestedRow.SetViewMargin(Extents(50, 50, 50, 50));

    View childA = View::New();
    childA.SetBackgroundColor(Color::MAGENTA);
    childA.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.Add(childA);

    View childB = View::New();
    childB.SetBackgroundColor(Color::YELLOW);
    childB.SetViewMargin(Extents(50, 50, 50, 50));
    childB.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.Add(childB);

    View childC = View::New();
    childC.SetBackgroundColor(Color::CYAN);
    childC.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.Add(childC);

    root.Add(nestedRow);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &FlexLayoutMarginPaddingController::OnKeyEvent);
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
  FlexLayoutMarginPaddingController controller(application);
  application.MainLoop();
  return 0;
}
