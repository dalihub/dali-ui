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

inline Vector2 GetTextGradientBoundsPixelSize(const Vector4& bounds, const Vector2& targetSize)
{
  return Vector2(std::max(bounds.z * targetSize.x, Math::MACHINE_EPSILON_1000),
                 std::max(bounds.w * targetSize.y, Math::MACHINE_EPSILON_1000));
}

inline Vector2 ResolveTextGradientPosition(Dali::Ui::Gradient::Units units,
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

  const Vector2 boundsPixelSize = GetTextGradientBoundsPixelSize(bounds, targetSize);
  return Vector2(position.x / boundsPixelSize.x, position.y / boundsPixelSize.y);
}

inline Vector2 ResolveTextGradientRadialScale(Dali::Ui::Gradient::Units units,
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
    return GetTextGradientBoundsPixelSize(bounds, targetSize) / safeRadius;
  }

  return Vector2(1.0f / safeRadius, 1.0f / safeRadius);
}

inline Vector2 ResolveTextGradientConicScale(Dali::Ui::Gradient::Units units,
                                             const Vector4&            bounds,
                                             const Vector2&            targetSize)
{
  // OBJECT_BOUNDING_BOX evaluates the angle in normalized selected-bounds
  // coordinates. USER_SPACE converts the vector to selected-bounds pixels
  // before atan().
  if(units == Dali::Ui::Gradient::Units::USER_SPACE)
  {
    return GetTextGradientBoundsPixelSize(bounds, targetSize);
  }

  return Vector2::ONE;
}

