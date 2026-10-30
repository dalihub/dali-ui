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

#ifndef DALI_UI_ELEMENTS_SCROLL_VIEW_H
#define DALI_UI_ELEMENTS_SCROLL_VIEW_H

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/math/vector2.h>

namespace DALI_NAMESPACE
{

namespace Ui
{
namespace Integration
{
class ScrollViewImpl;
}

/**
 * @brief ScrollView is a scrollable container that displays content larger than its viewport.
 *
 * ScrollView provides pan gesture support for navigating content that exceeds the view's
 * visible area, based on OneUI Scrollable component design.
 *
 * Key features:
 * - Pan gesture detection for scrolling
 * - Configurable scroll direction (vertical/horizontal/both)
 * - Content management
 * - Scroll position control with animation
 * - Fling/inertia scrolling with velocity tracking
 * - Edge effects for over-scrolling
 * - Scroll event signals
 * - Drag event signals
 *
 * The content rests at this view's padding (View::SetPadding()). Like every layout length the
 * padding is in natural units and is multiplied by the view's effective scale.
 *
 * Basic usage:
 * @code
 * // Create a ScrollView
 * ScrollView scrollView = ScrollView::New();
 * scrollView.SetRequestedWidth(400.0f);
 * scrollView.SetRequestedHeight(300.0f);
 *
 * // Create content view larger than the scroll view
 * View content = View::New();
 * content.SetRequestedWidth(800.0f);
 * content.SetRequestedHeight(600.0f);
 * content.BackgroundColor(Color::WHITE);
 * scrollView.SetContent(content);
 *
 * // Connect to scroll signals
 * scrollView.ScrollStartedSignal().Connect(this, &MyClass::OnScrollStarted);
 * scrollView.ScrollingSignal().Connect(this, &MyClass::OnScrolling);
 * scrollView.ScrollFinishedSignal().Connect(this, &MyClass::OnScrollFinished);
 * @endcode
 * @SINCE_3_0.0
 */
class DALI_UI_API ScrollView : public View
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized ScrollView handle.
   * @SINCE_3_0.0
   */
  ScrollView();

  /**
   * @brief Creates an initialized ScrollView.
   *
   * @SINCE_3_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static ScrollView New();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  ScrollView(const ScrollView& scrollView);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  ScrollView(ScrollView&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~ScrollView();

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  ScrollView& operator=(const ScrollView& handle);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  ScrollView& operator=(ScrollView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(ScrollView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to ScrollView handle.
   *
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Layout or an uninitialized handle
   */
  static ScrollView DownCast(BaseHandle handle);

public: // Setters for chaining
  /**
   * @brief Sets the content view to be scrolled.
   *
   * @SINCE_3_0.0
   * @param[in] content The content view
   */
  void SetContent(View content);

  /**
   * @brief Gets the content view.
   *
   * @SINCE_3_0.0
   * @return The content view
   */
  View GetContent() const;

  /**
   * @brief Sets the scroll position.
   *
   * @SINCE_3_0.0
   * @param[in] position The scroll position to set
   */
  void SetScrollPosition(const Vector2& position);

  /**
   * @brief Gets the current scroll position.
   *
   * @SINCE_3_0.0
   * @return The current scroll position
   */
  Vector2 GetScrollPosition() const;

  /**
   * @brief Sets the direction of the scroll.
   *
   * @SINCE_3_0.0
   * @param[in] direction The scroll direction
   */
  void SetScrollDirection(ScrollDirection direction);

  /**
   * @brief Gets the direction of the scroll.
   *
   * @SINCE_3_0.0
   * @return The scroll direction
   */
  ScrollDirection GetScrollDirection() const;

  /**
   * @brief Sets whether touch and mouse pan gestures can scroll the content.
   *
   * Disabling pan scrolling leaves programmatic scrolling APIs such as
   * ScrollTo() available. This is useful while a child view owns an in-scene
   * drag gesture and the ScrollView should move only through application
   * controlled edge auto-scroll.
   *
   * If a pan scroll is active when this is disabled, the interaction is
   * stopped without starting a fling.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to enable pan scrolling (default: true)
   */
  void SetPanScrollEnabled(bool enabled);

