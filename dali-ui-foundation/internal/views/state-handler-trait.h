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

#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/signal-slot-observers.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/view-state.h>

#include <algorithm>
#include <string>
#include <vector>

namespace Dali
{

namespace Ui
{

namespace Internal
{

// StateHandlerTrait
// ============================================================================

/**
 * @brief Internal lifecycle trait for named state-change handlers.
 *
 * Manages a set of named callbacks (identified by string keys) that are
 * invoked when the owner View's ViewState changes. Participates in the DALi
 * connection tracking protocol via SlotObserver so handlers are automatically
 * removed when their associated ConnectionTrackerInterface is destroyed.
 */
class StateHandlerTrait : public TraitObject, public SlotObserver, public ConnectionTracker
{
public:
  StateHandlerTrait();

  /**
   * @brief Registers a named state-change handler.
   *
   * If a handler with the same @a id already exists, it is replaced.
   *
   * @param[in] id       Unique string identifier for this handler
   * @param[in] tracker  ConnectionTrackerInterface for automatic lifetime management
   * @param[in] callback Callback to invoke. Ownership is taken.
   *                     Signature: void(View, ViewState, ViewState)
   */
  void Set(const std::string& id, ConnectionTrackerInterface* tracker, CallbackBase* callback);

  /**
   * @brief Removes a named state-change handler.
   *
   * @param[in] id The handler identifier to remove
   * @return True if a handler was found and removed
   */
  bool Unset(const std::string& id);

  /**
   * @brief Removes a named handler only if it is not currently being processed.
   *
   * Prevents re-entrant removal when a handler's own callback triggers
   * code that would unset the same handler.
   *
   * @param[in] id The handler identifier
   * @return True if removed, false if currently processing or not found
   */
  bool UnsetWhenNotProcessing(const std::string& id);

  /**
   * @brief Invokes all registered handlers.
   *
   * Called by Integration::View::SetState when the view's ViewState changes.
   *
   * @param[in] view The owner view
   * @param[in] prev The previous ViewState
   * @param[in] cur  The current ViewState
   */
  void NotifyStateChanged(View view, StateEvent event);

public: // SlotObserver
  void SlotDisconnected(CallbackBase* callback) override;

protected:
  ~StateHandlerTrait() override;

  void OnAttached(TraitId id, View& view) override;
  void OnDetaching(TraitId id, View& view) override;
  void OnViewDestroying(ViewImpl* viewImpl) override;

private:
  struct Handler
  {
    CallbackBase*               callback;             ///< Owned. Signature: void(View, ViewState, ViewState)
    ConnectionTrackerInterface* tracker;              ///< Non-owning ref for auto-disconnect
    bool                        pendingRemove{false}; ///< Deferred removal when SlotDisconnected fires during Execute
  };

  struct HandlerEntry
  {
    std::string id;
    Handler     handler;
  };

  void CleanupHandler(Handler& handler);
  void ClearAllHandlers();

private:
  WeakHandle<View>          mOwner;
  std::vector<HandlerEntry> mHandlers; ///< Insertion-order-preserving handler list
  std::vector<std::string>  mKeys;     ///< Reusable snapshot buffer for NotifyStateChanged
  std::string               mProcessingId;
  bool                      mNotifying{false}; ///< Re-entrancy guard for NotifyStateChanged
};

} // namespace Internal

} // namespace Ui

} // namespace Dali
