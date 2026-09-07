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
 * AbsoluteLayout sample (LayoutMode::STANDALONE): based on absolutelayout-example.
 *
 * Same structure as absolutelayout-example, but the third child (Blue) is set
 * to LayoutMode::STANDALONE with RequestedWidth/Height = (100, 100) and
 * SetRequestedX/Y = (300, 300). It is excluded from the AbsoluteLayout's
 * LayoutParams-driven placement and instead drawn at (300, 300) using its
 * own RequestedWidth/Height.
 *
 */
class TcStandaloneAbsoluteLayout : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "73. Standalone: AbsoluteLayout";
  }

  Dali::String GetDescription() const override
  {
    return "Verify standalone exclusion from AbsoluteLayout placement";
  }

  void OnEnter(View contentArea) override
  {

    // Root: AbsoluteLayout filling the window
    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    // Red box: top-left corner, absolute position and size
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(50.0f, 50.0f, 50.0f, 50.0f)));
    root.Add(redBox);

    // Green box: center area
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(100.0f, 100.0f, 100.0f, 100.0f)));
    root.Add(greenBox);

    // Blue box: Standalone (100x100 at (300, 300))
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(100.0f);
    blueBox.SetRequestedHeight(100.0f);
    blueBox.SetRequestedX(300.0f);
    blueBox.SetRequestedY(300.0f);
    blueBox.SetLayoutMode(LayoutMode::STANDALONE);
    root.Add(blueBox);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcStandaloneAbsoluteLayout)
