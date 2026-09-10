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
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
Label MakeDialogLabel(const Dali::String& text, float fontSize = 16.0f)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0x202124u));
  return label;
}

const char* FlagText(bool value)
{
  return value ? "true" : "false";
}
} // namespace

// Observe regressions in section styling and label reuse after title changes,
// custom scrim preservation, and restoration of the original explicit modal flag.
class TcDialogBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  ~TcDialogBasics() override
  {
    OnExit();
  }

  Dali::String GetName() const override
  {
    return "Dialog: 스타일과 modal 소유권";
  }

  Dali::String GetDescription() const override
  {
    return "기본/사용자 스타일, 제목 갱신, scrim 교체, modal 제거·재설치와 명시 flag 복원을 관찰합니다.";
  }

  void OnEnter(View contentArea) override
  {
    OnExit();
    // The launcher has already applied Components UiConfig; do not apply it again here.
    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(WRAP_CONTENT);
    mRoot.SetSpacing(12.0f);
    mRoot.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    mRoot.SetBackgroundColor(UiColor(0xF6F7F9u));
    mRoot.Add(MakeDialogLabel("아래로 스크롤해 네 미리보기를 확인하세요. focus·dismiss 정책은 앱 책임입니다."));

    AddControl("제목 갱신", &TcDialogBasics::OnUpdateTitle);
    AddControl("scrim 교체 / 복원", &TcDialogBasics::OnReplaceScrim);
    AddControl("modal 제거 / 재설치", &TcDialogBasics::OnToggleModalContent);
    mStatus = MakeDialogLabel("");
    mStatus.SetMultiLine(true);
    mRoot.Add(mStatus);

    mRoot.Add(MakeDialogLabel("1. 기본 AlertDialog", 18.0f));
    mDefaultAlert = AlertDialog::New();
    mDefaultAlert.SetRequestedWidth(MATCH_PARENT);
    mDefaultAlert.SetTitle("기본 제목");
    mDefaultAlert.SetMessage("무인자 New()의 기본 스타일입니다.");
    mDefaultAlert.SetAccessibilityName("기본 Alert 미리보기");
    mDefaultAlert.AddActionButton("기본 동작").ClickedSignal().Connect(this, &TcDialogBasics::OnDefaultAction);
    mRoot.Add(mDefaultAlert);

    DialogStyle customDialogStyle = DialogStyle::Default().Configure()
                                      .SetRequestedWidth(MATCH_PARENT)
                                      .SetBackgroundColor(UiColor(0xFFFFFFu))
                                      .SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f))
                                      .SetSpacing(12.0f)
                                      .SetCornerRadius(Vector4(12.0f, 12.0f, 12.0f, 12.0f))
                                      .Build();
    mRoot.Add(MakeDialogLabel("2. 사용자 DialogStyle", 18.0f));
    Dialog customDialog = Dialog::New(customDialogStyle);
    auto customHeader = MakeDialogLabel("사용자 header");
    customHeader.SetPadding(Insets(8.0f, 8.0f, 4.0f, 4.0f));
    auto customBody = MakeDialogLabel("body: section 간격 12, 별도 padding");
    customBody.SetPadding(Insets(12.0f, 12.0f, 4.0f, 4.0f));
    customDialog.SetHeaderView(customHeader);
    customDialog.SetBodyView(customBody);
    customDialog.SetFooterView(MakeDialogLabel("footer: 명시 padding 없음"));
    customDialog.SetAccessibilityName("사용자 Dialog 미리보기");
    mRoot.Add(customDialog);

    mRoot.Add(MakeDialogLabel("3. 기본 Container / 원래 explicit modal true", 18.0f));
    mDefaultDialog = Dialog::New();
    mDefaultDialog.SetHeaderView(MakeDialogLabel("기본 Dialog"));
    mDefaultDialog.SetBodyView(MakeDialogLabel("제거 후에도 앱이 지정한 true를 유지해야 합니다."));
    mDefaultDialog.SetAccessibilityName("원래 modal true인 기본 Dialog");
    mDefaultDialog.SetAccessibilityModal(true);
    mDefaultDialog.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(16.0f, 16.0f, 280.0f, 120.0f)));
    mDefaultContainer = DialogContainer::New();
    mDefaultContainer.SetRequestedWidth(MATCH_PARENT);
    mDefaultContainer.SetRequestedHeight(160.0f);
    mDefaultContainer.SetModalContent(mDefaultDialog);
    mDefaultContainer.ScrimClickedSignal().Connect(this, &TcDialogBasics::OnScrimClicked);
    mRoot.Add(mDefaultContainer);

    mRoot.Add(MakeDialogLabel("4. 사용자 Container / 원래 explicit modal false", 18.0f));
    TextButtonStyle actionStyle = TextButtonStyle::Default().Configure()
                                    .SetBackgroundColor(UiColor(0x2455A4u))
                                    .SetTextColor(UiColor(0xFFFFFFu))
                                    .SetFontSize(16.0f)
                                    .Build();
    AlertDialogStyle alertStyle = AlertDialogStyle::Default().Configure()
                                    .SetDialogStyle(customDialogStyle)
                                    .SetTitleFontSize(22.0f)
                                    .SetTitleTextColor(UiColor(0x2455A4u))
                                    .SetMessageFontSize(16.0f)
                                    .SetActionButtonStyle(actionStyle)
                                    .SetActionRowHeight(48.0f)
                                    .SetActionButtonSpacing(8.0f)
                                    .Build();
    mAlert = AlertDialog::New(alertStyle);
    mAlert.SetTitle("사용자 제목");
    mAlert.SetMessage("분리 중에도 제목을 바꿀 수 있습니다.");
    mAlert.SetAccessibilityName("원래 modal false인 사용자 Alert");
    mAlert.SetAccessibilityModal(false);
    mAlert.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(16.0f, 16.0f, 280.0f, 220.0f)));
    mAlert.AddActionButton("사용자 동작").ClickedSignal().Connect(this, &TcDialogBasics::OnCustomAction);
    mOriginalTitleHeader = mAlert.GetHeaderView();

    DialogContainerStyle containerStyle = DialogContainerStyle::Default().Configure()
                                            .SetScrimColor(UiColor(0x204070u, 0.45f))
                                            .SetScrimBlurRadius(0.0f)
                                            .Build();
    mContainer = DialogContainer::New(containerStyle);
    mContainer.SetRequestedWidth(MATCH_PARENT);
    mContainer.SetRequestedHeight(260.0f);
    mOriginalScrim = mContainer.GetScrim();
    mCustomScrim = InteractiveView::New();
    mCustomScrim.SetFocusable(false);
    mCustomScrim.SetFocusOnTouchEnabled(false);
    mCustomScrim.SetAccessibilityHidden(true);
    mCustomScrim.SetBackgroundColor(UiColor(0xF6A23Bu, 0.75f));
    mContainer.SetModalContent(mAlert);
    mContainer.ScrimClickedSignal().Connect(this, &TcDialogBasics::OnScrimClicked);
    mRoot.Add(mContainer);

    ScrollView scroll = ScrollView::New();
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scroll.SetContent(mRoot);
    contentArea.Add(scroll);
    UpdateStatus();
  }

  void OnExit() override
  {
    DisconnectAll();
    // Exercise the container's removal path instead of restoring flags in test code.
    if(mContainer)
    {
      mContainer.SetModalContent({});
    }
    if(mDefaultContainer)
    {
      mDefaultContainer.SetModalContent({});
    }
    mOriginalTitleHeader.Reset();
    mOriginalScrim.Reset();
    mCustomScrim.Reset();
    mAlert.Reset();
    mDefaultAlert.Reset();
    mDefaultDialog.Reset();
    mContainer.Reset();
    mDefaultContainer.Reset();
    mStatus.Reset();
    mRoot.Reset();
    mTitleRevision = 0u;
    mActionCount = 0u;
    mScrimClickCount = 0u;
    mLastAction = "없음";
  }

