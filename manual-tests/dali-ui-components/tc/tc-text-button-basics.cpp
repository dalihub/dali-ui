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

const char* TARGET_SHORT_TEXT = "TextButton";
const char* TARGET_LONG_TEXT  = "TextButton TextButton TextButton TextButton TextButton TextButton";

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
  section.SetPadding(Insets(SECTION_PADDING, SECTION_PADDING, SECTION_PADDING, SECTION_PADDING));
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
  frame.SetPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f));
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
      return "START";
    case LayoutAlignment::CENTER:
      return "CENTER";
    case LayoutAlignment::END:
      return "END";
    case LayoutAlignment::FILL:
      return "FILL";
  }
  return "UNKNOWN";
}

} // namespace

class TcTextButtonBasics : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "TextButton Basics";
  }

  Dali::String GetDescription() const override
  {
    return "Checks alignment, dynamic text, wrap, fixed, and min/max sizing without TextButtonStyle";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetPadding(Insets(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
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
    StackLayout section = MakeSection("1. Dynamic alignment", "Tap the control to cycle START / CENTER / END. The text should move inside the fixed-width TextButton.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetRequestedWidth(MATCH_PARENT);
    demo.SetRequestedHeight(64.0f);
    demo.SetMinimumHeight(0.0f);

    Label state = MakeLabel("Horizontal: CENTER", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("Change horizontal alignment");
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
      state.SetText(Dali::String("Horizontal: ") + AlignmentName(alignment));
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
    StackLayout section = MakeSection("2. WRAP_CONTENT dynamic text", "The TextButton uses WRAP_CONTENT. Tap the control to switch between short and long text; the rendered size should follow the text.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    Label state = MakeLabel("Text: short", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("Toggle wrap text length");
    bool longText = false;
    control.ClickedSignal().Connect(this, [demo, state, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      state.SetText(longText ? "Text: long" : "Text: short");
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
    StackLayout section = MakeSection("3. Min/max constrained dynamic size", "The TextButton has min 160x48 and max 260x80. Cycle requested size below min, within range, and above max, then compare actual size with the requested guide.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetMinimumWidth(160.0f);
    demo.SetMinimumHeight(48.0f);
    demo.SetMaximumWidth(260.0f);
    demo.SetMaximumHeight(80.0f);
    demo.SetRequestedWidth(100.0f);
    demo.SetRequestedHeight(32.0f);

    Label state = MakeLabel("Requested: 100 x 32; expected min clamp 160 x 48", 13.0f, COLOR_BODY);
    Label textState = MakeLabel("Text: short", 13.0f, COLOR_BODY);
    StackLayout requestedGuide = MakeSizeGuide("REQUESTED 100 x 32", 100.0f, 32.0f, COLOR_GUIDE_REQ);
    Label requestedLabel = Label::DownCast(requestedGuide.GetChildViewAt(0u));

    TextButton control = MakeControlButton("Cycle requested size");
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
        requestedLabel.SetText("REQUESTED 100 x 32");
        state.SetText("Requested: 100 x 32; expected min clamp 160 x 48");
      }
      else if(mode == 1)
      {
        demo.SetRequestedWidth(220.0f);
        demo.SetRequestedHeight(64.0f);
        requestedGuide.SetRequestedWidth(220.0f);
        requestedGuide.SetRequestedHeight(64.0f);
        requestedLabel.SetText("REQUESTED 220 x 64");
        state.SetText("Requested: 220 x 64");
      }
      else
      {
        demo.SetRequestedWidth(340.0f);
        demo.SetRequestedHeight(120.0f);
        requestedGuide.SetRequestedWidth(340.0f);
        requestedGuide.SetRequestedHeight(120.0f);
        requestedLabel.SetText("REQUESTED 340 x 120");
        state.SetText("Requested: 340 x 120; expected max clamp 260 x 80");
      }
    });

    TextButton textControl = MakeControlButton("Toggle min/max text length");
    bool longText = false;
    textControl.ClickedSignal().Connect(this, [demo, textState, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      textState.SetText(longText ? "Text: long" : "Text: short");
    });

    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeLabel("Size guides", 13.0f, COLOR_BODY));
    section.Add(MakeSizeGuide("MIN 160 x 48", 160.0f, 48.0f, COLOR_GUIDE_MIN));
    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeSizeGuide("MAX 260 x 80", 260.0f, 80.0f, COLOR_GUIDE_MAX));
    section.Add(MakeGap(ROW_GAP));
    section.Add(requestedGuide);
    section.Add(MakeGap(ROW_GAP));
    section.Add(MakeLabel("Target TextButton", 13.0f, COLOR_BODY));
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
    StackLayout section = MakeSection("4. Fixed-size dynamic text", "The TextButton is fixed at 240x64. Tap the control to switch short/long text; the button bounds should stay fixed.");

    TextButton demo = MakeDemoButton(TARGET_SHORT_TEXT);
    demo.SetRequestedWidth(240.0f);
    demo.SetRequestedHeight(64.0f);
    demo.SetMinimumHeight(0.0f);

    Label state = MakeLabel("Text: short; size: 240 x 64", 13.0f, COLOR_BODY);
    TextButton control = MakeControlButton("Toggle fixed-size text length");
    bool longText = false;
    control.ClickedSignal().Connect(this, [demo, state, longText](View, InputEvent) mutable
    {
      longText = !longText;
      demo.SetText(longText ? TARGET_LONG_TEXT : TARGET_SHORT_TEXT);
      state.SetText(longText ? "Text: long; size should remain 240 x 64" : "Text: short; size: 240 x 64");
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
