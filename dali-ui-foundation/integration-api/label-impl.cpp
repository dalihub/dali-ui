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
 *
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/label-property-handler.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/internal/render-effects/mask-effect-impl.h>
#include <dali-ui-foundation/internal/text/text-style-helper.h>

#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

#define LABEL_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, LabelPropertyIndex, Ui::Integration, LabelImpl, text, valueType, enumIndex)

#define LABEL_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT(text, value, enumIndex) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT_EXTERNAL(Ui::Text, LabelPropertyIndex, Ui::Integration, LabelImpl, text, value, enumIndex)

#define LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(text, enumIndex, baseEnumIndex, componentIndex) \
  DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION_EXTERNAL(Ui::Text, LabelPropertyIndex, Ui::Integration, LabelImpl, text, enumIndex, baseEnumIndex, componentIndex)

#define LABEL_ANIMATABLE_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_ANIMATABLE_PROPERTY_REGISTRATION_EXTERNAL(Ui::Text, LabelPropertyIndex, Ui::Integration, LabelImpl, text, valueType, enumIndex)

// clang-format off
// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(LabelImpl, ViewImpl, Create)

LABEL_PROPERTY_REGISTRATION("text",                       STRING,  TEXT                          )
LABEL_PROPERTY_REGISTRATION("fontFamily",                 STRING,  FONT_FAMILY                   )
LABEL_PROPERTY_REGISTRATION("fontSize",                   FLOAT,   FONT_SIZE                     )
LABEL_PROPERTY_REGISTRATION("multiLine",                  BOOLEAN, MULTI_LINE                    )
LABEL_PROPERTY_REGISTRATION("lineWrapMode",               INTEGER, LINE_WRAP_MODE                )
LABEL_PROPERTY_REGISTRATION("horizontalAlignment",        INTEGER, HORIZONTAL_ALIGNMENT          )
LABEL_PROPERTY_REGISTRATION("verticalAlignment",          INTEGER, VERTICAL_ALIGNMENT            )
LABEL_PROPERTY_REGISTRATION("overflowMode",               INTEGER, OVERFLOW_MODE                 )
LABEL_PROPERTY_REGISTRATION("lineHeight",                 FLOAT,   LINE_HEIGHT                   )
LABEL_PROPERTY_REGISTRATION("lineHeightMode",             INTEGER, LINE_HEIGHT_MODE              )
LABEL_PROPERTY_REGISTRATION("layoutDirectionMode",        INTEGER, LAYOUT_DIRECTION_MODE         )
LABEL_PROPERTY_REGISTRATION("markupEnabled",              BOOLEAN, MARKUP_ENABLED                )
LABEL_PROPERTY_REGISTRATION("anchorColor",                VECTOR4, ANCHOR_COLOR                  )
LABEL_PROPERTY_REGISTRATION("anchorClickedColor",         VECTOR4, ANCHOR_CLICKED_COLOR          )
LABEL_PROPERTY_REGISTRATION("marqueeSpeed",               INTEGER, MARQUEE_SPEED                 )
LABEL_PROPERTY_REGISTRATION("marqueeLoopCount",           INTEGER, MARQUEE_LOOP_COUNT            )
LABEL_PROPERTY_REGISTRATION("marqueeLoopDelay",           FLOAT,   MARQUEE_LOOP_DELAY            )
LABEL_PROPERTY_REGISTRATION("marqueeGap",                 INTEGER, MARQUEE_GAP                   )
LABEL_PROPERTY_REGISTRATION("marqueeStopMode",            INTEGER, MARQUEE_STOP_MODE             )
LABEL_PROPERTY_REGISTRATION("marqueeOrientation",         INTEGER, MARQUEE_ORIENTATION           )
LABEL_PROPERTY_REGISTRATION("fontWeight",                 INTEGER, FONT_WEIGHT                   )
LABEL_PROPERTY_REGISTRATION("fontWidth",                  INTEGER, FONT_WIDTH                    )
LABEL_PROPERTY_REGISTRATION("fontSlant",                  INTEGER, FONT_SLANT                    )
LABEL_PROPERTY_REGISTRATION("textBackgroundColor",        VECTOR4, TEXT_BACKGROUND_COLOR         )
LABEL_PROPERTY_REGISTRATION("fontSizeScale",              FLOAT,   FONT_SIZE_SCALE               )
LABEL_PROPERTY_REGISTRATION("minimumFontSizeScale",       FLOAT,   MINIMUM_FONT_SIZE_SCALE       )
LABEL_PROPERTY_REGISTRATION("maximumFontSizeScale",       FLOAT,   MAXIMUM_FONT_SIZE_SCALE       )
LABEL_PROPERTY_REGISTRATION("systemFontSizeScaleEnabled", BOOLEAN, SYSTEM_FONT_SIZE_SCALE_ENABLED)
LABEL_PROPERTY_REGISTRATION("cutoutEnabled",              BOOLEAN, CUTOUT_ENABLED                )
LABEL_PROPERTY_REGISTRATION("asyncRendering",             BOOLEAN, ASYNC_RENDERING               )
LABEL_PROPERTY_REGISTRATION("renderScale",                FLOAT,   RENDER_SCALE                  )

LABEL_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT("textColor",       Color::BLACK,     TEXT_COLOR       )
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorRed",    TEXT_COLOR_RED,   TEXT_COLOR,     0)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorGreen",  TEXT_COLOR_GREEN, TEXT_COLOR,     1)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorBlue",   TEXT_COLOR_BLUE,  TEXT_COLOR,     2)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorAlpha",  TEXT_COLOR_ALPHA, TEXT_COLOR,     3)
LABEL_ANIMATABLE_PROPERTY_REGISTRATION          (   "pixelSnapFactor", FLOAT,            PIXEL_SNAP_FACTOR)

DALI_TYPE_REGISTRATION_END()
// clang-format on

/**
 * @brief Lookup table that converts Text::Alignment values
 *        to a normalized vertical alignment factor.
 */
const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Text::Alignment::END) + 1] = {
  0.0f, // Text::Alignment::START
  0.5f, // Text::Alignment::CENTER
  1.0f  // Text::Alignment::END
};

/**
 * @brief Discard the given visual into VisualFactory. The visual will be destroyed at next idle time.
 * @param[in,out] visual Visual to be discarded. It will be reset to an empty handle.
 */
void DiscardLabelVisual(Dali::Ui::Visual::Base& visual)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && visual))
  {
    Dali::Ui::VisualFactory::Get().DiscardVisual(visual);
  }
  visual.Reset();
}

} // namespace

LabelImplPtr LabelImpl::New()
{
  return LabelImplPtr(new LabelImpl());
}

LabelImpl::LabelImpl()
: ViewImpl(),
  mSize(),
  mTouchPosition(),
  mLineHeight(Text::LINE_HEIGHT_AUTO),
  mLineHeightMode(Text::LineHeightMode::RELATIVE),
  mOverflowMode(Text::OverflowMode::ELLIPSIS),
  mAsyncLineCount(0),
  mTextColorAnimatedCount(0),
  mRendererUpdateNeeded(false),
  mMeasureInvalidated(false),
  mIsAsyncRenderRequested(false),
  mIsAsyncRenderLayoutDirty(false),
  mLastMarqueeEnabled(false),
  mIsTouchDown(false),
  mHasAnchors(false),
  mIsVisible(false),
  mIsVisibleInitialized(false),
  mIsViewBackgroundEnabled(true),
  mIsManualRenderInProgress(false),
  mIsManualRenderFinished(false)
{
}

LabelImpl::~LabelImpl()
{
  // This prevents access to the async text interface until the visual is actually destroyed.
  Internal::TextVisual::SetAsyncTextInterface(mVisual, nullptr);
  DiscardLabelVisual(mVisual);
}

// =============================================================================
// Properties
// =============================================================================
void LabelImpl::SetText(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());
  mController->SetText(ToStdString(text));
  UpdateAnchorTouchInterception();
  InvalidateTextMeasure();
}

Dali::String LabelImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return ToDaliString(text);
}

void LabelImpl::SetFontFamily(const Dali::String& fontFamily)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), fontFamily.CStr());
  mController->SetDefaultFontFamily(ToStdString(fontFamily));
}

Dali::String LabelImpl::GetFontFamily() const
{
  return ToDaliString(mController->GetDefaultFontFamily());
}

void LabelImpl::SetFontSize(float fontSize)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), fontSize);
  if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize, Math::MACHINE_EPSILON_1000))
  {
    mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
    InvalidateTextMeasure();
  }
}

float LabelImpl::GetFontSize() const
{
  return mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE);
}

void LabelImpl::SetMultiLine(bool multiLine)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), multiLine);
  mController->SetMultiLineEnabled(multiLine);
  UpdateMarqueeState();
}

bool LabelImpl::IsMultiLine() const
{
  return mController->IsMultiLineEnabled();
}

void LabelImpl::SetLineWrapMode(Text::LineWrapMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  mController->SetLineWrapMode(mode);
}

Text::LineWrapMode LabelImpl::GetLineWrapMode() const
{
  return mController->GetLineWrapMode();
}

void LabelImpl::SetTextColor(const UiColor& color)
{
  SetColorBinding("TextColor", color, this, &LabelImpl::SetTextColorInternal);
}

UiColor LabelImpl::GetTextColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextColor", outColor))
  {
    return outColor;
  }
  return mController->GetDefaultColor();
}

void LabelImpl::SetHorizontalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetHorizontalAlignment(alignment);
}

