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
 * FlexLayout direction reverse sample.
 *
 * Demonstrates RowReverse and ColumnReverse directions.
 *
 * Layout: a vertical StackLayout holding two FlexLayout sections:
 *
 *   Section 1: FlexDirection::ROW_REVERSE
 *     - Items laid out right-to-left (1, 2, 3 appear as 3, 2, 1)
 *     - JustifyContent: FlexStart (items pack to the right/start of reverse)
 *
 *   Section 2: FlexDirection::COLUMN_REVERSE
 *     - Items laid out bottom-to-top (1, 2, 3 appear as 3, 2, 1)
 *     - JustifyContent: FlexStart (items pack to the bottom/start of reverse)
 *
 */
class TcFlexLayoutDirectionReverse : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "60. FlexLayout: Reverse Directions";
  }

  Dali::String GetDescription() const override
  {
    return "Verify row-reverse and column-reverse ordering";
  }

  void OnEnter(View contentArea) override
  {

    // Outer vertical StackLayout to hold two sections
    StackLayout outer = StackLayout::New(StackOrientation::VERTICAL);
    outer.SetRequestedWidth(MATCH_PARENT);
    outer.SetRequestedHeight(MATCH_PARENT);
    outer.SetSpacing(50);
    outer.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Section 1: RowReverse
    FlexLayout rowReverse = FlexLayout::New();
    rowReverse.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    rowReverse.SetDirection(FlexDirection::ROW_REVERSE);
    rowReverse.SetJustifyContent(FlexJustify::FLEX_START);
    rowReverse.SetAlignItems(FlexAlign::CENTER);
    rowReverse.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    rowReverse.SetBackgroundColor(Vector4(0.95f, 0.95f, 0.95f, 1.0f));

    View rrBox1 = View::New();
    rrBox1.SetBackgroundColor(Color::RED);
    rrBox1.SetRequestedWidth(100.0f);
    rrBox1.SetRequestedHeight(50.0f);
    rowReverse.Add(rrBox1);

    View rrBox2 = View::New();
    rrBox2.SetBackgroundColor(Color::GREEN);
    rrBox2.SetRequestedWidth(100.0f);
    rrBox2.SetRequestedHeight(100.0f);
    rowReverse.Add(rrBox2);

    View rrBox3 = View::New();
    rrBox3.SetBackgroundColor(Color::BLUE);
    rrBox3.SetRequestedWidth(100.0f);
    rrBox3.SetRequestedHeight(200.0f);
    rowReverse.Add(rrBox3);

    outer.Add(rowReverse);

    // Section 2: ColumnReverse
    FlexLayout columnReverse = FlexLayout::New();
    columnReverse.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    columnReverse.SetDirection(FlexDirection::COLUMN_REVERSE);
    columnReverse.SetJustifyContent(FlexJustify::FLEX_START);
    columnReverse.SetAlignItems(FlexAlign::CENTER);
    columnReverse.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));
    columnReverse.SetBackgroundColor(Vector4(0.9f, 0.9f, 0.9f, 1.0f));

    View crBox1 = View::New();
    crBox1.SetBackgroundColor(Color::YELLOW);
    crBox1.SetRequestedWidth(50.0f);
    crBox1.SetRequestedHeight(100.0f);
    columnReverse.Add(crBox1);

    View crBox2 = View::New();
    crBox2.SetBackgroundColor(Color::CYAN);
    crBox2.SetRequestedWidth(100.0f);
    crBox2.SetRequestedHeight(100.0f);
    columnReverse.Add(crBox2);

    View crBox3 = View::New();
    crBox3.SetBackgroundColor(Color::MAGENTA);
    crBox3.SetRequestedWidth(200.0f);
    crBox3.SetRequestedHeight(100.0f);
    columnReverse.Add(crBox3);

    outer.Add(columnReverse);

    contentArea.Add(outer);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcFlexLayoutDirectionReverse)
