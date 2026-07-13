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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING    = 12.0f;
constexpr float HEADER_PADDING   = 12.0f;
constexpr float HEADER_ROW_GAP   = 8.0f;
constexpr float HEADER_BADGE_HEIGHT = 30.0f;
constexpr float HEADER_INFO_HEIGHT  = 34.0f;
constexpr float CONTENT_PADDING  = 16.0f;
constexpr float FOOTER_PADDING   = 12.0f;
constexpr float FOOTER_ROW_GAP   = 8.0f;
constexpr float FOOTER_TITLE_WIDTH = 120.0f;
constexpr float FOOTER_BADGE_HEIGHT = 28.0f;
constexpr float FOOTER_LINE_HEIGHT  = 24.0f;
constexpr float HEADER_HEIGHT     = HEADER_PADDING + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_INFO_HEIGHT + HEADER_PADDING;
constexpr float FOOTER_HEIGHT     = FOOTER_PADDING + FOOTER_BADGE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_PADDING;
constexpr float MATRIX_TITLE_HEIGHT = 18.0f;
constexpr float MATRIX_HEADER_HEIGHT = 20.0f;
constexpr float MATRIX_HORIZONTAL_ROW_HEIGHT = 30.0f;
constexpr float MATRIX_VERTICAL_ROW_HEIGHT   = 72.0f;
constexpr float MATRIX_CAPTION_WIDTH = 76.0f;
constexpr float MATRIX_CELL_WIDTH    = 188.0f;
constexpr int   WINDOW_WIDTH     = 920;
constexpr int   WINDOW_HEIGHT    = 880;
constexpr std::size_t CASE_COUNT = 10u;
constexpr std::size_t INITIAL_CASE_INDEX                 = 0u;
constexpr std::size_t INITIAL_ALIGNMENT_INDEX            = 1u;
constexpr std::size_t INITIAL_SPREAD_METHOD_INDEX        = 0u;
constexpr std::size_t INITIAL_GRADIENT_BOUNDS_MODE_INDEX = 0u;
constexpr uint32_t BADGE_DISABLED_BACKGROUND             = 0x1E293B;
constexpr uint32_t BADGE_DISABLED_BORDER                 = 0x475569;
constexpr uint32_t BADGE_DISABLED_TEXT                   = 0xCBD5E1;
constexpr uint32_t BADGE_READY_BACKGROUND                = 0x075985;
constexpr uint32_t BADGE_READY_BORDER                    = 0x38BDF8;
constexpr uint32_t BADGE_APPLY_BACKGROUND                = 0x047857;
constexpr uint32_t BADGE_APPLY_BORDER                    = 0x6EE7B7;
constexpr uint32_t BADGE_ON_TEXT                         = 0xF8FAFC;
constexpr float OVERLAY_EFFECT_SHIMMER_START_OFFSET      = 1.0f;
constexpr float OVERLAY_EFFECT_SHIMMER_END_OFFSET        = -1.0f;
constexpr float OVERLAY_EFFECT_SHIMMER_DURATION          = 1.4f;

enum class GradientKind
{
  NONE,
  LINEAR,
  RADIAL,
  CONIC
};

enum class PreviewSizeMode
{
  FIXED,
  WRAP
};

enum class OverlayFillMode
{
  EFFECT,
  FULL
};

struct CaseDefinition
{
  const char*  title;
  const char*  text;
  const char*  expected;
  const char*  notes;
  GradientKind gradient;
  float        fontSize;
  bool         multiline;
  bool         markup;
  bool         style;
  bool         marquee{false};
  Text::MarqueeOrientation marqueeOrientation{Text::MarqueeOrientation::HORIZONTAL};
  bool         marqueeDefaultRunning{true};
  bool         compactGradientSpan{false};
};

constexpr std::array<CaseDefinition, CASE_COUNT> CASES{{
  {
    "Simple TextGradient",
    "Gradient Rendering Sample",
    "Expected: selected gradient type visible on the simple Label path.",
    "Notes: single-color, monochrome, non-marquee, non-tiling, no style.",
    GradientKind::LINEAR,
    52.0f,
    false,
    false,
    false,
  },
  {
    "Large TextGradient",
    "Gradient coverage sample fills this multiline label with enough words to span a broad preview area, "
    "making linear, radial, and conic text gradients easier to inspect across ascenders, descenders, spaces, "
    "punctuation, and wrapped lines while changing spread methods and bounds modes during manual testing.",
    "Expected: dense multiline text should make the selected gradient type visible across a wide text area.",
    "Notes: long single-color Label path for inspecting large gradient coverage and wrapped line bounds.",
    GradientKind::LINEAR,
    28.0f,
    true,
    false,
    false,
  },
  {
    "Multiline TextGradient",
    "Text Gradient\nMultiline Label\nDALi UI Foundation",
    "Expected: useful for future bounds and multiline policy checks.",
    "Notes: current implementation maps TextGradient to logical text bounds.",
    GradientKind::LINEAR,
    40.0f,
    true,
    false,
    false,
  },
  {
    "Explicit Color Markup Mixed",
    "Learn how<color value='black'> to make more content appear in your Now brief.</color>",
    "Expected: unmarked glyphs use gradient, explicit black markup stays black; M toggles horizontal marquee.",
    "Notes: checks mixed multiple-color TextGradient and optional horizontal marquee.",
    GradientKind::LINEAR,
    32.0f,
    false,
    true,
    false,
    true,
    Text::MarqueeOrientation::HORIZONTAL,
    false,
  },
  {
    "Emoji Mixed",
    "Gradient 😀 Text 🌈 Test",
    "Expected: default glyphs use gradient, emoji/color glyphs keep original color; M toggles horizontal marquee.",
    "Notes: verifies color glyph preservation through the mixed marquee preserved/mask path; visible emoji depends on available color emoji fonts.",
    GradientKind::LINEAR,
    46.0f,
    false,
    false,
    false,
    true,
    Text::MarqueeOrientation::HORIZONTAL,
    false,
  },
  {
    "Emoji Mixed Underline",
    "Thinking 😀 with Gradient 🌈 Underline",
    "Expected: default glyphs use gradient, emoji keeps color, shadow stays below fill, and underline remains above fill in normal and marquee modes.",
    "Notes: verifies mixed preserved/mask composition with below-fill style and overlay-style underline.",
    GradientKind::LINEAR,
    40.0f,
    false,
    false,
    true,
    true,
    Text::MarqueeOrientation::HORIZONTAL,
    false,
  },
  {
    "Shadow And Underline Style",
    "Styled Text Gradient",
    "Expected: fill uses gradient, shadow stays below fill, and underline stays above fill in normal and marquee modes.",
    "Notes: checks below-fill style texture and overlay-style underline with marquee.",
    GradientKind::LINEAR,
    50.0f,
    false,
    false,
    true,
    true,
    Text::MarqueeOrientation::HORIZONTAL,
    false,
  },
  {
    "Spread Method",
    "Text gradient spread method inspection across the selected type",
    "Expected: P cycles PAD, REFLECT and REPEAT; the compact span makes spread behavior visible.",
    "Notes: TYPE toggles Linear/Radial/Conic so spread can be checked on each gradient evaluator.",
    GradientKind::LINEAR,
    46.0f,
    false,
    false,
    false,
    false,
    Text::MarqueeOrientation::HORIZONTAL,
    true,
    true,
  },
  {
    "Horizontal Marquee TextGradient",
    "Horizontal marquee text gradient keeps moving with the scrolling content",
    "Expected: moving text fill keeps the selected gradient type while scrolling horizontally.",
    "Notes: verifies horizontal marquee shader with TextGradient.",
    GradientKind::LINEAR,
    42.0f,
    false,
    false,
    false,
    true,
    Text::MarqueeOrientation::HORIZONTAL,
  },
  {
    "Vertical Marquee TextGradient",
    "Vertical\nmarquee\ntext\ngradient\nscrolls\nwith\ncontent",
    "Expected: moving text fill keeps the selected gradient type while scrolling vertically.",
    "Notes: verifies vertical marquee shader with TextGradient.",
    GradientKind::LINEAR,
    38.0f,
    true,
    false,
    false,
    true,
    Text::MarqueeOrientation::VERTICAL,
  },
}};

constexpr std::array<Text::Alignment, 3u> ALIGNMENTS{{
  Text::Alignment::START,
  Text::Alignment::CENTER,
  Text::Alignment::END,
}};

constexpr std::array<Gradient::SpreadMethod, 3u> SPREAD_METHODS{{
  Gradient::SpreadMethod::PAD,
  Gradient::SpreadMethod::REFLECT,
  Gradient::SpreadMethod::REPEAT,
}};

constexpr std::array<GradientKind, 3u> OVERLAY_GRADIENT_KINDS{{
  GradientKind::LINEAR,
  GradientKind::RADIAL,
  GradientKind::CONIC,
}};

constexpr std::array<Text::GradientBoundsMode, 2u> GRADIENT_BOUNDS_MODES{{
  Text::GradientBoundsMode::CONTENT_BOUND,
  Text::GradientBoundsMode::VIEW_BOUND,
}};

constexpr std::array<Text::GradientOverlayMode, 2u> OVERLAY_MODES{{
  Text::GradientOverlayMode::SRC_OVER,
  Text::GradientOverlayMode::SCREEN,
}};

Label CreateLabel(const char* text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

void ConfigureHudBadge(Label label, float height, const UiColor& backgroundColor, const UiColor& borderlineColor, const UiColor& textColor, Text::Alignment horizontalAlignment)
{
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetTextColor(textColor);
  label.SetMultiLine(false);
  label.SetHorizontalTextAlignment(horizontalAlignment);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetBackgroundColor(backgroundColor);
  label.SetCornerRadius(7.0f);
  label.SetBorderlineWidth(1.0f);
  label.SetBorderlineOffset(-1.0f);
  label.SetBorderlineColor(borderlineColor);
  label.SetPadding(Extents(10.0f, 10.0f, 0.0f, 0.0f));
}

void SetCommonGradientStops(Gradient::Base& gradient)
{
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(1.0f, 0.12f, 0.20f, 1.0f)),
    Gradient::StopNode(0.48f, UiColor(0.08f, 0.74f, 0.42f, 1.0f)),
    Gradient::StopNode(1.0f, UiColor(0.12f, 0.38f, 1.0f, 1.0f)),
  });
}

void SetOverlayEffectGradientStops(Gradient::Base& gradient)
{
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)),
    Gradient::StopNode(0.08f, UiColor(1.0f, 1.0f, 1.0f, 0.16f)),
    Gradient::StopNode(0.24f, UiColor(1.0f, 1.0f, 1.0f, 0.58f)),
    Gradient::StopNode(0.50f, UiColor(1.0f, 1.0f, 1.0f, 1.00f)),
    Gradient::StopNode(0.76f, UiColor(1.0f, 1.0f, 1.0f, 0.58f)),
    Gradient::StopNode(0.92f, UiColor(1.0f, 1.0f, 1.0f, 0.16f)),
    Gradient::StopNode(1.00f, UiColor(1.0f, 1.0f, 1.0f, 0.00f)),
  });
}