Text::Alignment LabelImpl::GetHorizontalTextAlignment() const
{
  return mController->GetHorizontalAlignment();
}

void LabelImpl::SetVerticalTextAlignment(Text::Alignment alignment)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);
  mController->SetVerticalAlignment(alignment);
}

Text::Alignment LabelImpl::GetVerticalTextAlignment() const
{
  return mController->GetVerticalAlignment();
}

void LabelImpl::SetOverflowMode(Text::OverflowMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mode != mOverflowMode)
  {
    mOverflowMode = mode;
    switch(mode)
    {
      case Text::OverflowMode::CLIP:
      {
        mController->SetTextElideEnabled(false);
        break;
      }
      case Text::OverflowMode::ELLIPSIS:
      {
        mController->SetTextElideEnabled(true);
        break;
      }
    }
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Text::OverflowMode LabelImpl::GetOverflowMode() const
{
  return mOverflowMode;
}

void LabelImpl::SetLineHeight(float lineHeight)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), lineHeight);
  if(mLineHeight != lineHeight)
  {
    mLineHeight = lineHeight;
    UpdateLineHeight();
    InvalidateTextMeasure();
  }
}

float LabelImpl::GetLineHeight() const
{
  return mLineHeight;
}

void LabelImpl::SetLineHeightMode(Text::LineHeightMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mLineHeightMode != mode)
  {
    mLineHeightMode = mode;
    UpdateLineHeight();
    InvalidateTextMeasure();
  }
}

Text::LineHeightMode LabelImpl::GetLineHeightMode() const
{
  return mLineHeightMode;
}

void LabelImpl::SetLayoutDirectionMode(Text::LayoutDirectionMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  if(mController->GetLayoutDirectionMode() != mode)
  {
    mController->SetLayoutDirectionMode(mode);
  }
}

Text::LayoutDirectionMode LabelImpl::GetLayoutDirectionMode() const
{
  return mController->GetLayoutDirectionMode();
}

void LabelImpl::SetMarkupEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetMarkupProcessorEnabled(enabled);
  UpdateAnchorTouchInterception();
}

bool LabelImpl::IsMarkupEnabled() const
{
  return mController->IsMarkupProcessorEnabled();
}

void LabelImpl::SetAnchorColor(const UiColor& color)
{
  SetColorBinding("AnchorColor", color, this, &LabelImpl::SetAnchorColorInternal);
}

UiColor LabelImpl::GetAnchorColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "AnchorColor", outColor))
  {
    return outColor;
  }
  return mController->GetAnchorColor();
}

void LabelImpl::SetAnchorClickedColor(const UiColor& color)
{
  SetColorBinding("AnchorClickedColor", color, this, &LabelImpl::SetAnchorClickedColorInternal);
}

UiColor LabelImpl::GetAnchorClickedColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "AnchorClickedColor", outColor))
  {
    return outColor;
  }
  return mController->GetAnchorClickedColor();
}

void LabelImpl::SetMarqueeSpeed(int speed)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), speed);
  GetTextScroller()->SetSpeed(speed);
}

int LabelImpl::GetMarqueeSpeed() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetSpeed();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeSpeed();
}

void LabelImpl::SetMarqueeLoopCount(int loopCount)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), loopCount);
  GetTextScroller()->SetLoopCount(loopCount);
}

int LabelImpl::GetMarqueeLoopCount() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetLoopCount();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeLoopCount();
}

void LabelImpl::SetMarqueeLoopDelay(float delay)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), delay);
  GetTextScroller()->SetLoopDelay(delay);
}

float LabelImpl::GetMarqueeLoopDelay() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetLoopDelay();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeLoopDelay();
}

void LabelImpl::SetMarqueeGap(int gap)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), gap);
  GetTextScroller()->SetGap(gap);
}

int LabelImpl::GetMarqueeGap() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetGap();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeGap();
}

void LabelImpl::SetMarqueeStopMode(Text::MarqueeStopMode mode)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(mode));
  GetTextScroller()->SetStopMode(mode);
}

Text::MarqueeStopMode LabelImpl::GetMarqueeStopMode() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetStopMode();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeStopMode();
}

void LabelImpl::SetMarqueeOrientation(Text::MarqueeOrientation orientation)
{
  DALI_LOG_RELEASE_INFO("[%p] %u\n", mController.Get(), static_cast<uint32_t>(orientation));
  GetTextScroller()->SetOrientation(orientation);
  UpdateMarqueeState();
}

Text::MarqueeOrientation LabelImpl::GetMarqueeOrientation() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetOrientation();
  }
  return Integration::UiConfigManager::Get().GetConfig().GetMarqueeOrientation();
}

void LabelImpl::SetFontWeight(Text::FontWeight weight)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWeight::Name[weight]);
  mController->SetDefaultFontWeight(weight);
}

Text::FontWeight LabelImpl::GetFontWeight() const
{
  return mController->GetDefaultFontWeight();
}

void LabelImpl::SetFontWidth(Text::FontWidth width)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWidth::Name[width]);
  mController->SetDefaultFontWidth(width);
}

Text::FontWidth LabelImpl::GetFontWidth() const
{
  return mController->GetDefaultFontWidth();
}

void LabelImpl::SetFontSlant(Text::FontSlant slant)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontSlant::Name[slant]);
  mController->SetDefaultFontSlant(slant);
}

Text::FontSlant LabelImpl::GetFontSlant() const
{
  return mController->GetDefaultFontSlant();
}

void LabelImpl::SetTextBackgroundColor(const UiColor& color)
{
  SetColorBinding("TextBackgroundColor", color, this, &LabelImpl::SetTextBackgroundColorInternal);
  if(!mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(true);
  }
}

UiColor LabelImpl::GetTextBackgroundColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "TextBackgroundColor", outColor))
  {
    return outColor;
  }
  return mController->GetBackgroundColor();
}

void LabelImpl::ClearTextBackgroundColor()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "TextBackgroundColor");
  if(mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(false);
    mController->SetBackgroundColor(Color::TRANSPARENT);
  }
}

void LabelImpl::SetUnderline(const Text::Underline& underline)
{
  const UiColor& color = underline.GetColor();

  SetColorBinding("UnderlineColor", color, this, &LabelImpl::SetUnderlineColorInternal);

  if(Text::ApplyUnderlineStyle(mController, underline))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

void LabelImpl::ClearUnderline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "UnderlineColor");
  if(mController->IsUnderlineEnabled())
  {
    mController->SetUnderlineEnabled(false);
  }
}

void LabelImpl::SetShadow(const Text::Shadow& shadow)
{
  const UiColor& color = shadow.GetColor();

  SetColorBinding("ShadowColor", color, this, &LabelImpl::SetShadowColorInternal);

  if(Text::ApplyShadowStyle(mController, shadow))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

void LabelImpl::ClearShadow()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "ShadowColor");
  if(Vector2::ZERO != mController->GetShadowOffset())
  {
    mController->SetShadowOffset(Vector2::ZERO);
  }
}

void LabelImpl::SetOutline(const Text::Outline& outline)
{
  const UiColor& color = outline.GetColor();

  SetColorBinding("OutlineColor", color, this, &LabelImpl::SetOutlineColorInternal);

  if(Text::ApplyOutlineStyle(mController, outline))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

void LabelImpl::ClearOutline()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "OutlineColor");
  if(0u != mController->GetOutlineWidth())
  {
    mController->SetOutlineWidth(0u);
  }
}

void LabelImpl::SetLineThrough(const Text::LineThrough& lineThrough)
{
  const UiColor& color = lineThrough.GetColor();

  SetColorBinding("LineThroughColor", color, this, &LabelImpl::SetLineThroughColorInternal);

  if(Text::ApplyLineThroughStyle(mController, lineThrough))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

void LabelImpl::ClearLineThrough()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "LineThroughColor");
  if(mController->IsStrikethroughEnabled())
  {
    mController->SetStrikethroughEnabled(false);
  }
}

void LabelImpl::SetBevel(const Text::Bevel& bevel)
{
  const UiColor& lightColor  = bevel.GetLightColor();
  const UiColor& shadowColor = bevel.GetShadowColor();

  SetColorBinding("BevelLightColor", lightColor, this, &LabelImpl::SetBevelLightColorInternal);
  SetColorBinding("BevelShadowColor", shadowColor, this, &LabelImpl::SetBevelShadowColorInternal);

  if(Text::ApplyBevelStyle(mController, bevel))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

void LabelImpl::ClearBevel()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  UiColorManager::Get().ClearBinding(Self(), "BevelLightColor");
  UiColorManager::Get().ClearBinding(Self(), "BevelShadowColor");
  if(mController->IsEmbossEnabled())
  {
    mController->SetEmbossEnabled(false);
  }
}

