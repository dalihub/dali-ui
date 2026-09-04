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
#include <dali/devel-api/actors/actor-enumerations-devel.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/system/system-settings.h>
#include <dali/public-api/actors/actor.h>
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/label-property-handler.h>
#include <dali-ui-foundation/integration-api/view-accessibility.h>
#include <dali-ui-foundation/internal/controls/text-controls/label-accessible.h>
#include <dali-ui-foundation/internal/render-effects/mask-effect-impl.h>
#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/font-variation/font-variation-property-data.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-builder.h>
#include <dali-ui-foundation/internal/text/marquee/marquee-start-geometry.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal-data.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-source-data.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-marquee-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-property-data.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/text/text-pixel-snap-data.h>
#include <dali-ui-foundation/internal/text/text-style-helper.h>

#include <dali-ui-foundation/extension-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/text/text-font-style.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <dali-ui-foundation/public-api/text/font-variation/font-variation.h>
#include <dali-ui-foundation/public-api/types/align-enumerations.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view.h>
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

float ClampTextRevealProgress(float value)
{
  return std::isnan(value) ? 0.0f : std::max(0.0f, std::min(1.0f, value));
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_TEXT_CONTROLS");
#endif

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
DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::Label, Ui::Integration::LabelImpl, Ui::View, Create)

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
LABEL_PROPERTY_REGISTRATION("anchorColor",                VECTOR4, ANCHOR_COLOR                  )
LABEL_PROPERTY_REGISTRATION("anchorClickedColor",         VECTOR4, ANCHOR_CLICKED_COLOR          )
LABEL_PROPERTY_REGISTRATION("marqueeTriggerPolicy",       INTEGER, MARQUEE_TRIGGER_POLICY        )
LABEL_PROPERTY_REGISTRATION("marqueeSpeed",               INTEGER, MARQUEE_SPEED                 )
LABEL_PROPERTY_REGISTRATION("marqueeLoopCount",           INTEGER, MARQUEE_LOOP_COUNT            )
LABEL_PROPERTY_REGISTRATION("marqueeLoopDelay",           FLOAT,   MARQUEE_LOOP_DELAY            )
LABEL_PROPERTY_REGISTRATION("marqueeGap",                 INTEGER, MARQUEE_GAP                   )
LABEL_PROPERTY_REGISTRATION("marqueeOrientation",         INTEGER, MARQUEE_ORIENTATION           )
LABEL_PROPERTY_REGISTRATION("marqueeStopMode",            INTEGER, MARQUEE_STOP_MODE             )
LABEL_PROPERTY_REGISTRATION("fontWeight",                 INTEGER, FONT_WEIGHT                   )
LABEL_PROPERTY_REGISTRATION("fontWidth",                  INTEGER, FONT_WIDTH                    )
LABEL_PROPERTY_REGISTRATION("fontSlant",                  INTEGER, FONT_SLANT                    )
LABEL_PROPERTY_REGISTRATION("textBackgroundColor",        VECTOR4, TEXT_BACKGROUND_COLOR         )
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

DALI_TYPE_REGISTRATION_END()
// clang-format on

constexpr const char* LOCALIZATION_TEXT_BINDING_ID                     = "Ui.Label.Text";
constexpr const char* TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME         = "uTextGradientStartOffset";
constexpr const char* TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME = "uTextGradientOverlayStartOffset";
constexpr const char* TEXT_REVEAL_PROGRESS_PROPERTY_NAME               = "uTextRevealProgress";

const AttachmentId LABEL_MASK_DATA_ATTACHMENT_ID              = AttachmentId::Alloc();
const AttachmentId LABEL_TRANSLATABLE_TEXT_DATA_ATTACHMENT_ID = AttachmentId::Alloc();

template<typename T>
T& GetOrCreateLabelData(Ui::View owner, AttachmentId id)
{
  T* data = owner.GetAttachment<T>(id);
  if(!data)
  {
    owner.SetAttachment(id, Dali::MakeUnique<T>());
    data = owner.GetAttachment<T>(id);
  }

  DALI_ASSERT_ALWAYS(data && "Label attachment creation failed");
  return *data;
}

float GetDpi()
{
  static uint32_t horizontalDpi = 0u;
  static uint32_t verticalDpi   = 0u;

  if(DALI_UNLIKELY(horizontalDpi == 0u))
  {
    Dali::TextAbstraction::FontClient fontClient = Dali::TextAbstraction::FontClient::Get();
    fontClient.GetDpi(horizontalDpi, verticalDpi);
  }
  return static_cast<float>(horizontalDpi);
}

/**
 * @brief Lookup table that converts Ui::Text::Alignment values
 *        to a normalized vertical alignment factor.
 */
const float VERTICAL_ALIGNMENT_TABLE[static_cast<int>(Ui::Text::Alignment::END) + 1] = {
  0.0f, // Ui::Text::Alignment::START
  0.5f, // Ui::Text::Alignment::CENTER
  1.0f  // Ui::Text::Alignment::END
};

/**
 * @brief Discard the given visual into VisualFactory. The visual will be destroyed at next idle time.
 * @param[in,out] visual Visual to be discarded. It will be reset to an empty handle.
 */
void DiscardLabelVisual(Dali::Ui::Integration::Visual::Base& visual)
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable() && visual))
  {
    Dali::Ui::Integration::VisualFactory::Get().DiscardVisual(visual);
  }
  visual.Reset();
}

/**
 * @brief Applies scale only to fixed size values.
 * Special size values such as WRAP_CONTENT and MATCH_PARENT are kept unchanged.
 */
float ScaleIfFixedSize(float value, float scale)
{
  return value >= 0.0f ? value * scale : value;
}

/**
 * @brief Restricts a value by applying the maximum bound first, then the minimum bound.
 * This keeps the minimum bound dominant when minValue is greater than maxValue.
 */
float ClampWithMinPriority(float value, float minValue, float maxValue)
{
  return std::max(std::min(value, maxValue), minValue);
}

UiConfig::SystemFontSize ToUiConfigSystemFontSize(Dali::Integration::SystemSettings::FontSize fontSize)
{
  using AdaptorFontSize = Dali::Integration::SystemSettings::FontSize;

  switch(fontSize)
  {
    case AdaptorFontSize::SMALL:
    {
      return UiConfig::SystemFontSize::SMALL;
    }
    case AdaptorFontSize::NORMAL:
    {
      return UiConfig::SystemFontSize::NORMAL;
    }
    case AdaptorFontSize::LARGE:
    {
      return UiConfig::SystemFontSize::LARGE;
    }
    case AdaptorFontSize::EXTRA_LARGE:
    {
      return UiConfig::SystemFontSize::EXTRA_LARGE;
    }
    case AdaptorFontSize::GIANT:
    {
      return UiConfig::SystemFontSize::GIANT;
    }
  }

  return UiConfig::SystemFontSize::NORMAL;
}

} // namespace

LabelImplPtr LabelImpl::New()
{
  LabelImplPtr impl(new LabelImpl());

  return impl;
}

LabelImpl::LabelImpl()
: SizeNegotiatedViewImpl(),
  mSize(),
  mLastMeasureConstraints(-1.0f, -1.0f),
  mLastMeasureRequestedSize(-1.0f, -1.0f),
  mLineHeight(Ui::Text::LINE_HEIGHT_AUTO),
  mLineHeightMode(Ui::Text::LineHeightMode::RELATIVE),
  mOverflowMode(Ui::Text::OverflowMode::ELLIPSIS),
  mMarqueeTriggerPolicy(Ui::Text::MarqueeTriggerPolicy::MANUAL),
  mAsyncLineCount(0),
  mTextColorAnimatedCount(0),
  mRendererUpdateNeeded(false),
  mMeasureInvalidated(false),
  mIsAsyncRenderRequested(false),
  mIsContentLayoutDirty(false),
  mSuppressAutoMarquee(false),
  mLastMarqueeEnabled(false),
  mRestartMarquee(false),
  mHasLastMeasureMetrics(false),
  mHasStyledTextSource(false),
  mHasVariationProperties(false),
  mHasAnchors(false),
  mHasAsyncAnchorHitRegions(false),
  mAsyncAnchorGeometryDirty(false),
  mHasA11yAnchors(false),
  mIsVisible(false),
  mIsVisibleInitialized(false),
  mIsViewBackgroundEnabled(true),
  mIsManualRenderInProgress(false),
  mIsManualRenderFinished(false)
{
  Dali::Ui::Integration::ViewAccessibility::SetAccessibleObjectCreator(
    *this,
    [](Dali::Ui::View view) -> ViewAccessible*
  {
    return new LabelAccessible(view);
  });
}

LabelImpl::~LabelImpl()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(Internal::Text::InlineReplacementData* data = Internal::Text::GetInlineReplacementData(*this))
  {
    if(data->resourceReadyConnected)
    {
      viewData.ResourceReadySignal().Disconnect(this, &LabelImpl::OnInlineReplacementResourcesReady);
      data->resourceReadyConnected = false;
    }
    data->manager.PrepareOwnerDestruction();
    Internal::Text::RemoveInlineReplacementData(*this);
  }
  // This prevents access to the async text interface until the visual is actually destroyed.
  Internal::TextVisual::SetAsyncTextInterface(mVisual, nullptr);
  DiscardLabelVisual(mVisual);
}

// =============================================================================
// Properties
// =============================================================================
void LabelImpl::SetText(const Dali::String& text)
{
  InvalidateMarqueeStartGeometry();
  const bool hadInlineReplacements = HasInlineReplacementSource();
  ClearStyledTextSourceState();
  ClearAnchorInteractionState();
  mController->SetText(ToStdString(text));
  UpdateAnchorConnections();
  InvalidateTextMeasure();
  if(hadInlineReplacements)
  {
    ClearInlineReplacementData();
    // Removing a marquee blocker does not implicitly restart previous scrolling.
    SuppressAutoMarqueeEvaluation();
    mLastMarqueeEnabled = false;
  }
}

Dali::String LabelImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return ToDaliString(text);
}

void LabelImpl::SetStyledText(const Ui::Text::StyledText& styledText)
{
  InvalidateMarqueeStartGeometry();
  const bool hadInlineReplacements = HasInlineReplacementSource();

  if(styledText)
  {
    Internal::Text::SetStyledTextSource(mStyledTextSourceData, styledText);
    mHasStyledTextSource = true;
  }
  else
  {
    ClearStyledTextSourceState();
  }
  ClearAnchorInteractionState();

  mController->SetStyledText(styledText);
  UpdateAnchorConnections();
  InvalidateTextMeasure();
  if(hadInlineReplacements)
  {
    ClearInlineReplacementData();
  }
  if(HasInlineReplacementSource())
  {
    StopMarqueeImmediately();
    mController->SetMarqueeEnabled(false);
    mLastMarqueeEnabled = false;
    SuppressAutoMarqueeEvaluation();
  }
  else if(hadInlineReplacements)
  {
    // A source edit may remove the blocker, but it never restarts scrolling.
    SuppressAutoMarqueeEvaluation();
    mLastMarqueeEnabled = false;
  }
}

Ui::Text::StyledText LabelImpl::GetStyledText() const
{
  if(!mHasStyledTextSource)
  {
    return Ui::Text::StyledText();
  }

  return Internal::Text::GetStyledTextSource(mStyledTextSourceData);
}

void LabelImpl::ClearStyledTextSourceState()
{
  if(!mHasStyledTextSource)
  {
    return;
  }

  Internal::Text::ClearStyledTextSource(mStyledTextSourceData);
  mHasStyledTextSource = false;
}

bool LabelImpl::HasInlineReplacementSource() const
{
  return mController && mController->HasValidReplacementSource();
}

struct LabelImpl::InlineReplacementUpdateData
{
  const Ui::Text::ReplacementSourceSnapshot&    source;
  const Vector<Ui::Text::ReplacementPlacement>& placements;
  uint64_t                                      sourceRevision;
};

void LabelImpl::ClearInlineReplacementData()
{
  Ui::View owner = Ui::View::DownCast(Self());
  if(Internal::Text::InlineReplacementData* data = Internal::Text::GetInlineReplacementData(owner))
  {
    if(data->resourceReadyConnected)
    {
      owner.ResourceReadySignal().Disconnect(this, &LabelImpl::OnInlineReplacementResourcesReady);
      data->resourceReadyConnected = false;
    }
    Internal::Text::RemoveInlineReplacementData(owner);
  }
}

void LabelImpl::UpdateInlineReplacementData(const InlineReplacementUpdateData& updateData, const Vector2& ownerSize, const Insets& padding)
{
  Ui::View                               owner = Ui::View::DownCast(Self());
  Internal::Text::InlineReplacementData* data  = Internal::Text::GetInlineReplacementData(owner);
  if(!data)
  {
    bool hasVisibleImage = false;
    for(const Ui::Text::ReplacementPlacement& placement : updateData.placements)
    {
      if(placement.visible && !placement.elided && placement.sourceRunIndex < updateData.source.runs.Count())
      {
        const Ui::Text::ReplacementRunSnapshot& run = updateData.source.runs[placement.sourceRunIndex];
        hasVisibleImage                             = run.type == Ui::Text::ReplacementType::IMAGE && !run.image.source.empty();
        if(hasVisibleImage)
        {
          break;
        }
      }
    }

    if(!hasVisibleImage)
    {
      return;
    }
    data = &Internal::Text::GetOrCreateInlineReplacementData(owner);
  }
  if(!data->resourceReadyConnected)
  {
    owner.ResourceReadySignal().Connect(this, &LabelImpl::OnInlineReplacementResourcesReady);
    data->resourceReadyConnected = true;
  }
  const Internal::Text::TextRevealData* revealData           = Internal::Text::GetTextRevealData(mTextRevealData);
  const bool                            pixelRevealRequested = revealData && revealData->enabled &&
                                    revealData->unit == Ui::Text::Reveal::Unit::PIXEL &&
                                    !mController->IsTextCutout();
  data->manager.Update(data->host,
                       updateData.source,
                       updateData.placements,
                       Vector2(static_cast<float>(padding.start), static_cast<float>(padding.top)),
                       Vector2(std::max(0.0f,
                                        ownerSize.x - static_cast<float>(padding.start + padding.end)),
                               std::max(0.0f,
                                        ownerSize.y - static_cast<float>(padding.top + padding.bottom))),
                       ownerSize,
                       GetEffectiveScale(),
                       updateData.sourceRevision,
                       pixelRevealRequested);
}

void LabelImpl::OnInlineReplacementResourcesReady(Ui::View)
{
  if(Internal::Text::InlineReplacementData* data =
       Internal::Text::GetInlineReplacementData(Ui::View::DownCast(Self())))
  {
    data->manager.Refresh();
  }
}

void LabelImpl::SetFontFamily(const Dali::String& fontFamily)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetDefaultFontFamily(ToStdString(fontFamily));
}

Dali::String LabelImpl::GetFontFamily() const
{
  return ToDaliString(mController->GetDefaultFontFamily());
}

void LabelImpl::SetFontSize(float fontSize)
{
  if(!Equals(mController->GetDefaultFontSize(Ui::Text::Controller::PIXEL_SIZE), fontSize, Math::MACHINE_EPSILON_1000))
  {
    mController->SetDefaultFontSize(fontSize, Ui::Text::Controller::PIXEL_SIZE);
    InvalidateTextMeasure();
  }
}

float LabelImpl::GetFontSize() const
{
  return mController->GetDefaultFontSize(Ui::Text::Controller::PIXEL_SIZE);
}

void LabelImpl::SetMultiLine(bool multiLine)
{
  mController->SetMultiLineEnabled(multiLine);
  UpdateMarqueeState();
}

bool LabelImpl::IsMultiLine() const
{
  return mController->IsMultiLineEnabled();
}

void LabelImpl::SetMaximumLines(int maximumLines)
{
  mController->SetMaximumNumberOfLines(maximumLines);
}

int LabelImpl::GetMaximumLines() const
{
  return mController->GetMaximumNumberOfLines();
}

void LabelImpl::SetLineWrapMode(Ui::Text::LineWrapMode mode)
{
  mController->SetLineWrapMode(mode);
}

Ui::Text::LineWrapMode LabelImpl::GetLineWrapMode() const
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

void LabelImpl::SetTextGradient(const Gradient::Base& gradient)
{
  if(Ui::Text::Internal::Gradient::IsRenderable(gradient))
  {
    auto& data        = Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData);
    data.textGradient = gradient;
    SyncGradientAnimProperties();
    UpdateTextGradientStyle();
    RequestTextRelayout();
    RequestRendererUpdate();
    RequestAsyncRender();
  }
  else
  {
    auto*      data            = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
    const bool hadTextGradient = data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradient);
    if(data)
    {
      data->textGradient = Gradient::Base::None();
    }

    if(hadTextGradient)
    {
      SyncGradientAnimProperties();
      UpdateTextGradientStyle();
      RequestTextRelayout();
      RequestRendererUpdate();
      RequestAsyncRender();
    }
  }
}

Gradient::Base LabelImpl::GetTextGradient() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data ? data->textGradient : Gradient::Base::None();
}

void LabelImpl::SetTextGradientBoundsMode(Ui::Text::GradientBoundsMode mode)
{
  const auto* data        = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const auto  currentMode = data ? data->textGradientBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  if(currentMode == mode)
  {
    return;
  }

  Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData).textGradientBoundsMode = mode;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetTextGradientBoundsMode(mVisual, mode);
  }

  RequestTextRelayout();
  RequestRendererUpdate();
  RequestAsyncRender();
}

