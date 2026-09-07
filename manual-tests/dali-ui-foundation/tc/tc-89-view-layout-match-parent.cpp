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
 * View sample: MATCH_PARENT sizing.
 *
 * A single root View with MATCH_PARENT width and height fills the entire window.
 *
 */
class TcViewLayoutMatchParent : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "89. View Layout: Match Parent";
  }

  Dali::String GetDescription() const override
  {
    return "Verify a match-parent root fills the available TC area";
  }

  void OnEnter(View contentArea) override
  {

    // Root: single View filling the window
    View root = View::New();
    root.SetBackgroundColor(Color::RED);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    contentArea.Add(root);
  }

  void OnExit() override
  {
    DisconnectAll();
  }

};

REGISTER_MANUAL_TEST(TcViewLayoutMatchParent)
