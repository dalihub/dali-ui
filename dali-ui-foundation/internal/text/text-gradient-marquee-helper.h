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

#include <cstdint>

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
namespace GradientMarquee
{

enum class CompositionUnsupportedReason : uint8_t
{
  NONE,
  MULTIPLE_TEXT_COLORS,
  COLOR_GLYPH,
  STYLE_TEXTURE,
  OVERLAY_STYLE,
  CUTOUT_FALLBACK,
  EMBOSS_SHADER_FEATURE
};

enum class CompositionResourceFlag : uint32_t
{
  NONE                                 = 0u,
  TEXT_TEXTURE                         = 1u << 0u,
  TEXT_GRADIENT_LOOKUP_TEXTURE         = 1u << 1u,
  TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE = 1u << 2u,
  PRESERVED_COLOR_TEXTURE              = 1u << 3u,
  GRADIENT_MASK_TEXTURE                = 1u << 4u,
  STYLE_TEXTURE                        = 1u << 5u,
  OVERLAY_STYLE_TEXTURE                = 1u << 6u
};

enum class CompositionShaderFeatureFlag : uint32_t
{
  NONE                  = 0u,
  TEXT_GRADIENT         = 1u << 0u,
  TEXT_GRADIENT_OVERLAY = 1u << 1u,
  TEXT_GRADIENT_MIXED   = 1u << 2u,
  STYLE_TEXTURE         = 1u << 3u,
  OVERLAY_STYLE         = 1u << 4u,
  EMBOSS                = 1u << 5u
};

struct CompositionPolicy
{
  bool hasMultipleTextColors{false};
  bool containsColorGlyph{false};
  bool styleTextureEnabled{false};
  bool isOverlayStyle{false};
  bool embossEnabled{false};
  bool cutoutEnabled{false};
  bool baseGradientEnabled{false};
  bool overlayGradientEnabled{false};
};

// Describes the currently selected marquee composition/fallback path.
// Future preserved/mask requirements are documented separately until a mixed
// marquee path is enabled.
struct CompositionResult
{
  bool                         supported{false};
  CompositionUnsupportedReason unsupportedReason{CompositionUnsupportedReason::NONE};
  uint32_t                     requiredResourceFlags{0u}; ///< Resources required by the selected current/fallback path.
  uint32_t                     shaderFeatureFlags{0u};    ///< Shader features directly selected by this composition policy.
};

bool IsRenderable(const Gradient::Style& style, const Size& textureSize);

CompositionResult GetCompositionResult(const CompositionPolicy& policy);

CompositionResult GetMixedColorCompositionResult(const CompositionPolicy& policy);

bool IsMixedColorCompositionSupported(const CompositionPolicy& policy);

bool IsCompositionSupported(bool hasMultipleTextColors,
                            bool containsColorGlyph,
                            bool styleTextureEnabled,
                            bool isOverlayStyle,
                            bool embossEnabled,
                            bool cutoutEnabled);

Dali::Ui::Text::TextScrollerGradient CreateScrollerGradient(const Gradient::Style& style,
                                                            const Vector4&         bounds,
                                                            const Vector2&         coordinateSize);

void SetOverlayGradient(Dali::Ui::Text::TextScrollerGradient&       textGradient,
                        const Dali::Ui::Text::TextScrollerGradient& overlayGradient,
                        Dali::Ui::Text::GradientOverlayMode         overlayMode);

} // namespace GradientMarquee
} // namespace Internal
} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_GRADIENT_MARQUEE_HELPER_H
