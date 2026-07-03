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
#include <dali-ui-foundation/internal/text/marquee/marquee-builder.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/rendering/texture.h>

#include <cmath>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/color-glyph-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

namespace
{

uint32_t GetNextSimpleOverlayStyleTextureIndex(const TextScrollerGradient& textGradient)
{
  uint32_t textureSetIndex = 1u;
  if(textGradient.enabled)
  {
    ++textureSetIndex;
  }
  if(textGradient.overlayEnabled)
  {
    ++textureSetIndex;
  }
  if(textGradient.styleTextureEnabled)
  {
    ++textureSetIndex;
  }
  return textureSetIndex;
}

uint32_t GetNextMixedOverlayStyleTextureIndex(const TextScrollerGradient& textGradient)
{
  uint32_t textureSetIndex = 3u;
  if(textGradient.overlayEnabled)
  {
    ++textureSetIndex;
  }
  if(textGradient.styleTextureEnabled)
  {
    ++textureSetIndex;
  }
  return textureSetIndex;
}

struct CompositionCandidates
{
  Internal::GradientMarquee::CompositionPolicy policy;
  Internal::GradientMarquee::CompositionResult simpleResult;
  Internal::GradientMarquee::CompositionResult mixedResult;

  bool hasGradientFeature{false};
  bool hasMixedColorTarget{false};
  bool styleMixedCandidate{false};
  bool styleSimpleCandidate{false};
  bool mixedColorCandidate{false};
  bool simpleGradientCandidate{false};
};

CompositionCandidates GetCompositionCandidates(const MarqueeBuilder::CompositionRequest& request)
{
  CompositionCandidates candidates;
  candidates.hasGradientFeature = request.gradientState.baseRenderable ||
                                  request.gradientState.overlayRenderable;

  if(!candidates.hasGradientFeature)
  {
    return candidates;
  }

  candidates.policy =
    MarqueeBuilder::CreateGradientCompositionPolicy(request.featureInfo,
                                                    request.embossEnabled,
                                                    request.gradientState);
  candidates.simpleResult =
    Internal::GradientMarquee::GetCompositionResult(candidates.policy);
  candidates.mixedResult =
    Internal::GradientMarquee::GetMixedColorCompositionResult(candidates.policy);
  candidates.hasMixedColorTarget = request.featureInfo.hasMultipleTextColors ||
                                   request.featureInfo.containsColorGlyph;

  candidates.styleMixedCandidate =
    request.featureInfo.styleTextureEnabled &&
    candidates.hasMixedColorTarget &&
    candidates.mixedResult.supported &&
    request.gradientState.baseStyleRenderable &&
    !request.featureInfo.cutoutEnabled &&
    !request.embossEnabled;

  candidates.styleSimpleCandidate =
    request.featureInfo.styleTextureEnabled &&
    candidates.simpleResult.supported &&
    !request.featureInfo.hasMultipleTextColors &&
    !request.featureInfo.containsColorGlyph &&
    !request.featureInfo.cutoutEnabled &&
    !request.embossEnabled;

  candidates.mixedColorCandidate =
    !candidates.styleMixedCandidate &&
    !candidates.styleSimpleCandidate &&
    candidates.mixedResult.supported &&
    request.gradientState.baseStyleRenderable;

  candidates.simpleGradientCandidate =
    !candidates.styleMixedCandidate &&
    !candidates.styleSimpleCandidate &&
    candidates.simpleResult.supported;

  return candidates;
}

void UploadTextScrollTexture(Texture texture, PixelData data)
{
#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    // Keep Label text out of this general content-building boundary. Do not add
    // runtime GetText/string plumbing only to enrich the GPU memory profile label;
    // a stable category label is enough for text-scroll texture uploads.
    std::string profilingLabel("Text marquee");
    Dali::Integration::TextureUploadWithContent(texture, data, Dali::Integration::ToDaliString(std::move(profilingLabel)), Dali::Integration::TextureContextTypeHint::TEXT_SCROLL);
  }
#else
  texture.Upload(data);
#endif
}

} // namespace

