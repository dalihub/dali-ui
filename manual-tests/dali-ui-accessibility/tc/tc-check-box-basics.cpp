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
    return "체크박스 기본 동작";
  }

  Dali::String GetDescription() const override
  {
    return "각 체크박스를 눌러 선택 상태와 애니메이션을 확인합니다. 레이블 없음, "
           "레이블 있음, 미리 선택됨, 읽기 전용, 항상 애니메이션, 애니메이션 사용 안 함 항목을 포함합니다.";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(WRAP_CONTENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Extents(16, 16, 16, 16));
    mRoot = root;

    root.Add(MakeLabel("각 체크박스를 눌러 선택 상태를 바꿔 보세요.", 14.0f, COLOR_BODY));

    // Status line — created up front so the pre-checked SetSelected below can safely
    // update it (the SelectionChangedSignal fires during OnEnter()).
    mStatus = MakeLabel("체크박스를 눌러 보세요…", 14.0f, COLOR_STATUS);

    // Box-only (default AUTO animation).
    CheckBox boxOnly = CheckBox::New();
    boxOnly.SetRequestedWidth(CHECKBOX_SIZE);
    boxOnly.SetRequestedHeight(CHECKBOX_SIZE);

    // A checkbox without displayed text gives the component no basis for inferring a name.
    // Provide an explicit name and description that explain the test intent, so TV Screen
    // Reader does not announce this item merely as an empty "checkbox".
    boxOnly.SetAccessibilityName("레이블 없는 체크박스");
    boxOnly.SetAccessibilityDescription("기본 자동 선택 애니메이션을 사용합니다.");
    boxOnly.SelectionChangedSignal().Connect(this, &TcCheckBoxBasics::OnSelectionChanged);
    root.Add(boxOnly);

    MakeCheckBox("레이블 있음 (자동 애니메이션)");

    CheckBox preChecked = MakeCheckBox("미리 선택됨");
    preChecked.SetSelected(true);

    CheckBox readOnly = MakeCheckBox("읽기 전용 (클릭 무시)");

    // This item must also announce why it does not operate, so users do not mistake it for
    // a disabled-control issue. Retain the displayed-text fallback for its name and add a description.
    readOnly.SetAccessibilityDescription("읽기 전용 항목이며, 실행해도 선택 상태가 바뀌지 않습니다.");
    readOnly.SetToggleByClickEnabled(false);

    CheckBox always = MakeCheckBox("항상 애니메이션 사용");
    always.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);

    CheckBox never = MakeCheckBox("애니메이션 사용 안 함");
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
    Dali::String name = (cb && !cb.GetText().Empty()) ? cb.GetText() : Dali::String("레이블 없음");
    mStatus.SetText(name + (selected ? " : 선택됨" : " : 선택 해제됨"));
  }

  StackLayout mRoot;
  Label       mStatus;
};

REGISTER_MANUAL_TEST(TcCheckBoxBasics)
