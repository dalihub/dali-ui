#pragma once

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

#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/signals/dali-signal.h>
#include <limits>
#include <memory>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class RecyclerViewImpl;
using RecyclerViewImplPtr = IntrusivePtr<RecyclerViewImpl>;

class DALI_UI_API RecyclerViewImpl : public LayoutImpl
{
public:
  static RecyclerViewImplPtr New();

protected:
  RecyclerViewImpl();
  ~RecyclerViewImpl() override;

public:
  void OnInitialize() override;

  void         SetAdapter(ItemAdapter& adapter);
  ItemAdapter* GetAdapter() const;
  void         ClearAdapter();

  void          SetItemsLayouter(ItemsLayouter layouter);
  ItemsLayouter GetItemsLayouter() const;

  void SetCacheExtent(float before, float after);
  void GetCacheExtent(float& before, float& after) const;

  void  ScrollToPosition(uint32_t position, bool animation);
  void  ScrollBy(float distance, bool animation);
  void  SetScrollOffset(float offset);
  float GetScrollOffset() const;

  uint32_t GetFirstVisiblePosition() const;
  uint32_t GetLastVisiblePosition() const;

  void                SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  void                SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

  void           SetOverScrollMode(OverScrollMode mode);
  OverScrollMode GetOverScrollMode() const;

  void       SetStartEdgeEffect(EdgeEffect effect);
  EdgeEffect GetStartEdgeEffect() const;

  void       SetEndEdgeEffect(EdgeEffect effect);
  EdgeEffect GetEndEdgeEffect() const;

  bool IsScrolling() const;

  RecyclerView::ScrollStartedSignalType&  ScrollStartedSignal();
  RecyclerView::ScrollFinishedSignalType& ScrollFinishedSignal();
  RecyclerView::DragStartedSignalType&    DragStartedSignal();
  RecyclerView::DragFinishedSignalType&   DragFinishedSignal();

  void  SetKeyScrollEnabled(bool enable);
  bool  IsKeyScrollEnabled() const;
  void  SetKeyScrollStep(float step);
  float GetKeyScrollStep() const;

  void  SetScrollOnFocus(bool enable);
  bool  GetScrollOnFocus() const;
  void  SetFocusScrollPeek(float peek);
  float GetFocusScrollPeek() const;

protected:
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

  View OnFocusRequested() override;
  View OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction) override;
  bool OnKeyEvent(const Dali::KeyEvent& event) override;

private:
  // Provides the Recycler interface to ItemsLayouter; defined in the .cpp.
  struct RecyclerImpl;

  void OnAdapterDestroyed(ItemAdapter& adapter);
  void OnAdapterDataChanged(const ItemAdapter::ChangeInfo& info);
  void OnLayoutInvalidated();

  struct ItemRecord
  {
    uint32_t position{0u};
    uint32_t viewType{0u};
    View     view;
  };

  void  EnsureScroller();
  void  UpdateScrollerSize();
  void  UpdateScrollBar();
  void  RecycleAll();
  void  RecycleRecord(size_t index);
  View  ObtainItemView(uint32_t position, uint32_t viewType);
  float GetViewportExtent() const;
  float GetCrossExtent() const;
  float GetMaxScrollOffset() const;
  float ClampScrollOffset(float offset) const;
  float SyncScrollOffsetFromScroller();
  float CalculateScrollDuration(float distance) const;
  void  StartScrollAnimation(float targetOffset, float durationSeconds);
  bool  CanOverScroll() const;
  void  EnsureDefaultEdgeEffects();
  void  UpdateEdgeEffectSources();
  void  FinishEdgeEffects();
  void  ReleaseEdgeEffects(float velocity);
  void  PullEdgeEffect(float requestedDelta, float consumedDelta);
  void  ApplyScrollerPosition();
  void  CancelScrollAnimation();

  void OnScrollerPositionChanged(PropertyNotification source);
  void OnScrollAnimationFinished(Animation animation);
  bool OnWheelEvent(Actor actor, WheelEvent event);
  void OnPanGesture(Actor actor, PanGesture gesture);

  // Scroll state helpers — mirror ScrollView's Send* pattern.
  // AbortScroll() forcefully resets state (e.g. adapter swap, destructor)
  // without emitting public signals.
  void SendScrollStarted();
  void SendScrollFinished();
  void SendDragStarted();
  void SendDragFinished();
  void AbortScroll();

  // Key-scroll helpers.
  void     ScrollToItemMakeVisible(uint32_t position, bool animate);
  void     TriggerKeyEdgeFeedback(FocusDirection dir);
  bool     IsLayoutAxisDirection(FocusDirection dir) const;
  bool     IsForwardDirection(FocusDirection dir) const;
  bool     IsAtScrollBoundary(FocusDirection dir) const;
  uint32_t FindActiveItemPosition(View view) const;
  View     FindActiveView(uint32_t position) const;
  uint32_t NextItemPosition(uint32_t pos, FocusDirection dir) const;
  void     StartKeyRepeatTimer();
  void     StopKeyRepeatTimer();
  bool     OnKeyRepeatTimerTick();
  void     OnFocusManagerChanged(View from, View to);

private:
  ItemAdapter*                  mAdapter;
  ItemsLayouter                 mLayouter;
  std::unique_ptr<RecyclerImpl> mRecyclerImpl;
  View                          mScroller;
  ScrollBar                     mScrollBar;

  std::vector<ItemRecord> mActiveItems;
  std::vector<ItemRecord> mRecycledItems;

  Animation            mScrollAnimation;
  PropertyNotification mScrollerPositionNotification;
  PanGestureDetector   mPanGestureDetector;

  float mViewportWidth;
  float mViewportHeight;
  float mCacheBefore;
  float mCacheAfter;
  float mLastPanMainPosition;
  float mMaxFlingDistance;
  float mMinimumFlingDuration;
  float mMaximumFlingDuration;

  EdgeEffect mStartEdgeEffect;
  EdgeEffect mEndEdgeEffect;
  bool       mUsingDefaultEdgeEffects;
  bool       mStartEdgeActive;
  bool       mEndEdgeActive;
  float      mEdgeDisplacement;

  ScrollBarVisibility mVerticalScrollBarVisibility;
  ScrollBarVisibility mHorizontalScrollBarVisibility;
  OverScrollMode      mOverScrollMode;

  bool mIsScrolling{false};
  bool mIsDragging{false};

  RecyclerView::ScrollStartedSignalType  mScrollStartedSignal;
  RecyclerView::ScrollFinishedSignalType mScrollFinishedSignal;
  RecyclerView::DragStartedSignalType    mDragStartedSignal;
  RecyclerView::DragFinishedSignalType   mDragFinishedSignal;

  // Key-scroll config.
  bool  mKeyScrollEnabled{false};
  float mKeyScrollStep{200.0f};
  bool  mScrollOnFocus{true};
  float mFocusScrollPeek{0.0f};

  // Key-repeat accumulation: target item position to focus once it becomes active.
  static constexpr uint32_t INVALID_ITEM_POSITION = std::numeric_limits<uint32_t>::max();
  uint32_t                  mKeyRepeatTargetPos{INVALID_ITEM_POSITION};
  FocusDirection            mKeyRepeatDir{FocusDirection::DOWN};
  Timer                     mKeyRepeatTimer;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
