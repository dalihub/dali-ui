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

// EXTERNAL INCLUDES
// #include <dali/public-api/adaptor-framework/pan-gesture-detector.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector2.h>
// #include <dali/public-api/signals/signal.h>
#include <dali-ui-foundation/public-api/scroll-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/layouts/scroll-view-layout-manager.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class ScrollViewImpl;
using ScrollViewImplPtr = IntrusivePtr<ScrollViewImpl>;
/**
 * @brief This is the internal implementation class for ScrollView.
 *
 * ScrollViewImpl extends ViewImpl and provides pan gesture detection
 * for scrolling content, based on OneUI Scrollable component design.
 *
 * @see Dali::UI::ScrollView
 */
class DALI_UI_API ScrollViewImpl : public LayoutImpl
{
public:
  // Creation & Destruction

  /**
   * @brief Creates a new ScrollView.
   */
  static ScrollViewImplPtr New();

protected:
  /**
   * @brief Destructor.
   */
  virtual ~ScrollViewImpl();

  // Construction

  /**
   * @brief ScrollView constructor.
   */
  ScrollViewImpl();

public: // From Ui::Internal::Control
  /**
   * @copydoc Ui::Internal::Control::OnInitialize
   */
  void OnInitialize() override;

public: // API
  /**
   * @brief Sets the content view.
   */
  void SetContent(View content);

  /**
   * @brief Gets the content view.
   */
  View GetContent() const;

  /**
   * @brief Gets the scroll position.
   */
  Vector2 GetScrollPosition() const;

  /**
   * @brief Sets the scroll position.
   */
  void SetScrollPosition(const Vector2& position);

  /**
   * @brief Checks if scrolling is in progress.
   */
  bool IsScrolling() const;

  /**
   * @brief Gets the scrollable width.
   */
  float GetScrollableWidth() const;

  /**
   * @brief Sets the scrollable width.
   */
  void SetScrollableWidth(float width);

  /**
   * @brief Gets the scrollable height.
   */
  float GetScrollableHeight() const;

  /**
   * @brief Sets the scrollable height.
   */
  void SetScrollableHeight(float height);

  /**
   * @brief Gets the scroll direction.
   */
  ScrollDirection GetScrollDirection() const;

  /**
   * @brief Sets the scroll direction.
   */
  void SetScrollDirection(ScrollDirection direction);

  /**
   * @brief Gets the maximum fling distance.
   */
  float GetMaxFlingDistance() const;

  /**
   * @brief Sets the maximum fling distance.
   */
  void SetMaxFlingDistance(float distance);

  /**
   * @brief Gets the minimum duration of fling scroll animation.
   */
  int GetMinimumFlingDuration() const;

  /**
   * @brief Sets the minimum duration of fling scroll animation.
   */
  void SetMinimumFlingDuration(int duration);

  /**
   * @brief Gets the maximum duration of fling scroll animation.
   */
  int GetMaximumFlingDuration() const;

  /**
   * @brief Sets the maximum duration of fling scroll animation.
   */
  void SetMaximumFlingDuration(int duration);

  /**
   * @brief Gets the fling sensitivity.
   */
  float GetFlingSensitivity() const;

  /**
   * @brief Sets the fling sensitivity.
   */
  void SetFlingSensitivity(float sensitivity);

  /**
   * @brief Gets the deceleration rate.
   */
  float GetDecelerationRate() const;

  /**
   * @brief Sets the deceleration rate.
   */
  void SetDecelerationRate(float rate);

  /**
   * @brief Gets the over scroll mode.
   */
  OverScrollMode GetOverScrollMode() const;

  /**
   * @brief Sets the over scroll mode.
   */
  void SetOverScrollMode(OverScrollMode mode);

  /**
   * @brief Scrolls to the specified position.
   */
  void ScrollTo(const Vector2& position, bool animation);

  /**
   * @brief Scrolls to the specified child view.
   */
  void ScrollTo(View child, bool animation, ScrollToPosition scrollToPosition);

  /**
   * @brief Scrolls to the specified X position.
   */
  void ScrollToX(float position, bool animation);

  /**
   * @brief Scrolls to the specified Y position.
   */
  void ScrollToY(float position, bool animation);

  /**
   * @brief Gets the vertical scroll bar visibility.
   */
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  /**
   * @brief Sets the vertical scroll bar visibility.
   */
  void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Gets the horizontal scroll bar visibility.
   */
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

  /**
   * @brief Sets the horizontal scroll bar visibility.
   */
  void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

  // Signals

  /**
   * @brief Gets the scroll started signal.
   */
  Ui::ScrollView::ScrollStartedSignalType& ScrollStartedSignal();

  /**
   * @brief Gets the scrolling signal.
   */
  Ui::ScrollView::ScrollingSignalType& ScrollingSignal();

  /**
   * @brief Gets the scroll finished signal.
   */
  Ui::ScrollView::ScrollFinishedSignalType& ScrollFinishedSignal();

  /**
   * @brief Gets the drag started signal.
   */
  Ui::ScrollView::DragStartedSignalType& DragStartedSignal();

  /**
   * @brief Gets the dragging signal.
   */
  Ui::ScrollView::DraggingSignalType& DraggingSignal();

  /**
   * @brief Gets the drag finished signal.
   */
  Ui::ScrollView::DragFinishedSignalType& DragFinishedSignal();

protected:
  /**
   * @brief Converts velocity to movement distance.
   */
  Vector2 VelocityToMovement(const Vector2& velocity) const;

