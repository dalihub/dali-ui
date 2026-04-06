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

#include <dali-ui-foundation/integration-api/image-view/image-view-impl.h>
#include <dali-ui-foundation/public-api/image-view/image-view.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{

inline Integration::ImageViewImpl& GetImpl(ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<Integration::ImageViewImpl&>(obj.GetImplementation());
}

inline const Integration::ImageViewImpl& GetImpl(const ImageView& obj)
{
  DALI_ASSERT_ALWAYS(obj);
  return static_cast<const Integration::ImageViewImpl&>(obj.GetImplementation());
}

ImageView::ImageView()
{
}

ImageView::ImageView(const ImageView& imageView)            = default;
ImageView::ImageView(ImageView&& rhs) noexcept              = default;
ImageView& ImageView::operator=(const ImageView& imageView) = default;
ImageView& ImageView::operator=(ImageView&& rhs) noexcept   = default;

ImageView::~ImageView()
{
}

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

ImageView& ImageView::SetResourceUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetResourceUrl(url);
  return *this;
}

Dali::String ImageView::GetResourceUrl() const
{
  return Ui::GetImpl(*this).GetResourceUrl();
}

ImageView& ImageView::SetPixelArea(const Vector4& pixelArea)
{
  Ui::GetImpl(*this).SetPixelArea(pixelArea);
  return *this;
}

Vector4 ImageView::GetPixelArea() const
{
  return Ui::GetImpl(*this).GetPixelArea();
}

ImageView& ImageView::SetPreMultipliedAlpha(bool preMultiplied)
{
  Ui::GetImpl(*this).SetPreMultipliedAlpha(preMultiplied);
  return *this;
}

bool ImageView::GetPreMultipliedAlpha() const
{
  return Ui::GetImpl(*this).GetPreMultipliedAlpha();
}

ImageView& ImageView::SetSamplingMode(Ui::SamplingMode::Type samplingMode)
{
  Ui::GetImpl(*this).SetSamplingMode(samplingMode);
  return *this;
}

Ui::SamplingMode::Type ImageView::GetSamplingMode() const
{
  return Ui::GetImpl(*this).GetSamplingMode();
}

ImageView& ImageView::SetFittingMode(Ui::FittingMode::Type fittingMode)
{
  Ui::GetImpl(*this).SetFittingMode(fittingMode);
  return *this;
}

Ui::FittingMode::Type ImageView::GetFittingMode() const
{
  return Ui::GetImpl(*this).GetFittingMode();
}

ImageView& ImageView::SetDesiredSize(Ui::ImageDimensions size)
{
  Ui::GetImpl(*this).SetDesiredSize(size);
  return *this;
}

Ui::ImageDimensions ImageView::GetDesiredSize() const
{
  return Ui::GetImpl(*this).GetDesiredSize();
}

ImageView& ImageView::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  Ui::GetImpl(*this).SetAlphaMaskUrl(maskUrl);
  return *this;
}

Dali::String ImageView::GetAlphaMaskUrl() const
{
  return Ui::GetImpl(*this).GetAlphaMaskUrl();
}

ImageView& ImageView::SetCropToMask(bool cropToMask)
{
  Ui::GetImpl(*this).SetCropToMask(cropToMask);
  return *this;
}

bool ImageView::GetCropToMask() const
{
  return Ui::GetImpl(*this).GetCropToMask();
}

ImageView& ImageView::SetLoadPolicy(Ui::LoadPolicy::Type loadPolicy)
{
  Ui::GetImpl(*this).SetLoadPolicy(loadPolicy);
  return *this;
}

Ui::LoadPolicy::Type ImageView::GetLoadPolicy() const
{
  return Ui::GetImpl(*this).GetLoadPolicy();
}

ImageView& ImageView::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  Ui::GetImpl(*this).SetReleasePolicy(releasePolicy);
  return *this;
}

Ui::ReleasePolicy::Type ImageView::GetReleasePolicy() const
{
  return Ui::GetImpl(*this).GetReleasePolicy();
}

ImageView& ImageView::SetSynchronousLoading(bool synchronous)
{
  Ui::GetImpl(*this).SetSynchronousLoading(synchronous);
  return *this;
}

bool ImageView::GetSynchronousLoading() const
{
  return Ui::GetImpl(*this).GetSynchronousLoading();
}

ImageView& ImageView::SetPlaceholderUrl(const Dali::String& url)
{
  Ui::GetImpl(*this).SetPlaceholderUrl(url);
  return *this;
}

Dali::String ImageView::GetPlaceholderUrl() const
{
  return Ui::GetImpl(*this).GetPlaceholderUrl();
}

ImageView& ImageView::SetImageLoadWithViewSize(bool enabled)
{
  Ui::GetImpl(*this).SetImageLoadWithViewSize(enabled);
  return *this;
}

bool ImageView::GetImageLoadWithViewSize() const
{
  return Ui::GetImpl(*this).GetImageLoadWithViewSize();
}

ImageView& ImageView::SetMaskingMode(Ui::MaskingType::Type maskingMode)
{
  Ui::GetImpl(*this).SetMaskingMode(maskingMode);
  return *this;
}

Ui::MaskingType::Type ImageView::GetMaskingMode() const
{
  return Ui::GetImpl(*this).GetMaskingMode();
}

ImageView& ImageView::SetImageColor(const UiColor& color)
{
  Ui::GetImpl(*this).SetImageColor(color);
  return *this;
}

UiColor ImageView::GetImageColor() const
{
  return Ui::GetImpl(*this).GetImageColor();
}

ImageView& ImageView::SetFastTrackUploading(bool fastTrack)
{
  Ui::GetImpl(*this).SetFastTrackUploading(fastTrack);
  return *this;
}

bool ImageView::GetFastTrackUploading() const
{
  return Ui::GetImpl(*this).GetFastTrackUploading();
}

ImageView& ImageView::SetOrientationCorrection(bool orientationCorrection)
{
  Ui::GetImpl(*this).SetOrientationCorrection(orientationCorrection);
  return *this;
}

bool ImageView::GetOrientationCorrection() const
{
  return Ui::GetImpl(*this).GetOrientationCorrection();
}

ImageView& ImageView::SetNPatchBorder(const Vector4& border)
{
  Ui::GetImpl(*this).SetNPatchBorder(border);
  return *this;
}

Vector4 ImageView::GetNPatchBorder() const
{
  return Ui::GetImpl(*this).GetNPatchBorder();
}

ImageView& ImageView::SetNPatchBorderOnly(bool borderOnly)
{
  Ui::GetImpl(*this).SetNPatchBorderOnly(borderOnly);
  return *this;
}

bool ImageView::GetNPatchBorderOnly() const
{
  return Ui::GetImpl(*this).GetNPatchBorderOnly();
}

ImageView& ImageView::SetDepthIndex(int depthIndex)
{
  Ui::GetImpl(*this).SetDepthIndex(depthIndex);
  return *this;
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
