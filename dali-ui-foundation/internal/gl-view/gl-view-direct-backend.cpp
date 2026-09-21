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
#include <dali-ui-foundation/internal/gl-view/gl-view-direct-backend.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
RenderCallback::ExecutionMode ToExecutionMode(GlView::BackendMode backendMode)
{
  return (backendMode == GlView::BackendMode::UNSAFE_DIRECT_RENDERING)
           ? RenderCallback::ExecutionMode::UNSAFE
           : RenderCallback::ExecutionMode::ISOLATED;
}
} // namespace

GlViewDirectBackend::GlViewDirectBackend(GlView::BackendMode backendMode)
{
  mRenderCallback = RenderCallback::New(this, &GlViewDirectBackend::OnRenderCallback, ToExecutionMode(backendMode));
}

GlViewDirectBackend::~GlViewDirectBackend()
{
  // The render side holds a reference of its own, so the callback outlives this object -
  // and DALi delivers the terminate invocation exactly once, possibly while the graphics
  // resources are torn down. Detaching here is what keeps it from reaching a destroyed
  // backend, and releases the bound textures on this thread.
  mRenderCallback->Invalidate();
}

void GlViewDirectBackend::Initialize(GlViewImpl& owner)
{
  mOwner = &owner;

  mRenderer = Dali::DevelRenderer::New(mRenderCallback);
  owner.Self().AddRenderer(mRenderer);
}

void GlViewDirectBackend::SetRenderingMode(GlView::RenderingMode mode)
{
  if(mRenderer)
  {
    mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR,
                          (mode == GlView::RenderingMode::ON_DEMAND) ? DevelRenderer::Rendering::IF_REQUIRED : DevelRenderer::Rendering::CONTINUOUSLY);
  }
}

void GlViewDirectBackend::RenderOnce()
{
  // No thread of its own, so the only way to run the callback once is to drive a
  // whole DALi frame.
  if(DALI_LIKELY(mOwner))
  {
    mOwner->KeepRendering();
  }
}

void GlViewDirectBackend::BindTextureResources(Dali::Vector<Dali::Texture> textures)
{
  mRenderCallback->BindTextureResources(std::move(textures));
}

bool GlViewDirectBackend::StartTerminate()
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable() || !mRenderer || !mOwner))
  {
    // Nothing will reach the render thread, so it cannot report back.
    return false;
  }

  // Forces the render callback to run once more so the terminate invocation is delivered.
  // DALi guarantees exactly one, even when the view was never drawn or the native API is
  // no longer usable - it just says so through RenderCallbackInput::isNativeApiUsable.
  DevelRenderer::TerminateRenderCallback(mRenderer);
  mOwner->Self().RemoveRenderer(mRenderer);

  mOwner->KeepRendering();
  return true;
}

void GlViewDirectBackend::AbandonQuietly()
{
  // Nothing to stop: the callback only runs while the renderer is drawn, and the
  // view is on its way out. Marking the invocation done keeps a frame that is
  // already in flight from reaching the application's callbacks.
  mTerminateInvoked = true;
}

void GlViewDirectBackend::OnTerminateCompleted()
{
  // The render thread is done with the callback, so the renderer can be released.
  mRenderer.Reset();
}

// Called from the DALi render thread.
bool GlViewDirectBackend::OnRenderCallback(const Dali::RenderCallbackInput& input)
{
  if(mTerminateInvoked || DALI_UNLIKELY(!mOwner))
  {
    return true;
  }

  if(input.isTerminated)
  {
    mTerminateInvoked = true;

    // Only the application's own GL resources need releasing, and it has none unless the
    // init callback ran. Skipped as well when there is no context left for the callback to
    // issue GL calls against.
    if(mInitInvoked && input.isNativeApiUsable)
    {
      mOwner->InvokeTerminateCallback();
    }

    // Last statement touching this object. The event thread starts releasing the view as
    // soon as the trigger lands, and it can get there before this returns.
    mOwner->NotifyTerminateCompleted();
    return true;
  }

  mOwner->GetRenderInfo().GetImplementation().input = &input;

  if(!mInitInvoked)
  {
    mInitInvoked = true;
    mOwner->InvokeInitCallback();
  }

  // The return value says whether new content was produced. Direct rendering backends
  // draw straight into the window surface, so there is no present step to gate and the
  // value is intentionally discarded here.
  static_cast<void>(mOwner->InvokeRenderFrameCallback());

  return true;
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
