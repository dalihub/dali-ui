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

#include <dali-ui-foundation/integration-api/recycler-view-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{
namespace Ui
{

inline Integration::RecyclerViewImpl& GetImpl(RecyclerView& recyclerView)
{
  DALI_ASSERT_ALWAYS(recyclerView);
  Dali::RefObject& handle = recyclerView.GetImplementation();
  return static_cast<Integration::RecyclerViewImpl&>(handle);
}

inline const Integration::RecyclerViewImpl& GetImpl(const RecyclerView& recyclerView)
{
  DALI_ASSERT_ALWAYS(recyclerView);
  const Dali::RefObject& handle = recyclerView.GetImplementation();
  return static_cast<const Integration::RecyclerViewImpl&>(handle);
}

RecyclerView::RecyclerView()
{
}

RecyclerView RecyclerView::New()
{
  Integration::RecyclerViewImplPtr impl = Integration::RecyclerViewImpl::New();

  RecyclerView recyclerView = RecyclerView(*impl);
  impl->Initialize();
  return recyclerView;
}

RecyclerView::RecyclerView(const RecyclerView& recyclerView)
: View(recyclerView)
{
}

RecyclerView::RecyclerView(RecyclerView&& rhs) noexcept
: View(std::move(rhs))
{
}

RecyclerView::~RecyclerView()
{
}

RecyclerView& RecyclerView::operator=(const RecyclerView& handle)
{
  if(&handle != this)
  {
    View::operator=(handle);
  }
  return *this;
}

RecyclerView& RecyclerView::operator=(RecyclerView&& rhs) noexcept
{
  View::operator=(std::move(rhs));
  return *this;
}

RecyclerView RecyclerView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<RecyclerView, Integration::RecyclerViewImpl>(handle);
}

RecyclerView::RecyclerView(Integration::RecyclerViewImpl& implementation)
: View(implementation)
{
}

RecyclerView::RecyclerView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::RecyclerViewImpl>(internal);
}

void RecyclerView::SetAdapter(ItemAdapter adapter)
{
  GetImpl(*this).SetAdapter(std::move(adapter));
}

ItemAdapter RecyclerView::GetAdapter() const
{
  return GetImpl(*this).GetAdapter();
}

void RecyclerView::ClearAdapter()
{
  GetImpl(*this).ClearAdapter();
}

void RecyclerView::SetItemsLayouter(ItemsLayouter layouter)
{
  GetImpl(*this).SetItemsLayouter(layouter);
}

ItemsLayouter RecyclerView::GetItemsLayouter() const
{
  return GetImpl(*this).GetItemsLayouter();
}

void RecyclerView::SetCacheExtent(float before, float after)
{
  GetImpl(*this).SetCacheExtent(before, after);
}

void RecyclerView::GetCacheExtent(float& before, float& after) const
{
  GetImpl(*this).GetCacheExtent(before, after);
}

void RecyclerView::ScrollToPosition(uint32_t position, bool animation)
{
  GetImpl(*this).ScrollToPosition(position, animation);
}

void RecyclerView::ScrollBy(float distance, bool animation)
{
  GetImpl(*this).ScrollBy(distance, animation);
}

void RecyclerView::SetScrollOffset(float offset)
{
  GetImpl(*this).SetScrollOffset(offset);
}

float RecyclerView::GetScrollOffset() const
{
  return GetImpl(*this).GetScrollOffset();
}

uint32_t RecyclerView::GetFirstVisiblePosition() const
{
  return GetImpl(*this).GetFirstVisiblePosition();
}

uint32_t RecyclerView::GetLastVisiblePosition() const
{
  return GetImpl(*this).GetLastVisiblePosition();
}

void RecyclerView::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetVerticalScrollBarVisibility(visibility);
}

ScrollBarVisibility RecyclerView::GetVerticalScrollBarVisibility() const
{
  return GetImpl(*this).GetVerticalScrollBarVisibility();
}

void RecyclerView::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  GetImpl(*this).SetHorizontalScrollBarVisibility(visibility);
}

ScrollBarVisibility RecyclerView::GetHorizontalScrollBarVisibility() const
{
  return GetImpl(*this).GetHorizontalScrollBarVisibility();
}

void RecyclerView::SetOverScrollMode(OverScrollMode mode)
{
  GetImpl(*this).SetOverScrollMode(mode);
}

OverScrollMode RecyclerView::GetOverScrollMode() const
{
  return GetImpl(*this).GetOverScrollMode();
}

void RecyclerView::SetStartEdgeEffect(EdgeEffect effect)
{
  GetImpl(*this).SetStartEdgeEffect(effect);
}

EdgeEffect RecyclerView::GetStartEdgeEffect() const
{
  return GetImpl(*this).GetStartEdgeEffect();
}

void RecyclerView::SetEndEdgeEffect(EdgeEffect effect)
{
  GetImpl(*this).SetEndEdgeEffect(effect);
}

EdgeEffect RecyclerView::GetEndEdgeEffect() const
{
  return GetImpl(*this).GetEndEdgeEffect();
}

bool RecyclerView::IsScrolling() const
{
  return GetImpl(*this).IsScrolling();
}

void RecyclerView::SetKeyScrollEnabled(bool enable)
{
  GetImpl(*this).SetKeyScrollEnabled(enable);
}

bool RecyclerView::IsKeyScrollEnabled() const
{
  return GetImpl(*this).IsKeyScrollEnabled();
}

void RecyclerView::SetKeyScrollStep(float step)
{
  GetImpl(*this).SetKeyScrollStep(step);
}

float RecyclerView::GetKeyScrollStep() const
{
  return GetImpl(*this).GetKeyScrollStep();
}

void RecyclerView::SetScrollOnFocus(bool enable)
{
  GetImpl(*this).SetScrollOnFocus(enable);
}

bool RecyclerView::GetScrollOnFocus() const
{
  return GetImpl(*this).GetScrollOnFocus();
}

void RecyclerView::SetFocusScrollPeek(float peek)
{
  GetImpl(*this).SetFocusScrollPeek(peek);
}

float RecyclerView::GetFocusScrollPeek() const
{
  return GetImpl(*this).GetFocusScrollPeek();
}

void RecyclerView::AddItemDecoration(ItemDecoration& decoration)
{
  GetImpl(*this).AddItemDecoration(decoration);
}

void RecyclerView::RemoveItemDecoration(ItemDecoration& decoration)
{
  GetImpl(*this).RemoveItemDecoration(decoration);
}

RecyclerView::ScrollStartedSignalType& RecyclerView::ScrollStartedSignal()
{
  return GetImpl(*this).ScrollStartedSignal();
}

RecyclerView::ScrollFinishedSignalType& RecyclerView::ScrollFinishedSignal()
{
  return GetImpl(*this).ScrollFinishedSignal();
}

RecyclerView::DragStartedSignalType& RecyclerView::DragStartedSignal()
{
  return GetImpl(*this).DragStartedSignal();
}

RecyclerView::DragFinishedSignalType& RecyclerView::DragFinishedSignal()
{
  return GetImpl(*this).DragFinishedSignal();
}

} // namespace Ui
} // namespace Dali
