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
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/stack-layout-params.h>

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

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetSpacing(12.0f);
    outer.SetViewPadding(Extents(16, 16, 16, 16));

    // Row 1: FlexEnd
    FlexLayout rowEnd = FlexLayout::New();
    rowEnd.SetRequestedWidth(MATCH_PARENT);
    rowEnd.SetRequestedHeight(WRAP_CONTENT);
    rowEnd.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    rowEnd.SetDirection(FlexDirection::ROW);
    rowEnd.SetJustifyContent(FlexJustify::FLEX_END);
    rowEnd.SetAlignItems(FlexAlign::CENTER);
    rowEnd.SetViewPadding(Extents(8, 8, 8, 8));
    rowEnd.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    AddThreeBoxes(rowEnd, Color::RED, Color::GREEN, Color::BLUE);
    outer.AddView(rowEnd);

    // Row 2: Center
    FlexLayout rowCenter = FlexLayout::New();
    rowCenter.SetRequestedWidth(MATCH_PARENT);
    rowCenter.SetRequestedHeight(WRAP_CONTENT);
    rowCenter.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    rowCenter.SetDirection(FlexDirection::ROW);
    rowCenter.SetJustifyContent(FlexJustify::CENTER);
    rowCenter.SetAlignItems(FlexAlign::CENTER);
    rowCenter.SetViewPadding(Extents(8, 8, 8, 8));
    rowCenter.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    AddThreeBoxes(rowCenter, Color::YELLOW, Color::CYAN, Color::MAGENTA);
    outer.AddView(rowCenter);

    // Row 3: SpaceAround
    FlexLayout rowAround = FlexLayout::New();
    rowAround.SetRequestedWidth(MATCH_PARENT);
    rowAround.SetRequestedHeight(WRAP_CONTENT);
    rowAround.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    rowAround.SetDirection(FlexDirection::ROW);
    rowAround.SetJustifyContent(FlexJustify::SPACE_AROUND);
    rowAround.SetAlignItems(FlexAlign::CENTER);
    rowAround.SetViewPadding(Extents(8, 8, 8, 8));
    rowAround.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    AddThreeBoxes(rowAround, Color::RED, Color::GREEN, Color::BLUE);
    outer.AddView(rowAround);

    // Row 4: SpaceEvenly
    FlexLayout rowEvenly = FlexLayout::New();
    rowEvenly.SetRequestedWidth(MATCH_PARENT);
    rowEvenly.SetRequestedHeight(WRAP_CONTENT);
    rowEvenly.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    rowEvenly.SetDirection(FlexDirection::ROW);
    rowEvenly.SetJustifyContent(FlexJustify::SPACE_EVENLY);
    rowEvenly.SetAlignItems(FlexAlign::CENTER);
    rowEvenly.SetViewPadding(Extents(8, 8, 8, 8));
    rowEvenly.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    AddThreeBoxes(rowEvenly, Color::YELLOW, Color::CYAN, Color::MAGENTA);
    outer.AddView(rowEvenly);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &FlexLayoutJustifyContentController::OnKeyEvent);
  }

private:
  void AddThreeBoxes(FlexLayout& parent, Vector4 color1, Vector4 color2, Vector4 color3)
  {
    View box1 = View::New();
    box1.SetBackgroundColor(color1);
    box1.SetRequestedWidth(80.0f);
    box1.SetRequestedHeight(60.0f);
    parent.AddView(box1);

    View box2 = View::New();
    box2.SetBackgroundColor(color2);
    box2.SetRequestedWidth(80.0f);
    box2.SetRequestedHeight(60.0f);
    parent.AddView(box2);

    View box3 = View::New();
    box3.SetBackgroundColor(color3);
    box3.SetRequestedWidth(80.0f);
    box3.SetRequestedHeight(60.0f);
    parent.AddView(box3);
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

  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  FlexLayoutJustifyContentController controller(application);
  application.MainLoop();
  return 0;
}
