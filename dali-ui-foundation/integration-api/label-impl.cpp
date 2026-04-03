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
#include <dali-ui-foundation/internal/text/text-style-helper.h>

#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/text/text-view.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view.h>

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

// clang-format off
// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(LabelImpl, ViewImpl, Create)

LABEL_PROPERTY_REGISTRATION("text",                 STRING,  TEXT                 )
LABEL_PROPERTY_REGISTRATION("fontFamily",           STRING,  FONT_FAMILY          )
LABEL_PROPERTY_REGISTRATION("fontSize",             FLOAT,   FONT_SIZE            )
LABEL_PROPERTY_REGISTRATION("multiLine",            BOOLEAN, MULTI_LINE           )
LABEL_PROPERTY_REGISTRATION("lineWrapMode",         INTEGER, LINE_WRAP_MODE       )
LABEL_PROPERTY_REGISTRATION("horizontalAlignment",  INTEGER, HORIZONTAL_ALIGNMENT )
LABEL_PROPERTY_REGISTRATION("verticalAlignment",    INTEGER, VERTICAL_ALIGNMENT   )
LABEL_PROPERTY_REGISTRATION("lineHeight",           FLOAT,   LINE_HEIGHT          )
LABEL_PROPERTY_REGISTRATION("lineHeightMode",       INTEGER, LINE_HEIGHT_MODE     )
LABEL_PROPERTY_REGISTRATION("layoutDirectionMode",  INTEGER, LAYOUT_DIRECTION_MODE)
LABEL_PROPERTY_REGISTRATION("markupEnabled",        BOOLEAN, MARKUP_ENABLED       )
LABEL_PROPERTY_REGISTRATION("anchorColor",          VECTOR4, ANCHOR_COLOR         )
LABEL_PROPERTY_REGISTRATION("anchorClickedColor",   VECTOR4, ANCHOR_CLICKED_COLOR )
LABEL_PROPERTY_REGISTRATION("marqueeSpeed",         INTEGER, MARQUEE_SPEED        )
LABEL_PROPERTY_REGISTRATION("marqueeLoopCount",     INTEGER, MARQUEE_LOOP_COUNT   )
LABEL_PROPERTY_REGISTRATION("marqueeLoopDelay",     FLOAT,   MARQUEE_LOOP_DELAY   )
LABEL_PROPERTY_REGISTRATION("marqueeGap",           INTEGER, MARQUEE_GAP          )
LABEL_PROPERTY_REGISTRATION("marqueeStopMode",      INTEGER, MARQUEE_STOP_MODE    )
LABEL_PROPERTY_REGISTRATION("marqueeOrientation",   INTEGER, MARQUEE_ORIENTATION  )
LABEL_PROPERTY_REGISTRATION("fontWeight",           INTEGER, FONT_WEIGHT          )
LABEL_PROPERTY_REGISTRATION("fontWidth",            INTEGER, FONT_WIDTH           )
LABEL_PROPERTY_REGISTRATION("fontSlant",            INTEGER, FONT_SLANT           )

LABEL_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT("textColor",      Color::BLACK,     TEXT_COLOR       )
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorRed",   TEXT_COLOR_RED,   TEXT_COLOR,     0)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorGreen", TEXT_COLOR_GREEN, TEXT_COLOR,     1)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorBlue",  TEXT_COLOR_BLUE,  TEXT_COLOR,     2)
LABEL_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(   "textColorAlpha", TEXT_COLOR_ALPHA, TEXT_COLOR,     3)

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

} // namespace

LabelImplPtr LabelImpl::New()
{
  return LabelImplPtr(new LabelImpl());
}

LabelImpl::LabelImpl()
: ViewImpl(),
  mTouchPosition(),
  mLineHeight(-1.0f),
  mLineHeightMode(Text::LineHeightMode::RELATIVE),
  mTextColorAnimatedCount(0),
  mTextUpdateNeeded(false),
  mLastMarqueeEnabled(false),
  mIsTouchDown(false),
  mHasAnchors(false),
  mIsVisible(false),
  mIsVisibleInitialized(false)
{
}

LabelImpl::~LabelImpl()
{
}

// =============================================================================
// Properties
// =============================================================================
void LabelImpl::SetText(const Dali::String& text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.CStr());
  mController->SetText(ToStdString(text));
  UpdateAnchorTouchInterception();
  mTextUpdateNeeded = true;
  RequestTextRelayout();
}

