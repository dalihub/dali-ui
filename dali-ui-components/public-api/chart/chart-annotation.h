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
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API ChartAnnotation : public BaseHandle
{
public:
  /**
   * @brief The immutable content type of an annotation.
   * @SINCE_3_0.0
   */
  enum class Type : uint32_t
  {
    MARKER = 0u, ///< A filled marker centered at its anchor. @SINCE_3_0.0
    TEXT   = 1u, ///< A text label near its anchor. @SINCE_3_0.0
    IMAGE  = 2u  ///< An image using a copied resource URL. @SINCE_3_0.0
  };
  /**
   * @brief Creates an uninitialized handle; initialize it before accessing properties.
   * @SINCE_3_0.0
   */
  ChartAnnotation();

  /**
   * @brief Releases this handle without invalidating other shared handles.
   * @SINCE_3_0.0
   */
  ~ChartAnnotation();

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   */
  ChartAnnotation(const ChartAnnotation& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   */
  ChartAnnotation(ChartAnnotation&& rhs) noexcept;

  /**
   * @brief Shares the object referenced by another handle.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   * @return This handle
   */
  ChartAnnotation& operator=(const ChartAnnotation& rhs);

  /**
   * @brief Transfers a handle, leaving the source uninitialized.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   * @return This handle
   */
  ChartAnnotation& operator=(ChartAnnotation&& rhs) noexcept;

  /**
   * @brief Creates an annotation.
   * @SINCE_3_0.0
   * @param[in] type The content type
   * @return The annotation
   */
  static ChartAnnotation New(Type type);

  /**
   * @brief Performs a checked cast.
   * @SINCE_3_0.0
   * @param[in] handle The source handle
   * @return The annotation, or an uninitialized handle
   */
  static ChartAnnotation DownCast(BaseHandle handle);

  /**
   * @brief Sets a data-coordinate anchor, removing any data-point anchor.
   * @SINCE_3_0.0
   * @param[in] position The finite data coordinate
   */
  void SetPosition(const Vector2& position);

  /**
   * @brief Anchors to a series point using its original data index.
   * @SINCE_3_0.0
   * @param[in] series The series whose lifetime is shared
   * @param[in] pointIndex The original point index
   */
  void SetDataPoint(ChartSeries series, uint32_t pointIndex);

  /**
   * @brief Copies the annotation text.
   * @SINCE_3_0.0
   * @param[in] text The text to copy
   */
  void SetText(Dali::StringView text);

  /**
   * @brief Copies the local image resource URL.
   * @SINCE_3_0.0
   * @param[in] url The resource URL to copy
   */
  void SetImageUrl(Dali::StringView url);

  /**
   * @brief Sets the marker or text color.
   * @SINCE_3_0.0
   * @param[in] color The marker or text color; theme identifiers are retained
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Sets the positive finite marker or image size in pixels.
   * @SINCE_3_0.0
   * @param[in] size The positive pixel dimensions
   */
  void SetSize(const Vector2& size);

  /**
   * @brief Sets a pixel offset relative to the anchor.
   * @SINCE_3_0.0
   * @param[in] offset The finite pixel offset
   */
  void SetOffset(const Vector2& offset);

  /**
   * @brief Returns the immutable content type.
   * @SINCE_3_0.0
   * @return The content type
   */
  Type GetType() const;

  /**
   * @brief Returns the owned annotation text.
   * @SINCE_3_0.0
   * @return The text
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the maximum text width; zero retains automatic chart-relative sizing.
   * @SINCE_3_0.0
   * @param[in] width A finite non-negative width in pixels
   */
  void SetTextMaximumWidth(float width);

  /**
   * @brief Returns the authored text width limit.
   * @SINCE_3_0.0
   * @return The maximum width, or zero for automatic sizing
   */
  float GetTextMaximumWidth() const;

  /**
   * @brief Sets how text exceeding its available width is rendered.
   * @SINCE_3_0.0
   * @param[in] mode The clipping or ellipsis mode
   */
  void SetTextOverflowMode(Text::OverflowMode mode);

  /**
   * @brief Returns the text overflow mode.
   * @SINCE_3_0.0
   * @return The overflow mode
   */
  Text::OverflowMode GetTextOverflowMode() const;

public: // Not intended for application developers
  /**
   * @brief Wraps an existing implementation object.
   *
   * @SINCE_3_0.0
   * @param[in] impl The implementation whose ownership is shared
   */
  explicit DALI_INTERNAL ChartAnnotation(Integration::ChartAnnotation* impl);
};
} // namespace Ui
} // namespace DALI_NAMESPACE
