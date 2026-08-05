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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_PAGE_BG    = 0xF6F7F9;
constexpr uint32_t COLOR_SECTION_BG = 0xFFFFFF;
constexpr uint32_t COLOR_TITLE      = 0x202124;
constexpr uint32_t COLOR_BODY       = 0x5F6368;
constexpr uint32_t COLOR_CONTROL_TXT = 0xFFFFFF;
constexpr uint32_t COLOR_TARGET_BG = 0xE6F4EA;
constexpr uint32_t COLOR_TARGET_TXT = 0x137333;
constexpr uint32_t COLOR_FRAME      = 0xDADCE0;
constexpr uint32_t COLOR_GUIDE_MIN  = 0xFCE8E6;
constexpr uint32_t COLOR_GUIDE_MAX  = 0xFEF7E0;
constexpr uint32_t COLOR_GUIDE_REQ  = 0xE8EAED;

constexpr float PAGE_PADDING    = 16.0f;
constexpr float SECTION_PADDING = 14.0f;
constexpr float SECTION_GAP     = 14.0f;
constexpr float ROW_GAP         = 8.0f;
constexpr float CONTROL_HEIGHT  = 44.0f;
constexpr float DEMO_HEIGHT     = 56.0f;

const char* TARGET_SHORT_TEXT = "텍스트 버튼";
const char* TARGET_LONG_TEXT  = "텍스트 버튼 긴 문구 텍스트 버튼 긴 문구 텍스트 버튼 긴 문구";

Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}

TextButton MakeControlButton(const Dali::String& text)
{
  TextButton button = TextButton::New(text);
  button.SetRequestedWidth(MATCH_PARENT);
  // button.SetRequestedHeight(CONTROL_HEIGHT);
  // button.SetTextColor(UiColor(COLOR_CONTROL_TXT));
  // button.SetFontSize(15.0f);
  // button.SetHorizontalAlignment(LayoutAlignment::CENTER);
  // button.SetVerticalAlignment(LayoutAlignment::CENTER);
  // button.SetFocusable(true);
  button.SetStateEffect(OverlayEffect::ListItem());
  return button;
}

TextButton MakeDemoButton(const Dali::String& text)
{
  TextButton button = TextButton::New(text);
  button.SetRequestedWidth(WRAP_CONTENT);
  button.SetRequestedHeight(WRAP_CONTENT);
  button.SetMinimumHeight(DEMO_HEIGHT);
  button.SetBackgroundColor(UiColor(COLOR_TARGET_BG));
  button.SetTextColor(UiColor(COLOR_TARGET_TXT));
  button.SetFontSize(18.0f);
  button.SetHorizontalAlignment(LayoutAlignment::CENTER);
  button.SetVerticalAlignment(LayoutAlignment::CENTER);
  button.SetFocusable(true);
  button.SetStateEffect(OverlayEffect::ListItem());
  return button;
}

StackLayout MakeSection(const Dali::String& title, const Dali::String& description)
{
  StackLayout section = StackLayout::New(StackOrientation::VERTICAL);
  section.SetRequestedWidth(MATCH_PARENT);
  section.SetRequestedHeight(WRAP_CONTENT);
  section.SetPadding(Extents(SECTION_PADDING, SECTION_PADDING, SECTION_PADDING, SECTION_PADDING));
  section.SetBackgroundColor(UiColor(COLOR_SECTION_BG));
  section.Add(MakeLabel(title, 18.0f, COLOR_TITLE));
  section.Add(MakeLabel(description, 13.0f, COLOR_BODY));
  return section;
}

View MakeGap(float height)
{
  View gap = View::New();
  gap.SetRequestedWidth(MATCH_PARENT);
  gap.SetRequestedHeight(height);
  return gap;
}

View MakeFrame(View child)
{
  StackLayout frame = StackLayout::New(StackOrientation::VERTICAL);
  frame.SetRequestedWidth(MATCH_PARENT);
  frame.SetRequestedHeight(WRAP_CONTENT);
  frame.SetMinimumHeight(78.0f);
  frame.SetPadding(Extents(8u, 8u, 8u, 8u));
  frame.SetBackgroundColor(UiColor(COLOR_FRAME));
  frame.Add(child);
  return frame;
}