Ui::Text::GradientBoundsMode LabelImpl::GetTextGradientBoundsMode() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data ? data->textGradientBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
}

void LabelImpl::SetTextGradientOverlay(const Gradient::Base& gradient)
{
  if(Ui::Text::Internal::Gradient::IsRenderable(gradient))
  {
    auto& data               = Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData);
    data.textGradientOverlay = gradient;
    SyncGradientOverlayAnimProperties();
    UpdateTextGradientOverlayStyle();
    RequestTextRelayout();
    RequestRendererUpdate();
    RequestAsyncRender();
  }
  else
  {
    auto*      data                   = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
    const bool hadTextGradientOverlay = data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradientOverlay);
    if(data)
    {
      data->textGradientOverlay = Gradient::Base::None();
    }

    if(hadTextGradientOverlay)
    {
      SyncGradientOverlayAnimProperties();
      UpdateTextGradientOverlayStyle();
      RequestTextRelayout();
      RequestRendererUpdate();
      RequestAsyncRender();
    }
  }
}

Gradient::Base LabelImpl::GetTextGradientOverlay() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data ? data->textGradientOverlay : Gradient::Base::None();
}

void LabelImpl::SetTextGradientOverlayBoundsMode(Ui::Text::GradientBoundsMode mode)
{
  const auto* data        = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const auto  currentMode = data ? data->textGradientOverlayBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  if(currentMode == mode)
  {
    return;
  }

  Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData).textGradientOverlayBoundsMode = mode;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetTextGradientOverlayBoundsMode(mVisual, mode);
  }

  RequestTextRelayout();
  RequestRendererUpdate();
  RequestAsyncRender();
}

Ui::Text::GradientBoundsMode LabelImpl::GetTextGradientOverlayBoundsMode() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data ? data->textGradientOverlayBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
}

void LabelImpl::SetTextGradientOverlayMode(Ui::Text::GradientOverlayMode mode)
{
  const auto* data        = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const auto  currentMode = data ? data->textGradientOverlayMode : Ui::Text::GradientOverlayMode::SRC_OVER;
  if(currentMode == mode)
  {
    return;
  }

  Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData).textGradientOverlayMode = mode;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetTextGradientOverlayMode(mVisual, mode);
  }

  RequestTextRelayout();
  RequestRendererUpdate();
  RequestAsyncRender();
}

Ui::Text::GradientOverlayMode LabelImpl::GetTextGradientOverlayMode() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data ? data->textGradientOverlayMode : Ui::Text::GradientOverlayMode::SRC_OVER;
}

Dali::Property::Index LabelImpl::EnsureGradientAnimOffset()
{
  if(!IsGradientAnimSupported())
  {
    return Property::INVALID_INDEX;
  }

  Actor self = Self();
  if(!self)
  {
    return Property::INVALID_INDEX;
  }

  auto& data = Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData);
  if(data.gradientAnimOffsetIndex == Property::INVALID_INDEX)
  {
    const Ui::Text::Internal::Gradient::Style style = Ui::Text::Internal::Gradient::CreateStyle(data.textGradient);
    data.gradientAnimOffsetIndex =
      self.RegisterProperty(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME, style.startOffset);
    BindGradientAnimProperties();
  }

  return data.gradientAnimOffsetIndex;
}

Dali::Property::Index LabelImpl::EnsureGradientOverlayAnimOffset()
{
  if(!IsGradientOverlayAnimSupported())
  {
    return Property::INVALID_INDEX;
  }

  Actor self = Self();
  if(!self)
  {
    return Property::INVALID_INDEX;
  }

  auto& data = Internal::Text::GetOrCreateTextGradientPropertyData(mTextGradientPropertyData);
  if(data.gradientOverlayAnimOffsetIndex == Property::INVALID_INDEX)
  {
    const Ui::Text::Internal::Gradient::Style style = Ui::Text::Internal::Gradient::CreateStyle(data.textGradientOverlay);
    data.gradientOverlayAnimOffsetIndex =
      self.RegisterProperty(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME, style.startOffset);
    BindGradientOverlayAnimProperties();
  }

  return data.gradientOverlayAnimOffsetIndex;
}

void LabelImpl::SetTextReveal(const Ui::Text::Reveal& reveal)
{
  const bool                       enabled                      = reveal != Ui::Text::Reveal::None();
  const Ui::Text::Reveal::Unit     authoredUnit                 = enabled ? reveal.GetUnit() : Ui::Text::Reveal::Unit::CHARACTER;
  const Ui::Text::Reveal::Sequence authoredSequence             = enabled ? reveal.GetSequence() : Ui::Text::Reveal::Sequence::WHOLE_TEXT;
  const float                      authoredFadeDurationRatio    = enabled ? reveal.GetFadeDurationRatio()
                                                                          : Ui::Text::Reveal::AUTO_FADE_DURATION_RATIO;
  const float                      authoredSequenceStaggerRatio = enabled ? reveal.GetSequenceStaggerRatio() : 0.0f;
  auto*                            data                         = Internal::Text::GetTextRevealData(mTextRevealData);
  if((!data && !enabled) ||
     (data && data->enabled == enabled &&
      (!enabled || (data->unit == authoredUnit &&
                    data->sequence == authoredSequence &&
                    Dali::Equals(data->fadeDurationRatio, authoredFadeDurationRatio) &&
                    Dali::Equals(data->sequenceStaggerRatio, authoredSequenceStaggerRatio)))))
  {
    return;
  }

  data                       = &Internal::Text::GetOrCreateTextRevealData(mTextRevealData);
  data->enabled              = enabled;
  data->unit                 = authoredUnit;
  data->sequence             = authoredSequence;
  data->fadeDurationRatio    = authoredFadeDurationRatio;
  data->sequenceStaggerRatio = authoredSequenceStaggerRatio;
  ++data->revision;

  if(!enabled)
  {
    if(Internal::Text::InlineReplacementData* replacementData =
         Internal::Text::GetInlineReplacementData(Ui::View::DownCast(Self())))
    {
      // Reveal -> None restores ordinary resource-ready visibility immediately.
      // Enabled reconfiguration keeps the previous valid binding until the
      // newly built schedule replaces it atomically.
      replacementData->manager.ClearReveal();
    }
  }

  Ui::Text::Internal::Reveal::Unit     unit                 = Ui::Text::Internal::Reveal::Unit::DISABLED;
  Ui::Text::Internal::Reveal::Sequence sequence             = Ui::Text::Internal::Reveal::Sequence::WHOLE_TEXT;
  float                                fadeDurationRatio    = Ui::Text::Reveal::AUTO_FADE_DURATION_RATIO;
  float                                sequenceStaggerRatio = 0.0f;
  Property::Index                      progressIndex        = Property::INVALID_INDEX;
  if(data->enabled)
  {
    unit                 = Ui::Text::Internal::Reveal::ToInternalUnit(data->unit);
    sequence             = Ui::Text::Internal::Reveal::ToInternalSequence(data->sequence);
    fadeDurationRatio    = data->fadeDurationRatio;
    sequenceStaggerRatio = data->sequenceStaggerRatio;
    progressIndex        = EnsureTextRevealProgress();
  }

  if(mVisual)
  {
    Internal::TextVisual::ConfigureTextReveal(mVisual,
                                              unit,
                                              fadeDurationRatio,
                                              progressIndex,
                                              data->revision,
                                              sequence,
                                              sequenceStaggerRatio);
  }

  if(mController && mController->IsAsyncRendering())
  {
    RequestAsyncRender();
    RelayoutRequest();
  }
}

Ui::Text::Reveal LabelImpl::GetTextReveal() const
{
  const auto* data = Internal::Text::GetTextRevealData(mTextRevealData);
  if(!data || !data->enabled)
  {
    return Ui::Text::Reveal::None();
  }

  Ui::Text::Reveal reveal;
  reveal.SetUnit(data->unit);
  reveal.SetSequence(data->sequence);
  reveal.SetFadeDurationRatio(data->fadeDurationRatio);
  reveal.SetSequenceStaggerRatio(data->sequenceStaggerRatio);
  return reveal;
}

void LabelImpl::SetTextRevealProgress(float progress)
{
  progress                    = ClampTextRevealProgress(progress);
  auto& data                  = Internal::Text::GetOrCreateTextRevealData(mTextRevealData);
  data.progress               = progress;
  const Property::Index index = EnsureTextRevealProgress();
  Actor                 self  = Self();
  if(self && index != Property::INVALID_INDEX)
  {
    self.SetProperty(index, progress);
  }
}

float LabelImpl::GetTextRevealProgress() const
{
  const auto* data = Internal::Text::GetTextRevealData(mTextRevealData);
  if(!data)
  {
    return 0.0f;
  }

  Actor self = Self();
  if(self && data->progressPropertyIndex != Property::INVALID_INDEX)
  {
    // Match DALi's current-property convention while the Label participates in
    // the scene graph. Off-scene there is no rendered frame, so expose the
    // event-side value set by the caller.
    const bool  onScene = self.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE);
    const float value   = onScene ? self.GetCurrentProperty<float>(data->progressPropertyIndex)
                                  : self.GetProperty<float>(data->progressPropertyIndex);
    return ClampTextRevealProgress(value);
  }
  return data->progress;
}

Property::Index LabelImpl::EnsureTextRevealProgress()
{
  Actor self = Self();
  if(!self)
  {
    return Property::INVALID_INDEX;
  }
  auto& data = Internal::Text::GetOrCreateTextRevealData(mTextRevealData);
  if(data.progressPropertyIndex == Property::INVALID_INDEX)
  {
    data.progressPropertyIndex = self.GetPropertyIndex(TEXT_REVEAL_PROGRESS_PROPERTY_NAME);
    if(data.progressPropertyIndex == Property::INVALID_INDEX)
    {
      data.progressPropertyIndex = self.RegisterProperty(TEXT_REVEAL_PROGRESS_PROPERTY_NAME, data.progress);
    }
  }
  return data.progressPropertyIndex;
}

void LabelImpl::SetHorizontalTextAlignment(Ui::Text::Alignment alignment)
{
  mController->SetHorizontalAlignment(alignment);
}

Ui::Text::Alignment LabelImpl::GetHorizontalTextAlignment() const
{
  return mController->GetHorizontalAlignment();
}

void LabelImpl::SetVerticalTextAlignment(Ui::Text::Alignment alignment)
{
  mController->SetVerticalAlignment(alignment);
}

Ui::Text::Alignment LabelImpl::GetVerticalTextAlignment() const
{
  return mController->GetVerticalAlignment();
}

void LabelImpl::SetTextOverflowMode(Ui::Text::OverflowMode mode)
{
  if(mode != mOverflowMode)
  {
    mOverflowMode = mode;
    switch(mode)
    {
      case Ui::Text::OverflowMode::CLIP:
      {
        mController->SetTextElideEnabled(false);
        break;
      }
      case Ui::Text::OverflowMode::ELLIPSIS:
      {
        mController->SetTextElideEnabled(true);
        break;
      }
    }
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::OverflowMode LabelImpl::GetTextOverflowMode() const
{
  return mOverflowMode;
}

void LabelImpl::SetLineHeight(float lineHeight)
{
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

void LabelImpl::SetLineHeightMode(Ui::Text::LineHeightMode mode)
{
  if(mLineHeightMode != mode)
  {
    mLineHeightMode = mode;
    UpdateLineHeight();
    InvalidateTextMeasure();
  }
}

Ui::Text::LineHeightMode LabelImpl::GetLineHeightMode() const
{
  return mLineHeightMode;
}

void LabelImpl::SetLayoutDirectionMode(Ui::Text::LayoutDirectionMode mode)
{
  if(mController->GetLayoutDirectionMode() != mode)
  {
    mController->SetLayoutDirectionMode(mode);
  }
}

Ui::Text::LayoutDirectionMode LabelImpl::GetLayoutDirectionMode() const
{
  return mController->GetLayoutDirectionMode();
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

void LabelImpl::SetMarqueeTriggerPolicy(Ui::Text::MarqueeTriggerPolicy policy)
{
  if(policy != mMarqueeTriggerPolicy)
  {
    mMarqueeTriggerPolicy = policy;
    EnableAutoMarqueeEvaluation();
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::MarqueeTriggerPolicy LabelImpl::GetMarqueeTriggerPolicy() const
{
  return mMarqueeTriggerPolicy;
}

void LabelImpl::SetMarqueeSpeed(int speed)
{
  GetTextScroller()->SetSpeed(speed);
}

int LabelImpl::GetMarqueeSpeed() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetSpeed();
  }
  return UiConfig::GetCurrent().GetMarqueeSpeed();
}

void LabelImpl::SetMarqueeLoopCount(int loopCount)
{
  GetTextScroller()->SetLoopCount(loopCount);
}

int LabelImpl::GetMarqueeLoopCount() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetLoopCount();
  }
  return UiConfig::GetCurrent().GetMarqueeLoopCount();
}

void LabelImpl::SetMarqueeLoopDelay(float delay)
{
  GetTextScroller()->SetLoopDelay(delay);
}

float LabelImpl::GetMarqueeLoopDelay() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetLoopDelay();
  }
  return UiConfig::GetCurrent().GetMarqueeLoopDelay();
}

void LabelImpl::SetMarqueeGap(int gap)
{
  GetTextScroller()->SetGap(gap);
}

int LabelImpl::GetMarqueeGap() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetGap();
  }
  return static_cast<int>(UiConfig::GetCurrent().GetMarqueeGap());
}

void LabelImpl::SetMarqueeOrientation(Ui::Text::MarqueeOrientation orientation)
{
  GetTextScroller()->SetOrientation(orientation);
  UpdateMarqueeState();
}

Ui::Text::MarqueeOrientation LabelImpl::GetMarqueeOrientation() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetOrientation();
  }
  return UiConfig::GetCurrent().GetMarqueeOrientation();
}

void LabelImpl::SetMarqueeStopMode(Ui::Text::MarqueeStopMode mode)
{
  GetTextScroller()->SetStopMode(mode);
}

Ui::Text::MarqueeStopMode LabelImpl::GetMarqueeStopMode() const
{
  if(mTextScroller)
  {
    return mTextScroller->GetStopMode();
  }
  return UiConfig::GetCurrent().GetMarqueeStopMode();
}

void LabelImpl::SetFontWeight(Ui::Text::FontWeight weight)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetDefaultFontWeight(Ui::Text::ToTextAbstractionFontWeight(weight));
}

Ui::Text::FontWeight LabelImpl::GetFontWeight() const
{
  return Ui::Text::ToFontWeight(mController->GetDefaultFontWeight());
}

void LabelImpl::SetFontWidth(Ui::Text::FontWidth width)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetDefaultFontWidth(Ui::Text::ToTextAbstractionFontWidth(width));
}

Ui::Text::FontWidth LabelImpl::GetFontWidth() const
{
  return Ui::Text::ToFontWidth(mController->GetDefaultFontWidth());
}

void LabelImpl::SetFontSlant(Ui::Text::FontSlant slant)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetDefaultFontSlant(Ui::Text::ToTextAbstractionFontSlant(slant));
}

Ui::Text::FontSlant LabelImpl::GetFontSlant() const
{
  return Ui::Text::ToFontSlant(mController->GetDefaultFontSlant());
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
  UiColorManager::Get().ClearBinding(Self(), "TextBackgroundColor");
  if(mController->IsBackgroundEnabled())
  {
    mController->SetBackgroundEnabled(false);
    mController->SetBackgroundColor(Color::TRANSPARENT);
  }
}

