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

#include <dali-ui-foundation/integration-api/image-view-impl.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>

namespace Dali
{
namespace Ui
{

inline Integration::ImageViewImpl& GetImpl(ImageView& imageView)
{
  DALI_ASSERT_ALWAYS(imageView);
  return static_cast<Integration::ImageViewImpl&>(imageView.GetImplementation());
}

inline const Integration::ImageViewImpl& GetImpl(const ImageView& imageView)
{
  DALI_ASSERT_ALWAYS(imageView);
  return static_cast<const Integration::ImageViewImpl&>(imageView.GetImplementation());
}

ImageView::ImageView()                                      = default;
ImageView::~ImageView()                                     = default;
ImageView::ImageView(const ImageView& imageView)            = default;
ImageView::ImageView(ImageView&& rhs) noexcept              = default;
ImageView& ImageView::operator=(const ImageView& imageView) = default;
ImageView& ImageView::operator=(ImageView&& rhs) noexcept   = default;

ImageView ImageView::New()
{
  Integration::ImageViewImplPtr impl = Integration::ImageViewImpl::New();
  ImageView                     view(*impl);
  impl->Initialize();
  return view;
}

ImageView ImageView::New(const Dali::String& url)
{
  Integration::ImageViewImplPtr impl = Integration::ImageViewImpl::New();
  ImageView                     view(*impl);
  impl->Initialize();
  if(!url.Empty())
  {
    view.SetResourceUrl(url);
  }
  return view;
}

ImageView ImageView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<ImageView, Ui::Integration::ImageViewImpl>(handle);
}

void ImageView::Reload()
{
  Ui::GetImpl(*this).Reload();
}

void ImageView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
}

Dali::String ImageView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

void ImageView::SetPixelArea(const Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
}

Vector4 ImageView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

void ImageView::SetPreMultipliedAlpha(bool preMultiplied)
{
  Ui::GetImpl(*this).SetPreMultipliedAlpha(preMultiplied);
}

bool ImageView::IsPreMultipliedAlpha() const
{
  return Ui::GetImpl(*this).IsPreMultipliedAlpha();
}

void ImageView::SetSamplingMode(Ui::Image::SamplingMode samplingMode)
{
  Ui::GetImpl(*this).SetSamplingMode(samplingMode);
}

Ui::Image::SamplingMode ImageView::GetSamplingMode() const
{
  return Ui::GetImpl(*this).GetSamplingMode();
}

void ImageView::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  Ui::GetImpl(*this).SetFittingMode(fittingMode);
}

Ui::Image::FittingMode ImageView::GetFittingMode() const
{
  return Ui::GetImpl(*this).GetFittingMode();
}

void ImageView::SetDesiredWidth(int width)
{
  Ui::GetImpl(*this).SetDesiredWidth(width);
}

int ImageView::GetDesiredWidth() const
{
  return Ui::GetImpl(*this).GetDesiredWidth();
}

void ImageView::SetDesiredHeight(int height)
{
  Ui::GetImpl(*this).SetDesiredHeight(height);
}

int ImageView::GetDesiredHeight() const
{
  return Ui::GetImpl(*this).GetDesiredHeight();
}

void ImageView::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  Ui::GetImpl(*this).SetAlphaMaskUrl(maskUrl);
}

Dali::String ImageView::GetAlphaMaskUrl() const
{
  return Ui::GetImpl(*this).GetAlphaMaskUrl();
}

void ImageView::SetCropToMask(bool cropToMask)
{
  Ui::GetImpl(*this).SetCropToMask(cropToMask);
}

bool ImageView::IsCropToMask() const
{
  return Ui::GetImpl(*this).IsCropToMask();
}

void ImageView::SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy)
{
  Ui::GetImpl(*this).SetLoadPolicy(loadPolicy);
}

Ui::Image::LoadPolicy ImageView::GetLoadPolicy() const
{
  return Ui::GetImpl(*this).GetLoadPolicy();
}

void ImageView::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
}

Ui::Image::ReleasePolicy ImageView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

void ImageView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
}

bool ImageView::IsSynchronousLoading() const
{
  return Ui::GetImpl(*this).IsSynchronousLoading();
}

void ImageView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
}

Dali::String ImageView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

void ImageView::SetImageLoadWithViewSize(bool enabled)
{
  Ui::GetImpl(*this).SetImageLoadWithViewSize(enabled);
}

bool ImageView::IsImageLoadWithViewSizeEnabled() const
{
  return Ui::GetImpl(*this).IsImageLoadWithViewSizeEnabled();
}

void ImageView::SetMaskingMode(Ui::Image::MaskingType maskingMode)
{
  Ui::GetImpl(*this).SetMaskingMode(maskingMode);
}

Ui::Image::MaskingType ImageView::GetMaskingMode() const
{
  return Ui::GetImpl(*this).GetMaskingMode();
}

void ImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
}

UiColor ImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

void ImageView::SetFastTrackUpload(bool fastTrack)
{
  Ui::GetImpl(*this).SetFastTrackUpload(fastTrack);
}

bool ImageView::IsFastTrackUploadEnabled() const
{
  return Ui::GetImpl(*this).IsFastTrackUploadEnabled();
}

void ImageView::SetOrientationCorrection(bool orientationCorrection)
{
  Ui::GetImpl(*this).SetOrientationCorrection(orientationCorrection);
}

bool ImageView::IsOrientationCorrectionEnabled() const
{
  return Ui::GetImpl(*this).IsOrientationCorrectionEnabled();
}

void ImageView::SetNPatchBorder(const Vector4& border)
{
  Ui::GetImpl(*this).SetNPatchBorder(border);
}

Vector4 ImageView::GetNPatchBorder() const
{
  return Ui::GetImpl(*this).GetNPatchBorder();
}

void ImageView::SetNPatchBorderOnly(bool borderOnly)
{
  Ui::GetImpl(*this).SetNPatchBorderOnly(borderOnly);
}

bool ImageView::IsNPatchBorderOnly() const
{
  return Ui::GetImpl(*this).IsNPatchBorderOnly();
}

Ui::Visual::ResourceStatus ImageView::GetLoadingStatus() const
{
  return Ui::GetImpl(*this).GetLoadingStatus();
}

View::ResourceReadySignalType& ImageView::ResourceReadySignal()
{
  return View::ResourceReadySignal();
}

ImageView::ImageView(Integration::ImageViewImpl& implementation)
: View(implementation)
{
}

ImageView::ImageView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Integration::ImageViewImpl>(internal);
}

} // namespace Ui
} // namespace Dali
