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

#ifndef DALI_UI_FOUNDATION_INTERNAL_TEXT_MARQUEE_BUILDER_H
#define DALI_UI_FOUNDATION_INTERNAL_TEXT_MARQUEE_BUILDER_H

// EXTERNAL INCLUDES
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture-set.h>

#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/text-gradient-marquee-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/internal/text/text-model-interface.h>
#include <dali-ui-foundation/internal/text/text-scroller.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Builds Label marquee content resources used by TextScroller.
 *
 * This builder prepares texture, resource, and composition inputs only.
 * It does not own TextScroller renderer replacement, animation, or
 * stop/restart lifecycle.
 */
struct MarqueeBuilder
{
  struct FeatureInfo
  {
    bool hasMultipleTextColors{false};
    bool containsColorGlyph{false};
    bool styleTextureEnabled{false};
    bool isOverlayStyle{false};
    bool cutoutEnabled{false};
  };

  struct GradientState
  {
    bool baseRenderable{false};
    bool overlayRenderable{false};

    Internal::Gradient::Style baseStyle;
    Internal::Gradient::Style overlayStyle;

    bool baseStyleRenderable{false};
    bool overlayStyleRenderable{false};
  };

  struct AnimationState
  {
    Property::Index baseStartOffsetIndex{Property::INVALID_INDEX};
    bool            baseApplyAlways{false};

    Property::Index overlayStartOffsetIndex{Property::INVALID_INDEX};
    bool            overlayApplyAlways{false};
  };

  struct PreparedContent
  {
    TextureSet           textureSet;
    TextScrollerGradient textGradient;
  };

  struct ResolvedGradientBounds
  {
    Vector4 bounds{0.0f, 0.0f, 1.0f, 1.0f};
    Vector2 coordinateSize{Vector2::ZERO};
  };

  struct MixedGradientContentRequest
  {
    PixelData preservedPixelData;
    PixelData maskPixelData;
    Sampler   sampler;

    Size verifiedSize{Size::ZERO};

    GradientState                                gradientState;
    AnimationState                               animationState;
    Internal::GradientMarquee::CompositionPolicy compositionPolicy;
    ResolvedGradientBounds                       baseBounds;

    bool                   overlayBoundsResolved{false};
    ResolvedGradientBounds overlayBounds;
    GradientOverlayMode    overlayMode{GradientOverlayMode::SRC_OVER};

    bool      styleTextureEnabled{false};
    PixelData stylePixelData;
  };

  struct SimpleGradientContentRequest
  {
    GradientState  gradientState;
    AnimationState animationState;

    bool                   baseBoundsResolved{false};
    ResolvedGradientBounds baseBounds;

    bool                   overlayBoundsResolved{false};
    ResolvedGradientBounds overlayBounds;

    GradientOverlayMode overlayMode{GradientOverlayMode::SRC_OVER};
  };

  struct SimpleStyleContentRequest
  {
    PixelData fillPixelData;
    PixelData stylePixelData;
    Sampler   sampler;

    Size verifiedSize{Size::ZERO};

    GradientState                                gradientState;
    AnimationState                               animationState;
    Internal::GradientMarquee::CompositionPolicy compositionPolicy;

    bool                   baseBoundsResolved{false};
    ResolvedGradientBounds baseBounds;

    bool                   overlayBoundsResolved{false};
    ResolvedGradientBounds overlayBounds;

    GradientOverlayMode overlayMode{GradientOverlayMode::SRC_OVER};
  };

  struct CompositionRequest
  {
    Sampler sampler;
    Size    verifiedSize{Size::ZERO};

    FeatureInfo    featureInfo;
    bool           embossEnabled{false};
    GradientState  gradientState;
    AnimationState animationState;

    bool                   baseBoundsResolved{false};
    ResolvedGradientBounds baseBounds;

    bool                   overlayBoundsResolved{false};
    ResolvedGradientBounds overlayBounds;

    GradientOverlayMode overlayMode{GradientOverlayMode::SRC_OVER};
  };

  struct PixelDataBundle
  {
    PixelData fillPixelData;
    PixelData stylePixelData;
    PixelData preservedPixelData;
    PixelData maskPixelData;
    PixelData overlayStylePixelData;
  };

  struct CompositionPlan
  {
    bool needsBaseBounds{false};
    bool needsOverlayBounds{false};
    bool needsFillPixelData{false};
    bool needsStylePixelData{false};
    bool needsPreservedMaskPixelData{false};
    bool needsOverlayStylePixelData{false};

    bool HasWork() const
    {
      return needsBaseBounds ||
             needsOverlayBounds ||
             needsFillPixelData ||
             needsStylePixelData ||
             needsPreservedMaskPixelData ||
             needsOverlayStylePixelData;
    }
  };

  static bool HasOverlayStyle(const ModelInterface& textModel);

  static FeatureInfo CollectGradientFeatureInfo(const ModelInterface& textModel,
                                                bool                  textCutoutEnabled);

  static Sampler CreateTextScrollSampler(bool isHorizontal);

  static PreparedContent CreateTextContent(PixelData textPixelData,
                                           Sampler   sampler);

  static GradientState ResolveGradientState(const Dali::Ui::Gradient::Base& baseGradient,
                                            const Dali::Ui::Gradient::Base& overlayGradient,
                                            const Size&                     verifiedSize);

  static Internal::GradientMarquee::CompositionPolicy CreateGradientCompositionPolicy(const FeatureInfo&   featureInfo,
                                                                                      bool                 embossEnabled,
                                                                                      const GradientState& gradientState);

  static void ApplyBaseAnimationState(TextScrollerGradient& textGradient,
                                      const AnimationState& animationState);

  static void ApplyOverlayAnimationState(TextScrollerGradient& textGradient,
                                         const AnimationState& animationState);

  static bool TryApplyMixedGradientContent(PreparedContent&                   content,
                                           const MixedGradientContentRequest& request);

  static bool TryApplySimpleStyleContent(PreparedContent&                 content,
                                         const SimpleStyleContentRequest& request);

  static void ApplySimpleGradientContent(PreparedContent&                    content,
                                         const SimpleGradientContentRequest& request);

  static CompositionPlan GetCompositionPlan(const CompositionRequest& request);

  static void ApplyPreparedComposition(PreparedContent&          content,
                                       const CompositionRequest& request,
                                       const PixelDataBundle&    pixels);

  static bool TryAppendPlainOverlayStyleContent(PreparedContent& content,
                                                PixelData        overlayStylePixelData,
                                                Sampler          sampler,
                                                const Size&      verifiedSize);

  static bool TryAppendSimpleOverlayStyleContent(PreparedContent& content,
                                                 PixelData        overlayStylePixelData,
                                                 Sampler          sampler,
                                                 const Size&      verifiedSize);

  static bool TryAppendMixedOverlayStyleContent(PreparedContent& content,
                                                PixelData        overlayStylePixelData,
                                                Sampler          sampler,
                                                const Size&      verifiedSize);

  static void AddTextScrollTexture(TextureSet& textureSet,
                                   PixelData   data,
                                   Sampler     sampler,
                                   uint32_t    textureSetIndex);

  static bool IsPixelDataSizeEqual(PixelData data, const Size& size);
};

} // namespace Text
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_FOUNDATION_INTERNAL_TEXT_MARQUEE_BUILDER_H