Label MakeCenteredLabel(const Dali::String& text)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(MATCH_PARENT);
  label.SetFontSize(12.0f);
  label.SetTextColor(UiColor(COLOR_BODY));
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

StackLayout MakeSizeGuide(const Dali::String& text, float width, float height, uint32_t color)
{
  StackLayout guide = StackLayout::New(StackOrientation::VERTICAL);
  guide.SetRequestedWidth(width);
  guide.SetRequestedHeight(height);
  guide.SetBackgroundColor(UiColor(color));
  guide.Add(MakeCenteredLabel(text));
  return guide;
}

const char* AlignmentName(LayoutAlignment alignment)
{
  switch(alignment)
  {
    case LayoutAlignment::START:
      return "시작";
    case LayoutAlignment::CENTER:
      return "가운데";
    case LayoutAlignment::END:
      return "끝";
    case LayoutAlignment::FILL:
      return "채우기";
  }
  return "알 수 없음";
}

} // namespace

class TcTextButtonBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "텍스트 버튼 기본 동작";
  }

  Dali::String GetDescription() const override
  {
    return "텍스트 버튼의 정렬, 동적 문구, 내용 맞춤, 고정 크기, 최소/최대 크기를 확인합니다.";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetPadding(Extents(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    content.SetBackgroundColor(UiColor(COLOR_PAGE_BG));

    AddAlignmentSection(content);
    AddWrapTextSection(content);
    AddMinMaxSection(content);
    AddFixedSizeSection(content);

    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    scrollView.SetContent(content);
    contentArea.Add(scrollView);
  }

private:
  void AddAlignmentSection(StackLayout content)
  {
    StackLayout section = MakeSection("1. 동적 정렬", "버튼을 누르면 시작, 가운데, 끝 순서로 정렬이 바뀝니다. 고정 너비 안에서 문구가 이동하는지 확인하세요.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetRequestedWidth(MATCH_PARENT);
    demo.SetRequestedHeight(64.0f);
    demo.SetMinimumHeight(0.0f);

    Label state = MakeLabel("가로 정렬: 가운데", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("가로 정렬 변경");
    LayoutAlignment alignment = LayoutAlignment::CENTER;
    control.ClickedSignal().Connect(this, [demo, state, alignment](View, InputEvent) mutable
    {
      if(alignment == LayoutAlignment::START)
      {
        alignment = LayoutAlignment::CENTER;
      }
      else if(alignment == LayoutAlignment::CENTER)
      {
        alignment = LayoutAlignment::END;
      }
      else
      {
        alignment = LayoutAlignment::START;
      }

      demo.SetHorizontalAlignment(alignment);
      state.SetText(Dali::String("가로 정렬: ") + AlignmentName(alignment));
    });

    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeFrame(demo));
    section.Add(MakeGap(ROW_GAP));
    section.Add(state);
    section.Add(control);
    content.Add(section);
    content.Add(MakeGap(SECTION_GAP));
  }

  void AddWrapTextSection(StackLayout content)
  {
    StackLayout section = MakeSection("2. 내용에 맞춘 동적 문구", "텍스트 버튼의 크기가 내용에 맞게 결정됩니다. 버튼을 눌러 짧은 문구와 긴 문구를 전환하고 크기 변화를 확인하세요.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    Label state = MakeLabel("현재 문구: 짧은 문구", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("내용 맞춤 문구 길이 전환");
    bool longText = false;
    control.ClickedSignal().Connect(this, [demo, state, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      state.SetText(longText ? "현재 문구: 긴 문구" : "현재 문구: 짧은 문구");
    });

    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeFrame(demo));
    section.Add(MakeGap(ROW_GAP));
    section.Add(state);
    section.Add(control);
    content.Add(section);
    content.Add(MakeGap(SECTION_GAP));
  }

  void AddMinMaxSection(StackLayout content)
  {
    StackLayout section = MakeSection("3. 최소/최대 범위가 있는 동적 크기", "텍스트 버튼의 최소 크기는 160 × 48, 최대 크기는 260 × 80입니다. 요청 크기를 바꾸며 실제 크기와 안내 영역을 비교하세요.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetMinimumWidth(160.0f);
    demo.SetMinimumHeight(48.0f);
    demo.SetMaximumWidth(260.0f);
    demo.SetMaximumHeight(80.0f);
    demo.SetRequestedWidth(100.0f);
    demo.SetRequestedHeight(32.0f);

    Label state = MakeLabel("요청 크기: 100 × 32, 예상 최소 제한: 160 × 48", 13.0f, COLOR_BODY);
    Label textState = MakeLabel("현재 문구: 짧은 문구", 13.0f, COLOR_BODY);
    StackLayout requestedGuide = MakeSizeGuide("요청 100 × 32", 100.0f, 32.0f, COLOR_GUIDE_REQ);
    Label requestedLabel = Label::DownCast(requestedGuide.GetChildViewAt(0u));

    TextButton control = MakeControlButton("요청 크기 순환 변경");
    int mode = 0;
    control.ClickedSignal().Connect(this, [demo, state, requestedGuide, requestedLabel, mode](View, InputEvent) mutable
    {
      mode = (mode + 1) % 3;
      if(mode == 0)
      {
        demo.SetRequestedWidth(100.0f);
        demo.SetRequestedHeight(32.0f);
        requestedGuide.SetRequestedWidth(100.0f);
        requestedGuide.SetRequestedHeight(32.0f);
        requestedLabel.SetText("요청 100 × 32");
        state.SetText("요청 크기: 100 × 32, 예상 최소 제한: 160 × 48");
      }
      else if(mode == 1)
      {
        demo.SetRequestedWidth(220.0f);
        demo.SetRequestedHeight(64.0f);
        requestedGuide.SetRequestedWidth(220.0f);
        requestedGuide.SetRequestedHeight(64.0f);
        requestedLabel.SetText("요청 220 × 64");
        state.SetText("요청 크기: 220 × 64");
      }
      else
      {
        demo.SetRequestedWidth(340.0f);
        demo.SetRequestedHeight(120.0f);
        requestedGuide.SetRequestedWidth(340.0f);
        requestedGuide.SetRequestedHeight(120.0f);
        requestedLabel.SetText("요청 340 × 120");
        state.SetText("요청 크기: 340 × 120, 예상 최대 제한: 260 × 80");
      }
    });

    TextButton textControl = MakeControlButton("최소/최대 영역 문구 길이 전환");
    bool longText = false;
    textControl.ClickedSignal().Connect(this, [demo, textState, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      textState.SetText(longText ? "현재 문구: 긴 문구" : "현재 문구: 짧은 문구");
    });

    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeLabel("크기 안내", 13.0f, COLOR_BODY));
    section.Add(MakeSizeGuide("최소 160 × 48", 160.0f, 48.0f, COLOR_GUIDE_MIN));
    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeSizeGuide("최대 260 × 80", 260.0f, 80.0f, COLOR_GUIDE_MAX));
    section.Add(MakeGap(ROW_GAP));
    section.Add(requestedGuide);
    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeLabel("대상 텍스트 버튼", 13.0f, COLOR_BODY));
    section.Add(demo);
    section.Add(MakeGap(ROW_GAP));
    section.Add(state);
    section.Add(textState);
    section.Add(control);
    section.Add(MakeGap(ROW_GAP));
    section.Add(textControl);
    content.Add(section);
    content.Add(MakeGap(SECTION_GAP));
  }

  void AddFixedSizeSection(StackLayout content)
  {
    StackLayout section = MakeSection("4. 고정 크기의 동적 문구", "텍스트 버튼은 240 × 64로 고정됩니다. 짧은 문구와 긴 문구를 전환해도 버튼 영역이 유지되는지 확인하세요.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetRequestedWidth(240.0f);
    demo.SetRequestedHeight(64.0f);
    demo.SetMinimumHeight(0.0f);

    Label state = MakeLabel("현재 문구: 짧은 문구, 크기: 240 × 64", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("고정 크기 문구 길이 전환");
    bool longText = false;
    control.ClickedSignal().Connect(this, [demo, state, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      state.SetText(longText ? "현재 문구: 긴 문구, 크기는 240 × 64로 유지되어야 합니다." : "현재 문구: 짧은 문구, 크기: 240 × 64");
    });

    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeFrame(demo));
    section.Add(MakeGap(ROW_GAP));
    section.Add(state);
    section.Add(control);
    content.Add(section);
  }
};

REGISTER_MANUAL_TEST(TcTextButtonBasics)
