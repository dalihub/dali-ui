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
#include <dali-ui-foundation/internal/views/view-state-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/state-event-impl.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

ViewStateManager& ViewStateManager::Get()
{
  static ViewStateManager instance;
  return instance;
}

void ViewStateManager::NotifyStateChanged(Ui::View view, ViewState prev, ViewState next, InputEvent cause)
{
  mPending.push_back({view, prev, next, cause});

  if(mNotifying)
  {
    return;
  }

  // RAII guard: ensures mNotifying is reset and mPending is cleared even if a
  // handler throws, preventing the manager from getting permanently stuck.
  struct NotifyGuard
  {
    ViewStateManager& mgr;
    NotifyGuard(ViewStateManager& m)
    : mgr(m)
    {
      mgr.mNotifying = true;
    }
    ~NotifyGuard()
    {
      mgr.mPending.clear();
      mgr.mNotifying = false;
    }
  } guard(*this);

  while(!mPending.empty())
  {
    // Swap the current batch out so re-entrant calls append to the (now empty) mPending.
    // Using swap instead of move preserves both buffers' capacity for reuse.
    mBatch.clear();
    mBatch.swap(mPending);

    for(auto& n : mBatch)
    {
      ViewImpl& impl = GetImpl(n.view);

      IntrusivePtr<TraitObject> object = IntegrationView::GetTrait(impl, Integration::ReservedTraitId::STATE_EFFECT);
      DALI_ASSERT_DEBUG(!object || dynamic_cast<Integration::StateEffectImpl*>(object.Get()));
      auto* stateEffectImpl = object ? static_cast<Integration::StateEffectImpl*>(object.Get()) : nullptr;

      if(stateEffectImpl || !impl.StateChangedSignal().Empty())
      {
        Internal::StateEventImplPtr stateEventImpl = Internal::StateEventImpl::New(n.prev, n.next, n.cause);
        StateEvent                  stateEvent(stateEventImpl.Get());

        if(stateEffectImpl)
        {
          stateEffectImpl->OnViewStateChanged(n.view, stateEvent);
        }

        if(!impl.StateChangedSignal().Empty())
        {
          impl.StateChangedSignal().Emit(n.view, stateEvent);
        }
      }
      else
      {
        continue;
      }
    }
  }
}

bool ViewStateManager::IsEffectivelyEnabled(const ViewImpl& viewImpl) const
{
  if(viewImpl.GetState().Contains(ViewState::DISABLED))
  {
    return false;
  }

  Actor parent = viewImpl.Self().GetParent();
  while(parent)
  {
    View parentView = View::DownCast(parent);
    if(parentView)
    {
      if(GetImpl(parentView).GetState().Contains(ViewState::DISABLED))
      {
        return false;
      }
    }
    parent = parent.GetParent();
  }

  return true;
}

bool ViewStateManager::IsEffectivelyFocused(const ViewImpl& viewImpl) const
{
  if(viewImpl.GetState().Contains(ViewState::FOCUSED))
  {
    return true;
  }

  Actor parent = viewImpl.Self().GetParent();
  while(parent)
  {
    View parentView = View::DownCast(parent);
    if(parentView)
    {
      if(GetImpl(parentView).GetState().Contains(ViewState::FOCUSED))
      {
        return true;
      }
    }
    parent = parent.GetParent();
  }

  return false;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
