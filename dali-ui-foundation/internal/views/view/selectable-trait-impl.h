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
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>

namespace Dali
{

namespace Ui
{

class InputEvent;

namespace Internal
{
class CoreInteractionObject;

/**
 * @brief Internal implementation of Selectable trait.
 *
 * SelectableTraitImpl is stored inside CoreInteractionObject.
 * It manages the selected state of a View and optionally
 * toggles selection on click by listening to the owner View's InteractiveTrait.
 * It is installed as the selectable trait implementation inside CoreInteractionObject.
 */
class SelectableTraitImpl : public ConnectionTracker
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
  Signal<void(View, bool, InputEvent)>& SelectionChangedSignal();

  /**
   * @brief Type of the internal-only post-commit selection observer.
   *
   * A plain (stateless) function pointer: it receives the owner View and re-resolves any
   * collaborator from it, so no per-instance object identity is captured or stored. This keeps
   * the slot one pointer wide, never dangling, and needing no lifetime cleanup.
   */
  using SelectionCommitObserver = void (*)(View view, bool selected, InputEvent event);

  /**
   * @brief Sets the internal-only post-commit selection observer (single slot).
   *
   * Invoked from SetSelectedInternal AFTER the state is committed and immediately BEFORE the
   * public SelectionChangedSignal, so an internal collaborator (GroupSelectableTraitImpl) can
   * fully settle group-selection arbitration before any user-facing SelectionChangedSignal
   * callback runs, independent of signal connection order. NOT mirrored on the public
   * SelectableTrait handle; single known consumer. Pass nullptr to clear.
   *
   * @param[in] observer The observer function, or nullptr to clear
   */
  void SetSelectionCommitObserver(SelectionCommitObserver observer);

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
   * @brief Sets the selection state, carrying the originating input cause.
   *
   * Internal-only overload (not exposed on the public SelectableTrait handle) used by
   * collaborators such as GroupSelectableTraitImpl to preserve the real click cause
   * instead of substituting InputEvent::Programmatic().
   *
   * @param[in] selected True to select, false to unselect
   * @param[in] event    The originating input cause
   */
  void SetSelected(bool selected, InputEvent event);

  /**
   * @copydoc Dali::Ui::SelectableTrait::IsToggleByClickEnabled
   */
  bool IsToggleByClickEnabled() const;

  /**
   * @copydoc Dali::Ui::SelectableTrait::SetToggleByClickEnabled
   */
  void SetToggleByClickEnabled(bool enabled);

  /**
   * @brief Returns whether clicks are select-only (a click can only select, never unselect).
   * @return True if click-to-unselect is prevented
   */
  bool IsSelectOnlyByClickEnabled() const;

  /**
   * @brief Internal-only knob (not on the public SelectableTrait handle), used only by
   * GroupSelectableTraitImpl.
   *
   * While set, OnClickedForToggle becomes select-only: a click keeps an already-selected
   * View selected (never unselects). It does NOT touch toggle-by-click
   * (mToggleByClickEnabled); if toggle-by-click is disabled the click path is inert.
   * Programmatic SetSelected(false) and SelectionGroupImpl::ClearSelection() are unaffected
   * (they bypass the click path).
   *
   * @param[in] selectOnly True to make clicks select-only, false for normal toggle
   */
  void SetSelectOnlyByClick(bool selectOnly);

  /**
   * @copydoc Dali::Ui::SelectableTrait::~SelectableTrait
   */
  virtual ~SelectableTraitImpl();

protected:
  friend class CoreInteractionObject;

  /**
   * @brief Gets the owner view
   */
  View GetOwner() const;

  void OnAttached(View& view);

  void OnDetaching(View& view);

  void OnViewDestroying(ViewImpl* viewImpl);

private:
  void EnsureClickableAndConnect();
  void DisconnectClickable();
  void SetSelectedInternal(bool selected, InputEvent event);
  void OnClickedForToggle(View view, InputEvent event);

private:
  WeakHandle<View>                     mOwner;
  Signal<void(View, bool, InputEvent)> mSelectionChangedSignal;
  SelectionCommitObserver              mSelectionCommitObserver; ///< Internal-only (GroupSelectableTraitImpl); stateless post-commit hook, invoked before mSelectionChangedSignal.
  bool                                 mSelected : 1;
  bool                                 mToggleByClickEnabled : 1;
  bool                                 mSelectOnlyByClick : 1; ///< Internal-only (GroupSelectableTraitImpl). When set, OnClickedForToggle keeps a selected View selected; never touches toggle-by-click.
  bool                                 mAttached : 1;
};

} // namespace Internal

Internal::SelectableTraitImpl&       GetImpl(SelectableTrait& obj);
const Internal::SelectableTraitImpl& GetImpl(const SelectableTrait& obj);

} // namespace Ui

} // namespace Dali
