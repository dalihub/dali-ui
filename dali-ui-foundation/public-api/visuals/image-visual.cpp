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

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

namespace Dali
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
  return VisualBase::GetProperty<Dali::String>(ImageVisual::Property::URL);
}

void ImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(ImageVisual::Property::URL, resourceUrl);
}

bool ImageVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::SYNCHRONOUS_LOADING);
}

void ImageVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(ImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
}

int ImageVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(ImageVisual::Property::DESIRED_WIDTH);
}

void ImageVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(ImageVisual::Property::DESIRED_WIDTH, desiredWidth);
}

int ImageVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(ImageVisual::Property::DESIRED_HEIGHT);
}

void ImageVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
}

Image::SamplingMode ImageVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(ImageVisual::Property::SAMPLING_MODE);
}

void ImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(ImageVisual::Property::SAMPLING_MODE, samplingMode);
}

Dali::Vector4 ImageVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(ImageVisual::Property::PIXEL_AREA);
}

void ImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(ImageVisual::Property::PIXEL_AREA, pixelArea);
}

Dali::WrapMode::Type ImageVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(ImageVisual::Property::WRAP_MODE_U);
}

void ImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(ImageVisual::Property::WRAP_MODE_U, wrapModeU);
}

Dali::WrapMode::Type ImageVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(ImageVisual::Property::WRAP_MODE_V);
}

void ImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(ImageVisual::Property::WRAP_MODE_V, wrapModeV);
}

bool ImageVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

void ImageVisual::SetEnableBrokenImage(bool enableBrokenImage)
{
  VisualBase::SetProperty(ImageVisual::Property::ENABLE_BROKEN_IMAGE, enableBrokenImage);
}

Image::LoadPolicy ImageVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(ImageVisual::Property::LOAD_POLICY);
}

void ImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(ImageVisual::Property::LOAD_POLICY, loadPolicy);
}

Image::ReleasePolicy ImageVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(ImageVisual::Property::RELEASE_POLICY);
}

void ImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(ImageVisual::Property::RELEASE_POLICY, releasePolicy);
}

Image::FittingMode ImageVisual::GetFittingMode() const
{
  return VisualBase::GetProperty<Image::FittingMode>(ImageVisual::Property::FITTING_MODE);
}

void ImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  VisualBase::SetProperty(ImageVisual::Property::FITTING_MODE, fittingMode);
}

bool ImageVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::ORIENTATION_CORRECTION);
}

void ImageVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(ImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
}

bool ImageVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::SYNCHRONOUS_SIZING);
}

void ImageVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(ImageVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
}

bool ImageVisual::IsFastTrackUploading() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::FAST_TRACK_UPLOADING);
}

void ImageVisual::SetFastTrackUploading(bool fastTrackUploading)
{
  VisualBase::SetProperty(ImageVisual::Property::FAST_TRACK_UPLOADING, fastTrackUploading);
}

Dali::Extents ImageVisual::GetNPatchBorder() const
{
  return VisualBase::GetProperty<Dali::Extents>(ImageVisual::Property::BORDER);
}

void ImageVisual::SetNPatchBorder(const Dali::Extents& border)
{
  VisualBase::SetProperty(ImageVisual::Property::BORDER, border);
}

bool ImageVisual::IsNPatchBorderOnly() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::BORDER_ONLY);
}

void ImageVisual::SetNPatchBorderOnly(bool borderOnly)
{
  VisualBase::SetProperty(ImageVisual::Property::BORDER_ONLY, borderOnly);
}

Dali::String ImageVisual::GetNPatchAuxiliaryImage() const
{
  return VisualBase::GetProperty<Dali::String>(ImageVisual::Property::AUXILIARY_IMAGE);
}

void ImageVisual::SetNPatchAuxiliaryImage(const Dali::String& auxiliaryImage)
{
  VisualBase::SetProperty(ImageVisual::Property::AUXILIARY_IMAGE, auxiliaryImage);
}

float ImageVisual::GetNPatchAuxiliaryImageAlpha() const
{
  return VisualBase::GetProperty<float>(ImageVisual::Property::AUXILIARY_IMAGE_ALPHA);
}

void ImageVisual::SetNPatchAuxiliaryImageAlpha(float auxiliaryImageAlpha)
{
  VisualBase::SetProperty(ImageVisual::Property::AUXILIARY_IMAGE_ALPHA, auxiliaryImageAlpha);
}

bool ImageVisual::IsPreMultipliedAlpha() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

void ImageVisual::SetPreMultipliedAlpha(bool preMultiplied)
{
  VisualBase::SetProperty(ImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
}

Dali::String ImageVisual::GetAlphaMaskUrl() const
{
  return VisualBase::GetProperty<Dali::String>(ImageVisual::Property::ALPHA_MASK_URL);
}

void ImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  VisualBase::SetProperty(ImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
}

float ImageVisual::GetMaskContentScale() const
{
  return VisualBase::GetProperty<float>(ImageVisual::Property::MASK_CONTENT_SCALE);
}

void ImageVisual::SetMaskContentScale(float maskContentScale)
{
  VisualBase::SetProperty(ImageVisual::Property::MASK_CONTENT_SCALE, maskContentScale);
}

bool ImageVisual::IsCropToMask() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::CROP_TO_MASK);
}

void ImageVisual::SetCropToMask(bool cropToMask)
{
  VisualBase::SetProperty(ImageVisual::Property::CROP_TO_MASK, cropToMask);
}

Image::MaskingType ImageVisual::GetMaskingType() const
{
  return VisualBase::GetProperty<Image::MaskingType>(ImageVisual::Property::MASKING_TYPE);
}

void ImageVisual::SetMaskingType(Image::MaskingType maskingType)
{
  VisualBase::SetProperty(ImageVisual::Property::MASKING_TYPE, maskingType);
}

// =============================================================================
// Internal API
// =============================================================================

ImageVisual::ImageVisual(Dali::Ui::Internal::VisualBaseImpl* object)
: VisualBase(object)
{
}
} // namespace Ui
} // namespace Dali
