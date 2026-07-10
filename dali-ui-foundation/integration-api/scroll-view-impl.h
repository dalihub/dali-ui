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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-notification.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/public-api/layouts/scroll-view-layout-manager.h>
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-view.h>

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
   * @brief Sets whether to automatically scroll to a child when it gains focus.
   */
  void SetScrollOnFocus(bool enable);

  /**
   * @brief Gets whether auto-scroll on focus is enabled.
   */
  bool GetScrollOnFocus() const;

  /**
   * @brief Sets the scroll target position used when auto-scrolling to a focused child.
   */
  void SetFocusScrollToPosition(ScrollToPosition position);

  /**
   * @brief Gets the scroll target position used when auto-scrolling to a focused child.
   */
  ScrollToPosition GetFocusScrollToPosition() const;

  /**
   * @brief Sets the peek distance applied when auto-scrolling to a focused child.
   */
  void SetFocusScrollPeek(float peek);

  /**
   * @brief Gets the peek distance applied when auto-scrolling to a focused child.
   */
  float GetFocusScrollPeek() const;

  /**
   * @brief Sets whether key-based step scrolling is enabled.
   *
   * When enabled, ScrollView intercepts focus navigation requests from children.
   * If the next focusable view is within mKeyScrollStep pixels it is focused normally;
   * otherwise the ScrollView scrolls by mKeyScrollStep instead.
   */
  void SetKeyScrollEnabled(bool enable);

  /**
   * @brief Gets whether key-based step scrolling is enabled.
   */
  bool IsKeyScrollEnabled() const;

  /**
   * @brief Sets the step distance (px) used for key-based step scrolling.
   *
   * Acts as both the proximity threshold (focus jumps if next item is within this
   * distance) and the scroll step size (scrolled by this amount when no nearby
   * focusable is found).
   */
  void SetKeyScrollStep(float step);

  /**
   * @brief Gets the step distance used for key-based step scrolling.
   */
  float GetKeyScrollStep() const;

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

  // Edge effects

  /**
   * @brief Sets the edge effect activated when the content is dragged past the start boundary.
   *
   * For Vertical scrolling this is the top edge; for Horizontal it is the left edge.
   */
  void SetStartEdgeEffect(Ui::EdgeEffect effect);

  /**
   * @brief Gets the start-edge effect.
   */
  Ui::EdgeEffect GetStartEdgeEffect() const;

  /**
   * @brief Sets the edge effect activated when the content is dragged past the end boundary.
   *
   * For Vertical scrolling this is the bottom edge; for Horizontal it is the right edge.
   */
  void SetEndEdgeEffect(Ui::EdgeEffect effect);

  /**
   * @brief Gets the end-edge effect.
   */
  Ui::EdgeEffect GetEndEdgeEffect() const;
  // Viewport query

  /**
   * @brief Returns the current viewport width (pixels).
   *
   * Reflects the last layout pass; zero before first layout.
   */
  float GetViewportWidth() const;

  /**
   * @brief Returns the current viewport height (pixels).
   *
   * Reflects the last layout pass; zero before first layout.
   */
  float GetViewportHeight() const;

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
  Vector2 AdjustDelta(const Vector2& movement, const Vector2& currentPosition);

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

  /**
   * @brief Cancels any in-progress scroll/fling animation.
   *
   * After this call the content position is frozen at its current rendered
   * position, which may differ from mScrollPosition if an animation was
   * mid-flight.
   */
  void CancelScrollAnimation();

  /**
   * @brief Scrolls to @p position over @p durationSec seconds.
   *
   * Uses an EASE_OUT_SINE curve.  If durationSec <= 0 the content is snapped
   * immediately with no animation.
   */
  void ScrollToWithDuration(const Vector2& position, float durationSec);

  /**
   * @brief Called inside OnDragFinished just before the scroll/fling animation starts.
   *
   * @p targetPosition is the computed fling destination (or the current scroll
   * position when there is no fling).  @p durationSec is the computed fling
   * duration in seconds (0 when there is no fling).
   *
   * Subclasses may override to redirect the animation to a snap point (e.g.
   * page boundaries).  Modifying @p targetPosition and/or @p durationSec here
   * takes effect for the animation that is about to be started.
   *
   * The default implementation is a no-op.
   *
   * @note ABI note: do NOT reorder or remove this method after first publication.
   */
  virtual void OnBeforeScrollAnimation(Vector2& targetPosition, float& durationSec);

  /**
   * @brief Called whenever the scrollable area dimensions change.
   *
   * Fired by SetScrollableWidth() and SetScrollableHeight() after
   * UpdateScrollingProperties() has been applied — i.e. the new bounds
   * are already in effect when this is invoked.
   *
   * Subclasses may override to react to layout-driven dimension changes
   * (e.g. PageScrollViewImpl uses this to re-emit PageChangedSignal when
   * the layout-computed page count first becomes accurate after the initial
   * layout pass).
   *
   * The default implementation is a no-op.
   *
   * @note ABI note: do NOT reorder or remove this method after first publication.
   */
  virtual void OnScrollableAreaChanged();

