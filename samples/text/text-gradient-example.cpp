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
constexpr float HEADER_HEIGHT     = HEADER_PADDING + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_INFO_HEIGHT + HEADER_PADDING;
constexpr float FOOTER_HEIGHT     = FOOTER_PADDING + FOOTER_BADGE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_PADDING;
constexpr float MATRIX_TITLE_HEIGHT = 18.0f;
constexpr float MATRIX_HEADER_HEIGHT = 20.0f;
constexpr float MATRIX_HORIZONTAL_ROW_HEIGHT = 30.0f;
constexpr float MATRIX_VERTICAL_ROW_HEIGHT   = 72.0f;
constexpr float MATRIX_CAPTION_WIDTH = 76.0f;
constexpr float MATRIX_CELL_WIDTH    = 188.0f;
constexpr int   WINDOW_WIDTH     = 920;
constexpr int   WINDOW_HEIGHT    = 880;
constexpr std::size_t CASE_COUNT = 9u;
constexpr std::size_t INITIAL_CASE_INDEX                 = 0u;
constexpr std::size_t INITIAL_ALIGNMENT_INDEX            = 1u;
constexpr std::size_t INITIAL_SPREAD_METHOD_INDEX        = 0u;
constexpr std::size_t INITIAL_GRADIENT_BOUNDS_MODE_INDEX = 0u;

enum class GradientKind
{
  NONE,
  LINEAR,
  RADIAL
};

enum class PreviewSizeMode
{
  FIXED,
  WRAP
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
  bool         compactGradientSpan{false};
};

constexpr std::array<CaseDefinition, CASE_COUNT> CASES{{
  {
    "Simple TextGradient",
    "Text Gradient",
    "Expected: selected gradient type visible on the simple Label path.",
    "Notes: single-color, monochrome, non-marquee, non-tiling, no style.",
    GradientKind::LINEAR,
    64.0f,
    false,
    false,
    false,
  },
  {
    "Large TextGradient",
    "Gradient Rendering Sample",
    "Expected: long text should make the selected gradient type easy to inspect.",
    "Notes: still a simple single-color Label path.",
    GradientKind::LINEAR,
    52.0f,
    false,
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
    "Default <color value='red'>Red Markup</color> Default",
    "Expected: default glyphs use gradient, explicit red markup stays red.",
    "Notes: mixed path uses preserved RGBA text plus a gradient mask.",
    GradientKind::LINEAR,
    42.0f,
    false,
    true,
    false,
  },
  {
    "Emoji Mixed",
    "Gradient 😀 Text 🌈 Test",
    "Expected: default glyphs use gradient, emoji/color glyphs keep original color.",
    "Notes: visible emoji depends on available color emoji fonts.",
    GradientKind::LINEAR,
    46.0f,
    false,
    false,
    false,
  },
  {
    "Shadow And Underline Style",
    "Styled Text Gradient",
    "Expected: fill uses gradient; shadow and underline keep their own style colors.",
    "Notes: verifies style/overlay composition with simple TextGradient.",
    GradientKind::LINEAR,
    50.0f,
    false,
    false,
    true,
  },
  {
    "Spread Method",
    "Text gradient spread method inspection across the selected type",
    "Expected: P cycles PAD, REFLECT and REPEAT; the compact span makes spread behavior visible.",
    "Notes: TYPE toggles Linear/Radial so spread can be checked on both gradient evaluators.",
    GradientKind::LINEAR,
    46.0f,
    false,
    false,
    false,
    false,
    Text::MarqueeOrientation::HORIZONTAL,
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

constexpr std::array<Text::GradientBoundsMode, 2u> GRADIENT_BOUNDS_MODES{{
  Text::GradientBoundsMode::CONTENT_BOUND,
  Text::GradientBoundsMode::VIEW_BOUND,
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

void ApplyRadialGradient(Label label, Gradient::SpreadMethod spreadMethod, bool compactSpan = false)
{
  Gradient::Radial gradient(Vector2(0.0f, 0.0f), compactSpan ? 0.18f : 0.5f);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  SetCommonGradientStops(gradient);
  label.SetTextGradient(gradient);
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

    mGradientBadge = CreateLabel("", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mGradientBadge, HEADER_BADGE_HEIGHT, UiColor(0x065F46), UiColor(0x34D399), UiColor(0xF8FAFC), Text::Alignment::CENTER);

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

    mAnimationBadge = CreateLabel("", 13.0f, UiColor(0xCBD5E1));
    ConfigureHudBadge(mAnimationBadge, HEADER_BADGE_HEIGHT, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1), Text::Alignment::CENTER);

    mClearBadge = CreateLabel("Clear", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mClearBadge, HEADER_BADGE_HEIGHT, UiColor(0x7F1D1D), UiColor(0xFCA5A5), UiColor(0xF8FAFC), Text::Alignment::CENTER);

    mResetBadge = CreateLabel("Reset", 13.0f, UiColor(0xF8FAFC));
    ConfigureHudBadge(mResetBadge, HEADER_BADGE_HEIGHT, UiColor(0x7C2D12), UiColor(0xFDBA74), UiColor(0xF8FAFC), Text::Alignment::CENTER);

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

    mNormalPreviewContainer.AddChildren({
      mPreviewLabel,
      mAsyncPreviewLabel,
    });

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
      "ACTIONS  Click top badges | D Type | A Anim | X Reset | C Clear | G Apply | P Spread | B Bounds | M Marquee | ESC Quit",
      12.0f,
      UiColor(0xCBD5E1));
    ConfigureHudBadge(mHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1), Text::Alignment::START);

    mViewHelpLabel = CreateLabel(
      "VIEW  Left/Right Case | H/V Align | S Fixed/Wrap | Q/W/E/R/T Scale 0.8/1.0/1.2/1.5/2.0",
      12.0f,
      UiColor(0xCBD5E1));
    ConfigureHudBadge(mViewHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1), Text::Alignment::START);

    ApplyPreviewOptions();
    SetMarqueeMatrixVisible(false);

    mHeaderRoot.AddChildren({
      mCaseBadge,
      mGradientBadge,
      mSpreadBadge,
      mBoundsBadge,
      mSizeBadge,
      mHAlignBadge,
      mVAlignBadge,
      mMarqueeBadge,
      mMatrixBadge,
      mAnimationBadge,
      mResetBadge,
      mClearBadge,
      mExpectedBadge,
    });

    mContentRoot.AddChildren({
      mNormalPreviewContainer,
      mMarqueeMatrixContainer,
    });

    mFooterRoot.AddChildren({
      mMenuTitleLabel,
      mCaseListLabel,
      mHelpLabel,
      mViewHelpLabel,
    });

    mRoot.AddChildren({
      mHeaderRoot,
      mContentRoot,
      mFooterRoot,
    });
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
    mMarqueeMatrixMode = false;
    mCaseIndex         = index % CASES.size();
    mGradientApplied   = true;
    mMarqueeRunning    = true;
    StopMarqueeMatrixLabels();
    SetMarqueeMatrixVisible(false);

    ApplyCurrentCase();
  }

  void ShowMarqueeMatrix()
  {
    StopGradientAnimation();
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
    const float noteY        = row2Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;
    const float row1Width    = std::max(contentWidth - HEADER_ROW_GAP * 4.0f, 0.0f);
    const float row2Width    = std::max(contentWidth - HEADER_ROW_GAP * 6.0f, 0.0f);

    const float caseWidth     = row1Width * 0.30f;
    const float gradientWidth = row1Width * 0.16f;
    const float spreadWidth   = row1Width * 0.15f;
    const float boundsWidth   = row1Width * 0.20f;
    const float sizeWidth     = std::max(row1Width - caseWidth - gradientWidth - spreadWidth - boundsWidth, 0.0f);

    const float hAlignWidth       = row2Width * 0.12f;
    const float vAlignWidth       = row2Width * 0.12f;
    const float marqueeWidth      = row2Width * 0.16f;
    const float matrixWidth       = row2Width * 0.11f;
    const float animationWidth    = row2Width * 0.17f;
    const float resetWidth        = row2Width * 0.14f;
    const float clearWidth        = std::max(row2Width - hAlignWidth - vAlignWidth - marqueeWidth - matrixWidth - animationWidth - resetWidth, 0.0f);

    float x = HEADER_PADDING;
    SetBadgeBounds(mCaseBadge, x, row1Y, caseWidth, HEADER_BADGE_HEIGHT);
    x += caseWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mGradientBadge, x, row1Y, gradientWidth, HEADER_BADGE_HEIGHT);
    x += gradientWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mSpreadBadge, x, row1Y, spreadWidth, HEADER_BADGE_HEIGHT);
    x += spreadWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mBoundsBadge, x, row1Y, boundsWidth, HEADER_BADGE_HEIGHT);
    x += boundsWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mSizeBadge, x, row1Y, sizeWidth, HEADER_BADGE_HEIGHT);

    x = HEADER_PADDING;
    SetBadgeBounds(mHAlignBadge, x, row2Y, hAlignWidth, HEADER_BADGE_HEIGHT);
    x += hAlignWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mVAlignBadge, x, row2Y, vAlignWidth, HEADER_BADGE_HEIGHT);
    x += vAlignWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mMarqueeBadge, x, row2Y, marqueeWidth, HEADER_BADGE_HEIGHT);
    x += marqueeWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mMatrixBadge, x, row2Y, matrixWidth, HEADER_BADGE_HEIGHT);
    x += matrixWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mAnimationBadge, x, row2Y, animationWidth, HEADER_BADGE_HEIGHT);
    x += animationWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mResetBadge, x, row2Y, resetWidth, HEADER_BADGE_HEIGHT);
    x += resetWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mClearBadge, x, row2Y, clearWidth, HEADER_BADGE_HEIGHT);

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
    mGradientBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
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
      SetGradientNone();
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
    row.AddChildren({
      CreateMatrixCell("", MATRIX_CAPTION_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f),
      CreateMatrixCell("Short Sync", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f),
      CreateMatrixCell("Short Async", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f),
      CreateMatrixCell("Long Sync", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f),
      CreateMatrixCell("Long Async", MATRIX_CELL_WIDTH, MATRIX_HEADER_HEIGHT, UiColor(0xE5E7EB), 12.0f),
    });
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

    row.AddChildren({
      CreateMatrixCell(caption, MATRIX_CAPTION_WIDTH, rowHeight, UiColor(0xE5E7EB), 13.0f),
      CreateMarqueeMatrixLabel(shortText, false, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight),
      CreateMarqueeMatrixLabel(shortText, true, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight),
      CreateMarqueeMatrixLabel(longText, false, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight),
      CreateMarqueeMatrixLabel(longText, true, orientation, horizontalAlignment, verticalAlignment, !isHorizontal, rowHeight),
    });
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
    label.SetTextUnderline(Text::Underline::None());
    label.SetTextShadow(Text::Shadow::None());
    label.SetMarkupEnabled(false);
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
    ApplyPreviewOptions(label);
    label.SetMarkupEnabled(item.markup);
    label.SetText(item.text);

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

    if(item.marquee && mMarqueeRunning)
    {
      label.StartMarquee();
    }
  }

  void ApplyGradientToLabel(Label label, const CaseDefinition& item)
  {
    ApplyGradientToLabelByKind(label, GetEffectiveCaseGradientKind(item), item.compactGradientSpan);
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

  void ReapplyGradient()
  {
    const bool wasAnimationRunning = StopAnimationForOptionChange();
    mGradientApplied = true;
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
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

  void ResetCurrentGradientState()
  {
    const bool wasMarqueeRunning = mMarqueeRunning;

    StopGradientAnimation();

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

  static bool IsGradientTypeSwitchable(GradientKind gradient)
  {
    return gradient == GradientKind::LINEAR || gradient == GradientKind::RADIAL;
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
      case GradientKind::NONE:
      default:
      {
        label.SetTextGradient(Gradient::Base::None());
        break;
      }
    }
  }

  void ApplyAnimationBaseGradientToLabel(Label label, GradientKind gradient, bool compactGradientSpan = false)
  {
    if(gradient == GradientKind::RADIAL)
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
    mGradientTypeOverride = (currentGradient == GradientKind::RADIAL) ? GradientKind::LINEAR : GradientKind::RADIAL;
    RefreshCurrentGradientAfterOptionChange(wasAnimationRunning);
  }

  void RefreshCurrentGradientAfterOptionChange(bool wasAnimationRunning)
  {
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

    UpdateStatus();
  }

  Gradient::SpreadMethod CurrentSpreadMethod() const
  {
    return SPREAD_METHODS[mSpreadMethodIndex];
  }

  Text::GradientBoundsMode CurrentGradientBoundsMode() const
  {
    return GRADIENT_BOUNDS_MODES[mGradientBoundsModeIndex];
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
      SetHudBadge(mGradientBadge, "TYPE N/A", UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));
      return;
    }

    const GradientKind gradient = CurrentDisplayedGradientKind();
    std::string        text     = "TYPE ";
    text += GetGradientName(gradient);
    SetHudBadge(mGradientBadge,
                text,
                gradient == GradientKind::RADIAL ? UiColor(0x6D28D9) : UiColor(0x075985),
                gradient == GradientKind::RADIAL ? UiColor(0xC4B5FD) : UiColor(0x38BDF8));
  }

  void UpdateStatus()
  {
    SetHudBadge(mResetBadge, "Reset", UiColor(0x7C2D12), UiColor(0xFDBA74));
    SetHudBadge(mClearBadge,
                mGradientApplied ? "Clear" : "Cleared",
                mGradientApplied ? UiColor(0x7F1D1D) : UiColor(0x334155),
                mGradientApplied ? UiColor(0xFCA5A5) : UiColor(0x64748B),
                mGradientApplied ? UiColor(0xF8FAFC) : UiColor(0xCBD5E1));
    UpdateGradientTypeBadge();

    if(mMarqueeMatrixMode)
    {
      SetHudBadge(mCaseBadge, "CASE 0  Matrix", UiColor(0x1D4ED8), UiColor(0x93C5FD));

      std::string spread = "SPREAD ";
      spread += GetSpreadMethodName(CurrentSpreadMethod());
      SetHudBadge(mSpreadBadge, spread, UiColor(0x312E81), UiColor(0x818CF8));

      std::string bounds = "BOUNDS ";
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
                  mAnimationInfo ? "Anim Run" : (canAnimate ? "Anim Ready" : "Anim N/A"),
                  mAnimationInfo ? UiColor(0x0F766E) : (canAnimate ? UiColor(0x075985) : UiColor(0x1E293B)),
                  mAnimationInfo ? UiColor(0x5EEAD4) : (canAnimate ? UiColor(0x38BDF8) : UiColor(0x475569)),
                  mAnimationInfo ? UiColor(0xF8FAFC) : UiColor(0xCBD5E1));

      std::string expected = "MATRIX  H/V, START/CENTER/END, short/long, sync/async | Expected: short cells align to text bounds; long cells use viewport bounds; async matches sync.";
      if(!mGradientApplied)
      {
        expected += " Gradient: cleared; G applies the selected type.";
      }
      else if(mAnimationInfo)
      {
        expected += " Animation: StartOffset drives gradient motion. X resets the current gradient state.";
      }
      else if(!canAnimate)
      {
        expected += " Animation: current gradient does not support StartOffset motion.";
      }
      SetHudBadge(mExpectedBadge, expected, UiColor(0x0F172A), UiColor(0x334155), UiColor(0xCBD5E1));
      return;
    }

    const CaseDefinition& item = CASES[mCaseIndex];

    std::string caseText = "CASE ";
    caseText += std::to_string(mCaseIndex + 1u);
    caseText += "/";
    caseText += std::to_string(CASES.size());
    caseText += "  ";
    caseText += item.title;
    SetHudBadge(mCaseBadge, caseText, UiColor(0x1D4ED8), UiColor(0x93C5FD));

    std::string spread = "SPREAD ";
    spread += GetSpreadMethodName(CurrentSpreadMethod());
    if(item.compactGradientSpan)
    {
      spread += " Short";
    }
    SetHudBadge(mSpreadBadge, spread, UiColor(0x312E81), UiColor(0x818CF8));

    std::string bounds = "BOUNDS ";
    bounds += GetGradientBoundsModeBadgeName(CurrentGradientBoundsMode());
    SetHudBadge(mBoundsBadge, bounds, UiColor(0x164E63), UiColor(0x67E8F9));

    std::string size = "SIZE ";
    size += GetPreviewSizeModeBadgeName(mPreviewSizeMode);
    SetHudBadge(mSizeBadge, size, UiColor(0x78350F), UiColor(0xFBBF24));

    std::string hAlign = "H ";
    hAlign += GetAlignmentName(ALIGNMENTS[mHorizontalAlignmentIndex]);
    SetHudBadge(mHAlignBadge, hAlign, UiColor(0x1E293B), UiColor(0x475569), UiColor(0xCBD5E1));

    std::string vAlign = "V ";
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
                mAnimationInfo ? "Anim Run" : (canAnimate ? "Anim Ready" : "Anim N/A"),
                mAnimationInfo ? UiColor(0x0F766E) : (canAnimate ? UiColor(0x075985) : UiColor(0x1E293B)),
                mAnimationInfo ? UiColor(0x5EEAD4) : (canAnimate ? UiColor(0x38BDF8) : UiColor(0x475569)),
                mAnimationInfo ? UiColor(0xF8FAFC) : UiColor(0xCBD5E1));

    std::string expected = "Coordinate: ";
    expected += item.marquee ? "visible marquee viewport bounds" : "logical text bounds";
    expected += " | ";
    expected += item.expected;
    expected += " ";
    expected += item.notes;
    if(!mGradientApplied)
    {
      expected += " Gradient: cleared; G applies the selected type.";
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
      SetGradientNone();
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
  Label          mGradientBadge;
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
  std::vector<Label> mMarqueeMatrixLabels;
  float              mWindowWidth{static_cast<float>(WINDOW_WIDTH)};
  std::size_t        mCaseIndex{INITIAL_CASE_INDEX};
  std::size_t        mHorizontalAlignmentIndex{INITIAL_ALIGNMENT_INDEX};
  std::size_t        mVerticalAlignmentIndex{INITIAL_ALIGNMENT_INDEX};
  std::size_t        mSpreadMethodIndex{INITIAL_SPREAD_METHOD_INDEX};
  std::size_t        mGradientBoundsModeIndex{INITIAL_GRADIENT_BOUNDS_MODE_INDEX};
  PreviewSizeMode mPreviewSizeMode{PreviewSizeMode::FIXED};
  GradientKind    mGradientTypeOverride{GradientKind::NONE};
  bool            mGradientApplied{true};
  bool            mAnimationInfo{false};
  bool            mMarqueeMatrixMode{false};
  bool            mMarqueeRunning{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextGradientController controller(application);
  application.MainLoop();

  return 0;
}
