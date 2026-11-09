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
#include <dali-ui-components/public-api/styles/alert-dialog-style.h>
#include <dali-ui-components/public-api/text-button.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>

namespace DALI_NAMESPACE
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
 * Generated title/message labels wrap to the content width. Sections stack from
 * the top with a content-driven height; the footer is not pinned to the bottom.
 * Actions retain their natural/minimum size and form a centered horizontal group.
 * Choose a width appropriate for the host and action content through the existing
 * style/layout APIs. Post does not clamp oversized content or add scrolling.
 * Use the inherited Dialog::Post() and Dialog::Dismiss() APIs for ordinary modal presentation.
 * Action buttons do not dismiss automatically; call Dismiss after handling an
 * action. For custom scrim composition, use a manually owned DialogContainer
 * instead and follow Dialog's prohibition on mixing the two ownership paths.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API AlertDialog : public Dialog
{
public:
  /**
   * @brief Creates an uninitialized AlertDialog handle.
   * @SINCE_3_0.0
   */
  AlertDialog();

  /**
   * @brief Creates an initialized AlertDialog.
   * @SINCE_3_0.0
   * @return A handle to a newly allocated Dali resource
   */
  static AlertDialog New();

  /**
   * @brief Creates an AlertDialog with the specified immutable style. Empty styles are not allowed.
   * @SINCE_3_0.0
   */
  static AlertDialog New(AlertDialogStyle style);

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   * @param[in] alertDialog Handle to copy
   */
  AlertDialog(const AlertDialog& alertDialog);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   * @param[in] rhs Handle to move
   */
  AlertDialog(AlertDialog&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~AlertDialog();

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  AlertDialog& operator=(const AlertDialog& handle);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  AlertDialog& operator=(AlertDialog&& rhs) noexcept;

  DALI_UI_VIEW_WITH(AlertDialog)

  /**
   * @brief Downcasts a handle to an AlertDialog handle.
   * @SINCE_3_0.0
   * @param[in] handle Handle to an object
   * @return A handle to an AlertDialog or an uninitialized handle
   */
  static AlertDialog DownCast(BaseHandle handle);

public: // Convenience API
  /**
   * @brief Sets the title text (shown as the header). Empty clears the header.
   * @SINCE_3_0.0
   * @param[in] title The title text
   */
  void SetTitle(const Dali::String& title);

  /// @brief Gets the title text.
  /// @SINCE_3_0.0
  Dali::String GetTitle() const;

  /**
   * @brief Sets the message text (shown as the body). Empty clears the body.
   * @SINCE_3_0.0
   * @param[in] message The message text
   */
  void SetMessage(const Dali::String& message);

  /// @brief Gets the message text.
  /// @SINCE_3_0.0
  Dali::String GetMessage() const;

  /**
   * @brief Adds an action button to the footer.
   *
   * Action buttons form a centered group at their natural/minimum sizes.
   * Connect a handler to the returned button's ClickedSignal() to handle the
   * action; clicking a button does not dismiss the dialog automatically.
   *
   * @SINCE_3_0.0
   * @param[in] text The button text
   * @return The newly added action button
   */
  TextButton AddActionButton(const Dali::String& text);

  /**
   * @brief Removes all action buttons and clears the footer.
   * @SINCE_3_0.0
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
} //namespace DALI_NAMESPACE
