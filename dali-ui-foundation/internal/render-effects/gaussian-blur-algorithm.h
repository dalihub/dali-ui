#ifndef DALI_UI_INTERNAL_BLUR_ALGORITHM_H
#define DALI_UI_INTERNAL_BLUR_ALGORITHM_H

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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
/**
 * @brief A utility file to generate gaussian kernel.
 * @note Every function is static. Do NOT instantiate this class.
 */
class GaussianBlurAlgorithm
{
public:
  /**
   * @brief Checks the kernel range before any fixed-size cache access.
   *
   * Zero-sample kernels are unsupported. Valid odd radii retain the existing
   * integer sample-count conversion; invalid inputs are not clamped.
   */
  static bool IsSupportedRadius(uint32_t blurRadius);

  /**
   * @brief Creates gaussian blur renderer.
   * @param[in] blurRadius Blur intensity
   * @return Gaussian blur renderer
   */
  static Dali::Renderer CreateRenderer(const uint32_t blurRadius);

  /**
   * @brief Get cached fragment shader with given blurRadius.
   * @param[in] blurRadius Blur intensity
   * @return Gaussian blur shader
   */
  static Dali::Shader& GetShader(const uint32_t blurRadius);

  /**
   * @brief Creates an uncached shader with the shared Gaussian kernel.
   *
   * Prepends NUM_SAMPLES (blurRadius >> 1) to the fragment source and connects
   * GaussianBlurSampleBlock at creation. The source uses uSampleOffsets and
   * uSampleWeights arrays of that size and must not redefine NUM_SAMPLES.
   * The kernel cache remains internal; no uniform block handle is exposed.
   * Callers own shader caching and any renderer-specific properties.
   *
   * @param[in] blurRadius Radius using the range accepted by IsSupportedRadius()
   * @param[in] vertexSource Vertex shader source
   * @param[in] fragmentSource Fragment shader source without the sample-count definition
   * @param[in] hints Shader hints
   * @param[in] shaderName Shader name used for identification and file caching
   * @return A new shader connected to the cached Gaussian kernel
   */
  static Dali::Shader CreateShader(uint32_t                  blurRadius,
                                   Dali::StringView          vertexSource,
                                   Dali::StringView          fragmentSource,
                                   Dali::Shader::Hint::Value hints,
                                   Dali::StringView          shaderName);

  /**
   * @brief Gets blur radius in a downscaled size. If the value is too big, fit arguments in desired range.
   * @param[in] downscaleFactor Reference value of downscale factor.
   * @param[in] blurRadius Reference value of blur radius.
   * @return Downscaled(optimized) blur radius
   */
  static uint32_t GetDownscaledBlurRadius(float& downscaleFactor, uint32_t& blurRadius);
};
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
#endif // DALI_UI_INTERNAL_BLUR_ALGORITHM_H
