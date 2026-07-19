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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int   WINDOW_WIDTH  = 760;
constexpr int   WINDOW_HEIGHT = 420;
constexpr float STACK_PADDING = 32.0f;
constexpr float STACK_SPACING = 18.0f;
} // namespace

class TextImageSpanSimpleController : public ConnectionTracker
{
public:
  explicit TextImageSpanSimpleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextImageSpanSimpleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF8FAFC));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(0xF8FAFC));

    Label titleLabel = Label::New("ImageSpan simple sample");
    titleLabel.SetFontSize(24.0f);
    titleLabel.SetTextColor(UiColor(0x334155));
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);

    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
    builder.AppendText("Before ");

    const uint32_t imageIndex = builder.GetUtf32Length();
    builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);

    builder.AppendText(" after");

    Text::ImageAttributes imageAttributes(RESOURCES_DIR "flag_kr.png", Vector2(64.0f, 40.0f));
    imageAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);

    Text::ImageSpan imageSpan = Text::ImageSpan::New(imageAttributes);
    builder.SetSpan(imageSpan, imageIndex, imageIndex + 1u);

    Label sampleLabel = Label::New();
    sampleLabel.SetFontSize(42.0f);
    sampleLabel.SetTextColor(UiColor(0x111827));
    sampleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    sampleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    sampleLabel.SetMultiLine(true);
    sampleLabel.SetStyledText(builder.Build());
    sampleLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    Text::StyledTextBuilder pushPopBuilder = Text::StyledTextBuilder::New();
    pushPopBuilder.AppendText("Push/Pop before ");

    Text::ImageAttributes pushPopImageAttributes(RESOURCES_DIR "flag_kr.png", Vector2(64.0f, 40.0f));
    pushPopImageAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);

    pushPopBuilder.PushSpan(Text::ImageSpan::New(pushPopImageAttributes));
    pushPopBuilder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    pushPopBuilder.PopSpan();

    pushPopBuilder.AppendText(" after");

    Label pushPopSampleLabel = Label::New();
    pushPopSampleLabel.SetFontSize(42.0f);
    pushPopSampleLabel.SetTextColor(UiColor(0x111827));
    pushPopSampleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    pushPopSampleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    pushPopSampleLabel.SetMultiLine(true);
    pushPopSampleLabel.SetStyledText(pushPopBuilder.Build());
    pushPopSampleLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    root.Add(titleLabel);
    root.Add(sampleLabel);
    root.Add(pushPopSampleLabel);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextImageSpanSimpleController::OnKeyEvent);
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextImageSpanSimpleController controller(application);
  application.MainLoop();
  return 0;
}
