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

namespace Dali
{
namespace Ui
{

/**
 * @brief Property index definitions for LottieAnimationView.
 *
 * This struct is the single source of truth for all property indices.
 * It is shared between:
 *   - LottieAnimationView::Property  (public-api, for application code)
 *   - LottieAnimationViewImpl         (integration-api, for DALI_PROPERTY_REGISTRATION_EXTERNAL)
 */
struct LottieAnimationViewPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = PROPERTY_REGISTRATION_START_INDEX,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000, ///< Reserve property indices.
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the LottieAnimationView class.
   */
  enum
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Image source
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the Lottie JSON animation file.
     * @details Name "image", type Property::STRING.
     * @note See also: LottieAnimationView::SetResourceUrl(), LottieAnimationView::GetResourceUrl().
     */
    IMAGE = PROPERTY_START_INDEX,

    ///////////////////////////////////////////////////////////////////////////////
    // Playback
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The number of times to loop. -1 for infinite.
     * @details Name "loopCount", type Property::INTEGER.
     * @note See also: LottieAnimationView::SetLoopCount(), LottieAnimationView::GetLoopCount().
     */
    LOOP_COUNT,

    /**
     * @brief The looping mode (RESTART or AUTO_REVERSE).
     * @details Name "loopingMode", type Ui::LottieAnimation::LoopingMode (Property::INTEGER).
     * @note See also: LottieAnimationView::SetLoopingMode(), LottieAnimationView::GetLoopingMode().
     */
    LOOPING_MODE,

    /**
     * @brief The behavior when the animation is stopped.
     * @details Name "stopBehavior", type Ui::AnimatedImage::StopBehavior (Property::INTEGER).
     * @note See also: LottieAnimationView::SetStopBehavior(), LottieAnimationView::GetStopBehavior().
     */
    STOP_BEHAVIOR,

    /**
     * @brief Playback speed multiplier. 1.0 is normal speed.
     * @details Name "frameSpeedFactor", type Property::FLOAT.
     * @note See also: LottieAnimationView::SetFrameSpeedFactor(), LottieAnimationView::GetFrameSpeedFactor().
     */
    FRAME_SPEED_FACTOR,

    ///////////////////////////////////////////////////////////////////////////////
    // Visual appearance
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The color tint to apply to the rendered animation.
     * @details Name "imageColor", type Property::VECTOR4.
     * @note See also: LottieAnimationView::SetImageColor(), LottieAnimationView::GetImageColor().
     */
    IMAGE_COLOR,

    ///////////////////////////////////////////////////////////////////////////////
    // Loading behavior
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The desired decoding width hint in pixels. 0 means no constraint.
     * @details Name "desiredWidth", type Property::INTEGER.
     * @note See also: LottieAnimationView::SetDesiredWidth(), LottieAnimationView::GetDesiredWidth().
     */
    DESIRED_WIDTH,

    /**
     * @brief The desired decoding height hint in pixels. 0 means no constraint.
     * @details Name "desiredHeight", type Property::INTEGER.
     * @note See also: LottieAnimationView::SetDesiredHeight(), LottieAnimationView::GetDesiredHeight().
     */
    DESIRED_HEIGHT,

    /**
     * @brief Controls when the visual's texture resources are released.
     * @details Name "releasePolicy", type Ui::Image::ReleasePolicy (Property::INTEGER).
     * @note See also: LottieAnimationView::SetReleasePolicy(), LottieAnimationView::GetReleasePolicy().
     */
    RELEASE_POLICY,

    /**
     * @brief Whether to load the animation synchronously on the event thread.
     * @details Name "synchronousLoading", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetSynchronousLoading(), LottieAnimationView::IsSynchronousLoading().
     */
    SYNCHRONOUS_LOADING,

    /**
     * @brief Whether to apply pre-multiplied alpha to the rendered output.
     * @details Name "preMultipliedAlpha", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetPreMultipliedAlpha(), LottieAnimationView::IsPreMultipliedAlpha().
     */
    PRE_MULTIPLIED_ALPHA,

    ///////////////////////////////////////////////////////////////////////////////
    // Rasterization tuning
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Whether to redraw every frame when the view is being scaled down.
     * @details Name "redrawInScalingDown", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetRedrawOnScaleDown(), LottieAnimationView::IsRedrawOnScaleDown().
     */
    REDRAW_IN_SCALING_DOWN,

    /**
     * @brief Whether to redraw every frame when the view is being scaled up.
     * @details Name "redrawInScalingUp", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetRedrawOnScaleUp(), LottieAnimationView::IsRedrawOnScaleUp().
     */
    REDRAW_IN_SCALING_UP,

    /**
     * @brief Whether to cache each rasterized frame to avoid re-rasterization.
     * @details Name "enableFrameCache", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetEnableFrameCache(), LottieAnimationView::IsFrameCacheEnabled().
     */
    ENABLE_FRAME_CACHE,

    /**
     * @brief Whether to notify ResourceReadySignal after each rasterization completes.
     * @details Name "notifyAfterRasterization", type Property::BOOLEAN.
     * @note See also: LottieAnimationView::SetNotifyAfterRasterization(), LottieAnimationView::IsNotifyAfterRasterizationEnabled().
     */
    NOTIFY_AFTER_RASTERIZATION,

    /**
     * @brief Scale factor applied to the rasterization size relative to the visual size.
     * @details Name "renderScale", type Property::FLOAT.
     *          A value of 2.0 rasterizes at twice the visual dimensions for sharper output
     *          on high-density displays. Negative values flip the image.
     *          Default is 1.0 (rasterize at natural visual size).
     * @note See also: LottieAnimationView::SetRenderScale(), LottieAnimationView::GetRenderScale().
     */
    RENDER_SCALE,

    /**
     * @brief Whether to enable aspect fit for AnimatedVectorImageVisual.
     * @details Name "enableAspectFit", type Property::BOOLEAN.
     *          If this property is true, the vector image will be scaled to fit within the visual size
     *          while maintaining the aspect ratio. If false, the image will be stretched to fill the visual size.
     * @note See also: LottieAnimationView::SetEnableAspectFit(), LottieAnimationView::IsEnableAspectFit().
     */
    ENABLE_ASPECT_FIT,

    ///////////////////////////////////////////////////////////////////////////////
    // Placeholder
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The URL of the placeholder image shown while loading.
     * @details Name "placeholderImage", type Property::STRING.
     * @note See also: LottieAnimationView::SetPlaceholderUrl(), LottieAnimationView::GetPlaceholderUrl().
     */
    PLACEHOLDER_IMAGE,

    ///////////////////////////////////////////////////////////////////////////////
    // Animatable
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The UV sub-region of the texture to display, as (x, y, width, height) in [0,1] normalized coordinates.
     * @details Name "pixelArea", type Property::VECTOR4 (animatable). Default is (0, 0, 1, 1) — full texture.
     * @note See also: LottieAnimationView::SetPixelArea(), LottieAnimationView::GetPixelArea().
     */
    PIXEL_AREA = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,

  };
};

} // namespace Ui
} // namespace Dali