Dali::String LabelImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return ToDaliString(text);
}

void LabelImpl::SetFontFamily(const Dali::String& fontFamily)
{
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

void LabelImpl::SetLineHeight(float lineHeight)
{
  DALI_LOG_RELEASE_INFO("[%p] %f\n", mController.Get(), lineHeight);
  if(mLineHeight != lineHeight)
  {
    mLineHeight = lineHeight;
    UpdateLineHeight();
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
    RequestTextRelayout();
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return 0;
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return 0;
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return 0.0f;
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return 0;
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return Text::MarqueeStopMode::IMMEDIATE;
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
  // TODO: Return the default value from UI config when the text scroller is not created.
  return Text::MarqueeOrientation::HORIZONTAL;
}

void LabelImpl::SetFontWeight(Text::FontWeight weight)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWeight::Name[weight]);
  mController->SetDefaultFontWeight(weight);
}

Text::FontWeight LabelImpl::GetFontWeight() const
{
  return mController->GetDefaultFontWeight();
}

void LabelImpl::SetFontWidth(Text::FontWidth width)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontWidth::Name[width]);
  mController->SetDefaultFontWidth(width);
}

Text::FontWidth LabelImpl::GetFontWidth() const
{
  return mController->GetDefaultFontWidth();
}

void LabelImpl::SetFontSlant(Text::FontSlant slant)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), TextAbstraction::FontSlant::Name[slant]);
  mController->SetDefaultFontSlant(slant);
}

Text::FontSlant LabelImpl::GetFontSlant() const
{
  return mController->GetDefaultFontSlant();
}

void LabelImpl::SetUnderline(const Text::Underline& underline)
{
  const UiColor& color = underline.GetColor();

  SetColorBinding("UnderlineColor", color, this, &LabelImpl::SetUnderlineColorInternal);

  Text::ApplyUnderlineStyle(mController, underline);
}

void LabelImpl::ResetUnderline()
{
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

  Text::ApplyShadowStyle(mController, shadow);
}

void LabelImpl::ResetShadow()
{
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

  Text::ApplyOutlineStyle(mController, outline);
}

void LabelImpl::ResetOutline()
{
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

  Text::ApplyLineThroughStyle(mController, lineThrough);
}

void LabelImpl::ResetLineThrough()
{
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

  Text::ApplyBevelStyle(mController, bevel);
}

void LabelImpl::ResetBevel()
{
  UiColorManager::Get().ClearBinding(Self(), "BevelLightColor");
  UiColorManager::Get().ClearBinding(Self(), "BevelShadowColor");
  if(mController->IsEmbossEnabled())
  {
    mController->SetEmbossEnabled(false);
  }
}

// =============================================================================
// Read Only
// =============================================================================
int LabelImpl::GetLineCount()
{
  float width = Self().GetProperty(Actor::Property::SIZE_WIDTH).Get<float>();
  float clamp = std::clamp(width, GetMinimumWidth(), GetMaximumWidth());
  return GetLineCount(clamp);
}

int LabelImpl::GetLineCount(float width)
{
  Extents padding      = GetViewPadding();
  float   contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  return mController->GetLineCount(contentWidth);
}

bool LabelImpl::IsMarqueeRunning() const
{
  if(mTextScroller)
  {
    return mTextScroller->IsScrolling();
  }
  return false;
}

// =============================================================================
// Method
// =============================================================================
void LabelImpl::StartMarquee()
{
  SetMarqueeEnabled(true);
}

void LabelImpl::StopMarquee()
{
  SetMarqueeEnabled(false);
}

// =============================================================================
// Signals
// =============================================================================
Signal<void(View, const Dali::String&)>& LabelImpl::AnchorClickedSignal()
{
  return mAnchorClickedSignal;
}

void LabelImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  Actor self = Self();

  Dali::Property::Map propertyMap;
  propertyMap.Add(Ui::Visual::Property::TYPE, Ui::Visual::TEXT);

  mVisual = Ui::VisualFactory::Get().CreateVisual(propertyMap);

  // TODO: Since the TEXT property is not available yet, this is a temporary index.
  View view = Ui::View::DownCast(self);
  Internal::ViewDataImpl::Get(Integration::GetImpl(view)).RegisterVisual(PROPERTY_REGISTRATION_START_INDEX, mVisual, DepthIndex::CONTENT);
  Internal::TextVisual::SetAnimatableTextColorProperty(mVisual, Text::LabelPropertyIndex::TEXT_COLOR);
  Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);

  mController = Internal::TextVisual::GetController(mVisual);
  DALI_ASSERT_DEBUG(mController && "Invalid Text Controller")

  mController->SetControlInterface(this);
  mController->SetAnchorControlInterface(this);

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
}

void LabelImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(mTextScroller && mTextScroller->IsStopRequested())
  {
    // When marquee stop is requested in FINISH_LOOP mode, defer relayout until scrolling finishes.
    return;
  }

  Actor self = Self();

  Extents padding = GetViewPadding();
  float   width   = std::max(size.x - (padding.start + padding.end), 0.0f);
  float   height  = std::max(size.y - (padding.top + padding.bottom), 0.0f);
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

  const Text::MarqueeOrientation marqueeOrientation = mTextScroller ? mTextScroller->GetOrientation() : Text::MarqueeOrientation::HORIZONTAL;
  // TODO: This is only meaningful after marquee ellipsis mode is supported.
  EvaluateAndApplyMarquee(contentSize, marqueeOrientation);

  Size originSize = Size::ZERO;
  PrepareMarqueeLayout(contentSize, marqueeOrientation, originSize);

  const Text::Controller::UpdateTextType updateTextType = mController->Relayout(contentSize, layoutDirection);

  if((Text::Controller::NONE_UPDATED != (Text::Controller::MODEL_UPDATED & updateTextType)) || mTextUpdateNeeded)
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
      DALI_LOG_DEBUG_INFO(
        "layoutSize(%f) > maxTextureSize(%d): To guarantee the behavior of Texture::New, layoutSize must not be "
        "bigger than maxTextureSize\n",
        layoutSize.width, maxTextureSize);
      layoutSize.width = maxTextureSize;
    }

    // This affects font rendering quality.
    // It need to be integerized.
    Vector2 visualTransformOffset;
    visualTransformOffset.x = roundf(padding.start + alignmentOffset.x);
    visualTransformOffset.y = roundf(padding.top + alignmentOffset.y);

    mController->SetLayoutAlignmentOffset(alignmentOffset);
    mController->SetLayoutOffsetWithPadding(visualTransformOffset);

    Vector2 visualTransformSize = layoutSize;

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

    if(mController->IsAutoScrollEnabled())
    {
      InitializeMarquee(contentSize, originSize);
    }

    mTextUpdateNeeded = false;
  }
}

Vector3 LabelImpl::GetNaturalSize()
{
  Extents padding     = GetViewPadding();
  Vector3 naturalSize = mController->GetNaturalSize();
  naturalSize.width += (padding.start + padding.end);
  naturalSize.height += (padding.top + padding.bottom);

  return naturalSize;
}

float LabelImpl::GetHeightForWidth(float width)
{
  Extents padding      = GetViewPadding();
  float   contentWidth = std::max(width - static_cast<float>(padding.start + padding.end), 0.0f);
  return mController->GetHeightForWidth(contentWidth) + static_cast<float>(padding.top + padding.bottom);
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
  Integration::ViewImpl::OnAnimateAnimatableProperty(animation, index, state);
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
  Integration::ViewImpl::OnConstraintAnimatableProperty(constraint, index, applied);
}

