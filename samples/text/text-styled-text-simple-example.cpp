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

#include <cstdio>
#include <cstdint>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int   WINDOW_WIDTH   = 760;
constexpr int   WINDOW_HEIGHT  = 320;
constexpr float STACK_PADDING  = 32.0f;
constexpr float STACK_SPACING  = 18.0f;
constexpr char  SAMPLE_TEXT[]  = "StyledText: red, bold, underline";
constexpr uint32_t RED_START       = 12u;
constexpr uint32_t RED_END         = 15u;
constexpr uint32_t BOLD_START      = 17u;
constexpr uint32_t BOLD_END        = 21u;
constexpr uint32_t UNDERLINE_START = 23u;
constexpr uint32_t UNDERLINE_END   = 32u;
} // namespace

class TextStyledTextSimpleController : public ConnectionTracker
{
public:
  explicit TextStyledTextSimpleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextStyledTextSimpleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF8FAFC));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(0xF8FAFC));

    Label titleLabel = Label::New("StyledTextBuilder simple sample");
    titleLabel.SetFontFamily("Ubuntu Mono");
    titleLabel.SetFontSize(24.0f);
    titleLabel.SetTextColor(UiColor(0x334155));
    titleLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);

    Label sampleLabel = Label::New();
    sampleLabel.SetFontFamily("Ubuntu Mono");
    sampleLabel.SetFontSize(42.0f);
    sampleLabel.SetTextColor(UiColor(0x111827));
    sampleLabel.SetMultiLine(true);

    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(SAMPLE_TEXT);

    Text::ForegroundColorSpan foregroundSpan = Text::ForegroundColorSpan::New(UiColor(0xDC2626));
    builder.SetSpan(foregroundSpan, RED_START, RED_END);

    Text::FontAttributes boldAttributes;
    boldAttributes.SetWeight(Text::FontWeight::BOLD);
    Text::FontSpan boldSpan = Text::FontSpan::New(boldAttributes);
    builder.SetSpan(boldSpan, BOLD_START, BOLD_END);

    Text::Underline underline;
    underline.SetColor(UiColor(0x2563EB));
    underline.SetThickness(2.0f);
    underline.SetType(Text::Underline::Type::SOLID);
    Text::UnderlineSpan underlineSpan = Text::UnderlineSpan::New(underline);
    builder.SetSpan(underlineSpan, UNDERLINE_START, UNDERLINE_END);

    Text::StyledText styledText = builder.Build();
    sampleLabel.SetStyledText(styledText);

    Text::StyledText storedStyledText = sampleLabel.GetStyledText();
    if(storedStyledText)
    {
      std::printf("[text-styled-text-simple-example] text=\"%s\", spanCount=%u\n",
                  storedStyledText.GetText().CStr(),
                  storedStyledText.GetSpanCount());

      for(uint32_t index = 0u; index < storedStyledText.GetSpanCount(); ++index)
      {
        const uint32_t startIndex = storedStyledText.GetSpanStartIndexAt(index);
        const uint32_t endIndex   = storedStyledText.GetSpanEndIndexAt(index);
        Text::Span    span        = storedStyledText.GetSpanAt(index);

        Text::ForegroundColorSpan foreground = Text::ForegroundColorSpan::DownCast(span);
        if(foreground)
        {
          const Vector4 color = foreground.GetColor().GetRgba();
          std::printf("[text-styled-text-simple-example] ForegroundColorSpan %u-%u rgba=(%.2f, %.2f, %.2f, %.2f)\n",
                      startIndex,
                      endIndex,
                      color.r,
                      color.g,
                      color.b,
                      color.a);
        }

        Text::FontSpan font = Text::FontSpan::DownCast(span);
        if(font)
        {
          Text::FontAttributes attributes = font.GetFontAttributes();
          const bool           hasWeight  = attributes.Has(Text::FontAttributes::Attribute::WEIGHT);
          const Text::FontWeight weight   = attributes.GetWeight();
          std::printf("[text-styled-text-simple-example] FontSpan %u-%u hasWeight=%d weight=%d\n",
                      startIndex,
                      endIndex,
                      hasWeight ? 1 : 0,
                      static_cast<int>(weight));
        }

        Text::UnderlineSpan underlineSpan = Text::UnderlineSpan::DownCast(span);
        if(underlineSpan)
        {
          Text::Underline inspectedUnderline = underlineSpan.GetUnderline();
          const Vector4   color              = inspectedUnderline.GetColor().GetRgba();
          std::printf("[text-styled-text-simple-example] UnderlineSpan %u-%u type=%d thickness=%.1f color=(%.2f, %.2f, %.2f, %.2f)\n",
                      startIndex,
                      endIndex,
                      static_cast<int>(inspectedUnderline.GetType()),
                      inspectedUnderline.GetThickness(),
                      color.r,
                      color.g,
                      color.b,
                      color.a);
        }
      }
    }

    root.Add(titleLabel);
    root.Add(sampleLabel);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextStyledTextSimpleController::OnKeyEvent);
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

  TextStyledTextSimpleController controller(application);
  application.MainLoop();
  return 0;
}