void SetOverlayFullGradientStops(Gradient::Base& gradient)
{
  SetCommonGradientStops(gradient);
}

void SetOverlayGradientStops(Gradient::Base& gradient, OverlayFillMode fillMode)
{
  switch(fillMode)
  {
    case OverlayFillMode::FULL:
    {
      SetOverlayFullGradientStops(gradient);
      return;
    }
    case OverlayFillMode::EFFECT:
    default:
    {
      SetOverlayEffectGradientStops(gradient);
      return;
    }
  }
}

struct GradientAnimationProfile
{
  Vector2 startPosition;
  Vector2 endPosition;
  float   startOffset;
  float   targetOffset;
  float   duration;
};

GradientAnimationProfile GetGradientAnimationProfile(Gradient::SpreadMethod spreadMethod)
{
  switch(spreadMethod)
  {
    case Gradient::SpreadMethod::REFLECT:
    {
      return {
        Vector2(-0.28f, 0.0f),
        Vector2(0.28f, 0.0f),
        0.0f,
        1.05f,
        2.8f,
      };
    }
    case Gradient::SpreadMethod::REPEAT:
    {
      return {
        Vector2(-0.20f, 0.0f),
        Vector2(0.20f, 0.0f),
        0.0f,
        1.0f,
        2.4f,
      };
    }
    case Gradient::SpreadMethod::PAD:
    default:
    {
      return {
        Vector2(-0.55f, 0.0f),
        Vector2(0.55f, 0.0f),
        -0.22f,
        0.42f,
        3.2f,
      };
    }
  }
}

void ApplyLinearGradient(Label label, Gradient::SpreadMethod spreadMethod, bool compactSpan)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), compactSpan ? Vector2(-0.08f, 0.0f) : Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  SetCommonGradientStops(gradient);
  label.SetTextGradient(gradient);
}

void SetAnimationGradientStops(Gradient::Base& gradient, Gradient::SpreadMethod spreadMethod)
{
  switch(spreadMethod)
  {
    case Gradient::SpreadMethod::REFLECT:
    {
      gradient.SetStopNodes({
        Gradient::StopNode(0.0f, UiColor(0.10f, 0.25f, 0.95f, 1.0f)),
        Gradient::StopNode(0.24f, UiColor(0.50f, 0.24f, 0.90f, 1.0f)),
        Gradient::StopNode(0.50f, UiColor(0.08f, 0.72f, 0.86f, 1.0f)),
        Gradient::StopNode(0.76f, UiColor(0.50f, 0.24f, 0.90f, 1.0f)),
        Gradient::StopNode(1.0f, UiColor(0.10f, 0.25f, 0.95f, 1.0f)),
      });
      return;
    }
    case Gradient::SpreadMethod::REPEAT:
    {
      gradient.SetStopNodes({
        Gradient::StopNode(0.0f, UiColor(0.10f, 0.25f, 0.95f, 1.0f)),
        Gradient::StopNode(0.28f, UiColor(0.06f, 0.70f, 0.60f, 1.0f)),
        Gradient::StopNode(0.52f, UiColor(0.48f, 0.25f, 0.86f, 1.0f)),
        Gradient::StopNode(0.76f, UiColor(0.92f, 0.36f, 0.12f, 1.0f)),
        Gradient::StopNode(1.0f, UiColor(0.10f, 0.25f, 0.95f, 1.0f)),
      });
      return;
    }
    case Gradient::SpreadMethod::PAD:
    default:
    {
      gradient.SetStopNodes({
        Gradient::StopNode(0.0f, UiColor(0.06f, 0.18f, 0.72f, 1.0f)),
        Gradient::StopNode(0.34f, UiColor(0.04f, 0.60f, 0.76f, 1.0f)),
        Gradient::StopNode(0.62f, UiColor(0.08f, 0.68f, 0.42f, 1.0f)),
        Gradient::StopNode(1.0f, UiColor(0.90f, 0.26f, 0.14f, 1.0f)),
      });
      return;
    }
  }
}

void ApplyAnimationLinearGradient(Label label, Gradient::SpreadMethod spreadMethod, bool compactSpan = false)
{
  const GradientAnimationProfile profile = GetGradientAnimationProfile(spreadMethod);

  Gradient::Linear gradient(compactSpan ? Vector2(-0.5f, 0.0f) : profile.startPosition,
                            compactSpan ? Vector2(-0.08f, 0.0f) : profile.endPosition);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(profile.startOffset);
  SetAnimationGradientStops(gradient, spreadMethod);
  label.SetTextGradient(gradient);
}

void ApplyAnimationRadialGradient(Label label, Gradient::SpreadMethod spreadMethod, bool compactSpan = false)
{
  const GradientAnimationProfile profile = GetGradientAnimationProfile(spreadMethod);

  Gradient::Radial gradient(Vector2(0.0f, 0.0f), compactSpan ? 0.18f : 0.5f);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(profile.startOffset);
  SetAnimationGradientStops(gradient, spreadMethod);
  label.SetTextGradient(gradient);
}

