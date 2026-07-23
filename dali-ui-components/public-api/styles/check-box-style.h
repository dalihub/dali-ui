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
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class CheckBoxStyleImpl;
}

/**
 * @brief Style values used to initialize CheckBox appearance and layout.
 */
class DALI_UI_API CheckBoxStyle : public UiStyle
{
public:
  class Builder;

  /**
   * @brief ABI-safe factory that creates the CheckBox icon (a selectable image).
   *
   * A stateless free function is the expected implementation, so the same generator can be
   * shared across style copies. Move-only (Ui::Callback is move-only).
   */
  using IconGenerator = Ui::Callback<SelectableImageInterface()>;

  CheckBoxStyle() = default;

  static UiStyleKey<CheckBoxStyle> DefaultKey();
  static CheckBoxStyle             DefaultPreset();
  static CheckBoxStyle             Default();
  static CheckBoxStyle             DownCast(BaseHandle handle);
  static CheckBoxStyle             StaticDownCast(UiStyle style);

  Builder Configure() const;

  float  GetMinimumWidth() const;
  float  GetMinimumHeight() const;
  Insets GetPadding() const;

  float GetIconWidth() const;
  float GetIconHeight() const;
  float GetLabelGap() const;

  SelectableImageInterface CreateIcon() const;

  UiColor GetIconColor() const;
  UiColor GetSelectedIconColor() const;
  UiColor GetTextColor() const;

  float           GetFontSize() const;
  Dali::String    GetFontFamily() const;
  Text::Underline GetTextUnderline() const;

  StateEffect GetStateEffect() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL CheckBoxStyle(Internal::CheckBoxStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create CheckBoxStyle handles.
 */
class DALI_UI_API CheckBoxStyle::Builder
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
  Builder&  SetPadding(const Insets& padding) &;
  Builder&& SetPadding(const Insets& padding) &&;

  Builder&  SetIconWidth(float width) &;
  Builder&& SetIconWidth(float width) &&;
  Builder&  SetIconHeight(float height) &;
  Builder&& SetIconHeight(float height) &&;
  Builder&  SetLabelGap(float gap) &;
  Builder&& SetLabelGap(float gap) &&;

  Builder&  SetIconGenerator(IconGenerator&& generator) &;
  Builder&& SetIconGenerator(IconGenerator&& generator) &&;

  Builder&  SetIconColor(const UiColor& color) &;
  Builder&& SetIconColor(const UiColor& color) &&;
  Builder&  SetSelectedIconColor(const UiColor& color) &;
  Builder&& SetSelectedIconColor(const UiColor& color) &&;
  Builder&  SetTextColor(const UiColor& color) &;
  Builder&& SetTextColor(const UiColor& color) &&;

  Builder&  SetFontSize(float fontSize) &;
  Builder&& SetFontSize(float fontSize) &&;
  Builder&  SetFontFamily(const Dali::String& fontFamily) &;
  Builder&& SetFontFamily(const Dali::String& fontFamily) &&;
  Builder&  SetTextUnderline(const Text::Underline& underline) &;
  Builder&& SetTextUnderline(const Text::Underline& underline) &&;

  Builder&  SetStateEffect(StateEffect effect) &;
  Builder&& SetStateEffect(StateEffect effect) &&;

  CheckBoxStyle Build() &&;

private:
  explicit Builder(Internal::CheckBoxStyleImpl* impl);
  friend class CheckBoxStyle;

private:
  IntrusivePtr<Internal::CheckBoxStyleImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
