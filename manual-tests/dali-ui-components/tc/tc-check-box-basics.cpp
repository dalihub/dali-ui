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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float    CHECKBOX_SIZE = 56.0f; // >= default box 36 + padding 8+8
constexpr uint32_t COLOR_BODY    = 0x5F6368;
constexpr uint32_t COLOR_STATUS  = 0x137333;

Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}
} // namespace

// Interactive checks for Ui::CheckBox: tap to toggle and observe the Lottie
// check/uncheck animation, labels, pre-checked, read-only (click ignored), and the
// three SelectionAnimationMode values. The CheckBox has no default outer size, so each is
// sized by this test; the default style draws a fixed 36 glyph inside 8 padding.
class TcCheckBoxBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "CheckBox: basics";
  }

  Dali::String GetDescription() const override
  {
    return "Tap each checkbox to toggle it and watch the animation. Covers box-only, "
           "labelled, pre-checked, read-only (click ignored), and always/never animate.";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(WRAP_CONTENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    mRoot = root;

    root.Add(MakeLabel("Tap each checkbox to toggle it.", 14.0f, COLOR_BODY));

    // Status line — created up front so the pre-checked SetSelected below can safely
    // update it (the SelectionChangedSignal fires during OnEnter()).
    mStatus = MakeLabel("Tap a checkbox…", 14.0f, COLOR_STATUS);

    // Box-only (default AUTO animation).
    CheckBox boxOnly = CheckBox::New();
    boxOnly.SetRequestedWidth(CHECKBOX_SIZE);
    boxOnly.SetRequestedHeight(CHECKBOX_SIZE);
    boxOnly.SelectionChangedSignal().Connect(this, &TcCheckBoxBasics::OnSelectionChanged);
    root.Add(boxOnly);

    MakeCheckBox("Labelled (AUTO animate)");

    CheckBox preChecked = MakeCheckBox("Pre-checked");
    preChecked.SetSelected(true);

    CheckBox readOnly = MakeCheckBox("Read-only (click ignored)");
    readOnly.SetToggleByClickEnabled(false);

    CheckBox always = MakeCheckBox("Always animate");
    always.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);

    CheckBox never = MakeCheckBox("Never animate");
    never.SetSelectionAnimationMode(SelectionAnimationMode::DISABLED);

    root.Add(mStatus);

    contentArea.Add(root);
  }

private:
  CheckBox MakeCheckBox(const Dali::String& text)
  {
    CheckBox cb = CheckBox::New(text);
    cb.SetRequestedWidth(WRAP_CONTENT);
    cb.SetRequestedHeight(CHECKBOX_SIZE);
    cb.SelectionChangedSignal().Connect(this, &TcCheckBoxBasics::OnSelectionChanged);
    mRoot.Add(cb);
    return cb;
  }

  void OnSelectionChanged(View view, bool selected, InputEvent /*event*/)
  {
    if(!mStatus)
    {
      return; // status line not built yet
    }
    CheckBox     cb   = CheckBox::DownCast(view);
    Dali::String name = (cb && !cb.GetText().Empty()) ? cb.GetText() : Dali::String("(box-only)");
    mStatus.SetText(name + (selected ? " : checked" : " : unchecked"));
  }

  StackLayout mRoot;
  Label       mStatus;
};

REGISTER_MANUAL_TEST(TcCheckBoxBasics)
