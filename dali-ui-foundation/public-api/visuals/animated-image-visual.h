#ifndef DALI_UI_ANIMATED_IMAGE_VISUAL_OBJECT_H
#define DALI_UI_ANIMATED_IMAGE_VISUAL_OBJECT_H

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/rendering/sampling.h> ///< Dali::WrapMode::Type

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief AnimatedImageVisual renders a sequence of images.
 *
 * Its visual type is VisualType::ANIMATED_IMAGE. It can use the CornerRadius,
 * CornerSquareness and Borderline features of VisualBase.
 */
class DALI_UI_API AnimatedImageVisual : public VisualBase
{
public:
  /**
   * @brief Creates a AnimatedImageVisual object.
   *
   * @return The newly created image visual object.
   */
  static AnimatedImageVisual New();

  /**
   * @brief Downcasts a handle to AnimatedImageVisual handle.
   *
   * If handle points to a AnimatedImageVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a AnimatedImageVisual or an uninitialized handle
   */
  static AnimatedImageVisual DownCast(BaseHandle handle);

public: // Properties
  /**
   * @brief Gets the resource url of the AnimatedImageVisual.
   *
   * @return The url set by SetResourceUrl(), or an empty string when the frames came from
   *         SetResourceUrlList() instead
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Sets the resource URL for a single image.
   *
   * This method cannot be used together with SetResourceUrlList; they are mutually exclusive.
   * Use SetResourceUrl when you want to display a static image.
   *
   * @param[in] resourceUrl The resource URL to set
   */
  void SetResourceUrl(const Dali::String& resourceUrl);

  /**
   * @brief Gets the list of resource urls of the AnimatedImageVisual.
   *
   * @return The urls set by SetResourceUrlList(), or an empty list when a single url was set
   *         with SetResourceUrl() instead
   */
  Dali::Vector<Dali::String> GetResourceUrlList() const;

  /**
   * @brief Sets a list of resource URLs for an animated image sequence.
   *
   * @param[in] resourceUrlList The list of resource URLs to set
   */
  void SetResourceUrlList(const Dali::Vector<Dali::String>& resourceUrlList);

  /**
   * @brief Sets a list of resource URLs for an animated image sequence.
   *
   * This method cannot be used together with SetResourceUrl; they are mutually exclusive.
   * Provide multiple URLs to create an animation.
   *
   * @param[in] resourceUrlList The list of resource URLs to set
   */
  void SetResourceUrlList(std::initializer_list<Dali::String> resourceUrlList)
  {
    Dali::Vector<Dali::String> list;
    list.Reserve(resourceUrlList.size());

    for(auto&& resourceUrl : resourceUrlList)
    {
      list.PushBack(std::move(resourceUrl));
    }
    SetResourceUrlList(list);
  }

  /**
   * @brief Gets whether synchronous loading is enabled.
   *
   * @return True if synchronous loading is enabled
   */
  bool IsSynchronousLoading() const;

  /**
   * @brief Sets whether the image is loaded on the calling thread.
   *
   * Loading synchronously blocks until the image is ready, so nothing is drawn without it
   * and no loading signal is needed. It also stalls whatever thread asked, so it suits small
   * local files and little else. The default is false, which loads on a worker thread.
   *
   * @param[in] synchronous True to load on the calling thread
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets the desired width of the AnimatedImageVisual.
   *
   * @return The desired width of the AnimatedImageVisual
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the width the image is loaded at.
   *
   * The image is resampled to this width as it loads, which bounds the memory a large
   * source costs. Both a width and a height must be set for either to take effect; the
   * default, 0, loads the image at its own size.
   *
   * @param[in] desiredWidth The desired width to set
   * @note Ignored while SetImageLoadWithViewSizeEnabled() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredHeight()
   * @see SetSamplingMode()
   */
  void SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the AnimatedImageVisual.
   *
   * @return The desired height of the AnimatedImageVisual
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the height the image is loaded at.
   *
   * The image is resampled to this height as it loads, which bounds the memory a large
   * source costs. Both a width and a height must be set for either to take effect; the
   * default, 0, loads the image at its own size.
   *
   * @param[in] desiredHeight The desired height to set
   * @note Ignored while SetImageLoadWithViewSizeEnabled() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredWidth()
   */
  void SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the AnimatedImageVisual.
   *
   * @return The sampling mode of the AnimatedImageVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the filter used when the image is resampled to the desired size.
   *
   * It only has an effect where a resample happens, which is when a desired size or
   * SetImageLoadWithViewSizeEnabled() asks for a size other than the source's own. The default is
   * Image::SamplingMode::BOX_THEN_LINEAR.
   *
   * @param[in] samplingMode The sampling mode to set
   * @see SetDesiredWidth()
   */
  void SetSamplingMode(Image::SamplingMode samplingMode);