private:
  /**
   * @brief Callback for child relayouted.
   */
  void OnChildRelayout(Actor actor);

  /**
   * @brief Intercepts touch events before children receive them.
   *
   * Feeds the raw touch to the PanGestureDetector via HandleEvent and tracks
   * cumulative displacement. Returns true (steals touch from children) once
   * the displacement exceeds mPanThreshold, ensuring scroll works even when
   * children consume touch events.
   */
  bool OnInterceptTouch(Actor actor, TouchEvent touch);

  /**
   * @brief Handles touch events on the ScrollView after interception.
   *
   * Called for subsequent events once OnInterceptTouch has returned true.
   * Feeds each event to the PanGestureDetector via HandleEvent.
   */
  bool OnTouch(Actor actor, TouchEvent touch);

  /**
   * @brief Handles mouse wheel events for scrolling.
   */
  bool OnWheelEvent(Actor actor, WheelEvent event);

  /**
   * @brief Callback for pan gesture detection.
   */
  void OnPanGesture(Actor actor, PanGesture gesture);

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
   * @brief Callback for scroll animation finished.
   */
  void OnScrollAnimationFinished(Animation animation);

  /**
   * @brief Checks if can scroll horizontally.
   */
  static bool CanScrollHorizontally(ScrollDirection direction);

  /**
   * @brief Checks if can scroll vertically.
   */
  static bool CanScrollVertically(ScrollDirection direction);

  /**
   * @brief Resolves which view actually receives focus when FocusManager targets Self().
   *
   * Called by FocusManager after Step 1 selects this ScrollView as a candidate.
   * Returns the content item nearest to the entry edge if it is already visible in
   * the current viewport (external-entry shortcut), or Self() when step-scrolling is
   * needed, or delegates to the base class when key-scroll is disabled.
   */
  View OnFocusRequested() override;

  bool OnKeyEvent(const Dali::KeyEvent& event) override;

  /**
   * @brief Intercepts focus navigation for content children.
   *
   * Called by FocusManager (Step 1 of MoveFocus pipeline) when the current
   * focused view is a descendant of this ScrollView. If key-scroll is enabled,
   * four cases apply:
   *
   *  !next AND at scroll boundary:
   *    Return View() so FocusFinder can exit to a neighboring view (e.g. Ext Bottom).
   *  Case 3 — no next focusable AND not at scroll boundary:
   *    Transfer focus to Self() (if FOCUSABLE) so OnKeyEvent drives
   *    further step-scrolling. Sets mKeyScrollLastChild/Dir to suppress re-selection
   *    of content items while in this blind-scroll mode.
   *  Case 4 — next focusable beyond mKeyScrollStep:
   *    Scroll one step, return currentFocusedView to keep focus in place and
   *    block FocusFinder from jumping further.
   *  Case 5 — next focusable within mKeyScrollStep (or at scroll boundary):
   *    Return next directly; ScrollOnFocus animates it into full view.
   */
  View OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction) override;

  /**
   * @brief Returns true if `child` (a content descendant) overlaps the current viewport.
   *
   * Uses rectangle intersection so partial visibility counts. This is used as
   * the trigger for transferring keyboard focus to a content item: we only focus
   * the item when it is actually visible, rather than pre-focusing off-screen items
   * and relying on scroll-on-focus to catch up.
   */
  bool IsChildInViewport(View child) const;

  /**
   * @brief Callback invoked when the focused view changes.
   *
   * Scrolls to the newly focused view if it is a descendant of mContent
   * and mScrollOnFocus is enabled.
   */
  void OnFocusManagerChanged(View from, View to);

  /**
   * @brief Returns true if @p view is a descendant of mContent.
   */
  bool IsDescendantOfContent(View view) const;

  /**
   * @brief Returns true if @p direction is along the ScrollView's scroll axis.
   */
  bool IsDirectionCompatible(FocusDirection direction) const;

  /**
   * @brief Finds the nearest focusable descendant of mContent in @p direction
   * from @p currentFocusedView. Returns an empty handle if none found.
   */
  View FindNextFocusableInContent(View currentFocusedView, const Vector2& currentPos, FocusDirection direction) const;

  /**
   * @brief Recursively collects the nearest focusable candidate in @p direction.
   */
  void CollectNextFocusCandidate(View container, View excludeView, const Vector2& currentPos,
                                 FocusDirection direction, View& bestView, float& bestDist) const;

  /**
   * @brief Returns true if the scroll position is already at the boundary in
   * @p direction and scrolling further would be a no-op.
   */
  bool IsAtScrollBoundary(FocusDirection direction) const;

  /**
   * @brief Fires a one-shot absorb edge effect when a key press hits a scroll boundary.
   */
  void TriggerKeyEdgeFeedback(FocusDirection direction);

  /**
   * @brief Scrolls by mKeyScrollStep in the given focus direction.
   * PAGE_UP / PAGE_DOWN scrolls by the full viewport size.
   */
  void ScrollByKeyDirection(FocusDirection direction);

  /**
   * @brief Scrolls one page in @p direction and focuses the topmost (PAGE_DOWN) or
   * bottommost (PAGE_UP) focusable item in the destination viewport.
   *
   * Returns the item to focus, Self() when no item is found but ScrollView is
   * keyboard-focusable, or View() otherwise.  Always clears mKeyScrollLastChild.
   */
  View PageScrollAndFocus(FocusDirection direction);

  /**
   * @brief Scrolls to the very start (toEnd=false) or end (toEnd=true) of content and
   * focuses the topmost (Home) or bottommost (End) focusable item in the destination
   * viewport.  Falls back to Self() or View() as in PageScrollAndFocus.
   *
   * Mirrors Android ScrollView behavior for KEYCODE_MOVE_HOME / KEYCODE_MOVE_END.
   */
  View FullScrollAndFocus(bool toEnd);

  /**
   * @brief Returns the signed axis distance from @p from to @p to in @p direction.
   * Positive means @p to is ahead in that direction.
   */
  static float FocusAxisDistance(const Vector2& from, const Vector2& to, FocusDirection direction);

  /**
   * @brief Returns true if @p to is strictly ahead of @p from in @p direction.
   */
  static bool IsAheadInDirection(const Vector2& from, const Vector2& to, FocusDirection direction);

  /**
   * @brief Callback for content relayout.
   */
  void OnContentRelayout();

  /**
   * @brief Callback for content position change (during animation).
   */
  void OnContentPositionChanged(PropertyNotification source);

  /**
   * @brief Callback for layout direction changes.
   *
   * Manually propagates the new direction to the ScrollBar, which is a
   * STANDALONE child and therefore excluded from the automatic
   * ApplyLayoutDirection traversal.
   */
  void OnLayoutDirectionChanged(Actor actor, LayoutDirection::Type type);

