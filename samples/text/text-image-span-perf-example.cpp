/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <array>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using Clock = std::chrono::steady_clock;

constexpr int                      WINDOW_WIDTH   = 1080;
constexpr int                      WINDOW_HEIGHT  = 820;
constexpr char                     IMAGE_MARKER[] = "[image]";
constexpr std::array<uint32_t, 4u> INTERVALS_MS{{500u, 1000u, 2000u, 3000u}};

struct ImageSpec
{
  const char*                            source{nullptr};
  Vector2                                size{24.0f, 24.0f};
  Text::ImageAttributes::InlineAlignment alignment{Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM};
  float                                  verticalOffset{0.0f};
};

struct StyledTextCase
{
  const char*            name{nullptr};
  const char*            description{nullptr};
  const char*            pattern{nullptr};
  std::vector<ImageSpec> images;
  bool                   multiline{true};
  bool                   expectedEllipsis{false};
  bool                   rtl{false};
  Text::Alignment        horizontalAlignment{Text::Alignment::START};
  Text::Alignment        verticalAlignment{Text::Alignment::CENTER};
  Text::LineWrapMode     lineWrapMode{Text::LineWrapMode::WORD};
  float                  fontSize{28.0f};
};

ImageSpec Image(const char* source,
                Vector2     size,
                Text::ImageAttributes::InlineAlignment alignment = Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM,
                float verticalOffset = 0.0f)
{
  return {source, size, alignment, verticalOffset};
}

std::vector<ImageSpec> RepeatedImages(uint32_t count, const char* source, Vector2 size)
{
  std::vector<ImageSpec> images;
  images.reserve(count);
  for(uint32_t index = 0u; index < count; ++index)
  {
    images.push_back(Image(source, size));
  }
  return images;
}

