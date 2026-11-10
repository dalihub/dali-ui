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

#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali/public-api/math/vector4.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief An immutable presentation policy applied at creation. Does not store content or modal state.
 * @SINCE_3_0.0
 * @note UiConfig::Apply must be called before using Default or DefaultPreset.
 */
class DALI_UI_COMPONENTS_API DialogStyle : public UiStyle
{
public:
  class Builder;
  /// @brief Creates an empty handle.
  /// @SINCE_3_0.0
  DialogStyle() = default;
  /// @brief Gets the default key for registration with Components UiConfig.
  /// @SINCE_3_0.0
  static UiStyleKey<DialogStyle> DefaultKey();
  /// @brief Gets the OneUI fallback surface, independent of the configured provider.
  /// Uses absolute radius 44, translucent white (alpha 0.6), two cutout shadows,
  /// and an inner border. Content defaults are width 908, WRAP_CONTENT height,
  /// own padding (44, 44, 36, 32), and section spacing 12. Child padding is preserved.
  /// Register a DefaultKey provider for theme-specific surface values.
  /// @SINCE_3_0.0
  static DialogStyle DefaultPreset();
  /// @brief Gets the style from the current configuration, falling back to the built-in defaults.
  /// @SINCE_3_0.0
  static DialogStyle Default();
  /// @brief Returns an empty handle if the type does not match.
  /// @SINCE_3_0.0
  static DialogStyle DownCast(BaseHandle handle);
  /// @brief Does not check the type. The caller must ensure that the actual type matches.
  /// @SINCE_3_0.0
  static DialogStyle StaticDownCast(UiStyle style);
  /// @brief Returns a mutable builder independent of the original style.
  /// @SINCE_3_0.0
  Builder Configure() const;

  /// @brief Default requested width: non-negative or WRAP_CONTENT/MATCH_PARENT.
  /// @SINCE_3_0.0
  float GetRequestedWidth() const;
  /// @brief Default requested height: non-negative or WRAP_CONTENT/MATCH_PARENT.
  /// @SINCE_3_0.0
  float GetRequestedHeight() const;
  /// @brief Background color. Preserves UiColor tokens unchanged.
  /// @SINCE_3_0.0
  UiColor GetBackgroundColor() const;
  /// @brief Overall padding. Each component must be finite and non-negative.
  /// @SINCE_3_0.0
  Insets GetPadding() const;
  /// @brief Spacing between sections. Must be finite and non-negative.
  /// @SINCE_3_0.0
  float GetSpacing() const;
  /// @brief Corner radius. Each component must be finite and non-negative.
  /// @SINCE_3_0.0
  Vector4 GetCornerRadius() const;
  /// @brief Absolute or relative interpretation of the corner radius.
  /// @SINCE_3_0.0
  CornerRadiusPolicy GetCornerRadiusPolicy() const;
  /// @brief Shadow list, stored by value.
  /// @SINCE_3_0.0
  ShadowStack GetShadow() const;
  /// @brief Borderline width. Must be finite and non-negative.
  /// @SINCE_3_0.0
  float GetBorderlineWidth() const;
  /// @brief Borderline offset. Must be finite; negative values are allowed.
  /// @SINCE_3_0.0
  float GetBorderlineOffset() const;
  /// @brief Borderline color.
  /// @SINCE_3_0.0
  UiColor GetBorderlineColor() const;
  /// @brief Background blur radius. Must be finite and within [0, 4096]; zero disables blur. The fractional part is discarded when applied.
  /// @SINCE_3_0.0
  float GetBackgroundBlurRadius() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL DialogStyle(Extension::UiStyleImpl* impl);
  /// @endcond
};

/**
 * @brief A move-only builder that cannot be reused after Build.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API DialogStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;
  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;
  ~Builder();

  /// @brief Default requested width: non-negative or WRAP_CONTENT/MATCH_PARENT. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetRequestedWidth(float value) &;
  /// @copydoc SetRequestedWidth
  Builder&& SetRequestedWidth(float value) &&;
  /// @brief Default requested height: non-negative or WRAP_CONTENT/MATCH_PARENT. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetRequestedHeight(float value) &;
  /// @copydoc SetRequestedHeight
  Builder&& SetRequestedHeight(float value) &&;
  /// @brief Background color. Preserves UiColor tokens unchanged.
  /// @SINCE_3_0.0
  Builder& SetBackgroundColor(const UiColor& value) &;
  /// @copydoc SetBackgroundColor
  Builder&& SetBackgroundColor(const UiColor& value) &&;
  /// @brief Overall padding. Each component must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetPadding(const Insets& value) &;
  /// @copydoc SetPadding
  Builder&& SetPadding(const Insets& value) &&;
  /// @brief Spacing between sections. Must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetSpacing(float value) &;
  /// @copydoc SetSpacing
  Builder&& SetSpacing(float value) &&;
  /// @brief Corner radius. Each component must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetCornerRadius(const Vector4& value) &;
  /// @copydoc SetCornerRadius
  Builder&& SetCornerRadius(const Vector4& value) &&;
  /// @brief Absolute or relative interpretation of the corner radius. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetCornerRadiusPolicy(CornerRadiusPolicy value) &;
  /// @copydoc SetCornerRadiusPolicy
  Builder&& SetCornerRadiusPolicy(CornerRadiusPolicy value) &&;
  /// @brief Shadow list, stored by value.
  /// @SINCE_3_0.0
  Builder& SetShadow(const ShadowStack& value) &;
  /// @copydoc SetShadow
  Builder&& SetShadow(const ShadowStack& value) &&;
  /// @brief Borderline width. Must be finite and non-negative. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetBorderlineWidth(float value) &;
  /// @copydoc SetBorderlineWidth
  Builder&& SetBorderlineWidth(float value) &&;
  /// @brief Borderline offset. Must be finite; negative values are allowed. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetBorderlineOffset(float value) &;
  /// @copydoc SetBorderlineOffset
  Builder&& SetBorderlineOffset(float value) &&;
  /// @brief Borderline color.
  /// @SINCE_3_0.0
  Builder& SetBorderlineColor(const UiColor& value) &;
  /// @copydoc SetBorderlineColor
  Builder&& SetBorderlineColor(const UiColor& value) &&;
  /// @brief Background blur radius. Must be finite and within [0, 4096]; zero disables blur. The fractional part is discarded when applied. Invalid values trigger an assertion.
  /// @SINCE_3_0.0
  Builder& SetBackgroundBlurRadius(float value) &;
  /// @copydoc SetBackgroundBlurRadius
  Builder&& SetBackgroundBlurRadius(float value) &&;

  /// @brief Consumes this builder and returns an immutable style.
  /// @SINCE_3_0.0
  DialogStyle Build() &&;

private:
  friend class DialogStyle;
  explicit Builder(UiStyle style);
  UiStyle mStyle;
};
} // namespace Ui
} // namespace DALI_NAMESPACE