MeasuredSize LabelImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint,
                        heightConstraint);

  const float requestedWidth  = GetRequestedWidth();
  const float requestedHeight = GetRequestedHeight();

  const float minWidth  = GetMinimumWidth();
  const float maxWidth  = GetMaximumWidth();
  const float minHeight = GetMinimumHeight();
  const float maxHeight = GetMaximumHeight();

  const Vector3 naturalSize = GetNaturalSize();

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(requestedWidth > 0.0f)
  {
    measuredWidth = requestedWidth;
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    measuredWidth = std::max(0.0f, widthConstraint);
  }
  else
  {
    const float width           = std::max(0.0f, naturalSize.width);
    const float allowedMaxWidth = (widthConstraint >= 0.0f) ? std::min(maxWidth, widthConstraint) : maxWidth;
    measuredWidth               = std::max(std::min(width, allowedMaxWidth), minWidth);
  }

  // Height
  if(requestedHeight > 0.0f)
  {
    measuredHeight = requestedHeight;
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    measuredHeight = std::max(0.0f, heightConstraint);
  }
  else
  {
    const float allowedMaxHeight = (heightConstraint >= 0.0f) ? std::min(maxHeight, heightConstraint) : maxHeight;
    const float height           = std::max(0.0f, GetHeightForWidth(measuredWidth));
    measuredHeight               = std::max(std::min(height, allowedMaxHeight), minHeight);
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

// =============================================================================
// ControlInterface
// =============================================================================
void LabelImpl::RequestTextRelayout()
{
  // Signal that a Relayout may be needed
  RelayoutRequest();
}

// =============================================================================
// ScrollerInterface
// =============================================================================
void LabelImpl::ScrollingFinished()
{
  // Pure Virtual from TextScroller Interface
  DALI_LOG_RELEASE_INFO("[%p]\n", mController.Get());
  mController->SetAutoScrollEnabled(false);
  RequestTextRelayout();
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
// Implementation
// =============================================================================
void LabelImpl::SetMinimumLineHeight(float height)
{
  // If TextFitArray is enabled, do not update the default line size.
  if(!mController->IsTextFitArrayEnabled())
  {
    mTextUpdateNeeded = mController->SetDefaultLineSize(height) || mTextUpdateNeeded;
  }
  mController->SetCurrentLineSize(height);
}

void LabelImpl::UpdateLineHeight()
{
  if(mLineHeightMode == Text::LineHeightMode::RELATIVE)
  {
    SetMinimumLineHeight(0.0f);
    mController->SetRelativeLineSize(mLineHeight);
  }
  else
  {
    mController->SetRelativeLineSize(-1.0f);
    SetMinimumLineHeight(mLineHeight);
  }
  RequestTextRelayout();
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
        Extents       padding    = GetViewPadding();
        const Vector2 localPoint = touch.GetLocalPosition(0);
        mController->AnchorEvent(localPoint.x - padding.start, localPoint.y - padding.top);
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
  const Text::CharacterDirection direction = mController->GetAutoScrollTextDirection();

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
        mController->SetAutoScrollMaxTextureExceeded(true);
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
        mController->SetAutoScrollEnabled(false, false, Text::MarqueeOrientation::VERTICAL);
        mController->SetTextElideEnabled(true);
      }

      mController->CalculateLayoutSize(controlSize.width, maxTextureSize, true);
      wrapGap = std::max(maxTextureSize - textHeight, 0.0f);
      if(!mController->IsAutoScrollEnabled())
      {
        mController->SetAutoScrollEnabled(true, false, Text::MarqueeOrientation::VERTICAL);
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
  mTextScroller->SetParameters(Self(), renderer, textureSet, controlSize, verifiedSize, wrapGap, direction, mController->GetHorizontalAlignment(), mController->GetVerticalAlignment(), mTextUpdateNeeded);
  mController->SetTextElideEnabled(actualellipsis);
  mController->SetAutoScrollMaxTextureExceeded(false);
}

void LabelImpl::UpdateMarqueeState()
{
  if(mController->IsAutoScrollEnabled())
  {
    const Text::MarqueeStopMode stopMode = GetTextScroller()->GetStopMode();
    mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
    mTextScroller->StopScrolling();
    mTextScroller->SetStopMode(stopMode);
    mController->SetAutoScrollEnabled(true, true, mTextScroller->GetOrientation());
  }
}

void LabelImpl::OnMarqueeVisibilityChanged(bool visible)
{
  if(mTextScroller)
  {
    if(visible)
    {
      // TODO: Handle AUTO_SCROLL ellipsis on visibility change (disable when hidden, restore when visible)
      // if(mLastEllipsisMode == Text::Ellipsize::AUTO_SCROLL)
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
        if(mController->IsAutoScrollEnabled() || mLastMarqueeEnabled)
        {
          mController->SetAutoScrollEnabled(true, true, GetTextScroller()->GetOrientation());
        }
      }
    }
    else
    {
      // TODO: Handle AUTO_SCROLL ellipsis on visibility change (disable when hidden, restore when visible)
      // if(mController->GetEllipsisMode() == Text::Ellipsize::AUTO_SCROLL)
      // {
      //   mLastEllipsisMode = Text::Ellipsize::AUTO_SCROLL;
      //   mController->SetEllipsisMode(Text::Ellipsize::TRUNCATE);
      //   if(mTextScroller)
      //   {
      //     mTextScroller->SetStopMode(Text::MarqueeStopMode::IMMEDIATE);
      //     mTextScroller->StopScrolling();
      //   }
      // }
      // else
      {
        if(mLastMarqueeEnabled && !mController->IsAutoScrollEnabled())
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
  if(mController->IsTextElideEnabled() && mController->GetEllipsisMode() == Text::Ellipsize::AUTO_SCROLL)
  {
    DALI_LOG_DEBUG_INFO("Tried to autoscroll while in ellipsize auto scroll mode, request ignored.\n");
  }
  else
  {
    mLastMarqueeEnabled = enabled;
    // If request to auto scroll is the same as current state then do nothing.
    if(enabled != mController->IsAutoScrollEnabled())
    {
      // If request is disable (false) and auto scrolling is enabled then need to stop it
      if(enabled == false)
      {
        if(mTextScroller)
        {
          mTextScroller->StopScrolling();
        }
      }
      // If request is enable (true) then start autoscroll as not already running
      else
      {
        mController->SetAutoScrollEnabled(enabled, true, GetTextScroller()->GetOrientation());
      }
    }
  }
}

void LabelImpl::OnControlInheritedVisibilityChanged(Actor actor, bool visible)
{
  mIsVisible            = visible;
  mIsVisibleInitialized = true;
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
  if(mController->IsTextElideEnabled() && mController->GetEllipsisMode() == Text::Ellipsize::AUTO_SCROLL)
  {
    if(IsVisible())
    {
      bool marqueeEnabled = false;

      if(orientation == Text::MarqueeOrientation::HORIZONTAL)
      {
        if(mController->IsMultiLineEnabled())
        {
          DALI_LOG_DEBUG_INFO("Attempted ellipsize auto scroll on a non SINGLE_LINE_BOX, request ignored\n");
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

      if(marqueeEnabled != mController->IsAutoScrollEnabled())
      {
        mController->SetAutoScrollEnabled(marqueeEnabled, false, orientation);
      }
    }
  }
}

void LabelImpl::PrepareMarqueeLayout(const Size& contentSize, Text::MarqueeOrientation orientation, Size& originSize)
{
  originSize = Size::ZERO;

  if(mController->IsAutoScrollEnabled())
  {
    const bool isVerticalScroll = (orientation == Text::MarqueeOrientation::VERTICAL);

    const bool needLayoutSizeCalculation =
      isVerticalScroll && (mController->GetVerticalAlignment() != Text::Alignment::START);

    if(needLayoutSizeCalculation)
    {
      mController->SetAutoScrollEnabled(false, false, Text::MarqueeOrientation::VERTICAL);
      originSize = mController->CalculateLayoutSize(contentSize.x, contentSize.y, true);
      mController->SetAutoScrollEnabled(true, false, Text::MarqueeOrientation::VERTICAL);
    }
  }
}

// =============================================================================
// UiColorManager
// =============================================================================
void LabelImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    Self().SetProperty(Text::LabelPropertyIndex::TEXT_COLOR, color);
    mController->SetDefaultColor(color);
    mTextUpdateNeeded = true;

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
    mTextUpdateNeeded = true;
    RequestTextRelayout();
  }
}

void LabelImpl::SetAnchorClickedColorInternal(const Vector4& color)
{
  if(mController->GetAnchorClickedColor() != color)
  {
    mController->SetAnchorClickedColor(color);
    mTextUpdateNeeded = true;
    RequestTextRelayout();
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
  switch(index)
  {
    case Text::LabelPropertyIndex::TEXT_COLOR:
    {
      const Vector4& textColor = propertyValue.Get<Vector4>();
      if(mController->GetDefaultColor() != textColor)
      {
        mController->SetDefaultColor(textColor);
        mTextUpdateNeeded = true;

        // Trigger constraint always.
        if(DALI_LIKELY(mVisual))
        {
          Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount, true);
        }
      }
      break;
    }
    default:
    {
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
