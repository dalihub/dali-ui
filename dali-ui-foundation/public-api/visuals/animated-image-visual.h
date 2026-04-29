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
#include <dali/public-api/object/property-array.h> ///< Dali::Property::Array
#include <dali/public-api/rendering/sampling.h>    ///< Dali::WrapMode::Type

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
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

#include "animated-image-visual.autogen.h"
/**
 * @brief AnimatedImageVisual is a owner of Visual::Base with Visual::ANIMATED_IMAGE, to render sequence of images.
 *
 * It can use CornerRadius / CornerSquareness / Borderline feature
 *
 */
class DALI_UI_API AnimatedImageVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for AnimatedImageVisual.
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
      PRE_MULTIPLIED_ALPHA   = ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA,
      ALPHA_MASK_URL         = ImageVisualPropertyIndex::ALPHA_MASK_URL,
      MASK_CONTENT_SCALE     = ImageVisualPropertyIndex::MASK_CONTENT_SCALE,
      CROP_TO_MASK           = ImageVisualPropertyIndex::CROP_TO_MASK,
      MASKING_TYPE           = ImageVisualPropertyIndex::MASKING_TYPE,
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

      // For AnimatedImage
      BATCH_SIZE  = ImageVisualPropertyIndex::BATCH_SIZE,
      CACHE_SIZE  = ImageVisualPropertyIndex::CACHE_SIZE,
      FRAME_DELAY = ImageVisualPropertyIndex::FRAME_DELAY,

      // Read-only
      PLAY_STATE           = ImageVisualPropertyIndex::PLAY_STATE,
      CURRENT_FRAME_NUMBER = ImageVisualPropertyIndex::CURRENT_FRAME_NUMBER,
      TOTAL_FRAME_NUMBER   = ImageVisualPropertyIndex::TOTAL_FRAME_NUMBER,
    };
  };

public:
  /**
   * @brief Creates a AnimatedImageVisual object.
   *
   * @return The newly created image visual object.
   */
  static AnimatedImageVisual New();

