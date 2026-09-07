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

  /**
   * @brief Creates an uninitialized ProgressBarStyle handle.
   */
  ProgressBarStyle() = default;

  /**
   * @brief Returns the key used to register the default ProgressBarStyle in UiConfig.
   *
   * @return The default ProgressBarStyle key
   */
  static UiStyleKey<ProgressBarStyle> DefaultKey();

  /**
   * @brief Returns the cached built-in ProgressBarStyle preset.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The built-in default preset
   */
  static ProgressBarStyle DefaultPreset();

  /**
   * @brief Returns the default ProgressBarStyle from the current UiConfig.
   *
   * The style registered for DefaultKey() is returned when present. Otherwise,
   * DefaultPreset() is returned. This function requires UiConfig::Apply().
   *
   * @return The configured default ProgressBarStyle
   */
  static ProgressBarStyle Default();

  /**
   * @brief Downcasts a handle to ProgressBarStyle using a runtime type check.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ProgressBarStyle on success, otherwise an empty handle
   */
  static ProgressBarStyle DownCast(BaseHandle handle);

  /**
   * @brief Statically downcasts a UiStyle to ProgressBarStyle.
   *
   * The caller must ensure that @p style contains a ProgressBarStyle implementation.
   *
   * @param[in] style The style to downcast
   * @return The ProgressBarStyle handle
   */
  static ProgressBarStyle StaticDownCast(UiStyle style);

  /**
   * @brief Creates a mutable builder initialized with a copy of this style.
   *
   * @return A builder that can create a modified immutable style
   */
  Builder Configure() const;

  /**
   * @brief Returns the padding around the track.
   *
   * @return The configured padding
   */
  Insets GetPadding() const;

  /**
   * @brief Returns the track thickness.
   *
   * @return The configured track thickness
   */
  float GetTrackThickness() const;

  /**
   * @brief Returns the track color.
   *
   * @return The configured track color
   */
  UiColor GetTrackColor() const;

  /**
   * @brief Returns the trail color.
   *
   * @return The configured trail color
   */
  UiColor GetTrailColor() const;

  /**
   * @brief Returns the divider color.
   *
   * @return The configured divider color
   */
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
  /**
   * @brief Creates a builder initialized with the built-in ProgressBarStyle values.
   */
  Builder();

  /**
   * @brief Creates a builder by moving another builder.
   *
   * @param[in] rhs The builder to move
   */
  Builder(Builder&& rhs) noexcept;

  /**
   * @brief Move-assigns another builder to this builder.
   *
   * @param[in] rhs The builder to move
   * @return A reference to this builder
   */
  Builder& operator=(Builder&& rhs) noexcept;

  /**
   * @brief Copy construction is disabled because a builder has unique mutable state.
   */
  Builder(const Builder&) = delete;

  /**
   * @brief Copy assignment is disabled because a builder has unique mutable state.
   */
  Builder& operator=(const Builder&) = delete;

  /**
   * @brief Destroys the builder.
   */
  ~Builder();

  /**
   * @brief Sets the padding around the track on an lvalue builder.
   *
   * Every inset must be finite and non-negative.
   *
   * @param[in] padding The padding to set
   * @return A reference to this builder
   */
  Builder& SetPadding(const Insets& padding) &;

  /**
   * @brief Sets the padding around the track on an rvalue builder.
   *
   * Every inset must be finite and non-negative.
   *
   * @param[in] padding The padding to set
   * @return An rvalue reference to this builder
   */
  Builder&& SetPadding(const Insets& padding) &&;

  /**
   * @brief Sets the track thickness on an lvalue builder.
   *
   * @param[in] thickness The finite, positive track thickness
   * @return A reference to this builder
   */
  Builder& SetTrackThickness(float thickness) &;

  /**
   * @brief Sets the track thickness on an rvalue builder.
   *
   * @param[in] thickness The finite, positive track thickness
   * @return An rvalue reference to this builder
   */
  Builder&& SetTrackThickness(float thickness) &&;

  /**
   * @brief Sets the track color on an lvalue builder.
   *
   * @param[in] color The track color
   * @return A reference to this builder
   */
  Builder& SetTrackColor(const UiColor& color) &;

  /**
   * @brief Sets the track color on an rvalue builder.
   *
   * @param[in] color The track color
   * @return An rvalue reference to this builder
   */
  Builder&& SetTrackColor(const UiColor& color) &&;

  /**
   * @brief Sets the trail color on an lvalue builder.
   *
   * @param[in] color The trail color
   * @return A reference to this builder
   */
  Builder& SetTrailColor(const UiColor& color) &;

  /**
   * @brief Sets the trail color on an rvalue builder.
   *
   * @param[in] color The trail color
   * @return An rvalue reference to this builder
   */
  Builder&& SetTrailColor(const UiColor& color) &&;

  /**
   * @brief Sets the divider color on an lvalue builder.
   *
   * @param[in] color The divider color
   * @return A reference to this builder
   */
  Builder& SetDividerColor(const UiColor& color) &;

  /**
   * @brief Sets the divider color on an rvalue builder.
   *
   * @param[in] color The divider color
   * @return An rvalue reference to this builder
   */
  Builder&& SetDividerColor(const UiColor& color) &&;

  /**
   * @brief Consumes this builder and returns an immutable ProgressBarStyle.
   *
   * A builder can be consumed only once.
   *
   * @return The configured ProgressBarStyle
   */
  ProgressBarStyle Build() &&;

private:
  explicit Builder(UiStyle style);
  friend class ProgressBarStyle;

private:
  UiStyle mStyle;
};

} // namespace Ui
} // namespace Dali
