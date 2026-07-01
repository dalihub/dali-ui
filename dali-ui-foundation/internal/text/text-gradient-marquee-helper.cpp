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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/text-gradient-marquee-helper.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace Internal
{
namespace TextGradientMarquee
{

bool IsStyleSupported(const TextGradientStyle& style, const Size& textureSize)
{
  if(!TextGradient::IsSupportedTextGradientStyle(style))
  {
    return false;
  }

  if(textureSize.width < Math::MACHINE_EPSILON_1000 || textureSize.height < Math::MACHINE_EPSILON_1000)
  {
    return false;
  }

  return true;
}

bool IsCompositionSupported(bool hasMultipleTextColors,
                            bool containsColorGlyph,
                            bool styleTextureEnabled,
                            bool isOverlayStyle,
                            bool embossEnabled,
                            bool cutoutEnabled)
{
  return !(hasMultipleTextColors || containsColorGlyph || styleTextureEnabled || isOverlayStyle || embossEnabled || cutoutEnabled);
}

bool IsSupported(const TextGradientStyle& style,
                 const Size&              textureSize,
                 bool                     hasMultipleTextColors,
                 bool                     containsColorGlyph,
                 bool                     styleTextureEnabled,
                 bool                     isOverlayStyle,
                 bool                     embossEnabled,
                 bool                     cutoutEnabled)
{
  return IsStyleSupported(style, textureSize) &&
         IsCompositionSupported(hasMultipleTextColors,
                                containsColorGlyph,
                                styleTextureEnabled,
                                isOverlayStyle,
                                embossEnabled,
                                cutoutEnabled);
}

Dali::Ui::Text::TextScrollerTextGradient CreateMarqueeGradient(const TextGradientStyle& style,
                                                               const Vector4&           bounds,
                                                               const Vector2&           coordinateSize)
{
  Dali::Ui::Text::TextScrollerTextGradient textGradient;
  textGradient.enabled       = true;
  textGradient.type          = style.type;
  textGradient.startPosition = Text::Internal::ResolveTextGradientPosition(style.units, style.linearStart, bounds, coordinateSize);
  textGradient.endPosition   = Text::Internal::ResolveTextGradientPosition(style.units, style.linearEnd, bounds, coordinateSize);
  if(style.type == Gradient::Type::RADIAL)
  {
    textGradient.radialCenter =
      Text::Internal::ResolveTextGradientPosition(style.units, style.radialCenter, bounds, coordinateSize);
    textGradient.radialScale =
      Text::Internal::ResolveTextGradientRadialScale(style.units, style.radialRadius, bounds, coordinateSize);
  }
  textGradient.startOffset = style.startOffset;
  textGradient.bounds      = bounds;
  return textGradient;
}

} // namespace TextGradientMarquee
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
