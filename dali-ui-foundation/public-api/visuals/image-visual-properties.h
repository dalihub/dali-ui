#ifndef DALI_UI_IMAGE_VISUAL_PROPERTIES_H
#define DALI_UI_IMAGE_VISUAL_PROPERTIES_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief ImageVisual is to render an image into the control's quad.
 */
struct ImageVisualPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this visual.
   */
  enum PropertyRange
  {
    MUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::MUTABLE_PROPERTY_END_INDEX + 1,
    MUTABLE_PROPERTY_END_INDEX   = MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    IMMUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::IMMUTABLE_PROPERTY_END_INDEX + 1,
    IMMUTABLE_PROPERTY_END_INDEX   = IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    READ_ONLY_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::READ_ONLY_PROPERTY_END_INDEX + 1,
    READ_ONLY_PROPERTY_END_INDEX   = READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the ImageVisual.
   */
  enum
  {
    // For simple images

    /**
     * @brief The URL of the image.
     * @details Name "url", type Property::STRING or Property::ARRAY of Property::STRING.
     * @note The array form is used for generating animated image visuals.
     * @note The number of threads used for local and remote image loading can be controlled by the
     *       environment variables DALI_TEXTURE_LOCAL_THREADS and DALI_TEXTURE_REMOTE_THREADS respectively.
     *       The default values are 4 threads for local image loading and 8 threads for remote image loading.
     * @note Mandatory.
     */
    URL = IMMUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief Whether to load the image synchronously.
     * @details Name "synchronousLoading", type Property::BOOLEAN.
     * @note Optional. If not specified, the default is false, i.e. the image is loaded asynchronously.
     * @note For Normal Quad images only.
     */
    SYNCHRONOUS_LOADING,

    /**
     * @brief The desired image width.
     * @details Name "desiredWidth", type Property::INTEGER.
     * @note Optional. If not specified, the actual image width is used.
     * @note For Normal Quad images only.
     */
    DESIRED_WIDTH,

    /**
     * @brief The desired image height.
     * @details Name "desiredHeight", type Property::INTEGER.
     * @note Optional. If not specified, the actual image height is used.
     * @note For Normal Quad images only.
     */
    DESIRED_HEIGHT,

    /**
     * @brief Filtering options, used when resizing images to sample original pixels.
     * @details Name "samplingMode", type Dali::SamplingMode (Property::INTEGER) or Property::STRING.
     * @note Optional. If not supplied, default is SamplingMode::BOX.
     * @note For Normal Quad images only.
     * @see Dali::SamplingMode
     */
    SAMPLING_MODE,

    /**
     * @brief The image area to be displayed.
     * @details Name "pixelArea", type Property::VECTOR4.
     *          It is a rectangular area.
     *          The first two elements indicate the top-left position of the area, and the last two elements are the area
     * width and height respectively.
     * @note Optional. If not specified, the default value is [0.0, 0.0, 1.0, 1.0], i.e. the entire area of the image.
     * @note For Normal Quad images only.
     */
    PIXEL_AREA,

    /**
     * @brief The wrap mode for u coordinate.
     * @details Name "wrapModeU", type Dali::WrapMode::Type (Property::INTEGER) or Property::STRING.
     *          It decides how the texture should be sampled when the u coordinate exceeds the range of 0.0 to 1.0.
     * @note Optional. If not specified, the default is CLAMP.
     * @note For Normal QUAD image only.
     */
    WRAP_MODE_U,

    /**
     * @brief The wrap mode for v coordinate.
     * @details Name "wrapModeV", type Dali::WrapMode::Type (Property::INTEGER) or Property::STRING.
     *          it decides how the texture should be sampled when the v coordinate exceeds the range of 0.0 to 1.0.
     * @note Optional. If not specified, the default is CLAMP.
     * @note For Normal QUAD image only.
     */
    WRAP_MODE_V,

    /**
     * @brief Whether to enable broken image in image visual.
     * Some of visual don't need to show broken image(ex. placeholder)
     * Disable broken image for these visuals.
     * default is true.
     */
    ENABLE_BROKEN_IMAGE,

    /**
     * @brief The policy to determine when an image should be loaded.
     * @details Name "loadPolicy", Type Ui::Image::LoadPolicy (Property::INTEGER)or Property::STRING.
     * @note Default Ui::Image::LoadPolicy::ATTACHED
     * @see Ui::Image::LoadPolicy:
     */
    LOAD_POLICY,

    /**
     * @brief The policy to determine when an image request to be released so should no longer be cached.
     * @details Name "releasePolicy", Type Ui::Image::ReleasePolicy (Property::INTEGER) or Property::STRING
     * @note Default Ui::Image::ReleasePolicy::DETACHED
     * @see Ui::Image::ReleasePolicy
     */
    RELEASE_POLICY,

    /**
     * @brief The fitting mode of the visual.
     * @details Name "fittingMode", Type Ui::Image::FittingMode (Property::INTEGER) or Property::STRING.
     * @note Default Ui::Image::FittingMode::FILL
     * @see Ui::Image::FittingMode
     */
    FITTING_MODE,

    /**
     * @brief Determines if image orientation should be corrected so the image displays as it was intended.
     * @details Name "orientationCorrection", Type Property::BOOLEAN, if true the image's orientation will be corrected.
     * @note Default true
     */
    ORIENTATION_CORRECTION,

    /**
     * @brief Whether to synchronize image texture size to visual size.
     * @details Name "synchronousSizing", type Property::BOOLEAN.
     * If this property is true, ImageVisual ignores mDesiredSize.
     * @note Used by the ImageVisual. The default is false.
     */
    SYNCHRONOUS_SIZING,

    // For Image only.

    /**
     * @brief If true, uploads texture before ResourceReady signal is emitted. Otherwise uploads after texture load is
     * completed.
     * @details Name "fastTrackUploading", type Property::BOOLEAN
     * If true, the upload happens without event-thread dependency, but the following need to be considered:
     *  - Texture size is not valid until upload is fully complete.
     *  - Texture cannot be cached (a new image is uploaded every time).
     *  - Seamless visual change is not supported.
     *  - The following, if set are also not supported and will be ignored:
     *    - Alpha masking
     *    - Synchronous loading
     *    - Reload action
     *    - Custom shader
     * @note Used by the ImageVisual. The default is false.
     */
    FAST_TRACK_UPLOADING,

    // For n-patch images

    /**
     * @brief The border of the image.
     * @details Name "border", type Property::EXTENTS, Property::RECTANGLE or Property::VECTOR4.
     *          The border of the image in the order: left, right, top, bottom.
     * @note Optional.
     * @note For N-Patch images only.
     */
    BORDER,

    /**
     * @brief If true, only draws the borders.
     * @details Name "borderOnly", type Property::BOOLEAN.
     * @note Optional. If not specified, the default is false.
     * @note For N-Patch images only.
     */
    BORDER_ONLY,

    /**
     * @brief Overlays the auxiliary image on top of an NPatch image.
     *
     * The resulting visual image will be at least as large as the
     * smallest possible n-patch or the auxiliary image, whichever is
     * larger.
     *
     * @details Name "auxiliaryImage", Type Property::STRING, URL of the image.
     * @note Default true
     */
    AUXILIARY_IMAGE,

    /**
     * @brief An alpha value for mixing between the masked main NPatch image and the auxiliary image
     * @details Name "auxiliaryImageAlpha", Type Property::FLOAT, between 0 and 1
     * @note Default 0
     */
    AUXILIARY_IMAGE_ALPHA,

    // For both Image and AnimatedImage

    /**
     * @brief Enables/disables premultiplied alpha.
     * @details Name "premultipliedAlpha", type Property::BOOLEAN.
     * @note Optional.
     * @note The premultiplied alpha is true for ImageVisual, and false for AnimatedImageVisual.
     */
    PRE_MULTIPLIED_ALPHA,

    /**
     * @brief URL of a masking image
     * @details Name "alphaMaskUrl", type Property::STRING, URL of image to apply as
     * a mask after image loading. If set after the main URL has finished loading, this
     * may necessitate a re-load of the main image. The alpha mask image will be scaled
     * on load to match the size of the main image, then applied to the pixel data
     * before uploading to GL.
     * @note Optional.
     */
    ALPHA_MASK_URL,

    /**
     * @brief The scale factor to apply to the content image before masking
     * @details Name "maskContentScale", type Property::FLOAT, The scale factor
     * to apply to the content before masking. Note, scaled images are cropped to
     * the same size as the alpha mask.
     * @note Optional.
     */
    MASK_CONTENT_SCALE,

    /**
     * @brief Whether to crop image to mask or scale mask to fit image
     * @details Name "cropToMask", type Property::BOOLEAN, True if the image should
     * be cropped to match the mask size, or false if the image should remain the same size.
     * @note Optional, Default true
     * @note If this is false, then the mask is scaled to fit the image before being applied.
     */
    CROP_TO_MASK,

    /**
     * @brief Whether to apply mask in loading time or rendering time.
     * @details Name "maskingType", type Ui::Image::MaskingType (Property::INTEGER).
     * In general, Ui::Image::MASKING_ON_LOADING is the default behavior.
     * However, if the visual uses an external texture, only MASKING_ON_RENDERING is possible.
     * So we change its value to MASKING_ON_RENDERING even if the visual sets the MASKING_TYPE as MASKING_ON_LOADING when
     * it uses external texture.
     * @note The default is Ui::Image::MASKING_ON_LOADING.
     */
    MASKING_TYPE,

    // For both AnimatedImage and LottieAnimation

    /**
     * @brief The number of times the AnimatedImageVisual or AnimatedVectorImageVisual will be looped.
     * @details Name "loopCount", type Property::INTEGER.
     * @note For Animated images only. Default -1. if < 0, loop unlimited. else, loop loopCount times.
     */
    LOOP_COUNT,

    /**
     * @brief The playing range the AnimatedVectorImageVisual will use.
     *
     * Animation will play between the values specified. The array can have two integer values.
     * Or it can have one or two strings, which are markers. More will be ignored.
     * Both values should be between 0 and the total frame number, otherwise they will be ignored.
     * If the range provided is not in proper order ( minimum, maximum ), it will be reordered.
     *
     * A marker has its start frame and end frame.
     * Animation will play between the start frame and the end frame of the marker if one marker is specified.
     * Or animation will play between the start frame of the first marker and the end frame of the second marker if two
     * markers are specified.
     *
     * @details Name "playRange", Type Property::ARRAY of Property::INTEGER or Property::ARRAY of Property::STRING or
     * Property::STRING (one marker).
     * @note Default 0 and the total frame number.
     */
    PLAY_RANGE,

    /**
     * @brief  The stop behavior the AnimatedImageVisual and AnimatedVectorImageVisual will use.
     * @details Name "stopBehavior", Type StopBehavior::Type (Property::INTEGER)
     * @note Default value is StopBehavior::CURRENT_FRAME.
     */
    STOP_BEHAVIOR,

    /**
     * @brief Specifies a speed factor for the animated image frame.
     * @details Name "frameSpeedFactor", type Property::FLOAT.
     *
     * The speed factor is a multiplier of the normal velocity of the animation. Values between [0,1] will
     * slow down the animation and values above one will speed up the animation.
     *
     * @note The range of this value is clamped between [0.01f ~ 100.0f].
     *       It might be supported out of bound, and negative value in future.
     * @note It is used in the AnimatedImageVisual and AnimatedVectorImageVisual. The default is 1.0f.
     */
    FRAME_SPEED_FACTOR,

    // For Animated images only

    /**
     * @brief Defines the batch size for pre-loading images in the AnimatedImageVisual
     * @details Name "batchSize", type Property::INTEGER, number of images to pre-load
     * before starting to play. Default value: 2
     * @note Optional.
     * @note Minimum supported value is 2.
     */
    BATCH_SIZE,

    /**
     * @brief Defines the cache size for loading images in the AnimatedImageVisual
     * @details Name "cacheSize", type Property::INTEGER, number of images to keep
     * cached ahead during playback. Default value: 2
     *
     * @note Optional.
     * @note Minimum supported value is 2.
     * @note, cacheSize should be >= batchSize.
     * If it isn't, then the cache will automatically be changed to batchSize.
     * @note, because of the defaults, it is expected that the application developer
     * tune the batch and cache sizes to their particular use case.
     */
    CACHE_SIZE,

    /**
     * @brief The number of milliseconds between each frame in the AnimatedImageVisual
     * @details Name "frameDelay", type Property::INTEGER, The number of milliseconds between each frame.
     * @note Optional.
     * @note This is only used when multiple URLs are provided.
     */
    FRAME_DELAY,

    // For LottieAnimation only

    /**
     * @brief  The looping mode the AnimatedVectorImageVisual will use.
     * @details Name "loopingMode", Type LoopingMode::Type (Property::INTEGER)
     * @note Default value is LoopingMode::RESTART.
     */
    LOOPING_MODE,

    /**
     * @brief Whether to redraw the image when the visual is scaled down.
     * @details Name "redrawInScalingDown", type Property::BOOLEAN.
     * @note It is used in the AnimatedVectorImageVisual. The default is true.
     */
    REDRAW_IN_SCALING_DOWN,

    /**
     * @brief Whether to redraw the image when the visual is scaled up.
     * @details Name "redrawInScalingUp", type Property::BOOLEAN.
     * @note It is used in the AnimatedVectorImageVisual. The default is true.
     */
    REDRAW_IN_SCALING_UP,

    /**
     * @brief Whether to AnimatedVectorImageVisual fixed cache or not.
     * @details Name "enableFrameCache", type Property::BOOLEAN.
     * If this property is true, AnimatedVectorImageVisual enable frame cache for loading and keeps loaded frame
     * until the visual is removed. It reduces CPU cost when the animated image will be looping.
     * But it can spend a lot of memory if the resource has high resolution image or many frame count.
     * @note It is used in the AnimatedVectorImageVisual. The default is false
     */
    ENABLE_FRAME_CACHE,

    /**
     * @brief Whether notify AnimatedVectorImageVisual to render thread after every rasterization or not.
     * @details Name "notifyAfterRasterization", type Property::BOOLEAN.
     * If this property is true, AnimatedVectorImageVisual send notify to render thread after every rasterization.
     * If false, AnimatedVectorImageVisual set Renderer's Behaviour as Continouly (mean, always update the render thread.)
     *
     * This flag is useful if given resource has low fps, so we don't need to render every frame.
     * @note It is used in the AnimatedVectorImageVisual. The default is false.
     */
    NOTIFY_AFTER_RASTERIZATION,

    /**
     * @brief Renders a texture at a given scale.
     * @details Name "renderScale", type Property::FLOAT.
     * Using render scale with Actor::Property::SCALE(= mRedrawInScalingUp is true) may scale twice.
     * i.e RENDER_SCALE = 1.5f and SCALE = 2.0f would eventually multiply the texture by 3.0f
     * @note It is used when scaling up AnimatedVectorImageVisual. The default is 1.0f.
     */
    RENDER_SCALE,

    /**
     * @brief Whether to enable aspect fit for AnimatedVectorImageVisual.
     * @details Name "enableAspectFit", type Property::BOOLEAN.
     * If this property is true, the vector image will be scaled to fit within the visual size
     * while maintaining the aspect ratio. If false, the image will be stretched to fill the visual size.
     * @note It is used in the AnimatedVectorImageVisual. The default is true.
     */
    ENABLE_ASPECT_FIT,

    //// Read-only properties.

    // For both AnimatedImage and LottieAnimation

    /**
     * @brief The playing state the AnimatedImageVisual and AnimatedVectorImageVisual will use.
     * @details Name "playState", Type Ui::AnimatedImage::PlayState (Property::INTEGER)
     * @note This property is read-only.
     */
    PLAY_STATE = READ_ONLY_PROPERTY_START_INDEX,

    /**
     * @brief The current frame number the AnimatedImageVisual and AnimatedVectorImageVisual will use.
     * @details Name "currentFrameNumber", Type Property::INTEGER, between [0, the maximum frame number] or between the
     * play range if specified
     * @note This property is read-only.
     */
    CURRENT_FRAME_NUMBER,

    /**
     * @brief The total frame number the AnimatedImageVisual and AnimatedVectorImageVisual will use.
     * @details Name "totalFrameNumber", Type Property::INTEGER.
     * @note This property is read-only.
     */
    TOTAL_FRAME_NUMBER,

    // For LottieAnimation only

    /**
     * @brief The content information the AnimatedVectorImageVisual will use.
     * @details Type Property::MAP.
     * The map contains the layer name as a key and Property::Array as a value.
     * And the array contains 2 integer values which are the frame numbers, the start frame number and the end frame
     * number of the layer.
     * @note This property is read-only.
     */
    CONTENT_INFO,

    /**
     * @brief The marker information the AnimatedVectorImageVisual will use.
     * @details Type Property::MAP.
     * The map contains the marker name as a key and Property::Array as a value.
     * And the array contains 2 integer values which are the frame numbers, the start frame number and the end frame
     * number of the marker.
     * @note This property is read-only.
     */
    MARKER_INFO,
  };

}; //namespace ImageVisualPropertyIndex

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_IMAGE_VISUAL_PROPERTIES_H
