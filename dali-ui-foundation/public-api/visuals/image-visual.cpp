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

ImageVisual& ImageVisual::SetResourceUrl(const Dali::String& resourceUrl)
{
  VisualBase::SetProperty(ImageVisual::Property::URL, resourceUrl);
  return *this;
}

bool ImageVisual::IsSynchronousLoading() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::SYNCHRONOUS_LOADING);
}

ImageVisual& ImageVisual::SetSynchronousLoading(bool synchronous)
{
  VisualBase::SetProperty(ImageVisual::Property::SYNCHRONOUS_LOADING, synchronous);
  return *this;
}

int ImageVisual::GetDesiredWidth() const
{
  return VisualBase::GetProperty<int>(ImageVisual::Property::DESIRED_WIDTH);
}

ImageVisual& ImageVisual::SetDesiredWidth(int desiredWidth)
{
  VisualBase::SetProperty(ImageVisual::Property::DESIRED_WIDTH, desiredWidth);
  return *this;
}

int ImageVisual::GetDesiredHeight() const
{
  return VisualBase::GetProperty<int>(ImageVisual::Property::DESIRED_HEIGHT);
}

ImageVisual& ImageVisual::SetDesiredHeight(int desiredHeight)
{
  VisualBase::SetProperty(ImageVisual::Property::DESIRED_HEIGHT, desiredHeight);
  return *this;
}

Image::SamplingMode ImageVisual::GetSamplingMode() const
{
  return VisualBase::GetProperty<Image::SamplingMode>(ImageVisual::Property::SAMPLING_MODE);
}

ImageVisual& ImageVisual::SetSamplingMode(Image::SamplingMode samplingMode)
{
  VisualBase::SetProperty(ImageVisual::Property::SAMPLING_MODE, samplingMode);
  return *this;
}

Dali::Vector4 ImageVisual::GetPixelArea() const
{
  return VisualBase::GetProperty<Dali::Vector4>(ImageVisual::Property::PIXEL_AREA);
}

ImageVisual& ImageVisual::SetPixelArea(const Dali::Vector4& pixelArea)
{
  VisualBase::SetProperty(ImageVisual::Property::PIXEL_AREA, pixelArea);
  return *this;
}

Dali::WrapMode::Type ImageVisual::GetWrapModeU() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(ImageVisual::Property::WRAP_MODE_U);
}

ImageVisual& ImageVisual::SetWrapModeU(Dali::WrapMode::Type wrapModeU)
{
  VisualBase::SetProperty(ImageVisual::Property::WRAP_MODE_U, wrapModeU);
  return *this;
}

Dali::WrapMode::Type ImageVisual::GetWrapModeV() const
{
  return VisualBase::GetProperty<Dali::WrapMode::Type>(ImageVisual::Property::WRAP_MODE_V);
}

ImageVisual& ImageVisual::SetWrapModeV(Dali::WrapMode::Type wrapModeV)
{
  VisualBase::SetProperty(ImageVisual::Property::WRAP_MODE_V, wrapModeV);
  return *this;
}

bool ImageVisual::IsBrokenImageEnabled() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::ENABLE_BROKEN_IMAGE);
}

ImageVisual& ImageVisual::SetEnableBrokenImage(bool enableBrokenImage)
{
  VisualBase::SetProperty(ImageVisual::Property::ENABLE_BROKEN_IMAGE, enableBrokenImage);
  return *this;
}

Image::LoadPolicy ImageVisual::GetLoadPolicy() const
{
  return VisualBase::GetProperty<Image::LoadPolicy>(ImageVisual::Property::LOAD_POLICY);
}

ImageVisual& ImageVisual::SetLoadPolicy(Image::LoadPolicy loadPolicy)
{
  VisualBase::SetProperty(ImageVisual::Property::LOAD_POLICY, loadPolicy);
  return *this;
}

Image::ReleasePolicy ImageVisual::GetReleasePolicy() const
{
  return VisualBase::GetProperty<Image::ReleasePolicy>(ImageVisual::Property::RELEASE_POLICY);
}

ImageVisual& ImageVisual::SetReleasePolicy(Image::ReleasePolicy releasePolicy)
{
  VisualBase::SetProperty(ImageVisual::Property::RELEASE_POLICY, releasePolicy);
  return *this;
}

Image::FittingMode ImageVisual::GetFittingMode() const
{
  return VisualBase::GetProperty<Image::FittingMode>(ImageVisual::Property::FITTING_MODE);
}

ImageVisual& ImageVisual::SetFittingMode(Image::FittingMode fittingMode)
{
  VisualBase::SetProperty(ImageVisual::Property::FITTING_MODE, fittingMode);
  return *this;
}

