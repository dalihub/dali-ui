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
 */

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/selection-animation-mode.h>
#include <dali-ui-components/public-api/styles/radio-button-style.h>
#include <dali-ui-foundation/public-api/views/group-selectable-view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class RadioButtonImpl;
}

/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief A single-selection button that participates in a SelectionGroup.
 *
 * A RadioButton is selected by click but is not deselected by clicking it again.
 * Programmatic SetSelected(false) remains available. Group selection, selection signals,
 * click behavior, and explicit or parent-based grouping are inherited from
 * GroupSelectableView.
 *
 * @note RadioButton is icon-only. To compose a labelled option, use an outer
 * GroupSelectableView as the click, grouping, focus, and accessibility owner. Make the
 * display-only child RadioButton touch-transparent with SetSensitive(false), remove it from
 * focus with SetFocusable(false), hide it from accessibility, and mirror the outer row's
 * selection to the child. SetClickable(false) allows unhandled touch to propagate to the
 * parent, but does not remove a View from hit testing. Enabled long-press handling continues
 * to consume its touch stream independently of clicking; use SetLongPressEnabled(false) to
 * stop it while keeping its signal handlers connected.
 */
class DALI_UI_COMPONENTS_API RadioButton : public GroupSelectableView
{
public:
  /**
   * @brief Creates an uninitialized RadioButton handle.
   */
  RadioButton();

  /**
   * @brief Destructor.
   */
  ~RadioButton();

  /**
   * @brief Creates a RadioButton using the current default style.
   * @return An initialized RadioButton
   */
  static RadioButton New();
  /**
   * @brief Creates a RadioButton using @p style.
   * @param[in] style The style to apply
   * @return An initialized RadioButton
   * @pre @p style must be initialized.
   */
  static RadioButton New(RadioButtonStyle style);
  /**
   * @brief Downcasts a base handle to RadioButton.
   * @param[in] handle The handle to downcast
   * @return A RadioButton handle, or an uninitialized handle if the types do not match
   */
  static RadioButton DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   * @param[in] handle The handle to copy
   */
  RadioButton(const RadioButton& handle);
  /**
   * @brief Move constructor.
   * @param[in] rhs The handle to move
   */
  RadioButton(RadioButton&& rhs) noexcept;
  /**
   * @brief Copy assignment operator.
   * @param[in] handle The handle to copy
   * @return A reference to this handle
   */
  RadioButton& operator=(const RadioButton& handle);
  /**
   * @brief Move assignment operator.
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  RadioButton& operator=(RadioButton&& rhs) noexcept;

  DALI_UI_VIEW_WITH(RadioButton)

  /**
   * @brief Sets the selection animation mode.
   *
   * Animation is skipped when the RadioButton is off-scene or invisible, regardless of
   * the requested mode.
   *
   * @param[in] mode The animation policy
   */
  void SetSelectionAnimationMode(SelectionAnimationMode mode);
  /**
   * @brief Returns the selection animation mode.
   * @return The current animation policy
   */
  SelectionAnimationMode GetSelectionAnimationMode() const;

  /**
   * @brief Sets the requested icon width.
   *
   * A non-positive or non-finite value is normalized to zero, which means that no explicit
   * icon width is requested.
   *
   * @param[in] width The requested width in logical pixels
   */
  void SetIconWidth(float width);
  /**
   * @brief Returns the requested icon width.
   * @return The requested width, or zero when unset
   */
  float GetIconWidth() const;
  /**
   * @brief Sets the requested icon height.
   *
   * A non-positive or non-finite value is normalized to zero, which means that no explicit
   * icon height is requested.
   *
   * @param[in] height The requested height in logical pixels
   */
  void SetIconHeight(float height);
  /**
   * @brief Returns the requested icon height.
   * @return The requested height, or zero when unset
   */
  float GetIconHeight() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL RadioButton(Internal::RadioButtonImpl& implementation);
  explicit DALI_INTERNAL RadioButton(Dali::Internal::CustomActor* internal);
  /// @endcond
};

/** @} */

} // namespace Ui
} // namespace Dali
