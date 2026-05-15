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

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/public-api/scroll-view.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Ui
{

inline Integration::ScrollViewImpl& GetImpl(ScrollView& scrollView)
{
  DALI_ASSERT_ALWAYS(scrollView);
  Dali::RefObject& handle = scrollView.GetImplementation();
  return static_cast<Integration::ScrollViewImpl&>(handle);
}

inline const Integration::ScrollViewImpl& GetImpl(const ScrollView& scrollView)

{
  DALI_ASSERT_ALWAYS(scrollView);
  const Dali::RefObject& handle = scrollView.GetImplementation();
  return static_cast<const Integration::ScrollViewImpl&>(handle);
}

ScrollView::ScrollView()
{
}

ScrollView ScrollView::New()
{
  Integration::ScrollViewImplPtr impl = Integration::ScrollViewImpl::New();

  ScrollView scrollView = ScrollView(*impl);

  // Second-phase initialization
  impl->Initialize();
  return scrollView;
}

ScrollView::ScrollView(const ScrollView& scrollView)
: View(scrollView)
{
}

ScrollView::ScrollView(ScrollView&& rhs) noexcept
: View(std::move(rhs))
{
}

ScrollView::~ScrollView()
{
}

ScrollView& ScrollView::operator=(const ScrollView& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

ScrollView& ScrollView::operator=(ScrollView&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

ScrollView ScrollView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<ScrollView, Integration::ScrollViewImpl>(handle);
}

ScrollView::ScrollView(Integration::ScrollViewImpl& implementation)
: View(implementation)
{
}

ScrollView::ScrollView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::ScrollViewImpl>(internal);
}

// =============================================================================
// Properties
// =============================================================================

ScrollView& ScrollView::SetContent(View content)
{
  GetImpl(*this).SetContent(content);
  return *this;
}

View ScrollView::GetContent() const
{
  return GetImpl(*this).GetContent();
}

Vector2 ScrollView::GetScrollPosition() const
{
  return GetImpl(*this).GetScrollPosition();
}

ScrollView& ScrollView::SetScrollPosition(const Vector2& position)
{
  GetImpl(*this).SetScrollPosition(position);
  return *this;
}

bool ScrollView::IsScrolling() const
{
  return GetImpl(*this).IsScrolling();
}

ScrollDirection ScrollView::GetScrollDirection() const
{
  return GetImpl(*this).GetScrollDirection();
}

ScrollView& ScrollView::SetScrollDirection(ScrollDirection direction)
{
  GetImpl(*this).SetScrollDirection(direction);
  return *this;
}

float ScrollView::GetMaxFlingDistance() const
{
  return GetImpl(*this).GetMaxFlingDistance();
}

ScrollView& ScrollView::SetMaxFlingDistance(float distance)
{
  GetImpl(*this).SetMaxFlingDistance(distance);
  return *this;
}

int ScrollView::GetMinimumFlingDuration() const
{
  return GetImpl(*this).GetMinimumFlingDuration();
}

ScrollView& ScrollView::SetMinimumFlingDuration(int duration)
{
  GetImpl(*this).SetMinimumFlingDuration(duration);
  return *this;
}

int ScrollView::GetMaximumFlingDuration() const
{
  return GetImpl(*this).GetMaximumFlingDuration();
}

ScrollView& ScrollView::SetMaximumFlingDuration(int duration)
{
  GetImpl(*this).SetMaximumFlingDuration(duration);
  return *this;
}

float ScrollView::GetFlingSensitivity() const
{
  return GetImpl(*this).GetFlingSensitivity();
}

ScrollView& ScrollView::SetFlingSensitivity(float sensitivity)
{
  GetImpl(*this).SetFlingSensitivity(sensitivity);
  return *this;
}

float ScrollView::GetDecelerationRate() const
{
  return GetImpl(*this).GetDecelerationRate();
}

ScrollView& ScrollView::SetDecelerationRate(float rate)
{
  GetImpl(*this).SetDecelerationRate(rate);
  return *this;
}

OverScrollMode ScrollView::GetOverScrollMode() const
{
  return GetImpl(*this).GetOverScrollMode();
}

ScrollView& ScrollView::SetOverScrollMode(OverScrollMode mode)
{
  GetImpl(*this).SetOverScrollMode(mode);
  return *this;
}

ScrollView& ScrollView::SetScrollOnFocus(bool enable)
{
  GetImpl(*this).SetScrollOnFocus(enable);
  return *this;
}

bool ScrollView::GetScrollOnFocus() const
{
  return GetImpl(*this).GetScrollOnFocus();
}

ScrollView& ScrollView::SetFocusScrollToPosition(ScrollToPosition position)
{
  GetImpl(*this).SetFocusScrollToPosition(position);
  return *this;
}

ScrollToPosition ScrollView::GetFocusScrollToPosition() const
{
  return GetImpl(*this).GetFocusScrollToPosition();
}

ScrollView& ScrollView::SetFocusScrollPeek(float peek)
{
  GetImpl(*this).SetFocusScrollPeek(peek);
  return *this;
}

float ScrollView::GetFocusScrollPeek() const
{
  return GetImpl(*this).GetFocusScrollPeek();
}

void ScrollView::ScrollTo(const Vector2& position, bool animation)
{
  GetImpl(*this).ScrollTo(position, animation);
}

void ScrollView::ScrollTo(View child, bool animation, ScrollToPosition scrollToPosition)
{
  GetImpl(*this).ScrollTo(child, animation, scrollToPosition);
}

void ScrollView::ScrollToX(float position, bool animation)
{
  GetImpl(*this).ScrollToX(position, animation);
}

void ScrollView::ScrollToY(float position, bool animation)
{
  GetImpl(*this).ScrollToY(position, animation);
}

ScrollBarVisibility ScrollView::GetVerticalScrollBarVisibility() const
{
  return GetImpl(*this).GetVerticalScrollBarVisibility();
}

ScrollView& ScrollView::SetKeyScrollEnabled(bool enable)
{
  GetImpl(*this).SetKeyScrollEnabled(enable);
  return *this;
}

bool ScrollView::GetKeyScrollEnabled() const
{
  return GetImpl(*this).GetKeyScrollEnabled();
}

ScrollView& ScrollView::SetKeyScrollStep(float step)
{
  GetImpl(*this).SetKeyScrollStep(step);
  return *this;
}

float ScrollView::GetKeyScrollStep() const
{
  return GetImpl(*this).GetKeyScrollStep();
}

ScrollView& ScrollView::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetVerticalScrollBarVisibility(visibility);
  return *this;
}

ScrollBarVisibility ScrollView::GetHorizontalScrollBarVisibility() const
{
  return GetImpl(*this).GetHorizontalScrollBarVisibility();
}

ScrollView& ScrollView::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetHorizontalScrollBarVisibility(visibility);
  return *this;
}

ScrollView::ScrollStartedSignalType& ScrollView::ScrollStartedSignal()
{
  return GetImpl(*this).ScrollStartedSignal();
}

ScrollView::ScrollingSignalType& ScrollView::ScrollingSignal()
{
  return GetImpl(*this).ScrollingSignal();
}

ScrollView::ScrollFinishedSignalType& ScrollView::ScrollFinishedSignal()
{
  return GetImpl(*this).ScrollFinishedSignal();
}

ScrollView::DragStartedSignalType& ScrollView::DragStartedSignal()
{
  return GetImpl(*this).DragStartedSignal();
}

ScrollView::DraggingSignalType& ScrollView::DraggingSignal()
{
  return GetImpl(*this).DraggingSignal();
}

ScrollView::DragFinishedSignalType& ScrollView::DragFinishedSignal()
{
  return GetImpl(*this).DragFinishedSignal();
}

} // namespace Ui

} // namespace Dali