std::vector<StyledTextCase> CreateCases()
{
  std::vector<StyledTextCase> cases;
  cases.reserve(32u);

  cases.push_back({"Short text", "A short StyledText snapshot without replacements.", "Short text", {}, false});
  cases.push_back({"Medium text", "Ordinary medium-length text exercises the no-ImageSpan path.",
                   "A medium-length sentence changes repeatedly without any replacement span.", {}, false});
  cases.push_back({"Long wrapped text", "Long multiline content without ellipsis.",
                   "This long paragraph wraps over several lines while the same Label is repeatedly updated. "
                   "It keeps ordinary shaping and layout active without creating any image visual. "
                   "Additional words make the workload distinct from the short and medium cases."});
  cases.push_back({"Very long END ellipsis", "Single-line ordinary text requires END ellipsis.",
                   "A very long single line of ordinary text deliberately exceeds the preview width and must end with one stable ellipsis.",
                   {}, false, true});
  cases.push_back({"Explicit newlines", "Multiple explicit lines without replacements.",
                   "First explicit line\nSecond explicit line\nThird explicit line"});
  cases.push_back({"Image only", "One replacement is the complete logical text.", "[image]",
                   {Image("flag_kr.png", Vector2(72.0f, 44.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}, false});
  cases.push_back({"Text then image", "A trailing replacement follows ordinary text.", "Text before [image]",
                   {Image("flag_us.png", Vector2(58.0f, 34.0f))}, false});
  cases.push_back({"Image then text", "A leading replacement precedes ordinary text.", "[image] text after",
                   {Image("flag_ae.png", Vector2(58.0f, 34.0f))}, false});
  cases.push_back({"Text image text", "One replacement participates inside a sentence.", "Text before [image] text after",
                   {Image("flag_kr.png", Vector2(64.0f, 38.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}, false});
  cases.push_back({"Adjacent images", "Two consecutive replacements have different reserved sizes.", "Before [image][image] after",
                   {Image("flag_kr.png", Vector2(32.0f, 24.0f)), Image("flag_us.png", Vector2(68.0f, 38.0f))}, false});
  cases.push_back({"One baseline image", "A single baseline-aligned image exercises one-visual publication.", "Baseline [image] image",
                   {Image("flag_ae.png", Vector2(48.0f, 42.0f), Text::ImageAttributes::InlineAlignment::TEXT_BASELINE)}, false});
  cases.push_back({"Two local sources", "Two images use different bundled resources.", "Korea [image] and USA [image]",
                   {Image("flag_kr.png", Vector2(52.0f, 32.0f)), Image("flag_us.png", Vector2(52.0f, 32.0f))}, false});
  cases.push_back({"Five images", "Five replacements alternate bundled local resources.",
                   "One [image] two [image] three [image] four [image] five [image]",
                   {Image("flag_kr.png", Vector2(30.0f, 20.0f)), Image("flag_us.png", Vector2(30.0f, 20.0f)),
                    Image("flag_ae.png", Vector2(30.0f, 20.0f)), Image("flag_kr_alt.png", Vector2(30.0f, 20.0f)),
                    Image("flag_us_alt.png", Vector2(30.0f, 20.0f))}});
  cases.push_back({"Ten images", "Ten small replacements exercise a denser runtime sidecar.",
                   "[image] [image] [image] [image] [image] [image] [image] [image] [image] [image]",
                   RepeatedImages(10u, "flag_kr.png", Vector2(24.0f, 16.0f))});
  cases.push_back({"Same source repeated", "Three occurrences reuse one decoded local source.",
                   "Repeated [image] source [image] reuse [image]",
                   RepeatedImages(3u, "flag_us.png", Vector2(56.0f, 32.0f))});
  cases.push_back({"Different local sources", "Six bundled sources alternate in one paragraph.",
                   "[image] [image] [image] [image] [image] [image]",
                   {Image("flag_kr.png", Vector2(42.0f, 26.0f)), Image("flag_us.png", Vector2(42.0f, 26.0f)),
                    Image("flag_ae.png", Vector2(42.0f, 26.0f)), Image("flag_kr_alt.png", Vector2(42.0f, 26.0f)),
                    Image("flag_us_alt.png", Vector2(42.0f, 26.0f)), Image("flag_ae_alt.png", Vector2(42.0f, 26.0f))}});
  cases.push_back({"Tiny image", "An 8x8 replacement stays visible in ordinary text.", "Tiny [image] image",
                   {Image("flag_kr.png", Vector2(8.0f, 8.0f))}, false});
  cases.push_back({"Large image", "A large replacement expands its surrounding line.",
                   "Text before the large [image] replacement and enough trailing text to wrap below it.",
                   {Image("flag_us.png", Vector2(180.0f, 100.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}});
  cases.push_back({"Mixed image sizes", "Tiny, medium, wide, and tall boxes share one paragraph.",
                   "Tiny [image], medium [image], wide [image], and tall [image] replacements.",
                   {Image("flag_kr.png", Vector2(10.0f, 10.0f)), Image("flag_us.png", Vector2(40.0f, 28.0f)),
                    Image("flag_ae.png", Vector2(100.0f, 34.0f)),
                    Image("flag_kr_alt.png", Vector2(42.0f, 86.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}});
  cases.push_back({"TEXT_BOTTOM", "The image bottom aligns with the surrounding text bottom.", "Bottom [image] alignment",
                   {Image("flag_us.png", Vector2(54.0f, 42.0f), Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM)}, false});
  cases.push_back({"TEXT_BASELINE", "The image bottom aligns with the text baseline.", "Baseline [image] alignment",
                   {Image("flag_kr.png", Vector2(54.0f, 42.0f), Text::ImageAttributes::InlineAlignment::TEXT_BASELINE)}, false});
  cases.push_back({"TEXT_CENTER", "The image is vertically centered in the text line.", "Center [image] alignment",
                   {Image("flag_ae.png", Vector2(54.0f, 42.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}, false});
  cases.push_back({"Positive vertical offset", "A positive offset moves the image down.", "Positive [image] offset",
                   {Image("flag_kr.png", Vector2(48.0f, 32.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER, 8.0f)}, false});
  cases.push_back({"Negative vertical offset", "A negative offset moves the image up.", "Negative [image] offset",
                   {Image("flag_us.png", Vector2(48.0f, 32.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER, -8.0f)}, false});
  cases.push_back({"LTR paragraph", "A replacement participates in left-to-right layout.",
                   "English before [image] and ordinary text after the image.",
                   {Image("flag_kr.png", Vector2(52.0f, 30.0f))}});
  cases.push_back({"RTL paragraph", "A replacement participates in right-to-left layout.",
                   "עברית לפני [image] العربية بعد الصورة",
                   {Image("flag_ae.png", Vector2(52.0f, 30.0f))}, true, false, true});
  cases.push_back({"Mixed bidi", "LTR, Hebrew, and Arabic runs surround one replacement.",
                   "LTR עברית [image] العربية trailing text",
                   {Image("flag_us.png", Vector2(52.0f, 30.0f))}, true, false, true});
  cases.push_back({"Replacement-only line", "An explicit line contains only one large replacement.",
                   "Line before\n[image]\nLine after",
                   {Image("flag_kr.png", Vector2(140.0f, 76.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}});
  cases.push_back({"Multiline wrapping", "Three replacements participate in word wrapping.",
                   "A long multiline paragraph places [image] between ordinary words, continues with enough text to wrap, "
                   "then adds [image] and a final [image] before the remaining tail.",
                   {Image("flag_kr.png", Vector2(44.0f, 28.0f)), Image("flag_us.png", Vector2(72.0f, 36.0f)),
                    Image("flag_ae.png", Vector2(36.0f, 44.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}});
  cases.push_back({"END ellipsis image boundary", "A replacement is fully visible or fully elided at END.",
                   "A readable prefix approaches [image] and a deliberately long trailing sentence exceeds the preview width.",
                   {Image("flag_us.png", Vector2(100.0f, 48.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}, false, true});
  cases.push_back({"Missing local resource", "A missing local source retains the authored reservation.",
                   "Missing image [image] keeps layout stable while loading fails.",
                   {Image("missing-image-span-perf.png", Vector2(80.0f, 48.0f), Text::ImageAttributes::InlineAlignment::TEXT_CENTER)}, false});
  cases.push_back({"Multiline END ellipsis", "Explicit lines and later images cross the vertical ellipsis boundary.",
                   "First line [image]\nSecond line contains enough words to wrap around [image]\n"
                   "Third line [image] continues with a long tail that must be elided before the final [image].",
                   {Image("flag_kr.png", Vector2(44.0f, 28.0f)), Image("flag_us.png", Vector2(90.0f, 48.0f)),
                    Image("flag_ae.png", Vector2(48.0f, 36.0f)), Image("flag_kr_alt.png", Vector2(72.0f, 40.0f))},
                   true, true});

  return cases;
}

const char* AlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::CENTER: return "CENTER";
    case Text::Alignment::END: return "END";
    case Text::Alignment::START:
    default: return "START";
  }
}

Label NewPanelLabel(const char* text, float height, uint32_t background, bool interactive = false)
{
  Label label = Label::New(text);
  label.SetFontSize(interactive ? 14.0f : 16.0f);
  label.SetTextColor(UiColor(0xF8FAFC));
  label.SetBackgroundColor(UiColor(background));
  label.SetPadding(Insets(10.0f, 10.0f, 6.0f, 6.0f));
  label.SetMultiLine(true);
  label.SetHorizontalTextAlignment(interactive ? Text::Alignment::CENTER : Text::Alignment::START);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetCornerRadius(6.0f);
  if(interactive)
  {
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  }
  return label;
}
} // unnamed namespace

class TextImageSpanPerfController : public ConnectionTracker
{
public:
  explicit TextImageSpanPerfController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextImageSpanPerfController::OnInit);
  }

private:
  std::string Resource(const char* file) const
  {
    return std::string(RESOURCES_DIR) + file;
  }

  Text::StyledText BuildCase(const StyledTextCase& data) const
  {
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
    const std::string       pattern(data.pattern ? data.pattern : "");
    std::size_t             cursor = 0u;

    for(const ImageSpec& image : data.images)
    {
      const std::size_t marker = pattern.find(IMAGE_MARKER, cursor);
      DALI_ASSERT_ALWAYS(marker != std::string::npos && "ImageSpan perf case has too few image markers");
      builder.AppendText(pattern.substr(cursor, marker - cursor).c_str());

      const uint32_t imageIndex = builder.GetUtf32Length();
      builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);

      Text::ImageAttributes attributes(Resource(image.source).c_str(), image.size);
      attributes.SetAlignment(image.alignment);
      attributes.SetVerticalOffset(image.verticalOffset);
      DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(attributes), imageIndex, imageIndex + 1u) &&
                         "ImageSpan perf range must be valid");
      cursor = marker + sizeof(IMAGE_MARKER) - 1u;
    }

    DALI_ASSERT_ALWAYS(pattern.find(IMAGE_MARKER, cursor) == std::string::npos &&
                       "ImageSpan perf case has too many image markers");
    builder.AppendText(pattern.substr(cursor).c_str());
    return builder.Build();
  }

  void ConfigureLabel(const StyledTextCase& data)
  {
    mPreview.SetAsyncRendering(mAsyncRendering);
    mPreview.SetMultiLine(data.multiline);
    mPreview.SetTextOverflowMode(data.expectedEllipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
    mPreview.SetLineWrapMode(data.lineWrapMode);
    mPreview.SetFontSize(data.fontSize);
    mPreview.SetTextColor(UiColor(0x111827));
    mPreview.SetBackgroundColor(UiColor(0xF8FAFC));
    mPreview.SetPadding(Insets(20.0f, 20.0f, 18.0f, 18.0f));
    mPreview.SetHorizontalTextAlignment(data.horizontalAlignment);
    mPreview.SetVerticalTextAlignment(data.verticalAlignment);
    mPreview.SetLayoutDirection(data.rtl ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    mPreview.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  }

  void CreatePreview(const StyledTextCase& data)
  {
    mPreview = Label::New();
    ConfigureLabel(data);
    mPreviewContainer.Add(mPreview);
  }

  void RecreatePreview(const StyledTextCase& data)
  {
    if(mPreview)
    {
      mPreviewContainer.Remove(mPreview, RemovePolicy::IMMEDIATE);
      mPreview.Reset();
    }
    CreatePreview(data);
  }

  void ApplyCurrentCase()
  {
    const Clock::time_point start = Clock::now();
    const StyledTextCase&   data  = mCases[mCaseIndex];
    Text::StyledText        text  = mFreshBuild ? BuildCase(data) : mPrebuilt[mCaseIndex];

    if(mRecreateLabel)
    {
      RecreatePreview(data);
    }
    else
    {
      ConfigureLabel(data);
    }
    mPreview.SetStyledText(text);

    mLastUpdateMilliseconds = std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    ++mTickCount;
    UpdateHud(text);
  }

  void UpdateControls()
  {
    std::ostringstream interval;
    interval << "Interval: " << std::fixed << std::setprecision(1)
             << static_cast<double>(INTERVALS_MS[mIntervalIndex]) / 1000.0 << "s";
    mPlaybackControl.SetText(mRunning ? "Playback: RUNNING" : "Playback: PAUSED");
    mIntervalControl.SetText(interval.str().c_str());
    mRenderingControl.SetText(mAsyncRendering ? "Rendering: ASYNC" : "Rendering: SYNC");
    mLabelControl.SetText(mRecreateLabel ? "Label: RECREATE" : "Label: REUSE");
    mStyledTextControl.SetText(mFreshBuild ? "StyledText: FRESH BUILD" : "StyledText: PREBUILT REUSE");
  }

  void UpdateHud(const Text::StyledText& text)
  {
    const StyledTextCase& data = mCases[mCaseIndex];
    std::ostringstream status;
    status << "Case " << mCaseIndex + 1u << " / " << mCases.size() << " — " << data.name << "\n"
           << data.description << "\n"
           << "UTF-32 length: " << text.GetUtf32Length() << " | ImageSpan count: " << data.images.size()
           << " | Expected ellipsis: " << (data.expectedEllipsis ? "YES" : "NO") << "\n"
           << "Interval: " << (static_cast<double>(INTERVALS_MS[mIntervalIndex]) / 1000.0) << "s"
           << " | Rendering: " << (mAsyncRendering ? "ASYNC" : "SYNC")
           << " | Label: " << (mRecreateLabel ? "RECREATE" : "REUSE")
           << " | StyledText: " << (mFreshBuild ? "FRESH BUILD" : "PREBUILT REUSE") << "\n"
           << "Tick count: " << mTickCount << " | Last update: " << std::fixed << std::setprecision(3)
           << mLastUpdateMilliseconds << " ms"
           << " | Layout: " << (data.multiline ? "multiline" : "single line")
           << ", " << AlignmentName(data.horizontalAlignment) << "\n"
           << "Last update time is sample telemetry, not a product performance pass criterion.";
    mStatus.SetText(status.str().c_str());
    UpdateControls();
  }

  void RestartTimer()
  {
    if(mTimer)
    {
      mTimer.Stop();
      mTimer.Reset();
    }
    mTimer = Timer::New(INTERVALS_MS[mIntervalIndex]);
    mTimer.TickSignal().Connect(this, &TextImageSpanPerfController::OnTick);
    if(mRunning)
    {
      mTimer.Start();
    }
  }

  bool OnTick()
  {
    if(!mRunning)
    {
      return false;
    }
    mCaseIndex = (mCaseIndex + 1u) % mCases.size();
    ApplyCurrentCase();
    return true;
  }

  void TogglePlayback()
  {
    mRunning = !mRunning;
    if(mRunning)
    {
      if(!mTimer)
      {
        RestartTimer();
      }
      else
      {
        mTimer.Start();
      }
    }
    else if(mTimer)
    {
      mTimer.Stop();
    }
    UpdateControls();
  }

  void CycleInterval()
  {
    mIntervalIndex = (mIntervalIndex + 1u) % INTERVALS_MS.size();
    RestartTimer();
    UpdateHud(mPreview.GetStyledText());
  }

  void ToggleRendering()
  {
    mAsyncRendering = !mAsyncRendering;
    UpdateControls();
  }

  void ToggleLabelMode()
  {
    mRecreateLabel = !mRecreateLabel;
    UpdateControls();
  }

  void ToggleStyledTextMode()
  {
    mFreshBuild = !mFreshBuild;
    UpdateControls();
  }

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0x0F172A));

    mCases = CreateCases();
    mPrebuilt.reserve(mCases.size());
    for(const StyledTextCase& data : mCases)
    {
      mPrebuilt.push_back(BuildCase(data));
    }

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(8.0f);
    root.SetPadding(Insets(16.0f, 16.0f, 16.0f, 16.0f));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);

    Label title = NewPanelLabel("ImageSpan repeated StyledText lifecycle and performance validation", 48.0f, 0x1D4ED8);
    title.SetHorizontalTextAlignment(Text::Alignment::CENTER);

    StackLayout primaryControls = StackLayout::New(StackOrientation::HORIZONTAL);
    primaryControls.SetSpacing(6.0f);
    primaryControls.SetRequestedWidth(MATCH_PARENT);
    primaryControls.SetRequestedHeight(46.0f);
    mPlaybackControl  = NewPanelLabel("", 46.0f, 0x0F766E, true);
    mIntervalControl  = NewPanelLabel("", 46.0f, 0x475569, true);
    mRenderingControl = NewPanelLabel("", 46.0f, 0x475569, true);
    for(Label control : {mPlaybackControl, mIntervalControl, mRenderingControl})
    {
      primaryControls.Add(control);
    }

    StackLayout lifecycleControls = StackLayout::New(StackOrientation::HORIZONTAL);
    lifecycleControls.SetSpacing(6.0f);
    lifecycleControls.SetRequestedWidth(MATCH_PARENT);
    lifecycleControls.SetRequestedHeight(46.0f);
    mLabelControl      = NewPanelLabel("", 46.0f, 0x475569, true);
    mStyledTextControl = NewPanelLabel("", 46.0f, 0x475569, true);
    lifecycleControls.Add(mLabelControl);
    lifecycleControls.Add(mStyledTextControl);

    mPlaybackControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { TogglePlayback(); });
    mIntervalControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { CycleInterval(); });
    mRenderingControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleRendering(); });
    mLabelControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleLabelMode(); });
    mStyledTextControl.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent) { ToggleStyledTextMode(); });

    mPreviewContainer = StackLayout::New(StackOrientation::VERTICAL);
    mPreviewContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    CreatePreview(mCases[mCaseIndex]);

    mStatus = NewPanelLabel("", 158.0f, 0x1E293B);
    mStatus.SetFontSize(15.0f);

    root.Add(title);
    root.Add(primaryControls);
    root.Add(lifecycleControls);
    root.Add(mPreviewContainer);
    root.Add(mStatus);
    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextImageSpanPerfController::OnKey);

    ApplyCurrentCase();
    RestartTimer();
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyString() == "p" || event.GetKeyString() == "P")
    {
      TogglePlayback();
    }
    else if(event.GetKeyString() == "i" || event.GetKeyString() == "I")
    {
      CycleInterval();
    }
    else if(event.GetKeyString() == "r" || event.GetKeyString() == "R")
    {
      ToggleRendering();
    }
    else if(event.GetKeyString() == "l" || event.GetKeyString() == "L")
    {
      ToggleLabelMode();
    }
    else if(event.GetKeyString() == "s" || event.GetKeyString() == "S")
    {
      ToggleStyledTextMode();
    }
  }

private:
  Application&                  mApplication;
  StackLayout                   mPreviewContainer;
  Label                         mPreview;
  Label                         mStatus;
  Label                         mPlaybackControl;
  Label                         mIntervalControl;
  Label                         mRenderingControl;
  Label                         mLabelControl;
  Label                         mStyledTextControl;
  Timer                         mTimer;
  std::vector<StyledTextCase>   mCases;
  std::vector<Text::StyledText> mPrebuilt;
  std::size_t                   mCaseIndex{0u};
  std::size_t                   mIntervalIndex{1u};
  uint64_t                      mTickCount{0u};
  double                        mLastUpdateMilliseconds{0.0};
  bool                          mRunning{true};
  bool                          mAsyncRendering{false};
  bool                          mRecreateLabel{false};
  bool                          mFreshBuild{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextImageSpanPerfController controller(application);
  application.MainLoop();
  return 0;
}
