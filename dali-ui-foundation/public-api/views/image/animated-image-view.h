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

#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class AnimatedImageViewImpl;
}

/**
 * @brief AnimatedImageView is a View for displaying animated image resources such as GIF files.
 *
 * It supports playback control (play, pause, stop) and loop count configuration.
 * The animation starts automatically when Play() is called after the image is loaded.
 *
 * @code
 * AnimatedImageView view = AnimatedImageView::New("animation.gif");
 * view.SetLoopCount(3);
 * view.Play();
 * @endcode
 */
class DALI_UI_API AnimatedImageView : public View
{
public:
  /**
   * @brief Property indices for AnimatedImageView.
   *
   * These can be used with Dali::Handle::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      IMAGE                     = AnimatedImageViewPropertyIndex::IMAGE,
      IMAGE_URLS                = AnimatedImageViewPropertyIndex::IMAGE_URLS,
      LOOP_COUNT                = AnimatedImageViewPropertyIndex::LOOP_COUNT,
      STOP_BEHAVIOR             = AnimatedImageViewPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR        = AnimatedImageViewPropertyIndex::FRAME_SPEED_FACTOR,
      BATCH_SIZE                = AnimatedImageViewPropertyIndex::BATCH_SIZE,
      CACHE_SIZE                = AnimatedImageViewPropertyIndex::CACHE_SIZE,
      FRAME_DELAY               = AnimatedImageViewPropertyIndex::FRAME_DELAY,
      IMAGE_COLOR               = AnimatedImageViewPropertyIndex::IMAGE_COLOR,
      PRE_MULTIPLIED_ALPHA      = AnimatedImageViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      FITTING_MODE              = AnimatedImageViewPropertyIndex::FITTING_MODE,
      SAMPLING_MODE             = AnimatedImageViewPropertyIndex::SAMPLING_MODE,
      DESIRED_WIDTH             = AnimatedImageViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT            = AnimatedImageViewPropertyIndex::DESIRED_HEIGHT,
      LOAD_POLICY               = AnimatedImageViewPropertyIndex::LOAD_POLICY,
      RELEASE_POLICY            = AnimatedImageViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING       = AnimatedImageViewPropertyIndex::SYNCHRONOUS_LOADING,
      IMAGE_LOAD_WITH_VIEW_SIZE = AnimatedImageViewPropertyIndex::IMAGE_LOAD_WITH_VIEW_SIZE,
      ALPHA_MASK_URL            = AnimatedImageViewPropertyIndex::ALPHA_MASK_URL,
      CROP_TO_MASK              = AnimatedImageViewPropertyIndex::CROP_TO_MASK,
      MASKING_MODE              = AnimatedImageViewPropertyIndex::MASKING_MODE,
      PLACEHOLDER_IMAGE         = AnimatedImageViewPropertyIndex::PLACEHOLDER_IMAGE,
      PIXEL_AREA                = AnimatedImageViewPropertyIndex::PIXEL_AREA,
    };
  };

  /// @brief Animation finished signal type. Emitted when the animation completes all loops.
  typedef Signal<void(View)> AnimationFinishedSignalType;

public: // Creation & Destruction
  AnimatedImageView();
  ~AnimatedImageView();
  AnimatedImageView(const AnimatedImageView& rhs);
  AnimatedImageView(AnimatedImageView&& rhs) noexcept;
  AnimatedImageView& operator=(const AnimatedImageView& rhs);
  AnimatedImageView& operator=(AnimatedImageView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(AnimatedImageView)

  /**
   * @brief Creates a new AnimatedImageView.
   *
   * @param[in] url Optional URL of the animated image resource (e.g. GIF file)
   * @return A handle to the newly allocated AnimatedImageView
   */
  static AnimatedImageView New(const Dali::String& url = "");

  /**
   * @brief Downcasts a handle to AnimatedImageView.
   *
   * @param[in] handle A handle to an object
   * @return A handle to AnimatedImageView, or an empty handle if not AnimatedImageView
   */
  static AnimatedImageView DownCast(BaseHandle handle);

public: // Image
  /**
   * @brief Sets the URL of the animated image resource to display.
   *
   * @param[in] url The URL of the image resource
   * @return Reference to this for fluent chaining
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @brief Gets the URL of the animated image resource.
   *
   * @return The URL of the image resource
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Reloads the animated image resource from its source.
   *
   * This forces the visual to reload the current URL instead of reusing the cached texture.
   */
  void Reload();

  /**
   * @brief Sets an array of image URLs for frame-by-frame animation.
   *
   * Setting this overrides any single URL set via SetResourceUrl().
   * The images are played in sequence as animation frames.
   *
   * @param[in] urls Array of image URLs
   * @return Reference to this for fluent chaining
   */
  void SetResourceUrls(const Dali::Vector<Dali::String>& urls);

  /**
   * @brief Gets the array of image URLs.
   *
   * @return The current array of image URLs, or empty if not set
   */
  const Dali::Vector<Dali::String>& GetResourceUrls() const;

