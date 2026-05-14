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
#include <dali/public-api/object/property-index-ranges.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for AnimatedImageView.
 *
 * This struct is the single source of truth for all property indices.
 * It is shared between:
 *   - AnimatedImageView::Property  (public-api, for application code)
 *   - AnimatedImageViewImpl         (integration-api, for DALI_PROPERTY_REGISTRATION_EXTERNAL)
 */
struct AnimatedImageViewPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000, ///< Reserve property indices.

    ANIMATABLE_PROPERTY_START_INDEX = Ui::VIEW_ANIMATABLE_PROPERTY_END_INDEX + 1,
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_START_INDEX + 1000, ///< Reserve property indices.
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the AnimatedImageView class.
   */
  enum
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Image source
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the animated image resource (e.g. GIF, WebP).
     * @details Name "image", type Property::STRING.
     * @note See also: AnimatedImageView::SetResourceUrl(), AnimatedImageView::GetResourceUrl().
     */
    IMAGE = PROPERTY_START_INDEX,

    /**
     * @brief Array of image URLs for frame-by-frame animation.
     * @details Name "imageUrls", type Property::ARRAY of STRING.
     * @note Setting this overrides IMAGE. See also: AnimatedImageView::SetResourceUrls().
     */
    IMAGE_URLS,

    ///////////////////////////////////////////////////////////////////////////////
    // Playback
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The number of times to loop. -1 for infinite.
     * @details Name "loopCount", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetLoopCount(), AnimatedImageView::GetLoopCount().
     */
    LOOP_COUNT,

    /**
     * @brief The behavior when the animation is stopped.
     * @details Name "stopBehavior", type Ui::AnimatedImage::StopBehavior (Property::INTEGER).
     * @note See also: AnimatedImageView::SetStopBehavior(), AnimatedImageView::GetStopBehavior().
     */
    STOP_BEHAVIOR,

    /**
     * @brief Playback speed multiplier. 1.0 is normal speed.
     * @details Name "frameSpeedFactor", type Property::FLOAT.
     * @note See also: AnimatedImageView::SetFrameSpeedFactor(), AnimatedImageView::GetFrameSpeedFactor().
     */
    FRAME_SPEED_FACTOR,

    /**
     * @brief The number of frames to pre-load into the batch.
     * @details Name "batchSize", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetBatchSize(), AnimatedImageView::GetBatchSize().
     */
    BATCH_SIZE,

    /**
     * @brief The number of frames to keep in the cache.
     * @details Name "cacheSize", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetCacheSize(), AnimatedImageView::GetCacheSize().
     */
    CACHE_SIZE,

    /**
     * @brief The frame delay in milliseconds. -1 to use the value from the file.
     * @details Name "frameDelay", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetFrameDelay(), AnimatedImageView::GetFrameDelay().
     */
    FRAME_DELAY,

    ///////////////////////////////////////////////////////////////////////////////
    // Visual appearance
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The color to apply to the image.
     * @details Name "imageColor", type Property::VECTOR4.
     * @note See also: AnimatedImageView::SetImageColor(), AnimatedImageView::GetImageColor().
     */
    IMAGE_COLOR,

    /**
     * @brief Whether to apply pre-multiplied alpha to the rendered output.
     * @details Name "preMultipliedAlpha", type Property::BOOLEAN.
     * @note See also: AnimatedImageView::SetPreMultipliedAlpha(), AnimatedImageView::IsPreMultipliedAlpha().
     */
    PRE_MULTIPLIED_ALPHA,

    ///////////////////////////////////////////////////////////////////////////////
    // Size & Fitting
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief How the image is fitted within the view bounds.
     * @details Name "fittingMode", type Ui::Image::FittingMode (Property::INTEGER).
     * @note See also: AnimatedImageView::SetFittingMode(), AnimatedImageView::GetFittingMode().
     */
    FITTING_MODE,

    /**
     * @brief The sampling mode used when scaling the image.
     * @details Name "samplingMode", type Ui::Image::SamplingMode (Property::INTEGER).
     * @note See also: AnimatedImageView::SetSamplingMode(), AnimatedImageView::GetSamplingMode().
     */
    SAMPLING_MODE,

    ///////////////////////////////////////////////////////////////////////////////
    // Loading behavior
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The desired decoding width hint in pixels. 0 means no constraint.
     * @details Name "desiredWidth", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetDesiredWidth(), AnimatedImageView::GetDesiredWidth().
     */
    DESIRED_WIDTH,

    /**
     * @brief The desired decoding height hint in pixels. 0 means no constraint.
     * @details Name "desiredHeight", type Property::INTEGER.
     * @note See also: AnimatedImageView::SetDesiredHeight(), AnimatedImageView::GetDesiredHeight().
     */
    DESIRED_HEIGHT,

    /**
     * @brief The load policy for the image resource.
     * @details Name "loadPolicy", type Ui::Image::LoadPolicy (Property::INTEGER).
     * @note See also: AnimatedImageView::SetLoadPolicy(), AnimatedImageView::GetLoadPolicy().
     */
    LOAD_POLICY,

    /**
     * @brief Controls when the visual's texture resources are released.
     * @details Name "releasePolicy", type Ui::Image::ReleasePolicy (Property::INTEGER).
     * @note See also: AnimatedImageView::SetReleasePolicy(), AnimatedImageView::GetReleasePolicy().
     */
    RELEASE_POLICY,

    /**
     * @brief Whether to load the image synchronously on the event thread.
     * @details Name "synchronousLoading", type Property::BOOLEAN.
     * @note See also: AnimatedImageView::SetSynchronousLoading(), AnimatedImageView::IsSynchronousLoading().
     */
    SYNCHRONOUS_LOADING,

    /**
     * @brief Whether the image is loaded at the current view size.
     * @details Name "imageLoadWithViewSize", type Property::BOOLEAN.
     * @note See also: AnimatedImageView::SetImageLoadWithViewSize(), AnimatedImageView::IsImageLoadWithViewSizeEnabled().
     */
    IMAGE_LOAD_WITH_VIEW_SIZE,

    ///////////////////////////////////////////////////////////////////////////////
    // Masking & Placeholder
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the alpha mask image.
     * @details Name "alphaMaskUrl", type Property::STRING.
     * @note See also: AnimatedImageView::SetAlphaMaskUrl(), AnimatedImageView::GetAlphaMaskUrl().
     */
    ALPHA_MASK_URL,

    /**
     * @brief Whether the image is cropped to the mask bounds.
     * @details Name "cropToMask", type Property::BOOLEAN.
     * @note See also: AnimatedImageView::SetCropToMask(), AnimatedImageView::IsCropToMask().
     */
    CROP_TO_MASK,

    /**
     * @brief The masking mode.
     * @details Name "maskingMode", type Ui::Image::MaskingType (Property::INTEGER).
     * @note See also: AnimatedImageView::SetMaskingMode(), AnimatedImageView::GetMaskingMode().
     */
    MASKING_MODE,

    /**
     * @brief The URL of the placeholder image shown while loading.
     * @details Name "placeholderImage", type Property::STRING.
     * @note See also: AnimatedImageView::SetPlaceholderUrl(), AnimatedImageView::GetPlaceholderUrl().
     */
    PLACEHOLDER_IMAGE,

    ///////////////////////////////////////////////////////////////////////////////
    // Animatable
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The normalized sub-region of the image to display.
     * @details Name "pixelArea", type Property::VECTOR4 (animatable).
     * @note See also: AnimatedImageView::SetPixelArea(), AnimatedImageView::GetPixelArea().
     */
    PIXEL_AREA = ANIMATABLE_PROPERTY_START_INDEX,
  };
};

} // namespace Ui
} // namespace Dali
