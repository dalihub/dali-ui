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
  if(&handle != this)
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
  return Ui::View::DownCast<AbsoluteLayout, Integration::AbsoluteLayoutImpl>(handle);
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

} // namespace Ui
} // namespace Dali
