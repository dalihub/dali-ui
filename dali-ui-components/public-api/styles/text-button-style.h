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
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-types.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class TextButtonStyleImpl;
}

/**
 * @brief Style values used to initialize TextButton appearance and layout.
 */
class DALI_UI_API TextButtonStyle : public UiStyle
{
public:
  class Builder;

  TextButtonStyle() = default;

  static UiStyleKey<TextButtonStyle> DefaultKey();

  /**
   * @brief Gets the cached built-in default preset.
   *
   * The returned style is created from the builder defaults and reused across
   * calls. This function requires UiConfig::Apply().
   *
   * @return The built-in default preset
   */
  static TextButtonStyle DefaultPreset();

  /**
   * @brief Gets the default style from the current UiConfig.
   *
   * This resolves the style registered for DefaultKey() in the current style
   * sheet. If no override is registered, DefaultPreset() is returned. This
   * function requires UiConfig::Apply().
   *
   * @return The default TextButton style
   */
  static TextButtonStyle Default();
  static TextButtonStyle DownCast(BaseHandle handle);
  static TextButtonStyle StaticDownCast(UiStyle style);

  Builder Configure() const;

  float GetMinimumWidth() const;

  float GetMinimumHeight() const;

  float GetMaximumWidth() const;

  float GetMaximumHeight() const;

  Vector4 GetCornerRadius() const;

  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  Extents GetPadding() const;

  UiColor GetBackgroundColor() const;

  LayoutAlignment GetHorizontalAlignment() const;

  LayoutAlignment GetVerticalAlignment() const;

  UiColor GetTextColor() const;

  float GetFontSize() const;

  Dali::String GetFontFamily() const;

  StateEffect GetStateEffect() const;

  Text::Underline GetTextUnderline() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL TextButtonStyle(Internal::TextButtonStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create TextButtonStyle handles.
 */
class DALI_UI_API TextButtonStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;

  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;

  ~Builder();

  Builder&  SetMinimumWidth(float width) &;
  Builder&& SetMinimumWidth(float width) &&;

  Builder&  SetMinimumHeight(float height) &;
  Builder&& SetMinimumHeight(float height) &&;

  Builder&  SetMaximumWidth(float width) &;
  Builder&& SetMaximumWidth(float width) &&;

  Builder&  SetMaximumHeight(float height) &;
  Builder&& SetMaximumHeight(float height) &&;

  Builder&  SetMinimumSize(const Vector2& size) &;
  Builder&& SetMinimumSize(const Vector2& size) &&;

  Builder&  SetMaximumSize(const Vector2& size) &;
  Builder&& SetMaximumSize(const Vector2& size) &&;

  Builder&  SetCornerRadius(float radius) &;
  Builder&& SetCornerRadius(float radius) &&;

  Builder&  SetCornerRadius(const Vector4& radius) &;
  Builder&& SetCornerRadius(const Vector4& radius) &&;

  Builder&  SetCornerRadiusPolicy(CornerRadiusPolicy policy) &;
  Builder&& SetCornerRadiusPolicy(CornerRadiusPolicy policy) &&;

  Builder&  SetPadding(const Extents& padding) &;
  Builder&& SetPadding(const Extents& padding) &&;

  Builder&  SetBackgroundColor(const UiColor& color) &;
  Builder&& SetBackgroundColor(const UiColor& color) &&;

  Builder&  SetHorizontalAlignment(LayoutAlignment alignment) &;
  Builder&& SetHorizontalAlignment(LayoutAlignment alignment) &&;

  Builder&  SetVerticalAlignment(LayoutAlignment alignment) &;
  Builder&& SetVerticalAlignment(LayoutAlignment alignment) &&;

  Builder&  SetTextColor(const UiColor& color) &;
  Builder&& SetTextColor(const UiColor& color) &&;

  Builder&  SetFontSize(float fontSize) &;
  Builder&& SetFontSize(float fontSize) &&;

  Builder&  SetFontFamily(const Dali::String& fontFamily) &;
  Builder&& SetFontFamily(const Dali::String& fontFamily) &&;

  Builder&  SetStateEffect(StateEffect effect) &;
  Builder&& SetStateEffect(StateEffect effect) &&;

  Builder&  SetTextUnderline(const Text::Underline& underline) &;
  Builder&& SetTextUnderline(const Text::Underline& underline) &&;

  TextButtonStyle Build() &&;

private:
  explicit Builder(Internal::TextButtonStyleImpl* impl);

  friend class TextButtonStyle;

private:
  IntrusivePtr<Internal::TextButtonStyleImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
