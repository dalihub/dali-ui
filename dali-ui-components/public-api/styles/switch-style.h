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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class SwitchStyleImpl;
}

/**
 * @brief Style values used to initialize Switch appearance and interaction.
 *
 * Besides layout and color values, the style owns the drag threshold and
 * press/focus overlay effect. Selection transition timing and easing are
 * intrinsic Switch behavior rather than style values.
 */
class DALI_UI_COMPONENTS_API SwitchStyle : public UiStyle
{
public:
  class Builder;

  SwitchStyle() = default;

  static UiStyleKey<SwitchStyle> DefaultKey();
  static SwitchStyle             DefaultPreset();
  static SwitchStyle             Default();
  static SwitchStyle             DownCast(BaseHandle handle);
  static SwitchStyle             StaticDownCast(UiStyle style);

  Builder Configure() const;

  float  GetMinimumWidth() const;
  float  GetMinimumHeight() const;
  Insets GetPadding() const;

  float  GetTrackWidth() const;
  float  GetTrackHeight() const;
  Insets GetThumbPadding() const;

  UiColor GetTrackColor() const;
  UiColor GetSelectedTrackColor() const;
  UiColor GetThumbColor() const;

  OverlayEffect GetOverlayEffect() const;

  float GetDragThreshold() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL SwitchStyle(Internal::SwitchStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create SwitchStyle handles.
 */
class DALI_UI_COMPONENTS_API SwitchStyle::Builder
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

  Builder&  SetTrackWidth(float width) &;
  Builder&& SetTrackWidth(float width) &&;
  Builder&  SetTrackHeight(float height) &;
  Builder&& SetTrackHeight(float height) &&;
  Builder&  SetTrackSize(float width, float height) &;
  Builder&& SetTrackSize(float width, float height) &&;
  Builder&  SetThumbPadding(const Insets& padding) &;
  Builder&& SetThumbPadding(const Insets& padding) &&;

  Builder&  SetTrackColor(const UiColor& color) &;
  Builder&& SetTrackColor(const UiColor& color) &&;
  Builder&  SetSelectedTrackColor(const UiColor& color) &;
  Builder&& SetSelectedTrackColor(const UiColor& color) &&;
  Builder&  SetTrackColors(const UiColor& color, const UiColor& selectedColor) &;
  Builder&& SetTrackColors(const UiColor& color, const UiColor& selectedColor) &&;
  Builder&  SetThumbColor(const UiColor& color) &;
  Builder&& SetThumbColor(const UiColor& color) &&;

  Builder&  SetOverlayEffect(OverlayEffect effect) &;
  Builder&& SetOverlayEffect(OverlayEffect effect) &&;

  Builder&  SetDragThreshold(float pixels) &;
  Builder&& SetDragThreshold(float pixels) &&;

  SwitchStyle Build() &&;

private:
  explicit Builder(Internal::SwitchStyleImpl* impl);
  friend class SwitchStyle;

private:
  IntrusivePtr<Internal::SwitchStyleImpl> mImpl;
};

} // namespace Ui
} // namespace Dali