private:
  using ControlHandler = void (TcDialogBasics::*)(View, InputEvent);

  void AddControl(const Dali::String& text, ControlHandler handler)
  {
    TextButton button = TextButton::New(text);
    button.SetRequestedWidth(MATCH_PARENT);
    button.SetRequestedHeight(48.0f);
    button.ClickedSignal().Connect(this, handler);
    mRoot.Add(button);
  }

  void OnUpdateTitle(View, InputEvent)
  {
    ++mTitleRevision;
    const std::string suffix = " 갱신 " + std::to_string(mTitleRevision);
    mDefaultAlert.SetTitle(Dali::String(("기본 제목" + suffix).c_str()));
    mAlert.SetTitle(Dali::String(("사용자 제목" + suffix).c_str()));
    UpdateStatus();
  }

  void OnReplaceScrim(View, InputEvent)
  {
    mContainer.SetScrim(mContainer.GetScrim() == mOriginalScrim ? View(mCustomScrim) : mOriginalScrim);
    UpdateStatus();
  }

  void OnToggleModalContent(View, InputEvent)
  {
    if(mContainer.GetModalContent() || mDefaultContainer.GetModalContent())
    {
      mContainer.SetModalContent({});
      mDefaultContainer.SetModalContent({});
    }
    else
    {
      mDefaultContainer.SetModalContent(mDefaultDialog);
      mContainer.SetModalContent(mAlert);
    }
    UpdateStatus();
  }

  void OnDefaultAction(View, InputEvent)
  {
    ++mActionCount;
    mLastAction = "기본 동작";
    UpdateStatus();
  }

  void OnCustomAction(View, InputEvent)
  {
    ++mActionCount;
    mLastAction = "사용자 동작";
    UpdateStatus();
  }

  void OnScrimClicked(DialogContainer)
  {
    ++mScrimClickCount;
    // Dismissal is application policy; this test observes clicks and retains the content.
    UpdateStatus();
  }

  void UpdateStatus()
  {
    const std::string status =
      std::string("기본 content 설치: ") + FlagText(mDefaultContainer.GetModalContent() == mDefaultDialog) +
      " / flag: " + FlagText(mDefaultDialog.IsAccessibilityModal()) + " (원래 true)\n" +
      "사용자 content 설치: " + FlagText(mContainer.GetModalContent() == mAlert) +
      " / flag: " + FlagText(mAlert.IsAccessibilityModal()) + " (원래 false)\n" +
      "scrim: " + (mContainer.GetScrim() == mOriginalScrim ? "generated" : "custom") +
      " / scrim 클릭: " + std::to_string(mScrimClickCount) + "\n" +
      "제목 갱신: " + std::to_string(mTitleRevision) +
      " / 제목 객체 유지: " + FlagText(mAlert.GetHeaderView() == mOriginalTitleHeader) + "\n" +
      "action 클릭: " + std::to_string(mActionCount) + " / 마지막: " + mLastAction;
    mStatus.SetText(Dali::String(status.c_str()));
  }

  StackLayout     mRoot;
  Label           mStatus;
  Dialog          mDefaultDialog;
  AlertDialog     mDefaultAlert;
  AlertDialog     mAlert;
  DialogContainer mDefaultContainer;
  DialogContainer mContainer;
  View            mOriginalTitleHeader;
  View            mOriginalScrim;
  InteractiveView mCustomScrim;
  uint32_t        mTitleRevision{0u};
  uint32_t        mActionCount{0u};
  uint32_t        mScrimClickCount{0u};
  std::string     mLastAction{"없음"};
};

REGISTER_MANUAL_TEST(TcDialogBasics)
