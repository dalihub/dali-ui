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
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <cstdint>
// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class ChartAnnotation;
}

/**
 * @brief A shared, mutable chart annotation anchored in data coordinates.
 *
 * An annotation can be shared by charts. ChartView observes changes while attached.
 */
class DALI_UI_COMPONENTS_API ChartAnnotation : public BaseHandle
{
public:
  /**
   * @brief The immutable content type of an annotation.
   */
  enum class Type : uint32_t
  {
    MARKER = 0u, ///< A filled marker centered at its anchor.
    TEXT   = 1u, ///< A text label near its anchor.
    IMAGE  = 2u  ///< An image using a copied resource URL.
  };
  /**
   * @brief Creates an uninitialized handle; initialize it before accessing properties.
   */
  ChartAnnotation();

  /**
   * @brief Releases this handle without invalidating other shared handles.
   */
  ~ChartAnnotation();

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @param[in] rhs The handle to copy
   */
  ChartAnnotation(const ChartAnnotation& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @param[in] rhs The handle to move
   */
  ChartAnnotation(ChartAnnotation&& rhs) noexcept;

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @param[in] rhs The handle to copy
   * @return This handle
   */
  ChartAnnotation& operator=(const ChartAnnotation& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @param[in] rhs The handle to move
   * @return This handle
   */
  ChartAnnotation& operator=(ChartAnnotation&& rhs) noexcept;

  /**
   * @brief Creates an annotation.
   * @param[in] type The content type
   * @return The annotation
   */
  static ChartAnnotation New(Type type);

  /**
   * @brief Performs a checked cast.
   * @param[in] handle The source handle
   * @return The annotation, or an uninitialized handle
   */
  static ChartAnnotation DownCast(BaseHandle handle);

  /**
   * @brief Sets a data-coordinate anchor, removing any data-point anchor.
   * @param[in] position The finite data coordinate
   */
  void SetPosition(const Vector2& position);

  /**
   * @brief Anchors to a series point using its original data index.
   * @param[in] series The series whose lifetime is shared
   * @param[in] pointIndex The original point index
   */
  void SetDataPoint(ChartSeries series, uint32_t pointIndex);

  /**
   * @brief Copies the annotation text.
   * @param[in] text The text to copy
   */
  void SetText(Dali::StringView text);

  /**
   * @brief Copies the local image resource URL.
   * @param[in] url The resource URL to copy
   */
  void SetImageUrl(Dali::StringView url);

  /**
   * @brief Sets the marker or text color.
   * @param[in] color The marker or text color; theme identifiers are retained
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Sets the positive finite marker or image size in pixels.
   * @param[in] size The positive pixel dimensions
   */
  void SetSize(const Vector2& size);

  /**
   * @brief Sets a pixel offset relative to the anchor.
   * @param[in] offset The finite pixel offset
   */
  void SetOffset(const Vector2& offset);

  /**
   * @brief Returns the immutable content type.
   * @return The content type
   */
  Type GetType() const;

  /**
   * @brief Returns the owned annotation text.
   * @return The text
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the maximum text width; zero retains automatic chart-relative sizing.
   * @param[in] width A finite non-negative width in pixels
   */
  void SetTextMaximumWidth(float width);

  /**
   * @brief Returns the authored text width limit.
   * @return The maximum width, or zero for automatic sizing
   */
  float GetTextMaximumWidth() const;

  /**
   * @brief Sets how text exceeding its available width is rendered.
   * @param[in] mode The clipping or ellipsis mode
   */
  void SetTextOverflowMode(Text::OverflowMode mode);

  /**
   * @brief Returns the text overflow mode.
   * @return The overflow mode
   */
  Text::OverflowMode GetTextOverflowMode() const;

public: // Not intended for application developers
  /**
   * @brief Wraps an existing implementation object.
   *
   * @param[in] impl The implementation whose ownership is shared
   */
  explicit DALI_INTERNAL ChartAnnotation(Integration::ChartAnnotation* impl);
};
} // namespace Ui
} // namespace DALI_NAMESPACE
