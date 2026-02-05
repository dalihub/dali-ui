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
#include <dali-ui-foundation/public-api/absolute-layout.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/absolute-layout-impl.h>

namespace Dali
{
namespace UI
{

AbsoluteLayout::AbsoluteLayout()
{
}

AbsoluteLayout::AbsoluteLayout(const AbsoluteLayout& layout)
  : Layout(layout)
{
}

AbsoluteLayout::AbsoluteLayout(AbsoluteLayout&& rhs) noexcept
  : Layout(std::move(rhs))
{
}

AbsoluteLayout::~AbsoluteLayout()
{
}

AbsoluteLayout& AbsoluteLayout::operator=(const AbsoluteLayout& handle)
{
  if (&handle != this)
  {
    Layout::operator=(handle);
  }
  return *this;
}

AbsoluteLayout& AbsoluteLayout::operator=(AbsoluteLayout&& rhs) noexcept
{
  Layout::operator=(std::move(rhs));
  return *this;
}

AbsoluteLayout AbsoluteLayout::New()
{
  return Integration::AbsoluteLayoutImpl::New();
}

AbsoluteLayout AbsoluteLayout::DownCast(BaseHandle handle)
{
  return Toolkit::Control::DownCast<AbsoluteLayout, Integration::AbsoluteLayoutImpl>(handle);
}

AbsoluteLayout::AbsoluteLayout(Integration::AbsoluteLayoutImpl& implementation)
  : Layout(implementation)
{
}

AbsoluteLayout::AbsoluteLayout(Dali::Internal::CustomActor* internal)
  : Layout(internal)
{
  VerifyCustomActorPointer<Integration::AbsoluteLayoutImpl>(internal);
}

// Static attached property methods
void AbsoluteLayout::SetLayoutBounds(View view, const LayoutRect& bounds)
{
  if (view)
  {
    view.RegisterProperty("absoluteLayoutX", bounds.x);
    view.RegisterProperty("absoluteLayoutY", bounds.y);
    view.RegisterProperty("absoluteLayoutWidth", bounds.width);
    view.RegisterProperty("absoluteLayoutHeight", bounds.height);
  }
}

LayoutRect AbsoluteLayout::GetLayoutBounds(View view)
{
  LayoutRect bounds(0.0f, 0.0f, -1.0f, -1.0f);
  if (view)
  {
    auto xIdx = view.GetPropertyIndex("absoluteLayoutX");
    auto yIdx = view.GetPropertyIndex("absoluteLayoutY");
    auto wIdx = view.GetPropertyIndex("absoluteLayoutWidth");
    auto hIdx = view.GetPropertyIndex("absoluteLayoutHeight");
    if (xIdx != Dali::Property::INVALID_INDEX)
    {
      bounds.x = view.GetProperty<float>(xIdx);
    }
    if (yIdx != Dali::Property::INVALID_INDEX)
    {
      bounds.y = view.GetProperty<float>(yIdx);
    }
    if (wIdx != Dali::Property::INVALID_INDEX)
    {
      bounds.width = view.GetProperty<float>(wIdx);
    }
    if (hIdx != Dali::Property::INVALID_INDEX)
    {
      bounds.height = view.GetProperty<float>(hIdx);
    }
  }
  return bounds;
}

void AbsoluteLayout::SetLayoutFlags(View view, AbsoluteLayoutFlags flags)
{
  if (view)
  {
    view.RegisterProperty("absoluteLayoutFlags", static_cast<int>(flags));
  }
}

AbsoluteLayoutFlags AbsoluteLayout::GetLayoutFlags(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("absoluteLayoutFlags");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<AbsoluteLayoutFlags>(view.GetProperty<int>(index));
    }
  }
  return AbsoluteLayoutFlags::None;
}

} // namespace UI
} // namespace Dali
