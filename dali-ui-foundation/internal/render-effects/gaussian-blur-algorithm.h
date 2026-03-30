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
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>

namespace Dali
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
  static Dali::Shader& GetGaussianBlurShader(const uint32_t blurRadius);

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
} // namespace Dali
#endif // DALI_UI_INTERNAL_BLUR_ALGORITHM_H
