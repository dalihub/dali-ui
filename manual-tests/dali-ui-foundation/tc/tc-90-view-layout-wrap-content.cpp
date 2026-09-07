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
 * View sample: WRAP_CONTENT sizing.
 *
 * Root View has WRAP_CONTENT size. It contains two children:
 * - Red box at (0, 0) with size (50, 50).
 * - Green box at (100, 100) with size (100, 100).
 *
 * The root should shrink-wrap to fit both children (200 x 200).
 *
 */
class TcViewLayoutWrapContent : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "90. View Layout: Wrap Content";
  }

  Dali::String GetDescription() const override
  {
    return "Verify wrap-content bounds include positioned children";
  }

  void OnEnter(View contentArea) override
  {

    // Root: WRAP_CONTENT View (background visible as bounding area)
    View root = View::New();
    root.SetBackgroundColor(Color::BLUE);
    root.SetRequestedWidth(WRAP_CONTENT);
    root.SetRequestedHeight(WRAP_CONTENT);

    // Child 1: Red box at (0, 0), size 50x50
    View child1 = View::New();
    child1.SetBackgroundColor(Color::RED);
    child1.SetRequestedWidth(50.0f);
    child1.SetRequestedHeight(50.0f);
    root.Add(child1);

    // Child 2: Green box at (100, 100), size 100x100
    View child2 = View::New();
    child2.SetBackgroundColor(Color::GREEN);
    child2.SetRequestedWidth(100.0f);
    child2.SetRequestedHeight(100.0f);
    child2.SetRequestedX(100.0f);
    child2.SetRequestedY(100.0f);
    root.Add(child2);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcViewLayoutWrapContent)
