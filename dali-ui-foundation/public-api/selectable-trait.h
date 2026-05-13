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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{

namespace Ui
{

// Forward declaration (view.h provides full definition when needed)
class View;

// Forward declarations
namespace Integration
{
class SelectableTraitImpl;
}

/**
 * @brief SelectableTrait is a state trait that provides selection behavior to a View.
 *
 * SelectableTrait manages a boolean selected state and emits a signal when
 * the state changes. It can optionally toggle the selected state on click
 * by leveraging the owner View's InteractiveTrait.
 *
 * Unlike InteractiveTrait, SelectableTrait does not handle key events or focus
 * directly. It uses a separate reserved trait slot (SELECTABLE_TRAIT) and can
 * coexist with InteractiveTrait on the same View.
 *
 * @note When toggle-by-click is enabled and the owner View does not have a
 * InteractiveTrait, one is automatically created and attached.
 */
class DALI_UI_API SelectableTrait : public BaseHandle
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized SelectableTrait handle.
   */
  SelectableTrait();

  /**
   * @brief Creates an initialized SelectableTrait.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static SelectableTrait New();

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
   * is emitted. Subclass implementations (e.g. GroupSelectable) may veto or
   * modify the state change.
   *
   * @param[in] selected True to select, false to deselect
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
   * and toggles the selected state on each click. If the owner View does not
   * already have a InteractiveTrait, one is automatically created and attached.
   *
   * @param[in] enabled True to enable toggle-by-click, false to disable
   */
  void EnableToggleByClick(bool enabled = true);

public: // Not intended for application developers
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The implementation
   */
  explicit SelectableTrait(Integration::SelectableTraitImpl* implementation);
};

} // namespace Ui

} // namespace Dali
