#ifndef DALI_UI_INTERNAL_TEXT_REVEAL_BLUR_RENDERER_H
#define DALI_UI_INTERNAL_TEXT_REVEAL_BLUR_RENDERER_H

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

#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/renderer.h>
#include <cstdint>

namespace DALI_NAMESPACE::Ui::Internal::TextRevealBlurRenderer
{
/**
 * @brief Bounds vertex uniform storage per draw, independently of page packing.
 *
 * Larger pages use additional draws in the same tasks and framebuffers.
 */
constexpr uint32_t MAX_LINES_PER_DRAW = 64u;

/**
 * @brief Creates a scalar Reveal blur pass using the shared Gaussian kernel.
 *
 * The caller binds the source texture/sampler and update-side timing properties.
 * This factory creates no actors, framebuffers or render tasks.
 */
Renderer Create(uint32_t blurRadius);

/**
 * @brief Creates a page draw with line-local sampling and CPU blur strengths.
 *
 * Geometry carries aPosition, aTexCoord, aRevealRectangle, aRevealInverseSize and
 * aRevealLineIndex. The caller supplies at most MAX_LINES_PER_DRAW line bindings
 * in uRevealBlurState and binds the page texture/sampler.
 */
Renderer CreateBatch(uint32_t blurRadius, Geometry geometry);
} //namespace DALI_NAMESPACE::Ui::Internal::TextRevealBlurRenderer

#endif // DALI_UI_INTERNAL_TEXT_REVEAL_BLUR_RENDERER_H
