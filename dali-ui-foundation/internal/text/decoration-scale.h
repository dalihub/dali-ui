#ifndef DALI_UI_TEXT_DECORATION_SCALE_H
#define DALI_UI_TEXT_DECORATION_SCALE_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/strikethrough-style-properties.h>
#include <dali-ui-foundation/internal/text/underline-style-properties.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{
/**
 * @brief Resolves an authored distance using only the effective UI scale.
 *
 * FontSizeScale, renderScale and automatic font metrics are not involved.
 * Invalid scales leave the authored value unchanged.
 *
 * @param[in] authored The authored distance, including signed offsets.
 * @param[in] uiScale The control's effective UI scale after its UI scale policy.
 * @return The scaled distance, preserving zero and its automatic/default meaning.
 */
inline float ResolveDecorationDistance(float authored, float uiScale)
{
  return (uiScale == 1.0f || authored == 0.0f || !std::isfinite(uiScale) || uiScale <= 0.0f)
           ? authored
           : authored * uiScale;
}

/**
 * @brief Resolves both components of an authored signed offset using UI scale only.
 *
 * @param[in] authored The authored offset.
 * @param[in] uiScale The effective UI scale; invalid scales leave the offset unchanged.
 * @return The effective offset, with each zero component preserved.
 */
inline Vector2 ResolveDecorationOffset(const Vector2& authored, float uiScale)
{
  return Vector2(ResolveDecorationDistance(authored.x, uiScale),
                 ResolveDecorationDistance(authored.y, uiScale));
}

/**
 * @brief Resolves an outline width to the shared layout/render integer representation.
 *
 * @param[in] authored The authored width in the existing uint16_t representation.
 * @param[in] uiScale The effective UI scale; invalid scales leave the width unchanged.
 * @return The nearest integer width, saturated to uint16_t. Zero remains zero.
 */
inline uint16_t ResolveDecorationOutlineWidth(uint16_t authored, float uiScale)
{
  if(uiScale == 1.0f || authored == 0u || !std::isfinite(uiScale) || uiScale <= 0.0f)
  {
    return authored;
  }
  // The existing layout/atlas interface requires uint16_t. Resolve once to
  // that shared representation, rounding fractional scales without overflow.
  return static_cast<uint16_t>(std::min(std::round(static_cast<double>(authored) * uiScale),
                                        static_cast<double>(std::numeric_limits<uint16_t>::max())));
}

/**
 * @brief Resolves defined underline geometry on a copy of the authored properties.
 *
 * @param[in] authored The authored properties, including unchanged definition flags.
 * @param[in] uiScale The effective UI scale, excluding FontSizeScale and renderScale.
 * @return Properties with resolved distances; colors, flags and default zeros are preserved.
 */
inline UnderlineStyleProperties ResolveDecorationProperties(UnderlineStyleProperties authored, float uiScale)
{
  if(authored.heightDefined)
  {
    authored.height = ResolveDecorationDistance(authored.height, uiScale);
  }
  if(authored.dashWidthDefined)
  {
    authored.dashWidth = ResolveDecorationDistance(authored.dashWidth, uiScale);
  }
  if(authored.dashGapDefined)
  {
    authored.dashGap = ResolveDecorationDistance(authored.dashGap, uiScale);
  }
  return authored;
}

/**
 * @brief Resolves defined line-through thickness on a copy of the authored properties.
 *
 * @param[in] authored The authored properties, including unchanged definition flags.
 * @param[in] uiScale The effective UI scale, excluding FontSizeScale and renderScale.
 * @return Properties with resolved thickness, preserving zero and other properties.
 */
inline StrikethroughStyleProperties ResolveDecorationProperties(StrikethroughStyleProperties authored, float uiScale)
{
  if(authored.heightDefined)
  {
    authored.height = ResolveDecorationDistance(authored.height, uiScale);
  }
  return authored;
}
} // namespace Text
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_TEXT_DECORATION_SCALE_H
