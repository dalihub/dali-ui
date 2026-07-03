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
namespace GradientMarquee
{

bool IsRenderable(const Gradient::Style& style, const Size& textureSize)
{
  if(!Gradient::IsRenderable(style))
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

Dali::Ui::Text::TextScrollerGradient CreateScrollerGradient(const Gradient::Style& style,
                                                            const Vector4&         bounds,
                                                            const Vector2&         coordinateSize)
{
  const Gradient::RenderData renderData =
    Gradient::ResolveRenderData(style, bounds, coordinateSize);

  Dali::Ui::Text::TextScrollerGradient textGradient;
  textGradient.enabled         = renderData.enabled;
  textGradient.type            = renderData.type;
  textGradient.startPosition   = renderData.startPosition;
  textGradient.endPosition     = renderData.endPosition;
  textGradient.radialCenter    = renderData.radialCenter;
  textGradient.radialScale     = renderData.radialScale;
  textGradient.conicCenter     = renderData.conicCenter;
  textGradient.conicScale      = renderData.conicScale;
  textGradient.conicStartAngle = renderData.conicStartAngle;
  textGradient.startOffset     = renderData.startOffset;
  textGradient.bounds          = renderData.bounds;
  return textGradient;
}

void SetOverlayGradient(Dali::Ui::Text::TextScrollerGradient&       textGradient,
                        const Dali::Ui::Text::TextScrollerGradient& overlayGradient,
                        Dali::Ui::Text::GradientOverlayMode         overlayMode)
{
  textGradient.overlayEnabled         = overlayGradient.enabled;
  textGradient.overlayType            = overlayGradient.type;
  textGradient.overlayStartPosition   = overlayGradient.startPosition;
  textGradient.overlayEndPosition     = overlayGradient.endPosition;
  textGradient.overlayRadialCenter    = overlayGradient.radialCenter;
  textGradient.overlayRadialScale     = overlayGradient.radialScale;
  textGradient.overlayConicCenter     = overlayGradient.conicCenter;
  textGradient.overlayConicScale      = overlayGradient.conicScale;
  textGradient.overlayConicStartAngle = overlayGradient.conicStartAngle;
  textGradient.overlayStartOffset     = overlayGradient.startOffset;
  textGradient.overlayBounds          = overlayGradient.bounds;
  textGradient.overlayMode            = overlayMode;
}

} // namespace GradientMarquee
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali
