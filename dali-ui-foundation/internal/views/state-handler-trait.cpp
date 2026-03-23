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
  for(auto& [id, handler] : mHandlers)
  {
    CleanupHandler(handler);
  }
  mHandlers.clear();
}

void StateHandlerTraitImpl::Set(const std::string& id, ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  Unset(id);

  if(tracker && callback)
  {
    tracker->SignalConnected(this, callback);
    mHandlers[id] = Handler{callback, tracker};
  }
  else
  {
    delete callback;
  }
}

bool StateHandlerTraitImpl::Unset(const std::string& id)
{
  auto it = mHandlers.find(id);
  if(it == mHandlers.end())
  {
    return false;
  }

  CleanupHandler(it->second);
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
  // Snapshot keys since handlers may be removed during iteration
  std::vector<std::string> keys;
  keys.reserve(mHandlers.size());
  for(const auto& [id, handler] : mHandlers)
  {
    keys.push_back(id);
  }

  for(const auto& id : keys)
  {
    auto it = mHandlers.find(id);
    if(it == mHandlers.end())
    {
      continue; // removed during iteration
    }

    mProcessingId = id;
    CallbackBase::Execute(*it->second.callback, view, event);
  }
  mProcessingId.clear();
}

void StateHandlerTraitImpl::SlotDisconnected(CallbackBase* callback)
{
  for(auto it = mHandlers.begin(); it != mHandlers.end(); ++it)
  {
    if(it->second.callback == callback)
    {
      // Tracker is already being destroyed — don't call SignalDisconnected on it.
      delete it->second.callback;
      mHandlers.erase(it);
      return;
    }
  }
}

void StateHandlerTraitImpl::OnBeforeAttached(Integration::TraitId /*id*/, View& view)
{
  mOwner = view;
}

void StateHandlerTraitImpl::OnAttached(Integration::TraitId /*id*/, View& /*view*/)
{
}

void StateHandlerTraitImpl::OnDetached(Integration::TraitId /*id*/, View& /*view*/)
{
  for(auto& [id, handler] : mHandlers)
  {
    CleanupHandler(handler);
  }
  mHandlers.clear();
  mOwner.Reset();
}

void StateHandlerTraitImpl::OnViewDestroying(Integration::ViewImpl* /*viewImpl*/)
{
  for(auto& [id, handler] : mHandlers)
  {
    CleanupHandler(handler);
  }
  mHandlers.clear();
  mOwner.Reset();
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
