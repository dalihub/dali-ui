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
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali/devel-api/adaptor-framework/image-loading.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-elements/integration-api/label-impl.h>

#include <dali-ui-foundation/devel-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/controls/control-devel.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>

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
 * @brief Lookup table that converts Text::VerticalAlignment values
 *        to a normalized vertical alignment factor.
 */
const float VERTICAL_ALIGNMENT_TABLE[Text::VerticalAlignment::BOTTOM + 1] = {
  0.0f, // VerticalAlignment::TOP
  0.5f, // VerticalAlignment::CENTER
  1.0f  // VerticalAlignment::BOTTOM
};

} // namespace

LabelImplPtr LabelImpl::New()
{
  return LabelImplPtr(new LabelImpl());
}

LabelImpl::LabelImpl()
: ViewImpl(),
  mTextColorAnimatedCount(0),
  mTextUpdateNeeded(false)
{
}

LabelImpl::~LabelImpl()
{
}

// Properties
void LabelImpl::SetText(std::string text)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), text.c_str());

  mController->SetText(text);
  mTextUpdateNeeded = true;
  RequestTextRelayout();
}

std::string LabelImpl::GetText() const
{
  std::string text;
  mController->GetText(text);
  return text;
}

void LabelImpl::SetFontFamily(std::string fontFamily)
{
  DALI_LOG_RELEASE_INFO("[%p] %s\n", mController.Get(), fontFamily.c_str());

  mController->SetDefaultFontFamily(fontFamily);
}
std::string LabelImpl::GetFontFamily() const
{
  return mController->GetDefaultFontFamily();
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

void LabelImpl::SetTextColor(const Vector4& color)
{
  DALI_LOG_RELEASE_INFO("[%p] %.2f,%.2f,%.2f,%.2f\n", mController.Get(), color.r, color.g, color.b, color.a);

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

const Vector4& LabelImpl::GetTextColor() const
{
  return mController->GetDefaultColor();
}

void LabelImpl::SetHorizontalTextAlignment(TextAlignment alignment)
{
  // TODO: Replace Text::HorizontalAlignment/Text::VerticalAlignment with TextAlignment internally.
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);

  Text::HorizontalAlignment::Type type;
  switch(alignment)
  {
    case TextAlignment::START:
    {
      type = Text::HorizontalAlignment::BEGIN;
      break;
    }
    case TextAlignment::CENTER:
    {
      type = Text::HorizontalAlignment::CENTER;
      break;
    }
    case TextAlignment::END:
    {
      type = Text::HorizontalAlignment::END;
      break;
    }
    default:
    {
      type = Text::HorizontalAlignment::BEGIN;
      break;
    }
  }
  mController->SetHorizontalAlignment(type);
}

TextAlignment LabelImpl::GetHorizontalTextAlignment() const
{
  // TODO: Replace Text::HorizontalAlignment/Text::VerticalAlignment with TextAlignment internally.
  TextAlignment alignment;
  switch(mController->GetHorizontalAlignment())
  {
    case Text::HorizontalAlignment::BEGIN:
    {
      alignment = TextAlignment::START;
      break;
    }
    case Text::HorizontalAlignment::CENTER:
    {
      alignment = TextAlignment::CENTER;
      break;
    }
    case Text::HorizontalAlignment::END:
    {
      alignment = TextAlignment::END;
      break;
    }
    default:
    {
      alignment = TextAlignment::START;
      break;
    }
  }
  return alignment;
}

void LabelImpl::SetVerticalTextAlignment(TextAlignment alignment)
{
  // TODO: Replace Text::HorizontalAlignment/Text::VerticalAlignment with TextAlignment internally.
  DALI_LOG_RELEASE_INFO("[%p] %d\n", mController.Get(), alignment);

  Text::VerticalAlignment::Type type;
  switch(alignment)
  {
    case TextAlignment::START:
    {
      type = Text::VerticalAlignment::TOP;
      break;
    }
    case TextAlignment::CENTER:
    {
      type = Text::VerticalAlignment::CENTER;
      break;
    }
    case TextAlignment::END:
    {
      type = Text::VerticalAlignment::BOTTOM;
      break;
    }
    default:
    {
      type = Text::VerticalAlignment::TOP;
      break;
    }
  }
  mController->SetVerticalAlignment(type);
}

TextAlignment LabelImpl::GetVerticalTextAlignment() const
{
  // TODO: Replace Text::HorizontalAlignment/Text::VerticalAlignment with TextAlignment internally.
  TextAlignment alignment;
  switch(mController->GetVerticalAlignment())
  {
    case Text::VerticalAlignment::TOP:
    {
      alignment = TextAlignment::START;
      break;
    }
    case Text::VerticalAlignment::CENTER:
    {
      alignment = TextAlignment::CENTER;
      break;
    }
    case Text::VerticalAlignment::BOTTOM:
    {
      alignment = TextAlignment::END;
      break;
    }
    default:
    {
      alignment = TextAlignment::START;
      break;
    }
  }
  return alignment;
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
  DevelControl::RegisterVisual(*this, PROPERTY_REGISTRATION_START_INDEX, mVisual, DepthIndex::CONTENT);
  Internal::TextVisual::SetAnimatableTextColorProperty(mVisual, LabelImpl::Property::TEXT_COLOR);
  Internal::TextVisual::SetConstraintApplyAlways(mVisual, mTextColorAnimatedCount > 0);

  mController = Internal::TextVisual::GetController(mVisual);
  DALI_ASSERT_DEBUG(mController && "Invalid Text Controller")

  mController->SetControlInterface(this);

  // Use height-for-width negotiation by default
  self.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH);
  self.SetResizePolicy(ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT);

  // Enable the text ellipsis.
  mController->SetTextElideEnabled(true);

  Text::Layout::Engine& engine = mController->GetLayoutEngine();
  engine.SetCursorWidth(0u);
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
    alignmentOffset.y = (contentSize.y - layoutSize.y) * VERTICAL_ALIGNMENT_TABLE[mController->GetVerticalAlignment()];

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
  Extents padding = GetViewPadding();
  return mController->GetHeightForWidth(width - (padding.start + padding.end)) + padding.top + padding.bottom;
}

// TODO: If the implementation in Control is moved to ViewImpl, this part will need to be updated accordingly.
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
  Internal::Control::OnAnimateAnimatableProperty(animation, index, state);
}

// TODO: If the implementation in Control is moved to ViewImpl, this part will need to be updated accordingly.
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
  Internal::Control::OnConstraintAnimatableProperty(constraint, index, applied);
}

MeasuredSize LabelImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  DALI_LOG_RELEASE_INFO("[%p] widthConstraint:%f, heightConstraint:%f\n", mController.Get(), widthConstraint,
                        heightConstraint);

  const float layoutWidth  = GetLayoutWidth();
  const float layoutHeight = GetLayoutHeight();

  const float minWidth  = GetMinimumWidth();
  const float maxWidth  = GetMaximumWidth();
  const float minHeight = GetMinimumHeight();
  const float maxHeight = GetMaximumHeight();

  const Vector3 naturalSize = GetNaturalSize();

  float measuredWidth  = 0.0f;
  float measuredHeight = 0.0f;

  // Width
  if(layoutWidth > 0.0f)
  {
    measuredWidth = layoutWidth;
  }
  else if(layoutWidth == LayoutDimension::MatchParent)
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
  if(layoutHeight > 0.0f)
  {
    measuredHeight = layoutHeight;
  }
  else if(layoutHeight == LayoutDimension::MatchParent)
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

void LabelImpl::RequestTextRelayout()
{
  // Signal that a Relayout may be needed
  RelayoutRequest();
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
