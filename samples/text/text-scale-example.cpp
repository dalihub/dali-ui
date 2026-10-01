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
#include <dali/devel-api/adaptor-framework/application.h>
#include <dali/integration-api/system/system-settings.h>

#include <array>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using SystemFont                          = Dali::Integration::SystemSettings::FontSize;
constexpr float       STACK_SPACING       = 8.f;
constexpr float       STACK_PADDING       = 16.f;
constexpr float       BUTTON_HEIGHT       = 34.f;
constexpr float       TARGET_INPUT_HEIGHT = 80.f;
const Vector2         IMAGE_SIZE(36.f, 24.f); // Match the 24px font height and flag_kr.png's 3:2 aspect ratio.
constexpr const char* TEST_TEXT = "The quick brown fox jumps over the lazy dog. 1234567890";

struct FontOption
{
  const char* key;
  const char* name;
  SystemFont  value;
};
constexpr FontOption FONT_OPTIONS[] = {
  {"1", "SMALL", SystemFont::SMALL},
  {"2", "NORMAL", SystemFont::NORMAL},
  {"3", "LARGE", SystemFont::LARGE},
  {"4", "EXTRA_LARGE", SystemFont::EXTRA_LARGE},
  {"5", "GIANT", SystemFont::GIANT}};

struct UiOption
{
  const char* key;
  float       scale;
};
constexpr UiOption UI_OPTIONS[] = {{"Q", 0.8f}, {"W", 1.f}, {"E", 1.2f}, {"R", 1.5f}, {"T", 2.f}};

struct ClampOption
{
  const char* key;
  const char* label;
  float       minimum;
  float       maximum;
};
constexpr ClampOption CLAMP_OPTIONS[] = {
  {"0", "Default 0.5..2.0", 0.5f, 2.f},
  {"F1", "Min 1.2", 1.2f, 2.f},
  {"F2", "Max 1.3", 0.5f, 1.3f},
  {"F3", "Min 1.4 > max 1.0", 1.4f, 1.f},
  {"F4", "Fixed 2.0", 2.f, 2.f}};

const Dali::Vector<Text::Fit::Candidate>& GetFitCandidates()
{
  static const Dali::Vector<Text::Fit::Candidate> candidates = []
  {
    Dali::Vector<Text::Fit::Candidate> values;
    values.PushBack(Text::Fit::Candidate(10.f, 20.f));
    values.PushBack(Text::Fit::Candidate(12.f, 24.f));
    values.PushBack(Text::Fit::Candidate(14.f, 28.f));
    values.PushBack(Text::Fit::Candidate(16.f, 32.f));
    values.PushBack(Text::Fit::Candidate(18.f, 36.f));
    values.PushBack(Text::Fit::Candidate(20.f, 40.f));
    return values;
  }();
  return candidates;
}

Label CreateHeaderLabel(const char* text)
{
  auto label = Label::New(text);
  label.SetUiScalePolicy(UiScalePolicy::DISABLED);
  label.SetSystemFontSizeScaleEnabled(false);
  label.SetFontSize(14.f);
  label.SetTextColor(UiColor(0x243447));
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}

Label CreateButton(const char* text)
{
  auto button = CreateHeaderLabel(text);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetTextColor(UiColor(0xFFFFFF));
  button.SetBackgroundColor(UiColor(0x365D7C));
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

StackLayout CreateRow()
{
  auto row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(STACK_SPACING);
  return row;
}
} // namespace

class TextScaleController : public ConnectionTracker
{
public:
  explicit TextScaleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextScaleController::OnInit);
  }

