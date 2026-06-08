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
#include <dali-ui-foundation/integration-api/layouts/stack-layout-impl.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

namespace Dali
{
namespace Ui
{

StackLayout::StackLayout()
{
}

StackLayout::StackLayout(const StackLayout& layout)
: Layout(layout)
{
}

StackLayout::StackLayout(StackLayout&& rhs) noexcept
: Layout(std::move(rhs))
{
}

StackLayout::~StackLayout()
{
}

StackLayout& StackLayout::operator=(const StackLayout& handle)
{
  if(&handle != this)
  {
    Layout::operator=(handle);
  }
  return *this;
}

StackLayout& StackLayout::operator=(StackLayout&& rhs) noexcept
{
  Layout::operator=(std::move(rhs));
  return *this;
}

StackLayout StackLayout::New(StackOrientation orientation)
{
  Integration::StackLayoutImplPtr impl = Integration::StackLayoutImpl::New(orientation);
  StackLayout                     handle(*impl);
  impl->Initialize();

  return handle;
}

StackLayout StackLayout::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<StackLayout, Integration::StackLayoutImpl>(handle);
}

StackLayout::StackLayout(Integration::StackLayoutImpl& implementation)
: Layout(implementation)
{
}

StackLayout::StackLayout(Dali::Internal::CustomActor* internal)
: Layout(internal)
{
  VerifyCustomActorPointer<Integration::StackLayoutImpl>(internal);
}

void StackLayout::SetOrientation(StackOrientation orientation)
{
  GetImpl(*this).SetOrientation(orientation);
}

StackOrientation StackLayout::GetOrientation() const
{
  return GetImpl(*this).GetOrientation();
}

void StackLayout::SetSpacing(float spacing)
{
  GetImpl(*this).SetSpacing(spacing);
}

float StackLayout::GetSpacing() const
{
  return GetImpl(*this).GetSpacing();
}

} // namespace Ui
} // namespace Dali