void LabelImpl::SetTextUnderline(const Ui::Text::Underline& underline)
{
  if(underline == Ui::Text::Underline::None())
  {
    UiColorManager::Get().ClearBinding(Self(), "UnderlineColor");
    if(mController->IsUnderlineEnabled())
    {
      mController->SetUnderlineEnabled(false);
    }
    return;
  }

  const UiColor& color = underline.GetColor();

  SetColorBinding("UnderlineColor", color, this, &LabelImpl::SetUnderlineColorInternal);

  if(Ui::Text::ApplyUnderlineStyle(mController, underline))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::Underline LabelImpl::GetTextUnderline() const
{
  if(!mController->IsUnderlineEnabled())
  {
    return Ui::Text::Underline::None();
  }

  Ui::Text::Underline underline;

  UiColor color;
  if(UiColorManager::Get().GetBindingColor(Self(), "UnderlineColor", color))
  {
    underline.SetColor(color);
  }
  else
  {
    underline.SetColor(mController->GetUnderlineColor());
  }
  underline.SetThickness(mController->GetUnderlineHeight());
  underline.SetType(mController->GetUnderlineType());
  underline.SetDashLength(mController->GetDashedUnderlineWidth());
  underline.SetDashGap(mController->GetDashedUnderlineGap());

  return underline;
}

void LabelImpl::SetTextShadow(const Ui::Text::Shadow& shadow)
{
  if(shadow == Ui::Text::Shadow::None())
  {
    UiColorManager::Get().ClearBinding(Self(), "ShadowColor");
    if(mController->IsShadowEnabled())
    {
      mController->SetShadowEnabled(false);
    }
    if(Vector2::ZERO != mController->GetShadowOffset())
    {
      mController->SetShadowOffset(Vector2::ZERO);
    }
    return;
  }

  const UiColor& color = shadow.GetColor();

  SetColorBinding("ShadowColor", color, this, &LabelImpl::SetShadowColorInternal);

  if(Ui::Text::ApplyShadowStyle(mController, shadow))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::Shadow LabelImpl::GetTextShadow() const
{
  if(!mController->IsShadowEnabled())
  {
    return Ui::Text::Shadow::None();
  }

  Ui::Text::Shadow shadow;

  UiColor color;
  if(UiColorManager::Get().GetBindingColor(Self(), "ShadowColor", color))
  {
    shadow.SetColor(color);
  }
  else
  {
    shadow.SetColor(mController->GetShadowColor());
  }
  shadow.SetOffset(mController->GetShadowOffset());
  shadow.SetBlurRadius(mController->GetShadowBlurRadius());

  return shadow;
}

void LabelImpl::SetTextOutline(const Ui::Text::Outline& outline)
{
  if(outline == Ui::Text::Outline::None())
  {
    UiColorManager::Get().ClearBinding(Self(), "OutlineColor");
    if(mController->IsOutlineEnabled())
    {
      mController->SetOutlineEnabled(false);
    }
    if(0u != mController->GetOutlineWidth())
    {
      mController->SetOutlineWidth(0u);
    }
    return;
  }

  const UiColor& color = outline.GetColor();

  SetColorBinding("OutlineColor", color, this, &LabelImpl::SetOutlineColorInternal);

  if(Ui::Text::ApplyOutlineStyle(mController, outline))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::Outline LabelImpl::GetTextOutline() const
{
  if(!mController->IsOutlineEnabled())
  {
    return Ui::Text::Outline::None();
  }

  Ui::Text::Outline outline;

  UiColor color;
  if(UiColorManager::Get().GetBindingColor(Self(), "OutlineColor", color))
  {
    outline.SetColor(color);
  }
  else
  {
    outline.SetColor(mController->GetOutlineColor());
  }
  outline.SetOffset(mController->GetOutlineOffset());
  outline.SetWidth(static_cast<float>(mController->GetOutlineWidth()));
  outline.SetBlurRadius(mController->GetOutlineBlurRadius());

  return outline;
}

void LabelImpl::SetTextLineThrough(const Ui::Text::LineThrough& lineThrough)
{
  if(lineThrough == Ui::Text::LineThrough::None())
  {
    UiColorManager::Get().ClearBinding(Self(), "LineThroughColor");
    if(mController->IsStrikethroughEnabled())
    {
      mController->SetStrikethroughEnabled(false);
    }
    return;
  }

  const UiColor& color = lineThrough.GetColor();

  SetColorBinding("LineThroughColor", color, this, &LabelImpl::SetLineThroughColorInternal);

  if(Ui::Text::ApplyLineThroughStyle(mController, lineThrough))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::LineThrough LabelImpl::GetTextLineThrough() const
{
  if(!mController->IsStrikethroughEnabled())
  {
    return Ui::Text::LineThrough::None();
  }

  Ui::Text::LineThrough lineThrough;

  UiColor color;
  if(UiColorManager::Get().GetBindingColor(Self(), "LineThroughColor", color))
  {
    lineThrough.SetColor(color);
  }
  else
  {
    lineThrough.SetColor(mController->GetStrikethroughColor());
  }
  lineThrough.SetThickness(mController->GetStrikethroughHeight());

  return lineThrough;
}

void LabelImpl::SetTextBevel(const Ui::Text::Bevel& bevel)
{
  if(bevel == Ui::Text::Bevel::None())
  {
    UiColorManager::Get().ClearBinding(Self(), "BevelLightColor");
    UiColorManager::Get().ClearBinding(Self(), "BevelShadowColor");
    if(mController->IsEmbossEnabled())
    {
      mController->SetEmbossEnabled(false);
    }
    return;
  }

  const UiColor& lightColor  = bevel.GetLightColor();
  const UiColor& shadowColor = bevel.GetShadowColor();

  SetColorBinding("BevelLightColor", lightColor, this, &LabelImpl::SetBevelLightColorInternal);
  SetColorBinding("BevelShadowColor", shadowColor, this, &LabelImpl::SetBevelShadowColorInternal);

  if(Ui::Text::ApplyBevelStyle(mController, bevel))
  {
    RequestTextRelayout();
    RequestAsyncRender();
  }
}

Ui::Text::Bevel LabelImpl::GetTextBevel() const
{
  if(!mController->IsEmbossEnabled())
  {
    return Ui::Text::Bevel::None();
  }

  Ui::Text::Bevel bevel;

  UiColor lightColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "BevelLightColor", lightColor))
  {
    bevel.SetLightColor(lightColor);
  }
  else
  {
    bevel.SetLightColor(mController->GetEmbossLightColor());
  }

  UiColor shadowColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "BevelShadowColor", shadowColor))
  {
    bevel.SetShadowColor(shadowColor);
  }
  else
  {
    bevel.SetShadowColor(mController->GetEmbossShadowColor());
  }
  bevel.SetDirection(mController->GetEmbossDirection());
  bevel.SetIntensity(mController->GetEmbossStrength());

  return bevel;
}

void LabelImpl::SetTextFit(const Ui::Text::Fit& fit)
{
  switch(fit.GetType())
  {
    case Ui::Text::Fit::Type::NONE:
    {
      ClearTextFitInternal();
      break;
    }
    case Ui::Text::Fit::Type::RANGE:
    {
      SetTextFit(fit.GetRange());
      break;
    }
    case Ui::Text::Fit::Type::CANDIDATES:
    {
      SetTextFit(fit.GetCandidates());
      break;
    }
  }
}

void LabelImpl::SetTextFit(const Ui::Text::Fit::Range& range)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] TextFit range min:%f, max:%f, step:%f\n", mController.Get(), range.GetMinimumFontSize(), range.GetMaximumFontSize(), range.GetFontSizeStep());
  // If TextFitCandidates is enabled, this should be disabled.
  if(mController->IsTextFitCandidatesEnabled())
  {
    mController->SetTextFitCandidatesEnabled(false);
    mController->ClearTextFitCandidates();
  }
  mController->SetTextFitEnabled(true);
  mController->SetTextFitMinSize(range.GetMinimumFontSize(), Ui::Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitMaxSize(range.GetMaximumFontSize(), Ui::Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitStepSize(range.GetFontSizeStep(), Ui::Text::Controller::FontSizeType::PIXEL_SIZE);
  mController->SetTextFitChanged(true);
  InvalidateTextMeasure();
}

void LabelImpl::SetTextFit(const Dali::Vector<Ui::Text::Fit::Candidate>& candidates)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] TextFit candidates:%u\n", mController.Get(), candidates.Count());
  if(candidates.Empty())
  {
    ClearTextFitInternal();
    return;
  }

  // If TextFit is enabled, this should be disabled.
  if(mController->IsTextFitEnabled())
  {
    mController->SetTextFitEnabled(false);
  }
  mController->SetTextFitCandidatesEnabled(true);
  mController->SetTextFitCandidates(candidates);
  InvalidateTextMeasure();
}

Ui::Text::Fit LabelImpl::GetTextFit() const
{
  if(mController->IsTextFitEnabled())
  {
    return Ui::Text::Fit::FromRange(Ui::Text::Fit::Range(mController->GetTextFitMinSize(Ui::Text::Controller::FontSizeType::PIXEL_SIZE),
                                                         mController->GetTextFitMaxSize(Ui::Text::Controller::FontSizeType::PIXEL_SIZE),
                                                         mController->GetTextFitStepSize(Ui::Text::Controller::FontSizeType::PIXEL_SIZE)));
  }

  if(mController->IsTextFitCandidatesEnabled())
  {
    return Ui::Text::Fit::FromCandidates(mController->GetTextFitCandidates());
  }

  return Ui::Text::Fit::None();
}

void LabelImpl::ClearTextFitInternal()
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
  mController->SetFontSizeScale(scale);
}

float LabelImpl::GetFontSizeScale() const
{
  return mController->GetFontSizeScale();
}

void LabelImpl::SetMinimumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetMinimumFontSizeScale(scale);
}

float LabelImpl::GetMinimumFontSizeScale() const
{
  return mController->GetMinimumFontSizeScale();
}

void LabelImpl::SetMaximumFontSizeScale(float scale)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetMaximumFontSizeScale(scale);
}

float LabelImpl::GetMaximumFontSizeScale() const
{
  return mController->GetMaximumFontSizeScale();
}

void LabelImpl::SetSystemFontSizeScaleEnabled(bool enabled)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetSystemFontSizeScaleEnabled(enabled);
}

bool LabelImpl::IsSystemFontSizeScaleEnabled() const
{
  return mController->IsSystemFontSizeScaleEnabled();
}

void LabelImpl::SetFontVariation(const Dali::Vector<Ui::Text::FontVariation::Axis>& axes)
{
  // InvalidateMeasure() may be called if needed.
  mController->SetVariations(axes);
}

void LabelImpl::SetFontVariation(const Dali::String& settings)
{
  if(settings.Empty())
  {
    DALI_LOG_WARNING(
      "Empty font variation string is not allowed. "
      "Use SetFontVariation(Ui::Text::FontVariation::None()) instead.\n");
    return;
  }

  auto axes = Ui::Text::FontVariation::FromString(settings);
  if(axes.Empty())
  {
    DALI_LOG_WARNING("Failed to parse font variation string: %s\n", settings.CStr());
    return;
  }

  SetFontVariation(axes);
}

Dali::Vector<Ui::Text::FontVariation::Axis> LabelImpl::GetFontVariation() const
{
  return mController->GetVariations();
}

// Integration-only implementation for now until public API support is introduced.
Dali::Property::Index LabelImpl::RegisterFontVariationProperty(const Dali::String& tag)
{
  if(tag.Size() != 4u) // Variation axis tag must be 4 characters.
  {
    DALI_LOG_WARNING("Font variation registration failed. The tag length is not 4.\n");
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

  const Dali::Property::Index                index = self.RegisterProperty(tag.CStr(), value);
  Internal::Text::FontVariationPropertyData& data =
    Internal::Text::GetOrCreateFontVariationPropertyData(Ui::View::DownCast(Self()));
  const bool inserted     = data.Insert(index, tag);
  mHasVariationProperties = true;
  if(inserted)
  {
    PropertyNotification notification = self.AddPropertyNotification(index, StepCondition(1.0f));
    // TODO: Make step value customizable by user.
    notification.NotifySignal().Connect(this, &LabelImpl::OnVariationPropertyNotify);
    // TODO: Support UnregisterProperty() and remove the tag from FontVariationPropertyData.
  }
  return index;
}

void LabelImpl::SetTextCutoutEnabled(bool enabled)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Text cutout enabled:%d\n", mController.Get(), enabled);
  // Set through the property system so that dependent background and rendering
  // state can be updated consistently in OnPropertySet().
  Self().SetProperty(Ui::Text::LabelPropertyIndex::CUTOUT_ENABLED, enabled);
}

bool LabelImpl::IsTextCutoutEnabled() const
{
  return mController->IsTextCutout();
}

// Integration-only implementation for now until public API support is introduced.
void LabelImpl::SetLetterSpacing(float spacing)
{
  mController->SetCharacterSpacing(spacing);
}

float LabelImpl::GetLetterSpacing() const
{
  return mController->GetCharacterSpacing();
}

void LabelImpl::SetMaskEffect(View view)
{
  if(!view)
  {
    DALI_LOG_WARNING("SetMaskEffect called with invalid view\n");
    return;
  }

  ClearMaskEffect();

  View selfView = Ui::View::DownCast(Self());

  Self().Add(view);
  GetOrCreateLabelData<WeakHandle<Ui::View>>(selfView, LABEL_MASK_DATA_ATTACHMENT_ID) = view;

  MaskEffect maskEffect = MaskEffect::New(view);
  GetImplementation(maskEffect).SetReverseMaskDirection(true);
  selfView.SetRenderEffect(maskEffect);
}

void LabelImpl::ClearMaskEffect()
{
  View selfView = Ui::View::DownCast(Self());

  WeakHandle<Ui::View>* sourceView = selfView.GetAttachment<WeakHandle<Ui::View>>(LABEL_MASK_DATA_ATTACHMENT_ID);
  View                  view       = sourceView ? sourceView->GetHandle() : View();
  if(view)
  {
    Self().Remove(view);
  }

  selfView.RemoveAttachment(LABEL_MASK_DATA_ATTACHMENT_ID);
  selfView.ClearRenderEffect();
}

void LabelImpl::SetAsyncRendering(bool asyncRendering)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async rendering:%d\n", mController.Get(), asyncRendering);
  const bool changed = mController->IsAsyncRendering() != asyncRendering;
  mController->SetAsyncRendering(asyncRendering);
  auto* revealData = Internal::Text::GetTextRevealData(mTextRevealData);
  if(changed && revealData && revealData->enabled)
  {
    // A completion issued on the old execution mode must never publish after
    // an off/on transition. Keep the Label scene property itself untouched.
    ++revealData->revision;
    if(mVisual)
    {
      Internal::TextVisual::ConfigureTextReveal(mVisual,
                                                Ui::Text::Internal::Reveal::ToInternalUnit(revealData->unit),
                                                revealData->fadeDurationRatio,
                                                EnsureTextRevealProgress(),
                                                revealData->revision,
                                                Ui::Text::Internal::Reveal::ToInternalSequence(revealData->sequence),
                                                revealData->sequenceStaggerRatio);
    }
  }
  if(!asyncRendering)
  {
    ClearAnchorInteractionState();
    UpdateAnchorConnections();
  }
}

bool LabelImpl::IsAsyncRendering() const
{
  return mController->IsAsyncRendering();
}

void LabelImpl::SetRenderScale(float scale)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Render scale:%f\n", mController.Get(), scale);
  mController->SetRenderScale(scale);
}

float LabelImpl::GetRenderScale() const
{
  return mController->GetRenderScale();
}

void LabelImpl::SetTranslatableText(StringView resourceId)
{
  SetTranslatableText(resourceId, StringView());
}

void LabelImpl::SetTranslatableText(StringView resourceId, StringView domain)
{
  Ui::View selfView                                                                        = Ui::View::DownCast(Self());
  GetOrCreateLabelData<Dali::String>(selfView, LABEL_TRANSLATABLE_TEXT_DATA_ATTACHMENT_ID) = resourceId;
  auto manager                                                                             = UiLocalizationManager::Get();
  if(manager)
  {
    manager.SetBindingResource(Self(),
                               LOCALIZATION_TEXT_BINDING_ID,
                               resourceId,
                               domain,
                               LocalizedStringCallback::New(this, &LabelImpl::ApplyLocalizedText));
  }
}

Dali::String LabelImpl::GetTranslatableText() const
{
  Ui::View            selfView = Ui::View::DownCast(Self());
  const Dali::String* text     = selfView.GetAttachment<Dali::String>(LABEL_TRANSLATABLE_TEXT_DATA_ATTACHMENT_ID);
  return text ? *text : Dali::String();
}

void LabelImpl::ClearTranslatableText()
{
  Ui::View::DownCast(Self()).RemoveAttachment(LABEL_TRANSLATABLE_TEXT_DATA_ATTACHMENT_ID);
  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.ClearBinding(Self(), LOCALIZATION_TEXT_BINDING_ID);
  }
  // Current Text value is not changed.
}

// =============================================================================
// Read Only
// =============================================================================
int LabelImpl::GetLineCount()
{
  const float width = Self().GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  const float clamp = ClampWithMinPriority(width, GetMinimumWidth(), GetMaximumWidth());
  return GetLineCount(clamp);
}

int LabelImpl::GetLineCount(float width)
{
  Insets padding      = GetEffectiveTextPadding();
  float  contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Line count content width:%f, padding start:%f, end:%f\n", mController.Get(), contentWidth, padding.start, padding.end);
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
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Start marquee\n", mController.Get());
  SetMarqueeEnabled(true);
}

void LabelImpl::StopMarquee()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Stop marquee\n", mController.Get());
  SetMarqueeEnabled(false);
}

void LabelImpl::SetPixelSnapFactor(float factor)
{
  View                  owner = View::DownCast(Self());
  const Property::Index index = Internal::EnsureTextPixelSnapFactorProperty(owner);
  if(index != Property::INVALID_INDEX)
  {
    owner.SetProperty(index, factor);
  }
}

float LabelImpl::GetPixelSnapFactor() const
{
  View                  owner = View::DownCast(Self());
  const Property::Index index = Internal::GetTextPixelSnapFactorPropertyIndex(owner);
  return index != Property::INVALID_INDEX ? owner.GetProperty<float>(index) : 0.0f;
}

void LabelImpl::RequestAsyncNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request async natural size\n", mController.Get());
  Actor                             self            = Self();
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  Ui::Text::AsyncTextParameters     parameters =
    GetAsyncTextParameters(Text::Async::COMPUTE_NATURAL_SIZE, Size::ZERO, GetEffectiveTextPadding(), layoutDirection);
  Internal::TextVisual::RequestAsyncSizeComputationOwned(mVisual, std::move(parameters));
}

void LabelImpl::RequestAsyncHeightForWidth(float width)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request async height for width:%f\n", mController.Get(), width);
  Actor                             self            = Self();
  Insets                            padding         = GetEffectiveTextPadding();
  float                             contentWidth    = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);
  Ui::Text::AsyncTextParameters     parameters =
    GetAsyncTextParameters(Text::Async::COMPUTE_HEIGHT_FOR_WIDTH, Size(contentWidth, 0.0f), padding, layoutDirection);
  Internal::TextVisual::RequestAsyncSizeComputationOwned(mVisual, std::move(parameters));
}

