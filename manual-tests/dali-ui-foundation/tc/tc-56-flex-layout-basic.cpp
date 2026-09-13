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
 * FlexLayout sample: basic row direction with flex grow.
 *
 * A horizontal flex row with three children:
 * - Red box: fixed 100px width
 * - Green box: flex-grow 1 (takes remaining space)
 * - Blue box: fixed 100px width
 *
 */
class TcFlexLayoutBasic : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "56. FlexLayout: Basic Grow";
  }

  Dali::String GetDescription() const override
  {
    return "Verify a row with fixed children and a growing child";
  }

  void OnEnter(View contentArea) override
  {

    // Root: FlexLayout in row direction
    FlexLayout root = FlexLayout::New();
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetDirection(FlexDirection::ROW);
    root.SetAlignItems(FlexAlign::STRETCH);
    root.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Red box: fixed width
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedWidth(100.0f);
    root.Add(redBox);

    // Green box: flex-grow 1 (fills remaining space)
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedWidth(WRAP_CONTENT);
    greenBox.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    root.Add(greenBox);

    // Blue box: fixed width
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(100.0f);
    root.Add(blueBox);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcFlexLayoutBasic)
