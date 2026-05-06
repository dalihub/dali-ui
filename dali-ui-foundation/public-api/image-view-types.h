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

#include <dali/public-api/adaptor-framework/image-options.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/int-pair.h>

namespace Dali
{
namespace Ui
{
/**
 * @brief FittingMode configures how the image fits within the defined bounds.
 */
namespace FittingMode
{
enum Type
{
  FIT_KEEP_ASPECT_RATIO,      ///< The visual should be scaled to fit, preserving aspect ratio
  FILL,                       ///< The visual should be stretched to fill, not preserving aspect ratio
  OVER_FIT_KEEP_ASPECT_RATIO, ///< The visual should be scaled to fit, preserving aspect ratio, outside is cropped away
  CENTER,                     ///< The visual should keep original size of image
};
} // namespace FittingMode

/**
 * @brief MaskingType configures the alpha clipping structure of the image.
 */
namespace MaskingType
{
enum Type
{
  MASKING_ON_RENDERING, ///< Masking is applied to rendering phase.
  MASKING_ON_LOADING    ///< Masking is applied to loading phase.
};
} // namespace MaskingType

/**
 * @brief SamplingMode is an alias for Dali::SamplingMode, configuring the filter applied when scaling the image.
 */
namespace SamplingMode = Dali::SamplingMode;

/**
 * @brief LoadPolicy controls when the image is loaded.
 */
namespace LoadPolicy
{
enum Type
{
  IMMEDIATE = 0, ///< The image is loaded when the ImageView is created.
  ATTACHED       ///< The image is loaded when the ImageView is attached to the scene.
};
} // namespace LoadPolicy

/**
 * @brief ReleasePolicy controls when the image texture is released from the cache.
 */
namespace ReleasePolicy
{
enum Type
{
  DETACHED = 0, ///< Image released from cache when the visual is detached from the scene.
  DESTROYED,    ///< Image released from cache when the visual is destroyed.
  NEVER         ///< Image is never released from cache.
};
} // namespace ReleasePolicy

} // namespace Ui
} // namespace Dali
