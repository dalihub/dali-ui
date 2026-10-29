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
#include <dali-ui-foundation/integration-api/media-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/streamed-image/streamed-image-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-factory-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
namespace
{
using Ui::Internal::StreamedImageVisual;

/**
 * @brief Converts backend presentation metadata to a streamed image frame.
 * @param[in] presentation The current frame and its displayed geometry
 * @return The frame with crop and rotation applied by StreamedImageVisual
 */
StreamedImageVisual::Frame MakeStreamedFrame(const Dali::Integration::MediaViewPresentation& presentation)
{
  StreamedImageVisual::Frame frame;
  frame.texture        = presentation.texture;
  frame.contentSize    = presentation.contentSize;
  frame.crop           = presentation.sourceCrop;
  frame.horizontalFlip = presentation.horizontalFlip;
  frame.verticalFlip   = presentation.verticalFlip;

  using Transform = Dali::Integration::MediaFrameTransform;
  using Rotation  = StreamedImageVisual::Rotation;
  switch(presentation.sourceTransform)
  {
    case Transform::IDENTITY:
    {
      break;
    }
    case Transform::ROTATE_90:
    {
      frame.rotation = Rotation::DEGREE_90;
      break;
    }
    case Transform::ROTATE_180:
    {
      frame.rotation = Rotation::DEGREE_180;
      break;
    }
    case Transform::ROTATE_270:
    {
      frame.rotation = Rotation::DEGREE_270;
      break;
    }
    case Transform::HORIZONTAL_FLIP:
    {
      frame.horizontalFlip = !frame.horizontalFlip;
      break;
    }
    case Transform::VERTICAL_FLIP:
    {
      frame.verticalFlip = !frame.verticalFlip;
      break;
    }
  }

  // The backend reports the cropped, rotated size, whereas StreamedImageVisual
  // expects the uncropped, unrotated content size. Undo both operations rather
  // than using texture dimensions, which may include producer padding.
  if(frame.rotation == Rotation::DEGREE_90 || frame.rotation == Rotation::DEGREE_270)
  {
    std::swap(frame.contentSize.width, frame.contentSize.height);
  }
  frame.contentSize.width /= frame.crop.width;
  frame.contentSize.height /= frame.crop.height;
  return frame;
}

BaseHandle Create()
{
  return Ui::MediaView::New();
}

DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::MediaView, Ui::Integration::MediaViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

MediaViewImpl::MediaViewImpl()
: ViewImpl()
{
}

MediaViewImpl::~MediaViewImpl()
{
  if(mBackend)
  {
    mBackend->PresentationChangedSignal().Disconnect(this, &MediaViewImpl::OnPresentationChanged);
    mBackend->ClearSource();
  }
  // The base class owns registered visuals during destruction.
  mVisual.Reset();
}

Ui::MediaView MediaViewImpl::New()
{
  auto*         impl = new MediaViewImpl;
  Ui::MediaView view(*impl);
  impl->Initialize();
  return view;
}

void MediaViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
  mContentVisualIndex = Self().RegisterProperty("mediaViewContent", "mediaViewContent", Property::AccessMode::READ_WRITE);
  mBackend            = Dali::MakeUnique<Dali::Integration::MediaViewBackend>();
  mBackend->PresentationChangedSignal().Connect(this, &MediaViewImpl::OnPresentationChanged);
}

void MediaViewImpl::OnPresentationChanged()
{
  const auto presentation = mBackend->GetPresentation();
  if(!presentation.texture)
  {
    ClearVisual();
    InvalidateMeasure();
    return;
  }
  const auto frame = MakeStreamedFrame(presentation);
  if(!mVisual)
  {
    auto  factoryHandle = Ui::Integration::VisualFactory::Get();
    auto& factory       = Ui::GetImplementation(factoryHandle);
    auto  visual        = Ui::Internal::StreamedImageVisual::New(factory.GetFactoryCache(), frame);
    mVisual             = Ui::Integration::Visual::Base(visual.Get());
    auto& viewData      = Ui::Internal::ViewDataImpl::Get(*this);
    viewData.RegisterVisual(mContentVisualIndex, mVisual, Ui::Integration::DepthIndex::CONTENT);
    viewData.EnableCornerPropertiesOverridden(mVisual, true);
  }
  else
  {
    auto& visual = static_cast<Ui::Internal::StreamedImageVisual&>(Ui::GetImplementation(mVisual));
    visual.SetFrame(frame);
  }
  InvalidateMeasure();
}

MeasuredSize MediaViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // widthConstraint/heightConstraint are visual sizes; convert to natural for image measurement.
  float s    = GetEffectiveLayoutScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  float layoutW = GetRequestedWidth();
  float layoutH = GetRequestedHeight();

  // Explicit dimensions take precedence over the frame natural size.
  if(layoutW > 0.0f && layoutH > 0.0f)
  {
    return MeasuredSize(layoutW * s, layoutH * s);
  }

  Vector2 naturalSize = mBackend->GetPresentation().contentSize;

  float w = naturalSize.width;
  float h = naturalSize.height;

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

  return MeasuredSize(w * s, h * s);
}

LayoutRect MediaViewImpl::OnArrange(const LayoutRect& bounds)
{
  const LayoutRect arranged = ViewImpl::OnArrange(bounds);
  if(mVisual)
  {
    Insets padding = GetPadding();
    if(Self().GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT)
    {
      std::swap(padding.start, padding.end);
    }
    Ui::GetImplementation(mVisual).ApplyFittingMode(Vector2(arranged.width, arranged.height), padding, GetEffectiveLayoutScale());
  }
  return arranged;
}

void MediaViewImpl::ClearVisual()
{
  if(mVisual)
  {
    Ui::Internal::ViewDataImpl::Get(*this).UnregisterVisual(mContentVisualIndex);
    mVisual.Reset();
  }
}

void MediaViewImpl::OnSceneConnection(int depth)
{
  ViewImpl::OnSceneConnection(depth);
  mBackend->SetSceneConnected(true);
}

void MediaViewImpl::OnSceneDisconnection()
{
  mBackend->SetSceneConnected(false);
  ViewImpl::OnSceneDisconnection();
}
} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