void LabelImpl::SetTextFit(const Text::FitRange& range)
{
  DALI_LOG_RELEASE_INFO("[%p] min:%f, max:%f, step:%f\n", mController.Get(), range.GetMinimumFontSize(), range.GetMaximumFontSize(), range.GetFontSizeStep());
  // If TextFitCandidates is enabled, this should be disabled.
  if(mController->IsTextFitCandidatesEnabled())
  {
    mController->SetTextFitCandidatesEnabled(false);
    mController->ClearTextFitCandidates();
  }
  mController->SetTextFitEnabled(true);
  // Use the current line size as the baseline for text fit.
  mController->SetTextFitLineSize(mController->GetDefaultLineSize());
  mController->SetTextFitMinSize(range.GetMinimumFontSize(), Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitMaxSize(range.GetMaximumFontSize(), Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitStepSize(range.GetFontSizeStep(), Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitChanged(true);
  InvalidateTextMeasure();
}

void LabelImpl::SetTextFit(const Dali::Vector<Text::FitCandidate>& candidates)
{
  DALI_LOG_RELEASE_INFO("[%p] number of candidates:%u\n", mController.Get(), candidates.Count());
  // If TextFit is enabled, this should be disabled.
  if(mController->IsTextFitEnabled())
  {
    mController->SetTextFitEnabled(false);
  }
  mController->SetTextFitCandidatesEnabled(true);
  mController->SetTextFitCandidates(candidates);
  InvalidateTextMeasure();
}

void LabelImpl::ClearTextFit()
{
  if(mController->IsTextFitEnabled() || mController->IsTextFitCandidatesEnabled())
  {
    mController->SetTextFitEnabled(false);
    mController->SetTextFitCandidatesEnabled(false);
    mController->ClearTextFitCandidates();
    UpdateLineHeight();
    InvalidateTextMeasure();
  }
}

void LabelImpl::SetFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetFontSizeScale(scale);
}

float LabelImpl::GetFontSizeScale() const
{
  return mController->GetFontSizeScale();
}

void LabelImpl::SetMinimumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMinimumFontSizeScale(scale);
}

float LabelImpl::GetMinimumFontSizeScale() const
{
  return mController->GetMinimumFontSizeScale();
}

void LabelImpl::SetMaximumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetMaximumFontSizeScale(scale);
}

float LabelImpl::GetMaximumFontSizeScale() const
{
  return mController->GetMaximumFontSizeScale();
}

void LabelImpl::SetSystemFontSizeScaleEnabled(bool enabled)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  mController->SetSystemFontSizeScaleEnabled(enabled);
}

bool LabelImpl::IsSystemFontSizeScaleEnabled() const
{
  return mController->IsSystemFontSizeScaleEnabled();
}

void LabelImpl::SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes)
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p] number of candidates:%u\n", mController.Get(), axes.Count());
  mController->SetVariations(axes);
}

void LabelImpl::SetFontVariation(const Dali::String& settings)
{
  if(settings.Empty())
  {
    DALI_LOG_WARNING("[%p] Empty font variation string is not allowed. Use ClearFontVariation() instead.\n", mController.Get());
    return;
  }

  auto axes = Text::FontVariation::FromString(settings);
  if(axes.Empty())
  {
    DALI_LOG_WARNING("[%p] Failed to parse font variation string: %s\n", mController.Get(), settings.CStr());
    return;
  }

  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), settings.CStr());
  SetFontVariation(axes);
}

Dali::Vector<Text::FontVariationAxis> LabelImpl::GetFontVariation() const
{
  return mController->GetVariations();
}

void LabelImpl::ClearFontVariation()
{
  // InvalidateMeasure() may be called if needed.
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  mController->ClearVariationsMap();
}

// Integration-only implementation for now until public API support is introduced.
Dali::Property::Index LabelImpl::RegisterFontVariationProperty(const Dali::String& tag)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), tag.CStr());

  if(tag.Size() != 4u) // Variation axis tag must be 4 characters.
  {
    DALI_LOG_ERROR("Font variation registration failed. The tag length is not 4.\n");
    return Property::INVALID_INDEX;
  }

  Actor self = Self();

  Property::Map variationsMap;
  mController->GetVariationsMap(variationsMap);

  float value = 0.0f;
  if(const Property::Value* tagValue = variationsMap.Find(tag))
  {
    tagValue->Get(value);
  }

  const Dali::Property::Index index    = self.RegisterProperty(tag.CStr(), value);
  const bool                  inserted = mVariationIndexMap.emplace(index, tag).second;
  if(inserted)
  {
    PropertyNotification notification = self.AddPropertyNotification(index, StepCondition(1.0f));
    // TODO: Make step value customizable by user.
    notification.NotifySignal().Connect(this, &LabelImpl::OnVariationPropertyNotify);
    // TODO: Support UnregisterProperty() and remove the tag from mVariationIndexMap.
  }
  return index;
}

void LabelImpl::SetCutoutEnabled(bool enabled)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), enabled);
  // Set through the property system so that dependent background and rendering
  // state can be updated consistently in OnPropertySet().
  Self().SetProperty(Text::LabelPropertyIndex::CUTOUT_ENABLED, enabled);
}

bool LabelImpl::IsCutoutEnabled() const
{
  return mController->IsTextCutout();
}

// Integration-only implementation for now until public API support is introduced.
void LabelImpl::SetLetterSpacing(float spacing)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), spacing);
  mController->SetCharacterSpacing(spacing);
}

float LabelImpl::GetLetterSpacing() const
{
  return mController->GetCharacterSpacing();
}

void LabelImpl::SetMaskEffect(View view)
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  if(!view)
  {
    DALI_LOG_WARNING("[%p] SetMaskEffect called with invalid view\n", mController.Get());
    return;
  }

  ClearMaskEffect();

  View selfView = Ui::View::DownCast(Self());

  Self().Add(view);
  mMaskSourceView = view;

  MaskEffect maskEffect = MaskEffect::New(view);
  GetImplementation(maskEffect).SetReverseMaskDirection(true);
  selfView.SetRenderEffect(maskEffect);
}

void LabelImpl::ClearMaskEffect()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  View selfView = Ui::View::DownCast(Self());

  View view = mMaskSourceView.GetHandle();
  if(view)
  {
    Self().Remove(view);
  }

  mMaskSourceView.Reset();
  selfView.ClearRenderEffect();
}

void LabelImpl::SetAsyncRendering(bool asyncRendering)
{
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), asyncRendering);
  mController->SetAsyncRendering(asyncRendering);
}

bool LabelImpl::IsAsyncRendering() const
{
  return mController->IsAsyncRendering();
}

void LabelImpl::SetRenderScale(float scale)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), scale);
  mController->SetRenderScale(scale);
}

float LabelImpl::GetRenderScale() const
{
  return mController->GetRenderScale();
}

// =============================================================================
// Read Only
// =============================================================================
int LabelImpl::GetLineCount()
{
  float width = Self().GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  float clamp = std::clamp(width, GetMinimumWidth(), GetMaximumWidth());
  DALI_LOG_RELEASE_INFO("[%p] width:%f, min:%f, max:%f, clamp:%f\n", mController.Get(), width, GetMinimumWidth(), GetMaximumWidth(), clamp);
  return GetLineCount(clamp);
}

int LabelImpl::GetLineCount(float width)
{
  Extents padding      = GetPadding();
  float   contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  DALI_LOG_RELEASE_INFO("[%p] contentWidth:%f, padding start:%d, end:%d\n", mController.Get(), contentWidth, padding.start, padding.end);
  return mController->GetLineCount(contentWidth);
}

int LabelImpl::GetAsyncLineCount() const
{
  return mAsyncLineCount;
}

bool LabelImpl::IsMarqueeRunning() const
{
  if(mTextScroller)
  {
    return mTextScroller->IsScrolling();
  }
  return false;
}

float LabelImpl::GetAdjustedFontSizeScale() const
{
  return mController->GetAdjustedFontSizeScale();
}

// =============================================================================
// Method
// =============================================================================
void LabelImpl::StartMarquee()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  SetMarqueeEnabled(true);
}

void LabelImpl::StopMarquee()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  SetMarqueeEnabled(false);
}

void LabelImpl::RequestAsyncNaturalSize()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  Actor                             self            = Self();
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  Text::AsyncTextParameters         parameters      = GetAsyncTextParameters(Text::Async::COMPUTE_NATURAL_SIZE, Size::ZERO, GetPadding(), layoutDirection);
  Internal::TextVisual::RequestAsyncSizeComputation(mVisual, parameters);
}

void LabelImpl::RequestAsyncHeightForWidth(float width)
{
  DALI_LOG_RELEASE_INFO("[%p] width:%f\n", mController.Get(), width);
  Actor                             self            = Self();
  Extents                           padding         = GetPadding();
  float                             contentWidth    = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  Text::AsyncTextParameters         parameters      = GetAsyncTextParameters(Text::Async::COMPUTE_HEIGHT_FOR_WIDTH, Size(contentWidth, 0.0f), padding, layoutDirection);
  Internal::TextVisual::RequestAsyncSizeComputation(mVisual, parameters);
}

