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
 * FlexLayout wrap and flex-grow sample.
 *
 * Demonstrates FlexWrap::WRAP so items flow to the next line when
 * there is not enough room, combined with flex-grow to distribute
 * remaining space on each line.
 *
 * - Row direction with Wrap enabled
 * - Two sections with three boxes each
 * - Flex-basis values of 50/50/100 and 100/100/200
 * - Flex-grow values of 1/1/2 in each section
 * - AlignContent: Center for wrapped lines
 *
 */
class TcFlexLayoutWrapGrow : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "58. FlexLayout: Wrap and Grow";
  }

  Dali::String GetDescription() const override
  {
    return "Verify wrapped lines and per-item grow factors";
  }

  void OnEnter(View contentArea) override
  {

    // Outer vertical StackLayout to hold two sections
    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    FlexLayout first = FlexLayout::New();
    first.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    first.SetDirection(FlexDirection::ROW);
    first.SetWrap(FlexWrap::WRAP);
    first.SetJustifyContent(FlexJustify::FLEX_START);
    first.SetAlignContent(FlexAlign::CENTER);

    // Box 1: Red, basis 50, grow 1
    View box1 = View::New();
    box1.SetBackgroundColor(Color::RED);
    box1.SetRequestedHeight(100.0f);
    box1.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(50.0f).SetFlexGrow(1.0f));
    first.Add(box1);

    // Box 2: Green, basis 50, grow 1
    View box2 = View::New();
    box2.SetBackgroundColor(Color::GREEN);
    box2.SetRequestedHeight(100.0f);
    box2.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(50.0f).SetFlexGrow(1.0f));
    first.Add(box2);

    // Box 3: Blue, basis 100, grow 2
    View box3 = View::New();
    box3.SetBackgroundColor(Color::BLUE);
    box3.SetRequestedHeight(100.0f);
    box3.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(100.0f).SetFlexGrow(2.0f));
    first.Add(box3);

    outer.Add(first);

    FlexLayout second = FlexLayout::New();
    second.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    second.SetDirection(FlexDirection::ROW);
    second.SetWrap(FlexWrap::WRAP);
    second.SetJustifyContent(FlexJustify::FLEX_START);
    second.SetAlignContent(FlexAlign::CENTER);

    // Box 4: Yellow, basis 100, grow 1
    View box4 = View::New();
    box4.SetBackgroundColor(Color::YELLOW);
    box4.SetRequestedHeight(100.0f);
    box4.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(100.0f).SetFlexGrow(1.0f));
    second.Add(box4);

    // Box 5: Cyan, basis 100, grow 1
    View box5 = View::New();
    box5.SetBackgroundColor(Color::CYAN);
    box5.SetRequestedHeight(100.0f);
    box5.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(100.0f).SetFlexGrow(1.0f));
    second.Add(box5);

    // Box 6: Magenta, basis 200, grow 2
    View box6 = View::New();
    box6.SetBackgroundColor(Color::MAGENTA);
    box6.SetRequestedHeight(100.0f);
    box6.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(200.0f).SetFlexGrow(2.0f));
    second.Add(box6);

    outer.Add(second);

    // // Box 5: Cyan, basis 100, grow 4
    // View box8 = View::New();
    // box8.SetBackgroundColor(Color::CYAN);
    // box8.SetRequestedHeight(100.0f);
    // box8.SetLayoutParams(FlexLayoutParams::New().SetFlexBasis(50.0f).SetFlexGrow(1.0f));
    // root.Add(box8);

    contentArea.Add(outer);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcFlexLayoutWrapGrow)
