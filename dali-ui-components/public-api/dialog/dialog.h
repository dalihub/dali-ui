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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class DialogImpl;
}

/**
 * @brief Dialog is a three-section (header / body / footer) container control.
 *
 * It arranges its header, body and footer sections vertically (via an internal
 * stack layout) and is typically presented modally inside a DialogContainer.
 * This is the base class for AlertDialog.
 */
class DALI_UI_API Dialog : public View
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

public: // Sections
  /**
   * @brief Sets the header section view (shown at the top).
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
} // namespace Dali
