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
#include "image-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/provider-api/property-registration-helper.h>
#include <dali-ui-foundation/public-api/types/align-enumerations.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle CreateImageView()
{
  ImageViewImplPtr impl = ImageViewImpl::New();
  Ui::View         view(*impl);
  impl->Initialize();
  return view;
}
// clang-format off
#define IMAGE_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, ImageViewPropertyIndex, Ui::Integration, ImageViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(ImageViewImpl, ViewImpl, CreateImageView)

IMAGE_VIEW_PROPERTY_REGISTRATION("image",                 STRING,  IMAGE)
IMAGE_VIEW_PROPERTY_REGISTRATION("fittingMode",           INTEGER, FITTING_MODE)
IMAGE_VIEW_PROPERTY_REGISTRATION("samplingMode",          INTEGER, SAMPLING_MODE)
IMAGE_VIEW_PROPERTY_REGISTRATION("imageColor",            VECTOR4, IMAGE_COLOR)
IMAGE_VIEW_PROPERTY_REGISTRATION("preMultipliedAlpha",    BOOLEAN, PRE_MULTIPLIED_ALPHA)
IMAGE_VIEW_PROPERTY_REGISTRATION("placeholderImage",      STRING,  PLACEHOLDER_IMAGE)
IMAGE_VIEW_PROPERTY_REGISTRATION("alphaMaskUrl",          STRING,  ALPHA_MASK_URL)
IMAGE_VIEW_PROPERTY_REGISTRATION("cropToMask",            BOOLEAN, CROP_TO_MASK)
IMAGE_VIEW_PROPERTY_REGISTRATION("maskingMode",           INTEGER, MASKING_MODE)
IMAGE_VIEW_PROPERTY_REGISTRATION("desiredWidth",          INTEGER, DESIRED_WIDTH)
IMAGE_VIEW_PROPERTY_REGISTRATION("desiredHeight",         INTEGER, DESIRED_HEIGHT)
IMAGE_VIEW_PROPERTY_REGISTRATION("loadPolicy",            INTEGER, LOAD_POLICY)
IMAGE_VIEW_PROPERTY_REGISTRATION("releasePolicy",         INTEGER, RELEASE_POLICY)
IMAGE_VIEW_PROPERTY_REGISTRATION("synchronousLoading",    BOOLEAN, SYNCHRONOUS_LOADING)
IMAGE_VIEW_PROPERTY_REGISTRATION("imageLoadWithViewSize", BOOLEAN, IMAGE_LOAD_WITH_VIEW_SIZE)
IMAGE_VIEW_PROPERTY_REGISTRATION("fastTrackUploading",    BOOLEAN, FAST_TRACK_UPLOADING)
IMAGE_VIEW_PROPERTY_REGISTRATION("orientationCorrection", BOOLEAN, ORIENTATION_CORRECTION)
IMAGE_VIEW_PROPERTY_REGISTRATION("nPatchBorder",          VECTOR4, N_PATCH_BORDER)
IMAGE_VIEW_PROPERTY_REGISTRATION("nPatchBorderOnly",      BOOLEAN, N_PATCH_BORDER_ONLY)
DALI_ANIMATABLE_PROPERTY_REGISTRATION(Ui::Integration, ImageViewImpl, "pixelArea", VECTOR4, PIXEL_AREA)

DALI_TYPE_REGISTRATION_END()
#undef IMAGE_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

ImageViewImpl::ImageViewImpl()
: ViewImpl(),
  mUrl(),
  mPlaceholderUrl(),
  mAlphaMaskUrl(),
  mPixelArea(0.0f, 0.0f, 1.0f, 1.0f),
  mNPatchBorder(0.0f, 0.0f, 0.0f, 0.0f),
  mImageColor(Color::WHITE),
  mSamplingMode(Ui::Image::SamplingMode::BOX_THEN_LINEAR),
  mFittingMode(Ui::Image::FittingMode::FILL),
  mMaskingMode(Ui::Image::MaskingType::MASKING_ON_RENDERING),
  mLoadPolicy(Ui::Image::LoadPolicy::ATTACHED),
  mReleasePolicy(Ui::Image::ReleasePolicy::DETACHED),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mPreMultipliedAlpha(true), ///< Default as true for ImageView.
  mImageLoadWithViewSize(false),
  mCropToMask(false),
  mSynchronousLoading(false),
  mFastTrackUploading(false),
  mOrientationCorrection(true),
  mNPatchBorderOnly(false),
  mVisualDirty(false)
{
}