// =============================================================================
// Integration-only
// =============================================================================
void LabelImpl::RequestAsyncRenderWithFixedSize(float width, float height)
{
  DALI_LOG_RELEASE_INFO("[%p] Request fixed size render: %f, %f\n", mController.Get(), width, height);
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_DEBUG_INFO("async render request ignored because async rendering is disabled.\n");
    return;
  }

  Actor                             self            = Self();
  Extents                           padding         = GetPadding();
  float                             contentWidth    = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeight   = std::max(height - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_FIXED_SIZE, Size(contentWidth, contentHeight), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithFixedWidth(float width, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] Request fixed width render: width=%f, heightConstraint=%f\n", mController.Get(), width, heightConstraint);
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_DEBUG_INFO("async render request ignored because async rendering is disabled.\n");
    return;
  }

  Actor                             self                    = Self();
  Extents                           padding                 = GetPadding();
  float                             contentWidth            = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeightConstraint = std::max(heightConstraint - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection         = mController->GetLayoutDirection(self);

  Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_FIXED_WIDTH, Size(contentWidth, contentHeightConstraint), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithFixedHeight(float widthConstraint, float height)
{
  DALI_LOG_RELEASE_INFO("[%p] Request fixed height render: widthConstraint=%f, height=%f\n", mController.Get(), widthConstraint, height);
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_DEBUG_INFO("async render request ignored because async rendering is disabled.\n");
    return;
  }

  Actor                             self                   = Self();
  Extents                           padding                = GetPadding();
  float                             contentWidthConstraint = std::max(widthConstraint - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeight          = std::max(height - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection        = mController->GetLayoutDirection(self);

  Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_FIXED_HEIGHT, Size(contentWidthConstraint, contentHeight), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithConstraints(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] Request constrained render: %f, %f\n", mController.Get(), widthConstraint, heightConstraint);
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_DEBUG_INFO("async render request ignored because async rendering is disabled.\n");
    return;
  }

  Actor                             self                    = Self();
  Extents                           padding                 = GetPadding();
  float                             contentWidthConstraint  = std::max(widthConstraint - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeightConstraint = std::max(heightConstraint - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection         = mController->GetLayoutDirection(self);

  Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_CONSTRAINT, Size(contentWidthConstraint, contentHeightConstraint), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRenderer(mVisual, parameters);
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View, const Dali::String&)>& LabelImpl::AnchorClickedSignal()
{
  return mAnchorClickedSignal;
}

Signal<void(View, float, float)>& LabelImpl::AsyncRenderFinishedSignal()
{
  return mAsyncRenderFinishedSignal;
}

Signal<void(View, float, float)>& LabelImpl::AsyncNaturalSizeComputedSignal()
{
  return mAsyncNaturalSizeComputedSignal;
}

Signal<void(View, float, float)>& LabelImpl::AsyncHeightForWidthComputedSignal()
{
  return mAsyncHeightForWidthComputedSignal;
}

// =============================================================================
// Config
// =============================================================================
void LabelImpl::ApplyInitialConfig()
{
  // UiConfigManager may not be initialized during preload phase
  if(!UiConfigManager::Get().IsInitialized())
  {
    DALI_LOG_RELEASE_INFO("ApplyInitialConfig skipped: UiConfigManager is not initialized (possible preload phase)\n");
    return;
  }

  const auto& config = UiConfigManager::Get().GetConfig();
  SetFontSize(config.GetDefaultFontSize());
  SetTextColor(config.GetDefaultTextColor());
  SetAsyncRendering(config.IsLabelAsyncRendering());
}

// =============================================================================
// ViewImpl
// =============================================================================
void LabelImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  Actor self = Self();

  Dali::Property::Map propertyMap;
  propertyMap.Add(Ui::Visual::Property::TYPE, Ui::Visual::TEXT);

  mVisual   = Ui::VisualFactory::Get().CreateVisual(propertyMap);
  View view = Ui::View::DownCast(self);
  Internal::ViewDataImpl::Get(GetImpl(view)).RegisterVisual(Text::LabelPropertyIndex::TEXT, mVisual, DepthIndex::CONTENT);

  Internal::TextVisual::SetAsyncTextInterface(mVisual, this);
  Internal::TextVisual::SetAnimatableTextColorProperty(mVisual, Text::LabelPropertyIndex::TEXT_COLOR);
  Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);

  mController = Internal::TextVisual::GetController(mVisual);
  DALI_ASSERT_DEBUG(mController && "Invalid Text Controller")
  mController->SetControlInterface(this);
  mController->SetAnchorControlInterface(this);

  self.SetProperty(Text::LabelPropertyIndex::PIXEL_SNAP_FACTOR, 0.0f);

  // Use height-for-width negotiation by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

  // Enable the text ellipsis.
  mController->SetTextElideEnabled(true);

  // Sets layoutDirection value
  Dali::Stage                 stage           = Dali::Stage::GetCurrent();
  Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(stage.GetRootLayer().GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());
  mController->SetLayoutDirection(layoutDirection);

  self.LayoutDirectionChangedSignal().Connect(this, &LabelImpl::OnLayoutDirectionChanged);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &LabelImpl::OnLocaleChanged);
  }

  Text::Layout::Engine& engine = mController->GetLayoutEngine();
  engine.SetCursorWidth(0u);

  mController->SetVerticalLineAlignment(Text::Alignment::CENTER);

  ApplyInitialConfig();
}

void LabelImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  const bool asyncRenderLayoutDirty = mIsAsyncRenderLayoutDirty;
  const bool manualRenderFinished   = mIsManualRenderFinished;
  mIsAsyncRenderLayoutDirty         = false;
  mIsManualRenderFinished           = false;

  if(mTextScroller && mTextScroller->IsStopRequested())
  {
    // When marquee stop is requested in FINISH_LOOP mode, defer relayout until scrolling finishes.
    return;
  }

  Actor self = Self();

  Extents padding = GetPadding();
  float   width   = std::max(size.x - static_cast<float>(padding.start + padding.end), 0.0f);
  float   height  = std::max(size.y - static_cast<float>(padding.top + padding.bottom), 0.0f);
  Vector2 contentSize(width, height);
  DALI_LOG_RELEASE_INFO("[%p] size:%f,%f, contentSize:%f,%f\n", mController.Get(), size.x, size.y, contentSize.x,
                        contentSize.y);

  // Support Right-To-Left
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  // Support Right-To-Left of padding
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  if(mController->IsAsyncRendering())
  {
    if(mTextScroller && mTextScroller->IsScrolling() && !(mRendererUpdateNeeded || asyncRenderLayoutDirty))
    {
      // When marquee is playing, a text load request is made only if a text update is absolutely necessary.
      return;
    }

    if(mIsManualRenderInProgress || !(asyncRenderLayoutDirty || mIsAsyncRenderRequested))
    {
      // Do not request async render while a manual render is in progress,
      // or when there are no size or property updates.
      return;
    }

    if(manualRenderFinished && asyncRenderLayoutDirty && !mIsAsyncRenderRequested)
    {
      // Skip async render when only the size changed immediately after manual render completion.
      // This avoids redundant recomputation when users resize the label in the completion callback.
      // Note: This behavior may have limitations in some edge cases.
      return;
    }

    DALI_LOG_RELEASE_INFO("[%p] Request render, size : %f, %f [%s] \n", mController.Get(), contentSize.width, contentSize.height, GetText().CStr());

    Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_FIXED_SIZE, contentSize, padding, layoutDirection);
    Internal::TextVisual::UpdateAsyncRenderer(mVisual, parameters);
    mRendererUpdateNeeded   = false;
    mIsAsyncRenderRequested = false;
    return;
  }

  if(mController->IsTextFitCandidatesEnabled())
  {
    mController->FitCandidatesPointSizeForLayout(contentSize);
    mController->SetTextFitContentSize(contentSize);
  }
  else if(mController->IsTextFitEnabled())
  {
    mController->FitPointSizeforLayout(contentSize);
    mController->SetTextFitContentSize(contentSize);
  }

  const Text::MarqueeOrientation marqueeOrientation = mTextScroller ? mTextScroller->GetOrientation() : Text::MarqueeOrientation::HORIZONTAL;
  // TODO: This is only meaningful after marquee ellipsis mode is supported.
  EvaluateAndApplyMarquee(contentSize, marqueeOrientation);

  Size originSize = Size::ZERO;
  PrepareMarqueeLayout(contentSize, marqueeOrientation, originSize);

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType)) || mRendererUpdateNeeded)
  {
    // Update the visual
    Internal::TextVisual::EnableRendererUpdate(mVisual);

    // Calculate the size of the visual that can fit the text
    Size layoutSize = mController->GetTextModel()->GetLayoutSize();
    layoutSize.x    = contentSize.x;

    const Vector2& shadowOffset = mController->GetTextModel()->GetShadowOffset();
    if(shadowOffset.y > Math::MACHINE_EPSILON_1)
    {
      layoutSize.y += shadowOffset.y;
    }

    float outlineWidth = mController->GetTextModel()->GetOutlineWidth();
    layoutSize.y += outlineWidth * 2.0f;
    layoutSize.y = std::min(layoutSize.y, contentSize.y);

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y = (marqueeOrientation == Text::MarqueeOrientation::VERTICAL) ? 0.0f : (contentSize.y - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[static_cast<int>(mController->GetVerticalAlignment())];

    const int maxTextureSize = Dali::GetMaxTextureSize();
    if(layoutSize.width > maxTextureSize)
    {
      DALI_LOG_DEBUG_INFO("[%p] layout width (%.2f) exceeds max texture size (%d), clamped to %d\n", mController.Get(), layoutSize.width, maxTextureSize, maxTextureSize);
      layoutSize.width = maxTextureSize;
    }

    // This affects font rendering quality.
    // It need to be integerized.
    Vector2 visualTransformOffset;
    visualTransformOffset.x = roundf(padding.start + alignmentOffset.x);
    visualTransformOffset.y = roundf(padding.top + alignmentOffset.y);

    mController->SetLayoutAlignmentOffset(alignmentOffset);
    mController->SetLayoutOffsetWithPadding(visualTransformOffset);

    Vector2 visualTransformSize = (marqueeOrientation == Text::MarqueeOrientation::VERTICAL) ? contentSize : layoutSize;

    Dali::Property::Map visualTransform;
    visualTransform.Add(Ui::Visual::Transform::Property::SIZE, visualTransformSize)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::OFFSET, visualTransformOffset)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::ANCHOR_POINT, Ui::Align::TOP_BEGIN);
    mVisual.SetTransformAndSize(visualTransform, size);

    if(mController->IsMarqueeEnabled())
    {
      InitializeMarquee(contentSize, originSize);
    }

    mRendererUpdateNeeded = false;
  }

  if(mController->IsTextFitChanged())
  {
    mController->SetTextFitChanged(false);
    EmitTextFitChanged();
  }
}

