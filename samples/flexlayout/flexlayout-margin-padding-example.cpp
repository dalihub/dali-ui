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
#include <dali-ui-foundation/public-api/flex-layout.h>
#include <dali-ui-foundation/public-api/flex-layout-params.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * FlexLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 25px for easy visual verification.
 *
 * 1. Padding: root FlexLayout has 25px padding (content inset from window edges).
 * 2. Margin: children alternate between no margin and 25px uniform margin.
 *    - Red box: no margin (flush with padding edge).
 *    - Green box: 25px margin all sides.
 *    - Blue box: no margin.
 * 3. Nested: a horizontal FlexLayout row with 25px padding and 25px margin,
 *    containing children that alternate no margin / 25px margin.
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
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);
    root.SetDirection(FlexDirection::COLUMN);
    root.SetViewPadding(Extents(25, 25, 25, 25)); // start, end, top, bottom

    // --- Red box: no margin (flush with padding edge) ---
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutWidth(LayoutDimension::MatchParent);
    redBox.SetLayoutHeight(60.0f);
    root.AddView(redBox);

    // --- Green box: 25px margin all sides ---
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutWidth(LayoutDimension::MatchParent);
    greenBox.SetLayoutHeight(60.0f);
    greenBox.SetViewMargin(Extents(25, 25, 25, 25));
    root.AddView(greenBox);

    // --- Blue box: no margin ---
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutWidth(LayoutDimension::MatchParent);
    blueBox.SetLayoutHeight(60.0f);
    root.AddView(blueBox);

    // --- Nested: horizontal FlexLayout with padding + children with margins ---
    FlexLayout nestedRow = FlexLayout::New();
    nestedRow.SetBackgroundColor(Color::GRAY);
    nestedRow.SetLayoutWidth(LayoutDimension::MatchParent);
    nestedRow.SetLayoutHeight(150.0f);
    nestedRow.SetDirection(FlexDirection::ROW);
    nestedRow.SetAlignItems(FlexAlign::STRETCH);
    nestedRow.SetViewPadding(Extents(25, 25, 25, 25));
    nestedRow.SetViewMargin(Extents(25, 25, 25, 25));

    View childA = View::New();
    childA.SetBackgroundColor(Color::MAGENTA);
    childA.SetLayoutWidth(LayoutDimension::WrapContent);
    childA.SetLayoutHeight(LayoutDimension::MatchParent);
    childA.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.AddView(childA);

    View childB = View::New();
    childB.SetBackgroundColor(Color::YELLOW);
    childB.SetLayoutWidth(LayoutDimension::WrapContent);
    childB.SetLayoutHeight(LayoutDimension::MatchParent);
    childB.SetViewMargin(Extents(25, 25, 25, 25));
    childB.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.AddView(childB);

    View childC = View::New();
    childC.SetBackgroundColor(Color::CYAN);
    childC.SetLayoutWidth(LayoutDimension::WrapContent);
    childC.SetLayoutHeight(LayoutDimension::MatchParent);
    childC.SetViewMargin(Extents(25, 25, 25, 25));
    childC.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    nestedRow.AddView(childC);

    root.AddView(nestedRow);

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
