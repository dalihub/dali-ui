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
#include <dali-ui-foundation/public-api/image-view-properties.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class ImageViewImpl;
}

/**
 * @brief ImageView is a View for displaying an image resource.
 *
 * It supports a variety of image formats and provides controls for fitting,
 * sampling, masking, loading policy, and N-patch borders.
 *
 * The image is loaded asynchronously by default. Use ResourceReadySignal()
 * to be notified when the image has finished loading.
 *
 * @code
 * ImageView view = ImageView::New("image.png");
 * view.ResourceReadySignal().Connect(...);
 * @endcode
 */
class DALI_UI_API ImageView : public View
{
public:
  /**
   * @brief Property indices for ImageView.
   *
   * These can be used with Dali::Handle::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      IMAGE                     = ImageViewPropertyIndex::IMAGE,
      FITTING_MODE              = ImageViewPropertyIndex::FITTING_MODE,
      SAMPLING_MODE             = ImageViewPropertyIndex::SAMPLING_MODE,
      DESIRED_WIDTH             = ImageViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT            = ImageViewPropertyIndex::DESIRED_HEIGHT,
      IMAGE_COLOR               = ImageViewPropertyIndex::IMAGE_COLOR,
      PRE_MULTIPLIED_ALPHA      = ImageViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      PLACEHOLDER_IMAGE         = ImageViewPropertyIndex::PLACEHOLDER_IMAGE,
      IMAGE_LOAD_WITH_VIEW_SIZE = ImageViewPropertyIndex::IMAGE_LOAD_WITH_VIEW_SIZE,
      ALPHA_MASK_URL            = ImageViewPropertyIndex::ALPHA_MASK_URL,
      CROP_TO_MASK              = ImageViewPropertyIndex::CROP_TO_MASK,
      MASKING_MODE              = ImageViewPropertyIndex::MASKING_MODE,
      LOAD_POLICY               = ImageViewPropertyIndex::LOAD_POLICY,
      RELEASE_POLICY            = ImageViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING       = ImageViewPropertyIndex::SYNCHRONOUS_LOADING,
      FAST_TRACK_UPLOADING      = ImageViewPropertyIndex::FAST_TRACK_UPLOADING,
      ORIENTATION_CORRECTION    = ImageViewPropertyIndex::ORIENTATION_CORRECTION,
      N_PATCH_BORDER            = ImageViewPropertyIndex::N_PATCH_BORDER,
      N_PATCH_BORDER_ONLY       = ImageViewPropertyIndex::N_PATCH_BORDER_ONLY,
      PIXEL_AREA                = ImageViewPropertyIndex::PIXEL_AREA,
    };
  };

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized ImageView handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  ImageView();

  /**
   * @brief Creates an initialized ImageView.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static ImageView New();

  /**
   * @brief Creates an initialized ImageView with a given URL.
   *
   * @param[in] url The URL of the image resource to display
   * @return A handle to a newly allocated Dali resource
   */
  static ImageView New(const Dali::String& url);

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ImageView();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] imageView Handle to copy
   */
  ImageView(const ImageView& imageView);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  ImageView(ImageView&& rhs) noexcept;

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] imageView Object to assign this to
   * @return Reference to this
   */
  ImageView& operator=(const ImageView& imageView);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  ImageView& operator=(ImageView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(ImageView)

public: // Static Methods
  /**
   * @brief Downcasts a handle to ImageView handle.
   *
   * If handle points to an ImageView, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to an ImageView or an uninitialized handle
   */
  static ImageView DownCast(BaseHandle handle);

public: // Image
  /**
   * @brief Sets the resource URL of the image to display.
   *
   * The image is loaded asynchronously. Use ResourceReadySignal() to be
   * notified when loading completes.
   *
   * @param[in] url The URL of the image resource
   * @return Reference to this for fluent chaining
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @brief Gets the current image URL.
   *
   * @return The URL of the image currently set on this view
   */
  Dali::String GetResourceUrl() const;

  /**
   * @brief Reloads the current image from its URL.
   *
   * Has no effect if no image URL has been set.
   */
  void Reload();

  /**
   * @brief Sets the URL of a placeholder image shown while the main image is loading.
   *
   * @param[in] url The URL of the placeholder image resource
   * @return Reference to this for fluent chaining
   */
  void SetPlaceholderUrl(const Dali::String& url);

  /**
   * @brief Gets the placeholder image URL.
   *
   * @return The URL of the placeholder image, or an empty string if not set
   */
  Dali::String GetPlaceholderUrl() const;

  /**
   * @brief Sets the color multiplier applied to the image.
   *
   * Accepts both direct RGBA values and theme color tokens via UiColor.
   *
   * @param[in] color The color to multiply with the image
   * @return Reference to this for fluent chaining
   */
  void SetImageColor(const UiColor& color);

  /**
   * @brief Gets the color multiplier applied to the image.
   *
   * @return The current image color
   */
  UiColor GetImageColor() const;

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

public: // Size & Fitting Control
  /**
   * @brief Sets the sampling mode used when scaling the image.
   *
   * @param[in] samplingMode The sampling mode to use
   * @return Reference to this for fluent chaining
   */
  void SetSamplingMode(Image::SamplingMode samplingMode);

  /**
   * @brief Gets the sampling mode.
   *
   * @return The current sampling mode
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets how the image is fitted within the view bounds.
   *
   * The default fitting mode is Ui::Image::FittingMode::FILL (stretch to fill).
   *
   * @param[in] fittingMode The fitting mode to use
   * @return Reference to this for fluent chaining
   */
  void SetFittingMode(Image::FittingMode fittingMode);

  /**
   * @brief Gets the fitting mode.
   *
   * @return The current fitting mode (default: FILL)
   */
  Image::FittingMode GetFittingMode() const;

  /**
   * @brief Sets the desired rasterization width as a hint for the image loader.
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
   * @brief Sets the desired rasterization height as a hint for the image loader.
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
   * @brief Sets whether the image is loaded synchronously at the current view size.
   *
   * When enabled, the image is (re)loaded at the view's resolved layout size each time
   * the view size changes. This avoids loading the full-resolution image when only a
   * smaller display size is needed, saving memory and decode time.
   *
   * Direction: view size → image load size.
   *
   * @note Do NOT use together with aspect-ratio layout on the same ImageView.
   *       ImageLoadWithViewSize causes GetNaturalSize to return the current view size
   *       instead of the true image dimensions, making aspect-ratio adjustment ineffective.
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

public: // Advanced Rendering & Masking
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

  /**
   * @brief Sets the URL of an alpha mask image.
   *
   * When set, the alpha channel of the mask image is applied to the main image.
   *
   * @param[in] maskUrl The URL of the alpha mask image
   * @return Reference to this for fluent chaining
   */
  void SetAlphaMaskUrl(const Dali::String& maskUrl);

  /**
   * @brief Gets the alpha mask image URL.
   *
   * @return The URL of the alpha mask image, or an empty string if not set
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
  void SetMaskingMode(Image::MaskingType maskingMode);

  /**
   * @brief Gets the masking mode.
   *
   * @return The current masking mode
   */
  Image::MaskingType GetMaskingMode() const;

public: // Loading Behavior
  /**
   * @brief Sets the load policy for the image resource.
   *
   * Determines whether the image is loaded immediately when the view is created (IMMEDIATE),
   * or deferred until the view is attached to the scene (ATTACHED).
   *
   * @param[in] loadPolicy The load policy to use
   * @return Reference to this for fluent chaining
   */
  void SetLoadPolicy(Image::LoadPolicy loadPolicy);

  /**
   * @brief Gets the load policy.
   *
   * @return The current load policy
   */
  Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @brief Sets the release policy for the image resource.
   *
   * Controls when the image texture is released from memory.
   *
   * @param[in] releasePolicy The release policy to use
   * @return Reference to this for fluent chaining
   */
  void SetReleasePolicy(Image::ReleasePolicy releasePolicy);

  /**
   * @brief Gets the release policy.
   *
   * @return The current release policy
   */
  Image::ReleasePolicy GetReleasePolicy() const;

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

  /**
   * @brief Sets whether fast-track uploading is enabled.
   *
   * When enabled, the image is uploaded to the GPU on a background thread
   * to reduce main-thread stalls.
   *
   * @param[in] fastTrack True to enable fast-track uploading
   * @return Reference to this for fluent chaining
   */
  void SetFastTrackUpload(bool fastTrack);

  /**
   * @brief Gets whether fast-track uploading is enabled.
   *
   * @return True if fast-track uploading is enabled
   */
  bool IsFastTrackUploadEnabled() const;

  /**
   * @brief Sets whether EXIF orientation metadata is applied automatically.
   *
   * @param[in] orientationCorrection True to apply orientation correction
   * @return Reference to this for fluent chaining
   */
  void SetOrientationCorrection(bool orientationCorrection);

  /**
   * @brief Gets whether orientation correction is enabled.
   *
   * @return True if orientation correction is enabled
   */
  bool IsOrientationCorrectionEnabled() const;

public: // N-Patch Border
  /**
   * @brief Sets the N-patch border insets.
   *
   * The border is specified as (left, top, right, bottom) pixel values.
   * Setting a non-zero border activates N-patch rendering.
   *
   * @param[in] border The border insets as (left, top, right, bottom)
   * @return Reference to this for fluent chaining
   */
  void SetNPatchBorder(const Vector4& border);

  /**
   * @brief Gets the N-patch border insets.
   *
   * @return The current border as (left, top, right, bottom)
   */
  Vector4 GetNPatchBorder() const;

  /**
   * @brief Sets whether only the N-patch border regions are rendered.
   *
   * When enabled, the center region of the N-patch is not rendered.
   *
   * @param[in] borderOnly True to render only the border regions
   * @return Reference to this for fluent chaining
   */
  void SetNPatchBorderOnly(bool borderOnly);

  /**
   * @brief Gets whether border-only rendering is enabled.
   *
   * @return True if only the border regions are rendered
   */
  bool IsNPatchBorderOnly() const;

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

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The ImageView implementation
   */
  explicit DALI_UI_API ImageView(Integration::ImageViewImpl& implementation);

  /**
   * @brief Allows the creation of this ImageView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API ImageView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} // namespace Dali
