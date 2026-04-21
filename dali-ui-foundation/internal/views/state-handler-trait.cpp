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

// CLASS HEADER
#include <dali-ui-foundation/internal/views/state-handler-trait.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

// ============================================================================
// StateHandlerTrait
// ============================================================================

StateHandlerTrait StateHandlerTrait::New()
{
  IntrusivePtr<StateHandlerTraitImpl> impl(new StateHandlerTraitImpl());
  return StateHandlerTrait(impl.Get());
}

StateHandlerTrait::StateHandlerTrait(StateHandlerTraitImpl* impl)
: Trait(impl)
{
}

StateHandlerTraitImpl& StateHandlerTrait::GetImpl()
{
  return static_cast<StateHandlerTraitImpl&>(GetBaseObject());
}

// ============================================================================
// StateHandlerTraitImpl
// ============================================================================

StateHandlerTraitImpl::StateHandlerTraitImpl()
{
}

StateHandlerTraitImpl::~StateHandlerTraitImpl()
{
  ClearAllHandlers();
}

void StateHandlerTraitImpl::Set(const std::string& id, ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  Unset(id);

  if(tracker && callback)
  {
    tracker->SignalConnected(this, callback);
    mHandlers.push_back({id, {callback, tracker}});
  }
  else
  {
    delete callback;
  }
}

bool StateHandlerTraitImpl::Unset(const std::string& id)
{
  auto it = std::find_if(mHandlers.begin(), mHandlers.end(), [&](const HandlerEntry& e)
  { return e.id == id; });
  if(it == mHandlers.end())
  {
    return false;
  }

  CleanupHandler(it->handler);
  mHandlers.erase(it);
  return true;
}

bool StateHandlerTraitImpl::UnsetWhenNotProcessing(const std::string& id)
{
  if(mProcessingId == id)
  {
    return false;
  }
  return Unset(id);
}

void StateHandlerTraitImpl::NotifyStateChanged(View view, const StateEvent& event)
{
  if(mNotifying)
  {
    // ViewStateManager defers re-entrant SetState calls via its own mPending queue.
    // This guard provides a secondary safety net: if NotifyStateChanged is somehow called
    // re-entrantly (e.g. a handler calls SetState and the deferred dispatch fires before
    // the current batch completes), skip it to prevent out-of-order delivery.
    return;
  }

  mNotifying = true;

  // Snapshot IDs since handlers may be removed during iteration.
  // mKeys is a member to reuse the allocated buffer across calls.
  mKeys.clear();
  mKeys.reserve(mHandlers.size());
  for(const auto& entry : mHandlers)
  {
    mKeys.push_back(entry.id);
  }

  for(const auto& id : mKeys)
  {
    auto it = std::find_if(mHandlers.begin(), mHandlers.end(), [&](const HandlerEntry& e)
    { return e.id == id; });
    if(it == mHandlers.end())
    {
      continue; // removed during iteration
    }

    mProcessingId = id;
    CallbackBase::Execute(*it->handler.callback, view, event);

    // SlotDisconnected may have fired during Execute and deferred removal
    // to avoid deleting the callback while it was still on the call stack.
    auto afterIt = std::find_if(mHandlers.begin(), mHandlers.end(), [&](const HandlerEntry& e)
    { return e.id == id; });
    if(afterIt != mHandlers.end() && afterIt->handler.pendingRemove)
    {
      delete afterIt->handler.callback;
      mHandlers.erase(afterIt);
    }
  }
  mProcessingId.clear();

  mNotifying = false;
}

void StateHandlerTraitImpl::SlotDisconnected(CallbackBase* callback)
{
  for(auto it = mHandlers.begin(); it != mHandlers.end(); ++it)
  {
    if(it->handler.callback == callback)
    {
      // Tracker is already being destroyed — don't call SignalDisconnected on it.
      it->handler.tracker = nullptr;

      if(mNotifying && mProcessingId == it->id)
      {
        // This callback is currently on the Execute call stack.
        // Deleting it now would be UB — defer removal until Execute returns.
        it->handler.pendingRemove = true;
      }
      else
      {
        delete it->handler.callback;
        mHandlers.erase(it);
      }
      return;
    }
  }
}

void StateHandlerTraitImpl::OnBeforeAttached(Integration::TraitId /*id*/, View& view)
{
  mOwner = view;
  Integration::GetImpl(view).StateChangedSignal().Connect(this, &StateHandlerTraitImpl::NotifyStateChanged);
}

void StateHandlerTraitImpl::OnAttached(Integration::TraitId /*id*/, View& /*view*/)
{
}

void StateHandlerTraitImpl::OnDetached(Integration::TraitId /*id*/, View& /*view*/)
{
  DisconnectAll();
  ClearAllHandlers();
  mOwner.Reset();
}

void StateHandlerTraitImpl::OnViewDestroying(Integration::ViewImpl* /*viewImpl*/)
{
  DisconnectAll();
  ClearAllHandlers();
  mOwner.Reset();
}

void StateHandlerTraitImpl::ClearAllHandlers()
{
  for(auto& entry : mHandlers)
  {
    CleanupHandler(entry.handler);
  }
  mHandlers.clear();
}

void StateHandlerTraitImpl::CleanupHandler(Handler& handler)
{
  if(handler.tracker && handler.callback)
  {
    handler.tracker->SignalDisconnected(this, handler.callback);
  }
  delete handler.callback;
  handler.callback = nullptr;
  handler.tracker  = nullptr;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
