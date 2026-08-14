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
#include "video-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/video/video-source-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/video/video-source.h>
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
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(VideoViewImpl, ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

// Visual slot for the transparent underlay hole. Reserved in this view's property range.
constexpr Property::Index UNDERLAY_VISUAL_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 1;

// Visual slot for the NativeImage texture (ESPlayer NativeImage mode).
constexpr Property::Index NATIVEIMAGE_VISUAL_INDEX = Ui::VIEW_PROPERTY_END_INDEX + 2;

// Applies the hole-punch blending used for underlay rendering: the video display area
// must become transparent so the platform-composited video shows through the UI. This
// matches the toolkit VideoView WindowSurfaceStrategy blending.
void ApplyHolePunchBlend(Dali::Renderer renderer)
{
  if(!renderer)
  {
    return;
  }

  renderer.SetProperty(Dali::Renderer::Property::BLEND_MODE, Dali::BlendMode::ON);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB, Dali::BlendFactor::ZERO);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB, Dali::BlendFactor::ONE_MINUS_SRC_ALPHA);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA, Dali::BlendFactor::ONE);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA, Dali::BlendFactor::ONE);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_EQUATION_RGB, Dali::BlendEquation::ADD);
  renderer.SetProperty(Dali::Renderer::Property::BLEND_EQUATION_ALPHA, Dali::BlendEquation::REVERSE_SUBTRACT);
}
} // namespace

VideoViewImpl::VideoViewImpl()
: ViewImpl(),
  mSource(),
  mVideoPlayer(),
  mSyncMode(Dali::VideoSyncMode::DISABLED),
  mAttachedToScene(false)
{
}

VideoViewImpl::~VideoViewImpl()
{
}

VideoViewImplPtr VideoViewImpl::New()
{
  return new VideoViewImpl();
}

bool VideoViewImpl::SetSource(VideoSource source)
{
  if(!source || !source.IsValid())
  {
    return false;
  }

  ClearSource();
  mSource = source;
  CreateVideoPlayer();

  if(!mVideoPlayer)
  {
    mSource = VideoSource();
    return false;
  }

  AttachToScene();
  return true;
}

VideoSource VideoViewImpl::GetSource() const
{
  return mSource;
}

void VideoViewImpl::ClearSource()
{
  DetachFromScene();
  RemoveUnderlayVisual();
  RemoveNativeImageVisual();
  mVideoPlayer = VideoPlayer();
  mSource      = VideoSource();
}

void VideoViewImpl::Play()
{
  if(mVideoPlayer)
  {
    mVideoPlayer.Play();
  }
}

void VideoViewImpl::Pause()
{
  if(mVideoPlayer)
  {
    mVideoPlayer.Pause();
  }
}

void VideoViewImpl::Stop()
{
  if(mVideoPlayer)
  {
    mVideoPlayer.Stop();
  }
}

void VideoViewImpl::SetSyncMode(Ui::VideoSyncMode syncMode)
{
  Dali::VideoSyncMode newSyncMode = (syncMode == Ui::VideoSyncMode::ENABLED) ? Dali::VideoSyncMode::ENABLED : Dali::VideoSyncMode::DISABLED;
  if(mSyncMode == newSyncMode)
  {
    return;
  }
  mSyncMode = newSyncMode;

  if(mSource)
  {
    // The video player only reads mSyncMode at creation time, so recreate it against the
    // same source for the new mode to take effect immediately instead of on the next SetSource().
    DetachFromScene();
    RemoveUnderlayVisual();
    RemoveNativeImageVisual();
    CreateVideoPlayer();
    AttachToScene();
  }
}

Ui::VideoSyncMode VideoViewImpl::GetSyncMode() const
{
  return (mSyncMode == Dali::VideoSyncMode::ENABLED) ? Ui::VideoSyncMode::ENABLED : Ui::VideoSyncMode::DISABLED;
}

void VideoViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();
}

LayoutRect VideoViewImpl::OnArrange(const LayoutRect& bounds)
{
  LayoutRect result = ViewImpl::OnArrange(bounds);
  UpdateDisplayArea();
  return result;
}

void VideoViewImpl::OnSceneConnection(int depth)
{
  ViewImpl::OnSceneConnection(depth);
  AttachToScene();
}

void VideoViewImpl::OnSceneDisconnection()
{
  DetachFromScene();
  ViewImpl::OnSceneDisconnection();
}

void VideoViewImpl::CreateVideoPlayer()
{
  if(!mSource)
  {
    return;
  }

  mVideoPlayer = VideoPlayer::New(Self(), GetImpl(mSource).ToAdaptorDescriptor(), mSyncMode);
}

void VideoViewImpl::AttachToScene()
{
  if(mAttachedToScene || !mVideoPlayer || !Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    return;
  }

  const bool isNativeImageMode = mSource &&
                                 (mSource.GetRenderingMode() == VideoRenderingMode::NATIVE_IMAGE);

  if(isNativeImageMode)
  {
    EnsureNativeImageVisual();
    if(mNativeImagePtr)
    {
      mVideoPlayer.SetRenderingTarget(Dali::Any(mNativeImagePtr));
    }
  }
  else
  {
    EnsureUnderlayVisual();
    if(Adaptor::IsAvailable())
    {
      Any windowHandle = Adaptor::Get().GetNativeWindowHandle(Self());
      mVideoPlayer.SetRenderingTarget(windowHandle);
    }
  }

  ConnectGeometrySync();
  UpdateDisplayArea();
  mVideoPlayer.SceneConnection();
  mAttachedToScene = true;
}

void VideoViewImpl::DetachFromScene()
{
  if(!mAttachedToScene || !mVideoPlayer)
  {
    mAttachedToScene = false;
    return;
  }

  DisconnectGeometrySync();
  mVideoPlayer.SceneDisconnection();
  mAttachedToScene = false;
}

void VideoViewImpl::UpdateDisplayArea()
{
  if(!mVideoPlayer)
  {
    return;
  }

  Actor self = Self();

  const bool    positionUsesPivot = self.GetProperty(Actor::Property::POSITION_USES_PIVOT).Get<bool>();
  const Vector3 actorSize         = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE) * self.GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  const Vector3 pivotOffset       = actorSize * (positionUsesPivot ? self.GetCurrentProperty<Vector3>(Actor::Property::PIVOT) : Pivot::TOP_LEFT);
  const Vector2 screenPosition    = self.GetProperty(Actor::Property::SCREEN_POSITION).Get<Vector2>();

  DisplayArea displayArea;
  displayArea.x      = static_cast<int32_t>(screenPosition.x - pivotOffset.x);
  displayArea.y      = static_cast<int32_t>(screenPosition.y - pivotOffset.y);
  displayArea.width  = static_cast<int32_t>(actorSize.x);
  displayArea.height = static_cast<int32_t>(actorSize.y);

  mVideoPlayer.SetDisplayArea(displayArea);
}

void VideoViewImpl::EnsureUnderlayVisual()
{
  if(mUnderlayVisual)
  {
    return;
  }

  Property::Map properties;
  properties.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::COLOR);
  properties.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, Color::BLACK);

  mUnderlayVisual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  if(!mUnderlayVisual)
  {
    DALI_LOG_ERROR("VideoViewImpl: failed to create underlay visual\n");
    return;
  }

  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.RegisterVisual(UNDERLAY_VISUAL_INDEX, mUnderlayVisual, Ui::Integration::DepthIndex::BACKGROUND);
  Ui::GetImplementation(mUnderlayVisual).CornerRadiusIgnoredAtOffscreenRendering(true);
  viewData.EnableCornerPropertiesOverridden(mUnderlayVisual, true);

  // The color visual draws the view quad; override its blend so the area is punched
  // transparent instead of filled. Applied after registration/corner sync so it is
  // the last writer.
  ApplyHolePunchBlend(mUnderlayVisual.GetRenderer());
}