bool ImageVisual::IsOrientationCorrection() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::ORIENTATION_CORRECTION);
}

ImageVisual& ImageVisual::SetOrientationCorrection(bool orientationCorrection)
{
  VisualBase::SetProperty(ImageVisual::Property::ORIENTATION_CORRECTION, orientationCorrection);
  return *this;
}

bool ImageVisual::IsSynchronousSizing() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::SYNCHRONOUS_SIZING);
}

ImageVisual& ImageVisual::SetSynchronousSizing(bool synchronousSizing)
{
  VisualBase::SetProperty(ImageVisual::Property::SYNCHRONOUS_SIZING, synchronousSizing);
  return *this;
}

bool ImageVisual::IsFastTrackUploading() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::FAST_TRACK_UPLOADING);
}

ImageVisual& ImageVisual::SetFastTrackUploading(bool fastTrackUploading)
{
  VisualBase::SetProperty(ImageVisual::Property::FAST_TRACK_UPLOADING, fastTrackUploading);
  return *this;
}

Dali::Extents ImageVisual::GetNPatchBorder() const
{
  return VisualBase::GetProperty<Dali::Extents>(ImageVisual::Property::BORDER);
}

ImageVisual& ImageVisual::SetNPatchBorder(const Dali::Extents& border)
{
  VisualBase::SetProperty(ImageVisual::Property::BORDER, border);
  return *this;
}

bool ImageVisual::IsNPatchBorderOnly() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::BORDER_ONLY);
}

ImageVisual& ImageVisual::SetNPatchBorderOnly(bool borderOnly)
{
  VisualBase::SetProperty(ImageVisual::Property::BORDER_ONLY, borderOnly);
  return *this;
}

Dali::String ImageVisual::GetNPatchAuxiliaryImage() const
{
  return VisualBase::GetProperty<Dali::String>(ImageVisual::Property::AUXILIARY_IMAGE);
}

ImageVisual& ImageVisual::SetNPatchAuxiliaryImage(const Dali::String& auxiliaryImage)
{
  VisualBase::SetProperty(ImageVisual::Property::AUXILIARY_IMAGE, auxiliaryImage);
  return *this;
}

float ImageVisual::GetNPatchAuxiliaryImageAlpha() const
{
  return VisualBase::GetProperty<float>(ImageVisual::Property::AUXILIARY_IMAGE_ALPHA);
}

ImageVisual& ImageVisual::SetNPatchAuxiliaryImageAlpha(float auxiliaryImageAlpha)
{
  VisualBase::SetProperty(ImageVisual::Property::AUXILIARY_IMAGE_ALPHA, auxiliaryImageAlpha);
  return *this;
}

bool ImageVisual::IsPreMultipliedAlpha() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::PRE_MULTIPLIED_ALPHA);
}

ImageVisual& ImageVisual::SetPreMultipliedAlpha(bool preMultiplied)
{
  VisualBase::SetProperty(ImageVisual::Property::PRE_MULTIPLIED_ALPHA, preMultiplied);
  return *this;
}

Dali::String ImageVisual::GetAlphaMaskUrl() const
{
  return VisualBase::GetProperty<Dali::String>(ImageVisual::Property::ALPHA_MASK_URL);
}

ImageVisual& ImageVisual::SetAlphaMaskUrl(const Dali::String& alphaMaskUrl)
{
  VisualBase::SetProperty(ImageVisual::Property::ALPHA_MASK_URL, alphaMaskUrl);
  return *this;
}

float ImageVisual::GetMaskContentScale() const
{
  return VisualBase::GetProperty<float>(ImageVisual::Property::MASK_CONTENT_SCALE);
}

ImageVisual& ImageVisual::SetMaskContentScale(float maskContentScale)
{
  VisualBase::SetProperty(ImageVisual::Property::MASK_CONTENT_SCALE, maskContentScale);
  return *this;
}

bool ImageVisual::IsCropToMask() const
{
  return VisualBase::GetProperty<bool>(ImageVisual::Property::CROP_TO_MASK);
}

ImageVisual& ImageVisual::SetCropToMask(bool cropToMask)
{
  VisualBase::SetProperty(ImageVisual::Property::CROP_TO_MASK, cropToMask);
  return *this;
}

Image::MaskingType ImageVisual::GetMaskingType() const
{
  return VisualBase::GetProperty<Image::MaskingType>(ImageVisual::Property::MASKING_TYPE);
}

ImageVisual& ImageVisual::SetMaskingType(Image::MaskingType maskingType)
{
  VisualBase::SetProperty(ImageVisual::Property::MASKING_TYPE, maskingType);
  return *this;
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
