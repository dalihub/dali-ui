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
using namespace Dali::Ui;

/**
 * StackLayout cross-axis alignment sample.
 *
 * Vertical stack: each row is a narrow box with different HorizontalAlignment
 *   - Row 1: Start (left)
 *   - Row 2: Center
 *   - Row 3: End (right)
 *   - Row 4: MatchParent (full width; cross-axis Fill is treated as Start)
 *
 * Press Escape or Back to quit.
 */
class StackLayoutAlignmentController : public ConnectionTracker
{
public:
  StackLayoutAlignmentController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutAlignmentController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root: vertical stack showing cross-axis (horizontal) alignment
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetLayoutWidth(LayoutDimension::MatchParent);
    root.SetLayoutHeight(LayoutDimension::MatchParent);
    root.SetSpacing(12.0f);
    root.SetViewPadding(Extents(24, 24, 24, 24));

    // Row 1: Start (left-aligned narrow box)
    View rowStart = View::New();
    rowStart.SetBackgroundColor(Color::RED);
    rowStart.SetLayoutWidth(80.0f);
    rowStart.SetLayoutHeight(50.0f);
    rowStart.SetHorizontalAlignment(LayoutAlignment::START);
    root.AddView(rowStart);

    // Row 2: Center
    View rowCenter = View::New();
    rowCenter.SetBackgroundColor(Color::GREEN);
    rowCenter.SetLayoutWidth(80.0f);
    rowCenter.SetLayoutHeight(50.0f);
    rowCenter.SetHorizontalAlignment(LayoutAlignment::CENTER);
    root.AddView(rowCenter);

    // Row 3: End (right-aligned narrow box)
    View rowEnd = View::New();
    rowEnd.SetBackgroundColor(Color::BLUE);
    rowEnd.SetLayoutWidth(80.0f);
    rowEnd.SetLayoutHeight(50.0f);
    rowEnd.SetHorizontalAlignment(LayoutAlignment::END);
    root.AddView(rowEnd);

    // Row 4: full width (use MatchParent; Fill on cross axis is treated as Start)
    View rowFill = View::New();
    rowFill.SetBackgroundColor(Color::CYAN);
    rowFill.SetLayoutWidth(LayoutDimension::MatchParent);
    rowFill.SetLayoutHeight(50.0f);
    rowFill.SetHorizontalAlignment(LayoutAlignment::FILL);
    root.AddView(rowFill);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &StackLayoutAlignmentController::OnKeyEvent);
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
  StackLayoutAlignmentController controller(application);
  application.MainLoop();
  return 0;
}