void VideoViewImpl::RemoveUnderlayVisual()
{
  if(!mUnderlayVisual)
  {
    return;
  }

  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(UNDERLAY_VISUAL_INDEX);
  mUnderlayVisual.Reset();
}

void VideoViewImpl::EnsureNativeImageVisual()
{
  if(mNativeImageVisual)
  {
    return;
  }

  Actor    self   = Self();
  Vector3  size   = self.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  uint32_t width  = (size.width > 0.0f) ? static_cast<uint32_t>(size.width) : 1u;
  uint32_t height = (size.height > 0.0f) ? static_cast<uint32_t>(size.height) : 1u;

  mNativeImagePtr = Dali::NativeImage::New(width, height, Dali::NativeImage::COLOR_DEPTH_DEFAULT);
  if(!mNativeImagePtr)
  {
    DALI_LOG_ERROR("VideoViewImpl: failed to create NativeImage (%ux%u)\n", width, height);
    return;
  }

  Dali::Ui::ImageUrl nativeImageUrl = Dali::Ui::ImageUrlUtils::GenerateUrl(mNativeImagePtr);

  Property::Map properties;
  properties.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
  properties.Insert(Ui::ImageVisualPropertyIndex::URL, nativeImageUrl.GetUrl());

  mNativeImageVisual = Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  if(!mNativeImageVisual)
  {
    DALI_LOG_ERROR("VideoViewImpl: failed to create NativeImage visual\n");
    mNativeImagePtr.Reset();
    return;
  }

  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.RegisterVisual(NATIVEIMAGE_VISUAL_INDEX, mNativeImageVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
  viewData.EnableCornerPropertiesOverridden(mNativeImageVisual, true);
}

void VideoViewImpl::RemoveNativeImageVisual()
{
  if(!mNativeImageVisual)
  {
    return;
  }

  auto& viewData = Internal::ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(NATIVEIMAGE_VISUAL_INDEX);
  mNativeImageVisual.Reset();
  mNativeImagePtr.Reset();
}

void VideoViewImpl::ConnectGeometrySync()
{
  Actor self = Self();

  if(!mPositionNotification)
  {
    mPositionNotification = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
    mSizeNotification     = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
    mScaleNotification    = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));

    mPositionNotification.NotifySignal().Connect(this, &VideoViewImpl::OnGeometryChanged);
    mSizeNotification.NotifySignal().Connect(this, &VideoViewImpl::OnGeometryChanged);
    mScaleNotification.NotifySignal().Connect(this, &VideoViewImpl::OnGeometryChanged);
  }

  if(!mWindow)
  {
    mWindow = Dali::Window::Get(self);
    if(mWindow)
    {
      mWindow.ResizedSignal().Connect(this, &VideoViewImpl::OnWindowResized);
    }
  }
}

void VideoViewImpl::DisconnectGeometrySync()
{
  Actor self = Self();

  if(mPositionNotification)
  {
    self.RemovePropertyNotification(mPositionNotification);
    self.RemovePropertyNotification(mSizeNotification);
    self.RemovePropertyNotification(mScaleNotification);
    mPositionNotification.Reset();
    mSizeNotification.Reset();
    mScaleNotification.Reset();
  }

  if(mWindow)
  {
    mWindow.ResizedSignal().Disconnect(this, &VideoViewImpl::OnWindowResized);
    mWindow.Reset();
  }
}

void VideoViewImpl::OnGeometryChanged(Dali::PropertyNotification /* source */)
{
  UpdateDisplayArea();
}

void VideoViewImpl::OnWindowResized(Dali::Window /* window */, Dali::Window::WindowSize /* size */)
{
  UpdateDisplayArea();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
