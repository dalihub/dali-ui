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
 * StackLayout cross-axis alignment sample.
 *
 * Vertical stack: each row is a narrow box with different HorizontalAlignment
 *   - Row 1: Start (left)
 *   - Row 2: Center
 *   - Row 3: End (right)
 *   - Row 4: Fill (full width)
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
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(50.0f);
    root.SetViewPadding(Extents(50, 50, 50, 50));

    // Row 1: Start (left-aligned narrow box)
    View rowStart = View::New();
    rowStart.SetBackgroundColor(Color::RED);
    rowStart.SetRequestedWidth(100.0f);
    rowStart.SetRequestedHeight(50.0f);
    rowStart.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::START));
    root.Add(rowStart);

    // Row 2: Center
    View rowCenter = View::New();
    rowCenter.SetBackgroundColor(Color::GREEN);
    rowCenter.SetRequestedWidth(100.0f);
    rowCenter.SetRequestedHeight(50.0f);
    rowCenter.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    root.Add(rowCenter);

    // Row 3: End (right-aligned narrow box)
    View rowEnd = View::New();
    rowEnd.SetBackgroundColor(Color::BLUE);
    rowEnd.SetRequestedWidth(100.0f);
    rowEnd.SetRequestedHeight(50.0f);
    rowEnd.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::END));
    root.Add(rowEnd);

    // Row 4: Fill (stretches to full width)
    View rowFill = View::New();
    rowFill.SetBackgroundColor(Color::CYAN);
    rowFill.SetRequestedHeight(50.0f);
    rowFill.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    root.Add(rowFill);

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
