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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>

#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/view-depth-index-ranges.h>
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

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(LabelImpl, ViewImpl, Create)

DALI_ANIMATABLE_PROPERTY_REGISTRATION_WITH_DEFAULT(Ui::Integration, LabelImpl, "textColor", Color::BLACK, TEXT_COLOR)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Ui::Integration, LabelImpl, "textColorRed", TEXT_COLOR_RED, TEXT_COLOR,
                                                0)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Ui::Integration, LabelImpl, "textColorGreen", TEXT_COLOR_GREEN,
                                                TEXT_COLOR, 1)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Ui::Integration, LabelImpl, "textColorBlue", TEXT_COLOR_BLUE,
                                                TEXT_COLOR, 2)
DALI_ANIMATABLE_PROPERTY_COMPONENT_REGISTRATION(Ui::Integration, LabelImpl, "textColorAlpha", TEXT_COLOR_ALPHA,
                                                TEXT_COLOR, 3)

DALI_TYPE_REGISTRATION_END()

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
  mIsTouchDown(false),
  mHasAnchors(false)
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
  if(!Equals(mController->GetDefaultFontSize(Text::Controller::PIXEL_SIZE), fontSize))
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
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &LabelImpl::SetTextColorInternal);
  SetTextColorInternal(color.Resolve());
}

UiColor LabelImpl::GetTextColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &LabelImpl::SetTextColorInternal, outColor))
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
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &LabelImpl::SetAnchorColorInternal);
  SetAnchorColorInternal(color.Resolve());
}

UiColor LabelImpl::GetAnchorColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &LabelImpl::SetAnchorColorInternal, outColor))
  {
    return outColor;
  }
  return mController->GetAnchorColor();
}

void LabelImpl::SetAnchorClickedColor(const UiColor& color)
{
  UiColorManager::Get().UpdateBinding(color, View::DownCast(Self()), this, &LabelImpl::SetAnchorClickedColorInternal);
  SetAnchorClickedColorInternal(color.Resolve());
}

UiColor LabelImpl::GetAnchorClickedColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(View::DownCast(Self()), this, &LabelImpl::SetAnchorClickedColorInternal, outColor))
  {
    return outColor;
  }
  return mController->GetAnchorClickedColor();
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
  Internal::TextVisual::SetAnimatableTextColorProperty(mVisual, LabelImpl::Property::TEXT_COLOR);
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
    alignmentOffset.y = (contentSize.y - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[static_cast<int>(mController->GetVerticalAlignment())];

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

// TODO: If the implementation in View is moved to ViewImpl, this part will need to be updated accordingly.
void LabelImpl::OnAnimateAnimatableProperty(Animation& animation, Dali::Property::Index index, Animation::State state)
{
  if(DALI_LIKELY(mVisual) && index == LabelImpl::Property::TEXT_COLOR)
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

// TODO: If the implementation in View is moved to ViewImpl, this part will need to be updated accordingly.
void LabelImpl::OnConstraintAnimatableProperty(Constraint& constraint, Dali::Property::Index index, bool applied)
{
  if(DALI_LIKELY(mVisual) && index == LabelImpl::Property::TEXT_COLOR)
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

// =============================================================================
// UiColorManager
// =============================================================================
void LabelImpl::SetTextColorInternal(const Vector4& color)
{
  if(mController->GetDefaultColor() != color)
  {
    Self().SetProperty(LabelImpl::Property::TEXT_COLOR, color);
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

} // namespace Integration

} // namespace Ui

} // namespace Dali