// =============================================================================
// Integration-only
// =============================================================================
void LabelImpl::RequestAsyncRenderWithFixedSize(float width, float height)
{
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Fixed size render ignored because async rendering is disabled\n", mController.Get());
    return;
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request fixed size render:%f,%f\n", mController.Get(), width, height);

  Actor                             self            = Self();
  Insets                            padding         = GetEffectiveTextPadding();
  float                             contentWidth    = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeight   = std::max(height - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  Ui::Text::AsyncTextParameters parameters =
    GetAsyncTextParameters(Text::Async::RENDER_FIXED_SIZE, Size(contentWidth, contentHeight), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRendererOwned(mVisual, std::move(parameters));
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithFixedWidth(float width, float heightConstraint)
{
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Fixed width render ignored because async rendering is disabled\n", mController.Get());
    return;
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request fixed width render, width:%f, height constraint:%f\n", mController.Get(), width, heightConstraint);

  Actor                             self                    = Self();
  Insets                            padding                 = GetEffectiveTextPadding();
  float                             contentWidth            = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeightConstraint = std::max(heightConstraint - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection         = mController->GetLayoutDirection(self);

  Ui::Text::AsyncTextParameters parameters =
    GetAsyncTextParameters(Text::Async::RENDER_FIXED_WIDTH, Size(contentWidth, contentHeightConstraint), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRendererOwned(mVisual, std::move(parameters));
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithFixedHeight(float widthConstraint, float height)
{
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Fixed height render ignored because async rendering is disabled\n", mController.Get());
    return;
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request fixed height render, width constraint:%f, height:%f\n", mController.Get(), widthConstraint, height);

  Actor                             self                   = Self();
  Insets                            padding                = GetEffectiveTextPadding();
  float                             contentWidthConstraint = std::max(widthConstraint - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeight          = std::max(height - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection        = mController->GetLayoutDirection(self);

  Ui::Text::AsyncTextParameters parameters =
    GetAsyncTextParameters(Text::Async::RENDER_FIXED_HEIGHT, Size(contentWidthConstraint, contentHeight), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRendererOwned(mVisual, std::move(parameters));
  mRendererUpdateNeeded     = false;
  mIsAsyncRenderRequested   = false;
}

void LabelImpl::RequestAsyncRenderWithConstraints(float widthConstraint, float heightConstraint)
{
  if(!mController->IsAsyncRendering())
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Constrained render ignored because async rendering is disabled\n", mController.Get());
    return;
  }
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request constrained render:%f,%f\n", mController.Get(), widthConstraint, heightConstraint);

  Actor                             self                    = Self();
  Insets                            padding                 = GetEffectiveTextPadding();
  float                             contentWidthConstraint  = std::max(widthConstraint - static_cast<float>(padding.start + padding.end), 0.0f);
  float                             contentHeightConstraint = std::max(heightConstraint - static_cast<float>(padding.top + padding.bottom), 0.0f);
  const Dali::LayoutDirection::Type layoutDirection         = mController->GetLayoutDirection(self);

  Ui::Text::AsyncTextParameters parameters =
    GetAsyncTextParameters(Text::Async::RENDER_CONSTRAINT, Size(contentWidthConstraint, contentHeightConstraint), padding, layoutDirection);

  mIsManualRenderInProgress = Internal::TextVisual::UpdateAsyncRendererOwned(mVisual, std::move(parameters));
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
  // UiConfig may not be applied during preload phase
  if(!UiConfig::HasCurrent())
  {
    return;
  }

  const auto config = UiConfig::GetCurrent();
  SetFontSize(config.GetDefaultFontSize());
  SetTextColor(config.GetDefaultTextColor());
  SetAsyncRendering(config.IsLabelAsyncRendering());
  SetMinimumFontSizeScale(config.GetDefaultMinimumFontSizeScale());
  SetMaximumFontSizeScale(config.GetDefaultMaximumFontSizeScale());
  SetSystemFontSizeScaleEnabled(config.IsDefaultSystemFontSizeScaleEnabled());

  auto systemSettings = Dali::Integration::SystemSettings::Get();
  if(systemSettings)
  {
    ApplySystemFontSize(systemSettings.GetFontSize());
  }
}

// =============================================================================
// UiScale
// =============================================================================
bool LabelImpl::SetTextUiScale(float scale)
{
  return mController->SetUiScale(scale);
}

float LabelImpl::GetTextUiScale() const
{
  return mController->GetUiScale();
}

Insets LabelImpl::GetEffectiveTextPadding() const
{
  Insets      padding     = GetPadding();
  const float textUiScale = GetTextUiScale();
  padding.start *= textUiScale;
  padding.end *= textUiScale;
  padding.top *= textUiScale;
  padding.bottom *= textUiScale;
  return padding;
}

// =============================================================================
// System FontSize
// =============================================================================
void LabelImpl::ApplySystemFontSize(Dali::Integration::SystemSettings::FontSize fontSize)
{
  if(!UiConfig::HasCurrent())
  {
    return;
  }

  const auto  config = UiConfig::GetCurrent();
  const float scale  = config.GetScaleForSystemFontSize(ToUiConfigSystemFontSize(fontSize));

  mController->SetSystemFontSizeScale(scale);
}

void LabelImpl::OnSystemFontSizeChanged(Dali::Integration::SystemSettings::FontSize fontSize)
{
  InvalidateMarqueeStartGeometry();
  ApplySystemFontSize(fontSize);
}

// =============================================================================
// ViewImpl
// =============================================================================
void LabelImpl::OnInitialize()
{
  // Call base class initialization
  SizeNegotiatedViewImpl::OnInitialize();

  Actor self = Self();

  Dali::Property::Map propertyMap;
  propertyMap.Add(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::TEXT);

  mVisual   = Ui::Integration::VisualFactory::Get().CreateVisual(propertyMap);
  View view = Ui::View::DownCast(self);
  Internal::ViewDataImpl::Get(GetImpl(view)).RegisterVisual(Ui::Text::LabelPropertyIndex::TEXT, mVisual, Dali::Ui::Integration::DepthIndex::CONTENT);

  Internal::TextVisual::SetAsyncTextInterface(mVisual, this);
  Internal::TextVisual::SetAnimatableTextColorProperty(mVisual, Ui::Text::LabelPropertyIndex::TEXT_COLOR);
  Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);

  mController = Internal::TextVisual::GetController(mVisual);
  DALI_ASSERT_DEBUG(mController && "Invalid Text Controller")
  mController->SetControlInterface(this);
  mController->SetAnchorControlInterface(this);

  // Use height-for-width negotiation by default
  DevelActor::SetResizePolicy(self, ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  DevelActor::SetResizePolicy(self, ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

  // Enable the text ellipsis.
  mController->SetTextElideEnabled(true);

  Dali::DevelActor::OnSceneVisibilityChangedSignal(self).Connect(this, &LabelImpl::OnViewEffectiveVisibilityChanged);
  self.LayoutDirectionChangedSignal().Connect(this, &LabelImpl::OnLayoutDirectionChanged);

  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().LocaleChangedSignal().Connect(this, &LabelImpl::OnLocaleChanged);
  }

  auto systemSettings = Dali::Integration::SystemSettings::Get();
  if(systemSettings)
  {
    systemSettings.FontSizeChangedSignal().Connect(this, &LabelImpl::OnSystemFontSizeChanged);
  }

  Ui::Text::Layout::Engine& engine = mController->GetLayoutEngine();
  engine.SetCursorWidth(0u);

  mController->SetVerticalLineAlignment(Ui::Text::Alignment::CENTER);

  Ui::View::DownCast(self).SetAccessibilityRole(Accessibility::Role::LABEL);

  ApplyInitialConfig();
}

void LabelImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  mSize                           = size;
  const bool contentLayoutDirty   = mIsContentLayoutDirty;
  const bool manualRenderFinished = mIsManualRenderFinished;
  mIsContentLayoutDirty           = false;
  mIsManualRenderFinished         = false;

  if(contentLayoutDirty)
  {
    InvalidateMarqueeStartGeometry();
  }

  if(mTextScroller && mTextScroller->IsStopRequested())
  {
    // When marquee stop is requested in FINISH_LOOP mode, defer relayout until scrolling finishes.
    return;
  }

  Actor self = Self();

  Insets  padding = GetEffectiveTextPadding();
  float   width   = std::max(size.x - static_cast<float>(padding.start + padding.end), 0.0f);
  float   height  = std::max(size.y - static_cast<float>(padding.top + padding.bottom), 0.0f);
  Vector2 contentSize(width, height);
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Relayout size:%f,%f, content size:%f,%f\n", mController.Get(), size.x, size.y, contentSize.x, contentSize.y);

  // Support Right-To-Left
  Dali::LayoutDirection::Type layoutDirection = mController->GetLayoutDirection(self);

  // Support Right-To-Left of padding
  if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
  {
    std::swap(padding.start, padding.end);
  }

  if(mController->IsAsyncRendering())
  {
    UpdateA11yAnchors(contentLayoutDirty);

    if(mTextScroller && mTextScroller->IsScrolling() && !(mRendererUpdateNeeded || contentLayoutDirty))
    {
      // When marquee is playing, a text load request is made only if a text update is absolutely necessary.
      return;
    }

    if(mIsManualRenderInProgress || !(contentLayoutDirty || mIsAsyncRenderRequested))
    {
      // Do not request async render while a manual render is in progress,
      // or when there are no size or property updates.
      return;
    }

    if(manualRenderFinished && contentLayoutDirty && !mIsAsyncRenderRequested)
    {
      // Skip async render when only the size changed immediately after manual render completion.
      // This avoids redundant recomputation when users resize the label in the completion callback.
      // Note: This behavior may have limitations in some edge cases.
      return;
    }

    DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Request async render, size:%f,%f\n", mController.Get(), contentSize.width, contentSize.height);

    Ui::Text::AsyncTextParameters parameters = GetAsyncTextParameters(Text::Async::RENDER_FIXED_SIZE, contentSize, padding, layoutDirection);
    Internal::TextVisual::UpdateAsyncRendererOwned(mVisual, std::move(parameters));
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

  if(contentLayoutDirty && mTextScroller && mTextScroller->IsScrolling())
  {
    mRestartMarquee = true;
    StopMarqueeImmediately();
  }

  const bool restartMarquee = mRestartMarquee;
  mRestartMarquee           = false;
  if(restartMarquee)
  {
    EnableAutoMarqueeEvaluation();
    if(mMarqueeTriggerPolicy != Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW && mLastMarqueeEnabled)
    {
      mController->SetMarqueeEnabled(true, true, GetTextScroller()->GetOrientation());
    }
  }

  const Ui::Text::MarqueeOrientation marqueeOrientation = mTextScroller ? mTextScroller->GetOrientation() : Ui::Text::MarqueeOrientation::HORIZONTAL;
  EvaluateAndApplyMarquee(contentSize, marqueeOrientation);

  Size originSize = Size::ZERO;
  PrepareMarqueeLayout(contentSize, marqueeOrientation, originSize);

  const Ui::Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);
  if(mController->HasValidReplacementSource())
  {
    const Ui::Text::ReplacementRenderState& replacementResult = mController->GetReplacementRenderState();
    if(replacementResult.processingModel && replacementResult.projection.HasReplacements())
    {
      UpdateInlineReplacementData({mController->GetReplacementSourceSnapshot(), replacementResult.placements, replacementResult.sourceRevision}, size, padding);
    }
    else
    {
      ClearInlineReplacementData();
    }
  }
  const bool textModelUpdated =
    Ui::Text::Controller::NONE_UPDATED != (Ui::Text::Controller::MODEL_UPDATED & updateTextType);
  const bool syncAnchorUpdateNeeded = !mHasA11yAnchors || contentLayoutDirty || textModelUpdated || mRendererUpdateNeeded;

  if(textModelUpdated || mRendererUpdateNeeded)
  {
    // Update the visual
    Internal::TextVisual::EnableRendererUpdate(mVisual);

    // Calculate the size of the visual that can fit the text
    const Ui::Text::ModelInterface* const renderModel = mController->GetRenderTextModel();
    Size                                  layoutSize  = renderModel->GetLayoutSize();
    layoutSize.x                                      = contentSize.x;

    const Vector2& shadowOffset = renderModel->GetShadowOffset();
    if(renderModel->IsShadowEnabled() && shadowOffset.y > Math::MACHINE_EPSILON_1)
    {
      layoutSize.y += shadowOffset.y;
    }

    float outlineWidth =
      renderModel->IsOutlineEnabled() ? renderModel->GetOutlineWidth() : 0.0f;
    layoutSize.y += outlineWidth * 2.0f;
    layoutSize.y = std::min(layoutSize.y, contentSize.y);

    // Calculate the offset for vertical alignment only, as the layout engine will do the horizontal alignment.
    Vector2 alignmentOffset;
    alignmentOffset.x = 0.0f;
    alignmentOffset.y =
      (marqueeOrientation == Ui::Text::MarqueeOrientation::VERTICAL)
        ? 0.0f
        : (contentSize.y - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[static_cast<int>(mController->GetVerticalAlignment())];

    const int maxTextureSize = Dali::GetMaxTextureSize();
    if(layoutSize.width > maxTextureSize)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Layout width %.2f exceeds max texture size %d, clamped to %d\n", mController.Get(), layoutSize.width, maxTextureSize, maxTextureSize);
      layoutSize.width = static_cast<float>(maxTextureSize);
    }

    // This affects font rendering quality.
    // It need to be integerized.
    Vector2 visualTransformOffset;
    visualTransformOffset.x = roundf(padding.start + alignmentOffset.x);
    visualTransformOffset.y = roundf(padding.top + alignmentOffset.y);

    mController->SetLayoutAlignmentOffset(alignmentOffset);
    mController->SetLayoutOffsetWithPadding(visualTransformOffset);

    Vector2 visualTransformSize = (marqueeOrientation == Ui::Text::MarqueeOrientation::VERTICAL) ? contentSize : layoutSize;

    // Mark that we don't use viewEffectiveScale at transform's size & offset for this visual.
    // (Because visual transform size and polic already apply viewEffectiveScale).
    auto& visualImpl = GetImplementation(mVisual);
    visualImpl.SetTransformMapUsageForFittingMode(true);

    // Reset cutout offset before call SetTransformAndSize.
    // (cutout offset will be set at OnSetTransform)
    if(mController->IsTextCutout())
    {
      mController->SetOffsetWithCutout(Vector2::ZERO);
    }

    Dali::Property::Map visualTransform;
    visualTransform.Add(Ui::Visual::Transform::Property::SIZE, visualTransformSize)
      .Add(Ui::Visual::Transform::Property::SIZE_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::OFFSET, visualTransformOffset)
      .Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Ui::Visual::Transform::Policy::ABSOLUTE, Ui::Visual::Transform::Policy::ABSOLUTE))
      .Add(Ui::Visual::Transform::Property::ORIGIN, Ui::Align::TOP_BEGIN)
      .Add(Ui::Visual::Transform::Property::PIVOT, Ui::Align::TOP_BEGIN);
    visualImpl.SetTransformAndSize(visualTransform, size, GetEffectiveScale());

    if(mController->IsMarqueeEnabled())
    {
      InitializeMarquee(contentSize, originSize);
    }

    mRendererUpdateNeeded = false;
  }

  if((mController->HasAnchors() || mHasA11yAnchors) && syncAnchorUpdateNeeded)
  {
    UpdateA11yAnchors(false);
  }

  if(mController->IsTextFitChanged())
  {
    mController->SetTextFitChanged(false);
    EmitTextFitChanged();
  }
}

Vector3 LabelImpl::GetNaturalSize()
{
  Insets  padding     = GetEffectiveTextPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += static_cast<float>(padding.start + padding.end);
  naturalSize.height += static_cast<float>(padding.top + padding.bottom);

  return naturalSize;
}

float LabelImpl::GetHeightForWidth(float width)
{
  Insets padding      = GetEffectiveTextPadding();
  float  contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  return mController->GetHeightForWidth(contentWidth) + static_cast<float>(padding.top + padding.bottom);
}

MeasuredSize LabelImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  const bool measureInvalidated = mMeasureInvalidated;
  mMeasureInvalidated           = false;

  const float effectiveScale = GetEffectiveScale();
  if(SetTextUiScale(effectiveScale))
  {
    mController->InvalidateFontData();
    RequestRendererUpdate();
    RequestSyncMarqueeRestart();
  }

  const float requestedWidth     = ScaleIfFixedSize(GetRequestedWidth(), effectiveScale);
  const float requestedHeight    = ScaleIfFixedSize(GetRequestedHeight(), effectiveScale);
  const bool  wrapContentMeasure = (requestedWidth == WRAP_CONTENT) || (requestedHeight == WRAP_CONTENT);
  const bool  measureInputsChanged =
    mHasLastMeasureMetrics &&
    (!Equals(mLastMeasureConstraints.width, widthConstraint, Math::MACHINE_EPSILON_1000) ||
     !Equals(mLastMeasureConstraints.height, heightConstraint, Math::MACHINE_EPSILON_1000) ||
     !Equals(mLastMeasureRequestedSize.width, requestedWidth, Math::MACHINE_EPSILON_1000) ||
     !Equals(mLastMeasureRequestedSize.height, requestedHeight, Math::MACHINE_EPSILON_1000));
  if(measureInputsChanged || (measureInvalidated && wrapContentMeasure))
  {
    RequestSyncMarqueeRestart();
  }
  mLastMeasureConstraints   = Vector2(widthConstraint, heightConstraint);
  mLastMeasureRequestedSize = Vector2(requestedWidth, requestedHeight);
  mHasLastMeasureMetrics    = true;

  const float minWidth  = GetMinimumWidth() * effectiveScale;
  const float maxWidth  = GetMaximumWidth() * effectiveScale;
  const float minHeight = GetMinimumHeight() * effectiveScale;
  const float maxHeight = GetMaximumHeight() * effectiveScale;
  const float fontSize  = GetFontSize();

  const bool useTextFitRange  = mController->IsTextFitEnabled();
  const bool useFitCandidates = mController->IsTextFitCandidatesEnabled();

  // Measure wrap-content size using a representative maximum text fit configuration.
  if(useTextFitRange && wrapContentMeasure)
  {
    mController->SetTextFitEnabled(false);
    mController->SetDefaultFontSize(mController->GetTextFitMaxSize(Ui::Text::Controller::PIXEL_SIZE), Ui::Text::Controller::PIXEL_SIZE);
  }
  else if(useFitCandidates && wrapContentMeasure)
  {
    mController->SetTextFitCandidatesEnabled(false);

    const Ui::Text::Fit::Candidate* fitCandidate = mController->GetMaxFitCandidate();
    if(fitCandidate)
    {
      mController->SetDefaultFontSize(fitCandidate->GetFontSize(), Ui::Text::Controller::PIXEL_SIZE);
      mController->SetDefaultLineSize(fitCandidate->GetLineHeight());
    }
    else
    {
      DALI_LOG_ERROR("TextFit candidate mode is enabled but no candidates are available\n");
    }
  }

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(requestedWidth >= 0.0f)
  {
    measuredWidth = ClampWithMinPriority(requestedWidth, minWidth, maxWidth);
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
    measuredWidth                 = ClampWithMinPriority(width, minWidth, allowedMaxWidth);
  }

  // Height
  if(requestedHeight >= 0.0f)
  {
    measuredHeight = ClampWithMinPriority(requestedHeight, minHeight, maxHeight);
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
    measuredHeight             = ClampWithMinPriority(height, minHeight, allowedMaxHeight);
  }

  if(useTextFitRange && wrapContentMeasure)
  {
    mController->SetTextFitEnabled(true);
    mController->SetDefaultFontSize(fontSize, Ui::Text::Controller::PIXEL_SIZE);
    UpdateLineHeight();
  }
  else if(useFitCandidates && wrapContentMeasure)
  {
    mController->SetTextFitCandidatesEnabled(true);
    mController->SetDefaultFontSize(fontSize, Ui::Text::Controller::PIXEL_SIZE);
    UpdateLineHeight();
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Measure constraints:%f,%f, measured:%f,%f\n", mController.Get(), widthConstraint, heightConstraint, measuredWidth, measuredHeight);
  return MeasuredSize(measuredWidth, measuredHeight);
}

LayoutRect LabelImpl::OnArrange(const LayoutRect& bounds)
{
  return bounds;
}

void LabelImpl::OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index, Animation::State state)
{
  auto* gradientData = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  if(DALI_LIKELY(mVisual) && index == Ui::Text::LabelPropertyIndex::TEXT_COLOR)
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
  else if(gradientData && index != Property::INVALID_INDEX && index == gradientData->gradientAnimOffsetIndex)
  {
    if(state == Animation::State::PLAYING)
    {
      ++gradientData->gradientAnimCount;
    }
    else if(state == Animation::State::STOPPED)
    {
      if(gradientData->gradientAnimCount)
      {
        --gradientData->gradientAnimCount;
      }
    }

    SetGradientAnimApplyRate();
  }
  else if(gradientData && index != Property::INVALID_INDEX && index == gradientData->gradientOverlayAnimOffsetIndex)
  {
    if(state == Animation::State::PLAYING)
    {
      ++gradientData->gradientOverlayAnimCount;
    }
    else if(state == Animation::State::STOPPED)
    {
      if(gradientData->gradientOverlayAnimCount)
      {
        --gradientData->gradientOverlayAnimCount;
      }
    }

    SetGradientOverlayAnimApplyRate();
  }
  ViewImpl::OnAnimateAnimatableProperty(animation, index, state);
}

void LabelImpl::OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied)
{
  auto* gradientData = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  if(DALI_LIKELY(mVisual) && index == Ui::Text::LabelPropertyIndex::TEXT_COLOR)
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
  else if(gradientData && index != Property::INVALID_INDEX && index == gradientData->gradientAnimOffsetIndex)
  {
    if(applied)
    {
      ++gradientData->gradientAnimCount;
    }
    else
    {
      if(gradientData->gradientAnimCount)
      {
        --gradientData->gradientAnimCount;
      }
    }

    SetGradientAnimApplyRate();
  }
  else if(gradientData && index != Property::INVALID_INDEX && index == gradientData->gradientOverlayAnimOffsetIndex)
  {
    if(applied)
    {
      ++gradientData->gradientOverlayAnimCount;
    }
    else
    {
      if(gradientData->gradientOverlayAnimCount)
      {
        --gradientData->gradientOverlayAnimCount;
      }
    }

    SetGradientOverlayAnimApplyRate();
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
    // Invalidate measure only when the label size depends on text measurement.
    if((GetRequestedWidth() == WRAP_CONTENT) || (GetRequestedHeight() == WRAP_CONTENT))
    {
      // Through the internal primitive, not ViewImpl::InvalidateMeasure(): the
      // mMeasureInvalidated latch below is only released by OnMeasure, so this
      // framework-internal invalidation must never be routed through the public
      // entry point.
      Internal::ViewDataImpl::Get(*this).InvalidateMeasure();
      mMeasureInvalidated = true;
    }
    EnableAutoMarqueeEvaluation();
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
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Scrolling finished\n", mController.Get());
  InvalidateMarqueeStartGeometry();
  SuppressAutoMarqueeEvaluation();
  mController->SetMarqueeEnabled(false);
  RequestTextRelayout();
  RequestAsyncRender();
}

// =============================================================================
// AnchorControlInterface
// =============================================================================
bool LabelImpl::AnchorClicked(uint32_t cursorPosition, std::string& href)
{
  if(mHasAsyncAnchorHitRegions)
  {
    Internal::Text::AnchorHitResult asyncAnchor = Internal::Text::ActivateAnchor(Ui::View::DownCast(Self()), cursorPosition);
    if(asyncAnchor.hit)
    {
      if(asyncAnchor.newlyClicked && mController->IsAsyncRendering())
      {
        RequestTextRelayout();
        RequestAsyncRender();
      }
      href = asyncAnchor.href;
      return true;
    }
  }

  return mController->AnchorClickEvent(cursorPosition, href);
}

void LabelImpl::EmitAnchorClicked(const std::string& href)
{
  Ui::View handle(GetOwner());
  mAnchorClickedSignal.Emit(handle, ToDaliString(href));
}

// =============================================================================
// AsyncTextInterface
// =============================================================================
void LabelImpl::AsyncInitializeMarquee(const Ui::Text::AsyncTextRenderInfo& renderInfo)
{
  if(HasInlineReplacementSource())
  {
    StopMarqueeImmediately();
    mController->SetMarqueeEnabled(false);
    return;
  }
  // Prevent restarting marquee after StopMarquee().
  if(!mController->IsMarqueeEnabled() && mMarqueeTriggerPolicy == Ui::Text::MarqueeTriggerPolicy::MANUAL)
  {
    if(!mIsAsyncRenderRequested)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async marquee initialization skipped because marquee was disabled\n", mController.Get());
    }
    // Marquee has been disabled since the async render was requested.
    // Do not start scrolling even though the render was completed with marquee enabled.
    // This issue occurs when ScrollingFinished and TextScroller::StartScrolling are called in the same loop.
    RequestAsyncRender();
    return;
  }

  Size      verifiedSize            = renderInfo.size;
  Size      controlSize             = renderInfo.controlSize;
  Size      textScrollerControlSize = controlSize;
  float     wrapGap                 = renderInfo.marqueeWrapGap;
  PixelData data                    = renderInfo.marqueePixelData;

  bool    isHorizontal = mTextScroller->GetOrientation() == Ui::Text::MarqueeOrientation::HORIZONTAL;
  Sampler sampler      = Ui::Text::MarqueeBuilder::CreateTextScrollSampler(isHorizontal);

  Ui::Text::MarqueeBuilder::PreparedContent preparedContent =
    Ui::Text::MarqueeBuilder::CreateTextContent(data, sampler);

  const auto*           gradientData                   = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const Gradient::Base& textGradient                   = gradientData ? gradientData->textGradient : Gradient::Base::None();
  const Gradient::Base& textGradientOverlay            = gradientData ? gradientData->textGradientOverlay : Gradient::Base::None();
  const auto            textGradientBoundsMode         = gradientData ? gradientData->textGradientBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  const auto            textGradientOverlayBoundsMode  = gradientData ? gradientData->textGradientOverlayBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  const auto            textGradientOverlayMode        = gradientData ? gradientData->textGradientOverlayMode : Ui::Text::GradientOverlayMode::SRC_OVER;
  const Property::Index gradientAnimOffsetIndex        = gradientData ? gradientData->gradientAnimOffsetIndex : Property::INVALID_INDEX;
  const Property::Index gradientOverlayAnimOffsetIndex = gradientData ? gradientData->gradientOverlayAnimOffsetIndex : Property::INVALID_INDEX;
  const bool            gradientAnimApplyAlways        = gradientAnimOffsetIndex != Property::INVALID_INDEX;
  const bool            gradientOverlayApplyAlways     = gradientOverlayAnimOffsetIndex != Property::INVALID_INDEX;

  const Ui::Text::MarqueeBuilder::GradientState gradientState =
    Ui::Text::MarqueeBuilder::ResolveGradientState(textGradient,
                                                   textGradientOverlay,
                                                   verifiedSize);
  const bool hasGradientFeature      = gradientState.baseRenderable || gradientState.overlayRenderable;
  const bool needsMarqueeComposition = hasGradientFeature || renderInfo.isOverlayStyle;
  if(needsMarqueeComposition)
  {
    const bool cutoutEnabled = renderInfo.isCutoutEnabled ||
                               mController->IsTextCutout() ||
                               mController->GetRenderTextModel()->IsBackgroundWithCutoutEnabled();
    Ui::Text::MarqueeBuilder::FeatureInfo featureInfo;
    if(hasGradientFeature)
    {
      featureInfo.hasMultipleTextColors = renderInfo.hasMultipleTextColors;
      featureInfo.containsColorGlyph    = renderInfo.containsColorGlyph;
      featureInfo.styleTextureEnabled   = renderInfo.styleTextureEnabled;
    }
    featureInfo.isOverlayStyle = renderInfo.isOverlayStyle;
    featureInfo.cutoutEnabled  = cutoutEnabled;

    Ui::Text::MarqueeBuilder::AnimationState animationState;
    animationState.baseStartOffsetIndex    = gradientAnimOffsetIndex;
    animationState.baseApplyAlways         = gradientAnimApplyAlways;
    animationState.overlayStartOffsetIndex = gradientOverlayAnimOffsetIndex;
    animationState.overlayApplyAlways      = gradientOverlayApplyAlways;

    Ui::Text::MarqueeBuilder::CompositionRequest compositionRequest;
    compositionRequest.sampler        = sampler;
    compositionRequest.verifiedSize   = verifiedSize;
    compositionRequest.featureInfo    = featureInfo;
    compositionRequest.embossEnabled  = renderInfo.isEmbossEnabled;
    compositionRequest.gradientState  = gradientState;
    compositionRequest.animationState = animationState;
    compositionRequest.overlayMode    = textGradientOverlayMode;

    const Ui::Text::MarqueeBuilder::CompositionPlan compositionPlan =
      Ui::Text::MarqueeBuilder::GetCompositionPlan(compositionRequest);
    if(compositionPlan.HasWork())
    {
      const bool hasContentBoundGradient =
        (compositionPlan.needsBaseBounds && textGradientBoundsMode == Ui::Text::GradientBoundsMode::CONTENT_BOUND) ||
        (compositionPlan.needsOverlayBounds && textGradientOverlayBoundsMode == Ui::Text::GradientBoundsMode::CONTENT_BOUND);
      if(isHorizontal && hasContentBoundGradient)
      {
        const Vector2 visualCoordinateSize = Internal::TextVisual::GetGradientViewCoordinateSize(mVisual);
        if(visualCoordinateSize.width > Math::MACHINE_EPSILON_1000 &&
           visualCoordinateSize.height > Math::MACHINE_EPSILON_1000)
        {
          // Async controlSize can describe the allocated Label while the text visual is
          // fitted to the rendered marquee height. Keep scroller geometry on the visual;
          // gradient evaluation independently uses content texture coordinates.
          textScrollerControlSize = visualCoordinateSize;
        }
      }

      auto resolveMarqueeGradientBounds = [&](Ui::Text::GradientBoundsMode boundsMode,
                                              Vector2&                     coordinateSize,
                                              bool&                        useTextureCoordinates) -> Vector4
      {
        useTextureCoordinates = false;
        if(boundsMode == Ui::Text::GradientBoundsMode::VIEW_BOUND)
        {
          coordinateSize = Internal::TextVisual::GetGradientViewCoordinateSize(mVisual);
          return Internal::TextVisual::CalculateGradientViewBounds(mVisual, coordinateSize);
        }

        coordinateSize        = verifiedSize;
        useTextureCoordinates = true;
        return renderInfo.textLogicalBounds;
      };

      if(compositionPlan.needsBaseBounds)
      {
        Vector2       textGradientCoordinateSize;
        bool          textGradientUseTextureCoordinates     = false;
        const Vector4 textGradientBounds                    = resolveMarqueeGradientBounds(textGradientBoundsMode,
                                                                                           textGradientCoordinateSize,
                                                                                           textGradientUseTextureCoordinates);
        compositionRequest.baseBoundsResolved               = true;
        compositionRequest.baseBounds.bounds                = textGradientBounds;
        compositionRequest.baseBounds.coordinateSize        = textGradientCoordinateSize;
        compositionRequest.baseBounds.useTextureCoordinates = textGradientUseTextureCoordinates;
      }

      if(compositionPlan.needsOverlayBounds)
      {
        Vector2       textGradientOverlayCoordinateSize;
        bool          textGradientOverlayUseTextureCoordinates = false;
        const Vector4 textGradientOverlayBounds                = resolveMarqueeGradientBounds(textGradientOverlayBoundsMode,
                                                                                              textGradientOverlayCoordinateSize,
                                                                                              textGradientOverlayUseTextureCoordinates);
        compositionRequest.overlayBoundsResolved               = true;
        compositionRequest.overlayBounds.bounds                = textGradientOverlayBounds;
        compositionRequest.overlayBounds.coordinateSize        = textGradientOverlayCoordinateSize;
        compositionRequest.overlayBounds.useTextureCoordinates = textGradientOverlayUseTextureCoordinates;
      }

      Ui::Text::MarqueeBuilder::PixelDataBundle pixels;
      if(compositionPlan.needsFillPixelData)
      {
        pixels.fillPixelData = renderInfo.marqueeFillPixelData;
      }
      if(compositionPlan.needsStylePixelData)
      {
        pixels.stylePixelData = renderInfo.marqueeStylePixelData;
      }
      if(compositionPlan.needsPreservedMaskPixelData)
      {
        pixels.preservedPixelData = renderInfo.textGradientPreservedPixelData;
        pixels.maskPixelData      = renderInfo.textGradientMaskPixelData;
      }
      if(compositionPlan.needsOverlayStylePixelData)
      {
        pixels.overlayStylePixelData = renderInfo.marqueeOverlayStylePixelData;
      }

      Ui::Text::MarqueeBuilder::ApplyPreparedComposition(preparedContent, compositionRequest, pixels);
    }
  }

  Ui::Text::MarqueeInitialDelta marqueeInitialDelta;
  if(isHorizontal && renderInfo.isMarqueeContentOverflow)
  {
    const float horizontalAlignment = Ui::Text::ResolveHorizontalMarqueeAlignment(true,
                                                                                  renderInfo.isTextDirectionRTL,
                                                                                  mController->GetHorizontalAlignment());
    marqueeInitialDelta             = Ui::Text::ResolveMarqueeInitialDelta(renderInfo.marqueeStartAnchor,
                                                                           renderInfo.marqueeTextureAnchor,
                                                                           horizontalAlignment,
                                                                           verifiedSize.width,
                                                                           textScrollerControlSize.width,
                                                                           wrapGap);
  }
  else if(isHorizontal)
  {
    const float horizontalAlignment = Ui::Text::ResolveHorizontalMarqueeAlignment(false,
                                                                                  renderInfo.isTextDirectionRTL,
                                                                                  mController->GetHorizontalAlignment());
    marqueeInitialDelta             = Ui::Text::ResolveMarqueeFittingInitialDelta(mMarqueeFittingStartGeometry,
                                                                                  horizontalAlignment,
                                                                                  verifiedSize.width,
                                                                                  textScrollerControlSize.width,
                                                                                  wrapGap);
  }

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, preparedContent.textureSet, textScrollerControlSize, verifiedSize, wrapGap,
                               renderInfo.isMarqueeContentOverflow, renderInfo.isTextDirectionRTL,
                               mController->GetHorizontalAlignment(),
                               mController->GetVerticalAlignment(), true, preparedContent.textGradient,
                               marqueeInitialDelta);
}

void LabelImpl::AsyncTextFitChanged(float pointSize)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async text fit point size:%f\n", mController.Get(), pointSize);
  if(mController->IsTextFitEnabled() || mController->IsTextFitCandidatesEnabled())
  {
    if(mController->IsTextFitCandidatesEnabled())
    {
      mController->SetDefaultLineSize(mController->GetCurrentLineSize());
    }
    mController->SetTextFitPointSize(pointSize);
    EmitTextFitChanged();
  }
}

void LabelImpl::AsyncRenderFinished(Ui::Text::AsyncTextRenderInfo&& renderInfo)
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async render finished, size:%f,%f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);

  // To avoid flickering issues, enable/disable the background visual when async load is completed.
  SetViewBackgroundEnabled(!mController->IsTextCutout());

  mAsyncLineCount = renderInfo.lineCount;
  if(renderInfo.isMarqueeStartAnchorResolved)
  {
    mMarqueeStartAnchor = renderInfo.marqueeStartAnchor;
  }
  if(renderInfo.isMarqueeFittingStartGeometryResolved)
  {
    mMarqueeFittingStartGeometry = renderInfo.marqueeFittingStartGeometry;
  }
  Ui::View selfView = Ui::View::DownCast(Self());
  if(renderInfo.anchorHitRegions.empty())
  {
    ClearAnchorInteractionState();
  }
  else
  {
    mHasA11yAnchors           = false;
    mAsyncAnchorGeometryDirty = false;
    mHasAsyncAnchorHitRegions =
      Internal::Text::SetAnchorHitRegions(selfView, std::move(renderInfo.anchorHitRegions));
    if(mHasAsyncAnchorHitRegions)
    {
      UpdateA11yAnchors(false);
    }
  }
  UpdateAnchorConnections();

  // Image visuals are event-thread objects. Worker output is applied only when
  // both immutable source and render request generations are still current.
  if(mController->HasValidReplacementSource())
  {
    const Ui::Text::ReplacementSourceSnapshot& replacementSource = mController->GetReplacementSourceSnapshot();
    Internal::Text::InlineReplacementData*     data              = Internal::Text::GetInlineReplacementData(selfView);
    if(data && renderInfo.replacementSourceRevision == replacementSource.sourceRevision &&
       renderInfo.replacementLayoutGeneration == data->lastRenderGeneration)
    {
      Insets replacementPadding = GetEffectiveTextPadding();
      Actor  self               = Self();
      if(Dali::LayoutDirection::RIGHT_TO_LEFT == mController->GetLayoutDirection(self))
      {
        std::swap(replacementPadding.start, replacementPadding.end);
      }
      UpdateInlineReplacementData({replacementSource, renderInfo.replacementPlacements, renderInfo.replacementSourceRevision}, mSize, replacementPadding);
    }
  }

  float width  = renderInfo.renderedSize.width;
  float height = renderInfo.renderedSize.height;

  // Padding is already included in renderedSize when cutout is enabled.
  if(!renderInfo.isCutoutEnabled)
  {
    Insets padding = GetEffectiveTextPadding();
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

void LabelImpl::AsyncSizeComputed(const Ui::Text::AsyncTextRenderInfo& renderInfo)
{
  switch(renderInfo.requestType)
  {
    case Text::Async::COMPUTE_NATURAL_SIZE:
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async natural size:%f,%f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);
      mAsyncLineCount     = renderInfo.lineCount;
      Insets      padding = GetEffectiveTextPadding();
      const float width   = renderInfo.renderedSize.width + static_cast<float>(padding.start + padding.end);
      const float height  = renderInfo.renderedSize.height + static_cast<float>(padding.top + padding.bottom);
      EmitAsyncNaturalSizeComputed(width, height);
      break;
    }
    case Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "[%p] Async height for width:%f,%f, line count:%d\n", mController.Get(), renderInfo.renderedSize.width, renderInfo.renderedSize.height, renderInfo.lineCount);
      mAsyncLineCount     = renderInfo.lineCount;
      Insets      padding = GetEffectiveTextPadding();
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
  InvalidateMarqueeStartGeometry();
}

