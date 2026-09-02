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
 * FlexLayout JustifyContent variations sample.
 *
 * Demonstrates all JustifyContent modes not covered in other samples:
 *   - FlexEnd:     items packed to the end
 *   - Center:      items centered
 *   - SpaceAround: equal space around each item
 *   - SpaceEvenly: equal space between all items and edges
 *
 * Layout: four horizontal FlexLayout rows stacked vertically, each
 * using a different JustifyContent mode with three fixed-size boxes.
 *
 * Press Escape or Back to quit.
 */
class FlexLayoutJustifyContentController : public ConnectionTracker
{
public:
  FlexLayoutJustifyContentController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FlexLayoutJustifyContentController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetSpacing(50.0f);
    outer.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Row 1: FlexEnd
    FlexLayout rowEnd = FlexLayout::New();
    rowEnd.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowEnd.SetDirection(FlexDirection::ROW);
    rowEnd.SetJustifyContent(FlexJustify::FLEX_END);
    rowEnd.SetAlignItems(FlexAlign::CENTER);
    rowEnd.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    AddThreeBoxes(rowEnd, Color::RED, Color::GREEN, Color::BLUE);
    outer.Add(rowEnd);

    // Row 2: Center
    FlexLayout rowCenter = FlexLayout::New();
    rowCenter.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowCenter.SetDirection(FlexDirection::ROW);
    rowCenter.SetJustifyContent(FlexJustify::CENTER);
    rowCenter.SetAlignItems(FlexAlign::CENTER);
    rowCenter.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    AddThreeBoxes(rowCenter, Color::YELLOW, Color::CYAN, Color::MAGENTA);
    outer.Add(rowCenter);

    // Row 3: SpaceAround
    FlexLayout rowAround = FlexLayout::New();
    rowAround.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowAround.SetDirection(FlexDirection::ROW);
    rowAround.SetJustifyContent(FlexJustify::SPACE_AROUND);
    rowAround.SetAlignItems(FlexAlign::CENTER);
    rowAround.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    AddThreeBoxes(rowAround, Color::RED, Color::GREEN, Color::BLUE);
    outer.Add(rowAround);

    // Row 4: SpaceEvenly
    FlexLayout rowEvenly = FlexLayout::New();
    rowEvenly.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowEvenly.SetDirection(FlexDirection::ROW);
    rowEvenly.SetJustifyContent(FlexJustify::SPACE_EVENLY);
    rowEvenly.SetAlignItems(FlexAlign::CENTER);
    rowEvenly.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    AddThreeBoxes(rowEvenly, Color::YELLOW, Color::CYAN, Color::MAGENTA);
    outer.Add(rowEvenly);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &FlexLayoutJustifyContentController::OnKeyEvent);
  }

private:
  void AddThreeBoxes(FlexLayout& parent, Vector4 color1, Vector4 color2, Vector4 color3)
  {
    View box1 = View::New();
    box1.SetBackgroundColor(color1);
    box1.SetRequestedWidth(50.0f);
    box1.SetRequestedHeight(50.0f);
    parent.Add(box1);

    View box2 = View::New();
    box2.SetBackgroundColor(color2);
    box2.SetRequestedWidth(50.0f);
    box2.SetRequestedHeight(50.0f);
    parent.Add(box2);

    View box3 = View::New();
    box3.SetBackgroundColor(color3);
    box3.SetRequestedWidth(50.0f);
    box3.SetRequestedHeight(50.0f);
    parent.Add(box3);
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  FlexLayoutJustifyContentController controller(application);
  application.MainLoop();
  return 0;
}
