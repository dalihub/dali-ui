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

// CLASS HEADER
#include <dali-ui-foundation/public-api/visuals/image-visual.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/visuals/image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
ImageVisual ImageVisual::New()
{
  Internal::VisualBaseImplPtr internal = Internal::VisualBaseImpl::New(Dali::Ui::VisualType::IMAGE);
  return ImageVisual(internal.Get());
}

ImageVisual ImageVisual::DownCast(BaseHandle handle)
{
  Internal::VisualBaseImpl* visualBaseImpl = dynamic_cast<Internal::VisualBaseImpl*>(handle.GetObjectPtr());
  if(visualBaseImpl && visualBaseImpl->GetVisualType() == Dali::Ui::VisualType::IMAGE)
  {
    return ImageVisual(visualBaseImpl);
  }
  return ImageVisual();
}

// =============================================================================
// Properties
// =============================================================================

Dali::String ImageVisual::GetResourceUrl() const
{
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::ImageVisual::Property::URL);
}

void ImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::URL, resourceUrl);
}

bool ImageVisual::IsSynchronousLoading() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING);
}

void ImageVisual::SetSynchronousLoading(bool synchronous)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int ImageVisual::GetDesiredWidth() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH);
}

void ImageVisual::SetDesiredWidth(int desiredWidth)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int ImageVisual::GetDesiredHeight() const
{
  return GetImplementation(*this).GetProperty<int>(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT);
}

void ImageVisual::SetDesiredHeight(int desiredHeight)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode ImageVisual::GetSamplingMode() const
{
  return GetImplementation(*this).GetProperty<Image::SamplingMode>(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE);
}

void ImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 ImageVisual::GetPixelArea() const
{
  return GetImplementation(*this).GetProperty<Dali::Vector4>(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA);
}

void ImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type ImageVisual::GetWrapModeU() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U);
}

void ImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type ImageVisual::GetWrapModeV() const
{
  return GetImplementation(*this).GetProperty<Dali::WrapMode::Type>(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V);
}

void ImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool ImageVisual::IsBrokenImageEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

void ImageVisual::SetBrokenImageEnabled(bool brokenImageEnabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ENABLE_BROKEN_IMAGE, brokenImageEnabled);
}

Image::LoadPolicy ImageVisual::GetLoadPolicy() const
{
  return GetImplementation(*this).GetProperty<Image::LoadPolicy>(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY);
}

void ImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy ImageVisual::GetReleasePolicy() const
{
  return GetImplementation(*this).GetProperty<Image::ReleasePolicy>(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY);
}

void ImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, releasePolicy);
}

Image::FittingMode ImageVisual::GetFittingMode() const
{
  return GetImplementation(*this).GetProperty<Image::FittingMode>(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE);
}

void ImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FITTING_MODE, fittingMode);
}

bool ImageVisual::IsOrientationCorrectionEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION);
}

void ImageVisual::SetOrientationCorrectionEnabled(bool orientationCorrection)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool ImageVisual::IsImageLoadWithViewSizeEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE);
}

void ImageVisual::SetImageLoadWithViewSizeEnabled(bool enabled)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::IMAGE_LOAD_WITH_VIEW_SIZE, enabled);
}

bool ImageVisual::IsFastTrackUploadEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::FAST_TRACK_UPLOADING);
}

void ImageVisual::SetFastTrackUploadEnabled(bool fastTrackUploading)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::FAST_TRACK_UPLOADING, fastTrackUploading);
}

Dali::Insets ImageVisual::GetNPatchBorder() const
{
  const Dali::Extents border = GetImplementation(*this).GetProperty<Dali::Extents>(Dali::Ui::Integration::ImageVisual::Property::BORDER);
  return Dali::Insets(static_cast<float>(border.start),
                      static_cast<float>(border.end),
                      static_cast<float>(border.top),
                      static_cast<float>(border.bottom));
}

void ImageVisual::SetNPatchBorder(const Dali::Insets& border)
{
  // The border selects whole columns and rows of the source image, so it is stored as Extents.
  const Dali::Extents rounded(static_cast<int16_t>(std::roundf(border.start)),
                              static_cast<int16_t>(std::roundf(border.end)),
                              static_cast<int16_t>(std::roundf(border.top)),
                              static_cast<int16_t>(std::roundf(border.bottom)));
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::BORDER, rounded);
}

bool ImageVisual::IsNPatchBorderOnly() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::BORDER_ONLY);
}

void ImageVisual::SetNPatchBorderOnly(bool borderOnly)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::BORDER_ONLY, borderOnly);
}

Dali::String ImageVisual::GetNPatchAuxiliaryImage() const
{
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE);
}

void ImageVisual::SetNPatchAuxiliaryImage(const Dali::String& auxiliaryImage)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE, auxiliaryImage);
}

float ImageVisual::GetNPatchAuxiliaryImageAlpha() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA);
}

void ImageVisual::SetNPatchAuxiliaryImageAlpha(float auxiliaryImageAlpha)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::AUXILIARY_IMAGE_ALPHA, auxiliaryImageAlpha);
}

bool ImageVisual::IsPreMultiplyAlphaOnLoadEnabled() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

void ImageVisual::SetPreMultiplyAlphaOnLoadEnabled(bool preMultiplied)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
}

Dali::String ImageVisual::GetAlphaMaskUrl() const
{
  return GetImplementation(*this).GetProperty<Dali::String>(Dali::Ui::Integration::ImageVisual::Property::ALPHA_MASK_URL);
}

void ImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
}

float ImageVisual::GetContentScaleForMasking() const
{
  return GetImplementation(*this).GetProperty<float>(Dali::Ui::Integration::ImageVisual::Property::CONTENT_SCALE_FOR_MASKING);
}

void ImageVisual::SetContentScaleForMasking(float contentScale)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::CONTENT_SCALE_FOR_MASKING, contentScale);
}

bool ImageVisual::IsCropToMask() const
{
  return GetImplementation(*this).GetProperty<bool>(Dali::Ui::Integration::ImageVisual::Property::CROP_TO_MASK);
}

void ImageVisual::SetCropToMask(bool cropToMask)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::CROP_TO_MASK, cropToMask);
}

Image::MaskingPolicy ImageVisual::GetMaskingPolicy() const
{
  return GetImplementation(*this).GetProperty<Image::MaskingPolicy>(Dali::Ui::Integration::ImageVisual::Property::MASKING_POLICY);
}

void ImageVisual::SetMaskingPolicy(Image::MaskingPolicy maskingPolicy)
{
  GetImplementation(*this).SetProperty(Dali::Ui::Integration::ImageVisual::Property::MASKING_POLICY, maskingPolicy);
}

// =============================================================================
// Internal API
// =============================================================================

ImageVisual::ImageVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
void ImageVisual::Reload()
{
  // Forcibly update properties before call DoAction
  GetImplementation(*this).UpdateProperty();
  GetImplementation(*this).DoAction(Ui::Integration::ImageVisual::Action::RELOAD, Dali::Property::Value());
}

} // namespace Ui
} //namespace DALI_NAMESPACE
