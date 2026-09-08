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

#include <dali-ui-components/public-api/styles/toast-style.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class ToastImpl;
}

/**
 * @brief A transient, non-modal message with an optional icon.
 *
 * Toast provides brief feedback that does not require an action or
 * confirmation. It disappears automatically after its finite duration.
 * Message text wraps without a fixed line-count limit; the ToastStyle maximum
 * height constrains visible content and overflowing text is ellipsized.
 */
class DALI_UI_COMPONENTS_API Toast : public View
{
public:
  /** @brief Predefined display durations in milliseconds. */
  struct Duration
  {
    /** @brief Displays the Toast for 1,500 milliseconds. */
    static constexpr uint32_t SHORT = 1500u;
    /** @brief Displays the Toast for 2,500 milliseconds. */
    static constexpr uint32_t LONG = 2500u;
  };

  /** @brief Signal emitted after the Toast is shown. */
  using ShownSignalType = Signal<void(Toast)>;
  /** @brief Signal emitted after the Toast is hidden. */
  using HiddenSignalType = Signal<void(Toast)>;

  /**
   * @brief Creates an uninitialized Toast handle.
   */
  Toast();

  /**
   * @brief Destructor.
   */
  ~Toast();

  /**
   * @brief Creates a Toast using the current default style and short duration.
   * @return The initialized Toast
   */
  static Toast New();

  /**
   * @brief Creates a Toast using @p style and short duration.
   * @param[in] style The initialized style to apply
   * @return The initialized Toast
   * @pre @p style must be initialized.
   */
  static Toast New(ToastStyle style);

  /**
   * @brief Creates a Toast with @p text and short duration.
   * @param[in] text The message to display
   * @return The initialized Toast
   */
  static Toast New(const Dali::String& text);

  /**
   * @brief Creates a Toast with @p text, @p style, and short duration.
   * @param[in] text The message to display
   * @param[in] style The initialized style to apply
   * @return The initialized Toast
   * @pre @p style must be initialized.
   */
  static Toast New(const Dali::String& text, ToastStyle style);

  /**
   * @brief Creates a Toast with @p text and @p duration.
   * @param[in] text The message to display
   * @param[in] duration The finite display duration in milliseconds
   * @return The initialized Toast
   * @pre @p duration must be greater than zero.
   */
  static Toast New(const Dali::String& text, uint32_t duration);

  /**
   * @brief Creates a Toast with @p text, @p duration, and @p style.
   * @param[in] text The message to display
   * @param[in] duration The finite display duration in milliseconds
   * @param[in] style The initialized style to apply
   * @return The initialized Toast
   * @pre @p duration must be greater than zero and @p style must be initialized.
   */
  static Toast New(const Dali::String& text, uint32_t duration, ToastStyle style);

  /**
   * @brief Downcasts a base handle to Toast.
   * @param[in] handle The handle to downcast
   * @return A valid Toast on success, or an uninitialized handle
   */
  static Toast DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   * @param[in] handle The Toast handle to copy
   */
  Toast(const Toast& handle);

  /**
   * @brief Move constructor.
   * @param[in] rhs The Toast handle to move from
   */
  Toast(Toast&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @param[in] handle The Toast handle to copy
   * @return A reference to this handle
   */
  Toast& operator=(const Toast& handle);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs The Toast handle to move from
   * @return A reference to this handle
   */
  Toast& operator=(Toast&& rhs) noexcept;

  DALI_UI_VIEW_WITH(Toast)

  /**
   * @brief Sets the message text.
   *
   * The message wraps without a fixed line-count limit and remains constrained
   * by the maximum Toast height.
   * @param[in] text The message to display
   */
  void SetText(const Dali::String& text);

  /**
   * @brief Returns the message text.
   * @return The current message
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the optional icon image URL.
   *
   * Setting an empty URL hides the icon and lets the message use the full
   * content width.
   * @param[in] url The image resource URL, or an empty string for no icon
   */
  void SetIconResourceUrl(const Dali::String& url);

  /**
   * @brief Returns the optional icon image URL.
   * @return The image resource URL, or an empty string when no icon is set
   */
  Dali::String GetIconResourceUrl() const;

  /**
   * @brief Sets the color multiplier applied to the optional icon.
   * @param[in] color The icon color
   */
  void SetIconColor(const UiColor& color);

  /**
   * @brief Returns the color multiplier applied to the optional icon.
   * @return The current icon color
   */
  UiColor GetIconColor() const;

  /**
   * @brief Sets whether the optional icon is loaded synchronously.
   * @param[in] synchronous True to load the image synchronously
   */
  void SetIconSynchronousLoading(bool synchronous);

  /**
   * @brief Returns whether synchronous icon loading is enabled.
   * @return True when the icon is loaded synchronously
   */
  bool IsIconSynchronousLoading() const;

  /**
   * @brief Sets the finite display duration in milliseconds.
   *
   * Changing the duration while the Toast is posted restarts its timeout.
   * @param[in] duration A value greater than zero
   * @pre @p duration must be greater than zero.
   */
  void SetDuration(uint32_t duration);

  /**
   * @brief Returns the display duration in milliseconds.
   * @return The finite display duration
   */
  uint32_t GetDuration() const;

  /**
   * @brief Posts this Toast to the explicit Window overlay layer.
   *
   * Reposting the same Toast replaces its active presentation without
   * emitting HiddenSignal() for the superseded presentation.
   * @param[in] window The initialized Window that owns the presentation
   * @pre @p window must be initialized and provide an overlay layer.
   */
  void Post(Dali::Window window);

  /**
   * @brief Dismisses this Toast if it is currently posted.
   */
  void Dismiss();

  /**
   * @brief Returns the signal emitted after the Toast is shown.
   * @return The shown signal
   */
  ShownSignalType& ShownSignal();

  /**
   * @brief Returns the signal emitted after the Toast is hidden.
   * @return The hidden signal
   */
  HiddenSignalType& HiddenSignal();

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL Toast(Internal::ToastImpl& implementation);
  explicit DALI_INTERNAL Toast(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