inline Vector4 CalculateTextGradientViewBounds(const Vector2& textureSize,
                                               const Vector2& viewSize,
                                               const Vector2& visualOffset)
{
  if(textureSize.width < Math::MACHINE_EPSILON_1000 ||
     textureSize.height < Math::MACHINE_EPSILON_1000 ||
     viewSize.width < Math::MACHINE_EPSILON_1000 ||
     viewSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  return Vector4(-visualOffset.x / textureSize.width,
                 -visualOffset.y / textureSize.height,
                 viewSize.width / textureSize.width,
                 viewSize.height / textureSize.height);
}

/**
 * @brief Calculates normalized logical text bounds inside a rendered text texture.
 *
 * This helper intentionally uses layout line extents, not glyph ink-tight bounds.
 * The returned vector is (x, y, width, height) in texture UV coordinates.
 *
 * @param[in] textureSize The rendered texture size.
 * @param[in] layoutSize The logical text layout size.
 * @param[in] lines Optional laid-out line runs.
 * @param[in] numberOfLines The number of line runs.
 * @param[in] verticalAlignment The block vertical alignment used by Typesetter.
 * @return Normalized logical text bounds, or full texture bounds for invalid input.
 */
inline Vector4 CalculateTextGradientBounds(const Vector2& textureSize,
                                           const Vector2& layoutSize,
                                           const LineRun* lines,
                                           Length         numberOfLines,
                                           Alignment      verticalAlignment)
{
  if(textureSize.width < Math::MACHINE_EPSILON_1000 ||
     textureSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  float boundsX     = 0.0f;
  float boundsWidth = std::min(std::max(layoutSize.width, 0.0f), textureSize.width);

  if(lines != nullptr && numberOfLines > 0u)
  {
    bool  hasLineBounds = false;
    float minX          = textureSize.width;
    float maxX          = 0.0f;

    for(Length index = 0u; index < numberOfLines; ++index)
    {
      const LineRun& line      = lines[index];
      const float    lineWidth = std::max(line.width, 0.0f);
      if(lineWidth <= Math::MACHINE_EPSILON_1000)
      {
        continue;
      }

      const float lineStart = line.alignmentOffset;
      const float lineEnd   = lineStart + lineWidth;

      hasLineBounds = true;
      minX          = std::min(minX, lineStart);
      maxX          = std::max(maxX, lineEnd);
    }

    minX = std::min(std::max(minX, 0.0f), textureSize.width);
    maxX = std::min(std::max(maxX, minX), textureSize.width);

    if(hasLineBounds && (maxX - minX) > Math::MACHINE_EPSILON_1000)
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

inline float GetTextGradientAlignmentFactor(Alignment alignment)
{
  switch(alignment)
  {
    case Alignment::CENTER:
    {
      return 0.5f;
    }
    case Alignment::END:
    {
      return 1.0f;
    }
    case Alignment::START:
    default:
    {
      return 0.0f;
    }
  }
}

inline Vector2 GetTextGradientContentSize(const Vector2& layoutSize,
                                          const LineRun* lines,
                                          Length         numberOfLines)
{
  Vector2 contentSize(std::max(layoutSize.width, 0.0f),
                      std::max(layoutSize.height, 0.0f));

  if(lines != nullptr && numberOfLines > 0u)
  {
    float maxLineWidth  = 0.0f;
    float lineHeightSum = 0.0f;
    for(Length index = 0u; index < numberOfLines; ++index)
    {
      maxLineWidth = std::max(maxLineWidth, std::max(lines[index].width, 0.0f));
      lineHeightSum += std::max(GetLineHeight(lines[index], index + 1u == numberOfLines), 0.0f);
    }

    if(maxLineWidth > Math::MACHINE_EPSILON_1000)
    {
      contentSize.width = maxLineWidth;
    }
    if(lineHeightSum > Math::MACHINE_EPSILON_1000)
    {
      contentSize.height = lineHeightSum;
    }
  }

  return contentSize;
}

inline Vector2 CalculateTextGradientViewportAxisBounds(float     viewportLength,
                                                       float     contentLength,
                                                       Alignment alignment)
{
  if(viewportLength < Math::MACHINE_EPSILON_1000)
  {
    return Vector2(0.0f, 1.0f);
  }

  if(contentLength >= viewportLength)
  {
    return Vector2(0.0f, 1.0f);
  }

  const float clampedContentLength = std::max(contentLength, Math::MACHINE_EPSILON_1000);
  const float offset               = (viewportLength - clampedContentLength) *
                       GetTextGradientAlignmentFactor(alignment);
  return Vector2(offset / viewportLength, clampedContentLength / viewportLength);
}

/**
 * @brief Calculates normalized TextGradient bounds in the visible marquee viewport.
 *
 * Overflow text uses full visible viewport bounds on the overflow axis.
 * Non-overflow text uses aligned text bounds inside the viewport.
 *
 * @param[in] viewportSize The visible marquee viewport size.
 * @param[in] layoutSize The logical text layout size.
 * @param[in] lines Optional laid-out line runs.
 * @param[in] numberOfLines The number of line runs.
 * @param[in] horizontalAlignment The horizontal text alignment.
 * @param[in] verticalAlignment The vertical text alignment.
 * @return Normalized viewport-local bounds, or full viewport bounds for invalid input.
 */
inline Vector4 CalculateMarqueeTextGradientViewportBounds(const Vector2& viewportSize,
                                                          const Vector2& layoutSize,
                                                          const LineRun* lines,
                                                          Length         numberOfLines,
                                                          Alignment      horizontalAlignment,
                                                          Alignment      verticalAlignment)
{
  if(viewportSize.width < Math::MACHINE_EPSILON_1000 ||
     viewportSize.height < Math::MACHINE_EPSILON_1000)
  {
    return Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  }

  const Vector2 contentSize = GetTextGradientContentSize(layoutSize, lines, numberOfLines);
  const Vector2 xBounds =
    CalculateTextGradientViewportAxisBounds(viewportSize.width, contentSize.width, horizontalAlignment);
  const Vector2 yBounds =
    CalculateTextGradientViewportAxisBounds(viewportSize.height, contentSize.height, verticalAlignment);

  return Vector4(xBounds.x, yBounds.x, xBounds.y, yBounds.y);
}

} // namespace Internal

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_BOUNDS_H
