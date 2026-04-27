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
#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float WINDOW_WIDTH       = 1600.0f;
constexpr float WINDOW_HEIGHT      = 960.0f;
constexpr float ROOT_PADDING       = 24.0f;
constexpr float ROOT_SPACING       = 18.0f;
constexpr float SECTION_SPACING    = 16.0f;
constexpr float PANEL_SPACING      = 24.0f;
constexpr float PANEL_WIDTH        = 720.0f;
constexpr float PANEL_HEIGHT       = 640.0f;
constexpr float TEXT_PADDING       = 24.0f;
constexpr float EXCLUSION_LABEL_H  = 36.0f;
constexpr float TITLE_HEIGHT       = 42.0f;
constexpr float INFO_HEIGHT        = 84.0f;
constexpr float STATUS_HEIGHT      = 56.0f;
constexpr float TEXT_FONT_SIZE     = 24.0f;
constexpr float TITLE_FONT_SIZE    = 18.0f;
constexpr float BODY_FONT_SIZE     = 14.0f;

const UiColor WINDOW_BG_COLOR(0x11161D);
const UiColor PANEL_BG_COLOR(0x182330);
const UiColor PANEL_BORDER_COLOR(0x38506C);
const UiColor TITLE_BG_COLOR(0x243447);
const UiColor LABEL_TEXT_COLOR(0xF3F6FA);
const UiColor STATUS_BG_COLOR(0x202E3D);
const UiColor OVERLAY_COLOR(0xF5528A);
const UiColor TEXT_AREA_BG_COLOR = UiColor(0xFFFFFF).WithAlpha(0.07f);
const UiColor FALLBACK_LABEL_BG_COLOR = UiColor(0x203040).WithAlpha(0.45f);

const UiColor TEXT_COLORS[] = {
  UiColor(0xFFB000),
  UiColor(0x00D1C1),
  UiColor(0x8CC8FF),
  UiColor(0xFFFFFF),
};

const char* TEXT_COLOR_NAMES[] = {
  "Amber",
  "Mint",
  "Sky",
  "White",
};

const Rect<float> EXCLUSION_PRESETS[] = {
  Rect<float>(150.0f, 110.0f, 220.0f, 170.0f),
  Rect<float>(280.0f, 250.0f, 180.0f, 200.0f),
  Rect<float>(110.0f, 430.0f, 260.0f, 120.0f),
};

const char* SAMPLE_TEXT =
  "TextVisualizer visual verification sample. English text should wrap naturally while Korean "
  "문장도 함께 보이고, emoji 😀🚀🌊 도 포함되어 atlas render path를 수동 확인할 수 있어야 합니다. "
  "같은 문자열을 오른쪽 패널에 다시 그리되, 분홍색 exclusion box 영역은 비워져야 합니다. "
  "키 1로 exclusion on/off, 키 2로 text color 변경, 키 3으로 exclusion 위치 이동을 반복해서 확인합니다.";

Label CreateInfoLabel(const char* text, float height, float fontSize)
{
  return Label::New(text)
    .SetRequestedWidth(MATCH_PARENT)
    .SetRequestedHeight(height)
    .SetFontSize(fontSize)
    .SetTextColor(LABEL_TEXT_COLOR)
    .SetMultiLine(true)
    .SetVerticalTextAlignment(Text::Alignment::CENTER)
    .SetPadding(Extents(12, 12, 10, 10))
    .SetBackgroundColor(TITLE_BG_COLOR);
}

StackLayout CreateSectionContainer(const char* title)
{
  return StackLayout::New(StackOrientation::VERTICAL)
    .SetRequestedWidth(PANEL_WIDTH)
    .SetRequestedHeight(WRAP_CONTENT)
    .SetSpacing(8.0f)
    .Children({
      Label::New(title)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(TITLE_HEIGHT)
        .SetFontSize(TITLE_FONT_SIZE)
        .SetTextColor(LABEL_TEXT_COLOR)
        .SetHorizontalTextAlignment(Text::Alignment::CENTER)
        .SetVerticalTextAlignment(Text::Alignment::CENTER)
        .SetBackgroundColor(TITLE_BG_COLOR)
    });
}
} // namespace

class TextVisualizerSampleController : public ConnectionTracker
{
public:
  explicit TextVisualizerSampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextVisualizerSampleController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(WINDOW_BG_COLOR);
    window.SetSize(Dali::Window::WindowSize(static_cast<uint32_t>(WINDOW_WIDTH), static_cast<uint32_t>(WINDOW_HEIGHT)));
    window.KeyEventSignal().Connect(this, &TextVisualizerSampleController::OnKeyEvent);

    mStatusLabel = CreateInfoLabel("", STATUS_HEIGHT, BODY_FONT_SIZE);

    mPlainVisualizer = CreateTextVisualizer();
    mExcludedVisualizer = CreateTextVisualizer();
    mPlainFallbackLabel = CreateFallbackLabel();
    mExcludedFallbackLabel = CreateFallbackLabel();
    mExclusionOverlay = CreateExclusionOverlay();

