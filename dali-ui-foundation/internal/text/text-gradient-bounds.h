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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_BOUNDS_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_BOUNDS_H

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/line-run.h>
#include <dali-ui-foundation/internal/text/text-definitions.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{

/**
 * @brief Returns the selected gradient bounds size in target-space pixels.
 *
 * @param[in] bounds Normalized gradient bounds in the target coordinate space.
 * @param[in] targetSize The target coordinate space size.
 * @return Pixel size of the selected bounds, clamped to a positive value.
 */
inline Vector2 GetGradientBoundsPixelSize(const Vector4& bounds, const Vector2& targetSize)
{
  return Vector2(std::max(bounds.z * targetSize.x, Math::MACHINE_EPSILON_1000),
                 std::max(bounds.w * targetSize.y, Math::MACHINE_EPSILON_1000));
}

/**
 * @brief Converts a gradient control point to normalized selected-bounds coordinates.
 *
 * OBJECT_BOUNDING_BOX positions are centered around (0, 0), so they are shifted
 * to the selected bounds. USER_SPACE positions are converted from target-space
 * pixels to the selected bounds.
 *
 * @param[in] units The coordinate units used by the gradient.
 * @param[in] position The source gradient position.
 * @param[in] bounds Normalized gradient bounds in the target coordinate space.
 * @param[in] targetSize The target coordinate space size.
 * @return The position in selected-bounds coordinates.
 */
inline Vector2 ResolveGradientPosition(Dali::Ui::Gradient::Units units,
                                       const Vector2&            position,
                                       const Vector4&            bounds,
                                       const Vector2&            targetSize)
{
  if(units == Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX)
  {
    return position + Vector2(0.5f, 0.5f);
  }

  if(units != Dali::Ui::Gradient::Units::USER_SPACE)
  {
    return position;
  }

  const Vector2 boundsPixelSize = GetGradientBoundsPixelSize(bounds, targetSize);
  return Vector2(position.x / boundsPixelSize.x, position.y / boundsPixelSize.y);
}

/**
 * @brief Calculates the scale used when evaluating radial distance.
 *
 * @param[in] units The coordinate units used by the gradient.
 * @param[in] radius The radial gradient radius.
 * @param[in] bounds Normalized gradient bounds in the target coordinate space.
 * @param[in] targetSize The target coordinate space size.
 * @return Scale applied to the center-to-fragment vector before length().
 */
inline Vector2 ResolveRadialGradientScale(Dali::Ui::Gradient::Units units,
                                          float                     radius,
                                          const Vector4&            bounds,
                                          const Vector2&            targetSize)
{
  // OBJECT_BOUNDING_BOX uses normalized selected-bounds coordinates, so
  // non-square bounds may look elliptical. USER_SPACE converts that coordinate
  // to pixel-like distance so the radius is evaluated in selected-bounds pixels.
  const float safeRadius = std::max(std::fabs(radius), Math::MACHINE_EPSILON_1000);
  if(units == Dali::Ui::Gradient::Units::USER_SPACE)
  {
    return GetGradientBoundsPixelSize(bounds, targetSize) / safeRadius;
  }

  return Vector2(1.0f / safeRadius, 1.0f / safeRadius);
}

/**
 * @brief Calculates the scale used when evaluating a conic angle.
 *
 * @param[in] units The coordinate units used by the gradient.
 * @param[in] bounds Normalized gradient bounds in the target coordinate space.
 * @param[in] targetSize The target coordinate space size.
 * @return Scale applied to the center-to-fragment vector before atan().
 */
inline Vector2 ResolveConicGradientScale(Dali::Ui::Gradient::Units units,
                                         const Vector4&            bounds,
                                         const Vector2&            targetSize)
{
  // OBJECT_BOUNDING_BOX evaluates the angle in normalized selected-bounds
  // coordinates. USER_SPACE converts the vector to selected-bounds pixels
  // before atan().
  if(units == Dali::Ui::Gradient::Units::USER_SPACE)
  {
    return GetGradientBoundsPixelSize(bounds, targetSize);
  }

  return Vector2::ONE;
}

/**
 * @brief Calculates VIEW_BOUND gradient bounds in a target coordinate space.
 *
 * @param[in] coordinateSize The coordinate space size used by the target shader.
 * @param[in] viewSize The Label view size.
 * @param[in] visualOffset The visual offset inside the coordinate space.
 * @return Normalized view bounds, or full bounds for invalid input.
 */