bool MarqueeBuilder::HasOverlayStyle(const ModelInterface& textModel,
                                     bool                  markupEnabled)
{
  const bool markupUnderlineEnabled     = markupEnabled && textModel.IsMarkupUnderlineSet();
  const bool markupStrikethroughEnabled = markupEnabled && textModel.IsMarkupStrikethroughSet();
  return textModel.IsUnderlineEnabled() ||
         textModel.IsStrikethroughEnabled() ||
         markupUnderlineEnabled ||
         markupStrikethroughEnabled;
}

MarqueeBuilder::FeatureInfo MarqueeBuilder::CollectGradientFeatureInfo(const ModelInterface& textModel,
                                                                       bool                  markupEnabled,
                                                                       bool                  textCutoutEnabled)
{
  FeatureInfo info;

  const Vector4* const          colorsBuffer        = textModel.GetColors();
  const Text::ColorIndex* const colorIndices        = textModel.GetColorIndices();
  const Text::GlyphInfo* const  glyphsBuffer        = textModel.GetGlyphs();
  const Text::Length            numberOfGlyphs      = textModel.GetNumberOfGlyphs();
  const bool                    hasColorIndexBuffer = nullptr != colorsBuffer && nullptr != colorIndices;
  TextAbstraction::FontClient   fontClient          = TextAbstraction::FontClient::Get();
  for(Text::Length glyphIndex = 0u; glyphIndex < numberOfGlyphs; ++glyphIndex)
  {
    if(hasColorIndexBuffer && *(colorIndices + glyphIndex) > 0u)
    {
      info.hasMultipleTextColors = true;
    }

    if(!info.containsColorGlyph && glyphsBuffer)
    {
      const Text::GlyphInfo* const glyphInfo = glyphsBuffer + glyphIndex;
      if(Text::Internal::IsRenderableColorGlyph(fontClient, glyphInfo->fontId, glyphInfo->index))
      {
        info.containsColorGlyph = true;
      }
    }

    if(info.hasMultipleTextColors && info.containsColorGlyph)
    {
      break;
    }
  }

  const Vector2& shadowOffset  = textModel.GetShadowOffset();
  const bool     shadowEnabled = fabsf(shadowOffset.x) > Math::MACHINE_EPSILON_1 ||
                             fabsf(shadowOffset.y) > Math::MACHINE_EPSILON_1;

  const bool outlineEnabled      = textModel.GetOutlineWidth() > Math::MACHINE_EPSILON_1;
  const bool backgroundEnabled   = textModel.IsBackgroundEnabled();
  const bool backgroundMarkupSet = textModel.IsMarkupBackgroundColorSet();

  info.styleTextureEnabled = shadowEnabled || outlineEnabled || backgroundEnabled || backgroundMarkupSet;
  info.isOverlayStyle      = HasOverlayStyle(textModel, markupEnabled);
  info.cutoutEnabled       = textCutoutEnabled || textModel.IsBackgroundWithCutoutEnabled();
  return info;
}

Sampler MarqueeBuilder::CreateTextScrollSampler(bool isHorizontal)
{
  Sampler sampler = Sampler::New();
  sampler.SetFilterMode(FilterMode::LINEAR, FilterMode::LINEAR);

  if(isHorizontal)
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT, Dali::WrapMode::DEFAULT);
  }
  else
  {
    sampler.SetWrapMode(Dali::WrapMode::DEFAULT, Dali::WrapMode::DEFAULT, Dali::WrapMode::REPEAT);
  }

  return sampler;
}

MarqueeBuilder::PreparedContent MarqueeBuilder::CreateTextContent(PixelData textPixelData,
                                                                  Sampler   sampler)
{
  PreparedContent content;
  content.textureSet = TextureSet::New();
  AddTextScrollTexture(content.textureSet, textPixelData, sampler, 0u);
  return content;
}

MarqueeBuilder::GradientState MarqueeBuilder::ResolveGradientState(const Dali::Ui::Gradient::Base& baseGradient,
                                                                   const Dali::Ui::Gradient::Base& overlayGradient,
                                                                   const Size&                     verifiedSize)
{
  GradientState state;
  state.baseRenderable    = Internal::Gradient::IsRenderable(baseGradient);
  state.overlayRenderable = Internal::Gradient::IsRenderable(overlayGradient);

  state.baseStyle    = state.baseRenderable ? Internal::Gradient::CreateStyle(baseGradient)
                                            : Internal::Gradient::Style();
  state.overlayStyle = state.overlayRenderable ? Internal::Gradient::CreateStyle(overlayGradient)
                                               : Internal::Gradient::Style();

  state.baseStyleRenderable =
    state.baseRenderable && Internal::GradientMarquee::IsRenderable(state.baseStyle, verifiedSize);
  state.overlayStyleRenderable =
    state.overlayRenderable && Internal::GradientMarquee::IsRenderable(state.overlayStyle, verifiedSize);

  return state;
}