private:
  std::array<Label, 4> Labels() const
  {
    return {mTargetLabel, mImageLabel, mFitLabel, mFitCandidateLabel};
  }

  template<typename Action>
  void AddButton(StackLayout row, const char* text, Action action)
  {
    auto button = CreateButton(text);
    button.TouchEventSignal().Connect(this, [action](Actor, TouchEvent event)
    {
      if(event.GetPointCount() > 0u && event.GetState(0u) == PointState::UP) action();
      return true;
    });
    row.Add(button);
  }

  void OnInit(Application application)
  {
    auto window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, 1120, 920));
    window.SetBackgroundColor(UiColor(0xF5F7FA));
    window.KeyEventSignal().Connect(this, &TextScaleController::OnKeyEvent);

    auto root = StackLayout::New(StackOrientation::VERTICAL);
    // Keep the test controls/HUD readable while the targets follow global UI scale.
    root.SetUiScalePolicy(UiScalePolicy::DISABLED);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    auto title = CreateHeaderLabel("Text scale: font, UI, ImageSpan and TextFit");
    title.SetFontSize(22.f);
    root.Add(title);

    auto fonts = CreateRow();
    for(const auto& option : FONT_OPTIONS)
    {
      const std::string caption = std::string(option.key) + ": " + option.name;
      AddButton(fonts, caption.c_str(), [this, value = option.value]()
      { SetSystemFontSize(value); });
    }
    root.Add(fonts);
    auto uiScales = CreateRow();
    for(const auto& option : UI_OPTIONS)
    {
      std::ostringstream caption;
      caption << option.key << ": UI " << option.scale;
      AddButton(uiScales, caption.str().c_str(), [this, scale = option.scale]()
      { SetUiScale(scale); });
    }
    root.Add(uiScales);
    auto modes = CreateRow();
    AddButton(modes, "6: System scale ON", [this]()
    { SetSystemFontSizeScaleEnabled(true); });
    AddButton(modes, "7: System scale OFF", [this]()
    { SetSystemFontSizeScaleEnabled(false); });
    AddButton(modes, "S: Sync Labels", [this]()
    { SetRendering(false); });
    AddButton(modes, "A: Async Labels", [this]()
    { SetRendering(true); });
    root.Add(modes);
    auto clamps = CreateRow();
    for(unsigned i = 0; i < std::size(CLAMP_OPTIONS); ++i)
    {
      const std::string caption = std::string(CLAMP_OPTIONS[i].key) + ": " + CLAMP_OPTIONS[i].label;
      AddButton(clamps, caption.c_str(), [this, i]()
      { ApplyMinMaxScale(i); });
    }
    root.Add(clamps);

    mStatusLabel = CreateHeaderLabel("");
    mStatusLabel.SetBackgroundColor(UiColor(0xE5EDF4));
    mStatusLabel.SetPadding(Insets(12.f, 12.f, 8.f, 8.f));
    root.Add(mStatusLabel);

    mTargetLabel       = Label::New(TEST_TEXT);
    mImageLabel        = Label::New();
    mFitLabel          = Label::New(TEST_TEXT);
    mFitCandidateLabel = Label::New(TEST_TEXT);
    for(auto label : Labels())
    {
      label.SetUiScalePolicy(UiScalePolicy::ENABLED);
      label.SetRequestedWidth(MATCH_PARENT);
      label.SetRequestedHeight(WRAP_CONTENT);
      label.SetMultiLine(true);
      label.SetFontSize(24.f);
      label.SetTextColor(UiColor(0x172B4D));
      label.SetBackgroundColor(UiColor(0xFFFFFF));
      label.SetPadding(Insets(16.f, 16.f, 16.f, 16.f));
    }
    mTargetLabel.SetMinimumHeight(80.f);
    mImageLabel.SetMinimumHeight(80.f);
    auto       builder    = Text::StyledTextBuilder::New("Text ABC ");
    const auto imageIndex = builder.GetUtf32Length();
    builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    Text::ImageAttributes image(RESOURCES_DIR "flag_kr.png", IMAGE_SIZE);
    image.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
    builder.SetSpan(Text::ImageSpan::New(image), imageIndex, imageIndex + 1u);
    builder.AppendText(" DEF");
    mImageLabel.SetStyledText(builder.Build());

    mTargetInputField = InputField::New();
    mTargetInputField.SetUiScalePolicy(UiScalePolicy::ENABLED);
    mTargetInputField.SetRequestedWidth(MATCH_PARENT);
    mTargetInputField.SetRequestedHeight(TARGET_INPUT_HEIGHT);
    mTargetInputField.SetFontSize(24.f);
    mTargetInputField.SetText("InputField test text");
    mTargetInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mTargetInputField.SetPadding(Insets(16.f, 16.f, 16.f, 16.f));

    mFitLabel.SetMaximumHeight(120.f);
    mFitLabel.SetTextFit(Text::Fit::Range(10.f, 20.f, 2.f));
    mFitLabel.SetLineHeight(40.f);
    mFitLabel.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    mFitLabel.SetPadding(Insets(16.f, 16.f, 0.f, 0.f));
    mFitCandidateLabel.SetMaximumHeight(120.f);
    mFitCandidateLabel.SetTextFit(GetFitCandidates());
    mFitCandidateLabel.SetPadding(Insets(16.f, 16.f, 0.f, 0.f));

    auto targets = StackLayout::New(StackOrientation::VERTICAL);
    targets.SetRequestedWidth(MATCH_PARENT);
    targets.SetRequestedHeight(WRAP_CONTENT);
    targets.SetSpacing(STACK_SPACING);
    targets.Add(CreateHeaderLabel("A. Normal Label | authored font 24px"));
    targets.Add(mTargetLabel);
    targets.Add(CreateHeaderLabel("B. ImageSpan Label | flag_kr.png, authored image 36x24 | Text ABC [image] DEF"));
    targets.Add(mImageLabel);
    targets.Add(CreateHeaderLabel("C. InputField | system font scale, synchronous control"));
    targets.Add(mTargetInputField);
    targets.Add(CreateHeaderLabel("D. TextFit Range | 10..20 step 2, absolute line height 40, max height 120"));
    targets.Add(mFitLabel);
    targets.Add(CreateHeaderLabel("E. TextFit Candidates | (font, line height): (10,20)..(20,40), step (2,4)"));
    targets.Add(mFitCandidateLabel);
    auto scroll = ScrollView::New();
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetRequestedHeight(MATCH_PARENT);
    scroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.f).SetAlignment(LayoutAlignment::FILL));
    scroll.SetContent(targets);
    root.Add(scroll);
    window.Add(root);

    auto settings = Dali::Integration::SystemSettings::Get();
    settings.FontSizeChangedSignal().Connect(this, &TextScaleController::OnSystemFontSizeChanged);
    SetRendering(false);
    ApplyMinMaxScale(0u);
    SetSystemFontSizeScaleEnabled(true);
    SetSystemFontSize(SystemFont::NORMAL);
    SetUiScale(1.f);
  }

  void SetSystemFontSize(SystemFont size)
  {
    // Sample/test-only injection through DALi Integration API. This simulates
    // notifications emitted by the platform SystemSettings provider, exercising
    // the real control update path on desktop. Applications must not emit this
    // signal to change the user's system font-size setting.
    Dali::Integration::SystemSettings::Get().FontSizeChangedSignal().Emit(size);
  }

  void OnSystemFontSizeChanged(SystemFont size)
  {
    mSystemFont = size;
    UpdateStatus();
  }

  void SetUiScale(float scale)
  {
    UiScaleManager::Get().SetScale(scale);
    UpdateStatus();
  }

  void SetSystemFontSizeScaleEnabled(bool enabled)
  {
    for(auto label : Labels()) label.SetSystemFontSizeScaleEnabled(enabled);
    mTargetInputField.SetSystemFontSizeScaleEnabled(enabled);
    UpdateStatus();
  }

  void ApplyMinMaxScale(unsigned preset)
  {
    mClampPreset       = preset;
    const auto& option = CLAMP_OPTIONS[preset];
    for(auto label : Labels())
    {
      label.SetMinimumFontSizeScale(option.minimum);
      label.SetMaximumFontSizeScale(option.maximum);
    }
    mTargetInputField.SetMinimumFontSizeScale(option.minimum);
    mTargetInputField.SetMaximumFontSizeScale(option.maximum);
    UpdateStatus();
  }

  void SetRendering(bool async)
  {
    mAsync = async;
    for(auto label : Labels()) label.SetAsyncRendering(async);
    UpdateStatus();
  }

  void UpdateStatus()
  {
    const char* fontName = "UNKNOWN";
    for(const auto& option : FONT_OPTIONS)
      if(option.value == mSystemFont) fontName = option.name;
    const float        font = mTargetLabel.GetAdjustedFontSizeScale();
    const float        ui   = UiScaleManager::Get().GetScale();
    const auto         size = IMAGE_SIZE * (font * ui);
    std::ostringstream status;
    status << std::fixed << std::setprecision(2)
           << "System Font: " << fontName << " | Adjusted Font Scale: " << font
           << " | UI Scale: " << ui << " | Effective Text Scale: " << font * ui
           << "\nSystem Scale Enabled: " << (mTargetLabel.IsSystemFontSizeScaleEnabled() ? "ON" : "OFF")
           << " | Rendering: " << (mAsync ? "ASYNC" : "SYNC")
           << " | InputField: SYNC, adjusted=" << mTargetInputField.GetAdjustedFontSizeScale()
           << "\nClamp: " << CLAMP_OPTIONS[mClampPreset].label
           << " | configured min/max=" << mTargetLabel.GetMinimumFontSizeScale() << "/" << mTargetLabel.GetMaximumFontSizeScale();
    if(mClampPreset == 3u) status << " | min > max: effective range is 1.40..1.40";
    status << "\nImageSpan authored: 36x24 | Expected display: " << size.x << "x" << size.y;
    mStatusLabel.SetText(status.str().c_str());
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP) return;
    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }
    const auto key = event.GetKeyName();
    for(const auto& option : FONT_OPTIONS)
    {
      if(key == option.key)
      {
        SetSystemFontSize(option.value);
        return;
      }
    }
    for(const auto& option : UI_OPTIONS)
    {
      std::string lower(option.key);
      lower[0] = static_cast<char>(lower[0] + ('a' - 'A'));
      if(key == option.key || key == lower.c_str())
      {
        SetUiScale(option.scale);
        return;
      }
    }
    for(unsigned i = 0u; i < std::size(CLAMP_OPTIONS); ++i)
    {
      if(key == CLAMP_OPTIONS[i].key)
      {
        ApplyMinMaxScale(i);
        return;
      }
    }
    if(key == "6")
      SetSystemFontSizeScaleEnabled(true);
    else if(key == "7")
      SetSystemFontSizeScaleEnabled(false);
    else if(key == "s" || key == "S")
      SetRendering(false);
    else if(key == "a" || key == "A")
      SetRendering(true);
  }

  Application& mApplication;
  Label        mTargetLabel;
  Label        mImageLabel;
  InputField   mTargetInputField;
  Label        mFitLabel;
  Label        mFitCandidateLabel;
  Label        mStatusLabel;
  SystemFont   mSystemFont{SystemFont::NORMAL};
  unsigned     mClampPreset{0u};
  bool         mAsync{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  auto application = Application::New(&argc, &argv);
  auto config      = UiConfig::New();
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::SMALL, 0.8f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::NORMAL, 1.f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::LARGE, 1.2f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::EXTRA_LARGE, 1.4f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::GIANT, 1.5f);
  config.Apply();
  TextScaleController controller(application);
  application.MainLoop();
  return 0;
}
