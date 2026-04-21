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
#include <dali-ui-foundation/integration-api/selectable-trait-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali::Ui::Integration
{

SelectableTraitImpl::SelectableTraitImpl()
: TraitImpl(),
  mSelectionChangedSignal(),
  mSelected(false),
  mToggleByClickEnabled(true),
  mAttached(false)
{
}

SelectableTraitImpl::~SelectableTraitImpl()
{
}

Signal<void(View, bool, const InputEvent&)>& SelectableTraitImpl::SelectionChangedSignal()
{
  return mSelectionChangedSignal;
}

bool SelectableTraitImpl::IsSelected() const
{
  return mSelected;
}

void SelectableTraitImpl::SetSelected(bool selected)
{
  SetSelectedInternal(selected, InputEvent::None());
}

void SelectableTraitImpl::SetSelectedInternal(bool selected, const InputEvent& event)
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

  if(!OnSelectionChanging(owner, selected))
  {
    return;
  }

  mSelected = selected;
  GetImpl(owner).SetState(ViewState::SELECTED, selected);
  mSelectionChangedSignal.Emit(owner, mSelected, event);
}

bool SelectableTraitImpl::IsToggleByClickEnabled() const
{
  return mToggleByClickEnabled;
}

void SelectableTraitImpl::EnableToggleByClick(bool enabled)
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

View SelectableTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void SelectableTraitImpl::OnBeforeAttached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;
}

void SelectableTraitImpl::OnAttached(TraitId id, View& view)
{
  mAttached = true;

  if(mToggleByClickEnabled)
  {
    EnsureClickableAndConnect();
  }
}

void SelectableTraitImpl::OnDetached(TraitId id, View& view)
{
  DisconnectClickable();
  mAttached = false;
  mOwner.Reset();
}

void SelectableTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

bool SelectableTraitImpl::OnSelectionChanging(View view, bool newSelected)
{
  return true;
}

void SelectableTraitImpl::EnsureClickableAndConnect()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Get or create InteractiveTrait on the owner view
  InteractiveTrait clickable = owner.EnsureInteractiveTrait();
  if(clickable)
  {
    clickable.ClickedSignal().Connect(this, &SelectableTraitImpl::OnClickedForToggle);
  }
}

void SelectableTraitImpl::DisconnectClickable()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  InteractiveTrait clickable = InteractiveTrait::DownCast(
    GetImpl(owner).GetTrait(ReservedTraitId::INTERACTION_TRAIT));
  if(clickable)
  {
    clickable.ClickedSignal().Disconnect(this, &SelectableTraitImpl::OnClickedForToggle);
  }
}

void SelectableTraitImpl::OnClickedForToggle(View view, const InputEvent& event)
{
  SetSelectedInternal(!mSelected, event);
}

} // namespace Dali::Ui::Integration
