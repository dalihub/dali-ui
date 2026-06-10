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
#include <dali-ui-foundation/public-api/image-view-properties.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-impl.h>
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
  /**
   * @brief Property indices — aliased from the shared ImageViewPropertyIndex.
   */
  struct Property
  {
    enum
    {
      IMAGE                     = Ui::ImageViewPropertyIndex::IMAGE,
      FITTING_MODE              = Ui::ImageViewPropertyIndex::FITTING_MODE,
      SAMPLING_MODE             = Ui::ImageViewPropertyIndex::SAMPLING_MODE,
      DESIRED_WIDTH             = Ui::ImageViewPropertyIndex::DESIRED_WIDTH,
      DESIRED_HEIGHT            = Ui::ImageViewPropertyIndex::DESIRED_HEIGHT,
      IMAGE_COLOR               = Ui::ImageViewPropertyIndex::IMAGE_COLOR,
      PRE_MULTIPLIED_ALPHA      = Ui::ImageViewPropertyIndex::PRE_MULTIPLIED_ALPHA,
      PLACEHOLDER_IMAGE         = Ui::ImageViewPropertyIndex::PLACEHOLDER_IMAGE,
      IMAGE_LOAD_WITH_VIEW_SIZE = Ui::ImageViewPropertyIndex::IMAGE_LOAD_WITH_VIEW_SIZE,
      ALPHA_MASK_URL            = Ui::ImageViewPropertyIndex::ALPHA_MASK_URL,
      CROP_TO_MASK              = Ui::ImageViewPropertyIndex::CROP_TO_MASK,
      MASKING_MODE              = Ui::ImageViewPropertyIndex::MASKING_MODE,
      LOAD_POLICY               = Ui::ImageViewPropertyIndex::LOAD_POLICY,
      RELEASE_POLICY            = Ui::ImageViewPropertyIndex::RELEASE_POLICY,
      SYNCHRONOUS_LOADING       = Ui::ImageViewPropertyIndex::SYNCHRONOUS_LOADING,
      FAST_TRACK_UPLOADING      = Ui::ImageViewPropertyIndex::FAST_TRACK_UPLOADING,
      ORIENTATION_CORRECTION    = Ui::ImageViewPropertyIndex::ORIENTATION_CORRECTION,
      N_PATCH_BORDER            = Ui::ImageViewPropertyIndex::N_PATCH_BORDER,
      N_PATCH_BORDER_ONLY       = Ui::ImageViewPropertyIndex::N_PATCH_BORDER_ONLY,
      PIXEL_AREA                = Ui::ImageViewPropertyIndex::PIXEL_AREA,
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
  void SetSamplingMode(Ui::Image::SamplingMode samplingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetSamplingMode
   */
  Ui::Image::SamplingMode GetSamplingMode() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetFittingMode
   */
  void SetFittingMode(Ui::Image::FittingMode fittingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetFittingMode
   */
  Ui::Image::FittingMode GetFittingMode() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetDesiredWidth
   */
  void SetDesiredWidth(int width);

  /**
   * @copydoc Dali::Ui::ImageView::GetDesiredWidth
   */
  int GetDesiredWidth() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetDesiredHeight
   */
  void SetDesiredHeight(int height);

  /**
   * @copydoc Dali::Ui::ImageView::GetDesiredHeight
   */
  int GetDesiredHeight() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetImageLoadWithViewSize
   */
  void SetImageLoadWithViewSize(bool enabled);

  /**
   * @copydoc Dali::Ui::ImageView::IsImageLoadWithViewSizeEnabled
   */
  bool IsImageLoadWithViewSizeEnabled() const;

public: // API — Advanced Rendering & Masking
  /**
   * @copydoc Dali::Ui::ImageView::SetPreMultipliedAlpha
   */
  void SetPreMultipliedAlpha(bool preMultiplied);

  /**
   * @copydoc Dali::Ui::ImageView::IsPreMultipliedAlpha
   */
  bool IsPreMultipliedAlpha() const;

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
   * @copydoc Dali::Ui::ImageView::IsCropToMask
   */
  bool IsCropToMask() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetMaskingMode
   */
  void SetMaskingMode(Ui::Image::MaskingType maskingMode);

  /**
   * @copydoc Dali::Ui::ImageView::GetMaskingMode
   */
  Ui::Image::MaskingType GetMaskingMode() const;

public: // API — Loading Behavior
  /**
   * @copydoc Dali::Ui::ImageView::SetLoadPolicy
   */
  void SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy);

  /**
   * @copydoc Dali::Ui::ImageView::GetLoadPolicy
   */
  Ui::Image::LoadPolicy GetLoadPolicy() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetReleasePolicy
   */
  void SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy);

  /**
   * @copydoc Dali::Ui::ImageView::GetReleasePolicy
   */
  Ui::Image::ReleasePolicy GetReleasePolicy() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetSynchronousLoading
   */
  void SetSynchronousLoading(bool synchronous);

  /**
   * @copydoc Dali::Ui::ImageView::IsSynchronousLoading
   */
  bool IsSynchronousLoading() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetFastTrackUpload
   */
  void SetFastTrackUpload(bool fastTrack);

  /**
   * @copydoc Dali::Ui::ImageView::IsFastTrackUploadEnabled
   */
  bool IsFastTrackUploadEnabled() const;

  /**
   * @copydoc Dali::Ui::ImageView::SetOrientationCorrection
   */
  void SetOrientationCorrection(bool orientationCorrection);

  /**
   * @copydoc Dali::Ui::ImageView::IsOrientationCorrectionEnabled
   */
  bool IsOrientationCorrectionEnabled() const;

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
   * @copydoc Dali::Ui::ImageView::IsNPatchBorderOnly
   */
  bool IsNPatchBorderOnly() const;

public: // Loading Status
  /**
   * @copydoc Dali::Ui::ImageView::GetLoadingStatus
   */
  Ui::Visual::ResourceStatus GetLoadingStatus() const;

protected: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnMeasure
   */
  MeasuredSize OnMeasure(float widthConstraint, float heightConstraint) override;

  /**
   * @copydoc ViewImpl::OnArrange
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
  Dali::String mPlaceholderUrl;
  Dali::String mAlphaMaskUrl;

  Vector4 mPixelArea;
  Vector4 mNPatchBorder;

  UiColor mImageColor;

  Ui::Image::SamplingMode  mSamplingMode;
  Ui::Image::FittingMode   mFittingMode;
  Ui::Image::MaskingType   mMaskingMode;
  Ui::Image::LoadPolicy    mLoadPolicy;
  Ui::Image::ReleasePolicy mReleasePolicy;
  int                      mDesiredWidth;
  int                      mDesiredHeight;

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
