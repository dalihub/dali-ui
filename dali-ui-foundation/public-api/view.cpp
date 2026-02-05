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

// CLASS HEADER
#include <dali-ui-foundation/public-api/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/layout.h>

namespace Dali
{

namespace UI
{

View::View()
{
}

View View::New()
{
  return Integration::ViewImpl::New();
}

View::View(const View& view)
  : Toolkit::Control(view)
{
}

View::View(View&& rhs) noexcept
  : Toolkit::Control(std::move(rhs))
{
}

View::~View()
{
}

View View::DownCast(BaseHandle handle)
{
  return Toolkit::Control::DownCast<View, Integration::ViewImpl>(handle);
}

View::View(Integration::ViewImpl& implementation)
  : Control(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
  : Control(internal)
{
  VerifyCustomActorPointer<Integration::ViewImpl>(internal);
}

// =============================================================================
// API (size, position, parent origin, pivot)
// =============================================================================

float View::GetSizeWidth() const
{
  return Integration::GetImpl(*this).GetSizeWidth();
}

void View::SetSizeWidth(float width)
{
  Integration::GetImpl(*this).SetSizeWidth(width);
}

float View::GetSizeHeight() const
{
  return Integration::GetImpl(*this).GetSizeHeight();
}

void View::SetSizeHeight(float height)
{
  Integration::GetImpl(*this).SetSizeHeight(height);
}

float View::GetPositionX() const
{
  return Integration::GetImpl(*this).GetPositionX();
}

void View::SetPositionX(float x)
{
  Integration::GetImpl(*this).SetPositionX(x);
}

float View::GetPositionY() const
{
  return Integration::GetImpl(*this).GetPositionY();
}

void View::SetPositionY(float y)
{
  Integration::GetImpl(*this).SetPositionY(y);
}

Vector3 View::GetParentOrigin() const
{
  return Integration::GetImpl(*this).GetParentOrigin();
}

void View::SetParentOrigin(const Vector3& point)
{
  Integration::GetImpl(*this).SetParentOrigin(point);
}

Vector3 View::GetPivotPoint() const
{
  return Integration::GetImpl(*this).GetPivotPoint();
}

void View::SetPivotPoint(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivotPoint(point);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize View::Measure(float widthConstraint, float heightConstraint)
{
  return Integration::GetImpl(*this).Measure(widthConstraint, heightConstraint);
}

MeasuredSize View::Arrange(const LayoutRect& bounds)
{
  return Integration::GetImpl(*this).Arrange(bounds);
}

void View::InvalidateMeasure()
{
  Integration::GetImpl(*this).InvalidateMeasure();
}

void View::InvalidateArrange()
{
  Integration::GetImpl(*this).InvalidateArrange();
}

MeasuredSize View::GetDesiredSize() const
{
  return Integration::GetImpl(*this).GetDesiredSize();
}

bool View::IsMeasureValid() const
{
  return Integration::GetImpl(*this).IsMeasureValid();
}

bool View::IsArrangeValid() const
{
  return Integration::GetImpl(*this).IsArrangeValid();
}

// =============================================================================
// Layout size API (LayoutWidth / LayoutHeight)
// =============================================================================

void View::SetLayoutWidth(float width)
{
  Integration::GetImpl(*this).SetLayoutWidth(width);
}

float View::GetLayoutWidth() const
{
  return Integration::GetImpl(*this).GetLayoutWidth();
}

void View::SetLayoutHeight(float height)
{
  Integration::GetImpl(*this).SetLayoutHeight(height);
}

float View::GetLayoutHeight() const
{
  return Integration::GetImpl(*this).GetLayoutHeight();
}

void View::SetMinimumWidth(float width)
{
  Integration::GetImpl(*this).SetMinimumWidth(width);
}

float View::GetMinimumWidth() const
{
  return Integration::GetImpl(*this).GetMinimumWidth();
}

void View::SetMinimumHeight(float height)
{
  Integration::GetImpl(*this).SetMinimumHeight(height);
}

float View::GetMinimumHeight() const
{
  return Integration::GetImpl(*this).GetMinimumHeight();
}

void View::SetMaximumWidth(float width)
{
  Integration::GetImpl(*this).SetMaximumWidth(width);
}

float View::GetMaximumWidth() const
{
  return Integration::GetImpl(*this).GetMaximumWidth();
}

void View::SetMaximumHeight(float height)
{
  Integration::GetImpl(*this).SetMaximumHeight(height);
}

float View::GetMaximumHeight() const
{
  return Integration::GetImpl(*this).GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

void View::SetViewMargin(const Extents& margin)
{
  Integration::GetImpl(*this).SetViewMargin(margin);
}

Extents View::GetViewMargin() const
{
  return Integration::GetImpl(*this).GetViewMargin();
}

void View::SetViewPadding(const Extents& padding)
{
  Integration::GetImpl(*this).SetViewPadding(padding);
}

Extents View::GetViewPadding() const
{
  return Integration::GetImpl(*this).GetViewPadding();
}

void View::SetViewVisibility(ViewVisibility visibility)
{
  Integration::GetImpl(*this).SetViewVisibility(visibility);
}

ViewVisibility View::GetViewVisibility() const
{
  return Integration::GetImpl(*this).GetViewVisibility();
}

void View::SetHorizontalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetHorizontalAlignment(alignment);
}

LayoutAlignment View::GetHorizontalAlignment() const
{
  return Integration::GetImpl(*this).GetHorizontalAlignment();
}

void View::SetVerticalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetVerticalAlignment(alignment);
}

LayoutAlignment View::GetVerticalAlignment() const
{
  return Integration::GetImpl(*this).GetVerticalAlignment();
}

void View::SetBackgroundColor(const Vector4& color)
{
  Toolkit::Control::SetBackgroundColor(color);
}

} // namespace UI
} // namespace Dali