  /**
   * @brief Adjusts movement based on scroll direction.
   */
  Vector2 AdjustMovement(const Vector2& movement) const;

  /**
   * @brief Adjusts delta to stay within bounds.
   */
  Vector2 AdjustDelta(const Vector2& movement, const Vector2& currentPosition = Vector2::ZERO);

  /**
   * @brief Adjusts scroll position to valid range.
   */
  Vector2 AdjustScrollPosition(const Vector2& position) const;

  /**
   * @brief Gets scroll position for a child view.
   */
  Vector2 GetScrollPositionForChild(View child, Vector2 current) const;

  /**
   * @brief Converts content position to scroll position.
   */
  Vector2 ContentPositionToScrollPosition(const Vector2& content) const;

  /**
   * @brief Converts scroll position to delta.
   */
  Vector2 DeltaFromScrollPosition(const Vector2& scrollPosition) const;

private:
  /**
   * @brief Callback for child relayouted.
   */
  void OnChildRelayout(Actor actor);

  /**
   * @brief Callback for pan gesture detection.
   */
  void OnPanGesture(Actor actor, const PanGesture& gesture);

  /**
   * @brief Handles drag started.
   */
  void OnDragStarted(const PanGesture& gesture);

  /**
   * @brief Handles dragging.
   */
  void OnDragging(const PanGesture& gesture);

  /**
   * @brief Handles drag finished.
   */
  void OnDragFinished(const PanGesture& gesture);

  /**
   * @brief Sends scroll started signal.
   */
  void SendScrollStarted();

  /**
   * @brief Sends scrolling signal.
   */
  void SendScrolling();

  /**
   * @brief Sends scroll finished signal.
   */
  void SendScrollFinished();

  /**
   * @brief Sends drag started signal.
   */
  void SendDragStarted();

  /**
   * @brief Sends dragging signal.
   */
  void SendDragging(float deltaX, float deltaY);

  /**
   * @brief Sends drag finished signal.
   */
  void SendDragFinished();

  /**
   * @brief Updates scrolling properties.
   */
  void UpdateScrollingProperties();

  /**
   * @brief Applies scroll position to content.
   */
  void ApplyScrollPosition(const Vector2& position);

  /**
   * @brief Cancels scroll animation.
   */
  void CancelScrollAnimation();

  /**
   * @brief Callback for scroll animation finished.
   */
  void OnScrollAnimationFinished(Animation& animation);

  /**
   * @brief Checks if can scroll horizontally.
   */
  static bool CanScrollHorizontally(ScrollDirection direction);

  /**
   * @brief Checks if can scroll vertically.
   */
  static bool CanScrollVertically(ScrollDirection direction);

  /**
   * @brief Callback for content relayout.
   */
  void OnContentRelayout();

private:
  // Not copyable or movable
  ScrollViewImpl(const ScrollViewImpl&)            = delete;
  ScrollViewImpl(ScrollViewImpl&&)                 = delete;
  ScrollViewImpl& operator=(const ScrollViewImpl&) = delete;
  ScrollViewImpl& operator=(ScrollViewImpl&&)      = delete;

private:
  // Data
  View            mContent;              ///< The content view
  Vector2         mScrollPosition;       ///< Current scroll position
  Vector2         mCurrentPosition;      ///< Current content position
  float           mScrollableWidth;      ///< Scrollable content width
  float           mScrollableHeight;     ///< Scrollable content height
  ScrollDirection mScrollDirection;      ///< Scroll direction
  float           mMaxFlingDistance;     ///< Maximum fling distance
  int             mMinimumFlingDuration; ///< Minimum fling duration
  int             mMaximumFlingDuration; ///< Maximum fling duration
  float           mFlingSensitivity;     ///< Fling sensitivity
  float           mDecelerationRate;     ///< Deceleration rate
  OverScrollMode  mOverScrollMode;       ///< Over scroll mode
  bool            mIsScrolling;          ///< Is scrolling in progress
  float           mViewportWidth;        ///< Viewport width
  float           mViewportHeight;       ///< Viewport height
  float           mMaximumStartY;        ///< Maximum start Y position
  float           mMaximumStartX;        ///< Maximum start X position
  float           mMinimumStartY;        ///< Minimum start Y position
  float           mMinimumStartX;        ///< Minimum start X position
  bool            mHasScrollableArea;    ///< Has scrollable area

  // Scroll bar visibility
  ScrollBarVisibility mVerticalScrollBarVisibility;   ///< Vertical scroll bar visibility
  ScrollBarVisibility mHorizontalScrollBarVisibility; ///< Horizontal scroll bar visibility

  // Animation
  Animation mScrollAnimation; ///< Fling scroll animation

  // Pan gesture
  PanGestureDetector mPanGestureDetector; ///< Pan gesture detector
  Vector2            mTotalDisplacement;  ///< Total displacement
  float              mPanThreshold;       ///< Pan gesture threshold
  bool               mIsThresholdMet;     ///< Threshold met flag
  Vector2            mStartPanPosition;   ///< Start pan position
  Vector2            mLastPanPosition;    ///< Last pan position

  // Signals
  Ui::ScrollView::ScrollStartedSignalType  mScrollStartedSignal;
  Ui::ScrollView::ScrollFinishedSignalType mScrollFinishedSignal;
  Ui::ScrollView::ScrollingSignalType      mScrollingSignal;
  Ui::ScrollView::DragStartedSignalType    mDragStartedSignal;
  Ui::ScrollView::DragFinishedSignalType   mDragFinishedSignal;
  Ui::ScrollView::DraggingSignalType       mDraggingSignal;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
