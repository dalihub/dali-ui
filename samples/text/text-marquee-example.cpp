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
#include <dali/devel-api/actors/actor-devel.h>
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
  : mApplication(application),
    mMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));

    Label title = Label::New("Text Marquee Example\n"
                             "Test options:\n"
                             "[1] Start Marquee, [2] Stop Marquee, [d] Toggle Stop Mode, [i] Information\n"
                             "Text::MarqueeOrientation::HORIZONTAL");
    title.SetFontSize(14.0f);
    title.SetMultiLine(true);
    title.SetLineHeight(1.6f);
    title.SetFocusable(true);
    title.SetFocusOnTouchEnabled(true);

    StackLayout manualSection = StackLayout::New(StackOrientation::VERTICAL);
    manualSection.SetRequestedWidth(MATCH_PARENT);
    manualSection.SetSpacing(STACK_SPACING);
    manualSection.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    manualSection.SetBackgroundColor(UiColor(0xEFEFEF));
    Label manualTitle = Label::New("MarqueeTriggerPolicy::MANUAL");
    manualTitle.SetFontSize(14.0f);
    Label verticalTitle = Label::New("Text::MarqueeOrientation::VERTICAL");
    verticalTitle.SetFontSize(14.0f);
    Label focusTitle = Label::New("When focused, the marquee runs if the text is long");
    focusTitle.SetFontSize(14.0f);
    mLabelLtr       = CreateMarqueeLabel(TEXT_LTR, false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL);
    mLabelRtl       = CreateMarqueeLabel(TEXT_RTL, false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL);
    mLabelMultiline = CreateMarqueeLabel(TEXT_LTR, true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL);
    mLabelFocus     = CreateMarqueeLabel(TEXT_LTR, false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL);
    mLabelFocus.SetFocusable(true);
    mLabelFocus.SetFocusOnTouchEnabled(true);
    manualSection.Add(manualTitle);
    manualSection.Add(mLabelLtr);
    manualSection.Add(mLabelRtl);
    manualSection.Add(verticalTitle);
    manualSection.Add(mLabelMultiline);
    manualSection.Add(focusTitle);
    manualSection.Add(mLabelFocus);

    StackLayout overflowSection = StackLayout::New(StackOrientation::VERTICAL);
    overflowSection.SetRequestedWidth(MATCH_PARENT);
    overflowSection.SetSpacing(STACK_SPACING);
    overflowSection.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    overflowSection.SetBackgroundColor(UiColor(0xEFEFEF));
    Label overflowTitle = Label::New("MarqueeTriggerPolicy::ON_OVERFLOW");
    overflowTitle.SetFontSize(14.0f);
    Label overflowVerticalTitle = Label::New("Text::MarqueeOrientation::VERTICAL");
    overflowVerticalTitle.SetFontSize(14.0f);
    Label overflowFocusTitle = Label::New("When focused, the marquee runs if the text is long");
    overflowFocusTitle.SetFontSize(14.0f);
    mOverflowLabel          = CreateMarqueeLabel(TEXT_LTR, false, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::HORIZONTAL);
    mOverflowLabelRtl       = CreateMarqueeLabel(TEXT_RTL, false, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::HORIZONTAL);
    mOverflowLabelMultiline = CreateMarqueeLabel(TEXT_LTR, true, Text::MarqueeTriggerPolicy::ON_OVERFLOW, Text::MarqueeOrientation::VERTICAL);
    mOverflowLabelFocus     = CreateMarqueeLabel(TEXT_LTR, false, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::HORIZONTAL);
    mOverflowLabelFocus.SetFocusable(true);
    mOverflowLabelFocus.SetFocusOnTouchEnabled(true);
    overflowSection.Add(overflowTitle);
    overflowSection.Add(mOverflowLabel);
    overflowSection.Add(mOverflowLabelRtl);
    overflowSection.Add(overflowVerticalTitle);
    overflowSection.Add(mOverflowLabelMultiline);
    overflowSection.Add(overflowFocusTitle);
    overflowSection.Add(mOverflowLabelFocus);

    StackLayout alignmentSection = StackLayout::New(StackOrientation::HORIZONTAL);
    alignmentSection.SetRequestedWidth(MATCH_PARENT);
    alignmentSection.SetSpacing(STACK_SPACING);
    alignmentSection.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    alignmentSection.SetBackgroundColor(UiColor(0xEFEFEF));
    mLabelVerticalStart = CreateMarqueeLabel(TEXT_LTR, true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL);
    mLabelVerticalStart.SetRequestedHeight(200);
    mLabelVerticalStart.SetMaximumHeight(200);
    mLabelVerticalStart.SetVerticalTextAlignment(Text::Alignment::START);
    mLabelVerticalCenter = CreateMarqueeLabel(TEXT_LTR, true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL);
    mLabelVerticalCenter.SetRequestedHeight(200);
    mLabelVerticalCenter.SetMaximumHeight(200);
    mLabelVerticalCenter.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mLabelVerticalEnd = CreateMarqueeLabel(TEXT_LTR, true, Text::MarqueeTriggerPolicy::MANUAL, Text::MarqueeOrientation::VERTICAL);
    mLabelVerticalEnd.SetRequestedHeight(200);
    mLabelVerticalEnd.SetMaximumHeight(200);
    mLabelVerticalEnd.SetVerticalTextAlignment(Text::Alignment::END);
    alignmentSection.Add(mLabelVerticalStart);
    alignmentSection.Add(mLabelVerticalCenter);
    alignmentSection.Add(mLabelVerticalEnd);

    root.Add(title);
    root.Add(manualSection);
    root.Add(overflowSection);
    root.Add(alignmentSection);
    window.Add(root);

    mLabelFocus.FocusChangedSignal().Connect(this, &TextController::OnFocusChanged);
    mOverflowLabelFocus.FocusChangedSignal().Connect(this, &TextController::OnOverflowFocusChanged);

    SetMarqueeStopModeForAll(mMarqueeStopMode);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  Label CreateMarqueeLabel(Dali::String text, bool multiLine, Text::MarqueeTriggerPolicy policy, Text::MarqueeOrientation orientation)
  {
    auto label = Label::New(text);
    label.SetTextColor(UiColor(0xFFFFFF));
    label.SetBackgroundColor(UiColor(0x000000));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetMultiLine(multiLine);
    label.SetMarqueeTriggerPolicy(policy);
    label.SetMarqueeOrientation(orientation);

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
        if(DevelActor::GetNaturalSize(label).width > label.GetSize().width)
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

  void OnKeyEvent(Window window, KeyEvent event)
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
    else if(event.GetKeyName() == "a")
    {
      SetOverflowVisibility(false);
    }
    else if(event.GetKeyName() == "s")
    {
      SetOverflowVisibility(true);
    }
    else if(event.GetKeyName() == "d")
    {
      ToggleMarqueeStopMode();
    }
    else if(event.GetKeyName() == "i")
    {
      PrintAllLabelInfo();
    }
    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(event.GetKeyName() == "r")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "t")
    {
      UiScaleManager::Get().SetScale(2.0f);
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

  void ToggleMarqueeStopMode()
  {
    mMarqueeStopMode = (mMarqueeStopMode == Text::MarqueeStopMode::IMMEDIATE) ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE;
    SetMarqueeStopModeForAll(mMarqueeStopMode);
  }

  void SetMarqueeStopModeForAll(Text::MarqueeStopMode stopMode)
  {
    mLabelLtr.SetMarqueeStopMode(stopMode);
    mLabelRtl.SetMarqueeStopMode(stopMode);
    mLabelMultiline.SetMarqueeStopMode(stopMode);
    mLabelFocus.SetMarqueeStopMode(stopMode);
    mOverflowLabel.SetMarqueeStopMode(stopMode);
    mOverflowLabelRtl.SetMarqueeStopMode(stopMode);
    mOverflowLabelMultiline.SetMarqueeStopMode(stopMode);
    mOverflowLabelFocus.SetMarqueeStopMode(stopMode);
    mLabelVerticalStart.SetMarqueeStopMode(stopMode);
    mLabelVerticalCenter.SetMarqueeStopMode(stopMode);
    mLabelVerticalEnd.SetMarqueeStopMode(stopMode);
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

  void SetOverflowFontSize(float size)
  {
    mOverflowLabel.SetFontSize(size);
    mOverflowLabelRtl.SetFontSize(size);
    mOverflowLabelMultiline.SetFontSize(size);
    mOverflowLabelFocus.SetFontSize(size);
  }

  void SetOverflowVisibility(bool visible)
  {
    mOverflowLabel.SetVisible(visible);
    mOverflowLabelRtl.SetVisible(visible);
    mOverflowLabelMultiline.SetVisible(visible);
    mOverflowLabelFocus.SetVisible(visible);
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
  Application&           mApplication;
  Text::MarqueeStopMode  mMarqueeStopMode;
  Label                  mLabelLtr;
  Label                  mLabelRtl;
  Label                  mLabelMultiline;
  Label                  mLabelFocus;
  Label                  mOverflowLabel;
  Label                  mOverflowLabelRtl;
  Label                  mOverflowLabelMultiline;
  Label                  mOverflowLabelFocus;
  Label                  mLabelVerticalStart;
  Label                  mLabelVerticalCenter;
  Label                  mLabelVerticalEnd;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetLabelAsyncRendering(true);
  config.SetMarqueeLoopCount(1);
  config.Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