Internal::GradientMarquee::CompositionPolicy MarqueeBuilder::CreateGradientCompositionPolicy(const FeatureInfo&   featureInfo,
                                                                                             bool                 embossEnabled,
                                                                                             const GradientState& gradientState)
{
  Internal::GradientMarquee::CompositionPolicy policy;
  policy.hasMultipleTextColors  = featureInfo.hasMultipleTextColors;
  policy.containsColorGlyph     = featureInfo.containsColorGlyph;
  policy.styleTextureEnabled    = featureInfo.styleTextureEnabled;
  policy.isOverlayStyle         = featureInfo.isOverlayStyle;
  policy.embossEnabled          = embossEnabled;
  policy.cutoutEnabled          = featureInfo.cutoutEnabled;
  policy.baseGradientEnabled    = gradientState.baseRenderable;
  policy.overlayGradientEnabled = gradientState.overlayRenderable;
  return policy;
}

void MarqueeBuilder::ApplyBaseAnimationState(TextScrollerGradient& textGradient,
                                             const AnimationState& animationState)
{
  textGradient.startOffsetPropertyIndex = animationState.baseStartOffsetIndex;
  textGradient.applyConstraintsAlways   = animationState.baseApplyAlways;
}

void MarqueeBuilder::ApplyOverlayAnimationState(TextScrollerGradient& textGradient,
                                                const AnimationState& animationState)
{
  textGradient.overlayStartOffsetPropertyIndex = animationState.overlayStartOffsetIndex;
  textGradient.overlayApplyConstraintsAlways   = animationState.overlayApplyAlways;
}

bool MarqueeBuilder::TryApplyMixedGradientContent(PreparedContent&                   content,
                                                  const MixedGradientContentRequest& request)
{
  if(!Internal::GradientMarquee::GetMixedColorCompositionResult(request.compositionPolicy).supported)
  {
    return false;
  }

  if(!request.gradientState.baseStyleRenderable)
  {
    return false;
  }

  const bool overlayEnabled = request.compositionPolicy.overlayGradientEnabled;
  if(overlayEnabled &&
     (!request.gradientState.overlayStyleRenderable || !request.overlayBoundsResolved))
  {
    return false;
  }

  const bool styleEnabled = request.compositionPolicy.styleTextureEnabled;
  if(styleEnabled != request.styleTextureEnabled)
  {
    return false;
  }

  if(!IsPixelDataSizeEqual(request.preservedPixelData, request.verifiedSize) ||
     !IsPixelDataSizeEqual(request.maskPixelData, request.verifiedSize))
  {
    return false;
  }

  if(styleEnabled &&
     !IsPixelDataSizeEqual(request.stylePixelData, request.verifiedSize))
  {
    return false;
  }

  TextureSet mixedTextureSet = TextureSet::New();
  AddTextScrollTexture(mixedTextureSet, request.preservedPixelData, request.sampler, 0u);
  AddTextScrollTexture(mixedTextureSet, request.maskPixelData, request.sampler, 1u);

  uint32_t textureSetIndex = 2u;
  Internal::Gradient::AddLookupTexture(mixedTextureSet, textureSetIndex, request.gradientState.baseStyle);

  TextScrollerGradient mixedGradient =
    Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.baseStyle,
                                                      request.baseBounds.bounds,
                                                      request.baseBounds.coordinateSize);
  mixedGradient.mixedTextGradient = true;
  ApplyBaseAnimationState(mixedGradient, request.animationState);

  if(overlayEnabled)
  {
    Internal::Gradient::AddLookupTexture(mixedTextureSet, textureSetIndex, request.gradientState.overlayStyle);

    const TextScrollerGradient textGradientOverlay =
      Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.overlayStyle,
                                                        request.overlayBounds.bounds,
                                                        request.overlayBounds.coordinateSize);

    Internal::GradientMarquee::SetOverlayGradient(mixedGradient,
                                                  textGradientOverlay,
                                                  request.overlayMode);
    ApplyOverlayAnimationState(mixedGradient, request.animationState);
  }

  if(styleEnabled)
  {
    AddTextScrollTexture(mixedTextureSet, request.stylePixelData, request.sampler, textureSetIndex);
    mixedGradient.styleTextureEnabled = true;
  }

  content.textureSet   = mixedTextureSet;
  content.textGradient = mixedGradient;

  return true;
}

