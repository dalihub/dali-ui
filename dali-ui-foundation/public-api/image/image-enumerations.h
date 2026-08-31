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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/image-options.h>
#include <cstdint>

namespace Dali
{

namespace Ui
{

namespace Image
{
/**
 * @brief FittingMode configures how the image fits within the defined bounds.
 */
enum class FittingMode : uint8_t
{
  FIT_KEEP_ASPECT_RATIO,      ///< The visual should be scaled to fit, preserving aspect ratio
  FILL,                       ///< The visual should be stretched to fill, not preserving aspect ratio
  OVER_FIT_KEEP_ASPECT_RATIO, ///< The visual should be scaled to fit, preserving aspect ratio, outside is cropped away
  CENTER,                     ///< Keeps the original image size when it fits on both axes; otherwise scales down preserving aspect ratio
};

/**
 * @brief MaskingType configures the alpha clipping structure of the image.
 */
enum class MaskingType : uint8_t
{
  MASKING_ON_RENDERING, ///< Masking is applied to rendering phase.
  MASKING_ON_LOADING    ///< Masking is applied to loading phase.
};

/**
 * @brief SamplingMode is an alias for Dali::SamplingMode, configuring the filter applied when scaling the image.
 */
enum class SamplingMode : uint8_t
{
  BOX              = Dali::SamplingMode::BOX,
  NEAREST          = Dali::SamplingMode::NEAREST,
  LINEAR           = Dali::SamplingMode::LINEAR,
  BOX_THEN_NEAREST = Dali::SamplingMode::BOX_THEN_NEAREST,
  BOX_THEN_LINEAR  = Dali::SamplingMode::BOX_THEN_LINEAR,
  NO_FILTER        = Dali::SamplingMode::NO_FILTER,
  DONT_CARE        = Dali::SamplingMode::DONT_CARE,
  LANCZOS          = Dali::SamplingMode::LANCZOS,
  BOX_THEN_LANCZOS = Dali::SamplingMode::BOX_THEN_LANCZOS,

  DEFAULT = Dali::SamplingMode::DEFAULT,
};

/**
 * @brief LoadPolicy controls when the image is loaded.
 */
enum class LoadPolicy : uint8_t
{
  IMMEDIATE = 0, ///< The image is loaded when the ImageView is created.
  ATTACHED,      ///< The image is loaded when the ImageView is attached to the scene.
};

/**
 * @brief ReleasePolicy controls when the image texture is released from the cache.
 */
enum class ReleasePolicy : uint8_t
{
  DETACHED = 0, ///< Image released from cache when the visual is detached from the scene.
  DESTROYED,    ///< Image released from cache when the visual is destroyed.
  NEVER,        ///< Image is not automatically released from cache. Unused resources may be explicitly released
                ///< through Dali::Ui::ImageCacheUtils::ClearUnusedTextures().
};

} // namespace Image

} // namespace Ui

} // namespace Dali