inline Vector4 CalculateGradientViewBounds(const Vector2& coordinateSize,
                                           const Vector2& viewSize,
                                           const Vector2& visualOffset)
{
  if(coordinateSize.width < Math::MACHINE_EPSILON_1000 ||
     coordinateSize.height < Math::MACHINE_EPSILON_1000 ||
     viewSize.width < Math::MACHINE_EPSILON_1000 ||
     viewSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  return Vector4(-visualOffset.x / coordinateSize.width,
                 -visualOffset.y / coordinateSize.height,
                 viewSize.width / coordinateSize.width,
                 viewSize.height / coordinateSize.height);
}

struct GradientLineExtents
{
  bool  hasHorizontalContent{false};
  float minX{0.0f};
  float maxX{0.0f};
  float height{0.0f};
};

/**
 * @brief Calculates shared horizontal line extents and vertical line-height sum.
 *
 * Zero-width lines do not affect the horizontal union, but every line still
 * contributes its layout height (including empty lines created by newlines).
 */
inline GradientLineExtents CalculateGradientLineExtents(const LineRun* lines,
                                                        Length         numberOfLines,
                                                        float          horizontalOffset = 0.0f)
{
  GradientLineExtents extents;
  if(lines == nullptr)
  {
    return extents;
  }

  for(Length index = 0u; index < numberOfLines; ++index)
  {
    const float lineWidth = std::max(lines[index].width, 0.0f);
    if(lineWidth > Math::MACHINE_EPSILON_1000)
    {
      const float lineStart        = lines[index].alignmentOffset - horizontalOffset;
      const float lineEnd          = lineStart + lineWidth;
      extents.minX                 = extents.hasHorizontalContent ? std::min(extents.minX, lineStart) : lineStart;
      extents.maxX                 = extents.hasHorizontalContent ? std::max(extents.maxX, lineEnd) : lineEnd;
      extents.hasHorizontalContent = true;
    }
    extents.height += std::max(GetLineHeight(lines[index], index + 1u == numberOfLines), 0.0f);
  }
  return extents;
}

/**
 * @brief Calculates CONTENT_BOUND gradient bounds inside a rendered text texture.
 *
 * This helper intentionally uses layout line extents, not glyph ink-tight bounds.
 * The returned vector is (x, y, width, height) in texture UV coordinates.
 *
 * @param[in] textureSize The rendered texture size.
 * @param[in] layoutSize The logical text layout size.
 * @param[in] lines Optional laid-out line runs.
 * @param[in] numberOfLines The number of line runs.
 * @param[in] verticalAlignment The block vertical alignment used by Typesetter.
 * @param[in] ignoreLineAlignmentOffset Whether LineRun::alignmentOffset values are ignored.
 * @return Normalized content bounds, or full texture bounds for invalid input.
 */
inline Vector4 CalculateGradientContentBounds(const Vector2& textureSize,
                                              const Vector2& layoutSize,
                                              const LineRun* lines,
                                              Length         numberOfLines,
                                              Alignment      verticalAlignment,
                                              bool           ignoreLineAlignmentOffset = false)
{
  if(textureSize.width < Math::MACHINE_EPSILON_1000 ||
     textureSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  float boundsX = 0.0f;
  // Preserve the texture-based Label adapter's historical fallback. Atlas has
  // its own epsilon-safe fallback below because it has no gradient texture.
  float boundsWidth = std::min(std::max(layoutSize.width, 0.0f), textureSize.width);

  if(!ignoreLineAlignmentOffset && lines != nullptr && numberOfLines > 0u)
  {
    const GradientLineExtents extents = CalculateGradientLineExtents(lines, numberOfLines);
    float                     minX    = extents.minX;
    float                     maxX    = extents.maxX;

    minX = std::min(std::max(minX, 0.0f), textureSize.width);
    maxX = std::min(std::max(maxX, minX), textureSize.width);

    if(extents.hasHorizontalContent && (maxX - minX) > Math::MACHINE_EPSILON_1000)
    {
      boundsX     = minX;
      boundsWidth = maxX - minX;
    }
  }

  boundsWidth = std::min(std::max(boundsWidth, Math::MACHINE_EPSILON_1000), textureSize.width);

  const float boundsHeight = std::min(std::max(layoutSize.height, Math::MACHINE_EPSILON_1000), textureSize.height);

  float boundsY = 0.0f;
  switch(verticalAlignment)
  {
    case Alignment::CENTER:
    {
      boundsY = std::round(0.5f * (textureSize.height - boundsHeight));
      break;
    }
    case Alignment::END:
    {
      boundsY = textureSize.height - boundsHeight;
      break;
    }
    case Alignment::START:
    default:
    {
      break;
    }
  }

  boundsY = std::min(std::max(boundsY, 0.0f), textureSize.height - boundsHeight);

  return Vector4(boundsX / textureSize.width,
                 boundsY / textureSize.height,
                 boundsWidth / textureSize.width,
                 boundsHeight / textureSize.height);
}

/**
 * @brief Adapts the shared laid-out content-extents policy to atlas local coordinates.
 *
 * Atlas vertices remove @p minLineOffset and use layoutSize as their coordinate
 * size. Unlike the texture adapter above, line bounds are not clipped: RTL/bidi
 * fragments are allowed to extend outside the nominal layout.
 */
inline Vector4 CalculateAtlasGradientContentBounds(const Vector2& layoutSize,
                                                   const LineRun* lines,
                                                   Length         numberOfLines,
                                                   float          minLineOffset)
{
  if(layoutSize.width < Math::MACHINE_EPSILON_1000 ||
     layoutSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  float minX          = 0.0f;
  float maxX          = Math::MACHINE_EPSILON_1000;
  float contentHeight = std::max(layoutSize.height, Math::MACHINE_EPSILON_1000);

  if(lines != nullptr && numberOfLines > 0u)
  {
    const GradientLineExtents extents = CalculateGradientLineExtents(lines, numberOfLines, minLineOffset);
    if(extents.hasHorizontalContent)
    {
      minX = extents.minX;
      maxX = std::max(extents.maxX, extents.minX + Math::MACHINE_EPSILON_1000);
    }
    contentHeight = std::max(extents.height, Math::MACHINE_EPSILON_1000);
  }

  return Vector4(minX / layoutSize.width,
                 0.0f,
                 (maxX - minX) / layoutSize.width,
                 contentHeight / layoutSize.height);
}

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_BOUNDS_H
