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
 * FlexLayout sample with a clickable LayoutDirection toggle.
 *
 * Same content as flexlayout-example. A 200x50 standalone child
 * at the top-left toggles the root view's LayoutDirection between
 * LEFT_TO_RIGHT and RIGHT_TO_LEFT on click. RTL flips direct children
 * horizontally; the standalone toggle button is excluded from mirroring
 * and stays at the top-left.
 *
 */
class TcFlexLayoutDirection : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "64. FlexLayout: Layout Direction";
  }

  Dali::String GetDescription() const override
  {
    return "Verify RTL mirroring while standalone content remains fixed";
  }

  void OnEnter(View contentArea) override
  {
    mIsRtl = false;

    // Root: FlexLayout in row direction
    mRoot = FlexLayout::New();
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetDirection(FlexDirection::ROW);
    mRoot.SetAlignItems(FlexAlign::STRETCH);
    mRoot.SetPadding(Insets(50.0f, 50.0f, 50.0f, 50.0f));

    // Red box: fixed width
    View redBox = View::New();
    redBox.SetBackgroundColor(Color::RED);
    redBox.SetRequestedWidth(100.0f);
    mRoot.Add(redBox);

    // Green box: flex-grow 1 (fills remaining space)
    View greenBox = View::New();
    greenBox.SetBackgroundColor(Color::GREEN);
    greenBox.SetRequestedWidth(WRAP_CONTENT);
    greenBox.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
    mRoot.Add(greenBox);

    // Blue box: fixed width
    View blueBox = View::New();
    blueBox.SetBackgroundColor(Color::BLUE);
    blueBox.SetRequestedWidth(100.0f);
    mRoot.Add(blueBox);

    // Toggle button: 200x50 translucent-black standalone child at top-left
    // with a centered white "Change LayoutDirection" label.
    InteractiveView toggleBtn = InteractiveView::New();
    toggleBtn.SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.5f));
    toggleBtn.SetRequestedWidth(200.0f);
    toggleBtn.SetRequestedHeight(50.0f);
    toggleBtn.SetRequestedX(0.0f);
    toggleBtn.SetRequestedY(0.0f);
    toggleBtn.SetLayoutMode(LayoutMode::STANDALONE);
    toggleBtn.SetLayoutDirection(Dali::LayoutDirection::LEFT_TO_RIGHT);
    Label toggleLabel = Label::New("Change LayoutDirection");
    toggleLabel.SetTextColor(UiColor(1.0f, 1.0f, 1.0f, 1.0f));
    toggleLabel.SetRequestedWidth(MATCH_PARENT);
    toggleLabel.SetRequestedHeight(MATCH_PARENT);
    toggleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    toggleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    toggleBtn.Add(toggleLabel);
    toggleBtn.ConnectClickedSignal(this, [this](View view, InputEvent event) -> bool {
      mIsRtl = !mIsRtl;
      mRoot.SetLayoutDirection(mIsRtl ? Dali::LayoutDirection::RIGHT_TO_LEFT : Dali::LayoutDirection::LEFT_TO_RIGHT);
      return true;
    });
    mRoot.Add(toggleBtn);

    contentArea.Add(mRoot);
  }

  void OnExit() override
  {
    DisconnectAll();
    mRoot.Reset();
  }

private:
  FlexLayout   mRoot;
  bool         mIsRtl{false};
};

REGISTER_MANUAL_TEST(TcFlexLayoutDirection)
