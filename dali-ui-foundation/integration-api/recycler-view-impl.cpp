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

#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/items-layouter-impl.h>
#include <dali-ui-foundation/integration-api/recycler-view-impl.h>
#include <dali-ui-foundation/integration-api/recycler.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector3.h>
#include <algorithm>
#include <cmath>
#include <limits>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace
{
constexpr float    DEFAULT_CACHE_EXTENT = 300.0f;
constexpr float    KEY_ABSORB_VEL       = 600.0f;
constexpr uint32_t KEY_FOCUS_CHECK_MS   = 50u;
constexpr float    WHEEL_SCROLL_STEP    = 120.0f;
constexpr float    FLING_K_DIST         = 0.002f;
constexpr float    FLING_K_DUR          = 0.003f;
constexpr float    FLING_VMIN           = 0.08f;
constexpr float    FLING_VSTOP          = 0.04f;

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(RecyclerViewImpl, LayoutImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

// ---------------------------------------------------------------------------
// RecyclerImpl — provides the Recycler interface to ItemsLayouter
// ---------------------------------------------------------------------------

struct RecyclerViewImpl::RecyclerImpl : public Recycler
{
  explicit RecyclerImpl(RecyclerViewImpl& owner)
  : rv(owner)
  {
  }

  View GetViewForPosition(uint32_t position) override
  {
    for(const auto& record : rv.mActiveItems)
    {
      if(record.position == position)
      {
        return record.view;
      }
    }

    if(!rv.mAdapter)
    {
      return View{};
    }

    const uint32_t viewType = rv.mAdapter->GetItemViewType(position);
    return rv.ObtainItemView(position, viewType);
  }

  void RecycleViewForPosition(uint32_t position) override
  {
    for(size_t i = 0u; i < rv.mActiveItems.size(); ++i)
    {
      if(rv.mActiveItems[i].position == position)
      {
        rv.RecycleRecord(i);
        return;
      }
    }
  }

  void RecycleAllViews() override
  {
    rv.RecycleAll();
  }

  uint32_t GetItemCount() const override
  {
    return rv.mAdapter ? rv.mAdapter->GetItemCount() : 0u;
  }

  float GetViewportExtent() const override
  {
    return rv.GetViewportExtent();
  }

  float GetCrossExtent() const override
  {
    return rv.GetCrossExtent();
  }

  float GetCacheBefore() const override
  {
    return rv.mCacheBefore;
  }

  float GetCacheAfter() const override
  {
    return rv.mCacheAfter;
  }

  RecyclerViewImpl& rv;
};

// ---------------------------------------------------------------------------
// RecyclerViewImpl
// ---------------------------------------------------------------------------

RecyclerViewImplPtr RecyclerViewImpl::New()
{
  return RecyclerViewImplPtr(new RecyclerViewImpl());
}

RecyclerViewImpl::RecyclerViewImpl()
: LayoutImpl(),
  mAdapter(nullptr),
  mLayouter(),
  mRecyclerImpl(std::make_unique<RecyclerImpl>(*this)),
  mScroller(),
  mScrollBar(ScrollBar::New()),
  mScrollAnimation(),
  mScrollerPositionNotification(),
  mPanGestureDetector(PanGestureDetector::New()),
  mViewportWidth(0.0f),
  mViewportHeight(0.0f),
  mCacheBefore(DEFAULT_CACHE_EXTENT),
  mCacheAfter(DEFAULT_CACHE_EXTENT),
  mLastPanMainPosition(0.0f),
  mMaxFlingDistance(6000.0f),
  mMinimumFlingDuration(200.0f),
  mMaximumFlingDuration(1200.0f),
  mStartEdgeEffect(),
  mEndEdgeEffect(),
  mUsingDefaultEdgeEffects(true),
  mStartEdgeActive(false),
  mEndEdgeActive(false),
  mEdgeDisplacement(0.0f),
  mVerticalScrollBarVisibility(ScrollBarVisibility::Auto),
  mHorizontalScrollBarVisibility(ScrollBarVisibility::Auto),
  mOverScrollMode(OverScrollMode::ContentScrolls)
{
}

RecyclerViewImpl::~RecyclerViewImpl()
{
  // Adapter and layouter signals: ConnectionTracker auto-disconnects all slots
  // registered via Connect(this, ...) when `this` is destroyed. No manual
  // disconnect is needed for LayoutInvalidatedSignal or DataChangedSignal here.
  if(mAdapter)
  {
    mAdapter->DestroyedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDestroyed);
    mAdapter->DataChangedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDataChanged);
  }
  AbortScroll();
  RecycleAll();
  mRecycledItems.clear();
  // mRecyclerImpl cleaned up by unique_ptr
}

void RecyclerViewImpl::OnInitialize()
{
  LayoutImpl::OnInitialize();

  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);
  EnsureScroller();
  EnsureDefaultEdgeEffects();
  UpdateEdgeEffectSources();

  Self().Add(mScrollBar);
  mScrollBar.RaiseToTop();
  mScrollBar.SetVerticalScrollBarVisibility(mVerticalScrollBarVisibility);
  mScrollBar.SetHorizontalScrollBarVisibility(mHorizontalScrollBarVisibility);

  mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
  mPanGestureDetector.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  mPanGestureDetector.DetectedSignal().Connect(this, &RecyclerViewImpl::OnPanGesture);
  mPanGestureDetector.Attach(Self());

  FocusManager::Get().FocusChangedSignal().Connect(this, &RecyclerViewImpl::OnFocusManagerChanged);
}

void RecyclerViewImpl::LayoutChildrenScoped()
{
  // The scope sits on the recycler side of the layouter boundary so that third-party
  // ItemsLayouterImpl subclasses are covered without having to opt in.
  Internal::LayoutDependency::RecyclerLayoutOwnerScope ownerScope(this, &mLayouter.GetImpl());
  mLayouter.GetImpl().OnLayoutChildren(*mRecyclerImpl);
}

float RecyclerViewImpl::ScrollByScoped(float delta)
{
  Internal::LayoutDependency::RecyclerLayoutOwnerScope ownerScope(this, &mLayouter.GetImpl());
  return mLayouter.CanScrollVertically() ? mLayouter.GetImpl().ScrollVerticallyBy(delta, *mRecyclerImpl)
                                         : mLayouter.GetImpl().ScrollHorizontallyBy(delta, *mRecyclerImpl);
}