void ApplyAnimationConicGradient(Label label, Gradient::SpreadMethod spreadMethod)
{
  const GradientAnimationProfile profile = GetGradientAnimationProfile(spreadMethod);

  Gradient::Conic gradient(Vector2(0.0f, 0.0f), Radian(0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(profile.startOffset);
  SetAnimationGradientStops(gradient, spreadMethod);
  label.SetTextGradient(gradient);
}

void ApplyRadialGradient(Label label, Gradient::SpreadMethod spreadMethod, bool compactSpan = false)
{
  Gradient::Radial gradient(Vector2(0.0f, 0.0f), compactSpan ? 0.18f : 0.5f);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  SetCommonGradientStops(gradient);
  label.SetTextGradient(gradient);
}

void ApplyConicGradient(Label label, Gradient::SpreadMethod spreadMethod)
{
  Gradient::Conic gradient(Vector2(0.0f, 0.0f), Radian(0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  SetCommonGradientStops(gradient);
  label.SetTextGradient(gradient);
}

void ApplyOverlayLinearGradient(Label label, Gradient::SpreadMethod spreadMethod, Text::GradientBoundsMode boundsMode, Text::GradientOverlayMode overlayMode, OverlayFillMode fillMode, float startOffset = 0.0f)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetOverlayGradientStops(gradient, fillMode);
  label.SetTextGradientOverlayBoundsMode(boundsMode);
  label.SetTextGradientOverlayMode(overlayMode);
  label.SetTextGradientOverlay(gradient);
}

void ApplyOverlayRadialGradient(Label label, Gradient::SpreadMethod spreadMethod, Text::GradientBoundsMode boundsMode, Text::GradientOverlayMode overlayMode, OverlayFillMode fillMode, float startOffset = 0.0f)
{
  Gradient::Radial gradient(Vector2(0.0f, 0.0f), 0.5f);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetOverlayGradientStops(gradient, fillMode);
  label.SetTextGradientOverlayBoundsMode(boundsMode);
  label.SetTextGradientOverlayMode(overlayMode);
  label.SetTextGradientOverlay(gradient);
}

void ApplyOverlayConicGradient(Label label, Gradient::SpreadMethod spreadMethod, Text::GradientBoundsMode boundsMode, Text::GradientOverlayMode overlayMode, OverlayFillMode fillMode, float startOffset = 0.0f)
{
  Gradient::Conic gradient(Vector2(0.0f, 0.0f), Radian(0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetOverlayGradientStops(gradient, fillMode);
  label.SetTextGradientOverlayBoundsMode(boundsMode);
  label.SetTextGradientOverlayMode(overlayMode);
  label.SetTextGradientOverlay(gradient);
}

const char* GetGradientName(GradientKind gradient)
{
  switch(gradient)
  {
    case GradientKind::LINEAR:
    {
      return "Linear";
    }
    case GradientKind::RADIAL:
    {
      return "Radial";
    }
    case GradientKind::CONIC:
    {
      return "Conic";
    }
    case GradientKind::NONE:
    default:
    {
      return "None";
    }
  }
}

const char* GetAlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START:
    {
      return "Start";
    }
    case Text::Alignment::CENTER:
    {
      return "Center";
    }
    case Text::Alignment::END:
    default:
    {
      return "End";
    }
  }
}

const char* GetSpreadMethodName(Gradient::SpreadMethod spreadMethod)
{
  switch(spreadMethod)
  {
    case Gradient::SpreadMethod::REFLECT:
    {
      return "REFLECT";
    }
    case Gradient::SpreadMethod::REPEAT:
    {
      return "REPEAT";
    }
    case Gradient::SpreadMethod::PAD:
    default:
    {
      return "PAD";
    }
  }
}

const char* GetGradientBoundsModeBadgeName(Text::GradientBoundsMode mode)
{
  switch(mode)
  {
    case Text::GradientBoundsMode::VIEW_BOUND:
    {
      return "VIEW";
    }
    case Text::GradientBoundsMode::CONTENT_BOUND:
    default:
    {
      return "CONTENT";
    }
  }
}

const char* GetGradientOverlayModeName(Text::GradientOverlayMode mode)
{
  switch(mode)
  {
    case Text::GradientOverlayMode::SCREEN:
    {
      return "SCREEN";
    }
    case Text::GradientOverlayMode::SRC_OVER:
    default:
    {
      return "SRC_OVER";
    }
  }
}

const char* GetOverlayFillModeName(OverlayFillMode mode)
{
  switch(mode)
  {
    case OverlayFillMode::FULL:
    {
      return "FULL";
    }
    case OverlayFillMode::EFFECT:
    default:
    {
      return "EFFECT";
    }
  }
}

const char* GetPreviewSizeModeBadgeName(PreviewSizeMode mode)
{
  switch(mode)
  {
    case PreviewSizeMode::WRAP:
    {
      return "Wrap";
    }
    case PreviewSizeMode::FIXED:
    default:
    {
      return "Fixed";
    }
  }
}

float GetAnimationTargetOffset(Gradient::SpreadMethod spreadMethod)
{
  return GetGradientAnimationProfile(spreadMethod).targetOffset;
}

float GetAnimationDuration(Gradient::SpreadMethod spreadMethod)
{
  return GetGradientAnimationProfile(spreadMethod).duration;
}

float GetOverlayAnimationTargetOffset(OverlayFillMode fillMode, Gradient::SpreadMethod spreadMethod)
{
  return fillMode == OverlayFillMode::EFFECT ? OVERLAY_EFFECT_SHIMMER_END_OFFSET : GetAnimationTargetOffset(spreadMethod);
}

float GetOverlayAnimationDuration(OverlayFillMode fillMode, Gradient::SpreadMethod spreadMethod)
{
  return fillMode == OverlayFillMode::EFFECT ? OVERLAY_EFFECT_SHIMMER_DURATION : GetAnimationDuration(spreadMethod);
}
} // namespace.

class TextGradientController : public ConnectionTracker
{
public:
  explicit TextGradientController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextGradientController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF8FAFC));

    mWindowWidth = static_cast<float>(WINDOW_WIDTH);

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetSpacing(0.0f);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(UiColor(0xF8FAFC));

    mHeaderRoot = AbsoluteLayout::New();
    mHeaderRoot.SetRequestedWidth(MATCH_PARENT);
    mHeaderRoot.SetRequestedHeight(HEADER_HEIGHT);
    mHeaderRoot.SetBackgroundColor(UiColor(0x111827));
    mHeaderRoot.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mContentRoot = StackLayout::New(StackOrientation::VERTICAL);
    mContentRoot.SetSpacing(0.0f);
    mContentRoot.SetRequestedWidth(MATCH_PARENT);
    mContentRoot.SetRequestedHeight(MATCH_PARENT);
    mContentRoot.SetBackgroundColor(UiColor(0xF8FAFC));
    mContentRoot.SetPadding(Extents(CONTENT_PADDING, CONTENT_PADDING, CONTENT_PADDING, CONTENT_PADDING));
    mContentRoot.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mFooterRoot = AbsoluteLayout::New();
    mFooterRoot.SetRequestedWidth(MATCH_PARENT);
    mFooterRoot.SetRequestedHeight(FOOTER_HEIGHT);
    mFooterRoot.SetBackgroundColor(UiColor(0x111827));
    mFooterRoot.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mCaseBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mCaseBadge, HEADER_BADGE_HEIGHT, UiColor(0x1D4ED8), UiColor(0x93C5FD), UiColor(0xF8FAFC), Text::Alignment::START);

    mGradientTypeBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mGradientTypeBadge, HEADER_BADGE_HEIGHT, UiColor(0x065F46), UiColor(0x34D399), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mSpreadBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mSpreadBadge, HEADER_BADGE_HEIGHT, UiColor(0x312E81), UiColor(0x818CF8), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mBoundsBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mBoundsBadge, HEADER_BADGE_HEIGHT, UiColor(0x164E63), UiColor(0x67E8F9), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mSizeBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mSizeBadge, HEADER_BADGE_HEIGHT, UiColor(0x78350F), UiColor(0xFBBF24), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mHAlignBadge = CreateLabel("", 13.0f, UiColor(0xCBD5E1));
    ConfigureHudBadge(mHAlignBadge, HEADER_BADGE_HEIGHT, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1), Text::Alignment::CENTER);

    mVAlignBadge = CreateLabel("", 13.0f, UiColor(0xCBD5E1));
    ConfigureHudBadge(mVAlignBadge, HEADER_BADGE_HEIGHT, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1), Text::Alignment::CENTER);

    mMarqueeBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mMarqueeBadge, HEADER_BADGE_HEIGHT, UiColor(0x065F46), UiColor(0x34D399), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mMatrixBadge = CreateLabel("MATRIX", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mMatrixBadge, HEADER_BADGE_HEIGHT, UiColor(0x581C87), UiColor(0xC084FC), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mAnimationBadge = CreateLabel("", 13.0f, UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mAnimationBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::CENTER);

    mClearBadge = CreateLabel("Clear", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mClearBadge, HEADER_BADGE_HEIGHT, UiColor(0x7F1D1D), UiColor(0xFCA5A5), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mResetBadge = CreateLabel("Reset", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mResetBadge, HEADER_BADGE_HEIGHT, UiColor(0x7C2D12), UiColor(0xFDBA74), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlayTypeBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mOverlayTypeBadge, HEADER_BADGE_HEIGHT, UiColor(0x155E75), UiColor(0x67E8F9), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlayFillBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mOverlayFillBadge, HEADER_BADGE_HEIGHT, UiColor(0x065F46), UiColor(0x34D399), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlaySpreadBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mOverlaySpreadBadge, HEADER_BADGE_HEIGHT, UiColor(0x4C1D95), UiColor(0xC4B5FD), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlayBoundsBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mOverlayBoundsBadge, HEADER_BADGE_HEIGHT, UiColor(0x713F12), UiColor(0xFCD34D), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlayAnimationBadge = CreateLabel("Ready", 13.0f, UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mOverlayAnimationBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::CENTER);

    mOverlayModeBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mOverlayModeBadge, HEADER_BADGE_HEIGHT, UiColor(0x9D174D), UiColor(0xF9A8D4), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mOverlayToggleBadge = CreateLabel("Apply", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mOverlayToggleBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_APPLY_BACKGROUND), UiColor(BADGE_APPLY_BORDER), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mOverlayResetBadge = CreateLabel("Reset", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mOverlayResetBadge, HEADER_BADGE_HEIGHT, UiColor(0x7C2D12), UiColor(0xFDBA74), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mExpectedBadge = CreateLabel("", 12.0f, UiColor(0xCBD5E1));
    ConfigureHudBadge(mExpectedBadge, HEADER_INFO_HEIGHT, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1), Text::Alignment::START);

    mNormalPreviewContainer = StackLayout::New(StackOrientation::VERTICAL);
    mNormalPreviewContainer.SetSpacing(STACK_SPACING);
    mNormalPreviewContainer.SetRequestedWidth(MATCH_PARENT);
    mNormalPreviewContainer.SetRequestedHeight(MATCH_PARENT);
    mNormalPreviewContainer.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mPreviewLabel = CreateLabel("", 56.0f, UiColor(0x111827));
    mPreviewLabel.SetAsyncRendering(false);
    mPreviewLabel.SetBackgroundColor(UiColor(0xFFFFFF));

    mAsyncPreviewLabel = CreateLabel("", 56.0f, UiColor(0x111827));
    mAsyncPreviewLabel.SetAsyncRendering(true);
    mAsyncPreviewLabel.SetBackgroundColor(UiColor(0xF8FAFC));

    mNormalPreviewContainer.Add(mPreviewLabel);
    mNormalPreviewContainer.Add(mAsyncPreviewLabel);

    mMarqueeMatrixContainer = StackLayout::New(StackOrientation::VERTICAL);
    mMarqueeMatrixContainer.SetSpacing(4.0f);
    mMarqueeMatrixContainer.SetRequestedWidth(MATCH_PARENT);
    mMarqueeMatrixContainer.SetRequestedHeight(0.0f);
    mMarqueeMatrixContainer.SetBackgroundColor(UiColor(0xFFFFFF));
    mMarqueeMatrixContainer.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    CreateMarqueeMatrix();

    mMenuTitleLabel = CreateLabel("CONTROLS", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mMenuTitleLabel, FOOTER_BADGE_HEIGHT, UiColor(0x1D4ED8), UiColor(0x93C5FD), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mCaseListLabel = CreateLabel(
      "CASES  0 Matrix | 1 Simple | 2 Large | 3 Multi | 4 Markup | 5 Emoji | 6 Style | 7 Spread | 8 H Marquee | 9 V Marquee",
      12.0f,
      UiColor(0xCBD5E1));
    ConfigureHudBadge(mCaseListLabel, FOOTER_BADGE_HEIGHT, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1), Text::Alignment::START);

    mHelpLabel = CreateLabel(
      "ACTIONS  D/P/B Base | Y/K/U/I/L Overlay | C Base OnOff | O Overlay OnOff | A TG Anim | N OV Anim | X Reset | M Marquee | ESC Quit",
      12.0f,
      UiColor(0xCBD5E1));
    ConfigureHudBadge(mHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1), Text::Alignment::START);

    mViewHelpLabel = CreateLabel(
      "VIEW  Left/Right Case | 0 Matrix | H/V Align | S Fixed/Wrap | Q/W/E/R/T Scale 0.8/1.0/1.2/1.5/2.0",
      12.0f,
      UiColor(0xCBD5E1));
    ConfigureHudBadge(mViewHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1), Text::Alignment::START);

    ApplyPreviewOptions();
    SetMarqueeMatrixVisible(false);

    mHeaderRoot.Add(mCaseBadge);
    mHeaderRoot.Add(mGradientTypeBadge);
    mHeaderRoot.Add(mSpreadBadge);
    mHeaderRoot.Add(mBoundsBadge);
    mHeaderRoot.Add(mSizeBadge);
    mHeaderRoot.Add(mHAlignBadge);
    mHeaderRoot.Add(mVAlignBadge);
    mHeaderRoot.Add(mMarqueeBadge);
    mHeaderRoot.Add(mMatrixBadge);
    mHeaderRoot.Add(mAnimationBadge);
    mHeaderRoot.Add(mResetBadge);
    mHeaderRoot.Add(mClearBadge);
    mHeaderRoot.Add(mOverlayTypeBadge);
    mHeaderRoot.Add(mOverlaySpreadBadge);
    mHeaderRoot.Add(mOverlayBoundsBadge);
    mHeaderRoot.Add(mOverlayAnimationBadge);
    mHeaderRoot.Add(mOverlayToggleBadge);
    mHeaderRoot.Add(mOverlayResetBadge);
    mHeaderRoot.Add(mOverlayModeBadge);
    mHeaderRoot.Add(mOverlayFillBadge);
    mHeaderRoot.Add(mExpectedBadge);

    mContentRoot.Add(mNormalPreviewContainer);
    mContentRoot.Add(mMarqueeMatrixContainer);

    mFooterRoot.Add(mMenuTitleLabel);
    mFooterRoot.Add(mCaseListLabel);
    mFooterRoot.Add(mHelpLabel);
    mFooterRoot.Add(mViewHelpLabel);

    mRoot.Add(mHeaderRoot);
    mRoot.Add(mContentRoot);
    mRoot.Add(mFooterRoot);
    ConnectHeaderActions();
    UpdateHeaderLayout();
    UpdateFooterLayout();

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &TextGradientController::OnKeyEvent);
    window.ResizedSignal().Connect(this, &TextGradientController::OnWindowResized);

    ShowCase(INITIAL_CASE_INDEX);
  }

  void ShowCase(std::size_t index)
  {
    StopGradientAnimation();
    StopGradientOverlayAnimation();
    mMarqueeMatrixMode = false;
    mCaseIndex         = index % CASES.size();
    mGradientApplied   = true;
    mMarqueeRunning    = CASES[mCaseIndex].marqueeDefaultRunning;
    StopMarqueeMatrixLabels();
    SetMarqueeMatrixVisible(false);

    ApplyCurrentCase();
  }

  void ShowMarqueeMatrix()
  {
    StopGradientAnimation();
    StopGradientOverlayAnimation();
    mMarqueeMatrixMode = true;
    mGradientApplied   = true;
    mMarqueeRunning    = true;
    SetMarqueeMatrixVisible(true);
    ApplyGradientToMarqueeMatrixLabels();
    StartMarqueeMatrixLabels();
    UpdateStatus();
  }

  void SetMarqueeMatrixVisible(bool visible)
  {
    mNormalPreviewContainer.SetProperty(Actor::Property::VISIBLE, !visible);
    mNormalPreviewContainer.SetRequestedHeight(visible ? 0.0f : MATCH_PARENT);
    mNormalPreviewContainer.SetLayoutParams(StackLayoutParams::New()
                                              .SetWeight(visible ? 0.0f : 1.0f)
                                              .SetAlignment(LayoutAlignment::FILL));
    mMarqueeMatrixContainer.SetProperty(Actor::Property::VISIBLE, visible);
    mMarqueeMatrixContainer.SetRequestedHeight(visible ? MATCH_PARENT : 0.0f);
    mMarqueeMatrixContainer.SetLayoutParams(StackLayoutParams::New()
                                              .SetWeight(visible ? 1.0f : 0.0f)
                                              .SetAlignment(LayoutAlignment::FILL));
  }

  void SetHudBadge(Label label, const std::string& text, const UiColor& backgroundColor, const UiColor& borderlineColor, const UiColor& textColor = UiColor(0xF8FAFC))
  {
    if(!label)
    {
      return;
    }

    label.SetText(text.c_str());
    label.SetTextColor(textColor);
    label.SetBackgroundColor(backgroundColor);
    label.SetBorderlineColor(borderlineColor);
  }

  void SetBadgeBounds(Label label, float x, float y, float width, float height)
  {
    if(!label)
    {
      return;
    }

    label.SetRequestedWidth(width);
    label.SetRequestedHeight(height);
    label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
  }

  void UpdateHeaderLayout()
  {
    const float headerWidth  = std::max(mWindowWidth, 0.0f);
    const float contentWidth = std::max(headerWidth - HEADER_PADDING * 2.0f, 0.0f);
    const float row1Y        = HEADER_PADDING;
    const float row2Y        = row1Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;
    const float row3Y        = row2Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;
    const float noteY        = row3Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;
    const float row1Width    = std::max(contentWidth - HEADER_ROW_GAP * 5.0f, 0.0f);
    const float row2Width    = std::max(contentWidth - HEADER_ROW_GAP * 5.0f, 0.0f);
    const float row3Width    = std::max(contentWidth - HEADER_ROW_GAP * 7.0f, 0.0f);

    const float caseWidth    = row1Width * 0.32f;
    const float sizeWidth    = row1Width * 0.12f;
    const float hAlignWidth  = row1Width * 0.13f;
    const float vAlignWidth  = row1Width * 0.13f;
    const float marqueeWidth = row1Width * 0.15f;
    const float matrixWidth  = std::max(row1Width - caseWidth - sizeWidth - hAlignWidth - vAlignWidth - marqueeWidth, 0.0f);

    const float gradientWidth = row2Width * 0.15f;
    const float spreadWidth   = row2Width * 0.15f;
    const float boundsWidth   = row2Width * 0.16f;
    const float animationWidth = row2Width * 0.16f;
    const float clearWidth     = row2Width * 0.16f;
    const float resetWidth     = std::max(row2Width - gradientWidth - spreadWidth - boundsWidth - animationWidth - clearWidth, 0.0f);

    const float overlayTypeWidth   = row3Width * 0.16f;
    const float overlaySpreadWidth = row3Width * 0.10f;
    const float overlayBoundsWidth = row3Width * 0.13f;
    const float overlayReadyWidth  = row3Width * 0.10f;
    const float overlayToggleWidth = row3Width * 0.10f;
    const float overlayResetWidth  = row3Width * 0.10f;
    const float overlayModeWidth   = row3Width * 0.15f;
    const float overlayFillWidth   = std::max(row3Width - overlayTypeWidth - overlaySpreadWidth - overlayBoundsWidth - overlayReadyWidth - overlayToggleWidth - overlayResetWidth - overlayModeWidth, 0.0f);

    float x = HEADER_PADDING;
    SetBadgeBounds(mCaseBadge, x, row1Y, caseWidth, HEADER_BADGE_HEIGHT);
    x += caseWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mSizeBadge, x, row1Y, sizeWidth, HEADER_BADGE_HEIGHT);
    x += sizeWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mHAlignBadge, x, row1Y, hAlignWidth, HEADER_BADGE_HEIGHT);
    x += hAlignWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mVAlignBadge, x, row1Y, vAlignWidth, HEADER_BADGE_HEIGHT);
    x += vAlignWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mMarqueeBadge, x, row1Y, marqueeWidth, HEADER_BADGE_HEIGHT);
    x += marqueeWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mMatrixBadge, x, row1Y, matrixWidth, HEADER_BADGE_HEIGHT);

    x = HEADER_PADDING;
    SetBadgeBounds(mGradientTypeBadge, x, row2Y, gradientWidth, HEADER_BADGE_HEIGHT);
    x += gradientWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mSpreadBadge, x, row2Y, spreadWidth, HEADER_BADGE_HEIGHT);
    x += spreadWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mBoundsBadge, x, row2Y, boundsWidth, HEADER_BADGE_HEIGHT);
    x += boundsWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mAnimationBadge, x, row2Y, animationWidth, HEADER_BADGE_HEIGHT);
    x += animationWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mClearBadge, x, row2Y, clearWidth, HEADER_BADGE_HEIGHT);
    x += clearWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mResetBadge, x, row2Y, resetWidth, HEADER_BADGE_HEIGHT);

    x = HEADER_PADDING;
    SetBadgeBounds(mOverlayTypeBadge, x, row3Y, overlayTypeWidth, HEADER_BADGE_HEIGHT);
    x += overlayTypeWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlaySpreadBadge, x, row3Y, overlaySpreadWidth, HEADER_BADGE_HEIGHT);
    x += overlaySpreadWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayBoundsBadge, x, row3Y, overlayBoundsWidth, HEADER_BADGE_HEIGHT);
    x += overlayBoundsWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayAnimationBadge, x, row3Y, overlayReadyWidth, HEADER_BADGE_HEIGHT);
    x += overlayReadyWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayToggleBadge, x, row3Y, overlayToggleWidth, HEADER_BADGE_HEIGHT);
    x += overlayToggleWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayResetBadge, x, row3Y, overlayResetWidth, HEADER_BADGE_HEIGHT);
    x += overlayResetWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayModeBadge, x, row3Y, overlayModeWidth, HEADER_BADGE_HEIGHT);
    x += overlayModeWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mOverlayFillBadge, x, row3Y, overlayFillWidth, HEADER_BADGE_HEIGHT);

    SetBadgeBounds(mExpectedBadge, HEADER_PADDING, noteY, contentWidth, HEADER_INFO_HEIGHT);
  }

  void UpdateFooterLayout()
  {
    const float footerWidth  = std::max(mWindowWidth, 0.0f);
    const float contentWidth = std::max(footerWidth - FOOTER_PADDING * 2.0f, 0.0f);
    const float caseX        = FOOTER_PADDING + FOOTER_TITLE_WIDTH + FOOTER_ROW_GAP;
    const float caseWidth    = std::max(contentWidth - FOOTER_TITLE_WIDTH - FOOTER_ROW_GAP, 0.0f);
    const float helpY        = FOOTER_PADDING + FOOTER_BADGE_HEIGHT + FOOTER_ROW_GAP;
    const float viewY        = helpY + FOOTER_LINE_HEIGHT + FOOTER_ROW_GAP;

    mMenuTitleLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, FOOTER_PADDING, FOOTER_TITLE_WIDTH, FOOTER_BADGE_HEIGHT)));
    mCaseListLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(caseX, FOOTER_PADDING, caseWidth, FOOTER_BADGE_HEIGHT)));
    mHelpLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, helpY, contentWidth, FOOTER_LINE_HEIGHT)));
    mViewHelpLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, viewY, contentWidth, FOOTER_LINE_HEIGHT)));
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    mWindowWidth = static_cast<float>(windowSize.GetWidth());
    UpdateHeaderLayout();
    UpdateFooterLayout();
  }

  void ConnectHeaderActions()
  {
    mCaseBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ShowCase(mMarqueeMatrixMode ? 0u : (mCaseIndex + 1u) % CASES.size());
    });
    mGradientTypeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleGradientType();
    });
    mSpreadBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleSpreadMethod();
    });
    mBoundsBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleGradientBoundsMode();
    });
    mSizeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      TogglePreviewSizeMode();
    });
    mHAlignBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleHorizontalAlignment();
    });
    mVAlignBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleVerticalAlignment();
    });
    mMarqueeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleMarqueeRunning();
    });
    mMatrixBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      if(mMarqueeMatrixMode)
      {
        ShowCase(0u);
      }
      else
      {
        ShowMarqueeMatrix();
      }
    });
    mAnimationBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleGradientAnimation();
    });
    mResetBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ResetCurrentGradientState();
    });
    mClearBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleBaseGradientApplied();
    });
    mOverlayTypeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleOverlayGradientType();
    });
    mOverlayFillBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleOverlayFillMode();
    });
    mOverlaySpreadBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleOverlaySpreadMethod();
    });
    mOverlayBoundsBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleOverlayBoundsMode();
    });
    mOverlayAnimationBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleGradientOverlayAnimation();
    });
    mOverlayModeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      CycleOverlayMode();
    });
    mOverlayToggleBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleOverlayApplied();
    });
    mOverlayResetBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ResetCurrentOverlayState();
    });
  }

  Label CreateMatrixCell(const char* text, float width, float height, const UiColor& backgroundColor, float fontSize = 13.0f)
  {
    Label label = CreateLabel(text, fontSize, UiColor(0x4B5563));
    label.SetRequestedWidth(width);
    label.SetRequestedHeight(height);
    label.SetBackgroundColor(backgroundColor);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    return label;
  }

  Label CreateMarqueeMatrixLabel(const char* text,
                                 bool asyncRendering,
                                 Text::MarqueeOrientation orientation,
                                 Text::Alignment horizontalAlignment,
                                 Text::Alignment verticalAlignment,
                                 bool multiline,
                                 float rowHeight)
  {
    Label label = CreateLabel(text, orientation == Text::MarqueeOrientation::HORIZONTAL ? 18.0f : 16.0f, UiColor(0x111827));
    label.SetAsyncRendering(asyncRendering);
    label.SetMultiLine(multiline);
    label.SetRequestedWidth(MATRIX_CELL_WIDTH);
    label.SetRequestedHeight(rowHeight);
    label.SetBackgroundColor(asyncRendering ? UiColor(0xF8FAFC) : UiColor(0xFFFFFF));
    label.SetHorizontalTextAlignment(horizontalAlignment);
    label.SetVerticalTextAlignment(verticalAlignment);
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
    label.SetMarqueeOrientation(orientation);
    label.SetMarqueeSpeed(58);
    label.SetMarqueeLoopCount(100);
    label.SetMarqueeLoopDelay(1.0f);
    label.SetMarqueeGap(48);
    label.SetTextGradientBoundsMode(CurrentGradientBoundsMode());
    ApplyLinearGradient(label, CurrentSpreadMethod(), false);
    mMarqueeMatrixLabels.push_back(label);
    return label;
  }

  void AddMarqueeMatrixSectionTitle(const char* title)
  {
    Label label = CreateLabel(title, 15.0f, UiColor(0x111827));
    label.SetRequestedHeight(MATRIX_TITLE_HEIGHT);
    label.SetBackgroundColor(UiColor(0xDBEAFE));
    mMarqueeMatrixContainer.Add(label);
  }

  void AddMarqueeMatrixHeaderRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(MATRIX_HEADER_HEIGHT);
    row.Add(CreateMatrixCell("", MATRIX_CAPTION_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f));
    row.Add(CreateMatrixCell("Short Sync", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f));
    row.Add(CreateMatrixCell("Short Async", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f));
    row.Add(CreateMatrixCell("Long Sync", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f));
    row.Add(CreateMatrixCell("Long Async", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f));
    mMarqueeMatrixContainer.Add(row);
  }

  void AddMarqueeMatrixRow(const char* caption,
                           Text::MarqueeOrientation orientation,
                           Text::Alignment alignment)
  {
    const bool isHorizontal = orientation == Text::MarqueeOrientation::HORIZONTAL;
    const float rowHeight   = isHorizontal ? MATRIX_HORIZONTAL_ROW_HEIGHT : MATRIX_VERTICAL_ROW_HEIGHT;
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(4.0f);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(rowHeight);

    const char* const shortText = isHorizontal ? "Short" : "V\nShort";
    const char* const longText  = isHorizontal
                                    ? "Horizontal marquee gradient matrix sample keeps moving with content bounds"
                                    : "Vertical\nmarquee\ngradient\nmatrix\ncontent\nbounds";
    const Text::Alignment horizontalAlignment = isHorizontal ? alignment : Text::Alignment::CENTER;
    const Text::Alignment verticalAlignment   = isHorizontal ? Text::Alignment::CENTER : alignment;

    row.Add(CreateMatrixCell(caption, MATRIX_CAPTION_WIDTH, rowHeight, UiColor(0xE5E7EB), 13.0f));
    row.Add(CreateMarqueeMatrixLabel(shortText, false, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight));
    row.Add(CreateMarqueeMatrixLabel(shortText, true, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight));
    row.Add(CreateMarqueeMatrixLabel(longText, false, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight));
    row.Add(CreateMarqueeMatrixLabel(longText, true, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight));
    mMarqueeMatrixContainer.Add(row);
  }

  void CreateMarqueeMatrix()
  {
    AddMarqueeMatrixSectionTitle("Horizontal Marquee - single line");
    AddMarqueeMatrixHeaderRow();
    AddMarqueeMatrixRow("H START", Text::MarqueeOrientation::HORIZONTAL, Text::Alignment::START);
    AddMarqueeMatrixRow("H CENTER", Text::MarqueeOrientation::HORIZONTAL, Text::Alignment::CENTER);
    AddMarqueeMatrixRow("H END", Text::MarqueeOrientation::HORIZONTAL, Text::Alignment::END);

    AddMarqueeMatrixSectionTitle("Vertical Marquee - multiline");
    AddMarqueeMatrixHeaderRow();
    AddMarqueeMatrixRow("V START", Text::MarqueeOrientation::VERTICAL, Text::Alignment::START);
    AddMarqueeMatrixRow("V CENTER", Text::MarqueeOrientation::VERTICAL, Text::Alignment::CENTER);
    AddMarqueeMatrixRow("V END", Text::MarqueeOrientation::VERTICAL, Text::Alignment::END);
  }

  void ApplyCurrentCase()
  {
    ApplyCurrentCaseContent();
    UpdateStatus();
  }

  void ApplyCurrentCaseContent()
  {
    const CaseDefinition& item = CASES[mCaseIndex];

    ApplyCaseToLabel(mPreviewLabel, item);
    ApplyCaseToLabel(mAsyncPreviewLabel, item);
  }

  void ApplyCaseToLabel(Label label, const CaseDefinition& item)
  {
    label.StopMarquee();
    label.SetTextGradient(Gradient::Base::None());
    label.SetTextGradientOverlay(Gradient::Base::None());
    label.SetTextUnderline(Text::Underline::None());
    label.SetTextShadow(Text::Shadow::None());
    label.SetText("");
    label.SetTextColor(UiColor(0x111827));
    label.SetFontSize(item.fontSize);
    label.SetMultiLine(item.multiline);
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
    label.SetMarqueeOrientation(item.marqueeOrientation);
    label.SetMarqueeSpeed(84);
    label.SetMarqueeLoopCount(100);
    label.SetMarqueeLoopDelay(1.0f);
    label.SetMarqueeGap(64);
    label.SetTextGradientBoundsMode(CurrentGradientBoundsMode());
    label.SetTextGradientOverlayBoundsMode(CurrentOverlayBoundsMode());
    label.SetTextGradientOverlayMode(CurrentOverlayMode());
    ApplyPreviewOptions(label);
    if(item.markup)
    {
      label.SetStyledText(Text::StyledText::FromMarkup(item.text));
    }
    else
    {
      label.SetText(item.text);
    }

    if(item.style)
    {
      Text::Shadow shadow;
      shadow.SetColor(UiColor(0xF472B6));
      shadow.SetOffset(Vector2(3.0f, 4.0f));
      shadow.SetBlurRadius(6.0f);
      label.SetTextShadow(shadow);

      Text::Underline underline;
      underline.SetColor(UiColor(0x2563EB));
      underline.SetThickness(3.0f);
      underline.SetType(Text::Underline::Type::DASHED);
      underline.SetDashLength(8.0f);
      underline.SetDashGap(5.0f);
      label.SetTextUnderline(underline);
    }

    if(mGradientApplied)
    {
      ApplyGradientToLabel(label, item);
    }

    ApplyOverlayToLabel(label, item);

    if(item.marquee && mMarqueeRunning)
    {
      label.StartMarquee();
    }
  }

  void ApplyGradientToLabel(Label label, const CaseDefinition& item)
  {
    ApplyGradientToLabelByKind(label, GetEffectiveCaseGradientKind(item), item.compactGradientSpan);
  }

  bool IsOverlaySupportedForCurrentCase() const
  {
    if(mMarqueeMatrixMode)
    {
      return false;
    }

    return IsOverlaySupportedForCase(CASES[mCaseIndex]);
  }

  bool IsOverlaySupportedForCase(const CaseDefinition&) const
  {
    return true;
  }

  void ApplyOverlayToLabel(Label label, const CaseDefinition& item)
  {
    label.SetTextGradientOverlay(Gradient::Base::None());
    label.SetTextGradientOverlayBoundsMode(CurrentOverlayBoundsMode());
    label.SetTextGradientOverlayMode(CurrentOverlayMode());

    if(mOverlayApplied && IsOverlaySupportedForCase(item))
    {
      ApplyOverlayGradientToLabelByKind(label, CurrentOverlayGradientKind());
    }
  }

  void SetGradientNone()
  {
    StopGradientAnimation();
    mPreviewLabel.SetTextGradient(Gradient::Base::None());
    mAsyncPreviewLabel.SetTextGradient(Gradient::Base::None());
    SetMarqueeMatrixGradientNone();
    mGradientApplied = false;
    UpdateStatus();
  }

  void ToggleBaseGradientApplied()
  {
    if(mGradientApplied)
    {
      SetGradientNone();
    }
    else
    {
      ReapplyGradient();
    }
  }

  void ReapplyGradient()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mGradientApplied = true;
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void SetOverlayNone()
  {
    StopGradientOverlayAnimation();
    mPreviewLabel.SetTextGradientOverlay(Gradient::Base::None());
    mAsyncPreviewLabel.SetTextGradientOverlay(Gradient::Base::None());
    mOverlayApplied = false;
    UpdateStatus();
  }

  void ReapplyOverlay()
  {
    mOverlayApplied = true;
    RefreshCurrentOverlay();
  }

  void ToggleOverlayApplied()
  {
    if(mOverlayApplied)
    {
      SetOverlayNone();
    }
    else
    {
      ReapplyOverlay();
    }
  }

  void RefreshCurrentOverlay()
  {
    if(mMarqueeMatrixMode)
    {
      UpdateStatus();
      return;
    }

    const CaseDefinition& item = CASES[mCaseIndex];
    ApplyOverlayToLabel(mPreviewLabel, item);
    ApplyOverlayToLabel(mAsyncPreviewLabel, item);
    UpdateStatus();
  }

  void RefreshCurrentOverlayAfterOptionChange(bool wasAnimationRunning)
  {
    RefreshCurrentOverlay();
    if(wasAnimationRunning && CanAnimateCurrentTextGradientOverlay())
    {
      StartGradientOverlayAnimation();
      UpdateStatus();
    }
  }

  void ApplyGradientToMarqueeMatrixLabels()
  {
    for(Label& label : mMarqueeMatrixLabels)
    {
      label.SetTextGradientBoundsMode(CurrentGradientBoundsMode());
      ApplyGradientToLabelByKind(label, CurrentMatrixGradientKind(), false);
    }
  }

  void ApplyAnimationBaseGradientToMarqueeMatrixLabels()
  {
    for(Label& label : mMarqueeMatrixLabels)
    {
      label.SetTextGradientBoundsMode(CurrentGradientBoundsMode());
      ApplyAnimationBaseGradientToLabel(label, CurrentMatrixGradientKind());
    }
  }

  void SetMarqueeMatrixGradientNone()
  {
    for(Label& label : mMarqueeMatrixLabels)
    {
      label.SetTextGradient(Gradient::Base::None());
    }
  }

  void StartMarqueeMatrixLabels()
  {
    for(Label& label : mMarqueeMatrixLabels)
    {
      label.StartMarquee();
    }
  }

  void StopMarqueeMatrixLabels()
  {
    for(Label& label : mMarqueeMatrixLabels)
    {
      label.StopMarquee();
    }
  }

  void ToggleMarqueeRunning()
  {
    mMarqueeRunning = !mMarqueeRunning;
    if(mMarqueeMatrixMode)
    {
      if(mMarqueeRunning)
      {
        StartMarqueeMatrixLabels();
      }
      else
      {
        StopMarqueeMatrixLabels();
      }
      UpdateStatus();
      return;
    }

    if(CASES[mCaseIndex].marquee)
    {
      if(mMarqueeRunning)
      {
        mPreviewLabel.StartMarquee();
        mAsyncPreviewLabel.StartMarquee();
      }
      else
      {
        mPreviewLabel.StopMarquee();
        mAsyncPreviewLabel.StopMarquee();
      }
      UpdateStatus();
    }
  }

  void ToggleGradientAnimation()
  {
    if(IsGradientAnimationRunning())
    {
      StopGradientAnimation();
    }
    else
    {
      StartGradientAnimation();
    }
    UpdateStatus();
  }

  bool IsGradientAnimationRunning() const
  {
    return mAnimationInfo;
  }

  bool StopAnimationForOptionChange()
  {
    const bool wasAnimationRunning = IsGradientAnimationRunning();
    if(wasAnimationRunning)
    {
      StopGradientAnimation();
    }
    return wasAnimationRunning;
  }

  void ToggleGradientOverlayAnimation()
  {
    if(IsGradientOverlayAnimationRunning())
    {
      StopGradientOverlayAnimation();
    }
    else
    {
      StartGradientOverlayAnimation();
    }
    UpdateStatus();
  }

  bool IsGradientOverlayAnimationRunning() const
  {
    return mOverlayAnimationInfo;
  }

  bool StopOverlayAnimationForOptionChange()
  {
    const bool wasAnimationRunning = IsGradientOverlayAnimationRunning();
    if(wasAnimationRunning)
    {
      StopGradientOverlayAnimation();
    }
    return wasAnimationRunning;
  }

  void ResetCurrentGradientState()
  {
    const bool wasMarqueeRunning = mMarqueeRunning;

    StopGradientAnimation();
    StopGradientOverlayAnimation();

    if(mMarqueeMatrixMode)
    {
      if(wasMarqueeRunning)
      {
        StopMarqueeMatrixLabels();
      }

      if(mGradientApplied)
      {
        ApplyGradientToMarqueeMatrixLabels();
      }
      else
      {
        SetMarqueeMatrixGradientNone();
      }

      if(wasMarqueeRunning)
      {
        StartMarqueeMatrixLabels();
      }

      UpdateStatus();
      return;
    }

    ApplyCurrentCase();
  }

  void ResetCurrentOverlayState()
  {
    StopGradientOverlayAnimation();

    if(mMarqueeMatrixMode || !IsOverlaySupportedForCurrentCase())
    {
      UpdateStatus();
      return;
    }

    RefreshCurrentOverlay();
  }

  bool CanAnimateCurrentTextGradient() const
  {
    if(!mGradientApplied)
    {
      return false;
    }

    if(mMarqueeMatrixMode)
    {
      return IsGradientTypeSwitchable(CurrentMatrixGradientKind());
    }

    return IsGradientTypeSwitchable(GetEffectiveCaseGradientKind(CASES[mCaseIndex]));
  }

  bool CanAnimateCurrentTextGradientOverlay() const
  {
    return mOverlayApplied && IsOverlaySupportedForCurrentCase() && IsGradientTypeSwitchable(CurrentOverlayGradientKind());
  }

  void StartGradientAnimation()
  {
    if(!CanAnimateCurrentTextGradient())
    {
      StopGradientAnimation();
      return;
    }

    StopGradientAnimation();

    const float duration = GetAnimationDuration(CurrentSpreadMethod());
    mTextGradientAnimation = Animation::New(duration);
    mTextGradientAnimation.SetLooping(true);
    mTextGradientAnimation.SetLoopingMode(Animation::AUTO_REVERSE);

    if(mMarqueeMatrixMode)
    {
      if(mMarqueeRunning)
      {
        StopMarqueeMatrixLabels();
      }
      ApplyAnimationBaseGradientToMarqueeMatrixLabels();
      for(Label& label : mMarqueeMatrixLabels)
      {
        ApplyGradientAnimation(label);
      }
      if(mMarqueeRunning)
      {
        StartMarqueeMatrixLabels();
      }
    }
    else
    {
      const bool restartMarquee = CASES[mCaseIndex].marquee && mMarqueeRunning;
      if(restartMarquee)
      {
        mPreviewLabel.StopMarquee();
        mAsyncPreviewLabel.StopMarquee();
      }
      const GradientKind animationGradient = GetEffectiveCaseGradientKind(CASES[mCaseIndex]);
      const bool compactGradientSpan = CASES[mCaseIndex].compactGradientSpan;
      ApplyAnimationBaseGradientToLabel(mPreviewLabel, animationGradient, compactGradientSpan);
      ApplyAnimationBaseGradientToLabel(mAsyncPreviewLabel, animationGradient, compactGradientSpan);
      ApplyGradientAnimation(mPreviewLabel);
      ApplyGradientAnimation(mAsyncPreviewLabel);
      if(restartMarquee)
      {
        mPreviewLabel.StartMarquee();
        mAsyncPreviewLabel.StartMarquee();
      }
    }

    mTextGradientAnimation.Play();
    mAnimationInfo = true;
  }

  void StopGradientAnimation()
  {
    if(mTextGradientAnimation)
    {
      mTextGradientAnimation.Stop();
      mTextGradientAnimation.Clear();
    }
    mAnimationInfo = false;
  }

  void ApplyGradientAnimation(Label label)
  {
    if(!label)
    {
      return;
    }

    const float targetOffset = GetAnimationTargetOffset(CurrentSpreadMethod());
    const float duration     = GetAnimationDuration(CurrentSpreadMethod());
    label.Animate(mTextGradientAnimation)
      .TextGradientStartOffset(targetOffset, Duration(duration));
  }

  void StartGradientOverlayAnimation()
  {
    if(!CanAnimateCurrentTextGradientOverlay())
    {
      StopGradientOverlayAnimation();
      return;
    }

    StopGradientOverlayAnimation();

    const float duration = GetOverlayAnimationDuration(CurrentOverlayFillMode(), CurrentOverlaySpreadMethod());
    mTextGradientOverlayAnimation = Animation::New(duration);
    mTextGradientOverlayAnimation.SetLooping(true);
    mTextGradientOverlayAnimation.SetLoopingMode(Animation::AUTO_REVERSE);

    if(CurrentOverlayFillMode() == OverlayFillMode::EFFECT)
    {
      ApplyOverlayGradientToLabelByKind(mPreviewLabel, CurrentOverlayGradientKind(), true);
      ApplyOverlayGradientToLabelByKind(mAsyncPreviewLabel, CurrentOverlayGradientKind(), true);
    }

    ApplyOverlayGradientAnimation(mPreviewLabel);
    ApplyOverlayGradientAnimation(mAsyncPreviewLabel);

    mTextGradientOverlayAnimation.Play();
    mOverlayAnimationInfo = true;
  }

  void StopGradientOverlayAnimation()
  {
    if(mTextGradientOverlayAnimation)
    {
      mTextGradientOverlayAnimation.Stop();
      mTextGradientOverlayAnimation.Clear();
    }
    mOverlayAnimationInfo = false;
  }

  void ApplyOverlayGradientAnimation(Label label)
  {
    if(!label)
    {
      return;
    }

    const OverlayFillMode fillMode     = CurrentOverlayFillMode();
    const float           targetOffset = GetOverlayAnimationTargetOffset(fillMode, CurrentOverlaySpreadMethod());
    const float           duration     = GetOverlayAnimationDuration(fillMode, CurrentOverlaySpreadMethod());
    if(fillMode == OverlayFillMode::EFFECT)
    {
      label.Animate(mTextGradientOverlayAnimation)
        .TextGradientOverlayStartOffset(targetOffset, Duration(duration), AlphaFunction::LINEAR);
    }
    else
    {
      label.Animate(mTextGradientOverlayAnimation)
        .TextGradientOverlayStartOffset(targetOffset, Duration(duration));
    }
  }

  static bool IsGradientTypeSwitchable(GradientKind gradient)
  {
    return gradient == GradientKind::LINEAR || gradient == GradientKind::RADIAL || gradient == GradientKind::CONIC;
  }

  GradientKind GetEffectiveCaseGradientKind(const CaseDefinition& item) const
  {
    return IsGradientTypeSwitchable(item.gradient) && IsGradientTypeSwitchable(mGradientTypeOverride)
             ? mGradientTypeOverride
             : item.gradient;
  }

  GradientKind CurrentMatrixGradientKind() const
  {
    return IsGradientTypeSwitchable(mGradientTypeOverride) ? mGradientTypeOverride : GradientKind::LINEAR;
  }

  GradientKind CurrentDisplayedGradientKind() const
  {
    return mMarqueeMatrixMode ? CurrentMatrixGradientKind() : GetEffectiveCaseGradientKind(CASES[mCaseIndex]);
  }

  bool CanCycleGradientType() const
  {
    return mMarqueeMatrixMode || IsGradientTypeSwitchable(CASES[mCaseIndex].gradient);
  }

  void ApplyGradientToLabelByKind(Label label, GradientKind gradient, bool compactGradientSpan)
  {
    switch(gradient)
    {
      case GradientKind::LINEAR:
      {
        ApplyLinearGradient(label, CurrentSpreadMethod(), compactGradientSpan);
        break;
      }
      case GradientKind::RADIAL:
      {
        ApplyRadialGradient(label, CurrentSpreadMethod(), compactGradientSpan);
        break;
      }
      case GradientKind::CONIC:
      {
        ApplyConicGradient(label, CurrentSpreadMethod());
        break;
      }
      case GradientKind::NONE:
      default:
      {
        label.SetTextGradient(Gradient::Base::None());
        break;
      }
    }
  }

  void ApplyOverlayGradientToLabelByKind(Label label, GradientKind gradient, bool useEffectAnimationStartOffset = false)
  {
    const Gradient::SpreadMethod      spreadMethod = CurrentOverlaySpreadMethod();
    const Text::GradientBoundsMode    boundsMode   = CurrentOverlayBoundsMode();
    const Text::GradientOverlayMode   overlayMode  = CurrentOverlayMode();
    const OverlayFillMode             fillMode     = CurrentOverlayFillMode();
    const float                       startOffset  = (useEffectAnimationStartOffset && fillMode == OverlayFillMode::EFFECT) ? OVERLAY_EFFECT_SHIMMER_START_OFFSET : 0.0f;

    switch(gradient)
    {
      case GradientKind::LINEAR:
      {
        ApplyOverlayLinearGradient(label, spreadMethod, boundsMode, overlayMode, fillMode, startOffset);
        break;
      }
      case GradientKind::RADIAL:
      {
        ApplyOverlayRadialGradient(label, spreadMethod, boundsMode, overlayMode, fillMode, startOffset);
        break;
      }
      case GradientKind::CONIC:
      {
        ApplyOverlayConicGradient(label, spreadMethod, boundsMode, overlayMode, fillMode, startOffset);
        break;
      }
      case GradientKind::NONE:
      default:
      {
        label.SetTextGradientOverlay(Gradient::Base::None());
        break;
      }
    }
  }

  void ApplyAnimationBaseGradientToLabel(Label label, GradientKind gradient, bool compactGradientSpan = false)
  {
    if(gradient == GradientKind::CONIC)
    {
      ApplyAnimationConicGradient(label, CurrentSpreadMethod());
    }
    else if(gradient == GradientKind::RADIAL)
    {
      ApplyAnimationRadialGradient(label, CurrentSpreadMethod(), compactGradientSpan);
    }
    else
    {
      ApplyAnimationLinearGradient(label, CurrentSpreadMethod(), compactGradientSpan);
    }
  }

  void CycleGradientType()
  {
    if(!CanCycleGradientType())
    {
      return;
    }

    const bool wasAnimationRunning = StopAnimationForOptionChange();
    const GradientKind currentGradient = CurrentDisplayedGradientKind();
    if(currentGradient == GradientKind::LINEAR)
    {
      mGradientTypeOverride = GradientKind::RADIAL;
    }
    else if(currentGradient == GradientKind::RADIAL)
    {
      mGradientTypeOverride = GradientKind::CONIC;
    }
    else
    {
      mGradientTypeOverride = GradientKind::LINEAR;
    }
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void RefreshCurrentGradientAfterOptionChange(bool wasAnimationRunning)
  {
    const bool wasOverlayAnimationRunning = StopOverlayAnimationForOptionChange();

    if(mMarqueeMatrixMode)
    {
      if(mGradientApplied)
      {
        ApplyGradientToMarqueeMatrixLabels();
      }
      else
      {
        SetMarqueeMatrixGradientNone();
      }

      if(mMarqueeRunning)
      {
        StopMarqueeMatrixLabels();
        StartMarqueeMatrixLabels();
      }
    }
    else
    {
      ApplyCurrentCaseContent();
    }

    if(wasAnimationRunning && CanAnimateCurrentTextGradient())
    {
      StartGradientAnimation();
    }

    if(wasOverlayAnimationRunning && CanAnimateCurrentTextGradientOverlay())
    {
      StartGradientOverlayAnimation();
    }

    UpdateStatus();
  }

  Gradient::SpreadMethod CurrentSpreadMethod() const
  {
    return SPREAD_METHODS[mSpreadMethodIndex];
  }

  Gradient::SpreadMethod CurrentOverlaySpreadMethod() const
  {
    return SPREAD_METHODS[mOverlaySpreadMethodIndex];
  }

  Text::GradientBoundsMode CurrentGradientBoundsMode() const
  {
    return GRADIENT_BOUNDS_MODES[mGradientBoundsModeIndex];
  }

  Text::GradientBoundsMode CurrentOverlayBoundsMode() const
  {
    return GRADIENT_BOUNDS_MODES[mOverlayBoundsModeIndex];
  }

  Text::GradientOverlayMode CurrentOverlayMode() const
  {
    return OVERLAY_MODES[mOverlayModeIndex];
  }

  OverlayFillMode CurrentOverlayFillMode() const
  {
    return mOverlayFillMode;
  }

  GradientKind CurrentOverlayGradientKind() const
  {
    return OVERLAY_GRADIENT_KINDS[mOverlayGradientTypeIndex];
  }

  void CycleSpreadMethod()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mSpreadMethodIndex = (mSpreadMethodIndex + 1u) % SPREAD_METHODS.size();
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void CycleGradientBoundsMode()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mGradientBoundsModeIndex = (mGradientBoundsModeIndex + 1u) % GRADIENT_BOUNDS_MODES.size();
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void CycleOverlayGradientType()
  {
    const bool wasAnimationRunning = StopOverlayAnimationForOptionChange();
    mOverlayGradientTypeIndex = (mOverlayGradientTypeIndex + 1u) % OVERLAY_GRADIENT_KINDS.size();
    RefreshCurrentOverlayAfterOptionChange(wasAnimationRunning);
  }

  void ToggleOverlayFillMode()
  {
    const bool wasAnimationRunning = StopOverlayAnimationForOptionChange();
    mOverlayFillMode = mOverlayFillMode == OverlayFillMode::EFFECT ? OverlayFillMode::FULL : OverlayFillMode::EFFECT;
    RefreshCurrentOverlayAfterOptionChange(wasAnimationRunning);
  }

  void CycleOverlaySpreadMethod()
  {
    const bool wasAnimationRunning = StopOverlayAnimationForOptionChange();
    mOverlaySpreadMethodIndex = (mOverlaySpreadMethodIndex + 1u) % SPREAD_METHODS.size();
    RefreshCurrentOverlayAfterOptionChange(wasAnimationRunning);
  }

  void CycleOverlayBoundsMode()
  {
    const bool wasAnimationRunning = StopOverlayAnimationForOptionChange();
    mOverlayBoundsModeIndex = (mOverlayBoundsModeIndex + 1u) % GRADIENT_BOUNDS_MODES.size();
    RefreshCurrentOverlayAfterOptionChange(wasAnimationRunning);
  }

  void CycleOverlayMode()
  {
    const bool wasAnimationRunning = StopOverlayAnimationForOptionChange();
    mOverlayModeIndex = (mOverlayModeIndex + 1u) % OVERLAY_MODES.size();
    RefreshCurrentOverlayAfterOptionChange(wasAnimationRunning);
  }

  void CycleHorizontalAlignment()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mHorizontalAlignmentIndex = (mHorizontalAlignmentIndex + 1u) % ALIGNMENTS.size();
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void CycleVerticalAlignment()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mVerticalAlignmentIndex = (mVerticalAlignmentIndex + 1u) % ALIGNMENTS.size();
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void TogglePreviewSizeMode()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mPreviewSizeMode = (mPreviewSizeMode == PreviewSizeMode::FIXED) ? PreviewSizeMode::WRAP : PreviewSizeMode::FIXED;
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void ApplyPreviewOptions()
  {
    ApplyPreviewOptions(mPreviewLabel);
    ApplyPreviewOptions(mAsyncPreviewLabel);
  }

  void ApplyPreviewOptions(Label label)
  {
    label.SetHorizontalTextAlignment(ALIGNMENTS[mHorizontalAlignmentIndex]);
    label.SetVerticalTextAlignment(ALIGNMENTS[mVerticalAlignmentIndex]);

    if(mPreviewSizeMode == PreviewSizeMode::WRAP)
    {
      label.SetRequestedWidth(WRAP_CONTENT);
      label.SetRequestedHeight(WRAP_CONTENT);
      label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
    }
    else
    {
      label.SetRequestedWidth(MATCH_PARENT);
      label.SetRequestedHeight(MATCH_PARENT);
      label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    }
  }

  void UpdateGradientTypeBadge()
  {
    if(!CanCycleGradientType())
    {
      SetHudBadge(mGradientTypeBadge, "Gradient N/A", UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));
      return;
    }

    const GradientKind gradient = CurrentDisplayedGradientKind();
    std::string        text     = "Gradient ";
    text += GetGradientName(gradient);
    UiColor backgroundColor = UiColor(0x075985);
    UiColor borderlineColor = UiColor(0x38BDF8);
    if(gradient == GradientKind::RADIAL)
    {
      backgroundColor = UiColor(0x6D28D9);
      borderlineColor = UiColor(0xC4B5FD);
    }
    else if(gradient == GradientKind::CONIC)
    {
      backgroundColor = UiColor(0x92400E);
      borderlineColor = UiColor(0xFBBF24);
    }
    SetHudBadge(mGradientTypeBadge, text, backgroundColor, borderlineColor);
  }

  void UpdateOverlayBadges(bool supported)
  {
    if(!supported)
    {
      SetHudBadge(mOverlayTypeBadge, "Overlay N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlaySpreadBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayBoundsBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayAnimationBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayToggleBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayResetBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayModeBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      SetHudBadge(mOverlayFillBadge, "N/A", UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
      return;
    }

    std::string type = "Overlay ";
    type += GetGradientName(CurrentOverlayGradientKind());
    SetHudBadge(mOverlayTypeBadge, type, UiColor(0x155E75), UiColor(0x67E8F9));

    SetHudBadge(mOverlaySpreadBadge, GetSpreadMethodName(CurrentOverlaySpreadMethod()), UiColor(0x4C1D95), UiColor(0xC4B5FD));
    SetHudBadge(mOverlayBoundsBadge, GetGradientBoundsModeBadgeName(CurrentOverlayBoundsMode()), UiColor(0x713F12), UiColor(0xFCD34D));
    const bool canAnimateOverlay = CanAnimateCurrentTextGradientOverlay();
    SetHudBadge(mOverlayAnimationBadge,
                mOverlayAnimationInfo ? "Run" : "Ready",
                mOverlayAnimationInfo ? UiColor(0x0F766E) : (canAnimateOverlay ? UiColor(BADGE_READY_BACKGROUND) : UiColor(BADGE_DISABLED_BACKGROUND)),
                mOverlayAnimationInfo ? UiColor(0x5EEAD4) : (canAnimateOverlay ? UiColor(BADGE_READY_BORDER) : UiColor(BADGE_DISABLED_BORDER)),
                mOverlayAnimationInfo ? UiColor(BADGE_ON_TEXT) : (canAnimateOverlay ? UiColor(BADGE_ON_TEXT) : UiColor(BADGE_DISABLED_TEXT)));

    SetHudBadge(mOverlayToggleBadge,
                mOverlayApplied ? "Clear" : "Apply",
                mOverlayApplied ? UiColor(0x7F1D1D) : UiColor(BADGE_APPLY_BACKGROUND),
                mOverlayApplied ? UiColor(0xFCA5A5) : UiColor(BADGE_APPLY_BORDER),
                UiColor(BADGE_ON_TEXT));
    SetHudBadge(mOverlayResetBadge, "Reset", UiColor(0x7C2D12), UiColor(0xFDBA74), UiColor(BADGE_ON_TEXT));
    SetHudBadge(mOverlayModeBadge, GetGradientOverlayModeName(CurrentOverlayMode()), UiColor(0x9D174D), UiColor(0xF9A8D4));

    std::string fill = "FILL ";
    fill += GetOverlayFillModeName(CurrentOverlayFillMode());
    SetHudBadge(mOverlayFillBadge, fill, UiColor(0x065F46), UiColor(0x34D399));
  }

  void UpdateStatus()
  {
    SetHudBadge(mResetBadge, "Reset", UiColor(0x7C2D12), UiColor(0xFDBA74));
    SetHudBadge(mClearBadge,
                mGradientApplied ? "Clear" : "Apply",
                mGradientApplied ? UiColor(0x7F1D1D) : UiColor(BADGE_APPLY_BACKGROUND),
                mGradientApplied ? UiColor(0xFCA5A5) : UiColor(BADGE_APPLY_BORDER),
                UiColor(BADGE_ON_TEXT));
    UpdateGradientTypeBadge();

    if(mMarqueeMatrixMode)
    {
      UpdateOverlayBadges(false);
      SetHudBadge(mCaseBadge, "CASE 0  Matrix", UiColor(0x1D4ED8), UiColor(0x93C5FD));

      std::string spread;
      spread += GetSpreadMethodName(CurrentSpreadMethod());
      SetHudBadge(mSpreadBadge, spread, UiColor(0x312E81), UiColor(0x818CF8));

      std::string bounds;
      bounds += GetGradientBoundsModeBadgeName(CurrentGradientBoundsMode());
      SetHudBadge(mBoundsBadge, bounds, UiColor(0x164E63), UiColor(0x67E8F9));

      std::string size = "SIZE ";
      size += GetPreviewSizeModeBadgeName(mPreviewSizeMode);
      SetHudBadge(mSizeBadge, size, UiColor(0x78350F), UiColor(0xFBBF24));

      SetHudBadge(mHAlignBadge, "H ALIGN Matrix", UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));
      SetHudBadge(mVAlignBadge, "V ALIGN Matrix", UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));
      SetHudBadge(mMarqueeBadge, mMarqueeRunning ? "MARQUEE Run" : "MARQUEE Stop", mMarqueeRunning ? UiColor(0x065F46) : UiColor(0x7F1D1D), mMarqueeRunning ? UiColor(0x34D399) : UiColor(0xFCA5A5));
      SetHudBadge(mMatrixBadge, "MATRIX On", UiColor(0x581C87), UiColor(0xC084FC));

      const bool canAnimate = CanAnimateCurrentTextGradient();
      SetHudBadge(mAnimationBadge,
                  mAnimationInfo ? "Run" : "Ready",
                  mAnimationInfo ? UiColor(0x0F766E) : (canAnimate ? UiColor(BADGE_READY_BACKGROUND) : UiColor(BADGE_DISABLED_BACKGROUND)),
                  mAnimationInfo ? UiColor(0x5EEAD4) : (canAnimate ? UiColor(BADGE_READY_BORDER) : UiColor(BADGE_DISABLED_BORDER)),
                  mAnimationInfo ? UiColor(BADGE_ON_TEXT) : (canAnimate ? UiColor(BADGE_ON_TEXT) : UiColor(BADGE_DISABLED_TEXT)));

      std::string expected = "MATRIX  H/V, START/CENTER/END, short/long, sync/async | Expected: short cells align to text bounds; long cells use viewport bounds; async matches sync.";
      if(!mGradientApplied)
      {
        expected += " TG: cleared; C applies the selected type.";
      }
      else if(mAnimationInfo)
      {
        expected += " Animation: StartOffset drives gradient motion. X resets the current gradient state.";
      }
      else if(!canAnimate)
      {
        expected += " Animation: current gradient does not support StartOffset motion.";
      }
      expected += " OV: N/A on matrix path. OV Anim: N/A.";
      SetHudBadge(mExpectedBadge, expected, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1));
      return;
    }

    const CaseDefinition& item = CASES[mCaseIndex];
    const bool            overlaySupported = IsOverlaySupportedForCase(item);
    UpdateOverlayBadges(overlaySupported);

    std::string caseText = "CASE ";
    caseText += std::to_string(mCaseIndex + 1u);
    caseText += "/";
    caseText += std::to_string(CASES.size());
    caseText += "  ";
    caseText += item.title;
    SetHudBadge(mCaseBadge, caseText, UiColor(0x1D4ED8), UiColor(0x93C5FD));

    std::string spread;
    spread += GetSpreadMethodName(CurrentSpreadMethod());
    SetHudBadge(mSpreadBadge, spread, UiColor(0x312E81), UiColor(0x818CF8));

    std::string bounds;
    bounds += GetGradientBoundsModeBadgeName(CurrentGradientBoundsMode());
    SetHudBadge(mBoundsBadge, bounds, UiColor(0x164E63), UiColor(0x67E8F9));

    std::string size = "SIZE ";
    size += GetPreviewSizeModeBadgeName(mPreviewSizeMode);
    SetHudBadge(mSizeBadge, size, UiColor(0x78350F), UiColor(0xFBBF24));

    std::string hAlign = "H ALIGN ";
    hAlign += GetAlignmentName(ALIGNMENTS[mHorizontalAlignmentIndex]);
    SetHudBadge(mHAlignBadge, hAlign, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));

    std::string vAlign = "V ALIGN ";
    vAlign += GetAlignmentName(ALIGNMENTS[mVerticalAlignmentIndex]);
    SetHudBadge(mVAlignBadge, vAlign, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));

    if(item.marquee)
    {
      std::string marquee = "MARQUEE ";
      marquee += item.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL ? "H" : "V";
      marquee += mMarqueeRunning ? " Run" : " Stop";
      SetHudBadge(mMarqueeBadge, marquee, mMarqueeRunning ? UiColor(0x065F46) : UiColor(0x7F1D1D), mMarqueeRunning ? UiColor(0x34D399) : UiColor(0xFCA5A5));
    }
    else
    {
      SetHudBadge(mMarqueeBadge, "MARQUEE N/A", UiColor(0x334155), UiColor(0x64748B), UiColor(0xCBD5E1));
    }

    SetHudBadge(mMatrixBadge, "MATRIX Off", UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));
    const bool canAnimate = CanAnimateCurrentTextGradient();
    SetHudBadge(mAnimationBadge,
                mAnimationInfo ? "Run" : "Ready",
                mAnimationInfo ? UiColor(0x0F766E) : (canAnimate ? UiColor(BADGE_READY_BACKGROUND) : UiColor(BADGE_DISABLED_BACKGROUND)),
                mAnimationInfo ? UiColor(0x5EEAD4) : (canAnimate ? UiColor(BADGE_READY_BORDER) : UiColor(BADGE_DISABLED_BORDER)),
                mAnimationInfo ? UiColor(BADGE_ON_TEXT) : (canAnimate ? UiColor(BADGE_ON_TEXT) : UiColor(BADGE_DISABLED_TEXT)));

    std::string expected = "Coordinate: ";
    expected += item.marquee ? "visible marquee viewport bounds" : "logical text bounds";
    expected += " | ";
    expected += item.expected;
    expected += " ";
    expected += item.notes;
    if(!mGradientApplied)
    {
      expected += " TG: cleared; C applies the selected type.";
    }
    else if(mAnimationInfo)
    {
      expected += " Animation: StartOffset drives gradient motion. X resets the current gradient state.";
    }
    else if(!canAnimate)
    {
      expected += " Animation: current gradient does not support StartOffset motion.";
    }
    else
    {
      expected += " Animation: A starts spread-tuned gradient motion; X resets the current gradient state.";
    }
    if(overlaySupported)
    {
      if(mOverlayApplied)
      {
        expected += " OV: ";
        if(item.marquee)
        {
          expected += "sync/async ";
        }
        expected += GetGradientName(CurrentOverlayGradientKind());
        expected += " ";
        expected += GetOverlayFillModeName(CurrentOverlayFillMode());
        expected += " ";
        expected += GetGradientOverlayModeName(CurrentOverlayMode());
        expected += " ";
        expected += GetGradientBoundsModeBadgeName(CurrentOverlayBoundsMode());
        expected += ".";
        expected += mOverlayAnimationInfo ? " OV Anim: Run." : " OV Anim: N or Ready starts overlay motion.";
      }
      else
      {
        expected += " OV: cleared; O applies overlay.";
      }
    }
    else
    {
      expected += " OV: N/A on unsupported path.";
    }
    SetHudBadge(mExpectedBadge, expected, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1));
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

    const Dali::String& keyName = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
    {
      ShowCase(mMarqueeMatrixMode ? CASES.size() - 1u : (mCaseIndex + CASES.size() - 1u) % CASES.size());
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
    {
      ShowCase(mMarqueeMatrixMode ? 0u : (mCaseIndex + 1u) % CASES.size());
    }
    else if(keyName == "0")
    {
      ShowMarqueeMatrix();
    }
    else if(keyName == "1")
    {
      ShowCase(0u);
    }
    else if(keyName == "2")
    {
      ShowCase(1u);
    }
    else if(keyName == "3")
    {
      ShowCase(2u);
    }
    else if(keyName == "4")
    {
      ShowCase(3u);
    }
    else if(keyName == "5")
    {
      ShowCase(4u);
    }
    else if(keyName == "6")
    {
      ShowCase(5u);
    }
    else if(keyName == "7")
    {
      ShowCase(6u);
    }
    else if(keyName == "8")
    {
      ShowCase(7u);
    }
    else if(keyName == "9")
    {
      ShowCase(8u);
    }
    else if(keyName == "c" || keyName == "C")
    {
      ToggleBaseGradientApplied();
    }
    else if(keyName == "g" || keyName == "G")
    {
      ReapplyGradient();
    }
    else if(keyName == "d" || keyName == "D")
    {
      CycleGradientType();
    }
    else if(keyName == "a" || keyName == "A")
    {
      ToggleGradientAnimation();
    }
    else if(keyName == "n" || keyName == "N")
    {
      ToggleGradientOverlayAnimation();
    }
    else if(keyName == "x" || keyName == "X")
    {
      ResetCurrentGradientState();
    }
    else if(keyName == "p" || keyName == "P")
    {
      CycleSpreadMethod();
    }
    else if(keyName == "b" || keyName == "B")
    {
      CycleGradientBoundsMode();
    }
    else if(keyName == "y" || keyName == "Y")
    {
      CycleOverlayGradientType();
    }
    else if(keyName == "k" || keyName == "K")
    {
      ToggleOverlayFillMode();
    }
    else if(keyName == "u" || keyName == "U")
    {
      CycleOverlaySpreadMethod();
    }
    else if(keyName == "i" || keyName == "I")
    {
      CycleOverlayBoundsMode();
    }
    else if(keyName == "l" || keyName == "L")
    {
      CycleOverlayMode();
    }
    else if(keyName == "o" || keyName == "O")
    {
      ToggleOverlayApplied();
    }
    else if(keyName == "m" || keyName == "M")
    {
      ToggleMarqueeRunning();
    }
    else if(keyName == "h" || keyName == "H")
    {
      CycleHorizontalAlignment();
    }
    else if(keyName == "v" || keyName == "V")
    {
      CycleVerticalAlignment();
    }
    else if(keyName == "s" || keyName == "S")
    {
      TogglePreviewSizeMode();
    }
    else if(keyName == "q" || keyName == "Q")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(keyName == "w" || keyName == "W")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(keyName == "e" || keyName == "E")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(keyName == "r" || keyName == "R")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(keyName == "t" || keyName == "T")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
  }

