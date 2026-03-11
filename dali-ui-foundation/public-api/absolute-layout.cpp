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
namespace Ui
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
  Integration::AbsoluteLayoutImplPtr impl = Integration::AbsoluteLayoutImpl::New();

  // Pass ownership to handle
  AbsoluteLayout handle(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

AbsoluteLayout AbsoluteLayout::DownCast(BaseHandle handle)
{
  return Ui::Control::DownCast<AbsoluteLayout, Integration::AbsoluteLayoutImpl>(handle);
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

namespace
{
/**
 * @brief Helper to register or update a float attached property.
 */
void SetFloatProperty(View view, const std::string& name, float value)
{
  Property::Index index = view.GetPropertyIndex(name);
  if (index == Dali::Property::INVALID_INDEX)
  {
    view.RegisterProperty(name, value);
  }
  else
  {
    view.SetProperty(index, value);
  }
}

/**
 * @brief Helper to register or update an int attached property.
 */
void SetIntProperty(View view, const std::string& name, int value)
{
  Property::Index index = view.GetPropertyIndex(name);
  if (index == Dali::Property::INVALID_INDEX)
  {
    view.RegisterProperty(name, value);
  }
  else
  {
    view.SetProperty(index, value);
  }
}
} // namespace

// Static attached property methods
void AbsoluteLayout::SetLayoutBounds(View view, const LayoutRect& bounds)
{
  if (view)
  {
    SetFloatProperty(view, "absoluteLayoutX", bounds.x);
    SetFloatProperty(view, "absoluteLayoutY", bounds.y);
    SetFloatProperty(view, "absoluteLayoutWidth", bounds.width);
    SetFloatProperty(view, "absoluteLayoutHeight", bounds.height);
    view.InvalidateMeasure();
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
    SetIntProperty(view, "absoluteLayoutFlags", static_cast<int>(flags));
    view.InvalidateMeasure();
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
  return AbsoluteLayoutFlags::NONE;
}

} // namespace Ui
} // namespace Dali
