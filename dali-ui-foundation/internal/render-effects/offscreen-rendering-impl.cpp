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
#include <dali-ui-foundation/internal/render-effects/offscreen-rendering-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/view-renderers.h>

// EXTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/integration-api/view-integration.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
OffScreenRenderingImpl::OffScreenRenderingImpl(Ui::View::OffScreenRenderingType type)
: mType(type)
{
  Initialize();
}

void OffScreenRenderingImpl::SetType(Ui::View::OffScreenRenderingType type)
{
  mType = type;

  if(mRenderTask)
  {
    if(mType == Ui::View::OffScreenRenderingType::REFRESH_ALWAYS)
    {
      mRenderTask.SetRefreshRate(RenderTask::REFRESH_ALWAYS);
    }
    else if(mType == Ui::View::OffScreenRenderingType::REFRESH_ONCE)
    {
      mRenderTask.SetRefreshRate(RenderTask::REFRESH_ONCE);
    }
  }
}

OffScreenRenderable::Type OffScreenRenderingImpl::GetOffScreenRenderableType() const
{
  return OffScreenRenderable::Type::FORWARD;
}

void OffScreenRenderingImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(isForward)
  {
    if(mRenderTask)
    {
      tasks.PushBack(mRenderTask);
    }
  }
}

Dali::Texture OffScreenRenderingImpl::GetTexture() const
{
  return mTexture;
}

void OffScreenRenderingImpl::OnInitialize()
{
}

void OffScreenRenderingImpl::OnActivate()
{
  if(mType == Ui::View::OffScreenRenderingType::NONE)
  {
    return;
  }
  Ui::View ownerView = GetOwnerView();
  DALI_ASSERT_ALWAYS(ownerView && "Set the owner of RenderEffect before you activate.");

  if(!mCamera)
  {
    mCamera = CameraActor::New();
    mCamera.SetInvertYAxis(true);
    mCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
    mCamera.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
    mCamera.SetType(Dali::Camera::FREE_LOOK);
  }
  mCamera.SetPerspectiveProjection(GetTargetSize());
  IntegrationView::AddActorChild(ownerView, mCamera);

  CreateFrameBuffer();
  CreateRenderTask();
  SetType(mType);

  Renderer renderer = GetTargetRenderer();
  SetRendererTexture(renderer, mFrameBuffer);

  ownerView.AddCacheRenderer(renderer);
  ownerView.GetImplementation().RegisterOffScreenRenderableType(GetOffScreenRenderableType());
  mRenderTask.SetScreenToFrameBufferMappingActor(ownerView);

  // Reorder render task
  // TODO : Can we remove this GetImplementation?
  Integration::GetImpl(ownerView).RequestRenderTaskReorder();
}

void OffScreenRenderingImpl::OnDeactivate()
{
  Renderer renderer = GetTargetRenderer();
  SetRendererTexture(renderer, Dali::Texture());

  Ui::View view = GetOwnerView();
  if(DALI_LIKELY(view))
  {
    view.RemoveCacheRenderer(renderer);
    view.GetImplementation().UnregisterOffScreenRenderableType(GetOffScreenRenderableType());

    mCamera.Unparent();
  }

  DestroyFrameBuffer();
  DestroyRenderTask();
}

void OffScreenRenderingImpl::OnRefresh()
{
  DestroyFrameBuffer();

  mCamera.SetPerspectiveProjection(GetTargetSize());

  CreateFrameBuffer();
  SetRendererTexture(GetTargetRenderer(), mFrameBuffer);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
}

void OffScreenRenderingImpl::CreateFrameBuffer()
{
  const Size size = GetTargetSize();

  mFrameBuffer    = FrameBuffer::New(size.width, size.height, FrameBuffer::Attachment::AUTO);
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, size.width, size.height);
  mFrameBuffer.AttachColorTexture(texture);
}

void OffScreenRenderingImpl::DestroyFrameBuffer()
{
  mFrameBuffer.Reset();
}

void OffScreenRenderingImpl::CreateRenderTask()
{
  Ui::View                       view        = GetOwnerView();
  Dali::Integration::SceneHolder sceneHolder = GetSceneHolder();
  RenderTaskList                 taskList    = sceneHolder.GetRenderTaskList();

  mRenderTask = taskList.CreateTask();
  mRenderTask.SetSourceActor(view);
  mRenderTask.SetCameraActor(mCamera);
  mRenderTask.SetExclusive(true);
  mRenderTask.SetInputEnabled(true);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
  mRenderTask.SetClearEnabled(true);
  mRenderTask.SetClearColor(Color::TRANSPARENT);
  mRenderTask.SetRenderPassTag(GetRenderPassTag());
  mRenderTask.FinishedSignal().Connect(this, &OffScreenRenderingImpl::OnRenderFinished);
}

void OffScreenRenderingImpl::DestroyRenderTask()
{
  auto sceneHolder = GetSceneHolder();
  if(DALI_LIKELY(sceneHolder))
  {
    RenderTaskList taskList = sceneHolder.GetRenderTaskList();
    taskList.RemoveTask(mRenderTask);
  }

  mRenderTask.Reset();
}

void OffScreenRenderingImpl::OnRenderFinished(Dali::RenderTask& task)
{
  if(DALI_LIKELY(mRenderTask == task))
  {
    Ui::View view = GetOwnerView();
    if(view)
    {
      mTexture = mFrameBuffer.GetColorTexture();

      view.OffScreenRenderingFinishedSignal().Emit(view);

      // Reset texture handle after signal completed.
      mTexture.Reset();
    }
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
