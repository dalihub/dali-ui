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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>

namespace Dali
{

namespace Ui
{

class InputEvent;

namespace Integration
{
/**
 * @brief Internal implementation of Selectable trait.
 *
 * SelectableTraitImpl manages the selected state of a View and optionally
 * toggles selection on click by listening to the owner View's InteractiveTrait.
 * Unlike InteractiveTraitImpl, this does not implement InteractiveTraitInterface and
 * uses a separate reserved trait slot (SELECTABLE_TRAIT).
 */
class DALI_UI_API SelectableTraitImpl : public TraitImpl, public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::Ui::SelectableTrait::SelectableTrait
   */
  SelectableTraitImpl();

public: // Signals
  /**
   * @copydoc Dali::Ui::SelectableTrait::SelectionChangedSignal
   */
  Signal<void(View, bool, const InputEvent&)>& SelectionChangedSignal();

public: // API
  /**
   * @copydoc Dali::Ui::SelectableTrait::IsSelected
   */
  bool IsSelected() const;

  /**
   * @copydoc Dali::Ui::SelectableTrait::SetSelected
   */
  void SetSelected(bool selected);

  /**
   * @copydoc Dali::Ui::SelectableTrait::IsToggleByClickEnabled
   */
  bool IsToggleByClickEnabled() const;

  /**
   * @copydoc Dali::Ui::SelectableTrait::EnableToggleByClick
   */
  void EnableToggleByClick(bool enabled);

protected:
  /**
   * @copydoc Dali::Ui::SelectableTrait::~SelectableTrait
   */
  virtual ~SelectableTraitImpl() override;

  /**
   * @brief Gets the owner view
   */
  View GetOwner() const;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnBeforeAttached
   */
  void OnBeforeAttached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnAttached
   */
  void OnAttached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnDetached
   */
  void OnDetached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnViewDestroying
   */
  void OnViewDestroying(ViewImpl* viewImpl) override;

  /**
   * @brief Called when the selection state is about to change.
   *
   * Subclasses (e.g. GroupSelectableTraitImpl) can override this to
   * implement group deselection logic or to veto a selection change.
   *
   * @param[in] view The owner view
   * @param[in] newSelected The proposed new selection state
   * @return True to allow the change, false to reject it
   */
  virtual bool OnSelectionChanging(View view, bool newSelected);

private:
  void EnsureClickableAndConnect();
  void DisconnectClickable();
  void SetSelectedInternal(bool selected, const InputEvent& event);
  void OnClickedForToggle(View view, const InputEvent& event);

private:
  WeakHandle<View>                            mOwner;
  Signal<void(View, bool, const InputEvent&)> mSelectionChangedSignal;
  bool                                        mSelected : 1;
  bool                                        mToggleByClickEnabled : 1;
  bool                                        mAttached : 1;
};

} // namespace Integration

inline Integration::SelectableTraitImpl& GetImpl(SelectableTrait& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::SelectableTraitImpl&>(handle);
}

inline const Integration::SelectableTraitImpl& GetImpl(const SelectableTrait& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::SelectableTraitImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