  /**
   * @brief Gets the pixel area of the AnimatedImageVisual.
   *
   * @return The pixel area of the AnimatedImageVisual
   */
  Dali::Vector4 GetPixelArea() const;

  /**
   * @brief Sets the pixel area of the AnimatedImageVisual.
   *
   * The area is given in texture coordinates as (x, y, width, height): the first two
   * elements are its top-left corner and the last two its size. The default,
   * [0.0, 0.0, 1.0, 1.0], is the whole image.
   *
   * @param[in] pixelArea The pixel area to set
   * @see SetWrapModeU()
   */
  void SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the wrap mode for u coordinate of the AnimatedImageVisual.
   *
   * @return The wrap mode for u coordinate of the AnimatedImageVisual
   */
  Dali::WrapMode::Type GetWrapModeU() const;

  /**
   * @brief Sets the wrap mode for u coordinate of the AnimatedImageVisual.
   *
   * u and v are the coordinates the image is sampled with: u runs from 0.0 at the left
   * edge of the image to 1.0 at the right edge, and v from 0.0 at the top edge to 1.0 at
   * the bottom. The wrap mode decides how the image is sampled where the coordinate falls
   * outside that range, which is what a pixel area reaching beyond the image asks for.
   * Dali::WrapMode::REPEAT tiles the image and Dali::WrapMode::MIRRORED_REPEAT tiles it
   * alternately flipped, while the default clamps to the edge pixel.
   *
   * @param[in] wrapModeU The wrap mode for u coordinate to set
   * @see SetPixelArea()
   */
  void SetWrapModeU(Dali::WrapMode::Type wrapModeU);

  /**
   * @brief Gets the wrap mode for v coordinate of the AnimatedImageVisual.
   *
   * @return The wrap mode for v coordinate of the AnimatedImageVisual
   */
  Dali::WrapMode::Type GetWrapModeV() const;

  /**
   * @brief Sets the wrap mode for v coordinate of the AnimatedImageVisual.
   *
   * v is the vertical coordinate, running from 0.0 at the top edge of the image to 1.0 at
   * the bottom. See SetWrapModeU() for how a wrap mode is applied.
   *
   * @param[in] wrapModeV The wrap mode for v coordinate to set
   * @see SetWrapModeU()
   */
  void SetWrapModeV(Dali::WrapMode::Type wrapModeV);

  /**
   * @brief Gets whether the broken image is enabled.
   *
   * @return True if a broken image is shown when loading fails
   */
  bool IsBrokenImageEnabled() const;

  /**
   * @brief Sets whether to show a broken image when the image fails to load.
   *
   * The broken image is a placeholder drawn in place of an image that could not be loaded,
   * so that the failure is visible rather than leaving an empty area. Which image is used is
   * configured per view size with UiConfig::SetBrokenImageUrl().
   *
   * This is enabled by default. Disable it where a failure should simply draw nothing.
   *
   * @param[in] brokenImageEnabled True to show a broken image when loading fails
   * @see UiConfig::SetBrokenImageUrl()
   */
  void SetBrokenImageEnabled(bool brokenImageEnabled);

