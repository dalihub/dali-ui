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
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @brief Immutable construction defaults for ChartView. Runtime properties remain independently mutable.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API ChartViewStyle : public UiStyle
{
public:
  class Builder;
  /**
   * @brief Creates an uninitialized handle; initialize it before accessing properties.
   * @SINCE_3_0.0
   */
  ChartViewStyle();

  /**
   * @brief Releases this handle without invalidating other shared handles.
   * @SINCE_3_0.0
   */
  ~ChartViewStyle();

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   */
  ChartViewStyle(const ChartViewStyle& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   */
  ChartViewStyle(ChartViewStyle&& rhs) noexcept;

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   * @return This handle
   */
  ChartViewStyle& operator=(const ChartViewStyle& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   * @return This handle
   */
  ChartViewStyle& operator=(ChartViewStyle&& rhs) noexcept;

  /**
   * @brief Returns the configuration key for the default style.
   * @SINCE_3_0.0
   * @return The typed key
   */
  static UiStyleKey<ChartViewStyle> DefaultKey();

  /**
   * @brief Returns the cached built-in style after UiConfig::Apply().
   * @SINCE_3_0.0
   * @return The built-in style
   */
  static ChartViewStyle DefaultPreset();

  /**
   * @brief Resolves the configured style, falling back to DefaultPreset().
   * @SINCE_3_0.0
   * @return The resolved style
   */
  static ChartViewStyle Default();

  /**
   * @brief Performs a checked cast.
   * @SINCE_3_0.0
   * @param[in] handle The source handle
   * @return The style, or an uninitialized handle
   */
  static ChartViewStyle DownCast(BaseHandle handle);

  /**
   * @brief Casts a style whose concrete type is known to be ChartViewStyle.
   * @SINCE_3_0.0
   * @param[in] style The source style
   * @return The typed style
   */
  static ChartViewStyle StaticDownCast(UiStyle style);

  /**
   * @brief Clones this style into an independent builder.
   * @SINCE_3_0.0
   * @return The builder
   */
  Builder Configure() const;

  /**
   * @brief Returns the authored background color, including any theme identifier.
   * @SINCE_3_0.0
   * @return The authored value
   */
  UiColor GetBackgroundColor() const;

  /**
   * @brief Returns the authored label color, including any theme identifier.
   * @SINCE_3_0.0
   * @return The authored value
   */
  UiColor GetLabelColor() const;

  /**
   * @brief Returns the authored legend text color, including any theme identifier.
   * @SINCE_3_0.0
   * @return The authored value
   */
  UiColor GetLegendTextColor() const;

  /**
   * @brief Returns the authored tooltip background color, including any theme identifier.
   * @SINCE_3_0.0
   * @return The authored value
   */
  UiColor GetTooltipBackgroundColor() const;

  /**
   * @brief Returns the authored tooltip text color, including any theme identifier.
   * @SINCE_3_0.0
   * @return The authored value
   */
  UiColor GetTooltipTextColor() const;

  /**
   * @brief Returns the Padding construction default.
   * @SINCE_3_0.0
   * @return The authored value
   */
  float GetPadding() const;

  /**
   * @brief Returns the TooltipPadding construction default.
   * @SINCE_3_0.0
   * @return The authored value
   */
  float GetTooltipPadding() const;

public: // Not intended for application developers
  /**
   * @brief Wraps an existing implementation object.
   *
   * @SINCE_3_0.0
   * @param[in] impl The implementation whose ownership is shared
   */
  explicit DALI_INTERNAL ChartViewStyle(Extension::UiStyleImpl* impl);
};

/**
 * @brief Move-only construction helper. Build consumes the builder.
 *
 * Move-construct or move-assign to transfer ownership of the mutable configuration.
 * Setters and Build() require a builder that has not been moved from or consumed.
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API ChartViewStyle::Builder
{
public:
  /**
   * @brief Creates a builder with complete built-in construction defaults.
   * @SINCE_3_0.0
   */
  Builder();

  /**
   * @brief Releases the builder and any unbuilt configuration.
   * @SINCE_3_0.0
   */
  ~Builder();

  /**
   * @brief Disables copying; use Configure() to clone a built style.
   * @SINCE_3_0.0
   * @param[in] rhs The builder that cannot be copied
   */
  Builder(const Builder& rhs) = delete;

  /**
   * @brief Disables copy assignment.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The builder that cannot be copied
   * @return This builder; the operation is deleted
   */
  Builder& operator=(const Builder& rhs) = delete;

  /**
   * @brief Transfers the mutable configuration.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The builder to move
   */
  Builder(Builder&& rhs) noexcept;

  /**
   * @brief Transfers the mutable configuration.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The builder to move
   * @return This builder
   */
  Builder& operator=(Builder&& rhs) noexcept;

  /**
   * @brief Sets the background color used when constructing a chart.
   * @SINCE_3_0.0
   * @param[in] color The authored color, retaining any theme color identifier
   * @return This builder
   */
  Builder& SetBackgroundColor(const UiColor& color) &;

  /**
   * @brief Sets the BackgroundColor construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] color The color, retaining any theme color identifier
   * @return This builder
   */
  Builder&& SetBackgroundColor(const UiColor& color) &&;

  /**
   * @brief Sets the label color used when constructing a chart.
   * @SINCE_3_0.0
   * @param[in] color The authored color, retaining any theme color identifier
   * @return This builder
   */
  Builder& SetLabelColor(const UiColor& color) &;

  /**
   * @brief Sets the LabelColor construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] color The color, retaining any theme color identifier
   * @return This builder
   */
  Builder&& SetLabelColor(const UiColor& color) &&;

  /**
   * @brief Sets the legend text color used when constructing a chart.
   * @SINCE_3_0.0
   * @param[in] color The authored color, retaining any theme color identifier
   * @return This builder
   */
  Builder& SetLegendTextColor(const UiColor& color) &;

  /**
   * @brief Sets the LegendTextColor construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] color The color, retaining any theme color identifier
   * @return This builder
   */
  Builder&& SetLegendTextColor(const UiColor& color) &&;

  /**
   * @brief Sets the tooltip background color used when constructing a chart.
   * @SINCE_3_0.0
   * @param[in] color The authored color, retaining any theme color identifier
   * @return This builder
   */
  Builder& SetTooltipBackgroundColor(const UiColor& color) &;

  /**
   * @brief Sets the TooltipBackgroundColor construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] color The color, retaining any theme color identifier
   * @return This builder
   */
  Builder&& SetTooltipBackgroundColor(const UiColor& color) &&;

  /**
   * @brief Sets the tooltip text color used when constructing a chart.
   * @SINCE_3_0.0
   * @param[in] color The authored color, retaining any theme color identifier
   * @return This builder
   */
  Builder& SetTooltipTextColor(const UiColor& color) &;

  /**
   * @brief Sets the TooltipTextColor construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] color The color, retaining any theme color identifier
   * @return This builder
   */
  Builder&& SetTooltipTextColor(const UiColor& color) &&;

  /**
   * @brief Sets the Padding construction default.
   * @SINCE_3_0.0
   * @param[in] value The finite non-negative chart padding in pixels
   * @return This builder
   */
  Builder& SetPadding(float value) &;

  /**
   * @brief Sets the Padding construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] value The authored value
   * @return This builder
   */
  Builder&& SetPadding(float value) &&;

  /**
   * @brief Sets the TooltipPadding construction default.
   * @SINCE_3_0.0
   * @param[in] value The finite non-negative tooltip padding in pixels
   * @return This builder
   */
  Builder& SetTooltipPadding(float value) &;

  /**
   * @brief Sets the TooltipPadding construction default on a temporary builder.
   * @SINCE_3_0.0
   * @param[in] value The authored value
   * @return This builder
   */
  Builder&& SetTooltipPadding(float value) &&;

  /**
   * @brief Consumes the builder and creates an immutable style.
   * @SINCE_3_0.0
   * @return The style
   */
  ChartViewStyle Build() &&;

private:
  /**
   * @brief Owns an independent mutable clone of a style.
   *
   * @SINCE_3_0.0
   * @param[in] style The configuration to consume
   */
  explicit Builder(UiStyle style);
  friend class ChartViewStyle;

  struct Impl;
  UniquePtr<Impl> mImpl; ///< Owns the mutable construction state hidden in the implementation.
};
} // namespace Ui
} // namespace DALI_NAMESPACE