Vector3 LabelImpl::GetNaturalSize()
{
  Extents padding     = GetPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += static_cast<float>(padding.start + padding.end);
  naturalSize.height += static_cast<float>(padding.top + padding.bottom);

  return naturalSize;
}

float LabelImpl::GetHeightForWidth(float width)
{
  Extents padding      = GetPadding();
  float   contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  return mController->GetHeightForWidth(contentWidth) + static_cast<float>(padding.top + padding.bottom);
}

MeasuredSize LabelImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint,
                        heightConstraint);

  mMeasureInvalidated = false;

  const float requestedWidth  = GetRequestedWidth();
  const float requestedHeight = GetRequestedHeight();

  const float minWidth  = GetMinimumWidth();
  const float maxWidth  = GetMaximumWidth();
  const float minHeight = GetMinimumHeight();
  const float maxHeight = GetMaximumHeight();
  const float fontSize  = GetFontSize();

  const bool useTextFitRange    = mController->IsTextFitEnabled();
  const bool useFitCandidates   = mController->IsTextFitCandidatesEnabled();
  const bool wrapContentMeasure = (requestedWidth == WRAP_CONTENT) || (requestedHeight == WRAP_CONTENT);

  // Measure wrap-content size using a representative maximum text fit configuration.
  if(useTextFitRange && wrapContentMeasure)
  {
    mController->SetTextFitEnabled(false);
    mController->SetDefaultFontSize(mController->GetTextFitMaxSize(Text::Controller::PIXEL_SIZE), Text::Controller::PIXEL_SIZE);
  }
  else if(useFitCandidates && wrapContentMeasure)
  {
    mController->SetTextFitCandidatesEnabled(false);

    const Text::FitCandidate* fitCandidate = mController->GetMaxFitCandidate();
    if(fitCandidate)
    {
      mController->SetDefaultFontSize(fitCandidate->GetFontSize(), Text::Controller::PIXEL_SIZE);
      mController->SetDefaultLineSize(fitCandidate->GetLineHeight());
    }
    else
    {
      DALI_LOG_ERROR("TextFit (FitCandidate) is enabled but no candidates are available\n");
    }
  }

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(requestedWidth >= 0.0f)
  {
    measuredWidth = std::max(std::min(requestedWidth, maxWidth), minWidth);
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    // MATCH_PARENT: report minimum desired size; actual size is determined
    // by the parent during the Arrange phase.
    measuredWidth = minWidth;
  }
  else // WRAP_CONTENT
  {
    const Vector3 naturalSize     = GetNaturalSize();
    const float   width           = std::max(0.0f, naturalSize.width);
    const float   allowedMaxWidth = (widthConstraint >= 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;
    measuredWidth                 = std::max(std::min(width, allowedMaxWidth), minWidth);
  }

  // Height
  if(requestedHeight >= 0.0f)
  {
    measuredHeight = std::max(std::min(requestedHeight, maxHeight), minHeight);
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    measuredHeight = minHeight;
  }
  else // WRAP_CONTENT
  {
    const float allowedMaxHeight = (heightConstraint >= 0.0f) ? std::min(maxHeight, heightConstraint) : maxHeight;
    // When width is MATCH_PARENT, measuredWidth is minWidth (0 by default).
    // Use widthConstraint for height calculation since that represents the
    // actual available width the label will receive in Arrange.
    const float widthForHeight = (requestedWidth == MATCH_PARENT) ? std::max(0.0f, widthConstraint) : measuredWidth;
    const float height         = (widthForHeight > 0.0f) ? std::max(0.0f, GetHeightForWidth(widthForHeight)) : 0.0f;
    measuredHeight             = std::max(std::min(height, allowedMaxHeight), minHeight);
  }

  if(useTextFitRange && wrapContentMeasure)
  {
    mController->SetTextFitEnabled(true);
    mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
    UpdateLineHeight();
  }
  else if(useFitCandidates && wrapContentMeasure)
  {
    mController->SetTextFitCandidatesEnabled(true);
    mController->SetDefaultFontSize(fontSize, Text::Controller::PIXEL_SIZE);
    UpdateLineHeight();
  }

  DALI_LOG_RELEASE_INFO("[%p] measured:%f,%f\n", mController.Get(), measuredWidth, measuredHeight);
  return MeasuredSize(measuredWidth, measuredHeight);
}

MeasuredSize LabelImpl::OnArrange(const LayoutRect& bounds)
{
  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, bounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

  DALI_LOG_RELEASE_INFO("[%p] pos:%f,%f, size:%f,%f\n", mController.Get(), bounds.x, bounds.y, bounds.width,
                        bounds.height);
  return {bounds.width, bounds.height};
}

void LabelImpl::OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index, Animation::State state)
{
  if(DALI_LIKELY(mVisual) && index == Text::LabelPropertyIndex::TEXT_COLOR)
  {
    if(state == Animation::State::PLAYING)
    {
      ++mTextColorAnimatedCount;
    }
    else if(state == Animation::State::STOPPED)
    {
      if(mTextColorAnimatedCount)
      {
        --mTextColorAnimatedCount;
      }
    }

    Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);
  }
  ViewImpl::OnAnimateAnimatableProperty(animation, index, state);
}

void LabelImpl::OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied)
{
  if(DALI_LIKELY(mVisual) && index == Text::LabelPropertyIndex::TEXT_COLOR)
  {
    if(applied)
    {
      ++mTextColorAnimatedCount;
    }
    else
    {
      if(mTextColorAnimatedCount)
      {
        --mTextColorAnimatedCount;
      }
    }

    Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);
  }
  ViewImpl::OnConstraintAnimatableProperty(constraint, index, applied);
}

// =============================================================================
// ControlInterface
// =============================================================================
void LabelImpl::RequestTextRelayout()
{
  // Signal that a Relayout may be needed
  RelayoutRequest();
}

void LabelImpl::InvalidateTextMeasure()
{
  if(!mMeasureInvalidated)
  {
    InvalidateMeasure();
    mMeasureInvalidated = true;
  }
}

void LabelImpl::RequestAsyncRender()
{
  mIsAsyncRenderRequested = true;
}

// =============================================================================
// ScrollerInterface
// =============================================================================
void LabelImpl::ScrollingFinished()
{
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  mController->SetMarqueeEnabled(false);
  RequestTextRelayout();
  RequestAsyncRender();
}

// =============================================================================
// AnchorControlInterface
// =============================================================================
bool LabelImpl::AnchorClicked(uint32_t cursorPosition, std::string& href)
{
  return mController->AnchorClickEvent(cursorPosition, href);
}

void LabelImpl::EmitAnchorClickedSignal(const std::string& href)
{
  Ui::View handle(GetOwner());
  mAnchorClickedSignal.Emit(handle, ToDaliString(href));
}

// =============================================================================
// AsyncTextInterface
// =============================================================================
void LabelImpl::AsyncInitializeMarquee(Text::AsyncTextRenderInfo renderInfo)
{
  // Check current state to prevent starting scroll when ENABLE_MARQUEE was set to false.
  if(!mController->IsMarqueeEnabled() && mController->GetEllipsisMode() == Text::Ellipsize::TRUNCATE)
  {
    if(!mIsAsyncRenderRequested)
    {
      DALI_LOG_ERROR("[%p] AsyncInitializeMarquee was called, but marquee was disabled and no next render was requested.\n", mController.Get());
    }
    // Marquee has been disabled since the async render was requested.
    // Do not start scrolling even though the render was completed with marquee enabled.
    // This issue occurs when ScrollingFinished and TextScroller::StartScrolling are called in the same loop.
    RequestAsyncRender();
    return;
  }

  Size      verifiedSize = renderInfo.size;
  Size      controlSize  = renderInfo.controlSize;
  float     wrapGap      = renderInfo.marqueeWrapGap;
  PixelData data         = renderInfo.marqueePixelData;
  Texture   texture      = Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());

#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    std::string text;
    mController->GetText(text);
    Dali::Integration::TextureUploadWithContent(texture, data, ToDaliString(std::move(text)), Dali::Integration::TextureContextTypeHint::TEXT_SCROLL);
  }
#else
  texture.Upload(data);
#endif

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  bool isHorizontal = mTextScroller->GetOrientation() == Text::MarqueeOrientation::HORIZONTAL;
  if(isHorizontal)
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT); // Wrap the texture in the x direction
  }
  else
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT); // Wrap the texture in the y direction
  }
  textureSet.SetSampler(0u, sampler);

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, renderInfo.isTextDirectionRTL, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment(), true);
}

void LabelImpl::AsyncTextFitChanged(float pointSize)
{
  DALI_LOG_RELEASE_INFO("[%p] point size:%f\n", mController.Get(), pointSize);
  if(mController->IsTextFitEnabled())
  {
    mController->SetTextFitPointSize(pointSize);
    EmitTextFitChanged();
  }
}

