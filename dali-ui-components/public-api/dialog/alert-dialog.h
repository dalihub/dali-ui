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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-components/public-api/text-button.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class AlertDialogImpl;
}

/**
 * @brief AlertDialog is a convenience Dialog with title, message and action buttons.
 *
 * It derives from Dialog and auto-builds the header (title), body (message) and
 * footer (action buttons) sections from SetTitle / SetMessage / AddActionButton.
 */
class DALI_UI_COMPONENTS_API AlertDialog : public Dialog
{
public:
  /**
   * @brief Creates an uninitialized AlertDialog handle.
   */
  AlertDialog();

  /**
   * @brief Creates an initialized AlertDialog.
   * @return A handle to a newly allocated Dali resource
   */
  static AlertDialog New();

  /**
   * @brief Copy constructor.
   * @param[in] alertDialog Handle to copy
   */
  AlertDialog(const AlertDialog& alertDialog);

  /**
   * @brief Move constructor.
   * @param[in] rhs Handle to move
   */
  AlertDialog(AlertDialog&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~AlertDialog();

  /**
   * @brief Copy assignment operator.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  AlertDialog& operator=(const AlertDialog& handle);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  AlertDialog& operator=(AlertDialog&& rhs) noexcept;

  DALI_UI_VIEW_WITH(AlertDialog)

  /**
   * @brief Downcasts a handle to an AlertDialog handle.
   * @param[in] handle Handle to an object
   * @return A handle to an AlertDialog or an uninitialized handle
   */
  static AlertDialog DownCast(BaseHandle handle);

public: // Convenience API
  /**
   * @brief Sets the title text (shown as the header). Empty clears the header.
   * @param[in] title The title text
   */
  void SetTitle(const Dali::String& title);

  /// @brief Gets the title text.
  Dali::String GetTitle() const;

  /**
   * @brief Sets the message text (shown as the body). Empty clears the body.
   * @param[in] message The message text
   */
  void SetMessage(const Dali::String& message);

  /// @brief Gets the message text.
  Dali::String GetMessage() const;

  /**
   * @brief Adds an action button to the footer.
   *
   * Action buttons are distributed evenly across the footer. Connect a handler
   * to the returned button's ClickedSignal() to handle the action.
   *
   * @param[in] text The button text
   * @return The newly added action button
   */
  TextButton AddActionButton(const Dali::String& text);

  /**
   * @brief Removes all action buttons and clears the footer.
   */
  void ClearActionButtons();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   * @param[in] implementation The AlertDialog implementation
   */
  DALI_INTERNAL AlertDialog(Integration::AlertDialogImpl& implementation);

  /**
   * @brief Allows the creation of this handle from an Internal::CustomActor pointer.
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL AlertDialog(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
