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
#include <dali-ui-components/public-api/dialog/dialog-properties.h>
#include <dali-ui-components/public-api/dialog/dialog-post-options.h>
#include <dali-ui-components/public-api/styles/dialog-style.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
class Navigator;
namespace Integration DALI_INTERNAL
{
class DialogImpl;
}

/**
 * @brief Dialog is a three-section (header / body / footer) container control.
 *
 * It arranges its header, body and footer sections vertically (via an internal
 * stack layout). This is the base class for AlertDialog.
 *
 * Prefer Post(navigator) and Dismiss() for modal presentation. Post creates and
 * registers the DialogContainer internally, preserving the Dialog's layout
 * parameters. Use DialogPostOptions::containerStyle for scrim color, blur, or
 * DialogContainerStyle::NoScrimPreset() without manually creating a container.
 *
 * For a custom scrim view or complex scrim composition, assemble a separate
 * DialogContainer with SetScrim/SetModalContent and Navigator::PushModal, and
 * manage its removal through Navigator. Do not mix manual parent management
 * with Post/Dismiss during a presentation, including its closing animation.
 * Switch paths only after the previous presentation and parent links are cleared.
 */
class DALI_UI_COMPONENTS_API Dialog : public View
{
public:
  /**
   * @brief Creates an uninitialized Dialog handle.
   */
  Dialog();

  /**
   * @brief Creates an initialized Dialog.
   * @return A handle to a newly allocated Dali resource
   */
  static Dialog New();

  /** @brief Creates a Dialog with the specified immutable style. Empty styles are not allowed. */
  static Dialog New(DialogStyle style);

  /**
   * @brief Copy constructor.
   * @param[in] dialog Handle to copy
   */
  Dialog(const Dialog& dialog);

  /**
   * @brief Move constructor.
   * @param[in] rhs Handle to move
   */
  Dialog(Dialog&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Dialog();

  /**
   * @brief Copy assignment operator.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  Dialog& operator=(const Dialog& handle);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  Dialog& operator=(Dialog&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Dialog)

  /**
   * @brief Downcasts a handle to a Dialog handle.
   * @param[in] handle Handle to an object
   * @return A handle to a Dialog or an uninitialized handle
   */
  static Dialog DownCast(BaseHandle handle);

public: // Presentation
  using DismissRequestedSignalType = Signal<bool(Dialog, DialogDismissReason)>;
  using ShownSignalType = Signal<void(Dialog)>;
  using HiddenSignalType = Signal<void(Dialog, DialogDismissReason)>;

  /**
   * @brief Presents this Dialog using a container owned by the given Navigator.
   * @param[in] navigator The Navigator that owns the modal presentation
   * @return True if accepted or already posted to the same Navigator. Returns
   * false for a different active host, a closing session, or rejected registration.
   * @pre Both handles must be initialized.
   * @pre An unposted Dialog must have no parent. Violations always assert.
   * @note Do not mix Post/Dismiss with direct Add/Remove/Unparent of this Dialog
   * or its managed container until HiddenSignal reports completion. This also
   * applies during closing animations. Use Dismiss or Navigator's modal removal
   * APIs instead. Detected violations always assert; hierarchy rollback and
   * continued use after catching the assertion exception are not supported.
   */
  bool Post(Navigator navigator);
  /**
   * @copydoc Post(Navigator)
   * @param[in] options Scrim style and transition settings for a new presentation
   * @note Posting again to the same Navigator preserves the active session;
   * it does not replace its options or move the Dialog to the top.
   */
  bool Post(Navigator navigator, const DialogPostOptions& options);
  /**
   * @brief Dismisses only this managed presentation. Repeated calls do nothing.
   * @param[in] animated Whether to use the Navigator's modal exit transition
   * @pre A Dialog without a managed presentation must have no parent.
   * Calling Dismiss on manually parented content always asserts.
   * @note Direct Add/Remove is allowed again only after presentation cleanup,
   * reported by HiddenSignal, not merely after this call returns when an
   * animation is running. For manual containers, use Navigator removal APIs
   * instead and detach the content before switching to Post.
   */
  void Dismiss(bool animated = true);
  /// @brief Whether a managed session exists, including covered and closing sessions.
  bool IsPosted() const;
  /// @brief Sets the allowed user requests. Defaults to BACK_AND_SCRIM.
  void SetDismissPolicy(DialogDismissPolicy policy);
  /// @brief Gets the allowed user requests.
  DialogDismissPolicy GetDismissPolicy() const;
  /// @brief Emitted after policy checks. Returning true vetoes a user request.
  DismissRequestedSignalType& DismissRequestedSignal();
  /// @brief Emitted once after first appearance in a presentation session.
  ShownSignalType& ShownSignal();
  /// @brief Emitted once after cleanup. The callback may post the Dialog again.
  HiddenSignalType& HiddenSignal();

public: // Sections
  /**
   * @brief Sets the header section view (shown at the top).
   * @note The supplied view retains its padding and appearance; Dialog manages section layout.
   * @param[in] headerView The view to use as the header, or an empty handle to clear it
   */
  void SetHeaderView(View headerView);

  /**
   * @brief Gets the header section view.
   * @return The header view, or an empty handle if none is set
   */
  View GetHeaderView() const;

  /**
   * @brief Sets the body section view (shown in the middle).
   * @note The supplied view retains its padding and appearance; Dialog manages section layout.
   * @param[in] bodyView The view to use as the body, or an empty handle to clear it
   */
  void SetBodyView(View bodyView);

  /**
   * @brief Gets the body section view.
   * @return The body view, or an empty handle if none is set
   */
  View GetBodyView() const;

  /**
   * @brief Sets the footer section view (shown at the bottom).
   * @note The supplied view retains its padding and appearance; Dialog manages section layout.
   * @param[in] footerView The view to use as the footer, or an empty handle to clear it
   */
  void SetFooterView(View footerView);

  /**
   * @brief Gets the footer section view.
   * @return The footer view, or an empty handle if none is set
   */
  View GetFooterView() const;

public: // Layout
  /**
   * @brief Sets the spacing between the header, body and footer sections.
   * @param[in] spacing The spacing in pixels
   */
  void SetSpacing(float spacing);

  /**
   * @brief Gets the spacing between sections.
   * @return The spacing in pixels
   */
  float GetSpacing() const;

  /**
   * @brief Sets the cross-axis alignment applied to the sections.
   * @param[in] alignment FILL (default), START, CENTER or END
   */
  void SetLayoutAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the cross-axis alignment applied to the sections.
   * @return The current layout alignment
   */
  LayoutAlignment GetLayoutAlignment() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   * @param[in] implementation The Dialog implementation
   */
  DALI_INTERNAL Dialog(Integration::DialogImpl& implementation);

  /**
   * @brief Allows the creation of this handle from an Internal::CustomActor pointer.
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL Dialog(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