  /**
   * @brief Gets whether touch and mouse pan gestures can scroll the content.
   *
   * @SINCE_3_0.0
   * @return True if pan scrolling is enabled
   */
  bool IsPanScrollEnabled() const;

  /**
   * @brief Sets the maximum distance by fling action.
   *
   * @SINCE_3_0.0
   * @param[in] distance The maximum fling distance
   */
  void SetMaxFlingDistance(float distance);

  /**
   * @brief Gets the maximum distance by fling action.
   *
   * @SINCE_3_0.0
   * @return The maximum fling distance
   */
  float GetMaxFlingDistance() const;

  /**
   * @brief Sets the minimum duration of fling scroll animation.
   *
   * @SINCE_3_0.0
   * @param[in] duration The minimum fling duration
   */
  void SetMinimumFlingDuration(int duration);

  /**
   * @brief Gets the minimum duration of fling scroll animation.
   *
   * @SINCE_3_0.0
   * @return The minimum fling duration
   */
  int GetMinimumFlingDuration() const;

  /**
   * @brief Sets the maximum duration of fling scroll animation.
   *
   * @SINCE_3_0.0
   * @param[in] duration The maximum fling duration
   */
  void SetMaximumFlingDuration(int duration);

  /**
   * @brief Gets the maximum duration of fling scroll animation.
   *
   * @SINCE_3_0.0
   * @return The maximum fling duration
   */
  int GetMaximumFlingDuration() const;

  /**
   * @brief Sets the sensitivity of fling action.
   *
   * @SINCE_3_0.0
   * @param[in] sensitivity The fling sensitivity
   */
  void SetFlingSensitivity(float sensitivity);

  /**
   * @brief Gets the sensitivity of fling action.
   *
   * @SINCE_3_0.0
   * @return The fling sensitivity
   */
  float GetFlingSensitivity() const;

  /**
   * @brief Sets the deceleration rate of fling.
   *
   * @SINCE_3_0.0
   * @param[in] rate The deceleration rate
   */
  void SetDecelerationRate(float rate);

  /**
   * @brief Gets the deceleration rate of fling.
   *
   * @SINCE_3_0.0
   * @return The deceleration rate
   */
  float GetDecelerationRate() const;

  /**
   * @brief Sets the over scroll mode.
   *
   * @SINCE_3_0.0
   * @param[in] mode The over scroll mode
   */
  void SetOverScrollMode(OverScrollMode mode);

  /**
   * @brief Gets the over scroll mode.
   *
   * @SINCE_3_0.0
   * @return The over scroll mode
   */
  OverScrollMode GetOverScrollMode() const;

public: // Focus Scroll
  /**
   * @brief Sets whether to automatically scroll to a child view when it gains focus.
   *
   * When enabled, any descendant of the scroll content that gains keyboard focus
   * will be scrolled into view according to the position set by SetFocusScrollToPosition().
   *
   * @SINCE_3_0.0
   * @param[in] enable True to enable auto-scroll on focus (default: true)
   */
  void SetScrollOnFocus(bool enable);

  /**
   * @brief Gets whether auto-scroll on focus is enabled.
   *
   * @SINCE_3_0.0
   * @return True if auto-scroll on focus is enabled
   */
  bool GetScrollOnFocus() const;

  /**
   * @brief Sets the target scroll position used when auto-scrolling to a focused child.
   *
   * @SINCE_3_0.0
   * @param[in] position The target scroll position (default: ScrollToPosition::MakeVisible)
   * @see ScrollToPosition
   */
  void SetFocusScrollToPosition(ScrollToPosition position);

  /**
   * @brief Gets the target scroll position used when auto-scrolling to a focused child.
   *
   * @SINCE_3_0.0
   * @return The current focus scroll-to position
   */
  ScrollToPosition GetFocusScrollToPosition() const;

