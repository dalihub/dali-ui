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
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-types.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class LottieAnimationViewImpl;
}

/**
 * @brief LottieAnimationView is a View for displaying Lottie vector animations.
 *
 * It renders JSON-based Lottie animation files and supports playback control
 * (play, pause, stop), loop configuration, and frame-range control.
 *
 * @code
 * LottieAnimationView view = LottieAnimationView::New("animation.json");
 * view.SetLoopCount(-1); // infinite
 * view.Play();
 * @endcode
 */
class DALI_UI_API LottieAnimationView : public View
{
public:
  /**
   * @brief Property indices for LottieAnimationView.
   *
   * These can be used with Dali::Handle::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      IMAGE                      = LottieAnimationViewPropertyIndex::IMAGE,
      LOOP_COUNT                 = LottieAnimationViewPropertyIndex::LOOP_COUNT,
      LOOPING_MODE               = LottieAnimationViewPropertyIndex::LOOPING_MODE,
      STOP_BEHAVIOR              = LottieAnimationViewPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR         = LottieAnimationViewPropertyIndex::FRAME_SPEED_FACTOR,
      IMAGE_COLOR                = LottieAnimationViewPropertyIndex::IMAGE_COLOR,
      DESIRED_WIDTH              = LottieAnimationViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT             = LottieAnimationViewPropertyIndex::DESIRED_HEIGHT,
      RELEASE_POLICY             = LottieAnimationViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING        = LottieAnimationViewPropertyIndex::SYNCHRONOUS_LOADING,
      REDRAW_IN_SCALING_DOWN     = LottieAnimationViewPropertyIndex::REDRAW_IN_SCALING_DOWN,
      REDRAW_IN_SCALING_UP       = LottieAnimationViewPropertyIndex::REDRAW_IN_SCALING_UP,
      ENABLE_FRAME_CACHE         = LottieAnimationViewPropertyIndex::ENABLE_FRAME_CACHE,
      NOTIFY_AFTER_RASTERIZATION = LottieAnimationViewPropertyIndex::NOTIFY_AFTER_RASTERIZATION,
      RENDER_SCALE               = LottieAnimationViewPropertyIndex::RENDER_SCALE,
      PLACEHOLDER_IMAGE          = LottieAnimationViewPropertyIndex::PLACEHOLDER_IMAGE,
      PIXEL_AREA                 = LottieAnimationViewPropertyIndex::PIXEL_AREA,
      ENABLE_ASPECT_FIT          = LottieAnimationViewPropertyIndex::ENABLE_ASPECT_FIT,
    };
  };

  /// @brief Animation finished signal type. Emitted when the animation completes all loops.
  typedef Signal<void(View)> AnimationFinishedSignalType;

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized LottieAnimationView handle.
   */
  LottieAnimationView();

  /**
   * @brief Creates an initialized LottieAnimationView.
   *
   * @param[in] url The URL of the Lottie JSON file. Defaults to empty string.
   * @return A handle to a newly allocated Dali resource
   */
  static LottieAnimationView New(const Dali::String& url = "");

  /**
   * @brief Virtual destructor.
   */
  ~LottieAnimationView();

  /**
   * @brief Copy constructor.
   * @param[in] rhs Handle to copy
   */
  LottieAnimationView(const LottieAnimationView& rhs);

