#pragma once

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

#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/math/rect.h>
#include <memory>
#include <vector>

namespace DALI_NAMESPACE::Ui::Internal
{
/**
 * @brief Resolves the shared shader, halo and target radius before preparation.
 *
 * The input is in displayed pixels after UI scaling, not raster supersampling.
 * Keep two Gaussian samples even for small radii, as BlurEffect does. Returns
 * zero outside the existing Gaussian kernel capacity, for ordinary fallback.
 */
uint32_t ResolveRevealBlurRadius(float radius);

/**
 * @brief Copyable final-line geometry and timing without scene-graph handles.
 *
 * lineIndex refers to the same final layout as replacement placements. Image
 * occurrences can therefore join this sequence without inferring lines from Y.
 */
struct RevealBlurSequence
{
  uint32_t       lineIndex{0u};
  float          start{0.0f};
  bool           hasTextForeground{true};
  Rect<uint32_t> coverage;
  Vector4        textureRect{0.0f, 0.0f, 1.0f, 1.0f};
};

/**
 * @brief Selects CPU planes using the resolved foreground feature combination.
 *
 * Capability checks run before preparation. No renderer, FontClient or model
 * from another request is accessed; the caller supplies its own Typesetter.
 */
struct RevealBlurPreparationOptions
{
  Vector2             rasterSize;
  Vector2             controlSize;
  uint32_t            radius{0u};
  uint32_t            maxTextureSize{0u};
  float               durationRatio{1.0f};
  float               stagger{0.0f};
  Pixel::Format       foregroundFormat{Pixel::L8};
  Ui::Text::Direction textDirection{Ui::Text::Direction::LEFT_TO_RIGHT};
  bool                perLine{false};
  bool                gradientMixed{false};
  bool                colorMask{false};
  bool                decorations{false};
};

/**
 * @brief Keeps the cropped planes of one sequence until event-thread upload.
 */
struct RevealBlurLineRaster
{
  RevealBlurSequence sequence;
  PixelData          foreground;
  PixelData          mask;
  PixelData          metadata;
};

/**
 * @brief Reserves a visible image's capture area before its resource is ready.
 *
 * Coverage uses the same raster coordinates as text. Occurrence and final line
 * identities come from this request's replacement placements, never from I/O.
 */
struct RevealBlurImage
{
  uint64_t       occurrenceIdentity{0u};
  uint32_t       lineIndex{0u};
  Rect<uint32_t> coverage;
};

/**
 * @brief Immutable optional CPU result for synchronous and async publication.
 *
 * The ordinary result remains independent for fallback. PixelData handles may
 * be copied, but their buffers must not be modified after this is published.
 * Runtime actors/renderers are deliberately absent.
 */
struct PreparedRevealBlur
{
  RevealBlurPreparationOptions              options;
  std::vector<RevealBlurLineRaster>         lines;
  PixelData                                 metadata;
  Vector<Ui::Text::ReplacementRevealTiming> timings;
  std::vector<RevealBlurImage>              images;
  Vector2                                   planTiming;
  float                                     blurDuration{0.0f};
  float                                     fadeDuration{0.0f};
};

/**
 * @brief Reads final lines and their existing schedule before blur normalization.
 */
std::vector<RevealBlurSequence> BuildRevealBlurSequences(
  Ui::Text::Typesetter& typesetter, const Ui::Text::Internal::Reveal::Plan& plan);

/**
 * @brief Reads actual Reveal completion, including PIXEL spatial progression.
 */
float GetRuntimeRevealEndProgress(const Ui::Text::Internal::Reveal::Plan& plan);

/**
 * @brief Reads the last text-bearing blur sequence start and actual Reveal end.
 */
Vector2 GetRevealBlurPlanTiming(Ui::Text::Typesetter& typesetter, const Ui::Text::Internal::Reveal::Plan& plan,
                                const std::vector<RevealBlurSequence>& sequences);

/**
 * @brief Accumulates actual foreground coverage without inspecting glyph metrics.
 *
 * Blur preparation inspects text/mask planes before upload, retaining coverage
 * from overlapping lines and color-glyph overhangs.
 */
void AccumulateRuntimeRevealBlurCoverage(PixelData pixels, Rect<uint32_t>& coverage);

/**
 * @brief Copies a plane rectangle without modifying encoded metadata timing.
 *
 * All line planes use the same rectangle, including transparent filtering support.
 * Invalid buffers or rectangles fail before the ordinary publication is replaced.
 */
PixelData CropRuntimeRevealBlurPixels(PixelData pixels, const Rect<uint32_t>& rectangle);

/**
 * @brief Prepares one complete blur result, or returns empty for ordinary fallback.
 *
 * The passed plan and timing values are private copies. Preparation never changes
 * the caller's ordinary metadata, schedule or replacement publication.
 */
std::shared_ptr<const PreparedRevealBlur> PrepareRevealBlur(
  Ui::Text::Typesetter& typesetter, Ui::Text::Internal::Reveal::Plan plan,
  const Vector<Ui::Text::ReplacementRevealTiming>& timings,
  const RevealBlurPreparationOptions&              options,
  const Vector<Ui::Text::ReplacementPlacement>*    placements = nullptr);
} // namespace Dali::Ui::Internal