void LabelImpl::RequestSyncMarqueeRestart()
{
  const bool marqueeActive =
    mController->IsMarqueeEnabled() ||
    (mTextScroller && (mTextScroller->IsScrolling() || mTextScroller->IsStopRequested()));

  if(!mController->IsAsyncRendering() && mTextScroller && mLastMarqueeEnabled && marqueeActive)
  {
    mIsContentLayoutDirty = true;
    mRestartMarquee       = true;
    StopMarqueeImmediately();
  }
}

void LabelImpl::UpdateTextGradientStyle()
{
  if(DALI_LIKELY(mVisual))
  {
    const auto*                         data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
    Ui::Text::Internal::Gradient::Style style;
    if(data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradient))
    {
      style = Ui::Text::Internal::Gradient::CreateStyle(data->textGradient);
    }

    Internal::TextVisual::SetTextGradientStyle(mVisual, style);
  }
}

void LabelImpl::UpdateTextGradientOverlayStyle()
{
  if(DALI_LIKELY(mVisual))
  {
    const auto*                         data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
    Ui::Text::Internal::Gradient::Style style;
    if(data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradientOverlay))
    {
      style = Ui::Text::Internal::Gradient::CreateStyle(data->textGradientOverlay);
    }

    Internal::TextVisual::SetTextGradientOverlayStyle(mVisual, style);
  }
}

