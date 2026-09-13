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

#include "manual-test-case.h"

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
 */
class TcStackLayoutAlignment : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "84. StackLayout: Alignment";
  }

  Dali::String GetDescription() const override
  {
    return "Compare start, center, end, and fill cross-axis alignment";
  }

  void OnEnter(View contentArea) override
  {

    // Root: vertical stack showing cross-axis (horizontal) alignment
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(50.0f);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

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

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcStackLayoutAlignment)
