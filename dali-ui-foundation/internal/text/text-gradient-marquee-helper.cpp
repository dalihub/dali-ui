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

namespace
{

constexpr uint32_t ToResourceFlag(CompositionResourceFlag flag)
{
  return static_cast<uint32_t>(flag);
}

constexpr uint32_t ToShaderFeatureFlag(CompositionShaderFeatureFlag flag)
{
  return static_cast<uint32_t>(flag);
}

void AddOverlayStyleResource(CompositionResult& result, bool overlayStyleEnabled)
{
  if(overlayStyleEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::OVERLAY_STYLE_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::OVERLAY_STYLE);
  }
}

CompositionResult MakeCompositionResult(bool supported, CompositionUnsupportedReason unsupportedReason)
{
  CompositionResult result;
  result.supported         = supported;
  result.unsupportedReason = unsupportedReason;
  // The current/fallback marquee path always consumes the precomposed text texture.
  // Gradient lookup textures and shader features are still added by the existing
  // LabelImpl/TextScroller setup path when actual gradient objects are enabled.
  result.requiredResourceFlags = ToResourceFlag(CompositionResourceFlag::TEXT_TEXTURE);
  result.shaderFeatureFlags    = 0u;
  return result;
}

CompositionResult MakeUnsupportedResult(CompositionUnsupportedReason unsupportedReason)
{
  return MakeCompositionResult(false, unsupportedReason);
}

CompositionResult MakeSimpleGradientCompositionResult(bool baseGradientEnabled, bool overlayGradientEnabled, bool overlayStyleEnabled)
{
  CompositionResult result;
  result.supported             = true;
  result.unsupportedReason     = CompositionUnsupportedReason::NONE;
  result.requiredResourceFlags = ToResourceFlag(CompositionResourceFlag::TEXT_TEXTURE);
  result.shaderFeatureFlags    = 0u;
  if(baseGradientEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_LOOKUP_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT);
  }
  if(overlayGradientEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT_OVERLAY);
  }
  AddOverlayStyleResource(result, overlayStyleEnabled);
  return result;
}

CompositionResult MakeMixedColorCompositionResult(bool overlayGradientEnabled, bool styleTextureEnabled, bool overlayStyleEnabled)
{
  CompositionResult result;
  result.supported             = true;
  result.unsupportedReason     = CompositionUnsupportedReason::NONE;
  result.requiredResourceFlags = ToResourceFlag(CompositionResourceFlag::PRESERVED_COLOR_TEXTURE);
  result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::GRADIENT_MASK_TEXTURE);
  result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_LOOKUP_TEXTURE);
  result.shaderFeatureFlags = ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT);
  result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT_MIXED);
  if(styleTextureEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::STYLE_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::STYLE_TEXTURE);
  }
  if(overlayGradientEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT_OVERLAY);
  }
  AddOverlayStyleResource(result, overlayStyleEnabled);
  return result;
}

CompositionResult MakeSimpleStyleCompositionResult(bool baseGradientEnabled, bool overlayGradientEnabled, bool overlayStyleEnabled)
{
  CompositionResult result;
  result.supported             = true;
  result.unsupportedReason     = CompositionUnsupportedReason::NONE;
  result.requiredResourceFlags = ToResourceFlag(CompositionResourceFlag::TEXT_TEXTURE);
  result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::STYLE_TEXTURE);
  result.shaderFeatureFlags = ToShaderFeatureFlag(CompositionShaderFeatureFlag::STYLE_TEXTURE);
  if(baseGradientEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_LOOKUP_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT);
  }
  if(overlayGradientEnabled)
  {
    result.requiredResourceFlags |= ToResourceFlag(CompositionResourceFlag::TEXT_GRADIENT_OVERLAY_LOOKUP_TEXTURE);
    result.shaderFeatureFlags |= ToShaderFeatureFlag(CompositionShaderFeatureFlag::TEXT_GRADIENT_OVERLAY);
  }
  AddOverlayStyleResource(result, overlayStyleEnabled);
  return result;
}

} // namespace

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

