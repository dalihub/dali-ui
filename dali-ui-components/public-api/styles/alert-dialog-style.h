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
 * @note UiConfig::Apply must be called before using Default or DefaultPreset.
 */
class DALI_UI_COMPONENTS_API AlertDialogStyle : public UiStyle
{
public:
  class Builder;
  /// @brief Creates an empty handle.
  AlertDialogStyle() = default;
  /// @brief Gets the default key for registration with Components UiConfig.
  static UiStyleKey<AlertDialogStyle> DefaultKey();
  /// @brief Gets the built-in defaults, independent of the configured provider.
  static AlertDialogStyle DefaultPreset();
  /// @brief Gets the style from the current configuration, falling back to the built-in defaults.
  static AlertDialogStyle Default();
  /// @brief Returns an empty handle if the type does not match.
  static AlertDialogStyle DownCast(BaseHandle handle);
  /// @brief Does not check the type. The caller must ensure that the actual type matches.
  static AlertDialogStyle StaticDownCast(UiStyle style);
  /// @brief Returns a mutable builder independent of the original style.
  Builder Configure() const;

  /// @brief Common appearance. An empty handle uses DialogStyle::Default at creation.
  DialogStyle GetDialogStyle() const;
  /// @brief Title text color.
  UiColor GetTitleTextColor() const;
  /// @brief Title font size. Must be finite and non-negative.
  float GetTitleFontSize() const;
  /// @brief Title font family. An empty string preserves the default Label font.
  Dali::String GetTitleFontFamily() const;
  /// @brief Message text color.
  UiColor GetMessageTextColor() const;
  /// @brief Message font size. Must be finite and non-negative.
  float GetMessageFontSize() const;
  /// @brief Message font family. An empty string preserves the default Label font.
  Dali::String GetMessageFontFamily() const;
  /// @brief Explicit action appearance. An empty handle uses the legacy Alert action defaults.
  TextButtonStyle GetActionButtonStyle() const;
  /// @brief Fixed action row height. Must be finite and non-negative.
  float GetActionRowHeight() const;
  /// @brief Spacing between actions. Must be finite and non-negative.
  float GetActionButtonSpacing() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL AlertDialogStyle(Extension::UiStyleImpl* impl);
  /// @endcond
};

/** @brief A move-only builder that cannot be reused after Build. */
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
  Builder& SetDialogStyle(DialogStyle value) &;
  /// @copydoc SetDialogStyle
  Builder&& SetDialogStyle(DialogStyle value) &&;
  /// @brief Title text color.
  Builder& SetTitleTextColor(const UiColor& value) &;
  /// @copydoc SetTitleTextColor
  Builder&& SetTitleTextColor(const UiColor& value) &&;
  /// @brief Title font size. Must be finite and non-negative. Invalid values trigger an assertion.
  Builder& SetTitleFontSize(float value) &;
  /// @copydoc SetTitleFontSize
  Builder&& SetTitleFontSize(float value) &&;
  /// @brief Title font family. An empty string preserves the default Label font.
  Builder& SetTitleFontFamily(const Dali::String& value) &;
  /// @copydoc SetTitleFontFamily
  Builder&& SetTitleFontFamily(const Dali::String& value) &&;
  /// @brief Message text color.
  Builder& SetMessageTextColor(const UiColor& value) &;
  /// @copydoc SetMessageTextColor
  Builder&& SetMessageTextColor(const UiColor& value) &&;
  /// @brief Message font size. Must be finite and non-negative. Invalid values trigger an assertion.
  Builder& SetMessageFontSize(float value) &;
  /// @copydoc SetMessageFontSize
  Builder&& SetMessageFontSize(float value) &&;
  /// @brief Message font family. An empty string preserves the default Label font.
  Builder& SetMessageFontFamily(const Dali::String& value) &;
  /// @copydoc SetMessageFontFamily
  Builder&& SetMessageFontFamily(const Dali::String& value) &&;
  /// @brief Explicit action appearance. An empty handle uses the legacy Alert action defaults.
  Builder& SetActionButtonStyle(TextButtonStyle value) &;
  /// @copydoc SetActionButtonStyle
  Builder&& SetActionButtonStyle(TextButtonStyle value) &&;
  /// @brief Fixed action row height. Must be finite and non-negative. Invalid values trigger an assertion.
  Builder& SetActionRowHeight(float value) &;
  /// @copydoc SetActionRowHeight
  Builder&& SetActionRowHeight(float value) &&;
  /// @brief Spacing between actions. Must be finite and non-negative. Invalid values trigger an assertion.
  Builder& SetActionButtonSpacing(float value) &;
  /// @copydoc SetActionButtonSpacing
  Builder&& SetActionButtonSpacing(float value) &&;

  /// @brief Consumes this builder and returns an immutable style.
  AlertDialogStyle Build() &&;

private:
  friend class AlertDialogStyle;
  explicit Builder(UiStyle style);
  UiStyle mStyle;
};
} // namespace Ui
} // namespace DALI_NAMESPACE