  /**
   * @brief Sets whether the image is loaded synchronously at the current view size.
   *
   * @param[in] enabled True to enable loading image with view size
   * @return Reference to this for fluent chaining
   */
  void SetImageLoadWithViewSize(bool enabled);

  /**
   * @brief Gets whether the image is loaded with the view size.
   *
   * @return True if loading image with view size is enabled
   */
  bool IsImageLoadWithViewSizeEnabled() const;

public: // Playback Control
  /**
   * @brief Starts or resumes playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  void Play();

  /**
   * @brief Pauses playback of the animation.
   *
   * @return Reference to this for fluent chaining
   */
  void Pause();

  /**
   * @brief Stops playback of the animation and resets to the first frame.
   *
   * @return Reference to this for fluent chaining
   */
  void Stop();

  /**
   * @brief Sets the number of times the animation should loop.
   *
   * A value of -1 means infinite looping. A value of 0 means the animation
   * will not play. A positive value specifies an exact loop count.
   *
   * @param[in] count The loop count (-1 for infinite)
   * @return Reference to this for fluent chaining
   */
  void SetLoopCount(int count);

  /**
   * @brief Gets the current loop count.
   *
   * @return The loop count, or -1 for infinite looping
   */
  int GetLoopCount() const;

public: // Frame Control
  /**
   * @brief Jumps to the specified frame number.
   *
   * @param[in] frame The frame index to jump to
   * @return Reference to this for fluent chaining
   */
  void JumpToFrame(int frame);

  /**
   * @brief Sets the behavior of the animation when it is stopped.
   *
   * @param[in] behavior The stop behavior (CURRENT_FRAME, FIRST_FRAME, or LAST_FRAME)
   * @return Reference to this for fluent chaining
   */
  void SetStopBehavior(AnimatedImage::StopBehavior behavior);

  /**
   * @brief Gets the current stop behavior.
   *
   * @return The current stop behavior
   */
  AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @brief Sets the speed factor for animation playback.
   *
   * Values between [0, 1] slow down the animation; values above 1 speed it up.
   * The actual clamping to [0.01, 100.0] is handled by the underlying animation renderer.
   *
   * @param[in] factor The speed multiplier (default: 1.0)
   * @return Reference to this for fluent chaining
   */
  void SetFrameSpeedFactor(float factor);

  /**
   * @brief Gets the current frame speed factor.
   *
   * @return The frame speed factor
   */
  float GetFrameSpeedFactor() const;

  /**
   * @brief Sets the number of frames to pre-load in each batch.
   *
   * @param[in] size The batch size (default: 1)
   * @return Reference to this for fluent chaining
   */
  void SetBatchSize(int size);

  /**
   * @brief Gets the current batch size.
   *
   * @return The batch size
   */
  int GetBatchSize() const;

  /**
   * @brief Sets the number of frames to keep in cache.
   *
   * @param[in] size The cache size (default: 1)
   * @return Reference to this for fluent chaining
   */
  void SetCacheSize(int size);

  /**
   * @brief Gets the current cache size.
   *
   * @return The cache size
   */
  int GetCacheSize() const;

  /**
   * @brief Sets the delay in milliseconds between each frame.
   *
   * Overrides the frame delay embedded in the image file.
   *
   * @param[in] milliseconds The delay between frames in milliseconds
   * @return Reference to this for fluent chaining
   */
  void SetFrameDelay(int milliseconds);

  /**
   * @brief Gets the current frame delay override.
   *
   * @return The frame delay in milliseconds, or -1 if not overridden
   */
  int GetFrameDelay() const;

  /**
   * @brief Gets the current playback state of the animation.
   *
   * @return The current PlayState
   */
  Ui::AnimatedImage::PlayState GetPlayState() const;

  /**
   * @brief Gets the current frame number being displayed.
   *
   * @return The current frame index
   */
  int GetCurrentFrame() const;

  /**
   * @brief Gets the total number of frames in the animation.
   *
   * @return The total frame count
   */
  int GetTotalFrame() const;

public: // Visual Appearance
  /**
   * @brief Sets the color applied to the image.
   *
   * @param[in] color The color to apply
   * @return Reference to this for fluent chaining
   */
  void SetImageColor(const UiColor& color);

  /**
   * @brief Gets the color applied to the image.
   *
   * @return The current image color
   */
  UiColor GetImageColor() const;

  /**
   * @brief Sets how the image is fitted within the view bounds.
   *
   * The default fitting mode is Ui::Image::FittingMode::FILL (stretch to fill).
   *
   * @param[in] fittingMode The fitting mode to use
   * @return Reference to this for fluent chaining
   */
  void SetFittingMode(Ui::Image::FittingMode fittingMode);

  /**
   * @brief Gets the fitting mode.
   *
   * @return The current fitting mode (default: FILL)
   */
  Ui::Image::FittingMode GetFittingMode() const;

  /**
   * @brief Sets the sampling mode used when scaling the image.
   *
   * @param[in] samplingMode The sampling mode to use
   * @return Reference to this for fluent chaining
   */
  void SetSamplingMode(Ui::Image::SamplingMode samplingMode);

