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
#include <dali-ui-foundation/public-api/layout.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layout-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace UI
{

Layout::Layout()
{
}

Layout Layout::New()
{
  return Integration::LayoutImpl::New();
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
  if (&handle != this)
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
  return Toolkit::Control::DownCast<Layout, Integration::LayoutImpl>(handle);
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

void Layout::SetClipsToBounds(bool clips)
{
  Integration::GetImpl(*this).SetClipsToBounds(clips);
}

bool Layout::GetClipsToBounds() const
{
  return Integration::GetImpl(*this).GetClipsToBounds();
}

// =============================================================================
// Child Management API
// =============================================================================

void Layout::AddView(View view)
{
  Integration::GetImpl(*this).AddView(view);
}

void Layout::AddView(View view, uint32_t index)
{
  Integration::GetImpl(*this).AddView(view, index);
}

void Layout::RemoveView(View view)
{
  Integration::GetImpl(*this).RemoveView(view);
}

void Layout::RemoveViewAt(uint32_t index)
{
  Integration::GetImpl(*this).RemoveViewAt(index);
}

void Layout::RemoveAllViews()
{
  Integration::GetImpl(*this).RemoveAllViews();
}

uint32_t Layout::GetChildCount() const
{
  return Integration::GetImpl(*this).GetChildCount();
}

View Layout::GetChildAt(uint32_t index) const
{
  return Integration::GetImpl(*this).GetChildAt(index);
}

int32_t Layout::IndexOfChild(View view) const
{
  return Integration::GetImpl(*this).IndexOfChild(view);
}

Layout& Layout::Contents(std::initializer_list<View> children)
{
  for (const auto& child : children)
  {
    AddView(child);
  }
  return *this;
}

} // namespace UI
} // namespace Dali
