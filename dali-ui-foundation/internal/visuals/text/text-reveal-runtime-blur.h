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

#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali-ui-foundation/internal/text/reveal/text-reveal-blur-preparation.h>
#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace DALI_NAMESPACE::Ui::Internal
{
/**
 * @brief Selects the runtime sampling path independently of Reveal timing.
 *
 * FULL_RESOLUTION retains the original source/H/V pipeline. AXIS_AWARE_QUARTER
 * reduces X in H, then Y in V, and blends back to the full-resolution source.
 * The path is resolved when publishing the renderer, independently of CPU
 * raster preparation and the authored Reveal timeline.
 */
enum class RuntimeRevealBlurPath
{
  FULL_RESOLUTION,
  AXIS_AWARE_QUARTER
};

struct RuntimeRevealBlurSettings
{
  RuntimeRevealBlurPath path{RuntimeRevealBlurPath::FULL_RESOLUTION};
  float                 authoredRadius{0.0f}; ///< Display-pixel radius from the validated publication, before kernel rounding.
};

/**
 * @brief Resolves the creation-time sampling policy for a validated publication.
 *
 * PERFORMANCE uses axis-aware quarter resolution at every radius. QUALITY keeps
 * the full-resolution pipeline. The display radius controls the sharp handoff,
 * not the choice of sampling path.
 */
RuntimeRevealBlurSettings ResolveRuntimeRevealBlurSettings(Ui::Integration::Text::Reveal::BlurMode mode, float authoredRadius);

/**
 * @brief Keeps one final line's source textures, rectangle and sequence start.
 *
 * The start comes from the final Reveal plan, including PIXEL progression.
 * It is not estimated from line height, animation duration, or event-side time.
 */
struct RuntimeRevealBlurSequence : RevealBlurSequence
{
  TextureSet textures;
  Shader     atlasShader;    ///< Empty for the isolated-texture source path.
  Vector4    atlasRectangle; ///< Normalized interior UV, excluding replicated edges.
};

/**
 * @brief Separates already published decoration planes from blur foreground.
 *
 * Only decorated blur allocates this state. Composition applies owner color
 * once, after background, blurred foreground and overlay have been combined.
 */
struct RuntimeRevealBlurDecorations
{
  Shader     foregroundShader;
  TextureSet foregroundTextures;
  TextureSet background;
  TextureSet overlay;
};

/**
 * @brief Supplies request-local image slots without decoded resource handles.
 */
struct RuntimeRevealBlurImages
{
  uint64_t                     sourceRevision{0u};
  std::vector<RevealBlurImage> placements;
};

/**
 * @brief Describes consecutive, independently sampled lines in one blur page.
 *
 * The three render tasks belong to the page, not the individual lines.
 * The allocated size may include padding to share source/H scratch targets.
 */
struct RuntimeRevealBlurBatch
{
  size_t  first{0u};
  size_t  count{0u};
  Vector2 size;
};

/**
 * @brief Packs independently timed lines within texture and page limits.
 *
 * The whole Label uses one page when its area and occupancy allow it.
 * Otherwise, balanced contiguous partitions may replace the four-line fallback
 * only without increasing page count or FBO storage, including shared scratch.
 * Common page extents may add bounded padding when sharing lowers that storage.
 * An oversized single target is unchanged; its dimensions are checked by publication.
 * Sizes are positive integer pixel dimensions from the resolved capture bounds.
 */
std::vector<RuntimeRevealBlurBatch> BuildRuntimeRevealBlurBatches(const std::vector<Vector2>& sizes, uint32_t maxTextureSize);

/**
 * @brief Maps coverage into an integer subrectangle of the original blur target.
 *
 * Coverage uses the original full-size foreground coordinates. A transparent
 * guard includes the Gaussian support and bilinear/pixel-snap margins.
 * horizontalBand optionally receives the source's filtered Y coverage in the
 * same full-target coordinates. Unusable coverage returns the full Y extent.
 */
Rect<int32_t> ResolveRuntimeRevealBlurTarget(Renderer foreground, const Vector2& controlSize,
                                             const Vector2& textureSize, const Rect<uint32_t>& coverage,
                                             uint32_t radius, Vector2* horizontalBand = nullptr);

/**
 * @brief Moves the resolved foreground renderer into exclusive source/H/V tasks.
 *
 * Foreground visibility and blur strength read the same update-side progress.
 * The plan is already normalized; public progress directly drives both
 * foreground metadata and blur intervals. Kernel weights remain fixed.
 * The companion owns its tasks and transparent halo, not the Label's effect slot.
 * singleColor must come from resolved TextVisual features (no gradients, color
 * glyphs, multicolor or cutout). It only permits the backend-gated alpha path.
 * An unavailable owner scene returns an empty handle without retiring the
 * original renderer. Failed activation restores its borrowed bindings.
 */
Actor CreateRuntimeRevealBlur(Actor owner, Renderer foreground, const Vector2& size,
                              Property::Index progressIndex, uint32_t radius, float blurDuration,
                              std::vector<RuntimeRevealBlurSequence>        sequences   = {},
                              bool                                          singleColor = false,
                              std::unique_ptr<RuntimeRevealBlurDecorations> decorations = {},
                              std::unique_ptr<RuntimeRevealBlurImages>      images      = {},
                              RuntimeRevealBlurSettings                     settings    = {});

/**
 * @brief Builds detached resources without borrowing the published renderer.
 *
 * The caller must revalidate its publication after allocation and own the
 * candidate before ActivateRuntimeRevealBlur(), which can reenter on attach.
 */
Actor PrepareRuntimeRevealBlur(Actor owner, Renderer foreground, const Vector2& size,
                               Property::Index progressIndex, uint32_t radius, float blurDuration,
                               std::vector<RuntimeRevealBlurSequence> sequences, bool singleColor,
                               std::unique_ptr<RuntimeRevealBlurDecorations> decorations,
                               std::unique_ptr<RuntimeRevealBlurImages>      images,
                               RuntimeRevealBlurSettings                     settings = {});

/**
 * @brief Borrows the foreground and connects a previously validated candidate.
 */
bool ActivateRuntimeRevealBlur(Actor companion, Actor owner);

/**
 * @brief Checks that the runtime still owns its installed foreground bindings.
 */
bool HasCurrentRuntimeRevealBlurForeground(Actor companion);

/**
 * @brief Connects current ready images to reserved capture slots without rasterization.
 */
void RefreshRuntimeRevealBlurImages(Actor companion);

/**
 * @brief Restores the source renderer before ordinary TextVisual replacement/removal.
 */
void RemoveRuntimeRevealBlur(Actor& companion, Actor owner);
} //namespace DALI_NAMESPACE::Ui::Internal
