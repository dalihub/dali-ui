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
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali::Ui::Internal
{
namespace
{

InteractiveTrait GetInteractiveTrait(ViewImpl& viewImpl)
{
  auto* traitObject = ViewDataImpl::Get(viewImpl).GetCoreInteractionObject();
  return traitObject ? InteractiveTrait::DownCast(BaseHandle(static_cast<BaseObject*>(traitObject))) : InteractiveTrait();
}

} // unnamed namespace

SelectableTraitImpl::SelectableTraitImpl()
: mSelectionChangedSignal(),
  mSelectionCommitObserver(nullptr),
  mSelected(false),
  mToggleByClickEnabled(true),
  mPersistentSelectOnlyByClickEnabled(false),
  mGroupSelectOnlyByClickEnabled(false),
  mAttached(false)
{
}

SelectableTraitImpl::~SelectableTraitImpl()
{
}

Signal<void(View, bool, InputEvent)>& SelectableTraitImpl::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

void SelectableTraitImpl::SetSelectionCommitObserver(SelectionCommitObserver observer)
{
  mSelectionCommitObserver = observer;
}

bool SelectableTraitImpl::IsSelected() const
{
  return mSelected;
}

void SelectableTraitImpl::SetSelected(bool selected)
{
  SetSelectedInternal(selected, InputEvent::Programmatic());
}

void SelectableTraitImpl::SetSelected(bool selected, InputEvent event)
{
  SetSelectedInternal(selected, event);
}

void SelectableTraitImpl::SetSelectedInternal(bool selected, InputEvent event)
{
  if(mSelected == selected)
  {
    return;
  }

  View owner = mOwner.GetHandle();
  if(!owner)
  {
    // Not attached yet, just store the state
    mSelected = selected;
    return;
  }

  mSelected = selected;
  Extension::View::SetState(GetImpl(owner), ViewState::SELECTED, selected, event);

  // Post-commit, pre-public notification. The internal commit observer lets an internal
  // collaborator (GroupSelectableTraitImpl) observe the already-committed state and fully
  // settle group-selection arbitration BEFORE any user-facing SelectionChangedSignal callback
  // runs, independent of signal connection order. Both emit the local `selected` (this
  // transition's result), not a re-read of mSelected: a re-entrant selection change during the
  // commit cascade (e.g. a callback that calls SetSelected on this view again) could otherwise
  // flip mSelected before the public emit reads it.
  if(mSelectionCommitObserver)
  {
    mSelectionCommitObserver(owner, selected, event);
  }
  mSelectionChangedSignal.Emit(owner, selected, event);
}

bool SelectableTraitImpl::IsToggleByClickEnabled() const
{
  return mToggleByClickEnabled;
}

void SelectableTraitImpl::SetToggleByClickEnabled(bool enabled)
{
  if(mToggleByClickEnabled == enabled)
  {
    return;
  }

  mToggleByClickEnabled = enabled;

  if(mAttached)
  {
    if(enabled)
    {
      EnsureClickableAndConnect();
    }
    else
    {
      DisconnectClickable();
    }
  }
}

bool SelectableTraitImpl::IsSelectOnlyByClickEnabled() const
{
  return mPersistentSelectOnlyByClickEnabled || mGroupSelectOnlyByClickEnabled;
}

void SelectableTraitImpl::EnablePersistentSelectOnlyByClick()
{
  mPersistentSelectOnlyByClickEnabled = true;
}

void SelectableTraitImpl::SetGroupSelectOnlyByClickEnabled(bool enabled)
{
  mGroupSelectOnlyByClickEnabled = enabled;
}

View SelectableTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void SelectableTraitImpl::OnAttached(View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;

  mAttached = true;

  if(mToggleByClickEnabled)
  {
    EnsureClickableAndConnect();
  }
}

void SelectableTraitImpl::OnDetaching(View& view)
{
  DisconnectClickable();
  mAttached = false;
  mOwner.Reset();
}

void SelectableTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

void SelectableTraitImpl::EnsureClickableAndConnect()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  InteractiveTrait clickable = GetInteractiveTrait(GetImpl(owner));
  DALI_ASSERT_ALWAYS(clickable && "SelectableTraitImpl requires InteractiveTrait");
  clickable.ClickedSignal().Connect(this, &SelectableTraitImpl::OnClickedForToggle);
}

void SelectableTraitImpl::DisconnectClickable()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  InteractiveTrait clickable = GetInteractiveTrait(GetImpl(owner));
  if(clickable)
  {
    clickable.ClickedSignal().Disconnect(this, &SelectableTraitImpl::OnClickedForToggle);
  }
}

void SelectableTraitImpl::OnClickedForToggle(View view, InputEvent event)
{
  if(mSelected && IsSelectOnlyByClickEnabled())
  {
    // Select-only: keep an already-selected View selected; a click never unselects.
    return;
  }
  SetSelectedInternal(!mSelected, event);
}

} // namespace Dali::Ui::Internal
