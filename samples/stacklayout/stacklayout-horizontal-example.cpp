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
#include <dali-ui-foundation/public-api/stack-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>

using namespace Dali;
using namespace Dali::UI;

/**
 * StackLayout horizontal orientation and vertical alignment sample.
 *
 * Demonstrates Horizontal StackLayout with cross-axis (vertical) alignment
 * on each child, plus LayoutWeight for proportional sizing.
 *
 * Layout:
 *   A vertical StackLayout holds four horizontal rows.
 *   Each row is a Horizontal StackLayout showing a different VerticalAlignment:
 *     Row 1: VerticalAlignment::Start   (top-aligned)
 *     Row 2: VerticalAlignment::Center  (center-aligned)
 *     Row 3: VerticalAlignment::End     (bottom-aligned)
 *     Row 4: VerticalAlignment::Fill    (stretched to fill)
 *
 * Press Escape or Back to quit.
 */
class StackLayoutHorizontalController : public ConnectionTracker
{
public:
  StackLayoutHorizontalController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutHorizontalController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Outer vertical stack to hold multiple horizontal rows
    StackLayout outer = StackLayout::New(StackOrientation::Vertical);
    outer.SetLayoutWidth(LayoutDimension::MatchParent);
    outer.SetLayoutHeight(LayoutDimension::MatchParent);
    outer.SetSpacing(10.0f);
    outer.SetViewPadding(Extents(16, 16, 16, 16));

    // Row 1: Horizontal stack, children aligned to Start (top)
    StackLayout row1 = StackLayout::New(StackOrientation::Horizontal);
    row1.SetLayoutWidth(LayoutDimension::MatchParent);
    row1.SetLayoutHeight(LayoutDimension::WrapContent);
    StackLayout::SetLayoutWeight(row1, 1.0f);
    row1.SetSpacing(8.0f);
    row1.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View box1a = View::New();
    box1a.SetBackgroundColor(Color::RED);
    box1a.SetLayoutWidth(60.0f);
    box1a.SetLayoutHeight(40.0f);
    box1a.SetVerticalAlignment(LayoutAlignment::Start);
    row1.AddView(box1a);

    View box1b = View::New();
    box1b.SetBackgroundColor(Color::GREEN);
    box1b.SetLayoutWidth(60.0f);
    box1b.SetLayoutHeight(60.0f);
    box1b.SetVerticalAlignment(LayoutAlignment::Start);
    row1.AddView(box1b);

    View box1c = View::New();
    box1c.SetBackgroundColor(Color::BLUE);
    box1c.SetLayoutWidth(LayoutDimension::WrapContent);
    box1c.SetLayoutHeight(30.0f);
    box1c.SetVerticalAlignment(LayoutAlignment::Start);
    StackLayout::SetLayoutWeight(box1c, 1.0f);
    row1.AddView(box1c);

    outer.AddView(row1);

    // Row 2: Horizontal stack, children aligned to Center
    StackLayout row2 = StackLayout::New(StackOrientation::Horizontal);
    row2.SetLayoutWidth(LayoutDimension::MatchParent);
    row2.SetLayoutHeight(LayoutDimension::WrapContent);
    StackLayout::SetLayoutWeight(row2, 1.0f);
    row2.SetSpacing(8.0f);
    row2.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View box2a = View::New();
    box2a.SetBackgroundColor(Color::RED);
    box2a.SetLayoutWidth(60.0f);
    box2a.SetLayoutHeight(40.0f);
    box2a.SetVerticalAlignment(LayoutAlignment::Center);
    row2.AddView(box2a);

    View box2b = View::New();
    box2b.SetBackgroundColor(Color::GREEN);
    box2b.SetLayoutWidth(60.0f);
    box2b.SetLayoutHeight(60.0f);
    box2b.SetVerticalAlignment(LayoutAlignment::Center);
    row2.AddView(box2b);

    View box2c = View::New();
    box2c.SetBackgroundColor(Color::BLUE);
    box2c.SetLayoutWidth(LayoutDimension::WrapContent);
    box2c.SetLayoutHeight(30.0f);
    box2c.SetVerticalAlignment(LayoutAlignment::Center);
    StackLayout::SetLayoutWeight(box2c, 1.0f);
    row2.AddView(box2c);

    outer.AddView(row2);

    // Row 3: Horizontal stack, children aligned to End (bottom)
    StackLayout row3 = StackLayout::New(StackOrientation::Horizontal);
    row3.SetLayoutWidth(LayoutDimension::MatchParent);
    row3.SetLayoutHeight(LayoutDimension::WrapContent);
    StackLayout::SetLayoutWeight(row3, 1.0f);
    row3.SetSpacing(8.0f);
    row3.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View box3a = View::New();
    box3a.SetBackgroundColor(Color::RED);
    box3a.SetLayoutWidth(60.0f);
    box3a.SetLayoutHeight(40.0f);
    box3a.SetVerticalAlignment(LayoutAlignment::End);
    row3.AddView(box3a);

    View box3b = View::New();
    box3b.SetBackgroundColor(Color::GREEN);
    box3b.SetLayoutWidth(60.0f);
    box3b.SetLayoutHeight(60.0f);
    box3b.SetVerticalAlignment(LayoutAlignment::End);
    row3.AddView(box3b);

    View box3c = View::New();
    box3c.SetBackgroundColor(Color::BLUE);
    box3c.SetLayoutWidth(LayoutDimension::WrapContent);
    box3c.SetLayoutHeight(30.0f);
    box3c.SetVerticalAlignment(LayoutAlignment::End);
    StackLayout::SetLayoutWeight(box3c, 1.0f);
    row3.AddView(box3c);

    outer.AddView(row3);

    // Row 4: Horizontal stack, children aligned to Fill (stretch height)
    StackLayout row4 = StackLayout::New(StackOrientation::Horizontal);
    row4.SetLayoutWidth(LayoutDimension::MatchParent);
    row4.SetLayoutHeight(LayoutDimension::WrapContent);
    StackLayout::SetLayoutWeight(row4, 1.0f);
    row4.SetSpacing(8.0f);
    row4.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View box4a = View::New();
    box4a.SetBackgroundColor(Color::RED);
    box4a.SetLayoutWidth(60.0f);
    box4a.SetLayoutHeight(LayoutDimension::WrapContent);
    box4a.SetVerticalAlignment(LayoutAlignment::Fill);
    row4.AddView(box4a);

    View box4b = View::New();
    box4b.SetBackgroundColor(Color::GREEN);
    box4b.SetLayoutWidth(60.0f);
    box4b.SetLayoutHeight(LayoutDimension::WrapContent);
    box4b.SetVerticalAlignment(LayoutAlignment::Fill);
    row4.AddView(box4b);

    View box4c = View::New();
    box4c.SetBackgroundColor(Color::BLUE);
    box4c.SetLayoutWidth(LayoutDimension::WrapContent);
    box4c.SetLayoutHeight(LayoutDimension::WrapContent);
    box4c.SetVerticalAlignment(LayoutAlignment::Fill);
    StackLayout::SetLayoutWeight(box4c, 1.0f);
    row4.AddView(box4c);

    outer.AddView(row4);

    window.Add(outer);
    window.KeyEventSignal().Connect(this, &StackLayoutHorizontalController::OnKeyEvent);
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
  StackLayoutHorizontalController controller(application);
  application.MainLoop();
  return 0;
}