ImageViewImpl::~ImageViewImpl() = default;

ImageViewImplPtr ImageViewImpl::New()
{
  return new ImageViewImpl();
}

void ImageViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    ImageViewImpl& impl = static_cast<ImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case Property::IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetResourceUrl(url);
        }
        break;
      }
      case Property::FITTING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetFittingMode(static_cast<Ui::Image::FittingMode>(mode));
        }
        break;
      }
      case Property::SAMPLING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetSamplingMode(static_cast<Ui::Image::SamplingMode>(mode));
        }
        break;
      }
      case Property::DESIRED_WIDTH:
      {
        int width;
        if(value.Get(width))
        {
          impl.SetDesiredWidth(width);
        }
        break;
      }
      case Property::DESIRED_HEIGHT:
      {
        int height;
        if(value.Get(height))
        {
          impl.SetDesiredHeight(height);
        }
        break;
      }
      case Property::IMAGE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          impl.SetImageColor(UiColor(color));
        }
        break;
      }
      case Property::PRE_MULTIPLIED_ALPHA:
      {
        bool preMultiplied;
        if(value.Get(preMultiplied))
        {
          impl.SetPreMultipliedAlpha(preMultiplied);
        }
        break;
      }
      case Property::PLACEHOLDER_IMAGE:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetPlaceholderUrl(url);
        }
        break;
      }
      case Property::IMAGE_LOAD_WITH_VIEW_SIZE:
      {
        bool imageLoadWithViewSize;
        if(value.Get(imageLoadWithViewSize))
        {
          impl.SetImageLoadWithViewSize(imageLoadWithViewSize);
        }
        break;
      }
      case Property::ALPHA_MASK_URL:
      {
        Dali::String url;
        if(value.Get(url))
        {
          impl.SetAlphaMaskUrl(url);
        }
        break;
      }
      case Property::CROP_TO_MASK:
      {
        bool cropToMask;
        if(value.Get(cropToMask))
        {
          impl.SetCropToMask(cropToMask);
        }
        break;
      }
      case Property::MASKING_MODE:
      {
        int maskingMode;
        if(value.Get(maskingMode))
        {
          impl.SetMaskingMode(static_cast<Ui::Image::MaskingType>(maskingMode));
        }
        break;
      }
      case Property::LOAD_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetLoadPolicy(static_cast<Ui::Image::LoadPolicy>(policy));
        }
        break;
      }
      case Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::Image::ReleasePolicy>(policy));
        }
        break;
      }
      case Property::SYNCHRONOUS_LOADING:
      {
        bool sync;
        if(value.Get(sync))
        {
          impl.SetSynchronousLoading(sync);
        }
        break;
      }
      case Property::FAST_TRACK_UPLOADING:
      {
        bool fastTrack;
        if(value.Get(fastTrack))
        {
          impl.SetFastTrackUpload(fastTrack);
        }
        break;
      }
      case Property::ORIENTATION_CORRECTION:
      {
        bool orientationCorrection;
        if(value.Get(orientationCorrection))
        {
          impl.SetOrientationCorrection(orientationCorrection);
        }
        break;
      }
      case Property::N_PATCH_BORDER:
      {
        Vector4 border;
        if(value.Get(border))
        {
          impl.SetNPatchBorder(border);
        }
        break;
      }
      case Property::N_PATCH_BORDER_ONLY:
      {
        bool borderOnly;
        if(value.Get(borderOnly))
        {
          impl.SetNPatchBorderOnly(borderOnly);
        }
        break;
      }
      case Property::PIXEL_AREA:
      {
        Vector4 area;
        if(value.Get(area))
        {
          impl.SetPixelArea(area);
        }
        break;
      }
    }
  }
}

