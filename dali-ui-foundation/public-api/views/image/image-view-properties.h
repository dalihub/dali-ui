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
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for ImageView.
 *
 * This struct is the single source of truth for all property indices.
 * It is shared between:
 *   - ImageView::Property  (public-api, for application code)
 *   - ImageViewImpl         (integration-api, for DALI_PROPERTY_REGISTRATION_EXTERNAL)
 */
struct ImageViewPropertyIndex
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
   * @brief Enumeration for the instance of properties belonging to the ImageView class.
   */
  enum
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Image source
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the image resource.
     * @details Name "image", type Property::STRING.
     * @note See also: ImageView::SetResourceUrl(), ImageView::GetResourceUrl().
     */
    IMAGE = PROPERTY_START_INDEX,

    ///////////////////////////////////////////////////////////////////////////////
    // Size & Fitting
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The fitting mode used when scaling the image.
     * @details Name "fittingMode", type Ui::Image::FittingMode (Property::INTEGER).
     * @note See also: ImageView::SetFittingMode(), ImageView::GetFittingMode().
     */
    FITTING_MODE,

    /**
     * @brief The sampling mode used when scaling the image.
     * @details Name "samplingMode", type Ui::Image::SamplingMode (Property::INTEGER).
     * @note See also: ImageView::SetSamplingMode(), ImageView::GetSamplingMode().
     */
    SAMPLING_MODE,

    ///////////////////////////////////////////////////////////////////////////////
    // Visual appearance
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The color multiplier applied to the image.
     * @details Name "imageColor", type Property::VECTOR4.
     * @note See also: ImageView::SetImageColor(), ImageView::GetImageColor().
     */
    IMAGE_COLOR,

    /**
     * @brief Whether the image uses pre-multiplied alpha.
     * @details Name "preMultipliedAlpha", type Property::BOOLEAN.
     * @note See also: ImageView::SetPreMultipliedAlpha(), ImageView::IsPreMultipliedAlpha().
     */
    PRE_MULTIPLIED_ALPHA,

    ///////////////////////////////////////////////////////////////////////////////
    // Placeholder
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the placeholder image shown while loading.
     * @details Name "placeholderImage", type Property::STRING.
     * @note See also: ImageView::SetPlaceholderUrl(), ImageView::GetPlaceholderUrl().
     */
    PLACEHOLDER_IMAGE,

    ///////////////////////////////////////////////////////////////////////////////
    // Masking
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the alpha mask image.
     * @details Name "alphaMaskUrl", type Property::STRING.
     * @note See also: ImageView::SetAlphaMaskUrl(), ImageView::GetAlphaMaskUrl().
     */
    ALPHA_MASK_URL,

    /**
     * @brief Whether the image is cropped to the mask bounds.
     * @details Name "cropToMask", type Property::BOOLEAN.
     * @note See also: ImageView::SetCropToMask(), ImageView::IsCropToMask().
     */
    CROP_TO_MASK,

    /**
     * @brief The masking mode.
     * @details Name "maskingMode", type Ui::Image::MaskingType (Property::INTEGER).
     * @note See also: ImageView::SetMaskingMode(), ImageView::GetMaskingMode().
     */
    MASKING_MODE,

    ///////////////////////////////////////////////////////////////////////////////
    // Loading behavior
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The desired image width hint for the image loader, in pixels.
     * @details Name "desiredWidth", type Property::INTEGER.
     * @note See also: ImageView::SetDesiredWidth(), ImageView::GetDesiredWidth().
     */
    DESIRED_WIDTH,

    /**
     * @brief The desired image height hint for the image loader, in pixels.
     * @details Name "desiredHeight", type Property::INTEGER.
     * @note See also: ImageView::SetDesiredHeight(), ImageView::GetDesiredHeight().
     */
    DESIRED_HEIGHT,

    /**
     * @brief The load policy for the image resource.
     * @details Name "loadPolicy", type Ui::Image::LoadPolicy (Property::INTEGER).
     * @note See also: ImageView::SetLoadPolicy(), ImageView::GetLoadPolicy().
     */
    LOAD_POLICY,

    /**
     * @brief The release policy for the image resource.
     * @details Name "releasePolicy", type Ui::Image::ReleasePolicy (Property::INTEGER).
     * @note See also: ImageView::SetReleasePolicy(), ImageView::GetReleasePolicy().
     */
    RELEASE_POLICY,

    /**
     * @brief Whether the image is loaded synchronously on the main thread.
     * @details Name "synchronousLoading", type Property::BOOLEAN.
     * @note See also: ImageView::SetSynchronousLoading(), ImageView::IsSynchronousLoading().
     */
    SYNCHRONOUS_LOADING,

    /**
     * @brief Whether the view size is used as the image load size.
     * @details Name "imageLoadWithViewSize", type Property::BOOLEAN.
     * @note See also: ImageView::SetImageLoadWithViewSize(), ImageView::IsImageLoadWithViewSizeEnabled().
     */
    IMAGE_LOAD_WITH_VIEW_SIZE,

    /**
     * @brief Whether fast-track GPU uploading is enabled.
     * @details Name "fastTrackUploading", type Property::BOOLEAN.
     * @note See also: ImageView::SetFastTrackUpload(), ImageView::IsFastTrackUploadEnabled().
     */
    FAST_TRACK_UPLOADING,

    /**
     * @brief Whether EXIF orientation metadata is applied automatically.
     * @details Name "orientationCorrection", type Property::BOOLEAN.
     * @note See also: ImageView::SetOrientationCorrection(), ImageView::IsOrientationCorrectionEnabled().
     */
    ORIENTATION_CORRECTION,

    ///////////////////////////////////////////////////////////////////////////////
    // N-Patch
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The N-patch border insets as (left, top, right, bottom).
     * @details Name "nPatchBorder", type Property::VECTOR4.
     * @note See also: ImageView::SetNPatchBorder(), ImageView::GetNPatchBorder().
     */
    N_PATCH_BORDER,

    /**
     * @brief Whether only the N-patch border regions are rendered.
     * @details Name "nPatchBorderOnly", type Property::BOOLEAN.
     * @note See also: ImageView::SetNPatchBorderOnly(), ImageView::IsNPatchBorderOnly().
     */
    N_PATCH_BORDER_ONLY,

    ///////////////////////////////////////////////////////////////////////////////
    // Animatable
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The normalized sub-region of the image to display.
     * @details Name "pixelArea", type Property::VECTOR4 (animatable).
     * @note See also: ImageView::SetPixelArea(), ImageView::GetPixelArea().
     */
    PIXEL_AREA = ANIMATABLE_PROPERTY_START_INDEX,
  };
};

} // namespace Ui
} // namespace Dali
