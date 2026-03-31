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
#include <dali/integration-api/debug.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/focus-manager/keyboard-focus-manager.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;
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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetViewPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Text Marquee Example\n"
                   "Test options:\n"
                   "[1] Start Marquee, [2] Stop Marquee, [i] Information\n"
                   "Text::MarqueeOrientation::HORIZONTAL")
          .SetMultiLine(true)
          .SetLineHeight(1.6f),
        Label::New("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id congue odio.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetRequestedWidth(400)
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetMarqueeSpeed(100)
          .SetMarqueeLoopCount(2)
          .SetMarqueeLoopDelay(1.0f)
          .SetMarqueeGap(50)
          .SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
          .SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL)
          .As(mMarqueeLabel),
        Label::New("שלום עולם, נעים מאוד,ומקווה שיהיה לנו שיחה נעימה וטובה יחד")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetRequestedWidth(400)
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetMarqueeSpeed(100)
          .SetMarqueeLoopCount(2)
          .SetMarqueeLoopDelay(1.0f)
          .SetMarqueeGap(50)
          .SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
          .SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL)
          .As(mRtlLabel),
        Label::New("Text::MarqueeOrientation::VERTICAL"),
        Label::New("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id congue odio.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetRequestedWidth(200)
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetMultiLine(true)
          .SetMarqueeSpeed(100)
          .SetMarqueeLoopCount(2)
          .SetMarqueeLoopDelay(1.0f)
          .SetMarqueeGap(50)
          .SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
          .SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL)
          .As(mVerticalLabel),
        Label::New("TODO: When focused, the marquee runs if the text is too long"),
          Label::New("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed id congue odio.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetMarqueeSpeed(100)
          .SetMarqueeLoopCount(0)
          .SetMarqueeLoopDelay(1.0f)
          .SetMarqueeGap(50)
          .SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
          .SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL)
          .As(mFocusLabel),
      }));

    // TODO: Add focus signals to View so marquee can react to focus gained/lost.
    // mFocusLabel.KeyInputFocusGainedSignal().Connect(this, &TextController::OnFocusGained);
    // mFocusLabel.KeyInputFocusLostSignal().Connect(this, &TextController::OnFocusLost);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);

    PrintLabelInfo(mMarqueeLabel, "Horizontal Marquee");
  }

  void OnFocusGained(View view)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      if(label.GetNaturalSize().width > label.GetSize().GetWidth())
      {
        label.StartMarquee();
      }
    }
  }

  void OnFocusLost(View view)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      label.StopMarquee();
    }
  }

  void OnAnchorClicked(View view, const Dali::String& href)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
      DALI_LOG_ERROR("OnAnchorClicked, href: %s, length: %zu\n", href.CStr(), href.Size());
    }
  }

  void PrintLabelInfo(Label label, const char* title)
  {
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text        : %s\n", label.GetText().CStr());
    DALI_LOG_ERROR("IsRunning   : %d\n", label.IsMarqueeRunning());
    DALI_LOG_ERROR("Speed       : %d\n", label.GetMarqueeSpeed());
    DALI_LOG_ERROR("LoopCount   : %d\n", label.GetMarqueeLoopCount());
    DALI_LOG_ERROR("LoopDelay   : %f\n", label.GetMarqueeLoopDelay());
    DALI_LOG_ERROR("Gap         : %f\n", label.GetMarqueeGap());
    DALI_LOG_ERROR("StopMode    : %d\n", label.GetMarqueeStopMode());
    DALI_LOG_ERROR("Orientation : %d\n", label.GetMarqueeOrientation());
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      DALI_LOG_ERROR("DALI_KEY_ESCAPE:%d, DALI_KEY_BACK:%d\n", IsKey(event, Dali::DALI_KEY_ESCAPE), IsKey(event, Dali::DALI_KEY_BACK));
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      mMarqueeLabel.StartMarquee();
      mRtlLabel.StartMarquee();
      mVerticalLabel.StartMarquee();
    }
    else if(event.GetKeyName() == "2")
    {
      mMarqueeLabel.StopMarquee();
      mRtlLabel.StopMarquee();
      mVerticalLabel.StopMarquee();
    }
    else if(event.GetKeyName() == "i")
    {
      PrintLabelInfo(mMarqueeLabel, "Horizontal Marquee");
      PrintLabelInfo(mRtlLabel, "RTL Marquee");
      PrintLabelInfo(mVerticalLabel, "Vertical Marquee");
    }
    // Temporary test.
    if(event.GetKeyName() == "3")
    {
      if(mFocusLabel.GetNaturalSize().width > mFocusLabel.GetSize().GetWidth())
      {
        mFocusLabel.StartMarquee();
      }
    }
    else if(event.GetKeyName() == "4")
    {
      mFocusLabel.StopMarquee();
    }
  }

private:
  Application& mApplication;
  Label        mMarqueeLabel;
  Label        mRtlLabel;
  Label        mVerticalLabel;
  Label        mFocusLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