    View plainPanel = CreateTextPanel(mPlainVisualizer, View(), mPlainFallbackLabel, "Same text without exclusion");
    View excludedPanel = CreateTextPanel(mExcludedVisualizer, mExclusionOverlay, mExcludedFallbackLabel, "Same text with visible exclusion box");

    StackLayout sections = StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetSpacing(PANEL_SPACING)
      .Children({
        CreateSection("Baseline", plainPanel),
        CreateSection("Exclusion", excludedPanel),
      });

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(ROOT_PADDING, ROOT_PADDING, ROOT_PADDING, ROOT_PADDING))
        .SetSpacing(ROOT_SPACING)
        .Children({
          CreateInfoLabel("TextVisualizer sample\n1: Toggle exclusion   2: Change text color   3: Move exclusion   4: Toggle fallback Label   ESC/BACK: Quit",
                          INFO_HEIGHT,
                          BODY_FONT_SIZE),
          mStatusLabel,
          sections
        }));

    ApplySampleState();
  }

  TextVisualizer CreateTextVisualizer()
  {
    TextVisualizer visualizer = TextVisualizer::New();
    visualizer.SetLayoutMode(LayoutMode::STANDALONE);
    visualizer.SetPositionX(TEXT_PADDING);
    visualizer.SetPositionY(TEXT_PADDING);
    visualizer.SetRequestedWidth(PANEL_WIDTH - (TEXT_PADDING * 2.0f));
    visualizer.SetRequestedHeight(PANEL_HEIGHT - (TEXT_PADDING * 2.0f));
    visualizer.SetBackgroundColor(TEXT_AREA_BG_COLOR);
    visualizer.SetText(SAMPLE_TEXT);
    visualizer.SetFontSize(TEXT_FONT_SIZE);
    visualizer.SetTextColor(TEXT_COLORS[mCurrentColorIndex]);
    return visualizer;
  }

  View CreateExclusionOverlay()
  {
    View overlay = View::New();
    overlay.SetLayoutMode(LayoutMode::STANDALONE);
    overlay.SetRequestedWidth(0.0f);
    overlay.SetRequestedHeight(0.0f);
    overlay.SetPositionX(TEXT_PADDING);
    overlay.SetPositionY(TEXT_PADDING);
    overlay.SetBackgroundColor(OVERLAY_COLOR.WithAlpha(0.35f));
    return overlay;
  }

  Label CreateFallbackLabel()
  {
    Label fallback = Label::New(SAMPLE_TEXT);
    fallback.SetLayoutMode(LayoutMode::STANDALONE);
    fallback.SetPositionX(TEXT_PADDING);
    fallback.SetPositionY(TEXT_PADDING);
    fallback.SetRequestedWidth(PANEL_WIDTH - (TEXT_PADDING * 2.0f));
    fallback.SetRequestedHeight(PANEL_HEIGHT - (TEXT_PADDING * 2.0f));
    fallback.SetFontSize(TEXT_FONT_SIZE);
    fallback.SetTextColor(TEXT_COLORS[mCurrentColorIndex]);
    fallback.SetMultiLine(true);
    fallback.SetPadding(Extents(8, 8, 8, 8));
    fallback.SetBackgroundColor(FALLBACK_LABEL_BG_COLOR);
    fallback.SetProperty(Actor::Property::VISIBLE, false);
    return fallback;
  }

  View CreateTextPanel(TextVisualizer visualizer, View overlay, Label fallbackLabel, const char* caption)
  {
    View panel = View::New();
    panel.SetRequestedWidth(PANEL_WIDTH);
    panel.SetRequestedHeight(PANEL_HEIGHT);
    panel.SetBackgroundColor(PANEL_BG_COLOR);
    panel.SetPadding(Extents(0, 0, 0, 0));
    panel.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);

    panel.Add(View::New()
                .SetLayoutMode(LayoutMode::STANDALONE)
                .SetPositionX(TEXT_PADDING)
                .SetPositionY(TEXT_PADDING)
                .SetRequestedWidth(PANEL_WIDTH - (TEXT_PADDING * 2.0f))
                .SetRequestedHeight(PANEL_HEIGHT - (TEXT_PADDING * 2.0f))
                .SetBackgroundColor(TEXT_AREA_BG_COLOR));

    panel.Add(visualizer);

    if(fallbackLabel)
    {
      panel.Add(fallbackLabel);
    }

    if(overlay)
    {
      panel.Add(overlay);
      panel.Add(Label::New("Exclusion region")
                  .SetLayoutMode(LayoutMode::STANDALONE)
                  .SetPositionX(TEXT_PADDING)
                  .SetPositionY(PANEL_HEIGHT - EXCLUSION_LABEL_H - 8.0f)
                  .SetRequestedWidth(210.0f)
                  .SetRequestedHeight(EXCLUSION_LABEL_H)
                  .SetFontSize(BODY_FONT_SIZE)
                  .SetTextColor(LABEL_TEXT_COLOR)
                  .SetVerticalTextAlignment(Text::Alignment::CENTER)
                  .SetPadding(Extents(10, 10, 0, 0))
                  .SetBackgroundColor(TITLE_BG_COLOR.WithAlpha(0.85f)));
    }

    panel.Add(Label::New(caption)
                .SetLayoutMode(LayoutMode::STANDALONE)
                .SetPositionX(12.0f)
                .SetPositionY(12.0f)
                .SetRequestedWidth(280.0f)
                .SetRequestedHeight(34.0f)
                .SetFontSize(BODY_FONT_SIZE)
                .SetTextColor(LABEL_TEXT_COLOR)
                .SetVerticalTextAlignment(Text::Alignment::CENTER)
                .SetPadding(Extents(10, 10, 0, 0))
                .SetBackgroundColor(TITLE_BG_COLOR.WithAlpha(0.88f)));

    return panel;
  }

  StackLayout CreateSection(const char* title, View panel)
  {
    StackLayout section = CreateSectionContainer(title);
    section.Add(panel);
    return section;
  }

  void ApplySampleState()
  {
    const UiColor textColor = TEXT_COLORS[mCurrentColorIndex];
    mPlainVisualizer.SetTextColor(textColor);
    mExcludedVisualizer.SetTextColor(textColor);
    mPlainFallbackLabel.SetTextColor(textColor);
    mExcludedFallbackLabel.SetTextColor(textColor);
    mPlainFallbackLabel.SetProperty(Actor::Property::VISIBLE, mFallbackLabelEnabled);
    mExcludedFallbackLabel.SetProperty(Actor::Property::VISIBLE, mFallbackLabelEnabled);

    if(mExclusionEnabled)
    {
      Dali::Vector<Rect<float>> regions;
      regions.PushBack(EXCLUSION_PRESETS[mCurrentExclusionIndex]);
      mExcludedVisualizer.SetExclusionRegions(regions);

      const Rect<float>& region = EXCLUSION_PRESETS[mCurrentExclusionIndex];
      mExclusionOverlay.SetProperty(Actor::Property::VISIBLE, true);
      mExclusionOverlay.SetPositionX(TEXT_PADDING + region.x);
      mExclusionOverlay.SetPositionY(TEXT_PADDING + region.y);
      mExclusionOverlay.SetRequestedWidth(region.width);
      mExclusionOverlay.SetRequestedHeight(region.height);
    }
    else
    {
      mExcludedVisualizer.ClearExclusionRegions();
      mExclusionOverlay.SetProperty(Actor::Property::VISIBLE, false);
      mExclusionOverlay.SetRequestedWidth(0.0f);
      mExclusionOverlay.SetRequestedHeight(0.0f);
    }

    Dali::String status("Color: ");
    status += TEXT_COLOR_NAMES[mCurrentColorIndex];
    status += " | Exclusion: ";
    status += (mExclusionEnabled ? "ON" : "OFF");
    status += " | Preset: ";
    status += std::to_string(mCurrentExclusionIndex + 1u).c_str();
    status += " | Fallback Label: ";
    status += (mFallbackLabelEnabled ? "ON" : "OFF");
    status += "\nSee log: TextVisualizer render diagnostics | Fallback Label: key 4";
    status += "\nManual check: if Label is visible but TextVisualizer glyphs are not, the atlas output path is still the main suspect.";
    mStatusLabel.SetText(status);

    DALI_LOG_ERROR("TextVisualizer sample state -> color=%s exclusion=%d preset=%u fallback=%d\n",
                   TEXT_COLOR_NAMES[mCurrentColorIndex],
                   mExclusionEnabled,
                   static_cast<unsigned int>(mCurrentExclusionIndex),
                   mFallbackLabelEnabled);
  }

  void OnKeyEvent(const KeyEvent& event)
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
      mExclusionEnabled = !mExclusionEnabled;
      ApplySampleState();
    }
    else if(event.GetKeyName() == "2")
    {
      mCurrentColorIndex = (mCurrentColorIndex + 1u) % (sizeof(TEXT_COLORS) / sizeof(TEXT_COLORS[0]));
      ApplySampleState();
    }
    else if(event.GetKeyName() == "3")
    {
      mCurrentExclusionIndex = (mCurrentExclusionIndex + 1u) % (sizeof(EXCLUSION_PRESETS) / sizeof(EXCLUSION_PRESETS[0]));
      ApplySampleState();
    }
    else if(event.GetKeyName() == "4")
    {
      mFallbackLabelEnabled = !mFallbackLabelEnabled;
      ApplySampleState();
    }
  }

private:
  Application& mApplication;
  Label          mStatusLabel;
  TextVisualizer mPlainVisualizer;
  TextVisualizer mExcludedVisualizer;
  Label          mPlainFallbackLabel;
  Label          mExcludedFallbackLabel;
  View           mExclusionOverlay;
  bool           mExclusionEnabled{true};
  bool           mFallbackLabelEnabled{false};
  uint32_t       mCurrentColorIndex{0u};
  uint32_t       mCurrentExclusionIndex{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);

  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextVisualizerSampleController controller(application);
  application.MainLoop();

  return 0;
}
