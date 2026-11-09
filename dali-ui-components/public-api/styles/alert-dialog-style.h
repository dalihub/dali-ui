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

#include <dali-ui-components/public-api/styles/dialog-style.h>
#include <dali-ui-components/public-api/styles/text-button-style.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/common/dali-string.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief An immutable presentation policy applied at creation. Does not store content or modal state.
 * @SINCE_3_0.0
 * @note UiConfig::Apply must be called before using Default or DefaultPreset.
 */
class DALI_UI_COMPONENTS_API AlertDialogStyle : public UiStyle
{
public:
  class Builder;
  /// @brief Creates an empty handle.
  /// @SINCE_3_0.0
  AlertDialogStyle() = default;
  /// @brief Gets the default key for registration with Components UiConfig.
  /// @SINCE_3_0.0
  static UiStyleKey<AlertDialogStyle> DefaultKey();
  /// @brief Gets the built-in defaults: content-height action row, top padding 12 and action spacing 32.
  /// @SINCE_3_0.0
  static AlertDialogStyle DefaultPreset();
  /// @brief Gets the style from the current configuration, falling back to the built-in defaults.
  /// @SINCE_3_0.0
  static AlertDialogStyle Default();
  /// @brief Returns an empty handle if the type does not match.
  /// @SINCE_3_0.0
  static AlertDialogStyle DownCast(BaseHandle handle);
  /// @brief Does not check the type. The caller must ensure that the actual type matches.
  /// @SINCE_3_0.0
  static AlertDialogStyle StaticDownCast(UiStyle style);
  /// @brief Returns a mutable builder independent of the original style.
  /// @SINCE_3_0.0
  Builder Configure() const;

  /// @brief Common appearance. An empty handle uses DialogStyle::Default at creation.
  /// @SINCE_3_0.0
  DialogStyle GetDialogStyle() const;
  /// @brief Title text color.
  /// @SINCE_3_0.0
  UiColor GetTitleTextColor() const;
  /// @brief Title font size. Must be finite and non-negative.
  /// @SINCE_3_0.0
  float GetTitleFontSize() const;
  /// @brief Title font family. An empty string preserves the default Label font.
  /// @SINCE_3_0.0
  Dali::String GetTitleFontFamily() const;
  /// @brief Message text color.
  /// @SINCE_3_0.0
  UiColor GetMessageTextColor() const;
  /// @brief Message font size. Must be finite and non-negative.
  /// @SINCE_3_0.0
  float GetMessageFontSize() const;
  /// @brief Message font family. An empty string preserves the default Label font.
  /// @SINCE_3_0.0
  Dali::String GetMessageFontFamily() const;
  /// @brief Explicit action appearance. An empty handle uses the Alert fallback and configured TextButton geometry.
  /// @SINCE_3_0.0
  TextButtonStyle GetActionButtonStyle() const;
  /// @brief Generated action row height: WRAP_CONTENT (default), or a finite non-negative fixed height.
  /// @SINCE_3_0.0
  float GetActionRowHeight() const;
  /// @brief Padding for the internally generated action row, not a user-supplied Footer.
  /// @SINCE_3_0.0
  Insets GetActionRowPadding() const;
  /// @brief Spacing between actions. Must be finite and non-negative.
  /// @SINCE_3_0.0
  float GetActionButtonSpacing() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL AlertDialogStyle(Extension::UiStyleImpl* impl);
  /// @endcond
};

/**
 * @brief A move-only builder that cannot be reused after Build.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API AlertDialogStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;
  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;
  ~Builder();

  /// @brief Common appearance. An empty handle uses DialogStyle::Default at creation.
  /// @SINCE_3_0.0
  Builder& SetDialogStyle(DialogStyle value) &;
  /// @copydoc SetDialogStyle
  Builder&& SetDialogStyle(DialogStyle value) &&;
  /// @brief Title text color.
  /// @SINCE_3_0.0
  Builder& SetTitleTextColor(const UiColor& value) &;
  /// @copydoc SetTitleTextColor
  Builder&& SetTitleTextColor(const UiColor& value) &&;
  /// @brief Title font size. Must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetTitleFontSize(float value) &;
  /// @copydoc SetTitleFontSize
  Builder&& SetTitleFontSize(float value) &&;
  /// @brief Title font family. An empty string preserves the default Label font.
  /// @SINCE_3_0.0
  Builder& SetTitleFontFamily(const Dali::String& value) &;
  /// @copydoc SetTitleFontFamily
  Builder&& SetTitleFontFamily(const Dali::String& value) &&;
  /// @brief Message text color.
  /// @SINCE_3_0.0
  Builder& SetMessageTextColor(const UiColor& value) &;
  /// @copydoc SetMessageTextColor
  Builder&& SetMessageTextColor(const UiColor& value) &&;
  /// @brief Message font size. Must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetMessageFontSize(float value) &;
  /// @copydoc SetMessageFontSize
  Builder&& SetMessageFontSize(float value) &&;
  /// @brief Message font family. An empty string preserves the default Label font.
  /// @SINCE_3_0.0
  Builder& SetMessageFontFamily(const Dali::String& value) &;
  /// @copydoc SetMessageFontFamily
  Builder&& SetMessageFontFamily(const Dali::String& value) &&;
  /// @brief Explicit action appearance. An empty handle uses the Alert fallback and configured TextButton geometry.
  /// @SINCE_3_0.0
  Builder& SetActionButtonStyle(TextButtonStyle value) &;
  /// @copydoc SetActionButtonStyle
  Builder&& SetActionButtonStyle(TextButtonStyle value) &&;
  /// @brief Generated action row height: WRAP_CONTENT, or a finite non-negative fixed height. Invalid values assert.
  /// @SINCE_3_0.0
  Builder& SetActionRowHeight(float value) &;
  /// @copydoc SetActionRowHeight
  Builder&& SetActionRowHeight(float value) &&;
  /// @brief Padding for the generated action row only. All sides must be finite and non-negative; invalid values assert.
  /// @SINCE_3_0.0
  Builder& SetActionRowPadding(const Insets& value) &;
  /// @copydoc SetActionRowPadding
  Builder&& SetActionRowPadding(const Insets& value) &&;
  /// @brief Spacing between actions. Must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetActionButtonSpacing(float value) &;
  /// @copydoc SetActionButtonSpacing
  Builder&& SetActionButtonSpacing(float value) &&;

  /// @brief Consumes this builder and returns an immutable style.
  /// @SINCE_3_0.0
  AlertDialogStyle Build() &&;

private:
  friend class AlertDialogStyle;
  explicit Builder(UiStyle style);
  UiStyle mStyle;
};
} // namespace Ui
} // namespace DALI_NAMESPACE
