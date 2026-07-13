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
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/image/i-selectable-image.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>

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
  using IconGenerator = Ui::Callback<ISelectableImage()>;

  CheckBoxStyle() = default;

  static UiStyleKey<CheckBoxStyle> DefaultKey();
  static CheckBoxStyle             DefaultPreset();
  static CheckBoxStyle             Default();
  static CheckBoxStyle             DownCast(BaseHandle handle);
  static CheckBoxStyle             StaticDownCast(UiStyle style);

  Builder Configure() const;

  float   GetMinimumWidth() const;
  float   GetMinimumHeight() const;
  Extents GetPadding() const;

  float GetBoxSize() const;
  float GetLabelGap() const;

  ISelectableImage CreateIcon() const;

  UiColor GetIconColor() const;
  UiColor GetSelectedIconColor() const;
  UiColor GetLabelColor() const;

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
  Builder&  SetMinimumSize(const Vector2& size) &;
  Builder&& SetMinimumSize(const Vector2& size) &&;
  Builder&  SetPadding(const Extents& padding) &;
  Builder&& SetPadding(const Extents& padding) &&;

  Builder&  SetBoxSize(float size) &;
  Builder&& SetBoxSize(float size) &&;
  Builder&  SetLabelGap(float gap) &;
  Builder&& SetLabelGap(float gap) &&;

  Builder&  SetIconGenerator(IconGenerator&& generator) &;
  Builder&& SetIconGenerator(IconGenerator&& generator) &&;

  Builder&  SetIconColor(const UiColor& color) &;
  Builder&& SetIconColor(const UiColor& color) &&;
  Builder&  SetSelectedIconColor(const UiColor& color) &;
  Builder&& SetSelectedIconColor(const UiColor& color) &&;
  Builder&  SetLabelColor(const UiColor& color) &;
  Builder&& SetLabelColor(const UiColor& color) &&;

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
