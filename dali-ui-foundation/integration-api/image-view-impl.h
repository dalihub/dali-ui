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

#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/image-view-types.h>
#include <dali-ui-foundation/public-api/ui-color.h>

#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ImageViewImpl;
using ImageViewImplPtr = IntrusivePtr<ImageViewImpl>;

/**
 * @brief This is the internal implementation class for ImageView.
 *
 * @see Dali::Ui::ImageView
 */
class DALI_UI_API ImageViewImpl : public ViewImpl
{
public: // Properties
  /// @brief The start and end property ranges for this impl.
  enum
  {
    PROPERTY_START_INDEX = Dali::PROPERTY_REGISTRATION_START_INDEX,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the ImageViewImpl class.
   */
  struct Property
  {
    enum
    {
      ///////////////////////////////////////////////////////////////////////////////
      // Event side (non-animatable) properties
      ///////////////////////////////////////////////////////////////////////////////

      /**
       * @brief The URL of the image resource.
       * @details Name "image", type Property::STRING.
       */
      IMAGE = PROPERTY_START_INDEX,

      /**
       * @brief The fitting mode used when scaling the image.
       * @details Name "fittingMode", type Property::INTEGER.
       */
      FITTING_MODE,

      /**
       * @brief The sampling mode used when scaling the image.
       * @details Name "samplingMode", type Property::INTEGER.
       */
      SAMPLING_MODE,

      /**
       * @brief The desired image width hint for the image loader.
       * @details Name "desiredWidth", type Property::FLOAT.
       */
      DESIRED_WIDTH,

      /**
       * @brief The desired image height hint for the image loader.
       * @details Name "desiredHeight", type Property::FLOAT.
       */
      DESIRED_HEIGHT,

      /**
       * @brief The color multiplier applied to the image.
       * @details Name "imageColor", type Property::VECTOR4.
       */
      IMAGE_COLOR,

      /**
       * @brief Whether the image uses pre-multiplied alpha.
       * @details Name "preMultipliedAlpha", type Property::BOOLEAN.
       */
      PRE_MULTIPLIED_ALPHA,

      /**
       * @brief The URL of the placeholder image shown while loading.
       * @details Name "placeholderImage", type Property::STRING.
       */
      PLACEHOLDER_IMAGE,

      /**
       * @brief Whether the view size is resolved synchronously during loading.
       * @details Name "imageLoadWithViewSize", type Property::BOOLEAN.
       */
      IMAGE_LOAD_WITH_VIEW_SIZE,

      /**
       * @brief The URL of the alpha mask image.
       * @details Name "alphaMaskUrl", type Property::STRING.
       */
      ALPHA_MASK_URL,

      /**
       * @brief Whether the image is cropped to the mask bounds.
       * @details Name "cropToMask", type Property::BOOLEAN.
       */
      CROP_TO_MASK,

      /**
       * @brief The masking mode.
       * @details Name "maskingMode", type Property::INTEGER.
       */
      MASKING_MODE,

      /**
       * @brief The load policy for the image resource.
       * @details Name "loadPolicy", type Property::INTEGER.
       */
      LOAD_POLICY,

      /**
       * @brief The release policy for the image resource.
       * @details Name "releasePolicy", type Property::INTEGER.
       */
      RELEASE_POLICY,

      /**
       * @brief Whether the image is loaded synchronously on the main thread.
       * @details Name "synchronousLoading", type Property::BOOLEAN.
       */
      SYNCHRONOUS_LOADING,

      /**
       * @brief Whether fast-track GPU uploading is enabled.
       * @details Name "fastTrackUploading", type Property::BOOLEAN.
       */
      FAST_TRACK_UPLOADING,

      /**
       * @brief Whether EXIF orientation metadata is applied automatically.
       * @details Name "orientationCorrection", type Property::BOOLEAN.
       */
      ORIENTATION_CORRECTION,

      /**
       * @brief The N-patch border insets as (left, top, right, bottom).
       * @details Name "nPatchBorder", type Property::VECTOR4.
       */
      N_PATCH_BORDER,

      /**
       * @brief Whether only the N-patch border regions are rendered.
       * @details Name "nPatchBorderOnly", type Property::BOOLEAN.
       */
      N_PATCH_BORDER_ONLY,

      ///////////////////////////////////////////////////////////////////////////////
      // Animatable Properties
      ///////////////////////////////////////////////////////////////////////////////

      /**
       * @brief The normalized sub-region of the image to display.
       * @details Name "pixelArea", type Property::VECTOR4.
       */
      PIXEL_AREA = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    };
  };

protected: // Construction & Destruction
  /**
   * @brief ImageViewImpl constructor.
   */
  ImageViewImpl();

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ImageViewImpl();

public: // Creation
  /**
   * @brief Creates a new ImageViewImpl.
   *
   * @return An intrusive pointer to the newly allocated ImageViewImpl
   */
  static ImageViewImplPtr New();

public: // Properties (required by DALI_PROPERTY_REGISTRATION)
  /// @cond internal
  static void                  SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value);
  static Dali::Property::Value GetProperty(Dali::BaseObject* object, Dali::Property::Index index);
  /// @endcond

public: // API — Image
  /**
   * @copydoc Dali::Ui::ImageView::SetResourceUrl
   */
  void SetResourceUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::ImageView::GetResourceUrl
   */
  Dali::String GetResourceUrl() const;

  /**
   * @copydoc Dali::Ui::ImageView::Reload
   */
  void Reload();

  /**
   * @copydoc Dali::Ui::ImageView::SetPlaceholderUrl
   */
  void SetPlaceholderUrl(const Dali::String& url);

  /**
   * @copydoc Dali::Ui::ImageView::GetPlaceholderUrl
   */
  Dali::String GetPlaceholderUrl() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetImageColor
   */
  void SetImageColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::ImageView::GetImageColor
   */
  UiColor GetImageColor() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetPixelArea
   */
  void SetPixelArea(const Vector4& pixelArea);

  /**
   * @copydoc Dali::Ui::ImageView::GetPixelArea
   */
  Vector4 GetPixelArea() const;

public: // API — Size & Fitting Control
  /**
   * @copydoc Dali::Ui::ImageView::SetSamplingMode
   */
  void SetSamplingMode(Ui::SamplingMode::Type samplingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetSamplingMode
   */
  Ui::SamplingMode::Type GetSamplingMode() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetFittingMode
   */
  void SetFittingMode(Ui::FittingMode::Type fittingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetFittingMode
   */
  Ui::FittingMode::Type GetFittingMode() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetDesiredSize
   */
  void SetDesiredSize(Ui::ImageDimensions size);

  /**
   * @copydoc Dali::Ui::ImageView::GetDesiredSize
   */
  Ui::ImageDimensions GetDesiredSize() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetImageLoadWithViewSize
   */
  void SetImageLoadWithViewSize(bool enabled);

  /**
   * @copydoc Dali::Ui::ImageView::GetImageLoadWithViewSize
   */
  bool GetImageLoadWithViewSize() const;

public: // API — Advanced Rendering & Masking
  /**
   * @copydoc Dali::Ui::ImageView::SetPreMultipliedAlpha
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @copydoc Dali::Ui::ImageView::GetPreMultipliedAlpha
   */
  bool GetPreMultipliedAlpha() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetAlphaMaskUrl
   */
  void SetAlphaMaskUrl(const Dali::String& maskUrl);

  /**
   * @copydoc Dali::Ui::ImageView::GetAlphaMaskUrl
   */
  Dali::String GetAlphaMaskUrl() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetCropToMask
   */
  void SetCropToMask(bool cropToMask);

  /**
   * @copydoc Dali::Ui::ImageView::GetCropToMask
   */
  bool GetCropToMask() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetMaskingMode
   */
  void SetMaskingMode(Ui::MaskingType::Type maskingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetMaskingMode
   */
  Ui::MaskingType::Type GetMaskingMode() const;

public: // API — Loading Behavior
  /**
   * @copydoc Dali::Ui::ImageView::SetLoadPolicy
   */
  void SetLoadPolicy(Ui::LoadPolicy::Type loadPolicy);

  /**
   * @copydoc Dali::Ui::ImageView::GetLoadPolicy
   */
  Ui::LoadPolicy::Type GetLoadPolicy() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetReleasePolicy
   */
  void SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy);

  /**
   * @copydoc Dali::Ui::ImageView::GetReleasePolicy
   */
  Ui::ReleasePolicy::Type GetReleasePolicy() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetSynchronousLoading
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @copydoc Dali::Ui::ImageView::GetSynchronousLoading
   */
  bool GetSynchronousLoading() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetFastTrackUploading
   */
  void SetFastTrackUploading(bool fastTrack);

  /**
   * @copydoc Dali::Ui::ImageView::GetFastTrackUploading
   */
  bool GetFastTrackUploading() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetOrientationCorrection
   */
  void SetOrientationCorrection(bool orientationCorrection);

  /**
   * @copydoc Dali::Ui::ImageView::GetOrientationCorrection
   */
  bool GetOrientationCorrection() const;

public: // API — N-Patch Border
  /**
   * @copydoc Dali::Ui::ImageView::SetNPatchBorder
   */
  void SetNPatchBorder(const Vector4& border);

  /**
   * @copydoc Dali::Ui::ImageView::GetNPatchBorder
   */
  Vector4 GetNPatchBorder() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetNPatchBorderOnly
   */
  void SetNPatchBorderOnly(bool borderOnly);

  /**
   * @copydoc Dali::Ui::ImageView::GetNPatchBorderOnly
   */
  bool GetNPatchBorderOnly() const;

public: // Depth Index
  /**
   * @copydoc Dali::Ui::ImageView::SetDepthIndex
   */
  void SetDepthIndex(int depthIndex);

public: // Loading Status
  /**
   * @copydoc Dali::Ui::ImageView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

private: // From ViewImpl
  /**
   * @copydoc Integration::ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Integration::ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc Integration::ViewImpl::OnArrange
   */
  MeasuredSize OnArrange(const LayoutRect& bounds) override;

private: // Internal methods
  /**
   * @brief Rebuilds and re-registers the image visual from current property values.
   */
  void UpdateVisual();

  /**
   * @brief Rebuilds and re-registers the placeholder visual.
   */
  void UpdatePlaceholderVisual();

  /**
   * @brief Applies the current fitting mode transform to the visual for the given size.
   *
   * @param[in] size The allocated size of the view
   */
  void ApplyFittingMode(const Vector2& size);

  /**
   * @brief Callback invoked when the view's resource becomes ready.
   *
   * @param[in] view The view whose resource became ready
   */
  void OnViewResourceReady(Ui::View view);

private:
  // Not copyable or movable
  ImageViewImpl(const ImageViewImpl&)            = delete;
  ImageViewImpl(ImageViewImpl&&)                 = delete;
  ImageViewImpl& operator=(const ImageViewImpl&) = delete;
  ImageViewImpl& operator=(ImageViewImpl&&)      = delete;

private: // Data
  Ui::Visual::Base mVisual;

  Dali::String mUrl;
  Dali::String mPlaceholderImageUrl;
  Dali::String mAlphaMaskUrl;

  Vector4 mPixelArea;
  Vector4 mNPatchBorder;

  UiColor mImageColor;

  Ui::SamplingMode::Type  mSamplingMode;
  Ui::FittingMode::Type   mFittingMode;
  Ui::MaskingType::Type   mMaskingMode;
  Ui::LoadPolicy::Type    mLoadPolicy;
  Ui::ReleasePolicy::Type mReleasePolicy;
  Ui::ImageDimensions     mDesiredSize;

  int mDepthIndex;

  bool mPreMultipliedAlpha;
  bool mImageLoadWithViewSize;
  bool mCropToMask;
  bool mSynchronousLoading;
  bool mFastTrackUploading;
  bool mOrientationCorrection;
  bool mNPatchBorderOnly;
  bool mVisualDirty;
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
