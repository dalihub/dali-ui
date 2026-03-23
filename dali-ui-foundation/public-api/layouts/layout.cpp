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
#include <dali-ui-foundation/public-api/layouts/layout.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace Ui
{

Layout::Layout()
{
}

Layout Layout::New()
{
  Integration::LayoutImplPtr impl = Integration::LayoutImpl::New();

  // Pass ownership to handle
  Layout handle(*impl);

  // Second-phase initialization
  impl->Initialize();

  return handle;
}

Layout::Layout(const Layout& layout)
: View(layout)
{
}

Layout::Layout(Layout&& rhs) noexcept
: View(std::move(rhs))
{
}

Layout::~Layout()
{
}

Layout& Layout::operator=(const Layout& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

Layout& Layout::operator=(Layout&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

Layout Layout::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<Layout, Integration::LayoutImpl>(handle);
}

Layout::Layout(Integration::LayoutImpl& implementation)
: View(implementation)
{
}

Layout::Layout(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::LayoutImpl>(internal);
}

// =============================================================================
// Layout Properties API
// =============================================================================

} // namespace Ui
} // namespace Dali
