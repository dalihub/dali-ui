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

#include <dali-ui-foundation/public-api/interactive-view.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>

namespace Dali
{

namespace Ui
{

class SelectableViewImpl;

/**
 * @brief SelectableView is an InteractiveView subclass with selectable behavior built in.
 *
 * SelectableView guarantees that a SelectableTrait is attached for the lifetime
 * of the view. Since SelectableTrait implies InteractiveTrait, SelectableView
 * also exposes the inherited InteractiveView API directly.
 *
 * SelectableView is intended as a base class for selectable UI components such
 * as Switch and Checkbox.
 *
 * @see SelectableTrait
 * @see InteractiveView
 */
class DALI_UI_API SelectableView : public InteractiveView
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized SelectableView handle.
   */
  SelectableView();

  /**
   * @brief Creates an initialized SelectableView.
   *
   * @return A handle to a newly allocated SelectableView
   */
  static SelectableView New();

  /**
   * @brief Downcasts a handle to a SelectableView handle.
   *
   * If the handle refers to a SelectableView, the downcast produces a valid handle.
   * Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a SelectableView or an uninitialized handle
   */
  static SelectableView DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * @param[in] view Handle to copy
   */
  SelectableView(const SelectableView& view);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  SelectableView(SelectableView&& rhs) noexcept;

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~SelectableView();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  SelectableView& operator=(const SelectableView& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  SelectableView& operator=(SelectableView&& rhs) noexcept = default;

  DALI_UI_VIEW_WITH(SelectableView)

public: // Signals
  /**
   * @brief Emitted when the selection state changes.
   *
   * @return The selection changed signal
   * @see SelectableTrait::SelectionChangedSignal
   */
  Signal<void(View, bool, InputEvent)>& SelectionChangedSignal();

public: // API
  /**
   * @brief Returns whether the view is currently selected.
   *
   * @return True if selected
   * @see SelectableTrait::IsSelected
   */
  bool IsSelected() const;

  /**
   * @brief Sets the selection state.
   *
   * @param[in] selected True to select, false to unselect
   * @see SelectableTrait::SetSelected
   */
  void SetSelected(bool selected);

  /**
   * @brief Returns whether toggle-by-click is enabled.
   *
   * @return True if toggle-by-click is enabled
   * @see SelectableTrait::IsToggleByClickEnabled
   */
  bool IsToggleByClickEnabled() const;

  /**
   * @brief Enables or disables toggle-by-click.
   *
   * @param[in] enabled True to enable toggle-by-click, false to disable
   * @see SelectableTrait::EnableToggleByClick
   */
  void EnableToggleByClick(bool enabled = true);

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The SelectableView implementation
   */
  explicit DALI_UI_API SelectableView(SelectableViewImpl& implementation);

  /**
   * @brief Allows the creation of this SelectableView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API SelectableView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui

} // namespace Dali