bool MarqueeBuilder::TryApplySimpleStyleContent(PreparedContent&                 content,
                                                const SimpleStyleContentRequest& request)
{
  const Internal::GradientMarquee::CompositionResult result =
    Internal::GradientMarquee::GetCompositionResult(request.compositionPolicy);
  if(!result.supported || !request.compositionPolicy.styleTextureEnabled)
  {
    return false;
  }

  const bool baseEnabled    = request.compositionPolicy.baseGradientEnabled;
  const bool overlayEnabled = request.compositionPolicy.overlayGradientEnabled;
  if(!baseEnabled && !overlayEnabled)
  {
    return false;
  }

  if(baseEnabled &&
     (!request.gradientState.baseStyleRenderable || !request.baseBoundsResolved))
  {
    return false;
  }

  if(overlayEnabled &&
     (!request.gradientState.overlayStyleRenderable || !request.overlayBoundsResolved))
  {
    return false;
  }

  if(!IsPixelDataSizeEqual(request.fillPixelData, request.verifiedSize) ||
     !IsPixelDataSizeEqual(request.stylePixelData, request.verifiedSize))
  {
    return false;
  }

  TextureSet styleTextureSet = TextureSet::New();
  AddTextScrollTexture(styleTextureSet, request.fillPixelData, request.sampler, 0u);

  uint32_t             textureSetIndex = 1u;
  TextScrollerGradient textGradient;
  if(baseEnabled)
  {
    Internal::Gradient::AddLookupTexture(styleTextureSet, textureSetIndex, request.gradientState.baseStyle);
    textGradient =
      Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.baseStyle,
                                                        request.baseBounds.bounds,
                                                        request.baseBounds.coordinateSize);
    ApplyBaseAnimationState(textGradient, request.animationState);
  }

  if(overlayEnabled)
  {
    Internal::Gradient::AddLookupTexture(styleTextureSet, textureSetIndex, request.gradientState.overlayStyle);
    const TextScrollerGradient textGradientOverlay =
      Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.overlayStyle,
                                                        request.overlayBounds.bounds,
                                                        request.overlayBounds.coordinateSize);
    Internal::GradientMarquee::SetOverlayGradient(textGradient,
                                                  textGradientOverlay,
                                                  request.overlayMode);
    ApplyOverlayAnimationState(textGradient, request.animationState);
  }

  AddTextScrollTexture(styleTextureSet, request.stylePixelData, request.sampler, textureSetIndex);
  textGradient.styleTextureEnabled = true;

  content.textureSet   = styleTextureSet;
  content.textGradient = textGradient;
  return true;
}

void MarqueeBuilder::ApplySimpleGradientContent(PreparedContent&                    content,
                                                const SimpleGradientContentRequest& request)
{
  uint32_t textureSetIndex = 1u;

  if(request.gradientState.baseStyleRenderable && request.baseBoundsResolved)
  {
    Internal::Gradient::AddLookupTexture(content.textureSet, textureSetIndex, request.gradientState.baseStyle);

    content.textGradient =
      Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.baseStyle,
                                                        request.baseBounds.bounds,
                                                        request.baseBounds.coordinateSize);
    ApplyBaseAnimationState(content.textGradient, request.animationState);
  }

  if(request.gradientState.overlayStyleRenderable && request.overlayBoundsResolved)
  {
    Internal::Gradient::AddLookupTexture(content.textureSet, textureSetIndex, request.gradientState.overlayStyle);

    const TextScrollerGradient textGradientOverlay =
      Internal::GradientMarquee::CreateScrollerGradient(request.gradientState.overlayStyle,
                                                        request.overlayBounds.bounds,
                                                        request.overlayBounds.coordinateSize);

    Internal::GradientMarquee::SetOverlayGradient(content.textGradient,
                                                  textGradientOverlay,
                                                  request.overlayMode);
    ApplyOverlayAnimationState(content.textGradient, request.animationState);
  }
}

