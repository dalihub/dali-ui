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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/toast-style.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class ToastImpl;
}

/**
 * @brief A transient, non-modal message posted above a Window's content.
 *
 * Toast supports an optional action button. The first action click expands the
 * message, and a second action click invokes the expanded action and dismisses
 * the Toast. Toast is attached only by Post(Window), and is never a Navigator
 * page.
 */
class DALI_UI_COMPONENTS_API Toast : public View
{
public:
  /**
   * @brief Predefined durations in milliseconds for a Toast presentation.
   */
  struct Duration
  {
    /** @brief Keeps the Toast visible until it is dismissed. */
    static constexpr uint32_t INDEFINITE = 0xFFFFFFFFu;
    /** @brief Displays the Toast for 1,500 milliseconds. */
    static constexpr uint32_t SHORT = 1500u;
    /** @brief Displays the Toast for 2,500 milliseconds. */
    static constexpr uint32_t LONG = 2500u;
  };

  /**
   * @brief Identifies the action-button confirmation stage.
   */
  enum class ActionStage
  {
    /** @brief The action button was clicked in the default presentation. */
    CONFIRM_IN_DEFAULT_MODE,
    /** @brief The action button was clicked in the expanded presentation. */
    CONFIRM_IN_EXPANDED_MODE
  };

  /** @brief Signal emitted after the Toast is shown. */
  using ShownSignalType = Signal<void(Toast)>;
  /** @brief Signal emitted after the Toast is hidden. */
  using HiddenSignalType = Signal<void(Toast)>;
  /** @brief Signal emitted when the Toast action button is clicked. */
  using ActionButtonClickedSignalType = Signal<void(Toast, ActionStage)>;

  /** @brief Creates an uninitialized Toast handle. */
  Toast();
  /** @brief Destructor. */
  ~Toast();

  /** @brief Creates a Toast using the current default style.
   * @return An initialized Toast
   */
  static Toast New();
  /** @brief Creates a Toast using @p style.
   * @param[in] style The style to apply
   * @return An initialized Toast
   * @pre @p style must be initialized.
   */
  static Toast New(ToastStyle style);
  /** @brief Creates a Toast with @p text and the current default style.
   * @param[in] text The message text
   * @return An initialized Toast
   */
  static Toast New(const Dali::String& text);
  /** @brief Creates a Toast with @p text and @p style.
   * @param[in] text The message text
   * @param[in] style The style to apply
   * @return An initialized Toast
   * @pre @p style must be initialized.
   */
  static Toast New(const Dali::String& text, ToastStyle style);
  /** @brief Creates a Toast with @p text and @p duration.
   * @param[in] text The message text
   * @param[in] duration The display duration in milliseconds
   * @return An initialized Toast
   */
  static Toast New(const Dali::String& text, uint32_t duration);
  /** @brief Creates a Toast with @p text, @p duration, and @p style.
   * @param[in] text The message text
   * @param[in] duration The display duration in milliseconds
   * @param[in] style The style to apply
   * @return An initialized Toast
   * @pre @p style must be initialized.
   */
  static Toast New(const Dali::String& text, uint32_t duration, ToastStyle style);
  /** @brief Downcasts a base handle to Toast.
   * @param[in] handle The handle to downcast
   * @return A Toast handle, or an uninitialized handle if the types do not match
   */
  static Toast DownCast(BaseHandle handle);

  /** @brief Copy constructor.
   * @param[in] handle The handle to copy
   */
  Toast(const Toast& handle);
  /** @brief Move constructor.
   * @param[in] rhs The handle to move
   */
  Toast(Toast&& rhs) noexcept;
  /** @brief Copy assignment operator.
   * @param[in] handle The handle to copy
   * @return A reference to this handle
   */
  Toast& operator=(const Toast& handle);
  /** @brief Move assignment operator.
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  Toast& operator=(Toast&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Toast)

  /** @brief Sets the message text.
   * @param[in] text The message text
   */
  void SetText(const Dali::String& text);
  /** @brief Returns the message text.
   * @return The message text
   */
  Dali::String GetText() const;

  /** @brief Sets the display duration.
   * @param[in] duration The duration in milliseconds, or Duration::INDEFINITE
   */
  void SetDuration(uint32_t duration);
  /** @brief Returns the display duration.
   * @return The duration in milliseconds, or Duration::INDEFINITE
   */
  uint32_t GetDuration() const;

  /** @brief Sets the message text color.
   * @param[in] color The color to apply
   */
  void SetTextColor(const UiColor& color);
  /** @brief Returns the message text color.
   * @return The message text color
   */
  UiColor GetTextColor() const;

  /** @brief Sets the message font size.
   * @param[in] fontSize The font size in logical pixels
   */
  void SetFontSize(float fontSize);
  /** @brief Returns the message font size.
   * @return The font size in logical pixels
   */
  float GetFontSize() const;

  /** @brief Sets the message font family.
   * @param[in] fontFamily The font family name
   */
  void SetFontFamily(const Dali::String& fontFamily);
  /** @brief Returns the message font family.
   * @return The font family name
   */
  Dali::String GetFontFamily() const;

  /** @brief Sets the action-button text.
   * @param[in] text The action-button text
   */
  void SetActionButtonText(const Dali::String& text);
  /** @brief Returns the action-button text.
   * @return The action-button text
   */
  Dali::String GetActionButtonText() const;

  /** @brief Sets the action-button text color.
   * @param[in] color The color to apply
   */
  void SetActionButtonTextColor(const UiColor& color);
  /** @brief Returns the action-button text color.
   * @return The action-button text color
   */
  UiColor GetActionButtonTextColor() const;

  /** @brief Sets the spacing between Toast content items.
   * @param[in] spacing The spacing in logical pixels
   */
  void SetItemSpacing(float spacing);
  /** @brief Returns the spacing between Toast content items.
   * @return The spacing in logical pixels
   */
  float GetItemSpacing() const;

  /**
   * @brief Posts this Toast to the explicit Window overlay layer.
   *
   * Reposting the same instance replaces its current presentation without
   * emitting HiddenSignal for the superseded presentation.
   */
  void Post(Dali::Window window);

  /**
   * @brief Dismisses this Toast if it is currently posted.
   */
  void Dismiss();

  /** @brief Returns the signal emitted after the Toast is shown.
   * @return The shown signal
   */
  ShownSignalType& ShownSignal();
  /** @brief Returns the signal emitted after the Toast is hidden.
   * @return The hidden signal
   */
  HiddenSignalType& HiddenSignal();
  /** @brief Returns the signal emitted when the action button is clicked.
   * @return The action-button-clicked signal
   */
  ActionButtonClickedSignalType& ActionButtonClickedSignal();

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL Toast(Internal::ToastImpl& implementation);
  explicit DALI_INTERNAL Toast(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
