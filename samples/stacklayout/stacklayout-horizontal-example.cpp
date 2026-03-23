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
 * StackLayout horizontal orientation and vertical alignment sample.
 *
 * Demonstrates Horizontal StackLayout with cross-axis (vertical) alignment
 * on each child, plus LayoutWeight for proportional sizing.
 *
 * Layout:
 *   A vertical StackLayout holds four horizontal rows.
 *   Each row is a Horizontal StackLayout showing a different cross-axis alignment:
 *     Row 1: Start   (top-aligned)
 *     Row 2: Center  (center-aligned)
 *     Row 3: End     (bottom-aligned)
 *     Row 4: Fill    (stretched to fill)
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
    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetSpacing(10.0f);
    outer.SetViewPadding(Extents(16, 16, 16, 16));

    // Row 1: Horizontal stack, children aligned to Start (top)
    StackLayout row1 = StackLayout::New(StackOrientation::HORIZONTAL);
    row1.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    row1.SetSpacing(8.0f);
    row1.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View box1a = View::New();
    box1a.SetBackgroundColor(Color::RED);
    box1a.SetRequestedWidth(60.0f);
    box1a.SetRequestedHeight(40.0f);
    box1a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    row1.Add(box1a);

    View box1b = View::New();
    box1b.SetBackgroundColor(Color::GREEN);
    box1b.SetRequestedWidth(60.0f);
    box1b.SetRequestedHeight(60.0f);
    box1b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    row1.Add(box1b);

    View box1c = View::New();
    box1c.SetBackgroundColor(Color::BLUE);
    box1c.SetRequestedWidth(WRAP_CONTENT);
    box1c.SetRequestedHeight(30.0f);
    box1c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START).SetWeight(1.0f));
    row1.Add(box1c);

    outer.Add(row1);

    // Row 2: Horizontal stack, children aligned to Center
    StackLayout row2 = StackLayout::New(StackOrientation::HORIZONTAL);
    row2.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    row2.SetSpacing(8.0f);
    row2.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View box2a = View::New();
    box2a.SetBackgroundColor(Color::RED);
    box2a.SetRequestedWidth(60.0f);
    box2a.SetRequestedHeight(40.0f);
    box2a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    row2.Add(box2a);

    View box2b = View::New();
    box2b.SetBackgroundColor(Color::GREEN);
    box2b.SetRequestedWidth(60.0f);
    box2b.SetRequestedHeight(60.0f);
    box2b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    row2.Add(box2b);

    View box2c = View::New();
    box2c.SetBackgroundColor(Color::BLUE);
    box2c.SetRequestedWidth(WRAP_CONTENT);
    box2c.SetRequestedHeight(30.0f);
    box2c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER).SetWeight(1.0f));
    row2.Add(box2c);

    outer.Add(row2);

    // Row 3: Horizontal stack, children aligned to End (bottom)
    StackLayout row3 = StackLayout::New(StackOrientation::HORIZONTAL);
    row3.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    row3.SetSpacing(8.0f);
    row3.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View box3a = View::New();
    box3a.SetBackgroundColor(Color::RED);
    box3a.SetRequestedWidth(60.0f);
    box3a.SetRequestedHeight(40.0f);
    box3a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
    row3.Add(box3a);

    View box3b = View::New();
    box3b.SetBackgroundColor(Color::GREEN);
    box3b.SetRequestedWidth(60.0f);
    box3b.SetRequestedHeight(60.0f);
    box3b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
    row3.Add(box3b);

    View box3c = View::New();
    box3c.SetBackgroundColor(Color::BLUE);
    box3c.SetRequestedWidth(WRAP_CONTENT);
    box3c.SetRequestedHeight(30.0f);
    box3c.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END).SetWeight(1.0f));
    row3.Add(box3c);

    outer.Add(row3);

    // Row 4: Horizontal stack, children use FILL to stretch cross-axis (height)
    StackLayout row4 = StackLayout::New(StackOrientation::HORIZONTAL);
    row4.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    row4.SetSpacing(8.0f);
    row4.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View box4a = View::New();
    box4a.SetBackgroundColor(Color::RED);
    box4a.SetRequestedWidth(60.0f);
    box4a.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    row4.Add(box4a);

    View box4b = View::New();
    box4b.SetBackgroundColor(Color::GREEN);
    box4b.SetRequestedWidth(60.0f);
    box4b.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    row4.Add(box4b);

    View box4c = View::New();
    box4c.SetBackgroundColor(Color::BLUE);
    box4c.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    row4.Add(box4c);

    outer.Add(row4);

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