void LabelImpl::SyncGradientAnimProperties()
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  if(!data || data->gradientAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  if(!IsGradientAnimSupported())
  {
    return;
  }

  Actor self = Self();
  if(!self)
  {
    return;
  }

  const Ui::Text::Internal::Gradient::Style style = Ui::Text::Internal::Gradient::CreateStyle(data->textGradient);
  self.SetProperty(data->gradientAnimOffsetIndex, style.startOffset);
}

void LabelImpl::SyncGradientOverlayAnimProperties()
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  if(!data || data->gradientOverlayAnimOffsetIndex == Property::INVALID_INDEX)
  {
    return;
  }

  if(!IsGradientOverlayAnimSupported())
  {
    return;
  }

  Actor self = Self();
  if(!self)
  {
    return;
  }

  const Ui::Text::Internal::Gradient::Style style = Ui::Text::Internal::Gradient::CreateStyle(data->textGradientOverlay);
  self.SetProperty(data->gradientOverlayAnimOffsetIndex, style.startOffset);
}

bool LabelImpl::IsGradientAnimSupported() const
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradient);
}

bool LabelImpl::IsGradientOverlayAnimSupported() const
{
  // The hidden animation source property belongs to Label, not to the current
  // renderer/scroller. Marquee renderers bind it later when overlay composition
  // is available.
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  return data && Ui::Text::Internal::Gradient::IsRenderable(data->textGradientOverlay);
}

void LabelImpl::BindGradientAnimProperties()
{
  const auto*           data        = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const Property::Index sourceIndex = data ? data->gradientAnimOffsetIndex : Property::INVALID_INDEX;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetGradientAnimProperties(mVisual, sourceIndex);
    SetGradientAnimApplyRate(true);
  }

  if(mTextScroller)
  {
    // Full marquee setup binds the initial source index.
    // This updates an active scroller when the source is created later.
    mTextScroller->SetGradientAnimProperties(sourceIndex);
  }
}

void LabelImpl::BindGradientOverlayAnimProperties()
{
  const auto*           data        = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const Property::Index sourceIndex = data ? data->gradientOverlayAnimOffsetIndex : Property::INVALID_INDEX;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetGradientOverlayAnimProperties(mVisual, sourceIndex);
    SetGradientOverlayAnimApplyRate(true);
  }

  if(mTextScroller)
  {
    // Full marquee setup binds the initial overlay source index.
    // This updates an active scroller when the source is created later.
    mTextScroller->SetGradientOverlayAnimProperties(sourceIndex);
  }
}

void LabelImpl::SetGradientAnimApplyRate(bool notifyToConstraint)
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  // The animation source is an instance custom property. DALi only emits the
  // CustomActor animation lifecycle callback for type-registered animatable
  // property indices, so gradientAnimCount cannot make this transition for the
  // custom-property range. Once the source exists, keep its renderer constraint
  // live so both typed and generic animations continue past their first frame.
  const bool applyAlways = data && data->gradientAnimOffsetIndex != Property::INVALID_INDEX;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetGradientAnimApplyAlways(mVisual, applyAlways, notifyToConstraint);
  }

  if(mTextScroller)
  {
    mTextScroller->SetGradientApplyAlways(applyAlways, notifyToConstraint);
  }
}

void LabelImpl::SetGradientOverlayAnimApplyRate(bool notifyToConstraint)
{
  const auto* data = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  // See SetGradientAnimApplyRate(): this source uses the same instance custom
  // property mechanism and therefore needs a persistent renderer constraint.
  const bool applyAlways = data && data->gradientOverlayAnimOffsetIndex != Property::INVALID_INDEX;
  if(DALI_LIKELY(mVisual))
  {
    Internal::TextVisual::SetGradientOverlayAnimApplyAlways(mVisual, applyAlways, notifyToConstraint);
  }

  if(mTextScroller)
  {
    mTextScroller->SetGradientOverlayApplyAlways(applyAlways, notifyToConstraint);
  }
}

void LabelImpl::UpdateLineHeight()
{
  bool rendererUpdateNeeded = false;
  if(Equals(mLineHeight, Ui::Text::LINE_HEIGHT_AUTO, Math::MACHINE_EPSILON_1000))
  {
    // clear explicit line height and use the natural line height.
    rendererUpdateNeeded |= mController->SetRelativeLineSize(-1.0f);
    rendererUpdateNeeded |= mController->SetDefaultLineSize(0.0f);
  }
  else if(mLineHeightMode == Ui::Text::LineHeightMode::RELATIVE)
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
  InvalidateMarqueeStartGeometry();
  mController->ChangedLayoutDirection();
}

void LabelImpl::OnLocaleChanged(std::string locale)
{
  InvalidateMarqueeStartGeometry();
  mController->InvalidateFontData();
}

bool LabelImpl::OnInterceptTouched(Actor actor, TouchEvent touch)
{
  const PointState::Type state = touch.GetState(0);
  Ui::View               self  = Ui::View::DownCast(Self());

  if(state == PointState::STARTED)
  {
    Internal::Text::GetOrCreateAnchorInteractionData(self).StartTouch(touch.GetScreenPosition(0));
    return false;
  }

  if(state == PointState::FINISHED || state == PointState::INTERRUPTED)
  {
    Internal::Text::AnchorInteractionData* data = Internal::Text::GetAnchorInteractionData(self);
    if(state == PointState::FINISHED && data && data->IsTouchDown() && mHasAnchors)
    {
      const Vector2 screen        = touch.GetScreenPosition(0);
      const Vector2 touchPosition = data->GetTouchPosition();
      const float   deltaX        = std::abs(touchPosition.x - screen.x);
      const float   deltaY        = std::abs(touchPosition.y - screen.y);

      if(deltaX < 20.0f && deltaY < 20.0f)
      {
        float localX = 0.0f;
        float localY = 0.0f;
        if(Self().ScreenToLocal(localX, localY, screen.x, screen.y))
        {
          const Vector2 contentOffset = GetTextContentOffset();
          const Vector2 contentPoint(localX - contentOffset.x, localY - contentOffset.y);
          const bool    useAsyncAnchorPath =
            mController->IsAsyncRendering() && mHasAsyncAnchorHitRegions;
          const bool asyncAnchorGeometryUsable =
            useAsyncAnchorPath && !mAsyncAnchorGeometryDirty && !mIsContentLayoutDirty;

          if(useAsyncAnchorPath)
          {
            if(asyncAnchorGeometryUsable)
            {
              const Internal::Text::AnchorHitResult asyncAnchor =
                Internal::Text::HitTestAnchor(self, contentPoint);
              if(asyncAnchor.hit)
              {
                if(asyncAnchor.newlyClicked)
                {
                  // RequestAsyncRender() only marks pending work; relayout is what schedules the async texture refresh.
                  RequestTextRelayout();
                  RequestAsyncRender();
                }
                EmitAnchorClicked(asyncAnchor.href);
              }
            }
          }
          else
          {
            mController->AnchorEvent(contentPoint.x, contentPoint.y);
          }
        }
      }
    }
    if(data)
    {
      data->EndTouch();
    }
  }
  return false;
}

void LabelImpl::UpdateAnchorConnections()
{
  const bool hasAnchors = mController->HasAnchors() || mHasAsyncAnchorHitRegions;
  if(hasAnchors == mHasAnchors)
  {
    return;
  }

  mHasAnchors = hasAnchors;
  if(hasAnchors)
  {
    Self().InterceptTouchEventSignal().Connect(this, &LabelImpl::OnInterceptTouched);
    Dali::Integration::Accessibility::Bridge::EnabledSignal().Connect(this, &LabelImpl::OnAccessibilityStatusChanged);
    Dali::Integration::Accessibility::Bridge::DisabledSignal().Connect(this, &LabelImpl::OnAccessibilityStatusChanged);
  }
  else
  {
    ClearA11yAnchors();
    Self().InterceptTouchEventSignal().Disconnect(this, &LabelImpl::OnInterceptTouched);
    Dali::Integration::Accessibility::Bridge::EnabledSignal().Disconnect(this, &LabelImpl::OnAccessibilityStatusChanged);
    Dali::Integration::Accessibility::Bridge::DisabledSignal().Disconnect(this, &LabelImpl::OnAccessibilityStatusChanged);
  }
}

Vector2 LabelImpl::GetTextContentOffset() const
{
  Insets padding = GetEffectiveTextPadding();
  Actor  self    = Self();

  if(Dali::LayoutDirection::RIGHT_TO_LEFT == mController->GetLayoutDirection(self))
  {
    std::swap(padding.start, padding.end);
  }

  return Vector2(static_cast<float>(padding.start), static_cast<float>(padding.top));
}

void LabelImpl::ClearAnchorInteractionState()
{
  Internal::Text::ClearAnchorInteractionData(Ui::View::DownCast(Self()));
  mHasAsyncAnchorHitRegions = false;
  mHasA11yAnchors           = false;

  mAsyncAnchorGeometryDirty = false;
}

void LabelImpl::ClearA11yAnchors()
{
  if(!mHasA11yAnchors)
  {
    return;
  }

  Internal::Text::ClearA11yAnchors(Ui::View::DownCast(Self()));
  mHasA11yAnchors = false;
}

void LabelImpl::UpdateA11yAnchors(bool contentDirty)
{
  if(contentDirty && mController->IsAsyncRendering() && mHasAsyncAnchorHitRegions)
  {
    mAsyncAnchorGeometryDirty = true;
  }

  if(!Dali::Integration::Accessibility::IsUp())
  {
    if(!mHasA11yAnchors)
    {
      return;
    }

    ClearA11yAnchors();
    return;
  }

  if(mController->IsAsyncRendering())
  {
    if(!mHasAsyncAnchorHitRegions || contentDirty || mAsyncAnchorGeometryDirty)
    {
      ClearA11yAnchors();
      return;
    }

    if(!mHasA11yAnchors)
    {
      UpdateAsyncA11yAnchors();
    }
    return;
  }

  if(mController->HasAnchors())
  {
    UpdateSyncA11yAnchors();
  }
  else if(mHasA11yAnchors)
  {
    ClearA11yAnchors();
  }
}

void LabelImpl::UpdateSyncA11yAnchors()
{
  std::vector<Ui::TextAnchor> anchorActors;
  mController->GetAnchorActors(anchorActors);
  mHasA11yAnchors = Internal::Text::SetA11yAnchors(Ui::View::DownCast(Self()), std::move(anchorActors));
}

void LabelImpl::UpdateAsyncA11yAnchors()
{
  mHasA11yAnchors = Internal::Text::UpdateA11yAnchorsFromHitRegions(Ui::View::DownCast(Self()), GetTextContentOffset());
}

void LabelImpl::OnAccessibilityStatusChanged()
{
  UpdateA11yAnchors(false);
}

