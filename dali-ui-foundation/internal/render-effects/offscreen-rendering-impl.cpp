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
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/integration-api/texture-integ.h>

#include <locale>

namespace IntegrationView = Dali::Ui::Integration::View;

using Dali::Integration::ToDaliString;

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
  GetImpl(ownerView).RequestRenderTaskReorder();
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
  }

  DestroyFrameBuffer();
  DestroyRenderTask();
}

void OffScreenRenderingImpl::OnRefresh()
{
  DestroyFrameBuffer();

  Ui::View view = GetOwnerView();
  if(DALI_LIKELY(view))
  {
    IntegrationView::AllowToAddActorToChildBegin(view);
  }
  mRenderTask.SetBuiltinCameraActor(Dali::RenderTask::BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR, GetTargetSize(), Property::Map().Add(Dali::Actor::Property::NAME, "OffScreenAutoCamera").Add(Dali::CameraActor::Property::INVERT_Y_AXIS, true));
  if(DALI_LIKELY(view))
  {
    IntegrationView::AllowToAddActorToChildEnd(view);
  }

  CreateFrameBuffer();
  SetRendererTexture(GetTargetRenderer(), mFrameBuffer);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
}

void OffScreenRenderingImpl::CreateFrameBuffer()
{
  const Size size = GetTargetSize();

  mFrameBuffer    = FrameBuffer::New(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), FrameBuffer::Attachment::AUTO);
  Texture texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height));

#if defined(GPU_MEMORY_PROFILE_ENABLED)
  {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << "OffScreenRendering type:" << mType;

    Dali::Integration::TextureUploadWithContent(texture, Dali::PixelData(), ToDaliString(oss.str()), Dali::Integration::TextureContextTypeHint::FBO_ATTACHED_COLOR_TEXTURE, true);
  }
#endif

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

  IntegrationView::AllowToAddActorToChildBegin(view);

  mRenderTask = taskList.CreateTask();
  mRenderTask.SetSourceActor(view);
  mRenderTask.SetBuiltinCameraActor(Dali::RenderTask::BuiltinCameraType::ATTACHED_TO_SOURCE_ACTOR, GetTargetSize(), Property::Map().Add(Dali::Actor::Property::NAME, "OffScreenAutoCamera").Add(Dali::CameraActor::Property::INVERT_Y_AXIS, true));
  mRenderTask.SetExclusive(true);
  mRenderTask.SetInputEnabled(true);
  mRenderTask.SetFrameBuffer(mFrameBuffer);
  mRenderTask.SetClearEnabled(true);
  mRenderTask.SetClearColor(Color::TRANSPARENT);
  mRenderTask.SetRenderPassTag(GetRenderPassTag());
  mRenderTask.FinishedSignal().Connect(this, &OffScreenRenderingImpl::OnRenderFinished);

  IntegrationView::AllowToAddActorToChildEnd(view);
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

void OffScreenRenderingImpl::OnRenderFinished(Dali::RenderTask task)
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