MarqueeBuilder::CompositionPlan MarqueeBuilder::GetCompositionPlan(const CompositionRequest& request)
{
  CompositionPlan plan;
  plan.needsOverlayStylePixelData = request.featureInfo.isOverlayStyle;

  const CompositionCandidates candidates = GetCompositionCandidates(request);
  if(!candidates.hasGradientFeature)
  {
    return plan;
  }

  const bool hasGradientComposition =
    candidates.styleMixedCandidate ||
    candidates.styleSimpleCandidate ||
    candidates.mixedColorCandidate ||
    candidates.simpleGradientCandidate;
  if(!hasGradientComposition)
  {
    return plan;
  }

  plan.needsBaseBounds    = request.gradientState.baseStyleRenderable;
  plan.needsOverlayBounds = request.gradientState.overlayStyleRenderable;

  if(candidates.styleMixedCandidate)
  {
    plan.needsPreservedMaskPixelData = true;
    plan.needsStylePixelData         = true;
  }
  else if(candidates.styleSimpleCandidate)
  {
    plan.needsFillPixelData  = true;
    plan.needsStylePixelData = true;
  }
  else if(candidates.mixedColorCandidate)
  {
    plan.needsPreservedMaskPixelData = true;
  }

  return plan;
}

void MarqueeBuilder::ApplyPreparedComposition(PreparedContent&          content,
                                              const CompositionRequest& request,
                                              const PixelDataBundle&    pixels)
{
  const CompositionCandidates candidates = GetCompositionCandidates(request);

  if(candidates.hasGradientFeature)
  {
    if(candidates.styleMixedCandidate)
    {
      MixedGradientContentRequest mixedRequest;
      mixedRequest.preservedPixelData    = pixels.preservedPixelData;
      mixedRequest.maskPixelData         = pixels.maskPixelData;
      mixedRequest.sampler               = request.sampler;
      mixedRequest.verifiedSize          = request.verifiedSize;
      mixedRequest.gradientState         = request.gradientState;
      mixedRequest.animationState        = request.animationState;
      mixedRequest.compositionPolicy     = candidates.policy;
      mixedRequest.baseBounds            = request.baseBounds;
      mixedRequest.overlayBoundsResolved = request.overlayBoundsResolved;
      mixedRequest.overlayBounds         = request.overlayBounds;
      mixedRequest.overlayMode           = request.overlayMode;
      mixedRequest.styleTextureEnabled   = true;
      mixedRequest.stylePixelData        = pixels.stylePixelData;
      TryApplyMixedGradientContent(content, mixedRequest);
    }
    else if(candidates.styleSimpleCandidate)
    {
      SimpleStyleContentRequest styleRequest;
      styleRequest.fillPixelData         = pixels.fillPixelData;
      styleRequest.stylePixelData        = pixels.stylePixelData;
      styleRequest.sampler               = request.sampler;
      styleRequest.verifiedSize          = request.verifiedSize;
      styleRequest.gradientState         = request.gradientState;
      styleRequest.animationState        = request.animationState;
      styleRequest.compositionPolicy     = candidates.policy;
      styleRequest.baseBoundsResolved    = request.baseBoundsResolved;
      styleRequest.baseBounds            = request.baseBounds;
      styleRequest.overlayBoundsResolved = request.overlayBoundsResolved;
      styleRequest.overlayBounds         = request.overlayBounds;
      styleRequest.overlayMode           = request.overlayMode;
      TryApplySimpleStyleContent(content, styleRequest);
    }
    else
    {
      if(candidates.mixedColorCandidate)
      {
        MixedGradientContentRequest mixedRequest;
        mixedRequest.preservedPixelData    = pixels.preservedPixelData;
        mixedRequest.maskPixelData         = pixels.maskPixelData;
        mixedRequest.sampler               = request.sampler;
        mixedRequest.verifiedSize          = request.verifiedSize;
        mixedRequest.gradientState         = request.gradientState;
        mixedRequest.animationState        = request.animationState;
        mixedRequest.compositionPolicy     = candidates.policy;
        mixedRequest.baseBounds            = request.baseBounds;
        mixedRequest.overlayBoundsResolved = request.overlayBoundsResolved;
        mixedRequest.overlayBounds         = request.overlayBounds;
        mixedRequest.overlayMode           = request.overlayMode;
        TryApplyMixedGradientContent(content, mixedRequest);
      }

      if(!content.textGradient.mixedTextGradient &&
         candidates.simpleGradientCandidate)
      {
        SimpleGradientContentRequest simpleRequest;
        simpleRequest.gradientState         = request.gradientState;
        simpleRequest.animationState        = request.animationState;
        simpleRequest.baseBoundsResolved    = request.baseBoundsResolved;
        simpleRequest.baseBounds            = request.baseBounds;
        simpleRequest.overlayBoundsResolved = request.overlayBoundsResolved;
        simpleRequest.overlayBounds         = request.overlayBounds;
        simpleRequest.overlayMode           = request.overlayMode;
        ApplySimpleGradientContent(content, simpleRequest);
      }
    }
  }

  if(request.featureInfo.isOverlayStyle)
  {
    if(content.textGradient.mixedTextGradient)
    {
      TryAppendMixedOverlayStyleContent(content,
                                        pixels.overlayStylePixelData,
                                        request.sampler,
                                        request.verifiedSize);
    }
    else
    {
      TryAppendSimpleOverlayStyleContent(content,
                                         pixels.overlayStylePixelData,
                                         request.sampler,
                                         request.verifiedSize);
    }
  }
}

