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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ChartSection;
}

/**
 * @brief Defines a highlighted rectangular region on the chart in data coordinates.
 *
 * NaN boundary values (the default for all four bounds) extend the section
 * to the corresponding plot area edge. xMin == xMax draws a vertical
 * threshold line; yMin == yMax draws a horizontal one. Add a section to a
 * chart with ChartView::AddSection().
 *
 * @code
 *   ChartSection band = ChartSection::New();
 *   band.SetMinimumY(70.0f);
 *   band.SetMaximumY(100.0f);
 *   band.SetFillColor(Vector4(1.0f, 0.0f, 0.0f, 0.15f));
 *   chart.AddSection(band);
 * @endcode
 */
class DALI_UI_COMPONENTS_API ChartSection : public BaseHandle
{
public:
  /**
   * @brief Creates a new ChartSection.
   *
   * @return A handle to the new section
   */
  static ChartSection New();

  /**
   * @brief Creates an uninitialized ChartSection handle.
   */
  ChartSection();

  /**
   * @brief Destructor.
   */
  ~ChartSection();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  ChartSection(const ChartSection&) = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  ChartSection& operator=(const ChartSection&) = default;

  /**
   * @brief Sets the minimum X bound of the section, in data coordinates.
   *
   * NaN (the default) extends the section to the left edge of the plot area.
   *
   * @param[in] value The minimum X bound, or NaN to extend to the plot area edge
   */
  void SetMinimumX(float value);

  /**
   * @brief Retrieves the minimum X bound of the section.
   *
   * @return The minimum X bound, in data coordinates, or NaN if extended to the plot area edge
   */
  float GetMinimumX() const;

  /**
   * @brief Sets the maximum X bound of the section, in data coordinates.
   *
   * NaN (the default) extends the section to the right edge of the plot area.
   * Setting this equal to the minimum X bound draws a vertical threshold
   * line instead of a filled band.
   *
   * @param[in] value The maximum X bound, or NaN to extend to the plot area edge
   */
  void SetMaximumX(float value);

  /**
   * @brief Retrieves the maximum X bound of the section.
   *
   * @return The maximum X bound, in data coordinates, or NaN if extended to the plot area edge
   */
  float GetMaximumX() const;

  /**
   * @brief Sets the minimum Y bound of the section, in data coordinates.
   *
   * NaN (the default) extends the section to the bottom edge of the plot area.
   *
   * @param[in] value The minimum Y bound, or NaN to extend to the plot area edge
   */
  void SetMinimumY(float value);

  /**
   * @brief Retrieves the minimum Y bound of the section.
   *
   * @return The minimum Y bound, in data coordinates, or NaN if extended to the plot area edge
   */
  float GetMinimumY() const;

  /**
   * @brief Sets the maximum Y bound of the section, in data coordinates.
   *
   * NaN (the default) extends the section to the top edge of the plot area.
   * Setting this equal to the minimum Y bound draws a horizontal threshold
   * line instead of a filled band.
   *
   * @param[in] value The maximum Y bound, or NaN to extend to the plot area edge
   */
  void SetMaximumY(float value);

  /**
   * @brief Retrieves the maximum Y bound of the section.
   *
   * @return The maximum Y bound, in data coordinates, or NaN if extended to the plot area edge
   */
  float GetMaximumY() const;

  /**
   * @brief Sets the fill color of the section.
   *
   * The default is (0.3, 0.6, 1.0, 0.15).
   *
   * @param[in] color The fill color
   */
  void SetFillColor(const Vector4& color);

  /**
   * @brief Retrieves the fill color of the section.
   *
   * @return The fill color
   */
  Vector4 GetFillColor() const;

  /**
   * @brief Sets the stroke color of the section's border.
   *
   * The default is (0.3, 0.6, 1.0, 0.8).
   *
   * @param[in] color The stroke color
   */
  void SetStrokeColor(const Vector4& color);

  /**
   * @brief Retrieves the stroke color of the section's border.
   *
   * @return The stroke color
   */
  Vector4 GetStrokeColor() const;

  /**
   * @brief Sets the width of the section's border, in pixels.
   *
   * A width of 0 (the default) draws no border.
   *
   * @param[in] width The stroke width, in pixels
   */
  void SetStrokeWidth(float width);

  /**
   * @brief Retrieves the width of the section's border, in pixels.
   *
   * @return The stroke width, in pixels
   */
  float GetStrokeWidth() const;

public:
  explicit ChartSection(Integration::ChartSection* impl);
};

} // namespace Ui
} // namespace Dali
