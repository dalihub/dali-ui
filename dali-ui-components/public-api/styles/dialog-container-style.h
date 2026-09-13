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

#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief An immutable presentation policy applied at creation. Does not store content or modal state.
 * @note UiConfig::Apply must be called before using Default or DefaultPreset.
 */
class DALI_UI_COMPONENTS_API DialogContainerStyle : public UiStyle
{
public:
  class Builder;
  /// @brief Creates an empty handle.
  DialogContainerStyle() = default;
  /// @brief Gets the default key for registration with Components UiConfig.
  static UiStyleKey<DialogContainerStyle> DefaultKey();
  /// @brief Gets the built-in defaults, independent of the configured provider.
  static DialogContainerStyle DefaultPreset();
  /// @brief Gets the style from the current configuration, falling back to the built-in defaults.
  static DialogContainerStyle Default();
  /// @brief Returns an empty handle if the type does not match.
  static DialogContainerStyle DownCast(BaseHandle handle);
  /// @brief Does not check the type. The caller must ensure that the actual type matches.
  static DialogContainerStyle StaticDownCast(UiStyle style);
  /// @brief Returns a mutable builder independent of the original style.
  Builder Configure() const;

  /// @brief Built-in scrim color. Not applied to a custom scrim.
  UiColor GetScrimColor() const;
  /// @brief Built-in scrim blur radius. Must be finite and within [0, 4096]; zero disables blur. The fractional part is discarded when applied.
  float GetScrimBlurRadius() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL DialogContainerStyle(Extension::UiStyleImpl* impl);
  /// @endcond
};

/** @brief A move-only builder that cannot be reused after Build. */
class DALI_UI_COMPONENTS_API DialogContainerStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;
  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;
  ~Builder();

  /// @brief Built-in scrim color. Not applied to a custom scrim.
  Builder& SetScrimColor(const UiColor& value) &;
  /// @copydoc SetScrimColor
  Builder&& SetScrimColor(const UiColor& value) &&;
  /// @brief Built-in scrim blur radius. Must be finite and within [0, 4096]; zero disables blur. The fractional part is discarded when applied. Invalid values trigger an assertion.
  Builder& SetScrimBlurRadius(float value) &;
  /// @copydoc SetScrimBlurRadius
  Builder&& SetScrimBlurRadius(float value) &&;

  /// @brief Consumes this builder and returns an immutable style.
  DialogContainerStyle Build() &&;

private:
  friend class DialogContainerStyle;
  explicit Builder(UiStyle style);
  UiStyle mStyle;
};
} // namespace Ui
} // namespace DALI_NAMESPACE