public: // Setters for chaining
  // @CHAIN_START(AnimatedImageVisual, VisualBase)
  /**
   * @brief Gets the resource url of the AnimatedImageVisual.
   * @note Only valid value returns if SetResourceUrl called.
   * Return empty string if SetResourceUrl called.
   *
   * @return The resource url of the AnimatedImageVisual
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
  AnimatedImageVisual& SetResourceUrl(const Dali::String& resourceUrl);

  /**
   * @brief Gets the list of resource urls of the AnimatedImageVisual.
   * @note Only valid value returns if SetResourceUrlList called.
   * Return empty list if SetResourceUrl called.
   *
   * @return The list of resource urls of the AnimatedImageVisual
   */
  Dali::Vector<Dali::String> GetResourceUrlList() const;

  // @CHAIN_MANUAL
  /**
   * @brief Sets a list of resource URLs for an animated image sequence.
   *
   * This method cannot be used together with SetResourceUrl; they are mutually exclusive.
   * Provide multiple URLs to create an animation.
   *
   * @param[in] resourceUrlList The list of resource URLs to set
   */
  AnimatedImageVisual& SetResourceUrlList(std::initializer_list<Dali::String> resourceUrlList)
  {
    Dali::Property::Array array;
    array.Reserve(resourceUrlList.size());

    for(const auto& resourceUrl : resourceUrlList)
    {
      array.PushBack(resourceUrl);
    }
    SetProperty(Property::URL, array);
    return *this;
  }

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
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets the desired width of the AnimatedImageVisual.
   *
   * @return The desired width of the AnimatedImageVisual
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the desired width of the AnimatedImageVisual.
   *
   * @param[in] desiredWidth The desired width to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the AnimatedImageVisual.
   *
   * @return The desired height of the AnimatedImageVisual
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the desired height of the AnimatedImageVisual.
   *
   * @param[in] desiredHeight The desired height to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the AnimatedImageVisual.
   *
   * @return The sampling mode of the AnimatedImageVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the sampling mode of the AnimatedImageVisual.
   *
   * @param[in] samplingMode The sampling mode to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetSamplingMode(Image::SamplingMode samplingMode);

  /**
   * @brief Gets the pixel area of the AnimatedImageVisual.
   *
   * @return The pixel area of the AnimatedImageVisual
   */
  Dali::Vector4 GetPixelArea() const;

  /**
   * @brief Sets the pixel area of the AnimatedImageVisual.
   *
   * @param[in] pixelArea The pixel area to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the wrap mode for u coordinate of the AnimatedImageVisual.
   *
   * @return The wrap mode for u coordinate of the AnimatedImageVisual
   */
  Dali::WrapMode::Type GetWrapModeU() const;

  /**
   * @brief Sets the wrap mode for u coordinate of the AnimatedImageVisual.
   *
   * @param[in] wrapModeU The wrap mode for u coordinate to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetWrapModeU(Dali::WrapMode::Type wrapModeU);

  /**
   * @brief Gets the wrap mode for v coordinate of the AnimatedImageVisual.
   *
   * @return The wrap mode for v coordinate of the AnimatedImageVisual
   */
  Dali::WrapMode::Type GetWrapModeV() const;

  /**
   * @brief Sets the wrap mode for v coordinate of the AnimatedImageVisual.
   *
   * @param[in] wrapModeV The wrap mode for v coordinate to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetWrapModeV(Dali::WrapMode::Type wrapModeV);

  /**
   * @brief Gets whether pre-multiplied alpha is enabled.
   *
   * @return True if pre-multiplied alpha is enabled
   */
  bool IsPreMultipliedAlpha() const;

  /**
   * @brief Sets whether the image uses pre-multiplied alpha.
   *
   * @param[in] preMultiplied True if the image has pre-multiplied alpha
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @brief Gets the alpha mask url of the AnimatedImageVisual.
   *
   * @return The alpha mask url of the AnimatedImageVisual
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @brief Sets the alpha mask url of the AnimatedImageVisual.
   *
   * @param[in] alphaMaskUrl The alpha mask url to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetAlphaMaskUrl(const Dali::String& alphaMaskUrl);

  /**
   * @brief Gets the mask content scale of the AnimatedImageVisual.
   *
   * @return The mask content scale of the AnimatedImageVisual
   */
  float GetMaskContentScale() const;

  /**
   * @brief Sets the mask content scale of the AnimatedImageVisual.
   *
   * @param[in] maskContentScale The mask content scale to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetMaskContentScale(float maskContentScale);

  /**
   * @brief Gets whether crop to mask is enabled.
   *
   * @return True if crop to mask is enabled
   */
  bool IsCropToMask() const;

  /**
   * @brief Sets whether to crop to mask.
   *
   * @param[in] cropToMask True to crop to mask
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetCropToMask(bool cropToMask);

  /**
   * @brief Gets the masking type of the AnimatedImageVisual.
   *
   * @return The masking type of the AnimatedImageVisual
   */
  Image::MaskingType GetMaskingType() const;

  /**
   * @brief Sets the masking type of the AnimatedImageVisual.
   *
   * @param[in] maskingType The masking type to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetMaskingType(Image::MaskingType maskingType);

  /**
   * @brief Gets whether broken image is enabled.
   *
   * @return True if broken image is enabled
   */
  bool IsBrokenImageEnabled() const;

  /**
   * @brief Sets whether to enable broken image.
   *
   * @param[in] enableBrokenImage True to enable broken image
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetEnableBrokenImage(bool enableBrokenImage);

  /**
   * @brief Gets the load policy of the AnimatedImageVisual.
   *
   * @return The load policy of the AnimatedImageVisual
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets the load policy of the AnimatedImageVisual.
   *
   * @param[in] loadPolicy The load policy to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetLoadPolicy(Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the release policy of the AnimatedImageVisual.
   *
   * @return The release policy of the AnimatedImageVisual
   */
  Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets the release policy of the AnimatedImageVisual.
   *
   * @param[in] releasePolicy The release policy to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetReleasePolicy(Image::ReleasePolicy releasePolicy);

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
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetOrientationCorrection(bool orientationCorrection);

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
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetSynchronousSizing(bool synchronousSizing);

  /**
   * @brief Gets the loop count of the AnimatedImageVisual.
   *
   * @return The loop count of the AnimatedImageVisual
   */
  int GetLoopCount() const;

  /**
   * @brief Sets the loop count of the AnimatedImageVisual.
   *
   * @param[in] loopCount The loop count to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetLoopCount(int loopCount);

  /**
   * @brief Gets the play range of the AnimatedImageVisual.
   *
   * @return The play range of the AnimatedImageVisual
   */
  Dali::Property::Array GetPlayRange() const;

  /**
   * @brief Sets the play range of the AnimatedImageVisual.
   *
   * @param[in] playRange The play range to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetPlayRange(const Dali::Property::Array& playRange);

  /**
   * @brief Gets the stop behavior of the AnimatedImageVisual.
   *
   * @return The stop behavior of the AnimatedImageVisual
   */
  AnimatedImage::StopBehavior GetStopBehavior() const;

  /**
   * @brief Sets the stop behavior of the AnimatedImageVisual.
   *
   * @param[in] stopBehavior The stop behavior to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetStopBehavior(AnimatedImage::StopBehavior stopBehavior);

  /**
   * @brief Gets the frame speed factor of the AnimatedImageVisual.
   *
   * @return The frame speed factor of the AnimatedImageVisual
   */
  float GetFrameSpeedFactor() const;

  /**
   * @brief Sets the frame speed factor of the AnimatedImageVisual.
   *
   * @param[in] frameSpeedFactor The frame speed factor to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetFrameSpeedFactor(float frameSpeedFactor);

  /**
   * @brief Gets the batch size of the AnimatedImageVisual.
   *
   * @return The batch size of the AnimatedImageVisual
   */
  int GetBatchSize() const;

  /**
   * @brief Sets the batch size of the AnimatedImageVisual.
   *
   * @param[in] batchSize The batch size to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetBatchSize(int batchSize);

  /**
   * @brief Gets the cache size of the AnimatedImageVisual.
   *
   * @return The cache size of the AnimatedImageVisual
   */
  int GetCacheSize() const;

  /**
   * @brief Sets the cache size of the AnimatedImageVisual.
   *
   * @param[in] cacheSize The cache size to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetCacheSize(int cacheSize);

  /**
   * @brief Gets the frame delay of the AnimatedImageVisual.
   *
   * @return The frame delay of the AnimatedImageVisual
   */
  int GetFrameDelay() const;

  /**
   * @brief Sets the frame delay of the AnimatedImageVisual.
   *
   * This property is only relevant when using SetResourceUrlList (animated sequence).
   * It specifies the delay between frames in milliseconds.
   *
   * @param[in] frameDelay The frame delay to set
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& SetFrameDelay(int frameDelay);

  /**
   * @brief Gets the play state of the AnimatedImageVisual.
   *
   * @return The play state of the AnimatedImageVisual
   * @note This property is read-only.
   */
  AnimatedImage::PlayState GetPlayState() const;

  /**
   * @brief Gets the current frame number of the AnimatedImageVisual.
   *
   * @return The current frame number of the AnimatedImageVisual
   * @note This property is read-only.
   */
  int GetCurrentFrameNumber() const;

  /**
   * @brief Gets the total frame number of the AnimatedImageVisual.
   *
   * @return The total frame number of the AnimatedImageVisual. -1 if image decode is not completed yet.
   * @note This property is read-only.
   */
  int GetTotalFrameNumber() const;

public: // API to control play state
  /**
   * @brief Play animation
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& Play();

  /**
   * @brief Pause animation
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& Pause();

  /**
   * @brief Stop animation
   *
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& Stop();

  /**
   * @brief Jump to specific frame
   *
   * @param[in] frame The frame to jump
   * @return Reference to this for fluent chaining
   */
  AnimatedImageVisual& JumpTo(int frame);

  // @CHAIN_END

public:
  AnimatedImageVisual()                                              = default;
  ~AnimatedImageVisual()                                             = default;
  AnimatedImageVisual(const AnimatedImageVisual& rhs)                = default;
  AnimatedImageVisual& operator=(const AnimatedImageVisual& rhs)     = default;
  AnimatedImageVisual(AnimatedImageVisual&& rhs) noexcept            = default;
  AnimatedImageVisual& operator=(AnimatedImageVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL AnimatedImageVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
  DALI_UI_CHAIN_VISUALBASE_METHODS(AnimatedImageVisual)
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_ANIMATED_IMAGE_VISUAL_OBJECT_H
