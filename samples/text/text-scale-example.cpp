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
#include <vector>

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
constexpr UiOption UI_OPTIONS[] = {{"Q", 0.8f}, {"W", 1.f}, {"E", 1.2f}, {"R", 1.25f}, {"T", 1.5f}, {"Y", 2.f}};

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

StackLayout CreateControlRow(const char* category)
{
  auto row   = CreateRow();
  auto label = CreateHeaderLabel(category);
  label.SetRequestedWidth(180.f);
  label.SetRequestedHeight(BUTTON_HEIGHT);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  row.Add(label);
  return row;
}

Label CreateSectionHeader(const char* text)
{
  auto label = CreateHeaderLabel(text);
  label.SetFontSize(18.f);
  label.SetFontWeight(Text::FontWeight::BOLD);
  label.SetBackgroundColor(UiColor(0xE5EDF4));
  label.SetPadding(Insets(8.f, 8.f, 6.f, 6.f));
  return label;
}

void AddCaseColumn(StackLayout row, const char* caption, Label label)
{
  auto column = StackLayout::New(StackOrientation::VERTICAL);
  column.SetRequestedHeight(WRAP_CONTENT);
  column.SetSpacing(4.f);
  column.SetLayoutParams(StackLayoutParams::New().SetWeight(1.f).SetAlignment(LayoutAlignment::FILL));
  column.Add(CreateHeaderLabel(caption));
  column.Add(label);
  row.Add(column);
}

Text::Underline MakeUnderline(Text::Underline::Type type = Text::Underline::Type::DASHED, float thickness = 2.f)
{
  Text::Underline style;
  style.SetType(type);
  style.SetThickness(thickness);
  style.SetDashLength(4.f);
  style.SetDashGap(2.f);
  style.SetColor(UiColor(0x0066CC));
  return style;
}

Text::LineThrough MakeLineThrough(float thickness = 2.f)
{
  Text::LineThrough style;
  style.SetThickness(thickness);
  style.SetColor(UiColor(0xC62828));
  return style;
}

Text::StyledText CreateStyledTextCase(Text::Underline::Type type, bool underline, bool lineThrough)
{
  auto builder = Text::StyledTextBuilder::New("ABCD");
  if(underline) builder.SetSpan(Text::UnderlineSpan::New(MakeUnderline(type)), 0u, 4u);
  if(lineThrough) builder.SetSpan(Text::LineThroughSpan::New(MakeLineThrough()), 0u, 4u);
  return builder.Build();
}

Text::StyledText CreateMarkupCase(const char* type, bool underline, bool lineThrough)
{
  std::ostringstream markup;
  if(underline) markup << "<u color='#0066CC' height='2' type='" << type << "' dash-width='4' dash-gap='2'>";
  if(lineThrough) markup << "<s color='#C62828' height='2'>";
  markup << "ABCD";
  if(lineThrough) markup << "</s>";
  if(underline) markup << "</u>";
  return Text::StyledText::FromMarkup(markup.str().c_str());
}

