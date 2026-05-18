#ifndef DALI_UI_IMAGE_VISUAL_OBJECT_H
#define DALI_UI_IMAGE_VISUAL_OBJECT_H

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

#include "image-visual.autogen.h"
/**
 * @brief ImageVisual is a owner of Visual::Base with Visual::IMAGE, to render simple image.
 *
 * It can use CornerRadius / CornerSquareness / Borderline feature only if image is not n-patch image
 * or set BORDER property.
 *
 */
class DALI_UI_API ImageVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for ImageVisual.
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
      FITTING_MODE           = ImageVisualPropertyIndex::FITTING_MODE,
      ORIENTATION_CORRECTION = ImageVisualPropertyIndex::ORIENTATION_CORRECTION,
      SYNCHRONOUS_SIZING     = ImageVisualPropertyIndex::SYNCHRONOUS_SIZING,

      // For Image
      FAST_TRACK_UPLOADING = ImageVisualPropertyIndex::FAST_TRACK_UPLOADING,

      // For n-patch images
      BORDER                = ImageVisualPropertyIndex::BORDER,
      BORDER_ONLY           = ImageVisualPropertyIndex::BORDER_ONLY,
      AUXILIARY_IMAGE       = ImageVisualPropertyIndex::AUXILIARY_IMAGE,
      AUXILIARY_IMAGE_ALPHA = ImageVisualPropertyIndex::AUXILIARY_IMAGE_ALPHA,

      // For both Image and AnimatedImage
      PRE_MULTIPLIED_ALPHA = ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA,
      ALPHA_MASK_URL       = ImageVisualPropertyIndex::ALPHA_MASK_URL,
      MASK_CONTENT_SCALE   = ImageVisualPropertyIndex::MASK_CONTENT_SCALE,
      CROP_TO_MASK         = ImageVisualPropertyIndex::CROP_TO_MASK,
      MASKING_TYPE         = ImageVisualPropertyIndex::MASKING_TYPE,
    };
  };

public:
  /**
   * @brief Creates a ImageVisual object.
   *
   * @return The newly created image visual object.
   */
  static ImageVisual New();

  /**
   * @brief Downcasts a handle to ImageVisual handle.
   *
   * If handle points to a ImageVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a ImageVisual or an uninitialized handle
   */
  static ImageVisual DownCast(BaseHandle handle);

public: // Setters for chaining
  // @CHAIN_START(ImageVisual, VisualBase)
  /**
   * @brief Gets the resource url of the ImageVisual.
   *
   * @return The resource url of the ImageVisual
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Sets the resource url of the ImageVisual.
   *
   * @param[in] resourceUrl The resource url to set
   */
  ImageVisual& SetResourceUrl(const Dali::String& resourceUrl);

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
  ImageVisual& SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets the desired width of the ImageVisual.
   *
   * @return The desired width of the ImageVisual
   */
  int GetDesiredWidth() const;

  /**
   * @brief Sets the desired width of the ImageVisual.
   *
   * @param[in] desiredWidth The desired width to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the ImageVisual.
   *
   * @return The desired height of the ImageVisual
   */
  int GetDesiredHeight() const;

  /**
   * @brief Sets the desired height of the ImageVisual.
   *
   * @param[in] desiredHeight The desired height to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the ImageVisual.
   *
   * @return The sampling mode of the ImageVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the sampling mode of the ImageVisual.
   *
   * @param[in] samplingMode The sampling mode to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetSamplingMode(Image::SamplingMode samplingMode);

  /**
   * @brief Gets the pixel area of the ImageVisual.
   *
   * @return The pixel area of the ImageVisual
   */
  Dali::Vector4 GetPixelArea() const;

  /**
   * @brief Sets the pixel area of the ImageVisual.
   *
   * @param[in] pixelArea The pixel area to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the wrap mode for u coordinate of the ImageVisual.
   *
   * @return The wrap mode for u coordinate of the ImageVisual
   */
  Dali::WrapMode::Type GetWrapModeU() const;

  /**
   * @brief Sets the wrap mode for u coordinate of the ImageVisual.
   *
   * @param[in] wrapModeU The wrap mode for u coordinate to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetWrapModeU(Dali::WrapMode::Type wrapModeU);

  /**
   * @brief Gets the wrap mode for v coordinate of the ImageVisual.
   *
   * @return The wrap mode for v coordinate of the ImageVisual
   */
  Dali::WrapMode::Type GetWrapModeV() const;

  /**
   * @brief Sets the wrap mode for v coordinate of the ImageVisual.
   *
   * @param[in] wrapModeV The wrap mode for v coordinate to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetWrapModeV(Dali::WrapMode::Type wrapModeV);

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
  ImageVisual& SetEnableBrokenImage(bool enableBrokenImage);

  /**
   * @brief Gets the load policy of the ImageVisual.
   *
   * @return The load policy of the ImageVisual
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets the load policy of the ImageVisual.
   *
   * @param[in] loadPolicy The load policy to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetLoadPolicy(Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the release policy of the ImageVisual.
   *
   * @return The release policy of the ImageVisual
   */
  Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @brief Sets the release policy of the ImageVisual.
   *
   * @param[in] releasePolicy The release policy to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetReleasePolicy(Image::ReleasePolicy releasePolicy);

  /**
   * @brief Gets the fitting mode of the ImageVisual.
   *
   * @return The fitting mode of the ImageVisual
   */
  Image::FittingMode GetFittingMode() const;

  /**
   * @brief Sets the fitting mode of the ImageVisual.
   *
   * @param[in] fittingMode The fitting mode to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetFittingMode(Image::FittingMode fittingMode);

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
  ImageVisual& SetOrientationCorrection(bool orientationCorrection);

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
  ImageVisual& SetSynchronousSizing(bool synchronousSizing);

  /**
   * @brief Gets whether fast track uploading is enabled.
   *
   * @return True if fast track uploading is enabled
   */
  bool IsFastTrackUploading() const;

  /**
   * @brief Sets whether to use fast track uploading.
   *
   * @param[in] fastTrackUploading True to use fast track uploading
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetFastTrackUploading(bool fastTrackUploading);

  /**
   * @brief Gets the NPatch border of the ImageVisual.
   *
   * @return The NPatch border of the ImageVisual
   */
  Dali::Extents GetNPatchBorder() const;

  /**
   * @brief Sets the NPatch border of the ImageVisual.
   *
   * @param[in] border The NPatch border to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetNPatchBorder(const Dali::Extents& border);

  /**
   * @brief Gets whether NPatch border only is enabled.
   *
   * @return True if NPatch border only is enabled
   */
  bool IsNPatchBorderOnly() const;

  /**
   * @brief Sets whether to draw only the borders.
   *
   * @param[in] borderOnly True to draw only the borders
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetNPatchBorderOnly(bool borderOnly);

  /**
   * @brief Gets the auxiliary image url of the ImageVisual.
   *
   * @return The auxiliary image url of the ImageVisual
   */
  Dali::String GetNPatchAuxiliaryImage() const;

  /**
   * @brief Sets the auxiliary image url of the ImageVisual.
   * @note Only available if resource is n-patch, or NpatchBorder applied.
   *
   * @param[in] auxiliaryImage The auxiliary image url to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetNPatchAuxiliaryImage(const Dali::String& auxiliaryImage);

  /**
   * @brief Gets the auxiliary image alpha of the ImageVisual.
   * @note Only available if resource is n-patch, or NpatchBorder applied.
   *
   * @return The auxiliary image alpha of the ImageVisual
   */
  float GetNPatchAuxiliaryImageAlpha() const;

  /**
   * @brief Sets the auxiliary image alpha of the ImageVisual.
   * @note Only available if resource is n-patch, or NpatchBorder applied.
   *
   * @param[in] auxiliaryImageAlpha The auxiliary image alpha to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetNPatchAuxiliaryImageAlpha(float auxiliaryImageAlpha);

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
  ImageVisual& SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @brief Gets the alpha mask url of the ImageVisual.
   *
   * @return The alpha mask url of the ImageVisual
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @brief Sets the alpha mask url of the ImageVisual.
   *
   * @param[in] alphaMaskUrl The alpha mask url to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetAlphaMaskUrl(const Dali::String& alphaMaskUrl);

  /**
   * @brief Gets the mask content scale of the ImageVisual.
   *
   * @return The mask content scale of the ImageVisual
   */
  float GetMaskContentScale() const;

  /**
   * @brief Sets the mask content scale of the ImageVisual.
   *
   * @param[in] maskContentScale The mask content scale to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetMaskContentScale(float maskContentScale);

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
  ImageVisual& SetCropToMask(bool cropToMask);

  /**
   * @brief Gets the masking type of the ImageVisual.
   *
   * @return The masking type of the ImageVisual
   */
  Image::MaskingType GetMaskingType() const;

  /**
   * @brief Sets the masking type of the ImageVisual.
   *
   * @param[in] maskingType The masking type to set
   * @return Reference to this for fluent chaining
   */
  ImageVisual& SetMaskingType(Image::MaskingType maskingType);
  // @CHAIN_END

public:
  ImageVisual()                                      = default;
  ~ImageVisual()                                     = default;
  ImageVisual(const ImageVisual& rhs)                = default;
  ImageVisual& operator=(const ImageVisual& rhs)     = default;
  ImageVisual(ImageVisual&& rhs) noexcept            = default;
  ImageVisual& operator=(ImageVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ImageVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
  DALI_UI_CHAIN_VISUALBASE_METHODS(ImageVisual)
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_IMAGE_VISUAL_OBJECT_H
