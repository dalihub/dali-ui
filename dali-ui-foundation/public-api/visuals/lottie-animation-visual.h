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
#include <dali-ui-foundation/public-api/image/lottie-animation-enumerations.h>
#include <dali-ui-foundation/public-api/image/lottie-animation-types.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

// TODO : Seperate it as n-patch / animated-image / animated-vector-image

namespace Dali
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief LottieAnimationVisual is a owner of Visual::Base with Visual::ANIMATED_VECTOR_IMAGE, to render lottie image.
 *
 * It can use CornerRadius / CornerSquareness / Borderline feature.
 *
 */
class DALI_UI_API LottieAnimationVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for LottieAnimationVisual.
   *
   * These can be used with Dali::Ui::VisualBase::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      // For simple images
      URL                    = ImageVisualPropertyIndex::URL,
      SYNCHRONOUS_LOADING    = ImageVisualPropertyIndex::SYNCHRONOUS_LOADING,
      DESIRED_WIDTH          = ImageVisualPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT         = ImageVisualPropertyIndex::DESIRED_HEIGHT,
      SAMPLING_MODE          = ImageVisualPropertyIndex::SAMPLING_MODE,
      PIXEL_AREA             = ImageVisualPropertyIndex::PIXEL_AREA,
      WRAP_MODE_U            = ImageVisualPropertyIndex::WRAP_MODE_U,
      WRAP_MODE_V            = ImageVisualPropertyIndex::WRAP_MODE_V,
      ENABLE_BROKEN_IMAGE    = ImageVisualPropertyIndex::ENABLE_BROKEN_IMAGE,
      LOAD_POLICY            = ImageVisualPropertyIndex::LOAD_POLICY,
      RELEASE_POLICY         = ImageVisualPropertyIndex::RELEASE_POLICY,
      ORIENTATION_CORRECTION = ImageVisualPropertyIndex::ORIENTATION_CORRECTION,
      SYNCHRONOUS_SIZING     = ImageVisualPropertyIndex::SYNCHRONOUS_SIZING,

      // For both AnimatedImage and LottieAnimation
      LOOP_COUNT         = ImageVisualPropertyIndex::LOOP_COUNT,
      PLAY_RANGE         = ImageVisualPropertyIndex::PLAY_RANGE,
      STOP_BEHAVIOR      = ImageVisualPropertyIndex::STOP_BEHAVIOR,
      FRAME_SPEED_FACTOR = ImageVisualPropertyIndex::FRAME_SPEED_FACTOR,

      // For LottieAnimation
      LOOPING_MODE               = ImageVisualPropertyIndex::LOOPING_MODE,
      REDRAW_IN_SCALING_DOWN     = ImageVisualPropertyIndex::REDRAW_IN_SCALING_DOWN,
      REDRAW_IN_SCALING_UP       = ImageVisualPropertyIndex::REDRAW_IN_SCALING_UP,
      ENABLE_FRAME_CACHE         = ImageVisualPropertyIndex::ENABLE_FRAME_CACHE,
      NOTIFY_AFTER_RASTERIZATION = ImageVisualPropertyIndex::NOTIFY_AFTER_RASTERIZATION,
      RENDER_SCALE               = ImageVisualPropertyIndex::RENDER_SCALE,
      ENABLE_ASPECT_FIT          = ImageVisualPropertyIndex::ENABLE_ASPECT_FIT,

      // Read-only
      PLAY_STATE           = ImageVisualPropertyIndex::PLAY_STATE,
      CURRENT_FRAME_NUMBER = ImageVisualPropertyIndex::CURRENT_FRAME_NUMBER,
      TOTAL_FRAME_NUMBER   = ImageVisualPropertyIndex::TOTAL_FRAME_NUMBER,
      CONTENT_INFO         = ImageVisualPropertyIndex::CONTENT_INFO,
      MARKER_INFO          = ImageVisualPropertyIndex::MARKER_INFO,
    };
  };

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

