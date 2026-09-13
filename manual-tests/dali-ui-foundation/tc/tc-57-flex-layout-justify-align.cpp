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
 * FlexLayout justify content and align items sample.
 *
 * Demonstrates SpaceBetween justify and Center cross-axis alignment
 * in a horizontal flex row.
 *
 * - Six fixed-size boxes with per-item alignment overrides
 * - JustifyContent: SpaceBetween (even spacing between items)
 * - AlignItems: Center (vertically centered on the cross axis)
 * - One child uses AlignSelf to override to FlexEnd
 *
 */
class TcFlexLayoutJustifyAlign : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "57. FlexLayout: Justify and Align";
  }

  Dali::String GetDescription() const override
  {
    return "Verify justify-content, align-items, and align-self";
  }

  void OnEnter(View contentArea) override
  {

    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::ROW);
    root.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
    root.SetAlignItems(FlexAlign::CENTER);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Red box
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedWidth(50.0f);
    redBox.SetRequestedHeight(200.0f);
    root.Add(redBox);

    // Green box
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedWidth(50.0f);
    greenBox.SetRequestedHeight(200.0f);
    greenBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::FLEX_START));
    root.Add(greenBox);

    // Blue box
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(50.0f);
    blueBox.SetRequestedHeight(200.0f);
    blueBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::CENTER));
    root.Add(blueBox);

    // Yellow box
    View yellowBox = View::New();
    yellowBox.SetBackgroundColor(Color::YELLOW);
    yellowBox.SetRequestedWidth(50.0f);
    yellowBox.SetRequestedHeight(200.0f);
    yellowBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::FLEX_END));
    root.Add(yellowBox);

    // Cyan box
    View cyanBox = View::New();
    cyanBox.SetBackgroundColor(Color::CYAN);
    cyanBox.SetRequestedWidth(50.0f);
    cyanBox.SetRequestedHeight(200.0f);
    cyanBox.SetLayoutParams(FlexLayoutParams::New().SetAlignSelf(FlexAlign::BASELINE));
    root.Add(cyanBox);

    // Magenta box
    View magentaBox = View::New();
    magentaBox.SetBackgroundColor(Color::MAGENTA);
    magentaBox.SetRequestedWidth(50.0f);
    magentaBox.SetRequestedHeight(200.0f);
    root.Add(magentaBox);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcFlexLayoutJustifyAlign)
