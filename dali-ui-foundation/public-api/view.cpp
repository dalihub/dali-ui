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
// Properties
// =============================================================================

View& View::SetSizeWidth(float width)
{
  Integration::GetImpl(*this).SetSizeWidth(width);
  return *this;
}

float View::GetSizeWidth() const
{
  return Integration::GetImpl(*this).GetSizeWidth();
}

View& View::SetSizeHeight(float height)
{
  Integration::GetImpl(*this).SetSizeHeight(height);
  return *this;
}

float View::GetSizeHeight() const
{
  return Integration::GetImpl(*this).GetSizeHeight();
}

View& View::SetPositionX(float x)
{
  Integration::GetImpl(*this).SetPositionX(x);
  return *this;
}

float View::GetPositionX() const
{
  return Integration::GetImpl(*this).GetPositionX();
}

View& View::SetPositionY(float y)
{
  Integration::GetImpl(*this).SetPositionY(y);
  return *this;
}

float View::GetPositionY() const
{
  return Integration::GetImpl(*this).GetPositionY();
}

View& View::SetParentOrigin(const Vector3& point)
{
  Integration::GetImpl(*this).SetParentOrigin(point);
  return *this;
}

Vector3 View::GetParentOrigin() const
{
  return Integration::GetImpl(*this).GetParentOrigin();
}

View& View::SetPivotPoint(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivotPoint(point);
  return *this;
}

Vector3 View::GetPivotPoint() const
{
  return Integration::GetImpl(*this).GetPivotPoint();
}

View& View::SetLayoutWidth(float width)
{
  Integration::GetImpl(*this).SetLayoutWidth(width);
  return *this;
}

float View::GetLayoutWidth() const
{
  return Integration::GetImpl(*this).GetLayoutWidth();
}

View& View::SetLayoutHeight(float height)
{
  Integration::GetImpl(*this).SetLayoutHeight(height);
  return *this;
}

float View::GetLayoutHeight() const
{
  return Integration::GetImpl(*this).GetLayoutHeight();
}

View& View::SetMinimumWidth(float width)
{
  Integration::GetImpl(*this).SetMinimumWidth(width);
  return *this;
}

float View::GetMinimumWidth() const
{
  return Integration::GetImpl(*this).GetMinimumWidth();
}

View& View::SetMinimumHeight(float height)
{
  Integration::GetImpl(*this).SetMinimumHeight(height);
  return *this;
}

float View::GetMinimumHeight() const
{
  return Integration::GetImpl(*this).GetMinimumHeight();
}

View& View::SetMaximumWidth(float width)
{
  Integration::GetImpl(*this).SetMaximumWidth(width);
  return *this;
}

float View::GetMaximumWidth() const
{
  return Integration::GetImpl(*this).GetMaximumWidth();
}

View& View::SetMaximumHeight(float height)
{
  Integration::GetImpl(*this).SetMaximumHeight(height);
  return *this;
}

float View::GetMaximumHeight() const
{
  return Integration::GetImpl(*this).GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

View& View::SetViewMargin(const Extents& margin)
{
  Integration::GetImpl(*this).SetViewMargin(margin);
  return *this;
}

Extents View::GetViewMargin() const
{
  return Integration::GetImpl(*this).GetViewMargin();
}

View& View::SetViewPadding(const Extents& padding)
{
  Integration::GetImpl(*this).SetViewPadding(padding);
  return *this;
}

Extents View::GetViewPadding() const
{
  return Integration::GetImpl(*this).GetViewPadding();
}

View& View::SetHorizontalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetHorizontalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetHorizontalAlignment() const
{
  return Integration::GetImpl(*this).GetHorizontalAlignment();
}

View& View::SetVerticalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetVerticalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetVerticalAlignment() const
{
  return Integration::GetImpl(*this).GetVerticalAlignment();
}

} // namespace UI

} // namespace Dali