CompositionResult GetCompositionResult(const CompositionPolicy& policy)
{
  // Keep fallback/pending reasons first so future callers can distinguish safe fallback
  // and missing shader work before the regular composition incompatibility checks.
  if(policy.cutoutEnabled)
  {
    return MakeUnsupportedResult(CompositionUnsupportedReason::CUTOUT_FALLBACK);
  }

  if(policy.embossEnabled)
  {
    return MakeUnsupportedResult(CompositionUnsupportedReason::EMBOSS_SHADER_FEATURE);
  }

  const bool hasMixedColorTarget = policy.hasMultipleTextColors || policy.containsColorGlyph;
  if(policy.isOverlayStyle && hasMixedColorTarget)
  {
    return MakeUnsupportedResult(CompositionUnsupportedReason::OVERLAY_STYLE);
  }

  if(policy.styleTextureEnabled)
  {
    if(policy.hasMultipleTextColors)
    {
      return MakeUnsupportedResult(CompositionUnsupportedReason::MULTIPLE_TEXT_COLORS);
    }

    if(policy.containsColorGlyph)
    {
      return MakeUnsupportedResult(CompositionUnsupportedReason::COLOR_GLYPH);
    }

    if(policy.baseGradientEnabled || policy.overlayGradientEnabled)
    {
      return MakeSimpleStyleCompositionResult(policy.baseGradientEnabled, policy.overlayGradientEnabled, policy.isOverlayStyle);
    }

    if(policy.isOverlayStyle)
    {
      return MakeSimpleGradientCompositionResult(false, false, true);
    }

    return MakeUnsupportedResult(CompositionUnsupportedReason::STYLE_TEXTURE);
  }

  const bool overlayOnlyGradient = !policy.baseGradientEnabled && policy.overlayGradientEnabled;
  if(hasMixedColorTarget && overlayOnlyGradient)
  {
    return MakeCompositionResult(true, CompositionUnsupportedReason::NONE);
  }

  if(policy.hasMultipleTextColors)
  {
    return MakeUnsupportedResult(CompositionUnsupportedReason::MULTIPLE_TEXT_COLORS);
  }

  if(policy.containsColorGlyph)
  {
    return MakeUnsupportedResult(CompositionUnsupportedReason::COLOR_GLYPH);
  }

  return MakeSimpleGradientCompositionResult(policy.baseGradientEnabled, policy.overlayGradientEnabled, policy.isOverlayStyle);
}

CompositionResult GetMixedColorCompositionResult(const CompositionPolicy& policy)
{
  // Mixed color composition covers both explicit text colors and renderable
  // color glyphs. Simple marquee composition still rejects them; mixed path
  // preserves them through RGBA and applies gradient only through the mask.
  const bool hasMixedColorTarget = policy.hasMultipleTextColors || policy.containsColorGlyph;
  if(!policy.baseGradientEnabled || !hasMixedColorTarget)
  {
    // A false result with NONE reason can mean "not a mixed-color/color-glyph candidate",
    // not a content fallback. Content incompatibility is reported below through
    // GetCompositionResult().
    return MakeUnsupportedResult(GetCompositionResult(policy).unsupportedReason);
  }

  CompositionPolicy contentPolicy      = policy;
  contentPolicy.hasMultipleTextColors  = false;
  contentPolicy.containsColorGlyph     = false;
  contentPolicy.styleTextureEnabled    = false;
  contentPolicy.baseGradientEnabled    = false;
  contentPolicy.overlayGradientEnabled = false;

  const CompositionResult contentResult = GetCompositionResult(contentPolicy);
  if(!contentResult.supported)
  {
    return contentResult;
  }

  return MakeMixedColorCompositionResult(policy.overlayGradientEnabled, policy.styleTextureEnabled, policy.isOverlayStyle);
}

bool IsMixedColorCompositionSupported(const CompositionPolicy& policy)
{
  return GetMixedColorCompositionResult(policy).supported;
}

bool IsCompositionSupported(bool hasMultipleTextColors,
                            bool containsColorGlyph,
                            bool styleTextureEnabled,
                            bool isOverlayStyle,
                            bool embossEnabled,
                            bool cutoutEnabled)
{
  // Legacy callers do not pass base/overlay gradient presence. Keep style
  // texture composition closed here; LabelImpl uses CompositionPolicy directly.
  CompositionPolicy policy;
  policy.hasMultipleTextColors = hasMultipleTextColors;
  policy.containsColorGlyph    = containsColorGlyph;
  policy.styleTextureEnabled   = styleTextureEnabled;
  policy.isOverlayStyle        = isOverlayStyle;
  policy.embossEnabled         = embossEnabled;
  policy.cutoutEnabled         = cutoutEnabled;

  return GetCompositionResult(policy).supported;
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