  /**
   * @brief Move constructor.
   * @param[in] rhs Handle to move
   */
  LottieAnimationView(LottieAnimationView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LottieAnimationView& operator=(const LottieAnimationView& rhs);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LottieAnimationView& operator=(LottieAnimationView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(LottieAnimationView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to LottieAnimationView handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a LottieAnimationView or an uninitialized handle
   */
  static LottieAnimationView DownCast(BaseHandle handle);

public: // Image
  /**
   * @brief Sets the resource URL of the Lottie animation file.
   *
   * @param[in] url The URL of the Lottie JSON file
   * @return Reference to this for fluent chaining
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @brief Gets the current resource URL.
   *
   * @return The URL of the Lottie JSON file currently set on this view
   */
  Dali::String GetResourceUrl() const;

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
   * @brief Stops playback of the animation.
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

  /**
   * @brief Jumps to the specified frame number.
   *
   * @param[in] frame The frame index to jump to
   * @return Reference to this for fluent chaining
   */
  void JumpToFrame(int frame);

public: // Frame Range
  /**
   * @brief Sets the playback range by frame numbers.
   *
   * The animation will play between @p minFrame and @p maxFrame.
   *
   * @param[in] minFrame The start frame index
   * @param[in] maxFrame The end frame index
   * @return Reference to this for fluent chaining
   */
  void SetMinMaxFrame(int minFrame, int maxFrame);

  /**
   * @brief Sets the playback range using marker names embedded in the Lottie file.
   *
   * If only @p minMarker is given, the animation plays the range of that marker.
   * If both are given, the animation plays from the start of @p minMarker to the
   * end of @p maxMarker.
   *
   * @param[in] minMarker Name of the start marker
   * @param[in] maxMarker Name of the end marker (optional)
   * @return Reference to this for fluent chaining
   */
  void SetMinMaxFrameByMarker(const Dali::String& minMarker, const Dali::String& maxMarker = "");

public: // Playback Options
  /**
   * @brief Sets the behavior of the animation when it is stopped.
   *
   * @param[in] behavior The stop behavior
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
   * @brief Sets the looping mode.
   *
   * @param[in] mode RESTART or AUTO_REVERSE
   * @return Reference to this for fluent chaining
   */
  void SetLoopingMode(LottieAnimation::LoopingMode mode);

  /**
   * @brief Gets the current looping mode.
   *
   * @return The current looping mode
   */
  LottieAnimation::LoopingMode GetLoopingMode() const;

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

public: // State Queries (read-only, requires live visual)
  /**
   * @brief Gets the current playback state of the animation.
   *
   * @return The current PlayState
   */
  AnimatedImage::PlayState GetPlayState() const;

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

public: // Performance Options
  /**
   * @brief Sets whether to redraw when the visual is scaled down.
   *
   * @param[in] redraw True to redraw on scale-down (default: true)
   * @return Reference to this for fluent chaining
   */
  void SetRedrawOnScaleDown(bool redraw);

  /**
   * @brief Returns whether redraw on scale-down is enabled.
   *
   * @return True if redraw on scale-down is enabled
   */
  bool IsRedrawOnScaleDown() const;

  /**
   * @brief Sets whether to redraw when the visual is scaled up.
   *
   * @param[in] redraw True to redraw on scale-up (default: true)
   * @return Reference to this for fluent chaining
   */
  void SetRedrawOnScaleUp(bool redraw);

  /**
   * @brief Returns whether redraw on scale-up is enabled.
   *
   * @return True if redraw on scale-up is enabled
   */
  bool IsRedrawOnScaleUp() const;

  /**
   * @brief Enables or disables frame caching.
   *
   * When enabled, all decoded frames are cached in memory to reduce CPU cost
   * during looping, at the expense of higher memory usage.
   *
   * @param[in] enable True to enable frame caching (default: false)
   * @return Reference to this for fluent chaining
   */
  void SetFrameCacheEnabled(bool enable);

  /**
   * @brief Returns whether frame caching is enabled.
   *
   * @return True if frame caching is enabled
   */
  bool IsFrameCacheEnabled() const;

  /**
   * @brief Sets whether to notify the render thread after each rasterization.
   *
   * Useful for low-fps Lottie files to avoid unnecessary render thread wakeups.
   *
   * @param[in] notify True to notify after rasterization (default: false)
   * @return Reference to this for fluent chaining
   */
  void SetNotifyAfterRasterization(bool notify);

  /**
   * @brief Returns whether notify-after-rasterization is enabled.
   *
   * @return True if notify-after-rasterization is enabled
   */
  bool IsNotifyAfterRasterizationEnabled() const;

  /**
   * @brief Sets the scale factor applied to the rasterization size.
   *
   * A value of 2.0 rasterizes at twice the visual dimensions, producing
   * sharper output on high-density displays at the cost of higher memory usage.
   * Negative values flip the image. The actual clamping is handled by the
   * underlying animation renderer.
   *
   * @param[in] scale The render scale multiplier (default: 1.0)
   * @return Reference to this for fluent chaining
   */
  void SetRenderScale(float scale);

  /**
   * @brief Gets the current render scale factor.
   *
   * @return The render scale (default: 1.0)
   */
  float GetRenderScale() const;

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
   * @brief Gets whether aspect fit scaling is enabled.
   *
   * @return True if aspect fit is enabled, false otherwise (default: true)
   */
  bool IsAspectFitEnabled() const;

public: // Content Info
  /**
   * @brief Gets layer information embedded in the Lottie file.
   *
   * The returned map contains layer names as keys and a two-element integer
   * array [startFrame, endFrame] as values.
   *
   * @return A Property::Map of layer info
   */
  Dali::Property::Map GetContentInfo();

  /**
   * @brief Gets marker information embedded in the Lottie file.
   *
   * The returned map contains marker names as keys and a two-element integer
   * array [startFrame, endFrame] as values.
   *
   * @return A Property::Map of marker info
   */
  Dali::Property::Map GetMarkerInfo();

public: // Advanced
  /**
   * @brief Sets a per-frame dynamic property callback on a specific layer/element.
   *
   * Ownership of DynamicPropertyInfo::callback is transferred to the visual.
   *
   * @param[in] info The dynamic property info
   * @return Reference to this for fluent chaining
   */
  void SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info);

public: // Visual Appearance
  /**
   * @brief Sets the color multiplier applied to the entire animation.
   *
   * For per-layer color control, use SetDynamicProperty() instead.
   *
   * @param[in] color The RGBA color to multiply with the animation
   * @return Reference to this for fluent chaining
   */
  void SetImageColor(const UiColor& color);

  /**
   * @brief Gets the color multiplier applied to the animation.
   *
   * @return The current image color
   */
  UiColor GetImageColor() const;

public: // Loading Behavior
  /**
   * @brief Sets the desired rasterization width as a hint for the renderer.
   *
   * @param[in] width The desired width in pixels (0 to use natural size)
   * @return Reference to this for fluent chaining
   */
  void SetDesiredWidth(int width);

  /**
   * @brief Gets the desired rasterization width hint.
   *
   * @return The desired width in pixels, or 0 if not set
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the desired rasterization height as a hint for the renderer.
   *
   * @param[in] height The desired height in pixels (0 to use natural size)
   * @return Reference to this for fluent chaining
   */
  void SetDesiredHeight(int height);

  /**
   * @brief Gets the desired rasterization height hint.
   *
   * @return The desired height in pixels, or 0 if not set
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the release policy for the animation resource.
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
   * @brief Sets whether the animation JSON is loaded synchronously.
   *
   * @param[in] synchronous True to load synchronously on the main thread
   * @return Reference to this for fluent chaining
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets whether synchronous loading is enabled.
   *
   * @return True if synchronous loading is enabled
   */
  bool IsSynchronousLoading() const;

public: // Placeholder
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
   * @brief Sets the pixel area used to display a sub-region of the animation.
   *
   * The area is specified as normalized coordinates: (x, y, width, height)
   * where each component is in the range [0, 1].
   *
   * @param[in] pixelArea The normalized sub-region of the animation to display
   * @return Reference to this for fluent chaining
   */
  void SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the pixel area.
   *
   * @return The current pixel area as normalized coordinates
   */
  Dali::Vector4 GetPixelArea() const;

public: // Loading Status & Signals
  /**
   * @brief Gets the current loading status of the animation.
   *
   * @return The current ResourceStatus of the animation visual
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

  /**
   * @brief Returns the signal emitted when the image has finished loading.
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
   * @param[in] implementation The LottieAnimationView implementation
   */
  explicit LottieAnimationView(Integration::LottieAnimationViewImpl& implementation);

  /**
   * @brief Allows the creation of this LottieAnimationView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit LottieAnimationView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} // namespace Dali
