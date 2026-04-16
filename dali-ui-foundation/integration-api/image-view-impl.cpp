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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/image-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>
#include <dali-ui-foundation/integration-api/property-registration-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/ui-color.h>
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
  mSamplingMode(Ui::SamplingMode::BOX_THEN_LINEAR),
  mFittingMode(Ui::FittingMode::FILL),
  mMaskingMode(Ui::MaskingType::MASKING_ON_RENDERING),
  mLoadPolicy(Ui::LoadPolicy::ATTACHED),
  mReleasePolicy(Ui::ReleasePolicy::DETACHED),
  mDesiredWidth(0),
  mDesiredHeight(0),
  mDepthIndex(DepthIndex::CONTENT),
  mPreMultipliedAlpha(false),
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
          impl.SetFittingMode(static_cast<Ui::FittingMode::Type>(mode));
        }
        break;
      }
      case Property::SAMPLING_MODE:
      {
        int mode;
        if(value.Get(mode))
        {
          impl.SetSamplingMode(static_cast<Ui::SamplingMode::Type>(mode));
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
          impl.SetMaskingMode(static_cast<Ui::MaskingType::Type>(maskingMode));
        }
        break;
      }
      case Property::LOAD_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetLoadPolicy(static_cast<Ui::LoadPolicy::Type>(policy));
        }
        break;
      }
      case Property::RELEASE_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          impl.SetReleasePolicy(static_cast<Ui::ReleasePolicy::Type>(policy));
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
        value = impl.GetImageColor().Resolve();
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
    viewData.DoAction(ImageViewImpl::Property::IMAGE, DevelImageVisual::Action::RELOAD, Dali::Property::Map());
  }
}

void ImageViewImpl::SetResourceUrl(const Dali::String& url)
{
  if(mUrl != url)
  {
    mUrl = url;
    // Re-show placeholder while new image loads
    UpdatePlaceholderVisual();

    if(mLoadPolicy == Ui::LoadPolicy::IMMEDIATE)
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

void ImageViewImpl::SetSamplingMode(Ui::SamplingMode::Type samplingMode)
{
  if(mSamplingMode != samplingMode)
  {
    mSamplingMode = samplingMode;
    mVisualDirty  = true;
    InvalidateMeasure();
  }
}

Ui::SamplingMode::Type ImageViewImpl::GetSamplingMode() const
{
  return mSamplingMode;
}

void ImageViewImpl::SetFittingMode(Ui::FittingMode::Type fittingMode)
{
  if(mFittingMode != fittingMode)
  {
    mFittingMode = fittingMode;
    // FittingMode only changes the visual transform, not the visual itself.
    // Apply immediately if the actor size is already available; otherwise
    // defer to the next OnArrange via InvalidateMeasure.
    Actor self = Self();
    float w    = self.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    float h    = self.GetProperty<float>(Actor::Property::SIZE_HEIGHT);
    if(w > 0.0f && h > 0.0f)
    {
      ApplyFittingMode(Vector2(w, h));
    }
    else
    {
      InvalidateMeasure();
    }
  }
}

Ui::FittingMode::Type ImageViewImpl::GetFittingMode() const
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

void ImageViewImpl::SetMaskingMode(Ui::MaskingType::Type maskingMode)
{
  if(mMaskingMode != maskingMode)
  {
    mMaskingMode = maskingMode;
    mVisualDirty = true;
    InvalidateMeasure();
  }
}

Ui::MaskingType::Type ImageViewImpl::GetMaskingMode() const
{
  return mMaskingMode;
}

void ImageViewImpl::SetImageColor(const UiColor& color)
{
  if(mImageColor.Resolve() != color.Resolve())
  {
    mImageColor = color;
    // Update the mix color directly on the existing visual without a full rebuild.
    if(mVisual)
    {
      Dali::Property::Map map;
      map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
      mVisual.DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
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

void ImageViewImpl::SetLoadPolicy(Ui::LoadPolicy::Type loadPolicy)
{
  if(mLoadPolicy != loadPolicy)
  {
    mLoadPolicy = loadPolicy;
    if(mLoadPolicy == Ui::LoadPolicy::IMMEDIATE && !mUrl.Empty())
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

Ui::LoadPolicy::Type ImageViewImpl::GetLoadPolicy() const
{
  return mLoadPolicy;
}

void ImageViewImpl::SetReleasePolicy(Ui::ReleasePolicy::Type releasePolicy)
{
  if(mReleasePolicy != releasePolicy)
  {
    mReleasePolicy = releasePolicy;
    mVisualDirty   = true;
    InvalidateMeasure();
  }
}

Ui::ReleasePolicy::Type ImageViewImpl::GetReleasePolicy() const
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

void ImageViewImpl::SetDepthIndex(int depthIndex)
{
  if(mDepthIndex != depthIndex)
  {
    mDepthIndex = depthIndex;
    if(mVisual)
    {
      Internal::ViewDataImpl::Get(*this).RegisterVisual(ImageViewImpl::Property::IMAGE, mVisual, mDepthIndex);
      UpdatePlaceholderVisual();
    }
  }
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

  auto visualFactory = Ui::VisualFactory::Get();
  if(!visualFactory)
  {
    return;
  }

  Dali::Property::Map map;
  map.Insert(Visual::Property::TYPE, Visual::IMAGE);
  map.Insert(Ui::ImageVisual::Property::URL, mPlaceholderUrl);

  auto visual = visualFactory.CreateVisual(map);
  if(visual)
  {
    viewData.RegisterVisual(ImageViewImpl::Property::PLACEHOLDER_IMAGE, visual, mDepthIndex + 1);
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
    w = widthConstraint;
  }
  else if(layoutW > 0)
  {
    w = layoutW;
  }

  if(layoutH == MATCH_PARENT)
  {
    h = heightConstraint;
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

  return MeasuredSize(w, h);
}

MeasuredSize ImageViewImpl::OnArrange(const LayoutRect& bounds)
{
  DALI_LOG_DEBUG_INFO("[ImageViewImpl] OnArrange: bounds=(x=%.0f,y=%.0f,w=%.0f,h=%.0f) hasVisual=%d\n",
                      bounds.x, bounds.y, bounds.width, bounds.height, mVisual ? 1 : 0);
  MeasuredSize result = ViewImpl::OnArrange(bounds);
  ApplyFittingMode(Vector2(bounds.width, bounds.height));
  return result;
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

  auto visualFactory = Ui::VisualFactory::Get();
  if(visualFactory)
  {
    Dali::Property::Map map;
    map.Insert(Visual::Property::TYPE, Visual::IMAGE);
    map.Insert(Ui::ImageVisual::Property::URL, mUrl);
    map.Insert(Ui::ImageVisual::Property::SAMPLING_MODE, static_cast<int>(mSamplingMode));
    map.Insert(Visual::Property::MIX_COLOR, mImageColor.Resolve());
    map.Insert(Visual::Property::PREMULTIPLIED_ALPHA, mPreMultipliedAlpha);

    if(mDesiredWidth > 0 || mDesiredHeight > 0)
    {
      map.Insert(Ui::ImageVisual::Property::DESIRED_WIDTH, mDesiredWidth);
      map.Insert(Ui::ImageVisual::Property::DESIRED_HEIGHT, mDesiredHeight);
    }

    map.Insert(Ui::ImageVisual::Property::LOAD_POLICY, static_cast<int>(mLoadPolicy));
    map.Insert(Ui::ImageVisual::Property::RELEASE_POLICY, static_cast<int>(mReleasePolicy));
    map.Insert(Ui::ImageVisual::Property::SYNCHRONOUS_LOADING, mSynchronousLoading);
    map.Insert(Ui::DevelImageVisual::Property::FAST_TRACK_UPLOADING, mFastTrackUploading);
    map.Insert(Ui::ImageVisual::Property::ORIENTATION_CORRECTION, mOrientationCorrection);
    map.Insert(Ui::DevelImageVisual::Property::SYNCHRONOUS_SIZING, mImageLoadWithViewSize);

    if(!mAlphaMaskUrl.Empty())
    {
      map.Insert(Ui::ImageVisual::Property::ALPHA_MASK_URL, mAlphaMaskUrl);
      map.Insert(Ui::ImageVisual::Property::CROP_TO_MASK, mCropToMask);
      map.Insert(Ui::DevelImageVisual::Property::MASKING_TYPE, static_cast<int>(mMaskingMode));
    }

    if(mNPatchBorder != Vector4::ZERO)
    {
      map.Insert(Ui::ImageVisual::Property::BORDER, mNPatchBorder);
      map.Insert(Ui::ImageVisual::Property::BORDER_ONLY, mNPatchBorderOnly);
    }

    // ImageView is a static image widget: always use ImageVisual (or NPatchVisual)
    // even when the URL points to an animated format such as GIF or WebP.
    // IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY tells the factory not to upgrade
    // a GIF/WebP URL to AnimatedImageVisual. Only the first frame is decoded.
    // (NPatch detection via BORDER property is unaffected by this flag.)
    mVisual = visualFactory.CreateVisual(map, Ui::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY);
    if(mVisual)
    {
      DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: RegisterVisual result=ok depthIndex=%d\n", mDepthIndex);
      viewData.RegisterVisual(ImageViewImpl::Property::IMAGE, mVisual, mDepthIndex);
    }
    else
    {
      DALI_LOG_DEBUG_INFO("[ImageViewImpl] UpdateVisual: CreateVisual failed\n");
    }
  }
}

void ImageViewImpl::ApplyFittingMode(const Vector2& size)
{
  DALI_LOG_DEBUG_INFO("[ImageViewImpl] ApplyFittingMode: size=(%.0f,%.0f) fittingMode=%d hasVisual=%d\n",
                      size.width, size.height, static_cast<int>(mFittingMode), mVisual ? 1 : 0);
  if(!mVisual)
  {
    return;
  }

  auto& visualImpl = Ui::GetImplementation(mVisual);

  if(visualImpl.IsIgnoreFittingMode())
  {
    return;
  }

  if(mFittingMode == Ui::FittingMode::DONT_CARE)
  {
    mVisual.SetTransformAndSize(Dali::Property::Map(), size);
    return;
  }

  Extents padding = GetPadding();

  // Reset PIXEL_AREA after using OVER_FIT_KEEP_ASPECT_RATIO
  if(visualImpl.IsPixelAreaSetForFittingMode())
  {
    static const Vector4 FULL_TEXTURE_RECT(0.f, 0.f, 1.f, 1.f);
    visualImpl.SetPixelAreaForFittingMode(FULL_TEXTURE_RECT);
  }

  Vector2 finalSize   = size - Vector2(padding.start + padding.end, padding.top + padding.bottom);
  Vector2 finalOffset = Vector2(padding.start, padding.top);

  bool                zeroPadding = (padding == Extents());
  Dali::Property::Map transformMap;

  auto fittingMode = static_cast<DevelVisual::FittingMode>(mFittingMode);

  // FIT_WIDTH/FIT_HEIGHT: resolve to FIT_KEEP or OVER_FIT based on aspect ratio
  if(fittingMode == DevelVisual::FIT_WIDTH || fittingMode == DevelVisual::FIT_HEIGHT)
  {
    Vector2 naturalSize;
    visualImpl.GetNaturalSize(naturalSize);
    const float widthRatio  = !Dali::EqualsZero(naturalSize.width) ? (finalSize.width / naturalSize.width) : 0.0f;
    const float heightRatio = !Dali::EqualsZero(naturalSize.height) ? (finalSize.height / naturalSize.height) : 0.0f;
    if(widthRatio < heightRatio)
    {
      fittingMode = (fittingMode == DevelVisual::FIT_WIDTH) ? DevelVisual::FIT_KEEP_ASPECT_RATIO : DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO;
    }
    else
    {
      fittingMode = (fittingMode == DevelVisual::FIT_WIDTH) ? DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO : DevelVisual::FIT_KEEP_ASPECT_RATIO;
    }
  }

  if((!zeroPadding) || (fittingMode != DevelVisual::FILL))
  {
    visualImpl.SetTransformMapUsageForFittingMode(true);

    Vector2 naturalSize;
    if(fittingMode != DevelVisual::FILL)
    {
      visualImpl.GetNaturalSize(naturalSize);
    }

    switch(fittingMode)
    {
      case DevelVisual::FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case DevelVisual::OVER_FIT_KEEP_ASPECT_RATIO:
      {
        auto availableVisualSize = finalSize;
        finalSize                = naturalSize * std::max((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                                          (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        auto originalOffset      = finalOffset;
        if(!visualImpl.IsPixelAreaSetForFittingMode() && !Dali::EqualsZero(finalSize.width) && !Dali::EqualsZero(finalSize.height))
        {
          float   x           = std::abs((availableVisualSize.width - finalSize.width) / finalSize.width) * 0.5f;
          float   y           = std::abs((availableVisualSize.height - finalSize.height) / finalSize.height) * 0.5f;
          float   widthRatio  = 1.f - std::abs((availableVisualSize.width - finalSize.width) / finalSize.width);
          float   heightRatio = 1.f - std::abs((availableVisualSize.height - finalSize.height) / finalSize.height);
          Vector4 pixelArea(x, y, widthRatio, heightRatio);
          visualImpl.SetPixelAreaForFittingMode(pixelArea);
        }
        transformMap.Add(Visual::Transform::Property::OFFSET, originalOffset)
          .Add(Visual::Transform::Property::SIZE, availableVisualSize);
        break;
      }
      case DevelVisual::CENTER:
      {
        auto availableVisualSize = finalSize;
        if(availableVisualSize.width > naturalSize.width && availableVisualSize.height > naturalSize.height)
        {
          finalSize = naturalSize;
        }
        else
        {
          finalSize = naturalSize * std::min((!Dali::EqualsZero(naturalSize.width) ? (availableVisualSize.width / naturalSize.width) : 0.0f),
                                             (!Dali::EqualsZero(naturalSize.height) ? (availableVisualSize.height / naturalSize.height) : 0.0f));
        }
        finalOffset += (availableVisualSize - finalSize) * 0.5f;
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
      case DevelVisual::FILL:
      default:
      {
        transformMap.Add(Visual::Transform::Property::OFFSET, finalOffset)
          .Add(Visual::Transform::Property::SIZE, finalSize);
        break;
      }
    }

    transformMap.Add(Visual::Transform::Property::OFFSET_POLICY,
                     Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE))
      .Add(Visual::Transform::Property::ORIGIN, Align::TOP_BEGIN)
      .Add(Visual::Transform::Property::ANCHOR_POINT, Align::TOP_BEGIN)
      .Add(Visual::Transform::Property::SIZE_POLICY,
           Vector2(Visual::Transform::Policy::ABSOLUTE, Visual::Transform::Policy::ABSOLUTE));
  }
  else if(visualImpl.IsTransformMapSetForFittingMode() && zeroPadding)
  {
    visualImpl.SetTransformMapUsageForFittingMode(false);
    transformMap.Add(Visual::Transform::Property::OFFSET, Vector2::ZERO)
      .Add(Visual::Transform::Property::OFFSET_POLICY,
           Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE))
      .Add(Visual::Transform::Property::SIZE, Vector2::ONE)
      .Add(Visual::Transform::Property::SIZE_POLICY,
           Vector2(Visual::Transform::Policy::RELATIVE, Visual::Transform::Policy::RELATIVE));
  }

  DALI_LOG_DEBUG_INFO("[ImageViewImpl] ApplyFittingMode: transformMap.Count=%zu finalSize=(%.1f,%.1f) finalOffset=(%.1f,%.1f)\n",
                      transformMap.Count(), finalSize.width, finalSize.height, finalOffset.x, finalOffset.y);

  mVisual.SetTransformAndSize(transformMap, size);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
