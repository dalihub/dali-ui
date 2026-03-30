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
#include <dali-ui-foundation/integration-api/layouts/flex-layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>

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
  return FlexLayout::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::FlexLayoutImpl, Ui::Integration::LayoutImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

FlexLayoutImplPtr FlexLayoutImpl::New()
{
  return FlexLayoutImplPtr(new FlexLayoutImpl());
}

FlexLayoutImpl::FlexLayoutImpl()
: LayoutImpl()
{
}

FlexLayoutImpl::~FlexLayoutImpl()
{
}

void FlexLayoutImpl::OnInitialize()
{
  LayoutImpl::OnInitialize();
  SetLayoutManager(
    new FlexLayoutManager(FlexDirection::ROW, FlexWrap::NO_WRAP, FlexJustify::FLEX_START, FlexAlign::STRETCH, FlexAlign::STRETCH));
}

void FlexLayoutImpl::SetDirection(FlexDirection direction)
{
  auto* manager = static_cast<FlexLayoutManager*>(GetLayoutManager());
  if(manager->GetDirection() != direction)
  {
    manager->SetDirection(direction);
    InvalidateMeasure();
  }
}

FlexDirection FlexLayoutImpl::GetDirection() const
{
  return static_cast<FlexLayoutManager*>(GetLayoutManager())->GetDirection();
}

void FlexLayoutImpl::SetWrap(FlexWrap wrap)
{
  auto* manager = static_cast<FlexLayoutManager*>(GetLayoutManager());
  if(manager->GetWrap() != wrap)
  {
    manager->SetWrap(wrap);
    InvalidateMeasure();
  }
}

FlexWrap FlexLayoutImpl::GetWrap() const
{
  return static_cast<FlexLayoutManager*>(GetLayoutManager())->GetWrap();
}

void FlexLayoutImpl::SetJustifyContent(FlexJustify justify)
{
  auto* manager = static_cast<FlexLayoutManager*>(GetLayoutManager());
  if(manager->GetJustifyContent() != justify)
  {
    manager->SetJustifyContent(justify);
    InvalidateArrange();
  }
}

FlexJustify FlexLayoutImpl::GetJustifyContent() const
{
  return static_cast<FlexLayoutManager*>(GetLayoutManager())->GetJustifyContent();
}

void FlexLayoutImpl::SetAlignItems(FlexAlign align)
{
  auto* manager = static_cast<FlexLayoutManager*>(GetLayoutManager());
  if(manager->GetAlignItems() != align)
  {
    manager->SetAlignItems(align);
    InvalidateArrange();
  }
}

FlexAlign FlexLayoutImpl::GetAlignItems() const
{
  return static_cast<FlexLayoutManager*>(GetLayoutManager())->GetAlignItems();
}

void FlexLayoutImpl::SetAlignContent(FlexAlign align)
{
  auto* manager = static_cast<FlexLayoutManager*>(GetLayoutManager());
  if(manager->GetAlignContent() != align)
  {
    manager->SetAlignContent(align);
    InvalidateArrange();
  }
}

FlexAlign FlexLayoutImpl::GetAlignContent() const
{
  return static_cast<FlexLayoutManager*>(GetLayoutManager())->GetAlignContent();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
