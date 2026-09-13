#ifndef DALI_UI_LOTTIE_ANIMATION_VISUAL_OBJECT_H
#define DALI_UI_LOTTIE_ANIMATION_VISUAL_OBJECT_H

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
#include <dali-ui-foundation/public-api/image/lottie-animation-dynamic-property.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
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
 * @brief LottieAnimationVisual renders a Lottie animation.
 *
 * Its visual type is VisualType::LOTTIE_ANIMATION. It can use the CornerRadius,
 * CornerSquareness and Borderline features of VisualBase.
 */
class DALI_UI_API LottieAnimationVisual : public VisualBase
{
public:
  /**
   * @brief Creates a LottieAnimationVisual object.
   *
   * @return The newly created lottie animation visual object.
   */
  static LottieAnimationVisual New();

  /**
   * @brief Downcasts a handle to LottieAnimationVisual handle.
   *
   * If handle points to a LottieAnimationVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a LottieAnimationVisual or an uninitialized handle
   */
  static LottieAnimationVisual DownCast(BaseHandle handle);

public: // Properties
  /**
   * @brief Gets the resource url of the LottieAnimationVisual.
   *
   * @return The resource url of the LottieAnimationVisual
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Sets the resource url of the LottieAnimationVisual.
   *
   * @param[in] resourceUrl The resource url to set
   */
  void SetResourceUrl(const Dali::String& resourceUrl);

  /**
   * @brief Gets whether synchronous loading is enabled.
   *
   * @return True if synchronous loading is enabled
   */
  bool IsSynchronousLoading() const;

  /**
   * @brief Sets whether the animation is loaded on the calling thread.
   *
   * Loading synchronously blocks until the animation is ready, so nothing is drawn without it
   * and no loading signal is needed. It also stalls whatever thread asked, so it suits small
   * local files and little else. The default is false, which loads on a worker thread.
   *
   * @param[in] synchronous True to load on the calling thread
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets the desired width of the LottieAnimationVisual.
   *
   * @return The desired width of the LottieAnimationVisual
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the width the animation is loaded at.
   *
   * The animation is resampled to this width as it loads, which bounds the memory a large
   * source costs. Both a width and a height must be set for either to take effect; the
   * default, 0, loads the animation at its own size.
   *
   * @param[in] desiredWidth The desired width to set
   * @note Ignored while SetImageLoadWithViewSizeEnabled() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredHeight()
   * @see SetSamplingMode()
   */
  void SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the LottieAnimationVisual.
   *
   * @return The desired height of the LottieAnimationVisual
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the height the animation is loaded at.
   *
   * The animation is resampled to this height as it loads, which bounds the memory a large
   * source costs. Both a width and a height must be set for either to take effect; the
   * default, 0, loads the animation at its own size.
   *
   * @param[in] desiredHeight The desired height to set
   * @note Ignored while SetImageLoadWithViewSizeEnabled() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredWidth()
   */
  void SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the LottieAnimationVisual.
   *
   * @return The sampling mode of the LottieAnimationVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the filter used when the animation is resampled to the desired size.
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
   * @brief Gets the pixel area of the LottieAnimationVisual.
   *
   * @return The pixel area of the LottieAnimationVisual
   */
  Dali::Vector4 GetPixelArea() const;

  /**
   * @brief Sets the pixel area of the LottieAnimationVisual.
   *
   * The area is given in texture coordinates as (x, y, width, height): the first two
   * elements are its top-left corner and the last two its size. The default,
   * [0.0, 0.0, 1.0, 1.0], is the whole animation.
   *
   * @param[in] pixelArea The pixel area to set
   * @see SetWrapModeU()
   */
  void SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the wrap mode for u coordinate of the LottieAnimationVisual.
   *
   * @return The wrap mode for u coordinate of the LottieAnimationVisual
   */
  Dali::WrapMode::Type GetWrapModeU() const;

  /**
   * @brief Sets the wrap mode for u coordinate of the LottieAnimationVisual.
   *
   * u and v are the coordinates the rasterized animation is sampled with: u runs from 0.0 at
   * its left edge to 1.0 at the right edge, and v from 0.0 at the top edge to 1.0 at the
   * bottom. The wrap mode decides how it is sampled where the coordinate falls outside that
   * range, which is what a pixel area reaching beyond it asks for.
   * Dali::WrapMode::REPEAT tiles the animation and Dali::WrapMode::MIRRORED_REPEAT tiles it
   * alternately flipped, while the default clamps to the edge pixel.
   *
   * @param[in] wrapModeU The wrap mode for u coordinate to set
   * @see SetPixelArea()
   */
  void SetWrapModeU(Dali::WrapMode::Type wrapModeU);

  /**
   * @brief Gets the wrap mode for v coordinate of the LottieAnimationVisual.
   *
   * @return The wrap mode for v coordinate of the LottieAnimationVisual
   */
  Dali::WrapMode::Type GetWrapModeV() const;

