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
 * FlexLayout sample: WRAP_CONTENT with minSize and flex-grow.
 *
 * Demonstrates that a flex-grow child correctly receives the extra
 * space created by a minimum size on a WRAP_CONTENT parent.
 *
 *   - Root is a FlexLayout (ROW) with WRAP_CONTENT width, minWidth=400,
 *     and fixed height 200.
 *   - Child 1 has fixed width 200 and height 200.
 *   - Child 2 (FlexLayout ROW) has flex-grow=1 — fills the
 *     remaining 200px. It contains two grandchildren that each take
 *     half via flex-grow.
 *
 * Expected result:
 *   The flex container wraps to 400 (driven by minWidth, since content
 *   is only 200). flex-grow distributes 400 - 200 = 200 to child 2.
 *   Inside child 2, each grandchild gets 100px via nested flex-grow.
 *
 */
class TcFlexLayoutWrapContentGrow : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "63. FlexLayout: Wrap Content Grow";
  }

  Dali::String GetDescription() const override
  {
    return "Verify grow allocation created by a minimum-sized wrap-content parent";
  }

  void OnEnter(View contentArea) override
  {

    FlexLayout root = FlexLayout::New();
    root.SetDirection(FlexDirection::ROW);
    root.SetBackgroundColor(Color::GAINSBORO);
    root.SetMinimumWidth(400.0f);
    root.SetRequestedHeight(200.0f);

    // Child 1: fixed 200x200 — drives the WRAP_CONTENT parent's content size.
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(200.0f);
    child1.SetRequestedHeight(200.0f);
    root.Add(child1);

    // Child 2: FlexLayout with flex-grow=1 — fills the remaining 200px (400 - 200).
    // Contains two grandchildren that each take 100px via flex-grow.
    FlexLayout child2 = FlexLayout::New();
    child2.SetDirection(FlexDirection::ROW);
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedHeight(200.0f);
    child2.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    root.Add(child2);

    View grandChild1 = View::New();
    grandChild1.SetBackgroundColor(Color::BLUE);
    grandChild1.SetRequestedHeight(200.0f);
    grandChild1.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    child2.Add(grandChild1);

    View grandChild2 = View::New();
    grandChild2.SetBackgroundColor(Color::YELLOW);
    grandChild2.SetRequestedHeight(200.0f);
    grandChild2.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    child2.Add(grandChild2);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcFlexLayoutWrapContentGrow)
