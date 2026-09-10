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
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/image-view-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/insets.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
class ImageUrl;

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
      MASKING_POLICY            = ImageViewPropertyIndex::MASKING_POLICY,
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
   * @brief Creates an initialized ImageView that stores the given ImageUrl.
   *
   * The view keeps a reference to @p imageUrl until another resource is set or
   * the view is destroyed. The caller may reset its ImageUrl handle after
   * this method returns.
   *
   * @param[in] imageUrl The image resource handle
   * @return A handle to a newly allocated Dali resource
   */
  static ImageView New(const ImageUrl& imageUrl);

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
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @brief Sets an ImageUrl resource to display and stores its handle.
   *
   * The view keeps a reference to @p imageUrl until another resource is set or
   * the view is destroyed. The caller may therefore reset its ImageUrl handle
   * immediately after this method returns.
   *
   * @param[in] imageUrl The image resource handle
   */
  void SetResourceUrl(const ImageUrl& imageUrl);

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
   * @brief Returns the natural size of the image content.
   *
   * The returned size reflects the image Visual, including the desired size or
   * decoded texture size when applicable. It may differ from the laid-out View size.
   *
   * @return The natural size of the image content
   */
  Vector3 GetNaturalSize() const;

  /**
   * @brief Sets the sampling mode used when scaling the image.
   *
   * @param[in] samplingMode The sampling mode to use
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
   * @param[in] maskingPolicy The masking mode to use
   */
  void SetMaskingPolicy(Image::MaskingPolicy maskingPolicy);

  /**
   * @brief Gets the masking mode.
   *
   * @return The current masking mode
   */
  Image::MaskingPolicy GetMaskingPolicy() const;

  /**
   * @brief Sets when the alpha mask is applied.
   *
   * TODO: remove. Kept only so applications written against the old name keep
   * compiling.
   *
   * @param[in] maskingPolicy The masking policy to set
   * @see SetMaskingPolicy()
   */
  void SetMaskingMode(Image::MaskingPolicy maskingPolicy);

  /**
   * @brief Gets when the alpha mask is applied.
   *
   * TODO: remove. Kept only so applications written against the old name keep
   * compiling.
   *
   * @see GetMaskingPolicy()
   */
  Image::MaskingPolicy GetMaskingMode() const;

public: // Loading Behavior
  /**
   * @brief Sets the load policy for the image resource.
   *
   * Determines whether the image is loaded immediately when the view is created (IMMEDIATE),
   * or deferred until the view is attached to the scene (ATTACHED).
   *
   * @param[in] loadPolicy The load policy to use
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
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @brief Gets whether synchronous loading is enabled.
   *
   * @return True if synchronous loading is enabled
   */
  bool IsSynchronousLoading() const;

  /**
   * @brief Sets whether the loaded image is uploaded straight to the render thread.
   *
   * Normally a decoded image travels back through the event thread before it is uploaded,
   * so it cannot appear while that thread is busy. With this enabled the loading thread
   * hands the image to the render thread directly, and it is drawn as soon as it is ready.
   *
   * The shortcut costs the steps it skips, so it is off by default:
   *  - the texture is not cached, and is uploaded again for every visual that asks for it;
   *  - its size is not known until the upload finishes, so a layout that depends on the
   *    image's natural size settles late;
   *  - the image cannot be swapped seamlessly for another.
   *
   * @param[in] fastTrack True to upload straight to the render thread
   * @note The request is dropped, without failing, whenever the shortcut cannot be taken:
   *       when an alpha mask, a custom shader, synchronous loading or
   *       SetImageLoadWithViewSize() is in use, when the load policy is not
   *       Image::LoadPolicy::ATTACHED or the release policy is not
   *       Image::ReleasePolicy::DETACHED, or when the url is not a local or remote image.
   */
  void SetFastTrackUpload(bool fastTrack);

  /**
   * @brief Gets whether the loaded image is uploaded straight to the render thread.
   *
   * @return True if the upload skips the event thread
   */
  bool IsFastTrackUploadEnabled() const;

  /**
   * @brief Sets whether EXIF orientation metadata is applied automatically.
   *
   * @param[in] orientationCorrection True to apply orientation correction
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
   * @brief Sets which part of the image is a fixed frame that does not stretch.
   *
   * Each value is a distance measured inward from its own edge of the source image, in
   * source pixels. What lies outside them keeps its size, while the region they enclose is
   * stretched to fill the view. Giving an ordinary image a non-zero border makes it behave
   * as an n-patch image.
   *
   * @param[in] border The frame widths as start, end, top and bottom distances
   * @note The values are rounded to whole source pixels, since they select which columns
   *       and rows of the image stretch.
   * @see SetNPatchBorderOnly()
   */
  void SetNPatchBorder(const Dali::Insets& border);

  /**
   * @brief Gets the N-patch border.
   *
   * @return The width of the fixed frame at each edge, in source image pixels
   */
  Dali::Insets GetNPatchBorder() const;

  /**
   * @brief Sets whether to draw only the border regions.
   *
   * When enabled, the region inside the border is not drawn at all, leaving a hollow frame
   * that whatever sits behind shows through — it is not filled with a colour. The default is
   * false, which draws the whole image with the inside stretched.
   *
   * @param[in] borderOnly True to draw only the border regions
   * @see SetNPatchBorder()
   */
  void SetNPatchBorderOnly(bool borderOnly);

  /**
   * @brief Gets whether only the border regions are drawn.
   *
   * @return True if the region inside the border is left undrawn
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
  explicit ImageView(Integration::ImageViewImpl& implementation);

  /**
   * @brief Allows the creation of this ImageView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit ImageView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} //namespace DALI_NAMESPACE
