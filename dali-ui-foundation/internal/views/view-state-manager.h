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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/types/view-state.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

class ViewImpl;

namespace Internal
{

/**
 * @brief Singleton that serialises View state-change notifications across the main loop thread.
 *
 * Because all Views run on a single thread, one global pending queue is sufficient
 * and avoids per-View overhead.
 *
 * When SetState() is called re-entrantly (e.g. a StateChangedSignal handler
 * triggers another state change), the new notification is queued instead of being
 * dispatched immediately.  The outermost caller drains the queue after its own
 * notifications complete, guaranteeing that:
 *  - every handler for transition A→B finishes before any handler for B→C is called.
 *  - handlers always receive the (prev, cur) pair that matches the transition they
 *    were registered for.
 */
class ViewStateManager
{
public:
  static ViewStateManager& Get();

  /**
   * @brief Schedules a state-change notification for @p view.
   *
   * Notification is deferred if a dispatch is already in progress, guaranteeing
   * that all handlers for the current transition finish before the next
   * transition is dispatched. All subscribers (including StateHandlerTrait)
   * receive the notification through the view's StateChangedSignal.
   *
   * @param[in] view   The View whose state changed (handle kept alive during dispatch)
   * @param[in] prev   State before the change
   * @param[in] next   State after the change
   * @param[in] cause  Input event that triggered the change, or invalid handle if programmatic
   */
  void NotifyStateChanged(Ui::View view, ViewState prev, ViewState next, InputEvent cause = InputEvent::Programmatic());

  /**
   * @brief Returns true if the view and all its View ancestors are enabled.
   *
   * @param[in] viewImpl  The ViewImpl to test
   * @return True if neither the view itself nor any ancestor carries ViewState::DISABLED
   */
  bool IsEffectivelyEnabled(const ViewImpl& viewImpl) const;

  /**
   * @brief Returns true if the view or any of its View ancestors is focused.
   *
   * @param[in] viewImpl  The ViewImpl to test
   * @return True if the view itself or at least one ancestor carries ViewState::FOCUSED
   */
  bool IsEffectivelyFocused(const ViewImpl& viewImpl) const;

private:
  ViewStateManager() = default;

  struct PendingNotification
  {
    Ui::View   view; ///< Public handle keeps impl alive during dispatch
    ViewState  prev;
    ViewState  next;
    InputEvent cause; ///< Invalid handle if not triggered by user input
  };

  bool                             mNotifying{false};
  std::vector<PendingNotification> mPending;
  std::vector<PendingNotification> mBatch; ///< Reusable dispatch buffer — swapped with mPending each iteration
};

} // namespace Internal

} // namespace Ui

} // namespace Dali
