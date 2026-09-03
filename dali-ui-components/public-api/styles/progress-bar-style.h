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

#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Immutable values used to initialize a ProgressBar.
 *
 * Runtime setters on ProgressBar may subsequently change track thickness, track color and
 * trail color. Padding and divider color remain construction-time style values.
 */
class DALI_UI_COMPONENTS_API ProgressBarStyle : public UiStyle
{
public:
  class Builder;

  ProgressBarStyle() = default;

  static UiStyleKey<ProgressBarStyle> DefaultKey();
  static ProgressBarStyle             DefaultPreset();
  static ProgressBarStyle             Default();
  static ProgressBarStyle             DownCast(BaseHandle handle);
  static ProgressBarStyle             StaticDownCast(UiStyle style);

  Builder Configure() const;

  Insets  GetPadding() const;
  float   GetTrackThickness() const;
  UiColor GetTrackColor() const;
  UiColor GetTrailColor() const;
  UiColor GetDividerColor() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL ProgressBarStyle(Extension::UiStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable, move-only builder for ProgressBarStyle.
 */
class DALI_UI_COMPONENTS_API ProgressBarStyle::Builder
{
public:
  Builder();
  Builder(Builder&& rhs) noexcept;
  Builder& operator=(Builder&& rhs) noexcept;
  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;
  ~Builder();

  Builder&  SetPadding(const Insets& padding) &;
  Builder&& SetPadding(const Insets& padding) &&;
  Builder&  SetTrackThickness(float thickness) &;
  Builder&& SetTrackThickness(float thickness) &&;
  Builder&  SetTrackColor(const UiColor& color) &;
  Builder&& SetTrackColor(const UiColor& color) &&;
  Builder&  SetTrailColor(const UiColor& color) &;
  Builder&& SetTrailColor(const UiColor& color) &&;
  Builder&  SetDividerColor(const UiColor& color) &;
  Builder&& SetDividerColor(const UiColor& color) &&;

  ProgressBarStyle Build() &&;

private:
  explicit Builder(UiStyle style);
  friend class ProgressBarStyle;

private:
  UiStyle mStyle;
};

} // namespace Ui
} // namespace Dali