  /**
   * @brief Gets the sampling mode.
   *
   * @return The current sampling mode
   */
  Ui::Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets whether the image uses pre-multiplied alpha.
   *
   * @param[in] preMultiplied True if the image has pre-multiplied alpha
   * @return Reference to this for fluent chaining
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @brief Gets whether pre-multiplied alpha is enabled.
   *
   * @return True if pre-multiplied alpha is enabled
   */
  bool IsPreMultipliedAlpha() const;

public: // Loading Behavior
  /**
   * @brief Sets the desired image width used as a hint for the image loader.
   *
   * @param[in] width The desired width in pixels (0 to use natural size)
   * @return Reference to this for fluent chaining
   */
  void SetDesiredWidth(int width);

  /**
   * @brief Gets the desired image width hint.
   *
   * @return The desired width in pixels, or 0 if not set
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the desired image height used as a hint for the image loader.
   *
   * @param[in] height The desired height in pixels (0 to use natural size)
   * @return Reference to this for fluent chaining
   */
  void SetDesiredHeight(int height);

  /**
   * @brief Gets the desired image height hint.
   *
   * @return The desired height in pixels, or 0 if not set
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the load policy for the image resource.
   *
   * @param[in] loadPolicy The load policy to use
   * @return Reference to this for fluent chaining
   */
  void SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the load policy.
   *
   * @return The current load policy
   */
  Ui::Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets the release policy for the image resource.
   *
   * @param[in] releasePolicy The release policy to use
   * @return Reference to this for fluent chaining
   */
  void SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy);

  /**
   * @brief Gets the release policy.
   *
   * @return The current release policy
   */
  Ui::Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets whether the image is loaded synchronously.
   *
   * @param[in] synchronous True to load the image on the main thread synchronously
   * @return Reference to this for fluent chaining
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets whether synchronous loading is enabled.
   *
   * @return True if synchronous loading is enabled
   */
  bool IsSynchronousLoading() const;

public: // Advanced Rendering & Masking
  /**
   * @brief Sets the URL of an alpha mask image.
   *
   * @param[in] maskUrl The URL of the alpha mask image
   * @return Reference to this for fluent chaining
   */
  void SetAlphaMaskUrl(const Dali::String& maskUrl);

  /**
   * @brief Gets the alpha mask image URL.
   *
   * @return The URL of the alpha mask image, or empty string if not set
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @brief Sets whether the image should be cropped to the mask bounds.
   *
   * @param[in] cropToMask True to crop the image to the mask
   * @return Reference to this for fluent chaining
   */
  void SetCropToMask(bool cropToMask);

  /**
   * @brief Gets whether crop-to-mask is enabled.
   *
   * @return True if crop-to-mask is enabled
   */
  bool IsCropToMask() const;

  /**
   * @brief Sets the masking mode.
   *
   * @param[in] maskingMode The masking mode to use
   * @return Reference to this for fluent chaining
   */
  void SetMaskingMode(Ui::Image::MaskingType maskingMode);

  /**
   * @brief Gets the masking mode.
   *
   * @return The current masking mode
   */
  Ui::Image::MaskingType GetMaskingMode() const;

  /**
   * @brief Sets the URL of a placeholder image shown while loading.
   *
   * @param[in] url The URL of the placeholder image
   * @return Reference to this for fluent chaining
   */
  void SetPlaceholderUrl(const Dali::String& url);

  /**
   * @brief Gets the placeholder image URL.
   *
   * @return The URL of the placeholder image, or empty string if not set
   */
  Dali::String GetPlaceholderUrl() const;

  /**
   * @brief Sets the pixel area used to display a sub-region of the image.
   *
   * The area is specified as normalized coordinates: (x, y, width, height)
   * where each component is in the range [0, 1].
   *
   * @param[in] pixelArea The normalized sub-region of the image to display
   * @return Reference to this for fluent chaining
   */
  void SetPixelArea(const Vector4& pixelArea);

  /**
   * @brief Gets the pixel area.
   *
   * @return The current pixel area as normalized coordinates
   */
  Vector4 GetPixelArea() const;

public: // Loading Status & Signals
  /**
   * @brief Gets the current loading status of the image.
   *
   * @return The current ResourceStatus of the image visual
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  /**
   * @brief Returns the signal emitted when the image has finished loading and is ready to display.
   *
   * @return A reference to the ResourceReady signal
   */
  ResourceReadySignalType& ResourceReadySignal();

  /**
   * @brief Returns the signal emitted when the animation finishes all loops.
   *
   * @return A reference to the AnimationFinished signal
   */
  AnimationFinishedSignalType& AnimationFinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The AnimatedImageView implementation
   */
  explicit DALI_UI_API AnimatedImageView(Integration::AnimatedImageViewImpl& implementation);

  /**
   * @brief Allows the creation of this AnimatedImageView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API AnimatedImageView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} // namespace Dali
