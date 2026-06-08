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

using namespace Dali;
using namespace Dali::Ui;

/**
 * @brief Verifies that Label renders a simple text string.
 *
 * Expected result:
 *   "Hello, World!" is displayed centered on screen in large text.
 */
class TcHelloWorld : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "Hello World";
  }

  Dali::String GetDescription() const override
  {
    return "Label displays a centered Hello World text";
  }

  void OnEnter(View contentArea) override
  {
    Label label = Label::New("Hello, World!");
    label.SetFontSize(40.0f);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    contentArea.Add(label);
  }
};

REGISTER_MANUAL_TEST(TcHelloWorld)
