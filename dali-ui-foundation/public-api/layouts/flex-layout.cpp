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
#include <dali-ui-foundation/public-api/layouts/flex-layout.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/flex-layout-impl.h>

namespace Dali
{
namespace Ui
{

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
  if(&handle != this)
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
  Integration::FlexLayoutImplPtr impl = Integration::FlexLayoutImpl::New();

  // Pass ownership to handle
  FlexLayout handle(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

FlexLayout FlexLayout::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<FlexLayout, Integration::FlexLayoutImpl>(handle);
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

FlexLayout& FlexLayout::SetDirection(FlexDirection direction)
{
  Integration::GetImpl(*this).SetDirection(direction);
  return *this;
}

FlexDirection FlexLayout::GetDirection() const
{
  return Integration::GetImpl(*this).GetDirection();
}

FlexLayout& FlexLayout::SetWrap(FlexWrap wrap)
{
  Integration::GetImpl(*this).SetWrap(wrap);
  return *this;
}

FlexWrap FlexLayout::GetWrap() const
{
  return Integration::GetImpl(*this).GetWrap();
}

FlexLayout& FlexLayout::SetJustifyContent(FlexJustify justify)
{
  Integration::GetImpl(*this).SetJustifyContent(justify);
  return *this;
}

FlexJustify FlexLayout::GetJustifyContent() const
{
  return Integration::GetImpl(*this).GetJustifyContent();
}

FlexLayout& FlexLayout::SetAlignItems(FlexAlign align)
{
  Integration::GetImpl(*this).SetAlignItems(align);
  return *this;
}

FlexAlign FlexLayout::GetAlignItems() const
{
  return Integration::GetImpl(*this).GetAlignItems();
}

FlexLayout& FlexLayout::SetAlignContent(FlexAlign align)
{
  Integration::GetImpl(*this).SetAlignContent(align);
  return *this;
}

FlexAlign FlexLayout::GetAlignContent() const
{
  return Integration::GetImpl(*this).GetAlignContent();
}

} // namespace Ui
} // namespace Dali
