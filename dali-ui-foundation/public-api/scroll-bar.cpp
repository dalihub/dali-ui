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
#include <dali-ui-foundation/integration-api/scroll-bar-impl.h>
#include <dali-ui-foundation/public-api/scroll-bar.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Ui
{

inline Integration::ScrollBarImpl& GetImpl(ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);
  Dali::RefObject& handle = scrollBar.GetImplementation();
  return static_cast<Integration::ScrollBarImpl&>(handle);
}

inline const Integration::ScrollBarImpl& GetImpl(const ScrollBar& scrollBar)
{
  DALI_ASSERT_ALWAYS(scrollBar);
  const Dali::RefObject& handle = scrollBar.GetImplementation();
  return static_cast<const Integration::ScrollBarImpl&>(handle);
}

ScrollBar::ScrollBar()
{
}

ScrollBar ScrollBar::New()
{
  Integration::ScrollBarImplPtr impl = Integration::ScrollBarImpl::New();

  ScrollBar scrollBar = ScrollBar(*impl);
  impl->Initialize();
  return scrollBar;
}

ScrollBar::ScrollBar(const ScrollBar& scrollBar)
: AbsoluteLayout(scrollBar)
{
}

ScrollBar::ScrollBar(ScrollBar&& rhs) noexcept
: AbsoluteLayout(std::move(rhs))
{
}

ScrollBar::~ScrollBar()
{
}

ScrollBar& ScrollBar::operator=(const ScrollBar& handle)
{
  if(&handle != this)
  {
    AbsoluteLayout::operator=(handle);
  }
  return *this;
}

ScrollBar& ScrollBar::operator=(ScrollBar&& rhs) noexcept
{
  AbsoluteLayout::operator=(std::move(rhs));
  return *this;
}

ScrollBar ScrollBar::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<ScrollBar, Integration::ScrollBarImpl>(handle);
}

ScrollBar::ScrollBar(Integration::ScrollBarImpl& implementation)
: AbsoluteLayout(implementation)
{
}

ScrollBar::ScrollBar(Dali::Internal::CustomActor* internal)
: AbsoluteLayout(internal)
{
  VerifyCustomActorPointer<Integration::ScrollBarImpl>(internal);
}

View ScrollBar::GetTarget() const
{
  return GetImpl(*this).GetTarget();
}

ScrollBarVisibility ScrollBar::GetVerticalScrollBarVisibility() const
{
  return GetImpl(*this).GetVerticalScrollBarVisibility();
}

void ScrollBar::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetVerticalScrollBarVisibility(visibility);
}

ScrollBarVisibility ScrollBar::GetHorizontalScrollBarVisibility() const
{
  return GetImpl(*this).GetHorizontalScrollBarVisibility();
}

void ScrollBar::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetHorizontalScrollBarVisibility(visibility);
}

void ScrollBar::UpdateBarSize(float scrollWidth, float scrollHeight, float viewportWidth, float viewportHeight)
{
  GetImpl(*this).UpdateBarSize(scrollWidth, scrollHeight, viewportWidth, viewportHeight);
}

void ScrollBar::UpdateScrollPosition(const Vector2& position)
{
  GetImpl(*this).UpdateScrollPosition(position);
}

void ScrollBar::HideBar()
{
  GetImpl(*this).HideBar();
}

float ScrollBar::GetBarThickness() const
{
  return GetImpl(*this).GetBarThickness();
}

void ScrollBar::SetBarThickness(float thickness)
{
  GetImpl(*this).SetBarThickness(thickness);
}

Vector4 ScrollBar::GetBarColor() const
{
  return GetImpl(*this).GetBarColor();
}

void ScrollBar::SetBarColor(const Vector4& color)
{
  GetImpl(*this).SetBarColor(color);
}

float ScrollBar::GetBarMinSize() const
{
  return GetImpl(*this).GetBarMinSize();
}

void ScrollBar::SetBarMinSize(float minSize)
{
  GetImpl(*this).SetBarMinSize(minSize);
}

Vector2 ScrollBar::GetScrollPosition() const
{
  return GetImpl(*this).GetScrollPosition();
}

void ScrollBar::SetScrollPosition(const Vector2& position)
{
  GetImpl(*this).SetScrollPosition(position);
}

float ScrollBar::GetBarOffset() const
{
  return GetImpl(*this).GetBarOffset();
}

void ScrollBar::SetBarOffset(float offset)
{
  GetImpl(*this).SetBarOffset(offset);
}

Vector4 ScrollBar::GetBarCornerRadius() const
{
  return GetImpl(*this).GetBarCornerRadius();
}

void ScrollBar::SetBarCornerRadius(float radius)
{
  GetImpl(*this).SetBarCornerRadius(radius);
}

void ScrollBar::SetBarCornerRadius(const Vector4& radius)
{
  GetImpl(*this).SetBarCornerRadius(radius);
}

} // namespace Ui

} // namespace Dali
