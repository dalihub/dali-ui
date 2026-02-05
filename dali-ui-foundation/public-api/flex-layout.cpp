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
#include <dali-ui-foundation/public-api/flex-layout.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/flex-layout-impl.h>

namespace Dali
{
namespace UI
{

namespace
{
// Property indices for attached properties
const Property::Index FLEX_GROW_INDEX = 10000;
const Property::Index FLEX_SHRINK_INDEX = 10001;
const Property::Index FLEX_BASIS_INDEX = 10002;
const Property::Index ALIGN_SELF_INDEX = 10003;
} // namespace

FlexLayout::FlexLayout()
{
}

FlexLayout::FlexLayout(const FlexLayout& layout)
  : Layout(layout)
{
}

FlexLayout::FlexLayout(FlexLayout&& rhs) noexcept
  : Layout(std::move(rhs))
{
}

FlexLayout::~FlexLayout()
{
}

FlexLayout& FlexLayout::operator=(const FlexLayout& handle)
{
  if (&handle != this)
  {
    Layout::operator=(handle);
  }
  return *this;
}

FlexLayout& FlexLayout::operator=(FlexLayout&& rhs) noexcept
{
  Layout::operator=(std::move(rhs));
  return *this;
}

FlexLayout FlexLayout::New()
{
  return Integration::FlexLayoutImpl::New();
}

FlexLayout FlexLayout::DownCast(BaseHandle handle)
{
  return Toolkit::Control::DownCast<FlexLayout, Integration::FlexLayoutImpl>(handle);
}

FlexLayout::FlexLayout(Integration::FlexLayoutImpl& implementation)
  : Layout(implementation)
{
}

FlexLayout::FlexLayout(Dali::Internal::CustomActor* internal)
  : Layout(internal)
{
  VerifyCustomActorPointer<Integration::FlexLayoutImpl>(internal);
}

void FlexLayout::SetDirection(FlexDirection direction)
{
  Integration::GetImpl(*this).SetDirection(direction);
}

FlexDirection FlexLayout::GetDirection() const
{
  return Integration::GetImpl(*this).GetDirection();
}

void FlexLayout::SetWrap(FlexWrap wrap)
{
  Integration::GetImpl(*this).SetWrap(wrap);
}

FlexWrap FlexLayout::GetWrap() const
{
  return Integration::GetImpl(*this).GetWrap();
}

void FlexLayout::SetJustifyContent(FlexJustify justify)
{
  Integration::GetImpl(*this).SetJustifyContent(justify);
}

FlexJustify FlexLayout::GetJustifyContent() const
{
  return Integration::GetImpl(*this).GetJustifyContent();
}

void FlexLayout::SetAlignItems(FlexAlign align)
{
  Integration::GetImpl(*this).SetAlignItems(align);
}

FlexAlign FlexLayout::GetAlignItems() const
{
  return Integration::GetImpl(*this).GetAlignItems();
}

void FlexLayout::SetAlignContent(FlexAlign align)
{
  Integration::GetImpl(*this).SetAlignContent(align);
}

FlexAlign FlexLayout::GetAlignContent() const
{
  return Integration::GetImpl(*this).GetAlignContent();
}

// Static attached property methods
void FlexLayout::SetFlexGrow(View view, float grow)
{
  if (view)
  {
    view.RegisterProperty("flexGrow", grow);
    view.InvalidateMeasure();
  }
}

float FlexLayout::GetFlexGrow(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("flexGrow");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return view.GetProperty<float>(index);
    }
  }
  return 0.0f; // Default
}

void FlexLayout::SetFlexShrink(View view, float shrink)
{
  if (view)
  {
    view.RegisterProperty("flexShrink", shrink);
    view.InvalidateMeasure();
  }
}

float FlexLayout::GetFlexShrink(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("flexShrink");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return view.GetProperty<float>(index);
    }
  }
  return 1.0f; // Default
}

void FlexLayout::SetFlexBasis(View view, float basis)
{
  if (view)
  {
    view.RegisterProperty("flexBasis", basis);
  }
}

float FlexLayout::GetFlexBasis(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("flexBasis");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return view.GetProperty<float>(index);
    }
  }
  return LayoutDimension::WrapContent; // Default
}

void FlexLayout::SetAlignSelf(View view, FlexAlign align)
{
  if (view)
  {
    view.RegisterProperty("alignSelf", static_cast<int>(align));
  }
}

FlexAlign FlexLayout::GetAlignSelf(View view)
{
  if (view)
  {
    auto index = view.GetPropertyIndex("alignSelf");
    if (index != Dali::Property::INVALID_INDEX)
    {
      return static_cast<FlexAlign>(view.GetProperty<int>(index));
    }
  }
  return FlexAlign::Auto; // Default
}

} // namespace UI
} // namespace Dali