void LabelImpl::AsyncRenderFinished(Text::AsyncTextRenderInfo renderInfo)
{
  DALI_LOG_RELEASE_INFO("[%p] rendered size:%f, %f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);

  // To avoid flickering issues, enable/disable the background visual when async load is completed.
  SetViewBackgroundEnabled(!mController->IsTextCutout());

  mAsyncLineCount = renderInfo.lineCount;

  float width  = renderInfo.renderedSize.width;
  float height = renderInfo.renderedSize.height;

  // Padding is already included in renderedSize when cutout is enabled.
  if(!renderInfo.isCutoutEnabled)
  {
    Extents padding = GetPadding();
    width += static_cast<float>(padding.start + padding.end);
    height += static_cast<float>(padding.top + padding.bottom);
  }

  if(mIsManualRenderInProgress)
  {
    mIsManualRenderInProgress = false;
    mIsManualRenderFinished   = true;
  }

  EmitAsyncRenderFinished(width, height);
}

void LabelImpl::AsyncSizeComputed(Text::AsyncTextRenderInfo renderInfo)
{
  switch(renderInfo.requestType)
  {
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
      DALI_LOG_RELEASE_INFO("[%p] natural size:%f, %f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);
      mAsyncLineCount     = renderInfo.lineCount;
      Extents     padding = GetPadding();
      const float width   = renderInfo.renderedSize.width + static_cast<float>(padding.start + padding.end);
      const float height  = renderInfo.renderedSize.height + static_cast<float>(padding.top + padding.bottom);
      EmitAsyncNaturalSizeComputed(width, height);
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      DALI_LOG_RELEASE_INFO("[%p] height for width:%f, %f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);
      mAsyncLineCount     = renderInfo.lineCount;
      Extents     padding = GetPadding();
      const float width   = renderInfo.renderedSize.width + static_cast<float>(padding.start + padding.end);
      const float height  = renderInfo.renderedSize.height + static_cast<float>(padding.top + padding.bottom);
      EmitAsyncHeightForWidthComputed(width, height);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type received : %d\n", renderInfo.requestType);
      break;
    }
  }
}

// =============================================================================
// Implementation
// =============================================================================
void LabelImpl::RequestRendererUpdate()
{
  mRendererUpdateNeeded = true;
}

void LabelImpl::UpdateLineHeight()
{
  bool rendererUpdateNeeded = false;
  if(Equals(mLineHeight, Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    // clear explicit line height and use the natural line height.
    rendererUpdateNeeded |= mController->SetRelativeLineSize(-1.0f);
    rendererUpdateNeeded |= mController->SetDefaultLineSize(0.0f);
  }
  else if(mLineHeightMode == Text::LineHeightMode::RELATIVE)
  {
    rendererUpdateNeeded |= mController->SetDefaultLineSize(0.0f);
    rendererUpdateNeeded |= mController->SetRelativeLineSize(mLineHeight);
  }
  else // LineHeightMode::ABSOLUTE
  {
    rendererUpdateNeeded |= mController->SetRelativeLineSize(-1.0f);
    rendererUpdateNeeded |= mController->SetDefaultLineSize(mLineHeight);
  }

  if(rendererUpdateNeeded)
  {
    RequestTextRelayout();
    RequestAsyncRender();
    RequestRendererUpdate();
  }
}

void LabelImpl::OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type)
{
  mController->ChangedLayoutDirection();
}

void LabelImpl::OnLocaleChanged(std::string locale)
{
  mController->InvalidateFontData();
}

bool LabelImpl::OnInterceptTouched(Actor actor, const TouchEvent& touch)
{
  const PointState::Type state = touch.GetState(0);

  if(state == PointState::STARTED)
  {
    mIsTouchDown   = true;
    mTouchPosition = touch.GetScreenPosition(0);
    return false;
  }

  if(state == PointState::FINISHED)
  {
    if(mIsTouchDown && mHasAnchors)
    {
      const Vector2 screen = touch.GetScreenPosition(0);
      const float   deltaX = std::abs(mTouchPosition.x - screen.x);
      const float   deltaY = std::abs(mTouchPosition.y - screen.y);

      if(deltaX < 20.0f && deltaY < 20.0f)
      {
        Extents       padding    = GetPadding();
        const Vector2 localPoint = touch.GetLocalPosition(0);
        mController->AnchorEvent(localPoint.x - static_cast<float>(padding.start), localPoint.y - static_cast<float>(padding.top));
      }
    }
    mIsTouchDown = false;
  }
  return false;
}

void LabelImpl::UpdateAnchorTouchInterception()
{
  if(mController->HasAnchors())
  {
    mHasAnchors = true;
    Dali::DevelActor::InterceptTouchedSignal(Self()).Connect(this, &LabelImpl::OnInterceptTouched);
  }
  else
  {
    mHasAnchors = false;
    Dali::DevelActor::InterceptTouchedSignal(Self()).Disconnect(this, &LabelImpl::OnInterceptTouched);
  }
}

void LabelImpl::InitializeMarquee(const Size& contentSize, const Size& originSize)
{
  const Text::CharacterDirection direction = mController->GetMarqueeTextDirection();

  float wrapGap        = 0.0f;
  Size  verifiedSize   = Size::ZERO;
  bool  actualellipsis = mController->IsTextElideEnabled();

  bool       isHorizontal   = GetTextScroller()->GetOrientation() == Text::MarqueeOrientation::HORIZONTAL;
  const Size controlSize    = isHorizontal ? mController->GetView().GetControlSize() : contentSize;
  const int  maxTextureSize = Dali::GetMaxTextureSize();

  if(isHorizontal)
  {
    const Size textNaturalSize = mController->GetNaturalSize().GetVectorXY(); // As relayout of text may not be done at this point natural size is used to get size. Single line scrolling only.
    DALI_LOG_RELEASE_INFO("[%p] natural size:%f,%f, control size:%f,%f\n", mController.Get(), textNaturalSize.x, textNaturalSize.y, controlSize.x, controlSize.y);

    // Calculate the actual gap before scrolling wraps.
    int textPadding     = std::max(controlSize.x - textNaturalSize.x, 0.0f);
    wrapGap             = std::max(mTextScroller->GetGap(), textPadding);
    Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture

    // Create a texture of the text for scrolling
    verifiedSize = textureSize;

    //if the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis
    if(verifiedSize.width > maxTextureSize)
    {
      verifiedSize.width = maxTextureSize;
      if(textNaturalSize.width > maxTextureSize)
      {
        mController->SetTextElideEnabled(true);
        mController->SetMarqueeMaxTextureExceeded(true);
      }
      float gap = static_cast<float>(mTextScroller->GetGap());
      mController->CalculateLayoutSize(verifiedSize.width - gap, controlSize.height, true);
      wrapGap = std::max(maxTextureSize - textNaturalSize.width, gap);
    }
  }
  else // MarqueeOrientation::VERTICAL
  {
    const float textHeight = mController->GetHeightForWidth(controlSize.width);

    // Calculate the actual gap before scrolling wraps.
    int textPadding = std::max(controlSize.height - textHeight, 0.0f);
    wrapGap         = std::max(mTextScroller->GetGap(), textPadding);
    Vector2 textureSize(controlSize.width, textHeight + wrapGap); // Add the gap as a part of the texture

    // Create a texture of the text for scrolling
    verifiedSize = textureSize;

    // if the texture size height exceed maxTextureSize, modify the visual model size and enabled the ellipsis
    if(verifiedSize.height > maxTextureSize)
    {
      verifiedSize.height = maxTextureSize;
      if(textHeight > maxTextureSize)
      {
        mController->SetMarqueeEnabled(false, false, Text::MarqueeOrientation::VERTICAL);
        mController->SetTextElideEnabled(true);
      }

      mController->CalculateLayoutSize(controlSize.width, maxTextureSize, true);
      wrapGap = std::max(maxTextureSize - textHeight, 0.0f);
      if(!mController->IsMarqueeEnabled())
      {
        mController->SetMarqueeEnabled(true, false, Text::MarqueeOrientation::VERTICAL);
      }
    }
  }

  Text::TypesetterPtr typesetter = Text::Typesetter::New(mController->GetTextModel());
  PixelData           data       = typesetter->Render(verifiedSize, mController->GetTextDirection(), Text::Typesetter::RENDER_TEXT_AND_STYLES, isHorizontal, Pixel::RGBA8888, originSize);
  Texture             texture    = Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());

#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    std::string text;
    mController->GetText(text);
    Dali::Integration::TextureUploadWithContent(texture, data, ToDaliString(std::move(text)), Dali::Integration::TextureContextTypeHint::TEXT_SCROLL);
  }
#else
  texture.Upload(data);
#endif

  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, texture);

  // Filter mode needs to be set to linear to produce better quality while scaling.
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  if(isHorizontal)
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT); // Wrap the texture in the x direction
  }
  else
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT); // Wrap the texture in the y direction
  }
  textureSet.SetSampler(0u, sampler);

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, direction, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment(), mRendererUpdateNeeded);
  mController->SetTextElideEnabled(actualellipsis);
  mController->SetMarqueeMaxTextureExceeded(false);
}

void LabelImpl::UpdateMarqueeState()
{
  if(mController->IsMarqueeEnabled())
  {
    const Text::MarqueeStopMode stopMode = GetTextScroller()->GetStopMode();
    mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
    mTextScroller->StopScrolling();
    mTextScroller->SetStopMode(stopMode);
    mController->SetMarqueeEnabled(true, true, mTextScroller->GetOrientation());
  }
}