Dali::Property::Value ImageViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    ImageViewImpl& impl = static_cast<ImageViewImpl&>(GetImpl(view));
    switch(index)
    {
      case Property::IMAGE:
        value = impl.GetResourceUrl();
        break;
      case Property::FITTING_MODE:
        value = static_cast<int>(impl.GetFittingMode());
        break;
      case Property::SAMPLING_MODE:
        value = static_cast<int>(impl.GetSamplingMode());
        break;
      case Property::DESIRED_WIDTH:
        value = impl.GetDesiredWidth();
        break;
      case Property::DESIRED_HEIGHT:
        value = impl.GetDesiredHeight();
        break;
      case Property::IMAGE_COLOR:
        value = impl.GetImageColor().GetRgba();
        break;
      case Property::PRE_MULTIPLIED_ALPHA:
        value = impl.IsPreMultipliedAlpha();
        break;
      case Property::PLACEHOLDER_IMAGE:
        value = impl.GetPlaceholderUrl();
        break;
      case Property::IMAGE_LOAD_WITH_VIEW_SIZE:
        value = impl.IsImageLoadWithViewSizeEnabled();
        break;
      case Property::ALPHA_MASK_URL:
        value = impl.GetAlphaMaskUrl();
        break;
      case Property::CROP_TO_MASK:
        value = impl.IsCropToMask();
        break;
      case Property::MASKING_MODE:
        value = static_cast<int>(impl.GetMaskingMode());
        break;
      case Property::LOAD_POLICY:
        value = static_cast<int>(impl.GetLoadPolicy());
        break;
      case Property::RELEASE_POLICY:
        value = static_cast<int>(impl.GetReleasePolicy());
        break;
      case Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case Property::FAST_TRACK_UPLOADING:
        value = impl.IsFastTrackUploadEnabled();
        break;
      case Property::ORIENTATION_CORRECTION:
        value = impl.IsOrientationCorrectionEnabled();
        break;
      case Property::N_PATCH_BORDER:
        value = impl.GetNPatchBorder();
        break;
      case Property::N_PATCH_BORDER_ONLY:
        value = impl.IsNPatchBorderOnly();
        break;
      case Property::PIXEL_AREA:
        value = impl.GetPixelArea();
        break;
    }
  }
  return value;
}

void ImageViewImpl::Reload()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisual(ImageViewImpl::Property::IMAGE))
  {
    viewData.DoAction(ImageViewImpl::Property::IMAGE, Dali::Ui::Integration::ImageVisual::Action::RELOAD, Dali::Property::Map());
  }
}

void ImageViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    // Re-show placeholder while new image loads
    UpdatePlaceholderVisual();

    if(mLoadPolicy == Ui::Image::LoadPolicy::IMMEDIATE)
    {
      // Start loading immediately regardless of scene attachment.
      mVisualDirty = false;
      UpdateVisual();
    }
    else
    {
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

Dali::String ImageViewImpl::GetResourceUrl() const
{
  return mUrl;
}

void ImageViewImpl::SetPixelArea(const Vector4& pixelArea)
{
  if(mPixelArea != pixelArea)
  {
    mPixelArea = pixelArea;
    // Update the pixel area directly on the existing visual without a full rebuild.
    if(mVisual)
    {
      auto& visualImpl = Ui::GetImplementation(mVisual);
      visualImpl.SetPixelAreaForFittingMode(pixelArea);
    }
  }
}

Vector4 ImageViewImpl::GetPixelArea() const
{
  return mPixelArea;
}

void ImageViewImpl::SetPreMultipliedAlpha(bool preMultiplied)
{
  if(mPreMultipliedAlpha != preMultiplied)
  {
    mPreMultipliedAlpha = preMultiplied;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsPreMultipliedAlpha() const
{
  return mPreMultipliedAlpha;
}

void ImageViewImpl::SetPlaceholderUrl(const Dali::String& url)
{
  if(mPlaceholderUrl != url)
  {
    mPlaceholderUrl = url;
    UpdatePlaceholderVisual();
  }
}

Dali::String ImageViewImpl::GetPlaceholderUrl() const
{
  return mPlaceholderUrl;
}

void ImageViewImpl::SetSamplingMode(Ui::Image::SamplingMode samplingMode)
{
  if(mSamplingMode != samplingMode)
  {
    mSamplingMode = samplingMode;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::Image::SamplingMode ImageViewImpl::GetSamplingMode() const
{
  return mSamplingMode;
}

void ImageViewImpl::SetFittingMode(Ui::Image::FittingMode fittingMode)
{
  if(mFittingMode != fittingMode)
  {
    mFittingMode = fittingMode;
    if(mVisual)
    {
      Ui::GetImplementation(mVisual).SetFittingMode(fittingMode);

      // FittingMode only changes the visual transform, not the visual itself.
      // Apply immediately if the actor size is already available; otherwise
      // defer to the next OnArrange via InvalidateMeasure.
      Actor self = Self();
      float w    = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
      float h    = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
      if(w > 0.0f && h > 0.0f)
      {
        Ui::GetImplementation(mVisual).ApplyFittingMode(Vector2(w, h), GetPadding(), GetEffectiveScale());
      }
      else
      {
        InvalidateMeasure();
      }
    }
    else
    {
      InvalidateMeasure();
    }
  }
}

Ui::Image::FittingMode ImageViewImpl::GetFittingMode() const
{
  return mFittingMode;
}

void ImageViewImpl::SetDesiredWidth(int width)
{
  if(mDesiredWidth != width)
  {
    mDesiredWidth = width;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

int ImageViewImpl::GetDesiredWidth() const
{
  return mDesiredWidth;
}

void ImageViewImpl::SetDesiredHeight(int height)
{
  if(mDesiredHeight != height)
  {
    mDesiredHeight = height;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

int ImageViewImpl::GetDesiredHeight() const
{
  return mDesiredHeight;
}

void ImageViewImpl::SetImageLoadWithViewSize(bool enabled)
{
  if(mImageLoadWithViewSize != enabled)
  {
    mImageLoadWithViewSize = enabled;
    mVisualDirty           = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsImageLoadWithViewSizeEnabled() const
{
  return mImageLoadWithViewSize;
}

void ImageViewImpl::SetAlphaMaskUrl(const Dali::String& maskUrl)
{
  if(mAlphaMaskUrl != maskUrl)
  {
    mAlphaMaskUrl = maskUrl;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Dali::String ImageViewImpl::GetAlphaMaskUrl() const
{
  return mAlphaMaskUrl;
}

void ImageViewImpl::SetCropToMask(bool cropToMask)
{
  if(mCropToMask != cropToMask)
  {
    mCropToMask  = cropToMask;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsCropToMask() const
{
  return mCropToMask;
}

void ImageViewImpl::SetMaskingMode(Ui::Image::MaskingType maskingMode)
{
  if(mMaskingMode != maskingMode)
  {
    mMaskingMode = maskingMode;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Ui::Image::MaskingType ImageViewImpl::GetMaskingMode() const
{
  return mMaskingMode;
}

void ImageViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.GetRgba() != color.GetRgba())
  {
    mImageColor = color;
    // Update the mix color directly on the existing visual without a full rebuild.
    if(mVisual)
    {
      Dali::Property::Map map;
      map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());
      mVisual.DoAction(Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, map);
    }
    else
    {
      // Visual not yet created: defer to next OnMeasure pass
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

UiColor ImageViewImpl::GetImageColor() const
{
  return mImageColor;
}

void ImageViewImpl::SetLoadPolicy(Ui::Image::LoadPolicy loadPolicy)
{
  if(mLoadPolicy != loadPolicy)
  {
    mLoadPolicy = loadPolicy;
    if(mLoadPolicy == Ui::Image::LoadPolicy::IMMEDIATE && !mUrl.Empty())
    {
      // URL already set — start loading immediately now that policy switched to IMMEDIATE.
      mVisualDirty = false;
      UpdateVisual();
    }
    else
    {
      mVisualDirty = true;
      InvalidateMeasure();
    }
  }
}

Ui::Image::LoadPolicy ImageViewImpl::GetLoadPolicy() const
{
  return mLoadPolicy;
}

void ImageViewImpl::SetReleasePolicy(Ui::Image::ReleasePolicy releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

Ui::Image::ReleasePolicy ImageViewImpl::GetReleasePolicy() const
{
  return mReleasePolicy;
}

void ImageViewImpl::SetSynchronousLoading(bool synchronous)
{
  if(mSynchronousLoading != synchronous)
  {
    mSynchronousLoading = synchronous;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsSynchronousLoading() const
{
  return mSynchronousLoading;
}

void ImageViewImpl::SetFastTrackUpload(bool fastTrack)
{
  if(mFastTrackUploading != fastTrack)
  {
    mFastTrackUploading = fastTrack;
    mVisualDirty        = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsFastTrackUploadEnabled() const
{
  return mFastTrackUploading;
}

void ImageViewImpl::SetOrientationCorrection(bool orientationCorrection)
{
  if(mOrientationCorrection != orientationCorrection)
  {
    mOrientationCorrection = orientationCorrection;
    mVisualDirty           = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsOrientationCorrectionEnabled() const
{
  return mOrientationCorrection;
}

void ImageViewImpl::SetNPatchBorder(const Vector4& border)
{
  if(mNPatchBorder != border)
  {
    mNPatchBorder = border;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Vector4 ImageViewImpl::GetNPatchBorder() const
{
  return mNPatchBorder;
}

void ImageViewImpl::SetNPatchBorderOnly(bool borderOnly)
{
  if(mNPatchBorderOnly != borderOnly)
  {
    mNPatchBorderOnly = borderOnly;
    mVisualDirty      = true;
    InvalidateMeasure();
  }
}

bool ImageViewImpl::IsNPatchBorderOnly() const
{
  return mNPatchBorderOnly;
}

Ui::Visual::ResourceStatus ImageViewImpl::GetLoadingStatus() const
{
  return Internal::ViewDataImpl::Get(*this).GetVisualResourceStatus(ImageViewImpl::Property::IMAGE);
}

void ImageViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  // Connect to View::ResourceReadySignal to handle placeholder removal and aspect-ratio re-layout
  // when the main image visual becomes ready.
  Ui::View::DownCast(Self()).ResourceReadySignal().Connect(this, &ImageViewImpl::OnViewResourceReady);
}

void ImageViewImpl::UpdatePlaceholderVisual()
{
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(ImageViewImpl::Property::PLACEHOLDER_IMAGE);

  if(mPlaceholderUrl.Empty())
  {
    return;
  }

  // Don't show placeholder if main image is already loaded
  if(viewData.GetVisualResourceStatus(ImageViewImpl::Property::IMAGE) == Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  auto visualFactory = Ui::Integration::VisualFactory::Get();
  if(!visualFactory)
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  map.Insert(Ui::ImageVisualPropertyIndex::URL, mPlaceholderUrl);

  auto visual = visualFactory.CreateVisual(map);
  if(visual)
  {
    viewData.RegisterVisual(ImageViewImpl::Property::PLACEHOLDER_IMAGE, visual, Dali::Ui::Integration::DepthIndex::CONTENT + 1);
    viewData.EnableCornerPropertiesOverridden(visual, true);
  }
}

void ImageViewImpl::OnViewResourceReady(Ui::View view)
{
  // Only process when the main image visual becomes ready
  auto& viewData = Internal::ViewDataImpl::Get(*this);
  if(viewData.GetVisualResourceStatus(ImageViewImpl::Property::IMAGE) != Ui::Visual::ResourceStatus::READY)
  {
    return;
  }

  // Main image is ready: remove placeholder
  viewData.UnregisterVisual(ImageViewImpl::Property::PLACEHOLDER_IMAGE);

  // Request a re-layout now that the natural size is known, so aspect-ratio adjustment applies.
  InvalidateMeasure();
}

MeasuredSize ImageViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  if(mVisualDirty)
  {
    mVisualDirty = false;
    UpdateVisual();
  }

  // widthConstraint/heightConstraint are visual sizes; convert to natural for image measurement.
  float s    = GetEffectiveScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  Vector2 naturalSize;
  if(mVisual)
  {
    mVisual.GetNaturalSize(naturalSize);
  }

  float w = naturalSize.width;
  float h = naturalSize.height;

  float layoutW = GetRequestedWidth();
  float layoutH = GetRequestedHeight();

  if(layoutW == MATCH_PARENT)
  {
    w = natW;
  }
  else if(layoutW > 0)
  {
    w = layoutW;
  }

  if(layoutH == MATCH_PARENT)
  {
    h = natH;
  }
  else if(layoutH > 0)
  {
    h = layoutH;
  }

  if(naturalSize.width > 0.0f && naturalSize.height > 0.0f)
  {
    const float aspectRatio = naturalSize.height / naturalSize.width;
    const bool  widthFixed  = (layoutW == MATCH_PARENT || layoutW > 0);
    const bool  heightFixed = (layoutH == MATCH_PARENT || layoutH > 0);
    if(widthFixed && !heightFixed)
    {
      h = w * aspectRatio;
    }
    else if(!widthFixed && heightFixed)
    {
      w = h / aspectRatio;
    }
  }

  DALI_LOG_DEBUG_INFO("[ImageViewImpl] OnMeasure: constraint=(%.0f,%.0f) layoutW=%.0f layoutH=%.0f natural=(%.0f,%.0f) -> result=(%.0f,%.0f) hasVisual=%d\n",
                      widthConstraint, heightConstraint, layoutW, layoutH,
                      naturalSize.width, naturalSize.height, w, h, mVisual ? 1 : 0);

  return MeasuredSize(w * s, h * s);
}

MeasuredSize ImageViewImpl::OnArrange(const LayoutRect& bounds)
{
  DALI_LOG_DEBUG_INFO("[ImageViewImpl] OnArrange: bounds=(x=%.0f,y=%.0f,w=%.0f,h=%.0f) hasVisual=%d\n",
                      bounds.x, bounds.y, bounds.width, bounds.height, mVisual ? 1 : 0);
  return ViewImpl::OnArrange(bounds);
}

void ImageViewImpl::UpdateVisual()
{
  DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: url=%s\n", mUrl.CStr());

  auto& viewData = Internal::ViewDataImpl::Get(*this);

  if(mVisual)
  {
    viewData.UnregisterVisual(ImageViewImpl::Property::IMAGE);
    mVisual.Reset();
  }

  if(mUrl.Empty())
  {
    DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: url is empty, visual cleared\n");
    return;
  }

  auto visualFactory = Ui::Integration::VisualFactory::Get();
  if(visualFactory)
  {
    Dali::Property::Map map;
    map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
    map.Insert(Ui::ImageVisualPropertyIndex::URL, mUrl);
    map.Insert(Ui::ImageVisualPropertyIndex::SAMPLING_MODE, static_cast<int>(mSamplingMode));
    map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, mImageColor.GetRgba());
    map.Insert(Ui::ImageVisualPropertyIndex::PRE_MULTIPLIED_ALPHA, mPreMultipliedAlpha);

    if(mDesiredWidth > 0 || mDesiredHeight > 0)
    {
      map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_WIDTH, mDesiredWidth);
      map.Insert(Ui::ImageVisualPropertyIndex::DESIRED_HEIGHT, mDesiredHeight);
    }

    map.Insert(Ui::ImageVisualPropertyIndex::LOAD_POLICY, static_cast<int>(mLoadPolicy));
    map.Insert(Ui::ImageVisualPropertyIndex::RELEASE_POLICY, static_cast<int>(mReleasePolicy));
    map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_LOADING, mSynchronousLoading);
    map.Insert(Ui::ImageVisualPropertyIndex::FAST_TRACK_UPLOADING, mFastTrackUploading);
    map.Insert(Ui::ImageVisualPropertyIndex::ORIENTATION_CORRECTION, mOrientationCorrection);
    map.Insert(Ui::ImageVisualPropertyIndex::SYNCHRONOUS_SIZING, mImageLoadWithViewSize);
    map.Insert(Ui::ImageVisualPropertyIndex::FITTING_MODE, static_cast<int>(mFittingMode));

    if(!mAlphaMaskUrl.Empty())
    {
      map.Insert(Ui::ImageVisualPropertyIndex::ALPHA_MASK_URL, mAlphaMaskUrl);
      map.Insert(Ui::ImageVisualPropertyIndex::CROP_TO_MASK, mCropToMask);
      map.Insert(Ui::ImageVisualPropertyIndex::MASKING_TYPE, static_cast<int>(mMaskingMode));
    }

    if(mNPatchBorder != Vector4::ZERO)
    {
      map.Insert(Ui::ImageVisualPropertyIndex::BORDER, mNPatchBorder);
      map.Insert(Ui::ImageVisualPropertyIndex::BORDER_ONLY, mNPatchBorderOnly);
    }

    // ImageView is a static image widget: always use ImageVisual (or NPatchVisual)
    // even when the URL points to an animated format such as GIF or WebP.
    // IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY tells the factory not to upgrade
    // a GIF/WebP URL to AnimatedImageVisual. Only the first frame is decoded.
    // (NPatch detection via BORDER property is unaffected by this flag.)
    mVisual = visualFactory.CreateVisual(map, Ui::Integration::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
    if(mVisual)
    {
      DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: RegisterVisual result=ok\n");
      viewData.RegisterVisual(ImageViewImpl::Property::IMAGE, mVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
      viewData.EnableCornerPropertiesOverridden(mVisual, true);
    }
    else
    {
      DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: CreateVisual failed\n");
    }
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
