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
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
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
  using StateSignalType = Signal<void()>;

  /**
   * @brief Returns the process-level singleton instance.
   */
  static ScrollStateObserver& Get();

  // -----------------------------------------------------------------------
  // State queries — all O(1)

  bool IsGestureDisambiguating() const
  {
    return mDisambiguatingDepth > 0;
  }
  bool IsDragging() const
  {
    return mDraggingDepth > 0;
  }
  bool IsScrolling() const
  {
    return mScrollingDepth > 0;
  }
  bool IsFlinging() const
  {
    return IsScrolling() && !IsDragging();
  }

  // -----------------------------------------------------------------------
  // Signals

  StateSignalType& DisambiguationBeganSignal()
  {
    return mDisambiguationBeganSignal;
  }
  StateSignalType& DisambiguationEndedSignal()
  {
    return mDisambiguationEndedSignal;
  }
  StateSignalType& ScrollStartedSignal()
  {
    return mScrollStartedSignal;
  }
  StateSignalType& ScrollFinishedSignal()
  {
    return mScrollFinishedSignal;
  }
  StateSignalType& DragStartedSignal()
  {
    return mDragStartedSignal;
  }
  StateSignalType& DragFinishedSignal()
  {
    return mDragFinishedSignal;
  }

  // -----------------------------------------------------------------------
  // Notifications — called by scrollable containers

  void NotifyGestureDisambiguationBegan();
  void NotifyGestureDisambiguationEnded();
  void NotifyDragStarted();
  void NotifyDragFinished();
  void NotifyScrollStarted();
  void NotifyScrollFinished();

private:
  ScrollStateObserver()                                      = default;
  ~ScrollStateObserver()                                     = default;
  ScrollStateObserver(const ScrollStateObserver&)            = delete;
  ScrollStateObserver& operator=(const ScrollStateObserver&) = delete;

  int mDisambiguatingDepth{0}; ///< pan recognised, threshold not yet met
  int mDraggingDepth{0};       ///< user finger actively dragging
  int mScrollingDepth{0};      ///< any scroll in progress (drag or fling)

  StateSignalType mDisambiguationBeganSignal;
  StateSignalType mDisambiguationEndedSignal;
  StateSignalType mScrollStartedSignal;
  StateSignalType mScrollFinishedSignal;
  StateSignalType mDragStartedSignal;
  StateSignalType mDragFinishedSignal;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