void LabelImpl::InitializeMarquee(const Size& contentSize, const Size& originSize)
{
  const Ui::Text::CharacterDirection direction = mController->GetMarqueeTextDirection();

  float wrapGap        = 0.0f;
  Size  verifiedSize   = Size::ZERO;
  bool  actualellipsis = mController->IsTextElideEnabled();

  bool       isHorizontal          = GetTextScroller()->GetOrientation() == Ui::Text::MarqueeOrientation::HORIZONTAL;
  bool       isTextContentOverflow = false;
  const Size controlSize           = isHorizontal ? mController->GetView().GetControlSize() : contentSize;
  const int  maxTextureSize        = Dali::GetMaxTextureSize();
  const int  scaledMarqueeGap      = static_cast<int>(mTextScroller->GetGap() * GetTextUiScale());

  if(isHorizontal)
  {
    // Use natural size because text relayout may not be complete at this point.
    const Size textNaturalSize = mController->GetNaturalSize().GetVectorXY();
    isTextContentOverflow      = textNaturalSize.width > controlSize.width;
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "[%p] Marquee natural size:%f,%f, control size:%f,%f\n", mController.Get(), textNaturalSize.x, textNaturalSize.y, controlSize.x, controlSize.y);

    // Calculate the actual gap before scrolling wraps.
    int textPadding     = static_cast<int>(std::max(controlSize.x - textNaturalSize.x, 0.0f));
    wrapGap             = static_cast<float>(std::max(scaledMarqueeGap, textPadding));
    Vector2 textureSize = textNaturalSize + Vector2(wrapGap, 0.0f); // Add the gap as a part of the texture

    // Create a texture of the text for scrolling
    verifiedSize = textureSize;

    //if the texture size width exceed maxTextureSize, modify the visual model size and enabled the ellipsis
    if(verifiedSize.width > maxTextureSize)
    {
      verifiedSize.width = static_cast<float>(maxTextureSize);
      if(textNaturalSize.width > maxTextureSize)
      {
        mController->SetTextElideEnabled(true);
        mController->SetMarqueeMaxTextureExceeded(true);
      }
      float gap = static_cast<float>(scaledMarqueeGap);
      mController->CalculateLayoutSize(verifiedSize.width - gap, controlSize.height, true);
      wrapGap = std::max(maxTextureSize - textNaturalSize.width, gap);
    }
  }
  else // MarqueeOrientation::VERTICAL
  {
    const float textHeight = mController->GetHeightForWidth(controlSize.width);

    // Calculate the actual gap before scrolling wraps.
    int textPadding = static_cast<int>(std::max(controlSize.height - textHeight, 0.0f));
    wrapGap         = static_cast<float>(std::max(scaledMarqueeGap, textPadding));
    Vector2 textureSize(controlSize.width, textHeight + wrapGap); // Add the gap as a part of the texture

    // Create a texture of the text for scrolling
    verifiedSize = textureSize;

    // if the texture size height exceed maxTextureSize, modify the visual model size and enabled the ellipsis
    if(verifiedSize.height > maxTextureSize)
    {
      verifiedSize.height = static_cast<float>(maxTextureSize);
      if(textHeight > maxTextureSize)
      {
        mController->SetMarqueeEnabled(false, false, Ui::Text::MarqueeOrientation::VERTICAL);
        mController->SetTextElideEnabled(true);
      }

      mController->CalculateLayoutSize(controlSize.width, static_cast<float>(maxTextureSize), true);
      wrapGap = std::max(maxTextureSize - textHeight, 0.0f);
      if(!mController->IsMarqueeEnabled())
      {
        mController->SetMarqueeEnabled(true, false, Ui::Text::MarqueeOrientation::VERTICAL);
      }
    }
  }

  const Ui::Text::Direction      textDirection = mController->GetTextDirection();
  Ui::Text::MarqueeTextureAnchor marqueeTextureAnchor;
  auto                           renderMarqueeText = [&](Ui::Text::Typesetter::RenderBehaviour renderBehaviour,
                               Pixel::Format                         pixelFormat,
                               bool                                  resolveTextureAnchor = false) -> PixelData
  {
    return Internal::TextVisual::RenderMarqueeText(mVisual,
                                                   verifiedSize,
                                                   textDirection,
                                                   renderBehaviour,
                                                   isHorizontal,
                                                   pixelFormat,
                                                   originSize,
                                                   mMarqueeStartAnchor,
                                                   resolveTextureAnchor ? &marqueeTextureAnchor : nullptr);
  };
  // Capture from the primary text texture before any supplemental style or
  // gradient renders. Supplemental renders do not update this anchor.
  PixelData data = renderMarqueeText(Ui::Text::Typesetter::RENDER_TEXT_AND_STYLES,
                                     Pixel::RGBA8888,
                                     true);

  Sampler sampler = Ui::Text::MarqueeBuilder::CreateTextScrollSampler(isHorizontal);

  Ui::Text::MarqueeBuilder::PreparedContent preparedContent =
    Ui::Text::MarqueeBuilder::CreateTextContent(data, sampler);

  const auto*           gradientData                   = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  const Gradient::Base& textGradient                   = gradientData ? gradientData->textGradient : Gradient::Base::None();
  const Gradient::Base& textGradientOverlay            = gradientData ? gradientData->textGradientOverlay : Gradient::Base::None();
  const auto            textGradientBoundsMode         = gradientData ? gradientData->textGradientBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  const auto            textGradientOverlayBoundsMode  = gradientData ? gradientData->textGradientOverlayBoundsMode : Ui::Text::GradientBoundsMode::CONTENT_BOUND;
  const auto            textGradientOverlayMode        = gradientData ? gradientData->textGradientOverlayMode : Ui::Text::GradientOverlayMode::SRC_OVER;
  const Property::Index gradientAnimOffsetIndex        = gradientData ? gradientData->gradientAnimOffsetIndex : Property::INVALID_INDEX;
  const Property::Index gradientOverlayAnimOffsetIndex = gradientData ? gradientData->gradientOverlayAnimOffsetIndex : Property::INVALID_INDEX;
  const bool            gradientAnimApplyAlways        = gradientAnimOffsetIndex != Property::INVALID_INDEX;
  const bool            gradientOverlayApplyAlways     = gradientOverlayAnimOffsetIndex != Property::INVALID_INDEX;

  const Ui::Text::ModelInterface* const         textModel = mController->GetRenderTextModel();
  const Ui::Text::MarqueeBuilder::GradientState gradientState =
    Ui::Text::MarqueeBuilder::ResolveGradientState(textGradient,
                                                   textGradientOverlay,
                                                   verifiedSize);
  const bool hasGradientFeature = gradientState.baseRenderable || gradientState.overlayRenderable;
  const bool needsMarqueeComposition =
    hasGradientFeature ||
    Ui::Text::MarqueeBuilder::HasOverlayStyle(*textModel);
  if(needsMarqueeComposition)
  {
    Ui::Text::MarqueeBuilder::FeatureInfo featureInfo;
    if(hasGradientFeature)
    {
      featureInfo =
        Ui::Text::MarqueeBuilder::CollectGradientFeatureInfo(*textModel,
                                                             mController->IsTextCutout());
    }
    else
    {
      featureInfo.isOverlayStyle = true;
    }

    Ui::Text::MarqueeBuilder::AnimationState animationState;
    animationState.baseStartOffsetIndex    = gradientAnimOffsetIndex;
    animationState.baseApplyAlways         = gradientAnimApplyAlways;
    animationState.overlayStartOffsetIndex = gradientOverlayAnimOffsetIndex;
    animationState.overlayApplyAlways      = gradientOverlayApplyAlways;

    Ui::Text::MarqueeBuilder::CompositionRequest compositionRequest;
    compositionRequest.sampler        = sampler;
    compositionRequest.verifiedSize   = verifiedSize;
    compositionRequest.featureInfo    = featureInfo;
    compositionRequest.embossEnabled  = mController->IsEmbossEnabled();
    compositionRequest.gradientState  = gradientState;
    compositionRequest.animationState = animationState;
    compositionRequest.overlayMode    = textGradientOverlayMode;

    const Ui::Text::MarqueeBuilder::CompositionPlan compositionPlan =
      Ui::Text::MarqueeBuilder::GetCompositionPlan(compositionRequest);
    if(compositionPlan.HasWork())
    {
      if(compositionPlan.needsBaseBounds || compositionPlan.needsOverlayBounds)
      {
        const Vector4 textGradientContentBounds =
          Ui::Text::Internal::CalculateGradientContentBounds(verifiedSize,
                                                             textModel->GetLayoutSize(),
                                                             textModel->GetLines(),
                                                             textModel->GetNumberOfLines(),
                                                             mController->GetVerticalAlignment(),
                                                             isHorizontal);
        auto resolveMarqueeGradientBounds = [&](Ui::Text::GradientBoundsMode boundsMode,
                                                Vector2&                     coordinateSize,
                                                bool&                        useTextureCoordinates) -> Vector4
        {
          useTextureCoordinates = false;
          if(boundsMode == Ui::Text::GradientBoundsMode::VIEW_BOUND)
          {
            coordinateSize = Internal::TextVisual::GetGradientViewCoordinateSize(mVisual);
            return Internal::TextVisual::CalculateGradientViewBounds(mVisual, coordinateSize);
          }

          coordinateSize        = verifiedSize;
          useTextureCoordinates = true;
          return textGradientContentBounds;
        };

        if(compositionPlan.needsBaseBounds)
        {
          Vector2       textGradientCoordinateSize;
          bool          textGradientUseTextureCoordinates     = false;
          const Vector4 textGradientBounds                    = resolveMarqueeGradientBounds(textGradientBoundsMode,
                                                                                             textGradientCoordinateSize,
                                                                                             textGradientUseTextureCoordinates);
          compositionRequest.baseBoundsResolved               = true;
          compositionRequest.baseBounds.bounds                = textGradientBounds;
          compositionRequest.baseBounds.coordinateSize        = textGradientCoordinateSize;
          compositionRequest.baseBounds.useTextureCoordinates = textGradientUseTextureCoordinates;
        }

        if(compositionPlan.needsOverlayBounds)
        {
          Vector2       textGradientOverlayCoordinateSize;
          bool          textGradientOverlayUseTextureCoordinates = false;
          const Vector4 textGradientOverlayBounds                = resolveMarqueeGradientBounds(textGradientOverlayBoundsMode,
                                                                                                textGradientOverlayCoordinateSize,
                                                                                                textGradientOverlayUseTextureCoordinates);
          compositionRequest.overlayBoundsResolved               = true;
          compositionRequest.overlayBounds.bounds                = textGradientOverlayBounds;
          compositionRequest.overlayBounds.coordinateSize        = textGradientOverlayCoordinateSize;
          compositionRequest.overlayBounds.useTextureCoordinates = textGradientOverlayUseTextureCoordinates;
        }
      }

      Ui::Text::MarqueeBuilder::PixelDataBundle pixels;
      if(compositionPlan.needsFillPixelData)
      {
        pixels.fillPixelData = renderMarqueeText(Ui::Text::Typesetter::RENDER_NO_STYLES, Pixel::RGBA8888);
      }
      if(compositionPlan.needsStylePixelData)
      {
        pixels.stylePixelData = renderMarqueeText(Ui::Text::Typesetter::RENDER_NO_TEXT, Pixel::RGBA8888);
      }
      if(compositionPlan.needsPreservedMaskPixelData)
      {
        pixels.preservedPixelData =
          Internal::TextVisual::RenderMarqueeTextGradientPreserved(mVisual,
                                                                   verifiedSize,
                                                                   textDirection,
                                                                   isHorizontal,
                                                                   Pixel::RGBA8888,
                                                                   originSize);
        pixels.maskPixelData =
          Internal::TextVisual::RenderMarqueeTextGradientMask(mVisual,
                                                              verifiedSize,
                                                              textDirection,
                                                              isHorizontal,
                                                              Pixel::L8,
                                                              originSize);
      }
      if(compositionPlan.needsOverlayStylePixelData)
      {
        pixels.overlayStylePixelData = renderMarqueeText(Ui::Text::Typesetter::RENDER_OVERLAY_STYLE, Pixel::RGBA8888);
      }

      Ui::Text::MarqueeBuilder::ApplyPreparedComposition(preparedContent, compositionRequest, pixels);
    }
  }

  Ui::Text::MarqueeInitialDelta marqueeInitialDelta;
  if(isHorizontal && isTextContentOverflow)
  {
    const float horizontalAlignment = Ui::Text::ResolveHorizontalMarqueeAlignment(true,
                                                                                  direction,
                                                                                  mController->GetHorizontalAlignment());
    marqueeInitialDelta             = Ui::Text::ResolveMarqueeInitialDelta(mMarqueeStartAnchor,
                                                                           marqueeTextureAnchor,
                                                                           horizontalAlignment,
                                                                           verifiedSize.width,
                                                                           controlSize.width,
                                                                           wrapGap);
  }
  else if(isHorizontal)
  {
    const float horizontalAlignment = Ui::Text::ResolveHorizontalMarqueeAlignment(false,
                                                                                  direction,
                                                                                  mController->GetHorizontalAlignment());
    marqueeInitialDelta             = Ui::Text::ResolveMarqueeFittingInitialDelta(mMarqueeFittingStartGeometry,
                                                                                  horizontalAlignment,
                                                                                  verifiedSize.width,
                                                                                  controlSize.width,
                                                                                  wrapGap);
  }

  // Set parameters for scrolling
  Renderer renderer = static_cast<Internal::Visual::Base&>(GetImplementation(mVisual)).GetRenderer();
  mTextScroller->SetParameters(Self(), renderer, preparedContent.textureSet, controlSize, verifiedSize, wrapGap,
                               isTextContentOverflow, direction,
                               mController->GetHorizontalAlignment(), mController->GetVerticalAlignment(),
                               mRendererUpdateNeeded, preparedContent.textGradient,
                               marqueeInitialDelta);
  mController->SetTextElideEnabled(actualellipsis);
  mController->SetMarqueeMaxTextureExceeded(false);
}

void LabelImpl::UpdateMarqueeState()
{
  if(HasInlineReplacementSource())
  {
    StopMarqueeImmediately();
    mController->SetMarqueeEnabled(false);
    mLastMarqueeEnabled = false;
    SuppressAutoMarqueeEvaluation();
    return;
  }
  EnableAutoMarqueeEvaluation();
  if(mController->IsMarqueeEnabled())
  {
    GetTextScroller();
    StopMarqueeImmediately();
    mController->SetMarqueeEnabled(true, true, mTextScroller->GetOrientation());
  }
}

void LabelImpl::EnableAutoMarqueeEvaluation()
{
  mSuppressAutoMarquee = false;
}

void LabelImpl::SuppressAutoMarqueeEvaluation()
{
  mSuppressAutoMarquee = true;
}

void LabelImpl::StopMarqueeImmediately()
{
  if(!mTextScroller)
  {
    return;
  }

  const Ui::Text::MarqueeStopMode stopMode = mTextScroller->GetStopMode();
  mTextScroller->SetStopMode(Ui::Text::MarqueeStopMode::IMMEDIATE);
  mTextScroller->StopScrolling();
  mTextScroller->SetStopMode(stopMode);
}

void LabelImpl::OnMarqueeVisibilityChanged(bool visible)
{
  if(!mTextScroller)
  {
    return;
  }

  if(visible)
  {
    if(mMarqueeTriggerPolicy == Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW)
    {
      EnableAutoMarqueeEvaluation();
      RequestTextRelayout();
    }
    else
    {
      if(mController->IsMarqueeEnabled() || mLastMarqueeEnabled)
      {
        mController->SetMarqueeEnabled(true, true, GetTextScroller()->GetOrientation());
      }
    }
  }
  else
  {
    if(mMarqueeTriggerPolicy == Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW)
    {
      SuppressAutoMarqueeEvaluation();
    }
    if(mLastMarqueeEnabled && !mController->IsMarqueeEnabled())
    {
      mLastMarqueeEnabled = false;
    }
    if(mTextScroller->IsScrolling())
    {
      StopMarqueeImmediately();
    }
  }
}

Ui::Text::TextScrollerPtr LabelImpl::GetTextScroller()
{
  if(!mTextScroller)
  {
    mTextScroller = Ui::Text::TextScroller::New(*this);
  }
  return mTextScroller;
}

void LabelImpl::InvalidateMarqueeStartGeometry()
{
  mMarqueeStartAnchor          = {};
  mMarqueeFittingStartGeometry = {};
}

void LabelImpl::CaptureMarqueeStartGeometry()
{
  if(mController->IsAsyncRendering())
  {
    return;
  }

  if(GetTextScroller()->GetOrientation() != Ui::Text::MarqueeOrientation::HORIZONTAL)
  {
    InvalidateMarqueeStartGeometry();
    return;
  }

  // Preserve the existing overflow capture path, including auto-marquee
  // activation during relayout.
  mMarqueeStartAnchor = mController->GetMarqueeStartAnchor();

  // A pending property/size update means the model no longer describes the
  // static renderer that will precede marquee. Prefer the legacy fallback to
  // applying a stale fitting translation.
  if(mRendererUpdateNeeded || mIsContentLayoutDirty)
  {
    mMarqueeFittingStartGeometry = {};
    return;
  }

  mMarqueeFittingStartGeometry =
    Ui::Text::ResolveMarqueeFittingStartGeometry(mController->GetRenderTextModel());
}

void LabelImpl::SetMarqueeEnabled(bool enabled)
{
  if(!enabled)
  {
    InvalidateMarqueeStartGeometry();
  }

  // FIXME: Baked VIEW_BOUND GradientSpan scrolls with marquee content until viewport-fixed span composition is supported.
  if(enabled && HasInlineReplacementSource())
  {
    InvalidateMarqueeStartGeometry();
    StopMarqueeImmediately();
    mController->SetMarqueeEnabled(false);
    mLastMarqueeEnabled = false;
    SuppressAutoMarqueeEvaluation();
    RequestAsyncRender();
    return;
  }
  if(mMarqueeTriggerPolicy == Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW)
  {
    if(enabled)
    {
      EnableAutoMarqueeEvaluation();
      RequestTextRelayout();
    }
    else
    {
      SuppressAutoMarqueeEvaluation();
      if(mTextScroller)
      {
        mTextScroller->StopScrolling();
      }
    }
    RequestAsyncRender();
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
        CaptureMarqueeStartGeometry();
        mController->SetMarqueeEnabled(enabled, true, GetTextScroller()->GetOrientation());
      }
      RequestAsyncRender();
    }
  }
}

void LabelImpl::OnViewEffectiveVisibilityChanged(Actor actor, bool visible)
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
    mIsContentLayoutDirty     = false;
    mIsManualRenderInProgress = false;
    mIsManualRenderFinished   = false;
  }
  OnMarqueeVisibilityChanged(visible);
}