  /**
   * @brief Sets the wrap mode for v coordinate of the LottieAnimationVisual.
   *
   * v is the vertical coordinate, running from 0.0 at the top edge to 1.0 at the bottom. See
   * SetWrapModeU() for how a wrap mode is applied.
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
   * @brief Gets the load policy of the LottieAnimationVisual.
   *
   * @return The load policy of the LottieAnimationVisual
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets when the animation starts loading.
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
   * @brief Gets the release policy of the LottieAnimationVisual.
   *
   * @return The release policy of the LottieAnimationVisual
   */
  Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets when the loaded texture is dropped from the cache.
   *
   * The default is Image::ReleasePolicy::DETACHED, which frees it once the visual leaves the
   * scene. The other values keep it until the visual is destroyed, or until it is released
   * explicitly, which avoids reloading a animation that comes and goes.
   *
   * @param[in] releasePolicy The release policy to set
   * @see SetLoadPolicy()
   */
  void SetReleasePolicy(Image::ReleasePolicy releasePolicy);

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
   * @brief Gets the loop count of the LottieAnimationVisual.
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
   * @brief Sets the playback range by frame numbers.
   *
   * The animation plays between @p minFrame and @p maxFrame. A frame past the end is
   * clamped to the last one, and a reversed pair is put back in order. By default the whole
   * animation is played.
   *
   * @param[in] minFrame The start frame number
   * @param[in] maxFrame The end frame number
   * @see SetMinMaxFrameByMarker()
   */
  void SetMinMaxFrame(int minFrame, int maxFrame);

  /**
   * @brief Gets the playback range as frame numbers.
   *
   * When the range was set by marker names instead, this returns the whole animation,
   * `[0, GetTotalFrameCount()]`.
   *
   * @param[out] minFrame The start frame number
   * @param[out] maxFrame The end frame number
   */
  void GetMinMaxFrame(int& minFrame, int& maxFrame) const;