  /**
   * @brief Sets the peek distance applied when auto-scrolling to a focused child.
   *
   * When @p peek is greater than zero and the scroll mode is ScrollToPosition::MakeVisible,
   * the view scrolls an additional @p peek pixels past the item edge in the direction of
   * travel. This hints to the user that more content is available beyond the focused item.
   *
   * The final position is still clamped to the valid scroll range, so peek is silently
   * reduced near the content boundaries.
   *
   * @SINCE_3_0.0
   * @param[in] peek Extra scroll distance in pixels (default: 0.0f, disabled)
   */
  void SetFocusScrollPeek(float peek);

  /**
   * @brief Gets the peek distance applied when auto-scrolling to a focused child.
   *
   * @SINCE_3_0.0
   * @return The peek distance in pixels
   */
  float GetFocusScrollPeek() const;

public: // Key Scroll
  /**
   * @brief Sets whether key-based step scrolling is enabled.
   *
   * When enabled, ScrollView intercepts focus navigation requests (UP/DOWN/LEFT/RIGHT/
   * PAGE_UP/PAGE_DOWN) from its content children. If the next focusable view in that
   * direction is within GetKeyScrollStep() pixels it receives focus as normal; otherwise
   * the ScrollView scrolls by one step instead, keeping the focus where it is.
   *
   * This gives a "gradual reveal" UX on keyboard/remote-control navigation: the user
   * scrolls through content one step at a time and focus only jumps to a new item when
   * that item is close enough to be immediately visible.
   *
   * @SINCE_3_0.0
   * @param[in] enable True to enable (default: false)
   */
  void SetKeyScrollEnabled(bool enable);

  /**
   * @brief Gets whether key-based step scrolling is enabled.
   *
   * @SINCE_3_0.0
   * @return True if key scroll is enabled
   */
  bool IsKeyScrollEnabled() const;

  /**
   * @brief Sets the step distance used for key-based step scrolling.
   *
   * Serves as both the proximity threshold and the scroll step size:
   * - Next focusable is within @p step px  → focus moves there.
   * - Next focusable is beyond @p step px, or none → scroll by @p step px.
   * PAGE_UP / PAGE_DOWN always scrolls by the full viewport size regardless of this value.
   *
   * @SINCE_3_0.0
   * @param[in] step Step distance in pixels (default: 200.0f)
   */
  void SetKeyScrollStep(float step);

  /**
   * @brief Gets the step distance used for key-based step scrolling.
   *
   * @SINCE_3_0.0
   * @return The step distance in pixels
   */
  float GetKeyScrollStep() const;

public: // Scroll Bar Visibility
  /**
   * @brief Sets the visibility of the vertical scroll bar.
   *
   * @SINCE_3_0.0
   * @param[in] visibility The vertical scroll bar visibility
   */
  void SetVerticalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Gets the visibility of the vertical scroll bar.
   *
   * @SINCE_3_0.0
   * @return The vertical scroll bar visibility
   */
  ScrollBarVisibility GetVerticalScrollBarVisibility() const;

  /**
   * @brief Sets the visibility of the horizontal scroll bar.
   *
   * @SINCE_3_0.0
   * @param[in] visibility The horizontal scroll bar visibility
   */
  void SetHorizontalScrollBarVisibility(ScrollBarVisibility visibility);

  /**
   * @brief Gets the visibility of the horizontal scroll bar.
   *
   * @SINCE_3_0.0
   * @return The horizontal scroll bar visibility
   */
  ScrollBarVisibility GetHorizontalScrollBarVisibility() const;

public: // Edge Effects
  /**
   * @brief Sets the edge effect shown when content is dragged or flung past the start boundary.
   *
   * For Vertical scrolling: the top (beginning) edge.
   * For Horizontal scrolling: the left (beginning) edge.
   * Pass an uninitialized EdgeEffect() to remove the effect.
   *
   * @SINCE_3_0.0
   * @param[in] effect The edge effect to activate at the start boundary.
   */
  void SetStartEdgeEffect(EdgeEffect effect);

  /**
   * @brief Gets the start-boundary edge effect (uninitialized if none is set).
   * @SINCE_3_0.0
   */
  EdgeEffect GetStartEdgeEffect() const;

