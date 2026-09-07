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
 * AbsoluteLayout sample: margin and padding.
 *
 * All margin and padding values use a uniform 50px for easy visual verification.
 *
 * 1. Padding: root AbsoluteLayout has 50px padding (children inset from the edges).
 * 2. Margin: children alternate between no margin and 50px uniform margin.
 *    - Red box: no margin (flush with padding edge).
 *    - Green box: 50px margin all sides.
 *    - Blue box: 50px margin all sides.
 * 3. Nested: a child AbsoluteLayout with 50px padding, containing inner boxes
 *    with no margin and 50px margin respectively.
 *
 */
class TcAbsoluteLayoutMarginPadding : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "48. AbsoluteLayout: Margin and Padding";
  }

  Dali::String GetDescription() const override
  {
    return "Verify container padding, child margin, and nested layouts";
  }

  void OnEnter(View contentArea) override
  {

    // Root: AbsoluteLayout with padding (content inset from window edges)
    AbsoluteLayout root = AbsoluteLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f)); // start, end, top, bottom

    // --- Red box: no margin (positioned at padding edge) ---
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetWidth(100.0f).SetHeight(100.0f));
    root.Add(redBox);

    // --- Green box: 50px margin all sides ---
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(100.0f).SetWidth(100.0f).SetHeight(50.0f));
    root.Add(greenBox);

    // --- Blue box: 50px margin all sides ---
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(100.0f).SetY(200.0f).SetWidth(100.0f).SetHeight(50.0f));
    root.Add(blueBox);

    // --- Nested AbsoluteLayout with its own padding ---
    AbsoluteLayout nested = AbsoluteLayout::New();
    nested.SetBackgroundColor(Color::GRAY);
    nested.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    nested.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    nested.SetLayoutParams(AbsoluteLayoutParams::New()
                             .SetY(300.0f).SetWidth(200.0f).SetHeight(200.0f));

    View innerA = View::New();
    innerA.SetBackgroundColor(Color::MAGENTA);
    innerA.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.0f, 0.0f, 1.0f, 1.0f))
      .SetFlags(AbsoluteLayoutFlags::ALL));
    nested.Add(innerA);

    View innerB = View::New();
    innerB.SetBackgroundColor(Color::YELLOW);
    innerB.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    innerB.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetX(50.0f).SetWidth(50.0f).SetHeight(50.0f));
    nested.Add(innerB);

    View innerC = View::New();
    innerC.SetBackgroundColor(Color::CYAN);
    innerC.SetMargin(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    innerC.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetY(50.0f).SetWidth(50.0f).SetHeight(50.0f));
    nested.Add(innerC);

    root.Add(nested);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcAbsoluteLayoutMarginPadding)
