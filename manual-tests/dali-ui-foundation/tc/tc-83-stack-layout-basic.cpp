/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 * StackLayout sample: a vertical stack with fixed-height and weighted children.
 * - Root is a StackLayout that fills the window.
 * - Three rows: top (fixed), middle (weight 1), bottom (fixed).
 */
class TcStackLayoutBasic : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "83. StackLayout: Basic Weight";
  }

  Dali::String GetDescription() const override
  {
    return "Verify fixed and weighted children in a vertical stack";
  }

  void OnEnter(View contentArea) override
  {

    // Root: vertical StackLayout filling the window
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(10.0f);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Top bar: fixed height, fill cross-axis
    View topBar = View::New();
    topBar.SetBackgroundColor(Color::RED);
    topBar.SetRequestedHeight(100.0f);
    topBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    root.Add(topBar);

    // Middle: weighted to take remaining space, fill cross-axis
    View middle = View::New();
    middle.SetBackgroundColor(Color::GREEN);
    middle.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    root.Add(middle);

    // Bottom bar: fixed height, fill cross-axis
    View bottomBar = View::New();
    bottomBar.SetBackgroundColor(Color::BLUE);
    bottomBar.SetRequestedHeight(100.0f);
    bottomBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    root.Add(bottomBar);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcStackLayoutBasic)