void LabelImpl::OnMarqueeVisibilityChanged(bool visible)
{
  if(mTextScroller)
  {
    if(visible)
    {
      // TODO: Handle MARQUEE ellipsis on visibility change (disable when hidden, restore when visible)
      // if(mLastEllipsisMode == Text::Ellipsize::MARQUEE)
      // {
      //   mController->SetEllipsisMode(mLastEllipsisMode);
      //   if(mTextScroller)
      //   {
      //     mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
      //     mTextScroller->StopScrolling();
      //   }
      // }
      // else
      {
        if(mController->IsMarqueeEnabled() || mLastMarqueeEnabled)
        {
          mController->SetMarqueeEnabled(true, true, GetTextScroller()->GetOrientation());
        }
      }
    }
    else
    {
      // TODO: Handle MARQUEE ellipsis on visibility change (disable when hidden, restore when visible)
      // if(mController->GetEllipsisMode() == Text::Ellipsize::MARQUEE)
      // {
      //   mLastEllipsisMode = Text::Ellipsize::MARQUEE;
      //   mController->SetEllipsisMode(Text::Ellipsize::TRUNCATE);
      //   if(mTextScroller)
      //   {
      //     mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
      //     mTextScroller->StopScrolling();
      //   }
      // }
      // else
      {
        if(mLastMarqueeEnabled && !mController->IsMarqueeEnabled())
        {
          mLastMarqueeEnabled = false;
        }
        if(mTextScroller->IsScrolling())
        {
          const Text::MarqueeStopMode stopMode = mTextScroller->GetStopMode();
          mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
          mTextScroller->StopScrolling();
          mTextScroller->SetStopMode(stopMode);
        }
      }
    }
  }
}

Text::TextScrollerPtr LabelImpl::GetTextScroller()
{
  if(!mTextScroller)
  {
    mTextScroller = Text::TextScroller::New(*this);
  }
  return mTextScroller;
}

void LabelImpl::SetMarqueeEnabled(bool enabled)
{
  if(mController->IsTextElideEnabled() && mController->GetEllipsisMode() == Text::Ellipsize::MARQUEE)
  {
    DALI_LOG_DEBUG_INFO("Tried to marquee while in ellipsize marquee mode, request ignored.\n");
  }
  else
  {
    mLastMarqueeEnabled = enabled;
    // If request to marquee is the same as current state then do nothing.
    if(enabled != mController->IsMarqueeEnabled())
    {
      // If request is disable (false) and marqueeing is enabled then need to stop it
      if(enabled == false)
      {
        if(mTextScroller)
        {
          mTextScroller->StopScrolling();
        }
      }
      // If request is enable (true) then start marquee as not already running
      else
      {
        mController->SetMarqueeEnabled(enabled, true, GetTextScroller()->GetOrientation());
      }
      RequestAsyncRender();
    }
  }
}

void LabelImpl::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  mIsVisible            = visible;
  mIsVisibleInitialized = true;
  if(visible)
  {
    if(mController->IsAsyncRendering())
    {
      RequestTextRelayout();
      RequestAsyncRender();
    }
  }
  else
  {
    mIsAsyncRenderLayoutDirty = false;
    mIsManualRenderInProgress = false;
    mIsManualRenderFinished   = false;
  }
  OnMarqueeVisibilityChanged(visible);
}

bool LabelImpl::IsVisible()
{
  if(!mIsVisibleInitialized)
  {
    mIsVisible            = DevelActor::IsEffectivelyVisible(Self());
    mIsVisibleInitialized = true;
  }
  return mIsVisible;
}

void LabelImpl::EvaluateAndApplyMarquee(const Size& contentSize, Text::MarqueeOrientation orientation)
{
  if(mController->IsTextElideEnabled() && mController->GetEllipsisMode() == Text::Ellipsize::MARQUEE)
  {
    if(IsVisible())
    {
      bool marqueeEnabled = false;

      if(orientation == Text::MarqueeOrientation::HORIZONTAL)
      {
        if(mController->IsMultiLineEnabled())
        {
          DALI_LOG_DEBUG_INFO("Attempted ellipsize marquee on a non SINGLE_LINE_BOX, request ignored\n");
          marqueeEnabled = false;
        }
        else
        {
          const Size naturalSize = mController->GetNaturalSize(false).GetVectorXY();
          marqueeEnabled         = contentSize.width < naturalSize.width;
        }
      }
      else
      {
        const float textHeight = mController->GetHeightForWidth(contentSize.width);
        marqueeEnabled         = contentSize.height < textHeight;
      }

      if(marqueeEnabled != mController->IsMarqueeEnabled())
      {
        mController->SetMarqueeEnabled(marqueeEnabled, false, orientation);
      }
    }
  }
}

void LabelImpl::PrepareMarqueeLayout(const Size& contentSize, Text::MarqueeOrientation orientation, Size& originSize)
{
  originSize = Size::ZERO;

  if(mController->IsMarqueeEnabled())
  {
    const bool isVerticalScroll = (orientation == Text::MarqueeOrientation::VERTICAL);

    const bool needLayoutSizeCalculation =
      isVerticalScroll && (mController->GetVerticalAlignment() != Text::Alignment::START);

    if(needLayoutSizeCalculation)
    {
      mController->SetMarqueeEnabled(false, false, Text::MarqueeOrientation::VERTICAL);
      originSize = mController->CalculateLayoutSize(contentSize.x, contentSize.y, true);
      mController->SetMarqueeEnabled(true, false, Text::MarqueeOrientation::VERTICAL);
    }
  }
}

void LabelImpl::OnVariationPropertyNotify(PropertyNotification& source)
{
  Actor self = Self();

  Property::Map map;
  mController->GetVariationsMap(map);

  for(const auto& [index, tag] : mVariationIndexMap)
  {
    if(self.DoesCustomPropertyExist(index))
    {
      float value = 0.f;
      self.GetCurrentProperty(index).Get(value);
      map[tag] = value;
    }
  }

  mController->SetVariationsMap(map);
}

bool LabelImpl::HandleVariationPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  Actor self = Self();

  auto iter = mVariationIndexMap.find(index);
  if(!self.DoesCustomPropertyExist(index) || iter == mVariationIndexMap.end())
  {
    return false;
  }

  float value = 0.0f;
  propertyValue.Get(value);

  Property::Map map;
  mController->GetVariationsMap(map);
  map[iter->second] = value;

  mController->SetVariationsMap(map);

  return true;
}

void LabelImpl::SetCutoutEnabledInternal(bool enabled)
{
  mController->SetTextCutout(enabled);
}

void LabelImpl::SetViewBackgroundEnabled(bool enabled)
{
  View view = Ui::View::DownCast(Self());
  // Avoid unnecessary updates when no background visual exists.
  if(!Internal::ViewDataImpl::Get(GetImpl(view)).GetVisual(Ui::View::Property::BACKGROUND))
  {
    return;
  }

  if(mIsViewBackgroundEnabled != enabled)
  {
    mIsViewBackgroundEnabled = enabled;
    Internal::ViewDataImpl::Get(GetImpl(view)).EnableVisual(Ui::View::Property::BACKGROUND, enabled);
  }
}

bool LabelImpl::GetViewBackgroundColor(Vector4& backgroundColor) const
{
  const Property::Value backgroundValue = Self().GetProperty(Ui::View::Property::BACKGROUND);

  if(backgroundValue.GetType() == Property::VECTOR4)
  {
    backgroundColor = backgroundValue.Get<Vector4>();
    return true;
  }

  if(backgroundValue.GetType() == Property::MAP)
  {
    const Property::Map& backgroundMap = backgroundValue.Get<Property::Map>();
    Property::Value*     mixColorValue = backgroundMap.Find(Ui::ColorVisual::Property::MIX_COLOR);
    if(mixColorValue)
    {
      backgroundColor = mixColorValue->Get<Vector4>();
      return true;
    }
  }

  return false;
}

void LabelImpl::OnBackgroundPropertyChanged()
{
  if(!mController->IsTextCutout())
  {
    return;
  }

  Vector4 backgroundColor = Vector4::ZERO;
  if(GetViewBackgroundColor(backgroundColor))
  {
    mController->SetBackgroundColorWithCutout(backgroundColor);
    mController->SetBackgroundWithCutoutEnabled(true);

    if(!mController->IsAsyncRendering())
    {
      SetViewBackgroundEnabled(false);
    }
  }
}

void LabelImpl::UpdateCutoutState(bool enabled)
{
  mController->SetBackgroundWithCutoutEnabled(enabled);

  if(enabled)
  {
    Vector4 backgroundColor = Vector4::ZERO;
    if(GetViewBackgroundColor(backgroundColor))
    {
      mController->SetBackgroundColorWithCutout(backgroundColor);
    }
  }

  if(!mController->IsAsyncRendering())
  {
    SetViewBackgroundEnabled(!enabled);
    Internal::TextVisual::SetRequireRender(mVisual, enabled);
  }
}