bool MarqueeBuilder::TryAppendPlainOverlayStyleContent(PreparedContent& content,
                                                       PixelData        overlayStylePixelData,
                                                       Sampler          sampler,
                                                       const Size&      verifiedSize)
{
  if(content.textGradient.enabled ||
     content.textGradient.mixedTextGradient ||
     content.textGradient.styleTextureEnabled ||
     content.textGradient.overlayEnabled ||
     content.textGradient.overlayStyleTextureEnabled)
  {
    return false;
  }

  return TryAppendSimpleOverlayStyleContent(content, overlayStylePixelData, sampler, verifiedSize);
}

bool MarqueeBuilder::TryAppendSimpleOverlayStyleContent(PreparedContent& content,
                                                        PixelData        overlayStylePixelData,
                                                        Sampler          sampler,
                                                        const Size&      verifiedSize)
{
  if(content.textGradient.mixedTextGradient ||
     content.textGradient.overlayStyleTextureEnabled)
  {
    return false;
  }

  if(!IsPixelDataSizeEqual(overlayStylePixelData, verifiedSize))
  {
    return false;
  }

  AddTextScrollTexture(content.textureSet,
                       overlayStylePixelData,
                       sampler,
                       GetNextSimpleOverlayStyleTextureIndex(content.textGradient));
  content.textGradient.overlayStyleTextureEnabled = true;
  return true;
}

bool MarqueeBuilder::TryAppendMixedOverlayStyleContent(PreparedContent& content,
                                                       PixelData        overlayStylePixelData,
                                                       Sampler          sampler,
                                                       const Size&      verifiedSize)
{
  if(!content.textGradient.mixedTextGradient ||
     content.textGradient.overlayStyleTextureEnabled)
  {
    return false;
  }

  if(!IsPixelDataSizeEqual(overlayStylePixelData, verifiedSize))
  {
    return false;
  }

  AddTextScrollTexture(content.textureSet,
                       overlayStylePixelData,
                       sampler,
                       GetNextMixedOverlayStyleTextureIndex(content.textGradient));
  content.textGradient.overlayStyleTextureEnabled = true;
  return true;
}

void MarqueeBuilder::AddTextScrollTexture(TextureSet& textureSet,
                                          PixelData   data,
                                          Sampler     sampler,
                                          uint32_t    textureSetIndex)
{
  Texture texture = Texture::New(Dali::TextureType::TEXTURE_2D, data.GetPixelFormat(), data.GetWidth(), data.GetHeight());
  UploadTextScrollTexture(texture, data);
  textureSet.SetTexture(textureSetIndex, texture);
  textureSet.SetSampler(textureSetIndex, sampler);
}

bool MarqueeBuilder::IsPixelDataSizeEqual(PixelData data, const Size& size)
{
  return data &&
         data.GetWidth() == static_cast<uint32_t>(size.width) &&
         data.GetHeight() == static_cast<uint32_t>(size.height);
}

} // namespace Text
} // namespace Ui
} // namespace Dali
