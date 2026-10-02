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
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Process-level singleton that observes scroll and drag state
 * across all scrollable containers (ScrollView, List, Grid, ...).
 *
 * Depth counters handle nested or concurrent scrollable containers:
 * a state remains active as long as at least one container is in it.
 *
 * Typical use — suppress pressed visual effects during disambiguation:
 * @code
 * if(ScrollStateObserver::Get().IsGestureDisambiguating())
 *   StartPressDelayTimer();
 * @endcode
 */
class DALI_UI_API ScrollStateObserver
{
public:
  using StateSignalType       = Signal<void()>;       ///< Process-wide state notification.
  using WindowStateSignalType = Signal<void(Window)>; ///< Notification carrying the source Window.

  /**
   * @brief Returns the process-level singleton instance.
   * @return The shared observer
   */
  static ScrollStateObserver& Get();

  // -----------------------------------------------------------------------
  // State queries — all O(1)

  /**
   * @brief Queries whether any container is resolving a recognized pan gesture.
   * @return True while at least one disambiguation is pending
   */
  bool IsGestureDisambiguating() const
  {
    return mDisambiguatingDepth > 0;
  }
  /**
   * @brief Queries whether any container is being dragged.
   * @return True while at least one drag is active
   */
  bool IsDragging() const
  {
    return mDraggingDepth > 0;
  }
  /**
   * @brief Queries whether any container is scrolling, by drag or fling.
   * @return True while at least one scroll is active
   */
  bool IsScrolling() const
  {
    return mScrollingDepth > 0;
  }
  /**
   * @brief Queries whether scrolling continues without an active drag.
   * @return True if scrolling is active and no container is being dragged
   */
  bool IsFlinging() const
  {
    return IsScrolling() && !IsDragging();
  }

  // -----------------------------------------------------------------------
  // Signals

  /**
   * @brief Gets the signal emitted for each disambiguation start.
   * @return The process-wide notification signal
   */
  StateSignalType& DisambiguationBeganSignal()
  {
    return mDisambiguationBeganSignal;
  }
  /**
   * @brief Gets the signal emitted for each disambiguation end.
   * @return The process-wide notification signal
   */
  StateSignalType& DisambiguationEndedSignal()
  {
    return mDisambiguationEndedSignal;
  }
  /**
   * @brief Gets the signal emitted for each scroll start.
   * @return The process-wide notification signal
   */
  StateSignalType& ScrollStartedSignal()
  {
    return mScrollStartedSignal;
  }
  /**
   * @brief Gets the signal emitted for each scroll end.
   * @return The process-wide notification signal
   */
  StateSignalType& ScrollFinishedSignal()
  {
    return mScrollFinishedSignal;
  }
  /**
   * @brief Gets the existing process-wide signal emitted for every drag start.
   * @return The notification signal, emitted after WindowDragStartedSignal()
   */
  StateSignalType& DragStartedSignal()
  {
    return mDragStartedSignal;
  }
  /**
   * @brief Gets the signal emitted for each drag end.
   * @return The process-wide notification signal
   */
  StateSignalType& DragFinishedSignal()
  {
    return mDragFinishedSignal;
  }

  /**
   * @brief Gets the drag-start signal carrying the source Window.
   * The callback receives an empty Window for legacy notifications without a
   * source. Scope-aware clients must not infer another Window from that value.
   * @return The signal, emitted before the process-wide DragStartedSignal()
   */
  WindowStateSignalType& WindowDragStartedSignal()
  {
    return mWindowDragStartedSignal;
  }

  // -----------------------------------------------------------------------
  // Notifications — called by scrollable containers

  /**
   * @brief Increments the disambiguation count and emits its start signal.
   */
  void NotifyGestureDisambiguationBegan();

  /**
   * @brief Decrements the disambiguation count without underflow and emits its end signal.
   */
  void NotifyGestureDisambiguationEnded();

  /**
   * @brief Starts a drag without source information for legacy callers.
   * Delegates to NotifyDragStarted(Window) with an empty Window. This cannot
   * identify which independent Window's pending touch focus should be cancelled.
   */
  void NotifyDragStarted();

  /**
   * @brief Increments the drag count once and emits scoped then process-wide signals.
   * @param[in] window The source Window; empty means the source is unknown
   */
  void NotifyDragStarted(Window window);

  /**
   * @brief Decrements the drag count without underflow and emits its end signal.
   */
  void NotifyDragFinished();

  /**
   * @brief Increments the scroll count and emits its start signal.
   */
  void NotifyScrollStarted();

  /**
   * @brief Decrements the scroll count without underflow and emits its end signal.
   */
  void NotifyScrollFinished();

private:
  ScrollStateObserver()                                      = default;
  ~ScrollStateObserver()                                     = default;
  ScrollStateObserver(const ScrollStateObserver&)            = delete;
  ScrollStateObserver& operator=(const ScrollStateObserver&) = delete;

  int mDisambiguatingDepth{0}; ///< pan recognised, threshold not yet met
  int mDraggingDepth{0};       ///< user finger actively dragging
  int mScrollingDepth{0};      ///< any scroll in progress (drag or fling)

  StateSignalType       mDisambiguationBeganSignal;
  StateSignalType       mDisambiguationEndedSignal;
  StateSignalType       mScrollStartedSignal;
  StateSignalType       mScrollFinishedSignal;
  StateSignalType       mDragStartedSignal;
  StateSignalType       mDragFinishedSignal;
  WindowStateSignalType mWindowDragStartedSignal;
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