  /**
   * @brief Gets the load policy of the AnimatedImageVisual.
   *
   * @return The load policy of the AnimatedImageVisual
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets when the image starts loading.
   *
   * The default is Image::LoadPolicy::ATTACHED, which waits until the visual is on the
   * scene. Image::LoadPolicy::IMMEDIATE starts as soon as the url is set, trading memory
   * held earlier for a shorter wait when the visual does appear.
   *
   * @param[in] loadPolicy The load policy to set
   * @see SetReleasePolicy()
   */
  void SetLoadPolicy(Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the release policy of the AnimatedImageVisual.
   *
   * @return The release policy of the AnimatedImageVisual
   */
  Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets when the loaded texture is dropped from the cache.
   *
   * The default is Image::ReleasePolicy::DETACHED, which frees it once the visual leaves the
   * scene. The other values keep it until the visual is destroyed, or until it is released
   * explicitly, which avoids reloading a image that comes and goes.
   *
   * @param[in] releasePolicy The release policy to set
   * @see SetLoadPolicy()
   */
  void SetReleasePolicy(Image::ReleasePolicy releasePolicy);

  /**
   * @brief Gets the fitting mode of the AnimatedImageVisual.
   *
   * @return The fitting mode of the AnimatedImageVisual
   */
  Image::FittingMode GetFittingMode() const;

  /**
   * @brief Sets the fitting mode of the AnimatedImageVisual.
   *
   * @param[in] fittingMode The fitting mode to set
   */
  void SetFittingMode(Image::FittingMode fittingMode);

  /**
   * @brief Gets whether orientation correction is enabled.
   *
   * @return True if orientation correction is enabled
   */
  bool IsOrientationCorrectionEnabled() const;

  /**
   * @brief Sets whether the EXIF orientation recorded in the file is applied.
   *
   * A photograph taken sideways records how it should be turned rather than storing the
   * turned pixels. Correcting it, the default, rotates and flips the image on load so that
   * it is shown the way it was taken. Disabling this draws the pixels as they are stored.
   *
   * @param[in] orientationCorrection True to apply the recorded orientation
   */
  void SetOrientationCorrectionEnabled(bool orientationCorrection);

  /**
   * @brief Gets whether the image is loaded at the view size.
   *
   * @return True if loading the image with the view size is enabled
   */
  bool IsImageLoadWithViewSizeEnabled() const;

  /**
   * @brief Sets whether the image is loaded at the size of the View it is drawn in.
   *
   * When enabled, the image is loaded at the View's size rather than at the desired size,
   * so it is resampled whenever that size changes.
   *
   * @param[in] enabled True to load the image with the view size
   * @see SetDesiredWidth()
   */
  void SetImageLoadWithViewSizeEnabled(bool enabled);

  /**
   * @brief Queries whether alpha is requested to be pre-multiplied as the image is loaded.
   *
   * This returns the requested value, which is not necessarily what happened to the loaded
   * image. See SetPreMultiplyAlphaOnLoadEnabled() for when a request is not honoured.
   *
   * @return True if alpha is requested to be pre-multiplied while loading
   * @see SetPreMultiplyAlphaOnLoadEnabled()
   */
  bool IsPreMultiplyAlphaOnLoadEnabled() const;

  /**
   * @brief Enables multiplying the colour channels of the image by its alpha channel as it is loaded.
   *
   * This is a request made at load time, so it must be set before the image starts loading;
   * changing it afterwards does not re-load the image.
   *
   * The request is not always honoured. It is ignored when a custom shader is set, and it is
   * dropped when the loaded image uses a compressed pixel format, which cannot be pre-multiplied.
   *
   * @param[in] enabled True to pre-multiply alpha while loading
   * @note Default is false.
   */
  void SetPreMultiplyAlphaOnLoadEnabled(bool enabled);

  /**
   * @brief Gets the alpha mask url of the AnimatedImageVisual.
   *
   * @return The alpha mask url of the AnimatedImageVisual
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @brief Sets the url of the image to use as an alpha mask.
   *
   * The alpha channel of the mask image becomes the alpha of this visual's own image, so
   * the mask decides which parts of it are drawn. How the two images are fitted to each
   * other is decided by SetCropToMask(), and when the mask is applied by SetMaskingPolicy().
   * Setting this once the image has finished loading may make it load again.
   *
   * @param[in] alphaMaskUrl The alpha mask url to set
   * @see SetCropToMask()
   * @see SetMaskingPolicy()
   */
  void SetAlphaMaskUrl(const Dali::String& alphaMaskUrl);

  /**
   * @brief Gets the scale factor applied to the content image before it is masked.
   *
   * @return The scale factor applied to the content image before it is masked
   */
  float GetContentScaleForMasking() const;

  /**
   * @brief Sets the scale factor applied to the content image before it is masked.
   *
   * This scales the image being masked, not the mask itself. The scaled image is then
   * cropped to the size of the alpha mask. The default is 1.0, which leaves the image at
   * its own size, and the value has no effect unless an alpha mask is set.
   *
   * @param[in] contentScale The content scale to set
   * @note With Image::MaskingPolicy::ON_RENDERING the scale is applied only while
   *       crop to mask is enabled.
   * @see SetAlphaMaskUrl()
   * @see SetCropToMask()
   */
  void SetContentScaleForMasking(float contentScale);

  /**
   * @brief Gets whether crop to mask is enabled.
   *
   * @return True if crop to mask is enabled
   */
  bool IsCropToMask() const;

  /**
   * @brief Sets whether to crop to mask.
   *
   * When enabled, the default, the image is cropped to the size of the alpha mask. When
   * disabled, the image keeps its own size and the mask is scaled to fit it instead.
   *
   * @param[in] cropToMask True to crop to mask
   * @see SetAlphaMaskUrl()
   */
  void SetCropToMask(bool cropToMask);

  /**
   * @brief Gets the masking type of the AnimatedImageVisual.
   *
   * @return Whether the alpha mask is applied as the image loads or as it is drawn
   */
  Image::MaskingPolicy GetMaskingPolicy() const;

  /**
   * @brief Sets whether the alpha mask is applied as the image loads or as it is drawn.
   *
   * Image::MaskingPolicy::ON_LOADING, the default, applies the mask to the pixel data
   * while loading, so a single masked texture is uploaded.
   * Image::MaskingPolicy::ON_RENDERING keeps the image and the mask as two textures
   * and combines them while drawing.
   *
   * @param[in] maskingPolicy The masking policy to set
   * @note An image backed by an external texture, such as one from
   *       ImageUrlUtils::GenerateUrl(), can only be masked while drawing, and the value is
   *       forced to Image::MaskingPolicy::ON_RENDERING.
   * @see SetAlphaMaskUrl()
   */
  void SetMaskingPolicy(Image::MaskingPolicy maskingPolicy);

  /**
   * @brief Gets the loop count of the AnimatedImageVisual.
   *
   * @return How many times the animation plays, or a negative value for no limit
   */
  int GetLoopCount() const;

  /**
   * @brief Sets how many times the animation plays.
   *
   * A negative value, the default, plays it without limit. Zero or more plays it that many
   * times and then stops, leaving the frame chosen by SetStopBehavior().
   *
   * @param[in] loopCount The loop count to set
   * @see SetStopBehavior()
   */
  void SetLoopCount(int loopCount);

  /**
   * @brief Gets the stop behavior of the AnimatedImageVisual.
   *
   * @return Which frame is shown once playback stops
   */
  AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @brief Sets which frame is shown once playback stops.
   *
   * The default is AnimatedImage::StopBehavior::CURRENT_FRAME, which leaves whichever frame
   * was being shown; the other values jump to the first or the last frame instead.
   *
   * @param[in] stopBehavior The stop behavior to set
   */
  void SetStopBehavior(AnimatedImage::StopBehavior stopBehavior);

  /**
   * @brief Gets the frame speed factor of the AnimatedImageVisual.
   *
   * @return The multiplier applied to the playback speed
   */
  float GetFrameSpeedFactor() const;

  /**
   * @brief Sets a multiplier for the playback speed.
   *
   * Each frame is shown for its own interval divided by this factor, so 2.0 plays the
   * animation twice as fast and 0.5 at half speed. The default is 1.0. It applies both to
   * the interval set by SetFrameDelay() and to the timing an animated image file carries
   * itself.
   *
   * @param[in] frameSpeedFactor The frame speed factor to set
   * @note The value is limited to the range 0.01 to 100.0.
   * @see SetFrameDelay()
   */
  void SetFrameSpeedFactor(float frameSpeedFactor);

  /**
   * @brief Gets the batch size of the AnimatedImageVisual.
   *
   * @return The number of frames loaded at a time
   */
  int GetBatchSize() const;

  /**
   * @brief Sets how many frames are loaded at a time.
   *
   * Frames are fetched in batches of this size: one batch is loaded before playback starts,
   * and a further batch each time playback catches up. A larger batch loads further ahead
   * at the cost of more work per fetch. The default is 2.
   *
   * @param[in] batchSize The batch size to set
   * @note Values below 2 are ignored, and the batch never exceeds the number of frames.
   * @see SetCacheSize()
   */
  void SetBatchSize(int batchSize);

  /**
   * @brief Gets the cache size of the AnimatedImageVisual.
   *
   * @return The number of frames kept loaded
   */
  int GetCacheSize() const;

  /**
   * @brief Sets how many frames are kept loaded ahead of the one being shown.
   *
   * This bounds how much memory the frames occupy. The default is 2, which is deliberately
   * small: an animation that stutters usually wants a larger cache and batch tuned to it.
   *
   * @param[in] cacheSize The cache size to set
   * @note Values below 2 are ignored. The cache is never smaller than the batch size, and
   *       never exceeds the number of frames.
   * @see SetBatchSize()
   */
  void SetCacheSize(int cacheSize);

  /**
   * @brief Gets the frame delay of the AnimatedImageVisual.
   *
   * @return How long each frame is shown, in milliseconds
   */
  int GetFrameDelay() const;

  /**
   * @brief Sets how long each frame is shown, in milliseconds.
   *
   * This applies only to a sequence built from SetResourceUrlList(), where the frames are
   * separate images and carry no timing of their own. An animated image file such as a GIF
   * carries a delay per frame and is played with those instead, so this value is ignored.
   *
   * @param[in] frameDelay The frame delay to set
   * @see SetResourceUrlList()
   * @see SetFrameSpeedFactor()
   */
  void SetFrameDelay(int frameDelay);

  /**
   * @brief Gets whether the animation is playing, paused or stopped.
   *
   * Play(), Pause() and Stop() each move it to the matching state; it starts out stopped.
   *
   * @return The play state of the AnimatedImageVisual
   * @note This property is read-only.
   */
  AnimatedImage::PlayState GetPlayState() const;

  /**
   * @brief Gets the number of the frame being shown.
   *
   * @return A frame number from 0 to GetTotalFrameCount() - 1, or -1 while the image has not
   *         finished decoding
   * @note This property is read-only.
   * @see GetTotalFrameCount()
   */
  int GetCurrentFrameNumber() const;

  /**
   * @brief Gets how many frames the AnimatedImageVisual has.
   *
   * Frame numbers run from 0 to one less than this count.
   *
   * @return The number of frames, or -1 while the image has not finished decoding
   * @note This property is read-only.
   * @see GetCurrentFrameNumber()
   */
  int GetTotalFrameCount() const;

public: // API to control play state
  /**
   * @brief Starts the animation, or resumes it if it was paused.
   */
  void Play();

  /**
   * @brief Pauses the animation on the frame being shown.
   *
   * The loop it is on is kept, so Play() carries on from here rather than starting over.
   */
  void Pause();

  /**
   * @brief Stops the animation and returns it to its first loop.
   *
   * Which frame is left on screen is decided by SetStopBehavior(): by default it is the one
   * being shown, so a stopped animation does not necessarily look different from a paused
   * one.
   *
   * @see SetStopBehavior()
   */
  void Stop();

  /**
   * @brief Shows a specific frame.
   *
   * This does not start or stop playback: a playing animation carries on from the frame
   * jumped to, and a paused or stopped one stays where it is put.
   *
   * @param[in] frame The frame number to show
   * @note A frame outside 0 to GetTotalFrameCount() - 1 is ignored.
   * @see GetTotalFrameCount()
   */
  void JumpToFrame(int frame);

public:
  AnimatedImageVisual()                                              = default;
  ~AnimatedImageVisual()                                             = default;
  AnimatedImageVisual(const AnimatedImageVisual& rhs)                = default;
  AnimatedImageVisual& operator=(const AnimatedImageVisual& rhs)     = default;
  AnimatedImageVisual(AnimatedImageVisual&& rhs) noexcept            = default;
  AnimatedImageVisual& operator=(AnimatedImageVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL AnimatedImageVisual(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_ANIMATED_IMAGE_VISUAL_OBJECT_H