private:
  Application& mApplication;
  StackLayout    mRoot;
  AbsoluteLayout mHeaderRoot;
  StackLayout    mContentRoot;
  AbsoluteLayout mFooterRoot;
  Label          mCaseBadge;
  Label          mGradientTypeBadge;
  Label          mSpreadBadge;
  Label          mBoundsBadge;
  Label          mSizeBadge;
  Label          mHAlignBadge;
  Label          mVAlignBadge;
  Label          mMarqueeBadge;
  Label          mMatrixBadge;
  Label          mAnimationBadge;
  Label          mResetBadge;
  Label          mClearBadge;
  Label          mOverlayTypeBadge;
  Label          mOverlayFillBadge;
  Label          mOverlaySpreadBadge;
  Label          mOverlayBoundsBadge;
  Label          mOverlayAnimationBadge;
  Label          mOverlayModeBadge;
  Label          mOverlayToggleBadge;
  Label          mOverlayResetBadge;
  Label          mExpectedBadge;
  StackLayout    mNormalPreviewContainer;
  StackLayout    mMarqueeMatrixContainer;
  Label          mPreviewLabel;
  Label          mAsyncPreviewLabel;
  Label          mMenuTitleLabel;
  Label          mCaseListLabel;
  Label          mHelpLabel;
  Label          mViewHelpLabel;
  Animation      mTextGradientAnimation;
  Animation      mTextGradientOverlayAnimation;
  std::vector<Label> mMarqueeMatrixLabels;
  float              mWindowWidth{static_cast<float>(WINDOW_WIDTH)};
  std::size_t        mCaseIndex{INITIAL_CASE_INDEX};
  std::size_t        mHorizontalAlignmentIndex{INITIAL_ALIGNMENT_INDEX};
  std::size_t        mVerticalAlignmentIndex{INITIAL_ALIGNMENT_INDEX};
  std::size_t        mSpreadMethodIndex{INITIAL_SPREAD_METHOD_INDEX};
  std::size_t        mGradientBoundsModeIndex{INITIAL_GRADIENT_BOUNDS_MODE_INDEX};
  std::size_t        mOverlayGradientTypeIndex{0u};
  std::size_t        mOverlaySpreadMethodIndex{INITIAL_SPREAD_METHOD_INDEX};
  std::size_t        mOverlayBoundsModeIndex{INITIAL_GRADIENT_BOUNDS_MODE_INDEX};
  std::size_t        mOverlayModeIndex{0u};
  PreviewSizeMode mPreviewSizeMode{PreviewSizeMode::FIXED};
  OverlayFillMode mOverlayFillMode{OverlayFillMode::EFFECT};
  GradientKind    mGradientTypeOverride{GradientKind::NONE};
  bool            mGradientApplied{true};
  bool            mOverlayApplied{false};
  bool            mAnimationInfo{false};
  bool            mOverlayAnimationInfo{false};
  bool            mMarqueeMatrixMode{false};
  bool            mMarqueeRunning{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig config = UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();

  TextGradientController controller(application);
  application.MainLoop();

  return 0;
}
