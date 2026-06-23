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
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{

namespace Ui
{

// Forward declaration (view.h provides full definition when needed)
class View;

// Forward declarations
namespace Internal
{
class CoreInteractionObject;
}

/**
 * @brief SelectableTrait is a state trait that provides selection behavior to a View.
 *
 * SelectableTrait manages a boolean selected state and emits a signal when
 * the state changes.
 *
 * Selectable implies Interactive. A View with SelectableTrait also has
 * InteractiveTrait behavior such as click handling, pressed state handling, and
 * interactive StateEffect attachment. SelectableTrait derives from
 * InteractiveTrait so callers can use the returned handle for both selection
 * APIs and interactive APIs such as ClickedSignal().
 *
 * Internally both traits share the core interaction trait slot.
 *
 * @note InteractiveTrait, SelectableTrait, and GroupSelectableTrait are facets of a
 * single shared interaction object on a View. Comparing handles with operator== compares
 * that same underlying object, and DownCast is presence-based (does the requested facet's
 * sub-implementation exist?) rather than identity-based.
 */
class DALI_UI_API SelectableTrait : public InteractiveTrait
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized SelectableTrait handle.
   */
  SelectableTrait();

  /**
   * @brief Downcasts a handle to SelectableTrait handle.
   *
   * If the handle refers to a SelectableTrait (e.g. a trait from GetTrait),
   * the downcast produces a valid handle. Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object stored in View's selectable trait slot
   * @return A handle to SelectableTrait or an uninitialized handle
   */
  static SelectableTrait DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] selectableTrait Handle to copy
   */
  SelectableTrait(const SelectableTrait& selectableTrait);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SelectableTrait();

public: // Signals
  /**
   * @brief Emitted when the selection state changes.
   *
   * This signal is emitted when the View's selected state changes, either
   * programmatically via SetSelected() or through toggle-by-click interaction.
   *
   * The callback signature is: void YourCallbackName(View view, bool selected)
   *
   * @return The selection changed signal
   */
  Signal<void(View, bool, InputEvent)>& SelectionChangedSignal();

public: // API
  /**
   * @brief Returns whether this View is currently selected.
   *
   * @return True if the View is selected
   */
  bool IsSelected() const;

  /**
   * @brief Sets the selection state of this View.
   *
   * If the new state differs from the current state, the SelectionChangedSignal
   * is emitted.
   *
   * @param[in] selected True to select, false to unselect
   */
  void SetSelected(bool selected);

  /**
   * @brief Returns whether toggle-by-click is enabled.
   *
   * When enabled, clicking the owner View toggles the selected state.
   *
   * @return True if toggle-by-click is enabled
   */
  bool IsToggleByClickEnabled() const;

  /**
   * @brief Enables or disables toggle-by-click.
   *
   * When enabled, the SelectableTrait listens to the owner View's ClickedSignal
   * and toggles the selected state on each click. Disabling this option only
   * disables automatic selection toggling; the inherited InteractiveTrait
   * behavior remains attached to the owner View.
   *
   * @note While the View is a member of a SelectionGroup, a click on the already-selected
   * member is a no-op (it cannot unselect the winner and empty the group); other members
   * still select normally. Grouping does NOT change this toggle-by-click setting; the group
   * never saves or restores this value.
   *
   * @param[in] enabled True to enable toggle-by-click, false to disable
   */
  void EnableToggleByClick(bool enabled = true);

public: // Not intended for application developers
  /**
   * @brief Creates an internal SelectableTrait handle.
   *
   * The returned handle stores a CoreInteractionObject and owns both interactive and selectable trait implementations.
   * Application developers should obtain this trait through View::AsSelectable().
   *
   * @return A handle to a newly allocated SelectableTrait.
   */
  DALI_INTERNAL static SelectableTrait New();

  /**
   * @brief Creates a handle using the internal core interaction trait object.
   *
   * @param[in] container The core interaction trait object
   * @return A handle to SelectableTrait
   */
  DALI_INTERNAL static SelectableTrait New(Internal::CoreInteractionObject* container);

protected:
  explicit DALI_INTERNAL SelectableTrait(Internal::CoreInteractionObject* container);
};

} // namespace Ui

} // namespace Dali
