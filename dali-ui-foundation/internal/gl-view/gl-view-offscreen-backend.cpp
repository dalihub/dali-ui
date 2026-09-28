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
#include <dali-ui-foundation/internal/gl-view/gl-view-offscreen-backend.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/rendering/texture-devel.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/rendering/shader.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
/// The queue starts at the smallest size it can: the layout has not run yet, and
/// OnArrange() resizes it before anything is drawn.
constexpr uint32_t INITIAL_QUEUE_EXTENT = 1u;

Dali::NativeImageQueue::ColorFormat ToQueueColorFormat(GlViewOffscreenConfig::ColorFormat format)
{
  // Mapped straight through: NativeImageQueue::ColorFormat follows pixel byte order,
  // and so does the format the application asked for.
  return (format == GlViewOffscreenConfig::ColorFormat::RGBA8888)
           ? Dali::NativeImageQueue::ColorFormat::RGBA8888
           : Dali::NativeImageQueue::ColorFormat::RGBX8888;
}

int ToGlesVersion(GlViewOffscreenConfig::GraphicsApiVersion version)
{
  return (version == GlViewOffscreenConfig::GraphicsApiVersion::GLES_VERSION_2_0) ? 20 : 30;
}
} // namespace

GlViewOffscreenBackend::GlViewOffscreenBackend(const GlViewOffscreenConfig& config)
: mConfig(config)
{
}

GlViewOffscreenBackend::~GlViewOffscreenBackend()
{
  // Nothing may outlive this object that could call back into it.
  JoinThread();
}

void GlViewOffscreenBackend::Initialize(GlViewImpl& owner)
{
  mOwner = &owner;

  mNativeImageQueue = Dali::NativeImageQueue::New(INITIAL_QUEUE_EXTENT,
                                                  INITIAL_QUEUE_EXTENT,
                                                  ToQueueColorFormat(mConfig.GetColorFormat()));
  if(!mNativeImageQueue)
  {
    DALI_LOG_ERROR("GlView: this platform provides no native image queue, offscreen rendering is unavailable\n");
    return;
  }

  // Asked before anything is built on top of it: the surface is what reports whether the
  // platform can do this at all, and if it cannot then there is nothing to composite
  // either - a renderer sampling a queue that is never filled would only draw a hole.
  mRenderThread = MakeUnique<GlViewRenderThread>(owner, mNativeImageQueue);
  if(!mRenderThread->SetGraphicsConfig(mConfig.IsDepthEnabled(),
                                       mConfig.IsStencilEnabled(),
                                       static_cast<int>(mConfig.GetSampleCount()),
                                       ToGlesVersion(mConfig.GetGraphicsApiVersion())))
  {
    DALI_LOG_ERROR("GlView: this platform cannot provide the requested offscreen surface\n");
    mRenderThread.Reset();
    mNativeImageQueue.Reset();
    return;
  }

  mRenderer = CreateCompositeRenderer();
  if(!mRenderer)
  {
    mRenderThread.Reset();
    mNativeImageQueue.Reset();
    return;
  }
  owner.Self().AddRenderer(mRenderer);
}

Dali::Renderer GlViewOffscreenBackend::CreateCompositeRenderer()
{
  Dali::Texture nativeTexture = Dali::Texture::New(*mNativeImageQueue);
  if(!nativeTexture)
  {
    DALI_LOG_ERROR("GlView: could not create a texture for the native image queue\n");
    return Dali::Renderer();
  }

  std::string fragmentShader = std::string(BASIC_FRAGMENT_SOURCE);
  static_cast<void>(DevelTexture::ApplyNativeFragmentShader(nativeTexture, fragmentShader, 1));

  Dali::Renderer renderer = Internal::CreateRenderer(BASIC_VERTEX_SOURCE,
                                                     fragmentShader,
                                                     static_cast<Dali::Shader::Hint::Value>(Dali::Shader::Hint::FILE_CACHE_SUPPORT | Dali::Shader::Hint::INTERNAL),
                                                     "GL_VIEW",
                                                     Dali::Uint16Pair(1u, 1u));
  SetRendererTexture(renderer, nativeTexture);

  return renderer;
}

void GlViewOffscreenBackend::SetRenderingMode(GlView::RenderingMode mode)
{
  const bool onDemand = (mode == GlView::RenderingMode::ON_DEMAND);

  // Both sides have to follow: the thread decides when a frame is produced, and the
  // renderer decides whether DALi looks for a new one.
  if(mRenderer)
  {
    mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR,
                          onDemand ? DevelRenderer::Rendering::IF_REQUIRED : DevelRenderer::Rendering::CONTINUOUSLY);
  }

  if(mRenderThread)
  {
    mRenderThread->SetOnDemandRenderMode(onDemand);
  }
}

void GlViewOffscreenBackend::RenderOnce()
{
  // Both are needed: this thread produces the frame, and a DALi frame is what composites
  // it onto the screen.
  if(mRenderThread)
  {
    mRenderThread->RenderOnce();
  }

  if(DALI_LIKELY(mOwner))
  {
    mOwner->KeepRendering();
  }
}

void GlViewOffscreenBackend::BindTextureResources(Dali::Vector<Dali::Texture> /*textures*/)
{
  DALI_LOG_ERROR("GlView: BindTextureResources() needs a context that shares resources with DALi's, which offscreen rendering does not have\n");
}

void GlViewOffscreenBackend::OnArrange(const Size& size)
{
  if(!mNativeImageQueue || !mRenderThread || size == mSize)
  {
    return;
  }

  if(size.width < 1.0f || size.height < 1.0f)
  {
    return;
  }

  mSize = size;

  // Taken so the thread is not rendering into the buffers being replaced.
  mRenderThread->AcquireSurface();
  mNativeImageQueue->SetSize(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height));
  mRenderThread->SetSurfaceSize(size);
  mRenderThread->ReleaseSurface();

  // This may be what the thread was waiting for.
  UpdateThreadState();
}

void GlViewOffscreenBackend::OnVisibilityChanged(bool visible)
{
  mVisible = visible;

  UpdateThreadState();
}

void GlViewOffscreenBackend::UpdateThreadState()
{
  if(!mRenderThread)
  {
    return;
  }

  // Waiting on the layout as well as on visibility. The queue is created at the smallest
  // size it can be, because the texture has to exist before anything has been measured,
  // and a frame produced at that size is both wasted work and briefly visible as a single
  // stretched pixel.
  if(mVisible && mSize.width >= 1.0f && mSize.height >= 1.0f)
  {
    // Starts the thread the first time, so nothing runs for a view never shown.
    mRenderThread->Resume();
    mThreadRunning = true;
  }
  else
  {
    mRenderThread->Pause();
  }
}

bool GlViewOffscreenBackend::StartTerminate()
{
  if(!mRenderThread || !mThreadRunning)
  {
    // The thread never ran, so it cannot report back and there is nothing for the
    // application's terminate callback to release.
    return false;
  }

  mRenderThread->Stop();
  return true;
}

void GlViewOffscreenBackend::AbandonQuietly()
{
  if(mRenderThread)
  {
    mRenderThread->StopQuietly();
    JoinThread();
  }
}

void GlViewOffscreenBackend::OnTerminateCompleted()
{
  // Joined before anything the thread was using is released. The thread has already
  // fired the trigger that led here, so this returns as soon as it unwinds.
  JoinThread();

  mRenderer.Reset();
  mNativeImageQueue.Reset();
}

void GlViewOffscreenBackend::JoinThread()
{
  if(mRenderThread && mThreadRunning)
  {
    mRenderThread->Join();
    mThreadRunning = false;
  }
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
