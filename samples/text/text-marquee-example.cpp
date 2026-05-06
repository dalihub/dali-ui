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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 5.0f;
constexpr float STACK_PADDING = 10.0f;
const char*     TEXT_LTR      = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id congue odio.";
const char*     TEXT_RTL      = "שלום עולם, נעים מאוד,ומקווה שיהיה לנו שיחה נעימה וטובה יחד";

} // namespace

class TextController : public ConnectionTracker
{
public:
  explicit TextController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Text Marquee Example\n"
                   "Test options:\n"
                   "[1] Start Marquee, [2] Stop Marquee, [i] Information\n"
                   "Text::MarqueeOrientation::HORIZONTAL")
          .SetFontSize(14.0f)
          .SetMultiLine(true)
          .SetLineHeight(1.6f)
          .SetFocusable(true)
          .SetTouchFocusable(true),

        StackLayout::New(StackOrientation::VERTICAL)
          .SetRequestedWidth(MATCH_PARENT)
          .SetSpacing(STACK_SPACING)
          .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .Children({
            Label::New("MarqueeTriggerPolicy::MANUAL")
              .SetFontSize(14.0f),

            CreateMarqueeLabel(TEXT_LTR, /* multiline */ false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL)
              .As(mLabelLtr),

            CreateMarqueeLabel(TEXT_RTL, /* multiline */ false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL)
              .As(mLabelRtl),

            Label::New("Text::MarqueeOrientation::VERTICAL")
              .SetFontSize(14.0f),
            CreateMarqueeLabel(TEXT_LTR, /* multiline */ true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL)
              .As(mLabelMultiline),

            Label::New("When focused, the marquee runs if the text is long")
              .SetFontSize(14.0f),
            CreateMarqueeLabel(TEXT_LTR, /* multiline */ false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL)
              .SetFocusable(true)
              .SetTouchFocusable(true)
              .As(mLabelFocus),
          }),

        StackLayout::New(StackOrientation::VERTICAL)
          .SetRequestedWidth(MATCH_PARENT)
          .SetSpacing(STACK_SPACING)
          .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .Children({
            Label::New("MarqueeTriggerPolicy::ON_OVERFLOW")
              .SetFontSize(14.0f),

            CreateMarqueeLabel(TEXT_LTR, /* multiline */ false, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::HORIZONTAL)
              .As(mOverflowLabel),

            CreateMarqueeLabel(TEXT_RTL, /* multiline */ false, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::HORIZONTAL)
              .As(mOverflowLabelRtl),

            Label::New("Text::MarqueeOrientation::VERTICAL")
              .SetFontSize(14.0f),
            CreateMarqueeLabel(TEXT_LTR, /* multiline */ true, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::VERTICAL)
              .As(mOverflowLabelMultiline),

            Label::New("When focused, the marquee runs if the text is long")
              .SetFontSize(14.0f),
            CreateMarqueeLabel(TEXT_LTR, /* multiline */ false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL)
              .SetFocusable(true)
              .SetTouchFocusable(true)
              .As(mOverflowLabelFocus),
          }),

        StackLayout::New(StackOrientation::HORIZONTAL)
          .SetRequestedWidth(MATCH_PARENT)
          .SetSpacing(STACK_SPACING)
          .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .Children({
            // Vertical alignment test
            CreateMarqueeLabel(TEXT_LTR, /* multiline */ true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL)
              .SetRequestedHeight(200)
              .SetMaximumHeight(200)
              .SetVerticalTextAlignment(Text::Alignment::START)
              .As(mLabelVerticalStart),

            CreateMarqueeLabel(TEXT_LTR, /* multiline */ true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL)
              .SetRequestedHeight(200)
              .SetMaximumHeight(200)
              .SetVerticalTextAlignment(Text::Alignment::CENTER)
              .As(mLabelVerticalCenter),

            CreateMarqueeLabel(TEXT_LTR, /* multiline */ true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL)
              .SetRequestedHeight(200)
              .SetMaximumHeight(200)
              .SetVerticalTextAlignment(Text::Alignment::END)
              .As(mLabelVerticalEnd),
          }),
      }));

    mLabelFocus.FocusChangedSignal().Connect(this, &TextController::OnFocusChanged);
    mOverflowLabelFocus.FocusChangedSignal().Connect(this, &TextController::OnOverflowFocusChanged);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  Label CreateMarqueeLabel(Dali::String text, bool multiLine, Text::MarqueeTriggerPolicy policy, Text::MarqueeOrientation orientation)
  {
    auto label = Label::New(text)
                   .SetTextColor(UiColor(0xFFFFFF))
                   .SetBackgroundColor(UiColor(0x000000))
                   .SetRequestedWidth(MATCH_PARENT)
                   .SetPadding(Extents(10, 10, 10, 10))
                   .SetMultiLine(multiLine)
                   .SetMarqueeTriggerPolicy(policy)
                   .SetMarqueeOrientation(orientation);

    if(multiLine)
    {
      // for overflow test.
      label.SetRequestedWidth(WRAP_CONTENT);
      label.SetMaximumWidth(200);
      label.SetMaximumHeight(100);
    }
    return label;
  }

  void OnFocusChanged(View view, bool focused)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      if(focused)
      {
        if(label.GetNaturalSize().width > label.GetSize().GetWidth())
        {
          label.StartMarquee();
        }
      }
      else
      {
        label.StopMarquee();
      }
    }
  }

  void OnOverflowFocusChanged(View view, bool focused)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      if(focused)
      {
        label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
      }
      else
      {
        label.StopMarquee();
        label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
      }
    }
  }

  void PrintLabelInfo(Label label, const char* title)
  {

  }

  void OnKeyEvent(KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      StartAllMarquee();
    }
    else if(event.GetKeyName() == "2")
    {
      StopAllMarquee();
    }
    else if(event.GetKeyName() == "3")
    {
      SetAsyncRenderingForAll(true);
    }
    else if(event.GetKeyName() == "4")
    {
      SetAsyncRenderingForAll(false);
    }
    else if(event.GetKeyName() == "q")
    {
      SetOverflowFontSizeScale(1.5f);
    }
    else if(event.GetKeyName() == "w")
    {
      SetOverflowFontSizeScale(1.0f);
    }
    else if(event.GetKeyName() == "a")
    {
      SetOverflowVisibility(false);
    }
    else if(event.GetKeyName() == "s")
    {
      SetOverflowVisibility(true);
    }
    else if(event.GetKeyName() == "i")
    {
      PrintAllLabelInfo();
    }
  }

  void StartAllMarquee()
  {
    mLabelLtr.StartMarquee();
    mLabelRtl.StartMarquee();
    mLabelMultiline.StartMarquee();
    mOverflowLabel.StartMarquee();
    mOverflowLabelRtl.StartMarquee();
    mOverflowLabelMultiline.StartMarquee();
    mLabelVerticalStart.StartMarquee();
    mLabelVerticalCenter.StartMarquee();
    mLabelVerticalEnd.StartMarquee();
  }

  void StopAllMarquee()
  {
    mLabelLtr.StopMarquee();
    mLabelRtl.StopMarquee();
    mLabelMultiline.StopMarquee();
    mOverflowLabel.StopMarquee();
    mOverflowLabelRtl.StopMarquee();
    mOverflowLabelMultiline.StopMarquee();
    mLabelVerticalStart.StopMarquee();
    mLabelVerticalCenter.StopMarquee();
    mLabelVerticalEnd.StopMarquee();
  }

  void SetAsyncRenderingForAll(bool enabled)
  {
    mLabelLtr.SetAsyncRendering(enabled);
    mLabelRtl.SetAsyncRendering(enabled);
    mLabelMultiline.SetAsyncRendering(enabled);
    mLabelFocus.SetAsyncRendering(enabled);
    mOverflowLabel.SetAsyncRendering(enabled);
    mOverflowLabelRtl.SetAsyncRendering(enabled);
    mOverflowLabelMultiline.SetAsyncRendering(enabled);
    mOverflowLabelFocus.SetAsyncRendering(enabled);
    mLabelVerticalStart.SetAsyncRendering(enabled);
    mLabelVerticalCenter.SetAsyncRendering(enabled);
    mLabelVerticalEnd.SetAsyncRendering(enabled);
  }

  void SetOverflowFontSizeScale(float scale)
  {
    mOverflowLabel.SetFontSizeScale(scale);
    mOverflowLabelRtl.SetFontSizeScale(scale);
    mOverflowLabelMultiline.SetFontSizeScale(scale);
    mOverflowLabelFocus.SetFontSizeScale(scale);
  }

  void SetOverflowVisibility(bool visible)
  {
    mOverflowLabel.SetVisibility(visible);
    mOverflowLabelRtl.SetVisibility(visible);
    mOverflowLabelMultiline.SetVisibility(visible);
    mOverflowLabelFocus.SetVisibility(visible);
  }

  void PrintAllLabelInfo()
  {
    PrintLabelInfo(mLabelLtr, "LTR Marquee");
    PrintLabelInfo(mLabelRtl, "RTL Marquee");
    PrintLabelInfo(mLabelMultiline, "Multiline Marquee");
    PrintLabelInfo(mLabelFocus, "Focus Marquee");
    PrintLabelInfo(mOverflowLabel, "LTR Marquee Overflow");
    PrintLabelInfo(mOverflowLabelRtl, "RTL Marquee Overflow");
    PrintLabelInfo(mOverflowLabelMultiline, "Multiline Marquee Overflow");
    PrintLabelInfo(mOverflowLabelFocus, "Focus Marquee Overflow");
  }

private:
  Application& mApplication;
  Label        mLabelLtr;
  Label        mLabelRtl;
  Label        mLabelMultiline;
  Label        mLabelFocus;
  Label        mOverflowLabel;
  Label        mOverflowLabelRtl;
  Label        mOverflowLabelMultiline;
  Label        mOverflowLabelFocus;
  Label        mLabelVerticalStart;
  Label        mLabelVerticalCenter;
  Label        mLabelVerticalEnd;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New()
    .SetLabelAsyncRendering(true)
    .SetMarqueeLoopCount(1)
    .Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