Text::AsyncTextParameters LabelImpl::GetAsyncTextParameters(const Text::Async::RequestType requestType, const Vector2& contentSize, const Extents& padding, const Dali::LayoutDirection::Type layoutDirection)
{
  // Logically, all properties of the label should be passed.

  std::string text;
  mController->GetRawText(text);

  Text::AsyncTextParameters parameters;
  parameters.requestType     = requestType;
  parameters.textWidth       = contentSize.width;
  parameters.textHeight      = contentSize.height;
  parameters.padding         = padding;
  parameters.layoutDirection = layoutDirection;
  parameters.text            = text;

  parameters.maxTextureSize             = Dali::GetMaxTextureSize();
  parameters.fontSize                   = mController->GetDefaultFontSize(Text::Controller::POINT_SIZE);
  parameters.textColor                  = mController->GetDefaultColor();
  parameters.fontFamily                 = mController->GetDefaultFontFamily();
  parameters.fontWeight                 = mController->GetDefaultFontWeight();
  parameters.fontWidth                  = mController->GetDefaultFontWidth();
  parameters.fontSlant                  = mController->GetDefaultFontSlant();
  parameters.isMultiLine                = mController->IsMultiLineEnabled();
  parameters.ellipsis                   = mController->IsTextElideEnabled();
  parameters.enableMarkup               = mController->IsMarkupProcessorEnabled();
  parameters.minLineSize                = mController->GetDefaultLineSize();
  parameters.relativeLineSize           = mController->GetRelativeLineSize();
  parameters.characterSpacing           = mController->GetCharacterSpacing();
  parameters.fontSizeScale              = mController->GetAdjustedFontSizeScale();
  parameters.horizontalAlignment        = mController->GetHorizontalAlignment();
  parameters.verticalAlignment          = mController->GetVerticalAlignment();
  parameters.verticalLineAlignment      = mController->GetVerticalLineAlignment();
  parameters.lineWrapMode               = mController->GetLineWrapMode();
  parameters.layoutDirectionPolicy      = mController->GetLayoutDirectionMode();
  parameters.ellipsisPosition           = mController->GetEllipsisPosition();
  parameters.isUnderlineEnabled         = mController->IsUnderlineEnabled();
  parameters.underlineType              = mController->GetUnderlineType();
  parameters.underlineColor             = mController->GetUnderlineColor();
  parameters.underlineHeight            = mController->GetUnderlineHeight();
  parameters.dashedUnderlineWidth       = mController->GetDashedUnderlineWidth();
  parameters.dashedUnderlineGap         = mController->GetDashedUnderlineGap();
  parameters.isStrikethroughEnabled     = mController->IsStrikethroughEnabled();
  parameters.strikethroughColor         = mController->GetStrikethroughColor();
  parameters.strikethroughHeight        = mController->GetStrikethroughHeight();
  parameters.isTextBackgroundEnabled    = mController->IsBackgroundEnabled();
  parameters.textBackgroundColor        = mController->GetBackgroundColor();
  parameters.shadowBlurRadius           = mController->GetShadowBlurRadius();
  parameters.shadowColor                = mController->GetShadowColor();
  parameters.shadowOffset               = mController->GetShadowOffset();
  parameters.outlineWidth               = mController->GetOutlineWidth();
  parameters.outlineColor               = mController->GetOutlineColor();
  parameters.outlineBlurRadius          = mController->GetOutlineBlurRadius();
  parameters.outlineOffset              = mController->GetOutlineOffset();
  parameters.isTextFitEnabled           = mController->IsTextFitEnabled();
  parameters.textFitMinSize             = mController->GetTextFitMinSize(Text::Controller::POINT_SIZE);
  parameters.textFitMaxSize             = mController->GetTextFitMaxSize(Text::Controller::POINT_SIZE);
  parameters.textFitStepSize            = mController->GetTextFitStepSize(Text::Controller::POINT_SIZE);
  parameters.isTextFitCandidatesEnabled = mController->IsTextFitCandidatesEnabled();
  parameters.textFitCandidates          = mController->GetTextFitCandidates();
  parameters.isMarqueeEnabled           = mController->IsMarqueeEnabled();
  parameters.ellipsisMode               = mController->GetEllipsisMode();
  if(parameters.isMarqueeEnabled || parameters.ellipsisMode == Text::Ellipsize::MARQUEE)
  {
    parameters.marqueeStopMode    = GetTextScroller()->GetStopMode();
    parameters.marqueeSpeed       = GetTextScroller()->GetSpeed();
    parameters.marqueeLoopCount   = GetTextScroller()->GetLoopCount();
    parameters.marqueeLoopDelay   = GetTextScroller()->GetLoopDelay();
    parameters.marqueeGap         = GetTextScroller()->GetGap();
    parameters.marqueeOrientation = GetTextScroller()->GetOrientation();
  }
  parameters.isCutoutEnabled               = mController->IsTextCutout();
  parameters.isBackgroundWithCutoutEnabled = mController->IsBackgroundWithCutoutEnabled();
  parameters.backgroundColorWithCutout     = mController->GetBackgroundColorWithCutout();
  Property::Map variationsMap;
  mController->GetVariationsMap(variationsMap);
  parameters.variationsMap     = variationsMap;
  parameters.renderScale       = mController->GetRenderScale();
  parameters.isEmbossEnabled   = mController->IsEmbossEnabled();
  parameters.embossDirection   = mController->GetEmbossDirection();
  parameters.embossStrength    = mController->GetEmbossStrength();
  parameters.embossLightColor  = mController->GetEmbossLightColor();
  parameters.embossShadowColor = mController->GetEmbossShadowColor();

  return parameters;
}

void LabelImpl::EmitTextFitChanged()
{
  // Intentionally not emitted for now.
  // Revisit when the public TextFitChanged API direction is finalized.
  // mController->GetTextFitFontSize(Text::Controller::PIXEL_SIZE);
}

void LabelImpl::EmitAsyncRenderFinished(float width, float height)
{
  Ui::View handle(GetOwner());
  mAsyncRenderFinishedSignal.Emit(handle, width, height);
}

void LabelImpl::EmitAsyncNaturalSizeComputed(float width, float height)
{
  Ui::View handle(GetOwner());
  mAsyncNaturalSizeComputedSignal.Emit(handle, width, height);
}

void LabelImpl::EmitAsyncHeightForWidthComputed(float width, float height)
{
  Ui::View handle(GetOwner());
  mAsyncHeightForWidthComputedSignal.Emit(handle, width, height);
}

// =============================================================================
// UiColorManager
// =============================================================================
void LabelImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    Self().SetProperty(Text::LabelPropertyIndex::TEXT_COLOR, color);
    mController->SetDefaultColor(color);
    RequestRendererUpdate();

    // Trigger constraint always.
    if(DALI_LIKELY(mVisual))
    {
      Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount, true);
    }
  }
}

void LabelImpl::SetAnchorColorInternal(const Vector4& color)
{
  if(mController->GetAnchorColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetAnchorColor(color);
    RequestRendererUpdate();
  }
}

void LabelImpl::SetAnchorClickedColorInternal(const Vector4& color)
{
  if(mController->GetAnchorClickedColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetAnchorClickedColor(color);
    RequestRendererUpdate();
  }
}

void LabelImpl::SetTextBackgroundColorInternal(const Vector4& color)
{
  if(mController->GetBackgroundColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetBackgroundColor(color);
    RequestRendererUpdate();
  }
}

void LabelImpl::SetUnderlineColorInternal(const Vector4& color)
{
  if(mController->GetUnderlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetUnderlineColor(color);
  }
}

void LabelImpl::SetShadowColorInternal(const Vector4& color)
{
  if(mController->GetShadowColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetShadowColor(color);
  }
}

void LabelImpl::SetOutlineColorInternal(const Vector4& color)
{
  if(mController->GetOutlineColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetOutlineColor(color);
  }
}

void LabelImpl::SetLineThroughColorInternal(const Vector4& color)
{
  if(mController->GetStrikethroughColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetStrikethroughColor(color);
  }
}

void LabelImpl::SetBevelLightColorInternal(const Vector4& color)
{
  if(mController->GetEmbossLightColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetEmbossLightColor(color);
  }
}

void LabelImpl::SetBevelShadowColorInternal(const Vector4& color)
{
  if(mController->GetEmbossShadowColor() != color)
  {
    DALI_LOG_RELEASE_INFO("[%p] %f,%f,%f,%f\n", mController.Get(), color.r, color.g, color.b, color.a);
    mController->SetEmbossShadowColor(color);
  }
}

// =============================================================================
// Properties
// =============================================================================
void LabelImpl::OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  switch(index)
  {
    case Dali::Actor::Property::SIZE:
    {
      const Vector2& size = propertyValue.Get<Vector2>();
      if(size != mSize)
      {
        mSize                     = size;
        mIsAsyncRenderLayoutDirty = true;
      }
      break;
    }
    case Dali::Actor::Property::SIZE_WIDTH:
    {
      const float width = propertyValue.Get<float>();
      if(width != mSize.width)
      {
        mSize.width               = width;
        mIsAsyncRenderLayoutDirty = true;
      }
      break;
    }
    case Dali::Actor::Property::SIZE_HEIGHT:
    {
      const float height = propertyValue.Get<float>();
      if(height != mSize.height)
      {
        mSize.height              = height;
        mIsAsyncRenderLayoutDirty = true;
      }
      break;
    }
    case Ui::View::Property::PADDING:
    {
      mIsAsyncRenderLayoutDirty = true;
      break;
    }
    case Ui::View::Property::BACKGROUND:
    {
      OnBackgroundPropertyChanged();
      break;
    }
    case Text::LabelPropertyIndex::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
        RequestRendererUpdate();

        // Trigger constraint always.
        if(DALI_LIKELY(mVisual))
        {
          Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount, true);
        }
      }
      break;
    }
    case Text::LabelPropertyIndex::CUTOUT_ENABLED:
    {
      UpdateCutoutState(propertyValue.Get<bool>());
      break;
    }
    default:
    {
      if(HandleVariationPropertySet(index, propertyValue))
      {
        // Handled as a font variation property.
        break;
      }

      ViewImpl::OnPropertySet(index, propertyValue); // up call to control for non-handled properties
      break;
    }
  }
}

void LabelImpl::SetProperty(BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    PropertyHandler::SetProperty(view, index, value);
  }
}

Dali::Property::Value LabelImpl::GetProperty(BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    value = PropertyHandler::GetProperty(view, index);
  }
  return value;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