private:
  // Not copyable or movable
  ScrollViewImpl(const ScrollViewImpl&)            = delete;
  ScrollViewImpl(ScrollViewImpl&&)                 = delete;
  ScrollViewImpl& operator=(const ScrollViewImpl&) = delete;
  ScrollViewImpl& operator=(ScrollViewImpl&&)      = delete;

  // Accumulates timestamped touch positions and computes a weighted-average
  // velocity over a recent time window.  Velocity is in pixels per millisecond.
  struct VelocityTracker
  {
    struct Sample
    {
      uint32_t timeMs{0u};
      Vector2  pos{};
    };
    static constexpr int      CAPACITY   = 20;
    static constexpr uint32_t HORIZON_MS = 100u;  // look-back window
    static constexpr float    WEIGHT_TAU = 30.0f; // exponential decay constant (ms)

    Sample samples[CAPACITY]{};
    int    writeIdx{0};
    int    count{0};

    void Clear()
    {
      count    = 0;
      writeIdx = 0;
    }

    void Add(uint32_t timeMs, const Vector2& pos)
    {
      samples[writeIdx] = {timeMs, pos};
      writeIdx          = (writeIdx + 1) % CAPACITY;
      if(count < CAPACITY) ++count;
    }

    // Weighted average of consecutive-sample velocities; recent samples have
    // higher weight (exp(-age/WEIGHT_TAU)).  Returns px/ms.
    Vector2 Compute() const;
  };

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

  // Focus-scroll behaviour
  bool             mScrollOnFocus;         ///< Auto-scroll to child when it gains focus
  ScrollToPosition mFocusScrollToPosition; ///< Target position for focus-triggered scroll
  float            mFocusScrollPeek;       ///< Extra scroll distance past the item edge on focus (MakeVisible only)

  // Key-scroll behaviour
  bool           mKeyScrollEnabled;   ///< Whether key-based step scrolling is active
  float          mKeyScrollStep;      ///< Proximity threshold and step size for key scrolling (px)
  View           mKeyScrollLastChild; ///< Content child that last triggered a case-3 focus transfer to Self()
  FocusDirection mKeyScrollLastDir;   ///< Direction of that transfer (only valid when mKeyScrollLastChild is set)

  // Scroll bar visibility
  ScrollBarVisibility mVerticalScrollBarVisibility;   ///< Vertical scroll bar visibility
  ScrollBarVisibility mHorizontalScrollBarVisibility; ///< Horizontal scroll bar visibility

  // Animation
  Animation mScrollAnimation; ///< Fling scroll animation

  // Pan gesture
  PanGestureDetector mPanGestureDetector; ///< Pan gesture detector
  float              mPanThreshold;       ///< Additional displacement threshold (px) after Pan is recognized
  Vector2            mLastPanPosition;    ///< Last pan position
  VelocityTracker    mVelocityTracker;    ///< Touch-history velocity tracker for fling

  // Intercept touch state
  bool    mIntercepting;     ///< True while this ScrollView owns the touch sequence
  bool    mJustIntercepted;  ///< True for one cycle when interception first begins (prevents double HandleEvent feed)
  bool    mPanRecognized;    ///< True once PanGestureDetector fires STARTED; waiting for post-recognition threshold
  bool    mIsDragging;       ///< True between SendDragStarted() and SendDragFinished(); guards FINISHED handler against mIntercepting timing race
  bool    mDisambiguating;   ///< True from touch DOWN until disambiguation resolves; guards DisambiguationEnded against double-emit
  Vector2 mPanStartPosition; ///< Screen position at Pan STARTED, used to measure post-recognition displacement

  // Scroll bar
  ScrollBar mScrollBar; ///< Scroll bar for visual indication of scroll position

  // Edge effects — primary axis (vertical for Vertical/Both, horizontal for Horizontal)
  Ui::EdgeEffect mStartEdgeEffect;        ///< Effect at start boundary (top for Vertical/Both, left for Horizontal)
  Ui::EdgeEffect mEndEdgeEffect;          ///< Effect at end boundary (bottom for Vertical/Both, right for Horizontal)
  bool           mStartEdgeActive{false}; ///< True while start edge effect is pulling
  bool           mEndEdgeActive{false};   ///< True while end edge effect is pulling
  float          mEdgeDisplacement{0.0f}; ///< Accumulated over-scroll displacement for the active primary effect

  // Property notification for content position changes during animation
  PropertyNotification mContentPositionNotification; ///< Notifies when content position changes

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
