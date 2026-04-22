/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
// =============================================================================
// Public API
// =============================================================================

View VisualBase::GetOwner() const
{
  return GetImplementation(*this).GetOwner();
}

Dali::Ui::Visual::ContainerRangeType VisualBase::GetContainerRangeType() const
{
  auto rangeType = GetImplementation(*this).GetInternalContainerRangeType();

  if(rangeType == Dali::Ui::DevelVisual::InternalContainerRangeType::INVALID ||
     static_cast<int>(rangeType) >= static_cast<int>(Dali::Ui::Visual::ContainerRangeType::MAX_COUNT)) ///< Use Internal::VisualBase::GetInternalContainerRangeType() if we are internal API
  {
    return Dali::Ui::Visual::ContainerRangeType::INVALID;
  }

  return static_cast<Dali::Ui::Visual::ContainerRangeType>(static_cast<int>(rangeType));
}

void VisualBase::Detach()
{
  GetImplementation(*this).DetachFromContainer();
}

void VisualBase::DoAction(Dali::Property::Index actionId, const Dali::Property::Value& attributes)
{
  GetImplementation(*this).DoAction(actionId, attributes);
}

Dali::Ui::Visual::Type VisualBase::GetVisualType() const
{
  return GetImplementation(*this).GetVisualType();
}

// =============================================================================
// Properties
// =============================================================================

Dali::Property::Value VisualBase::GetProperty(Dali::Property::Index index) const
{
  return GetImplementation(*this).GetProperty(index);
}

void VisualBase::SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue)
{
  GetImplementation(*this).SetProperty(index, propertyValue);
}

const Dali::String& VisualBase::GetName() const
{
  return GetImplementation(*this).GetName();
}

VisualBase& VisualBase::SetName(const Dali::String& name)
{
  GetImplementation(*this).SetName(name);
  return *this;
}

UiColor VisualBase::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

VisualBase& VisualBase::SetColor(const UiColor& color)
{
  GetImplementation(*this).SetColor(color);
  return *this;
}

// Transform relative properties
float VisualBase::GetOffsetX() const
{
  return GetImplementation(*this).GetOffsetX();
}

VisualBase& VisualBase::SetOffsetX(float x)
{
  GetImplementation(*this).SetOffsetX(x);
  return *this;
}

float VisualBase::GetOffsetY() const
{
  return GetImplementation(*this).GetOffsetY();
}

VisualBase& VisualBase::SetOffsetY(float y)
{
  GetImplementation(*this).SetOffsetY(y);
  return *this;
}

float VisualBase::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

VisualBase& VisualBase::SetWidth(float width)
{
  GetImplementation(*this).SetWidth(width);
  return *this;
}

float VisualBase::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

VisualBase& VisualBase::SetHeight(float height)
{
  GetImplementation(*this).SetHeight(height);
  return *this;
}

Dali::Ui::Visual::Transform::ProportionFlags VisualBase::GetProportionFlags() const
{
  return GetImplementation(*this).GetProportionFlags();
}

VisualBase& VisualBase::SetProportionFlags(Dali::Ui::Visual::Transform::ProportionFlags flags)
{
  GetImplementation(*this).SetProportionFlags(flags);
  return *this;
}

float VisualBase::GetExtraWidth() const
{
  return GetImplementation(*this).GetExtraWidth();
}

VisualBase& VisualBase::SetExtraWidth(float extraWidth)
{
  GetImplementation(*this).SetExtraWidth(extraWidth);
  return *this;
}

float VisualBase::GetExtraHeight() const
{
  return GetImplementation(*this).GetExtraHeight();
}

VisualBase& VisualBase::SetExtraHeight(float extraHeight)
{
  GetImplementation(*this).SetExtraHeight(extraHeight);
  return *this;
}

Align::Type VisualBase::GetOrigin() const
{
  return GetImplementation(*this).GetOrigin();
}

VisualBase& VisualBase::SetOrigin(Align::Type origin)
{
  GetImplementation(*this).SetOrigin(origin);
  return *this;
}

Align::Type VisualBase::GetPivot() const
{
  return GetImplementation(*this).GetPivot();
}

VisualBase& VisualBase::SetPivot(Align::Type pivot)
{
  GetImplementation(*this).SetPivot(pivot);
  return *this;
}

// Decorated properties (CornerRadius / Borderline)

Vector4 VisualBase::GetCornerRadius() const
{
  return GetProperty<Vector4>(DevelVisual::Property::CORNER_RADIUS);
}

VisualBase& VisualBase::SetCornerRadius(float radius)
{
  SetProperty(DevelVisual::Property::CORNER_RADIUS, Vector4(radius, radius, radius, radius));
  return *this;
}

VisualBase& VisualBase::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  SetProperty(DevelVisual::Property::CORNER_RADIUS, Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

VisualBase& VisualBase::SetCornerRadius(const Vector4& radius)
{
  SetProperty(DevelVisual::Property::CORNER_RADIUS, radius);
  return *this;
}

CornerRadiusPolicy VisualBase::GetCornerRadiusPolicy() const
{
  return GetProperty<CornerRadiusPolicy>(DevelVisual::Property::CORNER_RADIUS_POLICY);
}

VisualBase& VisualBase::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  SetProperty(DevelVisual::Property::CORNER_RADIUS_POLICY, policy);
  return *this;
}

VisualBase& VisualBase::SetCornerRadiusPolicyRelative()
{
  SetProperty(DevelVisual::Property::CORNER_RADIUS_POLICY, CornerRadiusPolicy::RELATIVE);
  return *this;
}

bool VisualBase::IsCornerRadiusPolicyRelative() const
{
  return GetCornerRadiusPolicy() == CornerRadiusPolicy::RELATIVE;
}

Vector4 VisualBase::GetCornerSquareness() const
{
  return GetProperty<Vector4>(DevelVisual::Property::CORNER_SQUARENESS);
}

VisualBase& VisualBase::SetCornerSquareness(float squareness)
{
  SetProperty(DevelVisual::Property::CORNER_SQUARENESS, Vector4(squareness, squareness, squareness, squareness));
  return *this;
}

VisualBase& VisualBase::SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  SetProperty(DevelVisual::Property::CORNER_SQUARENESS, Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

VisualBase& VisualBase::SetCornerSquareness(const Vector4& squareness)
{
  SetProperty(DevelVisual::Property::CORNER_SQUARENESS, squareness);
  return *this;
}

float VisualBase::GetBorderlineWidth() const
{
  return GetProperty<float>(DevelVisual::Property::BORDERLINE_WIDTH);
}

VisualBase& VisualBase::SetBorderlineWidth(float width)
{
  SetProperty(DevelVisual::Property::BORDERLINE_WIDTH, width);
  return *this;
}

UiColor VisualBase::GetBorderlineColor() const
{
  return GetImplementation(*this).GetBorderlineColor();
}

VisualBase& VisualBase::SetBorderlineColor(const UiColor& color)
{
  GetImplementation(*this).SetBorderlineColor(color);
  return *this;
}

float VisualBase::GetBorderlineOffset() const
{
  return GetProperty<float>(DevelVisual::Property::BORDERLINE_OFFSET);
}

VisualBase& VisualBase::SetBorderlineOffset(float offset)
{
  SetProperty(DevelVisual::Property::BORDERLINE_OFFSET, offset);
  return *this;
}

// =============================================================================
// Sibling Order Change API
// =============================================================================

uint32_t VisualBase::GetSiblingOrder() const
{
  return GetImplementation(*this).GetSiblingOrder();
}

void VisualBase::SetSiblingOrder(uint32_t siblingOrder)
{
  GetImplementation(*this).SetSiblingOrder(siblingOrder);
}

void VisualBase::Raise()
{
  GetImplementation(*this).Raise();
}

void VisualBase::Lower()
{
  GetImplementation(*this).Lower();
}

void VisualBase::RaiseToTop()
{
  GetImplementation(*this).RaiseToTop();
}

void VisualBase::LowerToBottom()
{
  GetImplementation(*this).LowerToBottom();
}

void VisualBase::RaiseAbove(Dali::Ui::VisualBase target)
{
  GetImplementation(*this).RaiseAbove(GetImplementation(target));
}

void VisualBase::LowerBelow(Dali::Ui::VisualBase target)
{
  GetImplementation(*this).LowerBelow(GetImplementation(target));
}

// =============================================================================
// Internal API
// =============================================================================

VisualBase::VisualBase(Dali::Ui::Internal::VisualBaseImpl* object)
: BaseHandle(object)
{
}

} // namespace Ui
} // namespace Dali