void RecyclerViewImpl::OnAdapterDestroyed(ItemAdapter& /*adapter*/)
{
  // Adapter is being destroyed — clear our reference without calling back into it.
  CancelScrollAnimation();
  RecycleAll();
  mRecycledItems.clear();
  mAdapter = nullptr;
  if(mLayouter)
  {
    mLayouter.GetImpl().OnAdapterChanged();
  }
  FinishEdgeEffects();
  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

void RecyclerViewImpl::OnAdapterDataChanged(const ItemAdapter::ChangeInfo& info)
{
  if(!mLayouter || !mAdapter)
  {
    return;
  }

  switch(info.changeType)
  {
    case ItemAdapter::ChangeType::CONTENT_CHANGED:
    {
      // Caller guarantees sizes are unchanged — rebind active views only.
      const uint32_t end = info.position + info.count;
      for(auto& record : mActiveItems)
      {
        if(record.position >= info.position && record.position < end)
        {
          mAdapter->BindItemView(record.view, record.position);
        }
      }
      return; // No layout change needed.
    }

    case ItemAdapter::ChangeType::CHANGED:
    {
      // Item sizes may differ — full relayout. OnLayoutChildren recycles all active
      // views and refills, re-binding and re-measuring each one in the process.
      LayoutChildrenScoped();
      break;
    }

    case ItemAdapter::ChangeType::INSERTED:
    {
      // Shift active-item positions at or after the insertion point.
      for(auto& record : mActiveItems)
      {
        if(record.position >= info.position)
        {
          record.position += info.count;
        }
      }
      mLayouter.GetImpl().OnAdapterChanged();
      LayoutChildrenScoped();
      break;
    }

    case ItemAdapter::ChangeType::REMOVED:
    {
      // Recycle views in the removed range; shift positions of survivors.
      const uint32_t end = info.position + info.count;
      for(int32_t i = static_cast<int32_t>(mActiveItems.size()) - 1; i >= 0; --i)
      {
        const uint32_t pos = mActiveItems[static_cast<size_t>(i)].position;
        if(pos >= info.position && pos < end)
        {
          RecycleRecord(static_cast<size_t>(i));
        }
        else if(pos >= end)
        {
          mActiveItems[static_cast<size_t>(i)].position -= info.count;
        }
      }
      mLayouter.GetImpl().OnAdapterChanged();
      LayoutChildrenScoped();
      break;
    }

    case ItemAdapter::ChangeType::MOVED:
    {
      // Adjust position of the moved item if it is active.
      for(auto& record : mActiveItems)
      {
        if(record.position == info.position)
        {
          record.position = info.toPosition;
          break;
        }
      }
      mLayouter.GetImpl().OnAdapterChanged();
      LayoutChildrenScoped();
      break;
    }

    case ItemAdapter::ChangeType::FULL:
    default:
    {
      mLayouter.GetImpl().OnAdapterChanged();
      LayoutChildrenScoped();
      break;
    }
  }

  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

void RecyclerViewImpl::OnLayoutInvalidated()
{
  if(!mAdapter || !mLayouter)
  {
    return;
  }
  LayoutChildrenScoped();
  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

void RecyclerViewImpl::SetAdapter(ItemAdapter& adapter)
{
  if(mAdapter == &adapter)
  {
    return;
  }

  if(mAdapter)
  {
    mAdapter->DestroyedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDestroyed);
    mAdapter->DataChangedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDataChanged);
  }

  AbortScroll();
  RecycleAll();
  mRecycledItems.clear();
  mAdapter = &adapter;
  mAdapter->DestroyedSignal().Connect(this, &RecyclerViewImpl::OnAdapterDestroyed);
  mAdapter->DataChangedSignal().Connect(this, &RecyclerViewImpl::OnAdapterDataChanged);

  if(mLayouter)
  {
    mLayouter.GetImpl().OnAdapterChanged();
  }
  FinishEdgeEffects();

  if(mLayouter)
  {
    LayoutChildrenScoped();
  }

  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

ItemAdapter* RecyclerViewImpl::GetAdapter() const
{
  return mAdapter;
}

void RecyclerViewImpl::ClearAdapter()
{
  if(mAdapter)
  {
    mAdapter->DestroyedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDestroyed);
    mAdapter->DataChangedSignal().Disconnect(this, &RecyclerViewImpl::OnAdapterDataChanged);
  }

  AbortScroll();
  RecycleAll();
  mRecycledItems.clear();
  mAdapter = nullptr;

  if(mLayouter)
  {
    mLayouter.GetImpl().OnAdapterChanged();
  }
  FinishEdgeEffects();
  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

void RecyclerViewImpl::SetItemsLayouter(ItemsLayouter layouter)
{
  if(mLayouter == layouter)
  {
    return;
  }

  if(mLayouter)
  {
    mLayouter.GetImpl().LayoutInvalidatedSignal().Disconnect(this, &RecyclerViewImpl::OnLayoutInvalidated);
  }

  AbortScroll();
  RecycleAll();
  mRecycledItems.clear();
  mLayouter = layouter;

  if(mLayouter)
  {
    mLayouter.GetImpl().LayoutInvalidatedSignal().Connect(this, &RecyclerViewImpl::OnLayoutInvalidated);
    mLayouter.GetImpl().OnAdapterChanged();
  }

  EnsureDefaultEdgeEffects();
  UpdateEdgeEffectSources();
  FinishEdgeEffects();

  if(mAdapter && mLayouter)
  {
    LayoutChildrenScoped();
  }

  UpdateScrollerSize();
  ApplyScrollerPosition();
  UpdateScrollBar();
}

ItemsLayouter RecyclerViewImpl::GetItemsLayouter() const
{
  return mLayouter;
}

void RecyclerViewImpl::SetCacheExtent(float before, float after)
{
  mCacheBefore = std::max(0.0f, before);
  mCacheAfter  = std::max(0.0f, after);

  if(mLayouter && mAdapter)
  {
    LayoutChildrenScoped();
    UpdateScrollerSize();
    ApplyScrollerPosition();
    UpdateScrollBar();
  }
}

void RecyclerViewImpl::GetCacheExtent(float& before, float& after) const
{
  before = mCacheBefore;
  after  = mCacheAfter;
}

void RecyclerViewImpl::ScrollToPosition(uint32_t position, bool animation)
{
  if(!mLayouter)
  {
    return;
  }

  const LayoutRect bounds    = mLayouter.GetItemBounds(position, GetCrossExtent());
  const float      itemStart = mLayouter.CanScrollVertically() ? bounds.y : bounds.x;
  const float      base      = mScrollAnimation ? SyncScrollOffsetFromScroller() : mLayouter.ComputeScrollOffset();
  ScrollBy(ClampScrollOffset(itemStart) - base, animation);
}

void RecyclerViewImpl::ScrollBy(float distance, bool animation)
{
  if(!mLayouter)
  {
    return;
  }

  const float base = mScrollAnimation ? SyncScrollOffsetFromScroller() : mLayouter.ComputeScrollOffset();
  CancelScrollAnimation();

  if(!animation)
  {
    const float consumed = ScrollByScoped(distance);
    ApplyScrollerPosition();
    PullEdgeEffect(distance, consumed);
    UpdateScrollBar();
    return;
  }

  const float target   = ClampScrollOffset(base + distance);
  const float consumed = target - base;

  if(std::abs(consumed) < 0.5f)
  {
    PullEdgeEffect(distance, 0.0f);
    ReleaseEdgeEffects(distance < 0.0f ? 1.0f : -1.0f);
    return;
  }

  StartScrollAnimation(target, CalculateScrollDuration(std::abs(consumed)));
}

void RecyclerViewImpl::SetScrollOffset(float offset)
{
  if(!mLayouter)
  {
    return;
  }
  CancelScrollAnimation();
  const float delta = offset - mLayouter.ComputeScrollOffset();
  ScrollByScoped(delta);
  ApplyScrollerPosition();
  UpdateScrollBar();
}

float RecyclerViewImpl::GetScrollOffset() const
{
  return mLayouter ? mLayouter.ComputeScrollOffset() : 0.0f;
}

uint32_t RecyclerViewImpl::GetFirstVisiblePosition() const
{
  return mLayouter ? mLayouter.GetFirstVisiblePosition() : 0u;
}

uint32_t RecyclerViewImpl::GetLastVisiblePosition() const
{
  return mLayouter ? mLayouter.GetLastVisiblePosition() : 0u;
}

void RecyclerViewImpl::SetVerticalScrollBarVisibility(ScrollBarVisibility visibility)
{
  mVerticalScrollBarVisibility = visibility;
  mScrollBar.SetVerticalScrollBarVisibility(visibility);
  UpdateScrollBar();
}

ScrollBarVisibility RecyclerViewImpl::GetVerticalScrollBarVisibility() const
{
  return mVerticalScrollBarVisibility;
}

void RecyclerViewImpl::SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility)
{
  mHorizontalScrollBarVisibility = visibility;
  mScrollBar.SetHorizontalScrollBarVisibility(visibility);
  UpdateScrollBar();
}

ScrollBarVisibility RecyclerViewImpl::GetHorizontalScrollBarVisibility() const
{
  return mHorizontalScrollBarVisibility;
}

void RecyclerViewImpl::SetOverScrollMode(OverScrollMode mode)
{
  mOverScrollMode = mode;
  if(!CanOverScroll())
  {
    FinishEdgeEffects();
  }
}

OverScrollMode RecyclerViewImpl::GetOverScrollMode() const
{
  return mOverScrollMode;
}

void RecyclerViewImpl::SetStartEdgeEffect(EdgeEffect effect)
{
  mUsingDefaultEdgeEffects = false;
  mStartEdgeEffect         = effect;
  UpdateEdgeEffectSources();
}

EdgeEffect RecyclerViewImpl::GetStartEdgeEffect() const
{
  return mStartEdgeEffect;
}

void RecyclerViewImpl::SetEndEdgeEffect(EdgeEffect effect)
{
  mUsingDefaultEdgeEffects = false;
  mEndEdgeEffect           = effect;
  UpdateEdgeEffectSources();
}

EdgeEffect RecyclerViewImpl::GetEndEdgeEffect() const
{
  return mEndEdgeEffect;
}

MeasuredSize RecyclerViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  const float width  = std::max(0.0f, widthConstraint);
  const float height = std::max(0.0f, heightConstraint);
  return MeasuredSize(width, height);
}