  /**
   * @brief Sets the edge effect shown when content is dragged or flung past the end boundary.
   *
   * For Vertical scrolling: the bottom (end) edge.
   * For Horizontal scrolling: the right (end) edge.
   * Pass an uninitialized EdgeEffect() to remove the effect.
   *
   * @SINCE_3_0.0
   * @param[in] effect The edge effect to activate at the end boundary.
   */
  void SetEndEdgeEffect(EdgeEffect effect);

  /**
   * @brief Gets the end-boundary edge effect (uninitialized if none is set).
   * @SINCE_3_0.0
   */
  EdgeEffect GetEndEdgeEffect() const;

  // @CHAIN_END

public: // Non-Chainable Methods
  /**
   * @brief Gets a value indicating whether the scroll is currently in progress.
   *
   * @SINCE_3_0.0
   * @return True if scrolling is in progress
   */
  bool IsScrolling() const;

  /**
   * @brief Scrolls the content to the specified position.
   *
   * @SINCE_3_0.0
   * @param[in] position The position to scroll to
   * @param[in] animation Whether to animate the scroll
   */
  void ScrollTo(const Vector2& position, bool animation = true);

  /**
   * @brief Scrolls the content to the specified child view.
   *
   * @SINCE_3_0.0
   * @param[in] child The child view to scroll to
   * @param[in] animation Whether to animate the scroll
   * @param[in] scrollToPosition The position to scroll the child to
   */
  void ScrollTo(View child, bool animation = true, ScrollToPosition scrollToPosition = ScrollToPosition::MakeVisible);

  /**
   * @brief Scrolls to the specified X position (for horizontal scrolling).
   *
   * @SINCE_3_0.0
   * @param[in] position The X position
   * @param[in] animation Whether to animate the scroll
   */
  void ScrollToX(float position, bool animation = true);

  /**
   * @brief Scrolls to the specified Y position (for vertical scrolling).
   *
   * @SINCE_3_0.0
   * @param[in] position The Y position
   * @param[in] animation Whether to animate the scroll
   */
  void ScrollToY(float position, bool animation = true);

public:
  using ScrollPosition = Vector2;
  /**
   * @brief Scroll started signal type.
   * @SINCE_3_0.0
   */
  using ScrollStartedSignalType = Signal<void(ScrollView)>;

  /**
   * @brief Scroll finished signal type.
   * @SINCE_3_0.0
   */
  using ScrollFinishedSignalType = Signal<void(ScrollView)>;

  /**
   * @brief Scrolling signal type.
   * @SINCE_3_0.0
   */
  using ScrollingSignalType = Signal<void(ScrollView)>;

  /**
   * @brief Drag started signal type.
   * @SINCE_3_0.0
   */
  using DragStartedSignalType = Signal<void(ScrollView)>;

  /**
   * @brief Drag finished signal type.
   * @SINCE_3_0.0
   */
  using DragFinishedSignalType = Signal<void(ScrollView)>;

  /**
   * @brief Dragging signal type.
   * @SINCE_3_0.0
   */
  using DraggingSignalType = Signal<void(ScrollView, float, float)>; // scrollView, deltaX, deltaY

  /**
   * @brief Signal emitted when scroll starts.
   * @SINCE_3_0.0
   */
  ScrollStartedSignalType& ScrollStartedSignal();

  /**
   * @brief Signal emitted while scrolling.
   * @SINCE_3_0.0
   */
  ScrollingSignalType& ScrollingSignal();

  /**
   * @brief Signal emitted when scroll finishes.
   * @SINCE_3_0.0
   */
  ScrollFinishedSignalType& ScrollFinishedSignal();

  /**
   * @brief Signal emitted when drag starts.
   * @SINCE_3_0.0
   */
  DragStartedSignalType& DragStartedSignal();

  /**
   * @brief Signal emitted while dragging.
   * @SINCE_3_0.0
   */
  DraggingSignalType& DraggingSignal();

  /**
   * @brief Signal emitted when drag finishes.
   * @SINCE_3_0.0
   */
  DragFinishedSignalType& DragFinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle from the Integration implementation.
   */
  explicit ScrollView(Integration::ScrollViewImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   */
  explicit ScrollView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_ELEMENTS_SCROLL_VIEW_H
