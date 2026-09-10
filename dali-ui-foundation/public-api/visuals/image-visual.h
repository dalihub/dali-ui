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
#include <dali/public-api/common/insets.h>      ///< Dali::Insets
#include <dali/public-api/rendering/sampling.h> ///< Dali::WrapMode::Type

// INTERNAL INCLUDES
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
 * @brief ImageVisual renders an image.
 *
 * Its visual type is VisualType::IMAGE. The url decides how the image is drawn: an n-patch
 * image, meaning a ".9" file or any image given a border with SetNPatchBorder(), is stretched
 * by its border regions rather than uniformly; an SVG file is rasterized at the size it is
 * drawn; and any other image is drawn as it is. An animated image such as a GIF shows only
 * its first frame here, so use AnimatedImageVisual to play one.
 *
 * It can use the CornerRadius, CornerSquareness and Borderline features of VisualBase, unless
 * the image is an n-patch image.
 */
class DALI_UI_API ImageVisual : public VisualBase
{
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

public: // Properties
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
  void SetResourceUrl(const Dali::String& resourceUrl);

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
   * @brief Gets the desired width of the ImageVisual.
   *
   * @return The desired width of the ImageVisual
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
   * @note Ignored while SetImageLoadWithViewSize() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredHeight()
   * @see SetSamplingMode()
   */
  void SetDesiredWidth(int desiredWidth);

  /**
   * @brief Gets the desired height of the ImageVisual.
   *
   * @return The desired height of the ImageVisual
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
   * @note Ignored while SetImageLoadWithViewSize() is enabled, which loads at the View's
   *       size instead.
   * @see SetDesiredWidth()
   */
  void SetDesiredHeight(int desiredHeight);

  /**
   * @brief Gets the sampling mode of the ImageVisual.
   *
   * @return The sampling mode of the ImageVisual
   */
  Image::SamplingMode GetSamplingMode() const;

  /**
   * @brief Sets the filter used when the image is resampled to the desired size.
   *
   * It only has an effect where a resample happens, which is when a desired size or
   * SetImageLoadWithViewSize() asks for a size other than the source's own. The default is
   * Image::SamplingMode::BOX_THEN_LINEAR.
   *
   * @param[in] samplingMode The sampling mode to set
   * @see SetDesiredWidth()
   */
  void SetSamplingMode(Image::SamplingMode samplingMode);

  /**
   * @brief Gets the pixel area of the ImageVisual.
   *
   * @return The pixel area of the ImageVisual
   */
  Dali::Vector4 GetPixelArea() const;

  /**
   * @brief Sets the pixel area of the ImageVisual.
   *
   * The area is given in texture coordinates as (x, y, width, height): the first two
   * elements are its top-left corner and the last two its size. The default,
   * [0.0, 0.0, 1.0, 1.0], is the whole image.
   *
   * @param[in] pixelArea The pixel area to set
   * @note Has no effect on an n-patch image.
   * @see SetWrapModeU()
   */
  void SetPixelArea(const Dali::Vector4& pixelArea);

  /**
   * @brief Gets the wrap mode for u coordinate of the ImageVisual.
   *
   * @return The wrap mode for u coordinate of the ImageVisual
   */
  Dali::WrapMode::Type GetWrapModeU() const;

  /**
   * @brief Sets the wrap mode for u coordinate of the ImageVisual.
   *
   * u and v are the coordinates the image is sampled with: u runs from 0.0 at the left
   * edge of the image to 1.0 at the right edge, and v from 0.0 at the top edge to 1.0 at
   * the bottom. The wrap mode decides how the image is sampled where the coordinate falls
   * outside that range, which is what a pixel area reaching beyond the image asks for.
   * Dali::WrapMode::REPEAT tiles the image and Dali::WrapMode::MIRRORED_REPEAT tiles it
   * alternately flipped, while the default clamps to the edge pixel.
   *
   * @param[in] wrapModeU The wrap mode for u coordinate to set
   * @note Has no effect on an n-patch image.
   * @see SetPixelArea()
   */
  void SetWrapModeU(Dali::WrapMode::Type wrapModeU);

  /**
   * @brief Gets the wrap mode for v coordinate of the ImageVisual.
   *
   * @return The wrap mode for v coordinate of the ImageVisual
   */
  Dali::WrapMode::Type GetWrapModeV() const;

  /**
   * @brief Sets the wrap mode for v coordinate of the ImageVisual.
   *
   * v is the vertical coordinate, running from 0.0 at the top edge of the image to 1.0 at
   * the bottom. See SetWrapModeU() for how a wrap mode is applied.
   *
   * @param[in] wrapModeV The wrap mode for v coordinate to set
   * @note Has no effect on an n-patch image.
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
   * @brief Gets the load policy of the ImageVisual.
   *
   * @return The load policy of the ImageVisual
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
   * @brief Gets the release policy of the ImageVisual.
   *
   * @return The release policy of the ImageVisual
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
   * @brief Gets the fitting mode of the ImageVisual.
   *
   * @return The fitting mode of the ImageVisual
   */
  Image::FittingMode GetFittingMode() const;

  /**
   * @brief Sets the fitting mode of the ImageVisual.
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
  void SetOrientationCorrection(bool orientationCorrection);

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
  void SetImageLoadWithViewSize(bool enabled);

  /**
   * @brief Gets whether the loaded image is uploaded straight to the render thread.
   *
   * @return True if the upload skips the event thread
   */
  bool IsFastTrackUploadEnabled() const;

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
   * @param[in] fastTrackUploading True to upload straight to the render thread
   * @note The request is dropped, without failing, whenever the shortcut cannot be taken:
   *       when an alpha mask, a custom shader, synchronous loading or
   *       SetImageLoadWithViewSize() is in use, when the load policy is not
   *       Image::LoadPolicy::ATTACHED or the release policy is not
   *       Image::ReleasePolicy::DETACHED, or when the url is not a local or remote image.
   */
  void SetFastTrackUpload(bool fastTrackUploading);

  /**
   * @brief Gets the NPatch border of the ImageVisual.
   *
   * @return The width of the fixed frame at each edge, in source image pixels
   */
  Dali::Insets GetNPatchBorder() const;

  /**
   * @brief Sets which part of the image is a fixed frame that does not stretch.
   *
   * Each value is a distance measured inward from its own edge of the source image, in
   * source pixels. What lies outside them keeps its size, while the region they enclose is
   * stretched to fill the visual. Giving an ordinary image a border makes it behave as an
   * n-patch image, which is otherwise decided by a ".9" file name.
   *
   * @param[in] border The frame widths as start, end, top and bottom distances
   * @note The values are rounded to whole source pixels, since they select which columns
   *       and rows of the image stretch.
   * @see SetNPatchBorderOnly()
   */
  void SetNPatchBorder(const Dali::Insets& border);

  /**
   * @brief Gets whether only the border regions are drawn.
   *
   * @return True if the region inside the border is left undrawn
   */
  bool IsNPatchBorderOnly() const;

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
   * @brief Gets the auxiliary image url of the ImageVisual.
   *
   * @return The auxiliary image url of the ImageVisual
   */
  Dali::String GetNPatchAuxiliaryImage() const;

  /**
   * @brief Sets a second image to draw over the n-patch image.
   *
   * The two are stretched differently: the n-patch keeps its frame and stretches only the
   * region inside the border, while the auxiliary image is stretched evenly across the whole
   * visual. That is what it is for — an ornament or highlight laid over a frame that must
   * keep its shape.
   *
   * The auxiliary image is drawn on top, blended by its own alpha scaled by
   * SetNPatchAuxiliaryImageAlpha(). Since that alpha starts at 0, setting only this url
   * shows nothing until the alpha is raised.
   *
   * @param[in] auxiliaryImage The auxiliary image url to set
   * @note Only available if resource is n-patch, or NPatchBorder applied.
   * @note The visual's natural size grows to hold both images, so an auxiliary image larger
   *       than the n-patch enlarges it.
   * @see SetNPatchAuxiliaryImageAlpha()
   */
  void SetNPatchAuxiliaryImage(const Dali::String& auxiliaryImage);

  /**
   * @brief Gets how opaque the auxiliary image is drawn.
   *
   * @return The opacity applied to the auxiliary image, from 0 to 1
   */
  float GetNPatchAuxiliaryImageAlpha() const;

  /**
   * @brief Sets how opaque the auxiliary image is drawn.
   *
   * The value multiplies the auxiliary image's own alpha, so 1 draws it as it is and 0 hides
   * it. The default is 0, which means an auxiliary image stays invisible until this is set.
   *
   * @param[in] auxiliaryImageAlpha The opacity to apply, from 0 to 1
   * @note Only available if resource is n-patch, or NPatchBorder applied.
   * @see SetNPatchAuxiliaryImage()
   */
  void SetNPatchAuxiliaryImageAlpha(float auxiliaryImageAlpha);

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
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @brief Gets the alpha mask url of the ImageVisual.
   *
   * @return The alpha mask url of the ImageVisual
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
   * @brief Gets the masking type of the ImageVisual.
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
   * @brief Forces the image to be reloaded.
   *
   * @note Every visual using the same image will get the latest one.
   */
  void Reload();

public:
  ImageVisual()                                      = default;
  ~ImageVisual()                                     = default;
  ImageVisual(const ImageVisual& rhs)                = default;
  ImageVisual& operator=(const ImageVisual& rhs)     = default;
  ImageVisual(ImageVisual&& rhs) noexcept            = default;
  ImageVisual& operator=(ImageVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ImageVisual(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_IMAGE_VISUAL_OBJECT_H
