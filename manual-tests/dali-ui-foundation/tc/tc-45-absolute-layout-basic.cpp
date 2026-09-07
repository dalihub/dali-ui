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
 * AbsoluteLayout sample: absolute pixel positioning.
 *
 * Children are placed at explicit (x, y) coordinates with explicit sizes.
 * - Red box at (50, 50), size 50x50
 * - Green box at (100, 100), size 100x100
 * - Blue box at (200, 200), size 100x50
 *
 */
class TcAbsoluteLayoutBasic : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "45. AbsoluteLayout: Basic";
  }

  Dali::String GetDescription() const override
  {
    return "Verify fixed-size children at absolute pixel coordinates";
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

    // Blue box: lower-right area
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(200.0f, 200.0f, 100.0f, 50.0f)));
    root.Add(blueBox);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcAbsoluteLayoutBasic)
