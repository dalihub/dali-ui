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
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui::Internal
{

CoreInteractionObject::CoreInteractionObject() = default;

CoreInteractionObject::~CoreInteractionObject() = default;

InteractiveTraitImpl& CoreInteractionObject::EnsureInteractiveTraitImpl()
{
  if(!mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl = std::make_unique<InteractiveTraitImpl>();

    View owner = mOwner.GetHandle();
    if(owner)
    {
      mInteractiveTraitImpl->OnAttached(owner);
    }
  }
  return *mInteractiveTraitImpl;
}

SelectableTraitImpl& CoreInteractionObject::EnsureSelectableTraitImpl()
{
  EnsureInteractiveTraitImpl();

  if(!mSelectableTraitImpl)
  {
    mSelectableTraitImpl = std::make_unique<SelectableTraitImpl>();

    View owner = mOwner.GetHandle();
    if(owner)
    {
      mSelectableTraitImpl->OnAttached(owner);
    }
  }
  return *mSelectableTraitImpl;
}

GroupSelectableTraitImpl& CoreInteractionObject::EnsureGroupSelectableTraitImpl()
{
  EnsureSelectableTraitImpl();

  if(!mGroupSelectableTraitImpl)
  {
    mGroupSelectableTraitImpl = std::make_unique<GroupSelectableTraitImpl>();

    View owner = mOwner.GetHandle();
    if(owner)
    {
      mGroupSelectableTraitImpl->OnAttached(owner);
    }
  }
  return *mGroupSelectableTraitImpl;
}

InteractiveTraitImpl* CoreInteractionObject::GetInteractiveTraitImpl() const
{
  return mInteractiveTraitImpl.get();
}

SelectableTraitImpl* CoreInteractionObject::GetSelectableTraitImpl() const
{
  return mSelectableTraitImpl.get();
}

GroupSelectableTraitImpl* CoreInteractionObject::GetGroupSelectableTraitImpl() const
{
  return mGroupSelectableTraitImpl.get();
}

void CoreInteractionObject::OnPressedClearedByViewState(View view, InputEvent event)
{
  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->OnPressedClearedByViewState(view, event);
  }
}

void CoreInteractionObject::OnHoveredClearedByViewState(View view, InputEvent event)
{
  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->OnHoveredClearedByViewState(view, event);
  }
}

bool CoreInteractionObject::HasIntrinsicHoverHandling() const
{
  return mInteractiveTraitImpl != nullptr;
}

bool CoreInteractionObject::OnHoverEvent(const HoverEvent& event)
{
  View owner = mOwner.GetHandle();
  if(owner && mInteractiveTraitImpl)
  {
    return mInteractiveTraitImpl->OnHoverEvent(owner, event);
  }
  return false;
}

bool CoreInteractionObject::OnKeyEvent(View view, const KeyEvent& event)
{
  return mInteractiveTraitImpl ? mInteractiveTraitImpl->OnKeyEvent(view, event) : false;
}

void CoreInteractionObject::FinalizeKeyEventDispatch()
{
  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->FinalizeKeyEventDispatch();
  }
}

void CoreInteractionObject::CancelKeyEventDispatch()
{
  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->CancelKeyEventDispatch();
  }
}

void CoreInteractionObject::OnAttached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "CoreInteractionObject can not be attached to multiple target views");
  mOwner = view;

  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->OnAttached(view);
  }

  if(mSelectableTraitImpl)
  {
    mSelectableTraitImpl->OnAttached(view);
  }

  if(mGroupSelectableTraitImpl)
  {
    mGroupSelectableTraitImpl->OnAttached(view);
  }
}

void CoreInteractionObject::OnDetaching(TraitId id, View& view)
{
  if(mGroupSelectableTraitImpl)
  {
    mGroupSelectableTraitImpl->OnDetaching(view);
  }

  if(mSelectableTraitImpl)
  {
    mSelectableTraitImpl->OnDetaching(view);
  }

  DALI_ASSERT_ALWAYS(false && "CoreInteractionObject can not be detached once it is attached to a view");
}

void CoreInteractionObject::OnViewDestroying(ViewImpl* viewImpl)
{
  if(mInteractiveTraitImpl)
  {
    mInteractiveTraitImpl->OnViewDestroying(viewImpl);
  }

  if(mSelectableTraitImpl)
  {
    mSelectableTraitImpl->OnViewDestroying(viewImpl);
  }

  if(mGroupSelectableTraitImpl)
  {
    mGroupSelectableTraitImpl->OnViewDestroying(viewImpl);
  }
}

} // namespace Dali::Ui::Internal