bool LabelImpl::IsVisible()
{
  if(!mIsVisibleInitialized)
  {
    mIsVisible            = Dali::DevelActor::IsOnSceneVisible(Self());
    mIsVisibleInitialized = true;
  }
  return mIsVisible;
}

void LabelImpl::OnSceneConnection(int depth)
{
  ViewImpl::OnSceneConnection(depth);

  Dali::Window window = Window::Get(Self());
  if(window)
  {
    // Sets layoutDirection value
    Dali::LayoutDirection::Type layoutDirection = window.GetRootLayer().GetEffectiveLayoutDirection();
    mController->SetLayoutDirection(layoutDirection);
  }
}

void LabelImpl::EvaluateAndApplyMarquee(const Size& contentSize, Ui::Text::MarqueeOrientation orientation)
{
  if(HasInlineReplacementSource())
  {
    if(mController->IsMarqueeEnabled())
    {
      StopMarqueeImmediately();
      mController->SetMarqueeEnabled(false);
    }
    return;
  }
  if(mMarqueeTriggerPolicy != Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW || mSuppressAutoMarquee || !IsVisible())
  {
    return;
  }

  bool       marqueeEnabled   = false;
  const bool multiLineEnabled = mController->IsMultiLineEnabled();

  if(orientation == Ui::Text::MarqueeOrientation::HORIZONTAL)
  {
    if(multiLineEnabled)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Horizontal marquee is valid only for single-line text\n");
      marqueeEnabled = false;
    }
    else
    {
      const Size naturalSize = mController->GetNaturalSize(false).GetVectorXY();
      marqueeEnabled         = contentSize.width < naturalSize.width;
    }
  }
  else // MarqueeOrientation::VERTICAL
  {
    if(!multiLineEnabled)
    {
      DALI_LOG_INFO(gLogFilter, Debug::General, "Vertical marquee is valid only for multi-line text\n");
      marqueeEnabled = false;
    }
    else
    {
      const float textHeight = mController->GetHeightForWidth(contentSize.width);
      marqueeEnabled         = contentSize.height < textHeight;
    }
  }

  if(marqueeEnabled != mController->IsMarqueeEnabled())
  {
    if(marqueeEnabled)
    {
      CaptureMarqueeStartGeometry();
    }
    else
    {
      InvalidateMarqueeStartGeometry();
    }
    mController->SetMarqueeEnabled(marqueeEnabled, true, orientation);
  }
}

void LabelImpl::PrepareMarqueeLayout(const Size& contentSize, Ui::Text::MarqueeOrientation orientation, Size& originSize)
{
  originSize = Size::ZERO;

  if(mController->IsMarqueeEnabled())
  {
    const bool isVerticalScroll = (orientation == Ui::Text::MarqueeOrientation::VERTICAL);

    const bool needLayoutSizeCalculation =
      isVerticalScroll && (mController->GetVerticalAlignment() != Ui::Text::Alignment::START);

    if(needLayoutSizeCalculation)
    {
      mController->SetMarqueeEnabled(false, false, Ui::Text::MarqueeOrientation::VERTICAL);
      originSize = mController->CalculateLayoutSize(contentSize.x, contentSize.y, true);
      mController->SetMarqueeEnabled(true, false, Ui::Text::MarqueeOrientation::VERTICAL);
    }
  }
}

void LabelImpl::OnVariationPropertyNotify(PropertyNotification source)
{
  if(!mHasVariationProperties)
  {
    return;
  }

  const auto* data = Internal::Text::GetFontVariationPropertyData(Ui::View::DownCast(Self()));
  if(!data)
  {
    return;
  }

  Actor self = Self();

  Property::Map map;
  mController->GetVariationsMap(map);

  data->ApplyCurrentPropertyValues(self, map);

  mController->SetVariationsMap(map);
}

bool LabelImpl::HandleVariationPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  if(!mHasVariationProperties)
  {
    return false;
  }

  const auto* data = Internal::Text::GetFontVariationPropertyData(Ui::View::DownCast(Self()));
  if(!data)
  {
    return false;
  }

  Dali::String tag;
  if(!data->Find(index, tag))
  {
    return false;
  }

  Actor self = Self();
  if(!self.DoesCustomPropertyExist(index))
  {
    return false;
  }

  float value = 0.0f;
  propertyValue.Get(value);

  Property::Map map;
  mController->GetVariationsMap(map);
  map[tag] = value;

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
    Property::Value*     mixColorValue = backgroundMap.Find(Ui::VisualBasePropertyIndex::MIX_COLOR);
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

void LabelImpl::ApplyLocalizedText(BaseHandle target, const Dali::String& text)
{
  SetText(text);
}

Ui::Text::AsyncTextParameters LabelImpl::GetAsyncTextParameters(const Text::Async::RequestType    requestType,
                                                                const Vector2&                    contentSize,
                                                                const Insets&                     padding,
                                                                const Dali::LayoutDirection::Type layoutDirection)
{
  // Logically, all properties of the label should be passed.

  std::string text;
  mController->GetText(text);

  Ui::Text::AsyncTextParameters parameters;
  parameters.requestType     = requestType;
  parameters.textWidth       = contentSize.width;
  parameters.textHeight      = contentSize.height;
  parameters.padding         = padding;
  parameters.layoutDirection = layoutDirection;
  parameters.text            = text;
  if(HasInlineReplacementSource())
  {
    parameters.replacementSourceSnapshot         = mController->GetReplacementSourceSnapshot();
    Ui::View                               owner = Ui::View::DownCast(Self());
    Internal::Text::InlineReplacementData& data  = Internal::Text::GetOrCreateInlineReplacementData(owner);
    parameters.replacementLayoutGeneration       = ++data.requestGeneration;
    if(requestType <= Text::Async::RENDER_CONSTRAINT)
    {
      data.lastRenderGeneration = parameters.replacementLayoutGeneration;
    }
  }

  parameters.maxTextureSize               = Dali::GetMaxTextureSize();
  parameters.maximumNumberOfLines         = static_cast<Ui::Text::Length>(mController->GetMaximumNumberOfLines());
  parameters.maximumNumberOfLinesRevision = mController->GetMaximumNumberOfLinesRevision();
  parameters.fontSize                     = mController->GetDefaultFontSize(Ui::Text::Controller::POINT_SIZE);
  parameters.textColor                    = mController->GetDefaultColor();
  parameters.anchorColor                  = mController->GetAnchorColor();
  parameters.anchorClickedColor           = mController->GetAnchorClickedColor();
  parameters.fontFamily                   = mController->GetDefaultFontFamily();
  parameters.fontWeight                   = mController->GetDefaultFontWeight();
  parameters.fontWidth                    = mController->GetDefaultFontWidth();
  parameters.fontSlant                    = mController->GetDefaultFontSlant();
  parameters.isMultiLine                  = mController->IsMultiLineEnabled();
  parameters.ellipsis                     = mController->IsTextElideEnabled();
  parameters.minLineSize                  = mController->GetDefaultLineSize();
  parameters.relativeLineSize             = mController->GetRelativeLineSize();
  parameters.characterSpacing             = mController->GetCharacterSpacing();
  parameters.effectiveTextScale           = mController->GetEffectiveTextScale();
  parameters.horizontalAlignment          = mController->GetHorizontalAlignment();
  parameters.verticalAlignment            = mController->GetVerticalAlignment();
  parameters.verticalLineAlignment        = mController->GetVerticalLineAlignment();
  parameters.lineWrapMode                 = mController->GetLineWrapMode();
  parameters.layoutDirectionPolicy        = mController->GetLayoutDirectionMode();
  parameters.ellipsisPosition             = mController->GetEllipsisPosition();
  parameters.isUnderlineEnabled           = mController->IsUnderlineEnabled();
  parameters.underlineType                = mController->GetUnderlineType();
  parameters.underlineColor               = mController->GetUnderlineColor();
  parameters.underlineHeight              = mController->GetUnderlineHeight();
  parameters.dashedUnderlineWidth         = mController->GetDashedUnderlineWidth();
  parameters.dashedUnderlineGap           = mController->GetDashedUnderlineGap();
  parameters.isStrikethroughEnabled       = mController->IsStrikethroughEnabled();
  parameters.strikethroughColor           = mController->GetStrikethroughColor();
  parameters.strikethroughHeight          = mController->GetStrikethroughHeight();
  parameters.isTextBackgroundEnabled      = mController->IsBackgroundEnabled();
  parameters.textBackgroundColor          = mController->GetBackgroundColor();
  parameters.isShadowEnabled              = mController->IsShadowEnabled();
  parameters.shadowBlurRadius             = mController->GetShadowBlurRadius();
  parameters.shadowColor                  = mController->GetShadowColor();
  parameters.shadowOffset                 = mController->GetShadowOffset();
  parameters.isOutlineEnabled             = mController->IsOutlineEnabled();
  parameters.outlineWidth                 = mController->GetOutlineWidth();
  parameters.outlineColor                 = mController->GetOutlineColor();
  parameters.outlineBlurRadius            = mController->GetOutlineBlurRadius();
  parameters.outlineOffset                = mController->GetOutlineOffset();
  parameters.isTextFitEnabled             = mController->IsTextFitEnabled();
  parameters.textFitMinSize               = mController->GetTextFitMinSize(Ui::Text::Controller::POINT_SIZE);
  parameters.textFitMaxSize               = mController->GetTextFitMaxSize(Ui::Text::Controller::POINT_SIZE);
  parameters.textFitStepSize              = mController->GetTextFitStepSize(Ui::Text::Controller::POINT_SIZE);
  parameters.isTextFitCandidatesEnabled   = mController->IsTextFitCandidatesEnabled();
  parameters.textFitCandidates            = mController->GetTextFitCandidates();
  parameters.isMarqueeEnabled             = mController->IsMarqueeEnabled();
  const bool hasMarqueeBlocker            = HasInlineReplacementSource();
  if(hasMarqueeBlocker)
  {
    parameters.isMarqueeEnabled = false;
  }
  parameters.marqueeTriggerPolicy = mMarqueeTriggerPolicy;
  parameters.suppressAutoMarquee  = mSuppressAutoMarquee || hasMarqueeBlocker;
  if(parameters.isMarqueeEnabled || parameters.marqueeTriggerPolicy == Ui::Text::MarqueeTriggerPolicy::ON_OVERFLOW)
  {
    parameters.marqueeStopMode    = GetTextScroller()->GetStopMode();
    parameters.marqueeSpeed       = GetTextScroller()->GetSpeed();
    parameters.marqueeLoopCount   = GetTextScroller()->GetLoopCount();
    parameters.marqueeLoopDelay   = GetTextScroller()->GetLoopDelay();
    parameters.marqueeGap         = static_cast<int>(GetTextScroller()->GetGap() * GetTextUiScale());
    parameters.marqueeOrientation = GetTextScroller()->GetOrientation();
    if(parameters.marqueeOrientation == Ui::Text::MarqueeOrientation::HORIZONTAL)
    {
      parameters.marqueeStartAnchor = mMarqueeStartAnchor;
    }
  }
  parameters.isCutoutEnabled               = mController->IsTextCutout();
  parameters.isBackgroundWithCutoutEnabled = mController->IsBackgroundWithCutoutEnabled();
  parameters.backgroundColorWithCutout     = mController->GetBackgroundColorWithCutout();
  const auto* revealData                   = Internal::Text::GetTextRevealData(mTextRevealData);
  parameters.textRevealRevision            = revealData ? revealData->revision : 0u;
  parameters.isTextRevealEnabled           = revealData && revealData->enabled &&
                                   !parameters.isMarqueeEnabled &&
                                   !parameters.isCutoutEnabled;
  if(parameters.isTextRevealEnabled)
  {
    parameters.textRevealUnit                 = Ui::Text::Internal::Reveal::ToInternalUnit(revealData->unit);
    parameters.textRevealSequence             = Ui::Text::Internal::Reveal::ToInternalSequence(revealData->sequence);
    parameters.textRevealFadeDurationRatio    = revealData->fadeDurationRatio;
    parameters.textRevealSequenceStaggerRatio = revealData->sequenceStaggerRatio;
  }
  mController->GetVariationsMap(parameters.variationsMap);
  parameters.renderScale                    = mController->GetRenderScale();
  parameters.isEmbossEnabled                = mController->IsEmbossEnabled();
  parameters.embossDirection                = mController->GetEmbossDirection();
  parameters.embossStrength                 = mController->GetEmbossStrength();
  parameters.embossLightColor               = mController->GetEmbossLightColor();
  parameters.embossShadowColor              = mController->GetEmbossShadowColor();
  const auto* gradientData                  = Internal::Text::GetTextGradientPropertyData(mTextGradientPropertyData);
  parameters.isTextGradientRequested        = gradientData && Ui::Text::Internal::Gradient::IsRenderable(gradientData->textGradient);
  parameters.isTextGradientOverlayRequested = gradientData && Ui::Text::Internal::Gradient::IsRenderable(gradientData->textGradientOverlay);
  if(mHasAsyncAnchorHitRegions)
  {
    parameters.clickedAnchors = Internal::Text::GetAnchorClickedStates(Ui::View::DownCast(Self()));
  }

  if(mHasStyledTextSource)
  {
    const Ui::Text::StyledText styledTextSource = Internal::Text::GetStyledTextSource(mStyledTextSourceData);
    if(styledTextSource)
    {
      const Dali::String styledText         = styledTextSource.GetText();
      parameters.text                       = std::string(styledText.CStr(), styledText.Size());
      parameters.hasStyledTextStyleSnapshot = true;
      parameters.styledTextStyleSnapshot =
        Dali::Ui::Internal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(styledTextSource,
                                                                               GetDpi(),
                                                                               mController->GetAnchorColor(),
                                                                               mController->GetAnchorClickedColor());
    }
  }

  return parameters;
}

void LabelImpl::EmitTextFitChanged()
{
  // Intentionally not emitted for now.
  // Revisit when the public TextFitChanged API direction is finalized.
  // mController->GetTextFitFontSize(Ui::Text::Controller::PIXEL_SIZE);
}

void LabelImpl::EmitAsyncRenderFinished(float width, float height)
{
  if(!mAsyncRenderFinishedSignal.Empty())
  {
    Ui::View handle(GetOwner());
    mAsyncRenderFinishedSignal.Emit(handle, width, height);
  }
}

void LabelImpl::EmitAsyncNaturalSizeComputed(float width, float height)
{
  if(!mAsyncNaturalSizeComputedSignal.Empty())
  {
    Ui::View handle(GetOwner());
    mAsyncNaturalSizeComputedSignal.Emit(handle, width, height);
  }
}

void LabelImpl::EmitAsyncHeightForWidthComputed(float width, float height)
{
  if(!mAsyncHeightForWidthComputedSignal.Empty())
  {
    Ui::View handle(GetOwner());
    mAsyncHeightForWidthComputedSignal.Emit(handle, width, height);
  }
}

// =============================================================================
// UiColorManager
// =============================================================================
void LabelImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    Self().SetProperty(Ui::Text::LabelPropertyIndex::TEXT_COLOR, color);
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
    mController->SetAnchorColor(color);
    RequestRendererUpdate();
    if(mController->IsAsyncRendering())
    {
      RequestTextRelayout();
      RequestAsyncRender();
    }
  }
}

void LabelImpl::SetAnchorClickedColorInternal(const Vector4& color)
{
  if(mController->GetAnchorClickedColor() != color)
  {
    mController->SetAnchorClickedColor(color);
    RequestRendererUpdate();
    if(mController->IsAsyncRendering())
    {
      RequestTextRelayout();
      RequestAsyncRender();
    }
  }
}

void LabelImpl::SetTextBackgroundColorInternal(const Vector4& color)
{
  if(mController->GetBackgroundColor() != color)
  {
    mController->SetBackgroundColor(color);
    RequestRendererUpdate();
  }
}

void LabelImpl::SetUnderlineColorInternal(const Vector4& color)
{
  if(mController->GetUnderlineColor() != color)
  {
    mController->SetUnderlineColor(color);
  }
}

void LabelImpl::SetShadowColorInternal(const Vector4& color)
{
  if(mController->GetShadowColor() != color)
  {
    mController->SetShadowColor(color);
  }
}

void LabelImpl::SetOutlineColorInternal(const Vector4& color)
{
  if(mController->GetOutlineColor() != color)
  {
    mController->SetOutlineColor(color);
  }
}

void LabelImpl::SetLineThroughColorInternal(const Vector4& color)
{
  if(mController->GetStrikethroughColor() != color)
  {
    mController->SetStrikethroughColor(color);
  }
}

void LabelImpl::SetBevelLightColorInternal(const Vector4& color)
{
  if(mController->GetEmbossLightColor() != color)
  {
    mController->SetEmbossLightColor(color);
  }
}

void LabelImpl::SetBevelShadowColorInternal(const Vector4& color)
{
  if(mController->GetEmbossShadowColor() != color)
  {
    mController->SetEmbossShadowColor(color);
  }
}

// =============================================================================
// Properties
// =============================================================================
void LabelImpl::OnPropertySet(Dali::Property::Index index, const Dali::Property::Value& propertyValue)
{
  if(!PropertyHandler::OnPropertySet(*this, index, propertyValue))
  {
    ViewImpl::OnPropertySet(index, propertyValue); // up call to control for non-handled properties
  }

  if(mIsContentLayoutDirty)
  {
    EnableAutoMarqueeEvaluation();
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
