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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_MARQUEE_HELPER_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_MARQUEE_HELPER_H

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/text/text-scroller.h>

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

bool IsRenderableStyle(const TextGradientStyle& style, const Size& textureSize);

bool IsCompositionSupported(bool hasMultipleTextColors,
                            bool containsColorGlyph,
                            bool styleTextureEnabled,
                            bool isOverlayStyle,
                            bool embossEnabled,
                            bool cutoutEnabled);

Dali::Ui::Text::TextScrollerTextGradient CreateMarqueeGradient(const TextGradientStyle& style,
                                                               const Vector4&           bounds,
                                                               const Vector2&           coordinateSize);

void SetMarqueeOverlayGradient(Dali::Ui::Text::TextScrollerTextGradient&       textGradient,
                               const Dali::Ui::Text::TextScrollerTextGradient& overlayGradient,
                               Dali::Ui::Text::GradientOverlayMode             overlayMode);

} // namespace TextGradientMarquee
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_MARQUEE_HELPER_H