LayoutRect RecyclerViewImpl::OnArrange(const LayoutRect& bounds)
{
  const float newWidth    = std::max(0.0f, bounds.width);
  const float newHeight   = std::max(0.0f, bounds.height);
  const bool  sizeChanged = (newWidth != mViewportWidth || newHeight != mViewportHeight);

  mViewportWidth  = newWidth;
  mViewportHeight = newHeight;

  EnsureScroller();

  // Full relayout only when the viewport dimensions actually change.
  // SetAdapter / SetItemsLayouter / NotifyDataSetChanged call OnLayoutChildren directly.
  if(sizeChanged && mLayouter && mAdapter)
  {
    LayoutChildrenScoped();
  }

  UpdateScrollerSize();
  ApplyScrollerPosition();
  mScrollBar.RaiseToTop();
  UpdateScrollBar();

  return bounds;
}

void RecyclerViewImpl::EnsureScroller()
{
  if(mScroller)
  {
    return;
  }

  mScroller = View::New();
  Self().Add(mScroller);
  mScroller.LowerToBottom();
  mScrollerPositionNotification = mScroller.AddPropertyNotification(Actor::Property::POSITION, StepCondition(1.0f, 0.0f));
  mScrollerPositionNotification.NotifySignal().Connect(this, &RecyclerViewImpl::OnScrollerPositionChanged);
  UpdateEdgeEffectSources();
}

void RecyclerViewImpl::UpdateScrollerSize()
{
  if(!mScroller || !mLayouter)
  {
    return;
  }

  const float contentExtent = mLayouter.ComputeScrollRange();
  if(mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
  {
    Dali::Ui::Extension::View::SetSizeWidth(mScroller, std::max(mViewportWidth, contentExtent));
    Dali::Ui::Extension::View::SetSizeHeight(mScroller, mViewportHeight);
  }
  else
  {
    Dali::Ui::Extension::View::SetSizeWidth(mScroller, mViewportWidth);
    Dali::Ui::Extension::View::SetSizeHeight(mScroller, std::max(mViewportHeight, contentExtent));
  }
}

void RecyclerViewImpl::UpdateScrollBar()
{
  if(!mScrollBar || !mLayouter)
  {
    return;
  }

  const bool  horizontal    = mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically();
  const float offset        = mLayouter.ComputeScrollOffset();
  const float contentExtent = mLayouter.ComputeScrollRange();

  float scrollWidth  = mViewportWidth;
  float scrollHeight = mViewportHeight;
  float scrollX      = 0.0f;
  float scrollY      = 0.0f;

  if(horizontal)
  {
    scrollWidth = std::max(mViewportWidth, contentExtent);
    scrollX     = offset;
  }
  else
  {
    scrollHeight = std::max(mViewportHeight, contentExtent);
    scrollY      = offset;
  }

  mScrollBar.SetVerticalScrollBarVisibility(horizontal ? ScrollBarVisibility::Never : mVerticalScrollBarVisibility);
  mScrollBar.SetHorizontalScrollBarVisibility(horizontal ? mHorizontalScrollBarVisibility : ScrollBarVisibility::Never);
  mScrollBar.UpdateBarSize(scrollWidth, scrollHeight, mViewportWidth, mViewportHeight);
  mScrollBar.UpdateScrollPosition(Vector2(scrollX, scrollY));
}

void RecyclerViewImpl::RecycleAll()
{
  for(auto& record : mActiveItems)
  {
    if(mAdapter && record.view)
    {
      mAdapter->ItemViewRecycled(record.view, record.viewType);
    }
    if(record.view && mScroller)
    {
      mScroller.Remove(record.view);
    }
    mRecycledItems.push_back(record);
  }
  mActiveItems.clear();
}

void RecyclerViewImpl::RecycleRecord(size_t index)
{
  if(index >= mActiveItems.size())
  {
    return;
  }

  ItemRecord record = mActiveItems[index];
  if(mAdapter && record.view)
  {
    mAdapter->ItemViewRecycled(record.view, record.viewType);
  }
  if(record.view && mScroller)
  {
    mScroller.Remove(record.view);
  }
  mActiveItems.erase(mActiveItems.begin() + static_cast<std::ptrdiff_t>(index));
  mRecycledItems.push_back(record);
}

View RecyclerViewImpl::ObtainItemView(uint32_t position, uint32_t viewType)
{
  for(size_t i = 0u; i < mRecycledItems.size(); ++i)
  {
    if(mRecycledItems[i].viewType == viewType)
    {
      View view = mRecycledItems[i].view;
      mRecycledItems.erase(mRecycledItems.begin() + static_cast<std::ptrdiff_t>(i));
      mAdapter->BindItemView(view, position);
      mScroller.Add(view);
      ItemRecord record;
      record.position = position;
      record.viewType = viewType;
      record.view     = view;
      mActiveItems.push_back(record);
      return view;
    }
  }

  View view = mAdapter->CreateItemView(viewType);
  if(view)
  {
    mAdapter->BindItemView(view, position);
    mScroller.Add(view);
    ItemRecord record;
    record.position = position;
    record.viewType = viewType;
    record.view     = view;
    mActiveItems.push_back(record);
  }
  return view;
}

float RecyclerViewImpl::GetViewportExtent() const
{
  if(mLayouter && mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
  {
    return mViewportWidth;
  }
  return mViewportHeight;
}

float RecyclerViewImpl::GetCrossExtent() const
{
  if(mLayouter && mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
  {
    return mViewportHeight;
  }
  return mViewportWidth;
}

float RecyclerViewImpl::GetMaxScrollOffset() const
{
  if(!mLayouter)
  {
    return 0.0f;
  }
  return std::max(0.0f, mLayouter.ComputeScrollRange() - mLayouter.ComputeScrollExtent());
}

float RecyclerViewImpl::ClampScrollOffset(float offset) const
{
  return std::min(std::max(0.0f, offset), GetMaxScrollOffset());
}

float RecyclerViewImpl::SyncScrollOffsetFromScroller()
{
  if(!mScroller || !mLayouter)
  {
    return mLayouter ? mLayouter.ComputeScrollOffset() : 0.0f;
  }

  const Vector3 pos            = mScroller.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  const float   physicalOffset = (mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically()) ? -pos.x : -pos.y;
  const float   delta          = physicalOffset - mLayouter.ComputeScrollOffset();

  if(std::abs(delta) > 0.001f)
  {
    ScrollByScoped(delta);
  }

  return mLayouter.ComputeScrollOffset();
}

float RecyclerViewImpl::CalculateScrollDuration(float distance) const
{
  const float maxDistance = std::max(1.0f, GetMaxScrollOffset());
  const float ratio       = std::min(1.0f, distance / maxDistance);
  const float durationMs  = mMinimumFlingDuration + ratio * (mMaximumFlingDuration - mMinimumFlingDuration);
  return durationMs / 1000.0f;
}

void RecyclerViewImpl::StartScrollAnimation(float targetOffset, float durationSeconds)
{
  EnsureScroller();

  SendScrollStarted();

  const float target   = ClampScrollOffset(targetOffset);
  const float distance = std::abs(target - mLayouter.ComputeScrollOffset());
  if(distance < 0.5f || durationSeconds <= 0.0f)
  {
    const float delta = target - mLayouter.ComputeScrollOffset();
    if(mLayouter)
    {
      ScrollByScoped(delta);
    }
    ApplyScrollerPosition();
    UpdateScrollBar();
    SendScrollFinished();
    return;
  }

  mScrollAnimation = Animation::New(durationSeconds);
  if(mLayouter && mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
  {
    mScrollAnimation.AnimateTo(Property(mScroller, Actor::Property::POSITION_X), -target, AlphaFunction::EASE_OUT);
  }
  else
  {
    mScrollAnimation.AnimateTo(Property(mScroller, Actor::Property::POSITION_Y), -target, AlphaFunction::EASE_OUT);
  }
  mScrollAnimation.FinishedSignal().Connect(this, &RecyclerViewImpl::OnScrollAnimationFinished);
  mScrollAnimation.Play();
}

bool RecyclerViewImpl::CanOverScroll() const
{
  if(mOverScrollMode == OverScrollMode::Never)
  {
    return false;
  }
  if(mOverScrollMode == OverScrollMode::Always)
  {
    return true;
  }
  return GetMaxScrollOffset() > 0.0f;
}

void RecyclerViewImpl::EnsureDefaultEdgeEffects()
{
  if(!mUsingDefaultEdgeEffects || !mLayouter)
  {
    return;
  }

  const ScrollDirection axis = (mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
                                 ? ScrollDirection::Horizontal
                                 : ScrollDirection::Vertical;

  mStartEdgeEffect = BounceEdgeEffect::New(axis);
  mEndEdgeEffect   = BounceEdgeEffect::New(axis);
  UpdateEdgeEffectSources();
}

void RecyclerViewImpl::UpdateEdgeEffectSources()
{
  if(mStartEdgeEffect && mScroller)
  {
    mStartEdgeEffect.SetSource(mScroller);
  }
  if(mEndEdgeEffect && mScroller)
  {
    mEndEdgeEffect.SetSource(mScroller);
  }
}

void RecyclerViewImpl::FinishEdgeEffects()
{
  if(mStartEdgeEffect)
  {
    mStartEdgeEffect.Finish();
  }
  if(mEndEdgeEffect)
  {
    mEndEdgeEffect.Finish();
  }
  mStartEdgeActive  = false;
  mEndEdgeActive    = false;
  mEdgeDisplacement = 0.0f;
}

void RecyclerViewImpl::ReleaseEdgeEffects(float velocity)
{
  if(mStartEdgeActive)
  {
    if(mStartEdgeEffect)
    {
      if(std::abs(velocity) > FLING_VMIN)
      {
        mStartEdgeEffect.OnAbsorb(std::abs(velocity));
      }
      else
      {
        mStartEdgeEffect.OnRelease();
      }
    }
    mStartEdgeActive = false;
  }

  if(mEndEdgeActive)
  {
    if(mEndEdgeEffect)
    {
      if(std::abs(velocity) > FLING_VMIN)
      {
        mEndEdgeEffect.OnAbsorb(-std::abs(velocity));
      }
      else
      {
        mEndEdgeEffect.OnRelease();
      }
    }
    mEndEdgeActive = false;
  }

  mEdgeDisplacement = 0.0f;
}

void RecyclerViewImpl::PullEdgeEffect(float requestedDelta, float consumedDelta)
{
  if(!CanOverScroll())
  {
    FinishEdgeEffects();
    return;
  }

  const float overDelta = requestedDelta - consumedDelta;
  if(std::abs(overDelta) < 0.001f)
  {
    if(std::abs(consumedDelta) > 0.001f && (mStartEdgeActive || mEndEdgeActive))
    {
      FinishEdgeEffects();
    }
    return;
  }

  const float edgeDelta = -overDelta;
  if(edgeDelta > 0.0f && mStartEdgeEffect)
  {
    if(mEndEdgeActive && mEndEdgeEffect)
    {
      mEndEdgeEffect.Finish();
      mEndEdgeActive    = false;
      mEdgeDisplacement = 0.0f;
    }
    mEdgeDisplacement += edgeDelta;
    mStartEdgeEffect.OnPull(edgeDelta, mEdgeDisplacement);
    mStartEdgeActive = true;
  }
  else if(edgeDelta < 0.0f && mEndEdgeEffect)
  {
    if(mStartEdgeActive && mStartEdgeEffect)
    {
      mStartEdgeEffect.Finish();
      mStartEdgeActive  = false;
      mEdgeDisplacement = 0.0f;
    }
    mEdgeDisplacement += -edgeDelta;
    mEndEdgeEffect.OnPull(edgeDelta, -mEdgeDisplacement);
    mEndEdgeActive = true;
  }
}

void RecyclerViewImpl::ApplyScrollerPosition()
{
  if(!mScroller || !mLayouter)
  {
    return;
  }

  const float offset = mLayouter.ComputeScrollOffset();
  if(mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically())
  {
    Dali::Ui::Extension::View::SetPositionX(mScroller, -offset);
    Dali::Ui::Extension::View::SetPositionY(mScroller, 0.0f);
  }
  else
  {
    Dali::Ui::Extension::View::SetPositionX(mScroller, 0.0f);
    Dali::Ui::Extension::View::SetPositionY(mScroller, -offset);
  }
}

void RecyclerViewImpl::CancelScrollAnimation()
{
  if(mScrollAnimation)
  {
    mScrollAnimation.Stop();
    mScrollAnimation.Reset();
  }
}

void RecyclerViewImpl::SendScrollStarted()
{
  if(!mIsScrolling)
  {
    mIsScrolling = true;
    Internal::ScrollStateObserver::Get().NotifyScrollStarted();
    RecyclerView rv = RecyclerView::DownCast(Self());
    mScrollStartedSignal.Emit(rv);
  }
}

void RecyclerViewImpl::SendScrollFinished()
{
  // Stay in scrolling state while drag is active or animation is still playing.
  if(mIsScrolling && !mIsDragging && !mScrollAnimation)
  {
    mIsScrolling = false;
    Internal::ScrollStateObserver::Get().NotifyScrollFinished();
    RecyclerView rv = RecyclerView::DownCast(Self());
    mScrollFinishedSignal.Emit(rv);
  }
}

void RecyclerViewImpl::SendDragStarted()
{
  mIsDragging = true;
  Internal::ScrollStateObserver::Get().NotifyDragStarted();
  RecyclerView rv = RecyclerView::DownCast(Self());
  mDragStartedSignal.Emit(rv);
}

void RecyclerViewImpl::SendDragFinished()
{
  mIsDragging = false;
  Internal::ScrollStateObserver::Get().NotifyDragFinished();
  RecyclerView rv = RecyclerView::DownCast(Self());
  mDragFinishedSignal.Emit(rv);
}

void RecyclerViewImpl::AbortScroll()
{
  StopKeyRepeatTimer();
  CancelScrollAnimation();
  if(mIsDragging)
  {
    mIsDragging = false;
    Internal::ScrollStateObserver::Get().NotifyDragFinished();
  }
  if(mIsScrolling)
  {
    mIsScrolling = false;
    Internal::ScrollStateObserver::Get().NotifyScrollFinished();
  }
}

bool RecyclerViewImpl::IsScrolling() const
{
  return mIsScrolling;
}

RecyclerView::ScrollStartedSignalType& RecyclerViewImpl::ScrollStartedSignal()
{
  return mScrollStartedSignal;
}

RecyclerView::ScrollFinishedSignalType& RecyclerViewImpl::ScrollFinishedSignal()
{
  return mScrollFinishedSignal;
}

RecyclerView::DragStartedSignalType& RecyclerViewImpl::DragStartedSignal()
{
  return mDragStartedSignal;
}

RecyclerView::DragFinishedSignalType& RecyclerViewImpl::DragFinishedSignal()
{
  return mDragFinishedSignal;
}

// ---------------------------------------------------------------------------
// Key-scroll config
// ---------------------------------------------------------------------------

void RecyclerViewImpl::SetKeyScrollEnabled(bool enable)
{
  mKeyScrollEnabled = enable;
  if(!enable)
  {
    StopKeyRepeatTimer();
  }
}

bool RecyclerViewImpl::IsKeyScrollEnabled() const
{
  return mKeyScrollEnabled;
}

void RecyclerViewImpl::SetKeyScrollStep(float step)
{
  mKeyScrollStep = std::max(1.0f, step);
}

float RecyclerViewImpl::GetKeyScrollStep() const
{
  return mKeyScrollStep;
}

void RecyclerViewImpl::SetScrollOnFocus(bool enable)
{
  mScrollOnFocus = enable;
}

bool RecyclerViewImpl::GetScrollOnFocus() const
{
  return mScrollOnFocus;
}

void RecyclerViewImpl::SetFocusScrollPeek(float peek)
{
  mFocusScrollPeek = std::max(0.0f, peek);
}

float RecyclerViewImpl::GetFocusScrollPeek() const
{
  return mFocusScrollPeek;
}

// ---------------------------------------------------------------------------
// Key-scroll helpers
// ---------------------------------------------------------------------------

bool RecyclerViewImpl::IsLayoutAxisDirection(FocusDirection dir) const
{
  if(!mLayouter) return false;
  if(mLayouter.CanScrollVertically())
  {
    return dir == FocusDirection::UP || dir == FocusDirection::DOWN ||
           dir == FocusDirection::PAGE_UP || dir == FocusDirection::PAGE_DOWN;
  }
  return dir == FocusDirection::LEFT || dir == FocusDirection::RIGHT;
}

bool RecyclerViewImpl::IsForwardDirection(FocusDirection dir) const
{
  return dir == FocusDirection::DOWN || dir == FocusDirection::RIGHT ||
         dir == FocusDirection::PAGE_DOWN;
}

bool RecyclerViewImpl::IsAtScrollBoundary(FocusDirection dir) const
{
  if(!mLayouter) return true;
  constexpr float kEpsilon = 1.0f;
  const float     offset   = mLayouter.ComputeScrollOffset();
  const float     maxOff   = GetMaxScrollOffset();

  return IsForwardDirection(dir) ? (offset >= maxOff - kEpsilon) : (offset <= kEpsilon);
}

uint32_t RecyclerViewImpl::FindActiveItemPosition(View view) const
{
  for(const auto& record : mActiveItems)
  {
    if(record.view == view) return record.position;
  }
  return INVALID_ITEM_POSITION;
}

View RecyclerViewImpl::FindActiveView(uint32_t position) const
{
  for(const auto& record : mActiveItems)
  {
    if(record.position == position) return record.view;
  }
  return View();
}

uint32_t RecyclerViewImpl::NextItemPosition(uint32_t pos, FocusDirection dir) const
{
  if(!mAdapter) return INVALID_ITEM_POSITION;
  const uint32_t count = mAdapter->GetItemCount();
  if(IsForwardDirection(dir))
  {
    return (pos + 1u < count) ? pos + 1u : INVALID_ITEM_POSITION;
  }
  return (pos > 0u) ? pos - 1u : INVALID_ITEM_POSITION;
}

void RecyclerViewImpl::TriggerKeyEdgeFeedback(FocusDirection dir)
{
  if(IsForwardDirection(dir))
  {
    if(mEndEdgeEffect) mEndEdgeEffect.OnAbsorb(-KEY_ABSORB_VEL);
  }
  else
  {
    if(mStartEdgeEffect) mStartEdgeEffect.OnAbsorb(KEY_ABSORB_VEL);
  }
}

void RecyclerViewImpl::ScrollToItemMakeVisible(uint32_t position, bool animate)
{
  if(!mLayouter || !mAdapter || position >= mAdapter->GetItemCount()) return;

  const float      curOffset  = mScrollAnimation ? SyncScrollOffsetFromScroller()
                                                 : mLayouter.ComputeScrollOffset();
  const float      viewExtent = GetViewportExtent();
  const LayoutRect bounds     = mLayouter.GetItemBounds(position, GetCrossExtent());
  const bool       vertical   = mLayouter.CanScrollVertically();
  const float      itemStart  = vertical ? bounds.y : bounds.x;
  const float      itemEnd    = itemStart + (vertical ? bounds.height : bounds.width);

  float target = curOffset;
  if(itemStart - mFocusScrollPeek < curOffset)
  {
    target = itemStart - mFocusScrollPeek;
  }
  else if(itemEnd + mFocusScrollPeek > curOffset + viewExtent)
  {
    target = itemEnd + mFocusScrollPeek - viewExtent;
  }
  else
  {
    return; // Already fully visible.
  }

  ScrollBy(ClampScrollOffset(target) - curOffset, animate);
}

void RecyclerViewImpl::StartKeyRepeatTimer()
{
  if(mKeyRepeatTimer && mKeyRepeatTimer.IsRunning()) return;
  mKeyRepeatTimer = Timer::New(KEY_FOCUS_CHECK_MS);
  mKeyRepeatTimer.TickSignal().Connect(this, &RecyclerViewImpl::OnKeyRepeatTimerTick);
  mKeyRepeatTimer.Start();
}

void RecyclerViewImpl::StopKeyRepeatTimer()
{
  if(mKeyRepeatTimer)
  {
    mKeyRepeatTimer.Stop();
    mKeyRepeatTimer.Reset();
  }
  mKeyRepeatTargetPos = INVALID_ITEM_POSITION;
}

bool RecyclerViewImpl::OnKeyRepeatTimerTick()
{
  if(mKeyRepeatTargetPos == INVALID_ITEM_POSITION) return false;

  // Check if the accumulated target position is now in the active (laid-out) item list.
  View targetView = FindActiveView(mKeyRepeatTargetPos);
  if(targetView)
  {
    // If the target is active but not keyboard-focusable, advance past it.
    if(!targetView.IsFocusable())
    {
      const uint32_t next = NextItemPosition(mKeyRepeatTargetPos, mKeyRepeatDir);
      if(next == INVALID_ITEM_POSITION)
      {
        mKeyRepeatTargetPos = INVALID_ITEM_POSITION;
        return false;
      }
      mKeyRepeatTargetPos = next;
      ScrollToItemMakeVisible(next, false);
      return true;
    }

    FocusManager::Get().SetCurrentFocusView(targetView);
    mKeyRepeatTargetPos = INVALID_ITEM_POSITION;
    return false; // Stop timer.
  }

  // Not yet visible — instant-scroll closer so layout catches up.
  ScrollToItemMakeVisible(mKeyRepeatTargetPos, false);
  return true; // Keep timer running.
}

// ---------------------------------------------------------------------------
// Focus overrides
// ---------------------------------------------------------------------------

void RecyclerViewImpl::OnFocusManagerChanged(View from, View to)
{
  if(!mScrollOnFocus || !mAdapter || !mLayouter || !to) return;
  if(from == to) return;

  const uint32_t pos = FindActiveItemPosition(to);
  if(pos == INVALID_ITEM_POSITION) return;

  ScrollToItemMakeVisible(pos, true);
}

View RecyclerViewImpl::OnFocusRequested()
{
  if(!mKeyScrollEnabled || !mLayouter) return LayoutImpl::OnFocusRequested();

  // Return the first focusable visible item, or Self() for step-scroll mode.
  const float offset = mLayouter.ComputeScrollOffset();
  const float extent = GetViewportExtent();
  const bool  vert   = mLayouter.CanScrollVertically();

  for(const auto& record : mActiveItems)
  {
    if(!record.view) continue;
    if(!record.view.IsFocusable()) continue;

    const LayoutRect bounds = mLayouter.GetItemBounds(record.position, GetCrossExtent());
    const float      start  = vert ? bounds.y : bounds.x;
    const float      end    = start + (vert ? bounds.height : bounds.width);
    if(end > offset && start < offset + extent)
    {
      return record.view;
    }
  }

  View selfView = View::DownCast(Self());
  if(selfView.IsFocusable()) return selfView;
  return LayoutImpl::OnFocusRequested();
}

FocusNavigationResult RecyclerViewImpl::OnFocusNavigationRequested(View currentFocusedView, FocusNavigationContext context)
{
  const FocusDirection direction = context.GetDirection();
  if(!mKeyScrollEnabled || !mAdapter || !mLayouter) return FocusNavigationResult::NotHandled();

  const uint32_t curPos = FindActiveItemPosition(currentFocusedView);
  if(curPos == INVALID_ITEM_POSITION) return FocusNavigationResult::NotHandled();
  if(!IsLayoutAxisDirection(direction)) return FocusNavigationResult::NotHandled();

  uint32_t nextPos = NextItemPosition(curPos, direction);
  if(nextPos == INVALID_ITEM_POSITION)
  {
    TriggerKeyEdgeFeedback(direction);
    return FocusNavigationResult::NotHandled(); // Let FocusFinder exit to a neighboring view.
  }

  // Scan past any active (visible) items that are not keyboard-focusable.
  // If an unfocusable item is off-screen we cannot check it here; the timer
  // path handles that case via OnKeyRepeatTimerTick.
  {
    uint32_t scanPos = nextPos;
    while(scanPos != INVALID_ITEM_POSITION)
    {
      View v = FindActiveView(scanPos);
      if(!v) break; // Off-screen — stop scan; use as scroll target.
      if(v.IsFocusable()) break;
      scanPos = NextItemPosition(scanPos, direction);
    }
    if(scanPos == INVALID_ITEM_POSITION)
    {
      TriggerKeyEdgeFeedback(direction);
      return FocusNavigationResult::NotHandled();
    }
    nextPos = scanPos;
  }

  // Check if the resolved item is already active and close enough to focus directly.
  View nextView = FindActiveView(nextPos);
  if(nextView)
  {
    const LayoutRect bounds    = mLayouter.GetItemBounds(nextPos, GetCrossExtent());
    const float      curOffset = mLayouter.ComputeScrollOffset();
    const bool       vert      = mLayouter.CanScrollVertically();
    const float      start     = vert ? bounds.y : bounds.x;
    const float      end       = start + (vert ? bounds.height : bounds.width);
    const float      dist      = IsForwardDirection(direction)
                                   ? std::max(0.0f, end - (curOffset + GetViewportExtent()))
                                   : std::max(0.0f, curOffset - start);

    if(dist < mKeyScrollStep)
    {
      // Item is within step distance — focus it directly.
      // ScrollOnFocus (OnFocusManagerChanged) handles making it fully visible.
      mKeyRepeatTargetPos = INVALID_ITEM_POSITION;
      return FocusNavigationResult::MoveTo(nextView);
    }
  }

  // Next item is not visible or beyond step distance.
  // Accumulate: if key is held in same direction, advance the target further.
  if(mKeyRepeatTargetPos != INVALID_ITEM_POSITION && mKeyRepeatDir == direction)
  {
    const uint32_t advanced = NextItemPosition(mKeyRepeatTargetPos, direction);
    if(advanced != INVALID_ITEM_POSITION) mKeyRepeatTargetPos = advanced;
  }
  else
  {
    mKeyRepeatTargetPos = nextPos;
    mKeyRepeatDir       = direction;
  }

  ScrollToItemMakeVisible(mKeyRepeatTargetPos, true);
  StartKeyRepeatTimer();

  return FocusNavigationResult::Stay(); // The timer updates focus once the target is active.
}

bool RecyclerViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(!mKeyScrollEnabled || !mAdapter || !mLayouter) return false;
  if(event.GetState() != Dali::KeyEvent::DOWN) return false;
  if(FocusManager::Get().GetCurrentFocusView() != View::DownCast(Self())) return false;

  const Dali::String keyName = event.GetKeyName();

  // Home / End: jump to first or last item.
  if(keyName == "Home" || keyName == "End")
  {
    const bool     toEnd   = (keyName == "End");
    const uint32_t targetP = toEnd ? mAdapter->GetItemCount() - 1u : 0u;
    mKeyRepeatTargetPos    = targetP;
    mKeyRepeatDir          = toEnd ? FocusDirection::DOWN : FocusDirection::UP;
    ScrollToItemMakeVisible(targetP, true);
    StartKeyRepeatTimer();
    return true;
  }

  FocusDirection direction;
  if(keyName == "Up")
    direction = FocusDirection::UP;
  else if(keyName == "Down")
    direction = FocusDirection::DOWN;
  else if(keyName == "Left")
    direction = FocusDirection::LEFT;
  else if(keyName == "Right")
    direction = FocusDirection::RIGHT;
  else if(keyName == "Prior" || keyName == "Page_Up")
    direction = FocusDirection::PAGE_UP;
  else if(keyName == "Next" || keyName == "Page_Down")
    direction = FocusDirection::PAGE_DOWN;
  else
    return false;

  if(!IsLayoutAxisDirection(direction)) return false;

  if(IsAtScrollBoundary(direction))
  {
    TriggerKeyEdgeFeedback(direction);
    return false;
  }

  const bool  pageScroll = (direction == FocusDirection::PAGE_UP || direction == FocusDirection::PAGE_DOWN);
  const float step       = pageScroll ? GetViewportExtent() : mKeyScrollStep;
  ScrollBy(IsForwardDirection(direction) ? step : -step, true);
  return true;
}

void RecyclerViewImpl::OnScrollerPositionChanged(PropertyNotification /*source*/)
{
  if(!mScroller || !mLayouter)
  {
    return;
  }

  SyncScrollOffsetFromScroller();
  UpdateScrollerSize();
  UpdateScrollBar();
}

void RecyclerViewImpl::OnScrollAnimationFinished(Animation /*animation*/)
{
  mScrollAnimation.Reset();
  SyncScrollOffsetFromScroller();
  ApplyScrollerPosition();
  UpdateScrollerSize();
  UpdateScrollBar();
  SendScrollFinished();
}

bool RecyclerViewImpl::HasIntrinsicWheelHandling() const
{
  return true;
}

bool RecyclerViewImpl::OnWheelEvent(const Dali::WheelEvent& event)
{
  if(!mAdapter || !mLayouter)
  {
    return false;
  }

  ScrollBy(static_cast<float>(event.GetDelta()) * WHEEL_SCROLL_STEP, true);
  return true;
}

void RecyclerViewImpl::OnPanGesture(Actor /*actor*/, PanGesture gesture)
{
  if(!mAdapter || !mLayouter)
  {
    return;
  }

  const Vector2 position     = gesture.GetPosition();
  const bool    horizontal   = mLayouter.CanScrollHorizontally() && !mLayouter.CanScrollVertically();
  const float   mainPosition = horizontal ? position.x : position.y;

  switch(gesture.GetState())
  {
    case GestureState::STARTED:
    {
      if(mScrollAnimation)
      {
        SyncScrollOffsetFromScroller();
      }
      StopKeyRepeatTimer();
      CancelScrollAnimation();
      FinishEdgeEffects();
      ApplyScrollerPosition();
      mLastPanMainPosition = mainPosition;
      SendScrollStarted();
      SendDragStarted();
      break;
    }
    case GestureState::CONTINUING:
    {
      const float delta    = mLastPanMainPosition - mainPosition;
      mLastPanMainPosition = mainPosition;
      ScrollBy(delta, false);
      break;
    }
    case GestureState::FINISHED:
    {
      if(mScrollAnimation)
      {
        SyncScrollOffsetFromScroller();
      }

      SendDragFinished();

      const Vector2 velocityVector = gesture.GetVelocity();
      const float   velocity       = horizontal ? velocityVector.x : velocityVector.y;
      const float   speed          = std::abs(velocity);
      if(speed < FLING_VMIN)
      {
        ApplyScrollerPosition();
        UpdateScrollBar();
        ReleaseEdgeEffects(0.0f);
        SendScrollFinished();
        break;
      }

      const float distance = std::min(speed / FLING_K_DIST, mMaxFlingDistance);
      const float movement = -velocity / speed * distance;
      const float current  = mLayouter.ComputeScrollOffset();
      const float target   = ClampScrollOffset(current + movement);
      if(std::abs(target - current) < 0.5f && std::abs(movement) > 0.5f)
      {
        ApplyScrollerPosition();
        UpdateScrollBar();
        PullEdgeEffect(movement, 0.0f);
        ReleaseEdgeEffects(velocity);
        SendScrollFinished();
        break;
      }

      const float effectiveVStop = std::min(FLING_VSTOP, speed * 0.5f);
      float       durationMs     = std::log(speed / effectiveVStop) / FLING_K_DUR;
      durationMs                 = std::max(mMinimumFlingDuration, std::min(mMaximumFlingDuration, durationMs));

      CancelScrollAnimation();
      FinishEdgeEffects();
      StartScrollAnimation(target, durationMs / 1000.0f);
      // SendScrollFinished() called from OnScrollAnimationFinished.
      break;
    }
    case GestureState::CANCELLED:
    {
      if(mScrollAnimation)
      {
        SyncScrollOffsetFromScroller();
      }
      CancelScrollAnimation();
      ApplyScrollerPosition();
      UpdateScrollBar();
      ReleaseEdgeEffects(0.0f);
      if(mIsDragging)
      {
        SendDragFinished();
      }
      SendScrollFinished();
      break;
    }
    default:
    {
      break;
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