public: // Setters
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
   * @brief Sets whether the image is loaded synchronously.
   *
   * @param[in] synchronous True to load the image on the main thread synchronously
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets the desired width of the LottieAnimationVisual.
   *
   * @return The desired width of the LottieAnimationVisual
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the desired width of the LottieAnimationVisual.
   *
   * @param[in] desiredWidth The desired width to set
   */
  void SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the LottieAnimationVisual.
   *
   * @return The desired height of the LottieAnimationVisual
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the desired height of the LottieAnimationVisual.
   *
   * @param[in] desiredHeight The desired height to set
   */
  void SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the LottieAnimationVisual.
   *
   * @return The sampling mode of the LottieAnimationVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the sampling mode of the LottieAnimationVisual.
   *
   * @param[in] samplingMode The sampling mode to set
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
   * @param[in] pixelArea The pixel area to set
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
   * @param[in] wrapModeU The wrap mode for u coordinate to set
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
   * @param[in] wrapModeV The wrap mode for v coordinate to set
   */
  void SetWrapModeV(Dali::WrapMode::Type wrapModeV);

  /**
   * @brief Gets whether broken image is enabled.
   *
   * @return True if broken image is enabled
   */
  bool IsBrokenImageEnabled() const;

  /**
   * @brief Sets whether to enable broken image.
   *
   * @param[in] brokenImageEnabled True to enable broken image
   */
  void SetBrokenImageEnabled(bool brokenImageEnabled);

  /**
   * @brief Gets the load policy of the LottieAnimationVisual.
   *
   * @return The load policy of the LottieAnimationVisual
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets the load policy of the LottieAnimationVisual.
   *
   * @param[in] loadPolicy The load policy to set
   */
  void SetLoadPolicy(Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the release policy of the LottieAnimationVisual.
   *
   * @return The release policy of the LottieAnimationVisual
   */
  Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets the release policy of the LottieAnimationVisual.
   *
   * @param[in] releasePolicy The release policy to set
   */
  void SetReleasePolicy(Image::ReleasePolicy releasePolicy);

  /**
   * @brief Gets whether orientation correction is enabled.
   *
   * @return True if orientation correction is enabled
   */
  bool IsOrientationCorrection() const;

  /**
   * @brief Sets whether to correct the image orientation.
   *
   * @param[in] orientationCorrection True to correct the image orientation
   */
  void SetOrientationCorrection(bool orientationCorrection);

  /**
   * @brief Gets whether synchronous sizing is enabled.
   *
   * @return True if synchronous sizing is enabled
   */
  bool IsSynchronousSizing() const;

  /**
   * @brief Sets whether to use synchronous sizing.
   *
   * @param[in] synchronousSizing True to use synchronous sizing
   */
  void SetSynchronousSizing(bool synchronousSizing);

  /**
   * @brief Gets the loop count of the LottieAnimationVisual.
   *
   * @return The loop count of the LottieAnimationVisual
   */
  int GetLoopCount() const;

  /**
   * @brief Sets the loop count of the LottieAnimationVisual.
   *
   * @param[in] loopCount The loop count to set
   */
  void SetLoopCount(int loopCount);

  /**
   * @brief Gets the play range of the LottieAnimationVisual.
   *
   * @return The play range of the LottieAnimationVisual
   */
  Dali::Property::Array GetPlayRange() const;

  /**
   * @brief Sets the play range of the LottieAnimationVisual.
   *
   * @param[in] playRange The play range to set
   */
  void SetPlayRange(const Dali::Property::Array& playRange);

  /**
   * @brief Gets the stop behavior of the LottieAnimationVisual.
   *
   * @return The stop behavior of the LottieAnimationVisual
   */
  AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @brief Sets the stop behavior of the LottieAnimationVisual.
   *
   * @param[in] stopBehavior The stop behavior to set
   */
  void SetStopBehavior(AnimatedImage::StopBehavior stopBehavior);

  /**
   * @brief Gets the frame speed factor of the LottieAnimationVisual.
   *
   * @return The frame speed factor of the LottieAnimationVisual
   */
  float GetFrameSpeedFactor() const;

  /**
   * @brief Sets the frame speed factor of the LottieAnimationVisual.
   *
   * @param[in] frameSpeedFactor The frame speed factor to set
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
   * @brief Gets whether redraw in scaling down is enabled.
   *
   * @return True if redraw in scaling down is enabled
   */
  bool IsRedrawInScalingDown() const;

  /**
   * @brief Sets whether to redraw when the visual is scaled down.
   *
   * @param[in] redrawInScalingDown True to redraw when scaled down
   */
  void SetRedrawInScalingDown(bool redrawInScalingDown);

  /**
   * @brief Gets whether redraw in scaling up is enabled.
   *
   * @return True if redraw in scaling up is enabled
   */
  bool IsRedrawInScalingUp() const;

  /**
   * @brief Sets whether to redraw when the visual is scaled up.
   *
   * @param[in] redrawInScalingUp True to redraw when scaled up
   */
  void SetRedrawInScalingUp(bool redrawInScalingUp);

  /**
   * @brief Gets whether frame cache is enabled.
   *
   * @return True if frame cache is enabled
   */
  bool IsFrameCacheEnabled() const;

  /**
   * @brief Sets whether to enable frame cache.
   *
   * @param[in] frameCacheEnabled True to enable frame cache
   */
  void SetFrameCacheEnabled(bool frameCacheEnabled);

  /**
   * @brief Gets whether notify after rasterization is enabled.
   *
   * @return True if notify after rasterization is enabled
   */
  bool IsNotifyAfterRasterization() const;

  /**
   * @brief Sets whether to notify after rasterization.
   *
   * @param[in] notifyAfterRasterization True to notify after rasterization
   */
  void SetNotifyAfterRasterization(bool notifyAfterRasterization);

  /**
   * @brief Gets the render scale of the LottieAnimationVisual.
   *
   * @return The render scale of the LottieAnimationVisual
   */
  float GetRenderScale() const;

  /**
   * @brief Sets the render scale of the LottieAnimationVisual.
   *
   * @param[in] renderScale The render scale to set
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
   * @brief Gets the play state of the LottieAnimationVisual.
   *
   * @return The play state of the LottieAnimationVisual
   * @note This property is read-only.
   */
  AnimatedImage::PlayState GetPlayState() const;

  /**
   * @brief Gets the current frame number of the LottieAnimationVisual.
   *
   * @return The current frame number of the LottieAnimationVisual
   * @note This property is read-only.
   */
  int GetCurrentFrameNumber() const;

  /**
   * @brief Gets the total frame number of the LottieAnimationVisual.
   *
   * @return The total frame number of the LottieAnimationVisual. -1 if image decode is not completed yet.
   * @note This property is read-only.
   */
  int GetTotalFrameNumber() const;

  /**
   * @brief Gets the content info of the LottieAnimationVisual.
   *
   * @return The content info of the LottieAnimationVisual
   * @note This property is read-only.
   */
  Dali::Property::Map GetContentInfo() const;

  /**
   * @brief Gets the marker info of the LottieAnimationVisual.
   *
   * @return The marker info of the LottieAnimationVisual
   * @note This property is read-only.
   */
  Dali::Property::Map GetMarkerInfo() const;

public: // API to control play state
  /**
   * @brief Play animation
   *
   */
  void Play();

  /**
   * @brief Pause animation
   *
   */
  void Pause();

  /**
   * @brief Stop animation
   *
   */
  void Stop();

  /**
   * @brief Jump to specific frame
   *
   * @param[in] frame The frame to jump
   */
  void JumpTo(int frame);

public: // Advanced
  /**
   * @brief Sets a per-frame dynamic property callback on a specific layer/element.
   *
   * Ownership of DynamicPropertyInfo::callback is transferred to the visual.
   *
   * @param[in] info The dynamic property info
   */
  void SetDynamicProperty(const LottieAnimation::DynamicPropertyInfo& info);

public:
  LottieAnimationVisual()                                                = default;
  ~LottieAnimationVisual()                                               = default;
  LottieAnimationVisual(const LottieAnimationVisual& rhs)                = default;
  LottieAnimationVisual& operator=(const LottieAnimationVisual& rhs)     = default;
  LottieAnimationVisual(LottieAnimationVisual&& rhs) noexcept            = default;
  LottieAnimationVisual& operator=(LottieAnimationVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL LottieAnimationVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_IMAGE_VISUAL_OBJECT_H