Text::StyledText CreateMixedCase()
{
  auto       builder = Text::StyledTextBuilder::New("plain ");
  const auto start   = builder.GetUtf32Length();
  builder.AppendText("ABCD ");
  const auto imageIndex = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  Text::ImageAttributes image(RESOURCES_DIR "flag_kr.png", IMAGE_SIZE);
  image.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
  builder.SetSpan(Text::ImageSpan::New(image), imageIndex, imageIndex + 1u);
  builder.SetSpan(Text::UnderlineSpan::New(MakeUnderline()), start, imageIndex + 1u);
  auto inner = MakeUnderline(Text::Underline::Type::SOLID, 4.f);
  inner.SetColor(UiColor(0xEF6C00));
  builder.SetSpan(Text::UnderlineSpan::New(inner), start + 1u, start + 3u);
  builder.SetSpan(Text::LineThroughSpan::New(MakeLineThrough()), start + 2u, imageIndex + 1u);
  builder.AppendText(" tail");
  return builder.Build();
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
  const std::vector<Label>& Labels() const
  {
    return mLabels;
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
    window.InterceptKeyEventSignal().Connect(this, &TextScaleController::OnNavigationKeyEvent);

    auto root = StackLayout::New(StackOrientation::VERTICAL);
    // Keep the test controls/HUD readable while the targets follow global UI scale.
    root.SetUiScalePolicy(UiScalePolicy::DISABLED);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));
    auto title = CreateHeaderLabel("Text Scale Test");
    title.SetFontSize(22.f);
    root.Add(title);

    auto pages = CreateControlRow("Page");
    AddButton(pages, "General Scale", [this]()
    { SetPage(0u); });
    AddButton(pages, "Text Style / StyledText", [this]()
    { SetPage(1u); });
    root.Add(pages);

    auto fonts = CreateControlRow("System Font Size Scale");
    for(const auto& option : FONT_OPTIONS)
    {
      const std::string caption = std::string(option.key) + ": " + option.name;
      AddButton(fonts, caption.c_str(), [this, value = option.value]()
      { SetSystemFontSize(value); });
    }
    root.Add(fonts);
    auto uiScales = CreateControlRow("UI Scale");
    for(const auto& option : UI_OPTIONS)
    {
      std::ostringstream caption;
      caption << option.key << ": " << option.scale;
      AddButton(uiScales, caption.str().c_str(), [this, scale = option.scale]()
      { SetUiScale(scale); });
    }
    root.Add(uiScales);
    auto systemScale = CreateControlRow("System Scale");
    AddButton(systemScale, "6: ON", [this]()
    { SetSystemFontSizeScaleEnabled(true); });
    AddButton(systemScale, "7: OFF", [this]()
    { SetSystemFontSizeScaleEnabled(false); });
    root.Add(systemScale);
    auto modes = CreateControlRow("Rendering");
    AddButton(modes, "S: Sync Labels", [this]()
    { SetRendering(false); });
    AddButton(modes, "A: Async Labels", [this]()
    { SetRendering(true); });
    root.Add(modes);
    auto clamps = CreateControlRow("Clamp");
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
    mLabels            = {mTargetLabel, mImageLabel, mFitLabel, mFitCandidateLabel};
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
    mTargetInputField.SetTextColor(UiColor(0x172B4D));
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
    targets.Add(CreateHeaderLabel("A. Normal Label | authored font 24px | plain text"));
    targets.Add(mTargetLabel);
    targets.Add(CreateHeaderLabel("B. ImageSpan Label | flag_kr.png, authored image 36x24 | Text ABC [image] DEF"));
    targets.Add(mImageLabel);
    targets.Add(CreateHeaderLabel("C. InputField | system font scale, synchronous control"));
    targets.Add(mTargetInputField);
    targets.Add(CreateHeaderLabel("D. TextFit Range | 10..20 step 2, absolute line height 40, max height 120"));
    targets.Add(mFitLabel);
    targets.Add(CreateHeaderLabel("E. TextFit Candidates | (font, line height): (10,20)..(20,40), step (2,4)"));
    targets.Add(mFitCandidateLabel);
    mPages[0] = targets;
    mPages[1] = CreateStylePage();
    mScroll   = ScrollView::New();
    mScroll.SetScrollDirection(ScrollDirection::Vertical);
    mScroll.SetRequestedWidth(MATCH_PARENT);
    mScroll.SetRequestedHeight(MATCH_PARENT);
    mScroll.SetLayoutParams(StackLayoutParams::New().SetWeight(1.f).SetAlignment(LayoutAlignment::FILL));
    mScroll.SetContent(mPages[0]);
    root.Add(mScroll);
    window.Add(root);

    auto settings = Dali::Integration::SystemSettings::Get();
    settings.FontSizeChangedSignal().Connect(this, &TextScaleController::OnSystemFontSizeChanged);
    SetRendering(false);
    ApplyMinMaxScale(0u);
    SetSystemFontSizeScaleEnabled(true);
    SetSystemFontSize(SystemFont::NORMAL);
    SetUiScale(1.f);
  }

  Label CreateStyleLabel()
  {
    auto label = Label::New("ABCD");
    label.SetUiScalePolicy(UiScalePolicy::ENABLED);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetMultiLine(true);
    label.SetFontSize(24.f);
    label.SetTextColor(UiColor(0x172B4D));
    label.SetBackgroundColor(UiColor(0xFFFFFF));
    label.SetPadding(Insets(12.f, 12.f, 8.f, 8.f));
    mLabels.push_back(label);
    return label;
  }

  void CreateStyleRow(StackLayout page, const char* caption, Text::Underline::Type type,
                      const char* markupType, bool underline, bool lineThrough)
  {
    page.Add(CreateHeaderLabel(caption));
    auto row     = CreateRow();
    auto control = CreateStyleLabel();
    if(underline) control.SetTextUnderline(MakeUnderline(type));
    if(lineThrough) control.SetTextLineThrough(MakeLineThrough());
    auto styled = CreateStyleLabel();
    styled.SetStyledText(CreateStyledTextCase(type, underline, lineThrough));
    auto markup = CreateStyleLabel();
    markup.SetStyledText(CreateMarkupCase(markupType, underline, lineThrough));
    AddCaseColumn(row, "Control Text Style", control);
    AddCaseColumn(row, "StyledText Span", styled);
    AddCaseColumn(row, "Markup", markup);
    page.Add(row);
  }

  StackLayout CreateStylePage()
  {
    // Build controls and immutable StyledText snapshots once. Scale and page
    // changes reuse these objects; they never call SetStyledText again.
    auto page = StackLayout::New(StackOrientation::VERTICAL);
    page.SetRequestedWidth(MATCH_PARENT);
    page.SetRequestedHeight(WRAP_CONTENT);
    page.SetSpacing(STACK_SPACING);
    page.Add(CreateHeaderLabel("Tab: switch page | PgUp/PgDn: scroll | All samples: font 24px. Row values are authored."));
    page.Add(CreateHeaderLabel("F5 vs F6: same 48px font, decoration 2x vs 1x. UI round trip: W > R > T > Y > W."));
    auto comparison = CreateControlRow("Style Comparison");
    AddButton(comparison, "F5: UI=2 / F=1", [this]()
    { SetContractPreset(false); });
    AddButton(comparison, "F6: UI=1 / F=2", [this]()
    { SetContractPreset(true); });
    page.Add(comparison);
    using Type = Text::Underline::Type;
    page.Add(CreateSectionHeader("Underline"));
    CreateStyleRow(page, "1. Underline solid | thickness 2", Type::SOLID, "solid", true, false);
    CreateStyleRow(page, "2. Underline dashed | thickness 2, dash length 4 / gap 2", Type::DASHED, "dashed", true, false);
    CreateStyleRow(page, "3. Underline double | thickness 2", Type::DOUBLE, "double", true, false);
    page.Add(CreateSectionHeader("LineThrough"));
    CreateStyleRow(page, "4. LineThrough | thickness 2", Type::SOLID, "solid", false, true);
    CreateStyleRow(page, "5. Underline + LineThrough | both thickness 2, dash 4/2 | Markup: nested <u><s>", Type::DASHED, "dashed", true, true);

    page.Add(CreateSectionHeader("Shadow / Outline"));
    page.Add(CreateHeaderLabel("6. Control-only effects | compare signed offsets against plain text"));
    auto effects = CreateRow();
    AddCaseColumn(effects, "Plain reference", CreateStyleLabel());
    auto         shadowLabel = CreateStyleLabel();
    Text::Shadow shadow;
    shadow.SetOffset(Vector2(-2.f, 3.f));
    shadow.SetColor(UiColor(0x607D8B));
    shadowLabel.SetTextShadow(shadow);
    AddCaseColumn(effects, "Shadow offset (-2,+3)", shadowLabel);
    auto          outlineLabel = CreateStyleLabel();
    Text::Outline outline;
    outline.SetWidth(2.f);
    outline.SetOffset(Vector2(2.f, -3.f));
    outline.SetColor(UiColor(0xEF6C00));
    outlineLabel.SetTextOutline(outline);
    AddCaseColumn(effects, "Outline width 2, offset (+2,-3)", outlineLabel);
    page.Add(effects);

    page.Add(CreateSectionHeader("Overlapping StyledText"));
    page.Add(CreateHeaderLabel("7. ABCD, UTF-32 ranges [start,end); later orange span wins"));
    auto overlaps = CreateRow();
    auto under    = Text::StyledTextBuilder::New("ABCD");
    under.SetSpan(Text::UnderlineSpan::New(MakeUnderline()), 0u, 4u);
    auto innerUnderline = MakeUnderline(Type::SOLID, 4.f);
    innerUnderline.SetColor(UiColor(0xEF6C00));
    under.SetSpan(Text::UnderlineSpan::New(innerUnderline), 1u, 3u);
    auto underLabel = CreateStyleLabel();
    underLabel.SetStyledText(under.Build());
    AddCaseColumn(overlaps, "Underline: [0,4) 2, dash 4/2\nthen [1,3) solid 4", underLabel);
    auto strike = Text::StyledTextBuilder::New("ABCD");
    strike.SetSpan(Text::LineThroughSpan::New(MakeLineThrough()), 0u, 4u);
    auto innerStrike = MakeLineThrough(4.f);
    innerStrike.SetColor(UiColor(0xEF6C00));
    strike.SetSpan(Text::LineThroughSpan::New(innerStrike), 1u, 3u);
    auto strikeLabel = CreateStyleLabel();
    strikeLabel.SetStyledText(strike.Build());
    AddCaseColumn(overlaps, "LineThrough: [0,4) 2\nthen [1,3) 4", strikeLabel);
    auto crossed = Text::StyledTextBuilder::New("ABCD");
    crossed.SetSpan(Text::UnderlineSpan::New(MakeUnderline()), 0u, 3u);
    crossed.SetSpan(Text::LineThroughSpan::New(MakeLineThrough()), 1u, 4u);
    auto crossedLabel = CreateStyleLabel();
    crossedLabel.SetStyledText(crossed.Build());
    AddCaseColumn(overlaps, "Underline [0,3) 2, dash 4/2\nLineThrough [1,4) 2", crossedLabel);
    page.Add(overlaps);

    page.Add(CreateSectionHeader("Mixed StyledText + ImageSpan"));
    page.Add(CreateHeaderLabel("8. Underline 2, dash 4/2; BC underline 4; CD + image LineThrough 2; flag 36x24"));
    mMixedText = CreateMixedCase();
    auto mixed = CreateStyleLabel();
    mixed.SetStyledText(mMixedText);
    page.Add(mixed);
    page.Add(CreateSectionHeader("Editable"));
    page.Add(CreateHeaderLabel("9. InputField (SYNC) | same mixed StyledText and authored values as row 8"));
    mStyleInputField = InputField::New();
    mStyleInputField.SetUiScalePolicy(UiScalePolicy::ENABLED);
    mStyleInputField.SetRequestedWidth(MATCH_PARENT);
    mStyleInputField.SetRequestedHeight(WRAP_CONTENT);
    mStyleInputField.SetFontSize(24.f);
    mStyleInputField.SetTextColor(UiColor(0x172B4D));
    mStyleInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mStyleInputField.SetPadding(Insets(12.f, 12.f, 8.f, 8.f));
    mStyleInputField.SetStyledText(mMixedText);
    page.Add(mStyleInputField);
    return page;
  }

  void SetPage(unsigned page)
  {
    if(page == mPage) return;
    mPage = page;
    mScroll.SetContent(mPages[page]);
    mScroll.ScrollTo(Vector2::ZERO, false);
    UpdateStatus();
  }

  void SetContractPreset(bool fontOnly)
  {
    SetSystemFontSizeScaleEnabled(true);
    ApplyMinMaxScale(fontOnly ? 4u : 0u);
    SetSystemFontSize(SystemFont::NORMAL);
    SetUiScale(fontOnly ? 1.f : 2.f);
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
    mStyleInputField.SetSystemFontSizeScaleEnabled(enabled);
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
    mStyleInputField.SetMinimumFontSizeScale(option.minimum);
    mStyleInputField.SetMaximumFontSizeScale(option.maximum);
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
           << "Page: " << (mPage == 0u ? "General Scale" : "Text Style / StyledText")
           << "\nSystem Font: " << fontName << " | Adjusted Font Scale: " << font
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

  bool OnNavigationKeyEvent(Window, KeyEvent event)
  {
    const auto key = event.GetKeyName();
    // Reserve page navigation before an editable control or FocusManager can
    // consume it. Other keys retain the sample's existing input behavior.
    if(key != "Tab" && key != "F5" && key != "F6" && key != "Prior" && key != "Next") return false;
    if(event.GetState() != KeyEvent::UP) return true;
    if(key == "Tab")
    {
      SetPage(1u - mPage);
    }
    else if(key == "F5" || key == "F6")
    {
      SetContractPreset(key == "F6");
    }
    else
    {
      mScroll.ScrollToY(mScroll.GetScrollPosition().y + (key == "Next" ? 1.f : -1.f) * mScroll.GetCurrentSize().y * 0.8f, false);
    }
    return true;
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
  std::vector<Label>         mLabels;
  std::array<StackLayout, 2> mPages;
  ScrollView                 mScroll;
  InputField                 mStyleInputField;
  Text::StyledText           mMixedText;
  unsigned                   mPage{0u};
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