  /**
   * @brief Sets the playback range using marker names embedded in the animation.
   *
   * With only @p minMarker, the animation plays that marker's own range. With both, it
   * plays from the start of @p minMarker to the end of @p maxMarker.
   *
   * @param[in] minMarker Name of the start marker
   * @param[in] maxMarker Name of the end marker, or empty to use only @p minMarker
   * @see GetMarkerInfo()
   */
  void SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker = "");

  /**
   * @brief Gets the stop behavior of the LottieAnimationVisual.
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
   * @brief Gets the frame speed factor of the LottieAnimationVisual.
   *
   * @return The multiplier applied to the playback speed
   */
  float GetFrameSpeedFactor() const;

  /**
   * @brief Sets a multiplier for the playback speed.
   *
   * Each frame is shown for its own duration divided by this factor, so 2.0 plays the
   * animation twice as fast and 0.5 at half speed. The default is 1.0, which plays it at
   * the frame rate the animation file declares.
   *
   * @param[in] frameSpeedFactor The frame speed factor to set
   * @note The value is limited to the range 0.01 to 100.0.
   */
  void SetFrameSpeedFactor(float frameSpeedFactor);

  /**
   * @brief Gets the looping mode of the LottieAnimationVisual.
   *
   * @return The looping mode of the LottieAnimationVisual
   */
  LottieAnimation::LoopingMode GetLoopingMode() const;

  /**
   * @brief Sets the looping mode of the LottieAnimationVisual.
   *
   * @param[in] loopingMode The looping mode to set
   */
  void SetLoopingMode(LottieAnimation::LoopingMode loopingMode);

  /**
   * @brief Gets whether the animation is rasterized again when it is scaled down.
   *
   * @return True if a scale down triggers a fresh rasterization
   */
  bool IsRedrawOnScaleDown() const;

  /**
   * @brief Sets whether the animation is rasterized again when it is scaled down.
   *
   * A Lottie animation is drawn by rasterizing it into an image at the size it occupies.
   * When the View's scale then shrinks, this decides whether that image is produced again at
   * the smaller size, or whether the existing one is kept and simply shown smaller. The
   * default is true.
   *
   * @param[in] redrawOnScaleDown True to rasterize again when scaled down
   * @see SetRedrawOnScaleUp()
   */
  void SetRedrawOnScaleDown(bool redrawOnScaleDown);

  /**
   * @brief Gets whether the animation is rasterized again when it is scaled up.
   *
   * @return True if a scale up triggers a fresh rasterization
   */
  bool IsRedrawOnScaleUp() const;

  /**
   * @brief Sets whether the animation is rasterized again when it is scaled up.
   *
   * Without this the existing image is stretched to the larger size, which blurs an
   * animation that could have been drawn sharply. Producing it again keeps it sharp, at the
   * cost of the extra work and the memory a larger image takes. The default is true.
   *
   * @param[in] redrawOnScaleUp True to rasterize again when scaled up
   * @note Combining this with SetRenderScale() scales twice: a render scale of 1.5 under a
   *       View scale of 2.0 rasterizes at 3.0.
   * @see SetRedrawOnScaleDown()
   * @see SetRenderScale()
   */
  void SetRedrawOnScaleUp(bool redrawOnScaleUp);

  /**
   * @brief Gets whether the visual is redrawn only once a new frame is ready.
   *
   * @return True if redrawing waits for a new frame
   */
  bool IsNotifyAfterRasterizationEnabled() const;

  /**
   * @brief Sets whether the visual is redrawn only once a new frame is ready.
   *
   * By default the visual is redrawn continuously, whether or not a new frame of the
   * animation has been produced yet. Enabling this redraws it only once a new frame is
   * actually ready, which spares that repeated work when the animation runs at a low frame
   * rate. The default is false.
   *
   * @param[in] notifyAfterRasterization True to redraw only once a new frame is ready
   */
  void SetNotifyAfterRasterizationEnabled(bool notifyAfterRasterization);

  /**
   * @brief Gets the factor the animation is rasterized at.
   *
   * @return The multiplier applied to the rasterization size
   */
  float GetRenderScale() const;

  /**
   * @brief Sets the factor the animation is rasterized at.
   *
   * The texture is produced at this multiple of the size the visual occupies, so a value
   * above 1.0 draws it in more detail than it is shown at and stays sharp if it is later
   * enlarged. The default is 1.0, which rasterizes at the size shown.
   *
   * @param[in] renderScale The multiplier to rasterize at
   * @note This multiplies with the View's own scale rather than replacing it: a render
   *       scale of 1.5 under a View scale of 2.0 rasterizes at 3.0.
   * @see SetRedrawOnScaleUp()
   */
  void SetRenderScale(float renderScale);

  /**
   * @brief Gets whether aspect fit scaling is enabled.
   *
   * @return True if aspect fit is enabled, false otherwise (default: true)
   */
  bool IsAspectFitEnabled() const;

  /**
   * @brief Sets whether to enable aspect fit scaling.
   *
   * When enabled (default), the animation is scaled to fit within the target size
   * while preserving its aspect ratio. When disabled, the animation stretches to
   * fill the entire target size.
   *
   * @param[in] aspectFitEnabled True to enable aspect fit, false to disable
   */
  void SetAspectFitEnabled(bool aspectFitEnabled);

  /**
   * @brief Gets whether the animation is playing, paused or stopped.
   *
   * Play(), Pause() and Stop() each move it to the matching state; it starts out stopped.
   *
   * @return The play state of the LottieAnimationVisual
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
   * @brief Gets how many frames the LottieAnimationVisual has.
   *
   * Frame numbers run from 0 to one less than this count.
   *
   * @return The number of frames, or -1 while the image has not finished decoding
   * @note This property is read-only.
   * @see GetCurrentFrameNumber()
   */
  int GetTotalFrameCount() const;

  /**
   * @brief Gets layer information embedded in the Lottie file.
   *
   * The returned map holds one entry per layer:
   *
   * |       | Type                         | Content                                    |
   * |-------|------------------------------|--------------------------------------------|
   * | Key   | Property::STRING             | The layer's name                           |
   * | Value | Property::ARRAY of 2 INTEGER | The first and last frame numbers it covers |
   *
   * @return A Property::Map of layer info, or an empty map until the file has loaded
   * @note This property is read-only.
   */
  Dali::Property::Map GetContentInfo() const;

  /**
   * @brief Gets marker information embedded in the Lottie file.
   *
   * The returned map holds one entry per marker:
   *
   * |       | Type                         | Content                                    |
   * |-------|------------------------------|--------------------------------------------|
   * | Key   | Property::STRING             | The marker's name                          |
   * | Value | Property::ARRAY of 2 INTEGER | The first and last frame numbers it covers |
   *
   * A key taken from this map can be passed straight to SetMinMaxFrameByMarker().
   *
   * @return A Property::Map of marker info, or an empty map until the file has loaded
   * @note This property is read-only.
   * @see SetMinMaxFrameByMarker()
   */
  Dali::Property::Map GetMarkerInfo() const;

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
   * @note A frame outside the playback range is clamped to the nearest end of it.
   * @see SetMinMaxFrame()
   */
  void JumpToFrame(int frame);

public: // Advanced
  /**
   * @brief Sets a per-frame dynamic property callback on a specific layer/element.
   *
   * The info is consumed: its callback is handed to the visual, so pass it with std::move().
   *
   * @param[in] info The dynamic property info
   */
  void SetDynamicProperty(LottieAnimation::DynamicProperty info);

public:
  LottieAnimationVisual()                                                = default;
  ~LottieAnimationVisual()                                               = default;
  LottieAnimationVisual(const LottieAnimationVisual& rhs)                = default;
  LottieAnimationVisual& operator=(const LottieAnimationVisual& rhs)     = default;
  LottieAnimationVisual(LottieAnimationVisual&& rhs) noexcept            = default;
  LottieAnimationVisual& operator=(LottieAnimationVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL LottieAnimationVisual(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_IMAGE_VISUAL_OBJECT_H
