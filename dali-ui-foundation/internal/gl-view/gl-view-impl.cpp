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
#include <dali-ui-foundation/internal/gl-view/gl-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/window.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
/**
 * GlView cannot be built from the type registry because the backend has to be chosen at
 * construction, so this returns an empty handle. The registration itself is still
 * required: it is what chains this type's TypeInfo to Ui::View, and therefore what makes
 * View's registered animatable properties - "viewEffectiveScale" among them, which the
 * layout pass reads - resolvable on a GlView.
 */
BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::GlView, Ui::Internal::GlViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

RenderCallback::ExecutionMode ToExecutionMode(GlView::BackendMode backendMode)
{
  return (backendMode == GlView::BackendMode::UNSAFE_DIRECT_RENDERING)
           ? RenderCallback::ExecutionMode::UNSAFE
           : RenderCallback::ExecutionMode::ISOLATED;
}
} // namespace

// ---------------------------------------------------------------------------
// Construction & destruction
// ---------------------------------------------------------------------------

GlViewImpl::GlViewImpl(GlView::BackendMode backendMode)
: ViewImpl(),
  mBackendMode(backendMode)
{
  mRenderCallback = RenderCallback::New(this, &GlViewImpl::OnRenderCallback, ToExecutionMode(backendMode));
}

GlViewImpl::~GlViewImpl()
{
  // Reaching here means either the terminate sequence completed - it holds a reference to
  // this object until it does - or the application let the last handle go without ever
  // calling Terminate(). Nothing registered by the application is invoked in the latter
  // case, by design: it has given the view up, so its callbacks may no longer be safe to
  // call into. Whatever GL resources they created are left to the graphics context.
  //
  // The render thread may still hold the raw RenderCallback pointer for an in-flight
  // frame at this point. That is only safe because no further frames are produced for a
  // renderer that has been released, which is an assumption rather than something DALi
  // guarantees.
  // TODO : Drop this assumption once RenderCallback ownership moves to the render side.
  mEventTrigger.reset();
}

GlViewImplPtr GlViewImpl::New(GlView::BackendMode backendMode)
{
  return new GlViewImpl(backendMode);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void GlViewImpl::RegisterGlCallbacks(Dali::Callback<void(const GlViewRenderInfo&)> initCallback,
                                     Dali::Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                                     Dali::Callback<void()>                        terminateCallback)
{
  if(mTerminateRequested)
  {
    DALI_LOG_ERROR("GlView: cannot register callbacks after Terminate()\n");
    return;
  }

  mInitCallback        = std::move(initCallback);
  mRenderFrameCallback = std::move(renderFrameCallback);
  mTerminateCallback   = std::move(terminateCallback);
}

void GlViewImpl::BindTextureResources(Dali::Vector<Dali::Texture> textures)
{
  mRenderCallback->BindTextureResources(std::move(textures));
}

void GlViewImpl::SetRenderingMode(GlView::RenderingMode mode)
{
  mRenderingMode = mode;

  if(mRenderer)
  {
    mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR,
                          (mode == GlView::RenderingMode::ON_DEMAND) ? DevelRenderer::Rendering::IF_REQUIRED : DevelRenderer::Rendering::CONTINUOUSLY);
  }
}

GlView::RenderingMode GlViewImpl::GetRenderingMode() const
{
  return mRenderingMode;
}

void GlViewImpl::RenderOnce()
{
  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable()))
  {
    return;
  }

  // Direct rendering backends have no thread of their own, so the only way to run the
  // callback once is to drive a whole DALi frame.
  KeepRendering();
}

GlView::BackendMode GlViewImpl::GetBackendMode() const
{
  return mBackendMode;
}

void GlViewImpl::Terminate(Dali::Callback<void()> onTerminated)
{
  if(mTerminateRequested)
  {
    return;
  }
  mTerminateRequested = true;

  mTerminateCompletedCallback = std::move(onTerminated);

  if(DALI_UNLIKELY(!Dali::Adaptor::IsAvailable() || !mRenderer))
  {
    // Nothing will reach the render thread, so it cannot report back. Complete right here
    // rather than leaving the caller waiting for a notification that cannot arrive.
    OnTerminateCompleted();
    return;
  }

  // Created before the terminate is requested: the render thread must never reach the
  // terminate invocation and find no trigger to fire.
  mEventTrigger = std::make_unique<EventThreadCallback>(MakeCallback(this, &GlViewImpl::OnTerminateCompleted));

  // Keeps this object alive until the render thread has reported back, so releasing the
  // last handle right after this call cannot pull the callback out from under it.
  mSelfReference = Self();

  // Forces the render callback to run once more so the terminate invocation is delivered.
  // DALi guarantees exactly one, even when the view was never drawn or the native API is
  // no longer usable - it just says so through RenderCallbackInput::isNativeApiUsable.
  DevelRenderer::TerminateRenderCallback(mRenderer, true);
  Self().RemoveRenderer(mRenderer);

  KeepRendering();
}

// ---------------------------------------------------------------------------
// From ViewImpl
// ---------------------------------------------------------------------------

void GlViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  mRenderer = Dali::DevelRenderer::New(*mRenderCallback);
  Self().AddRenderer(mRenderer);

  // Apply a rendering mode that may have been set before the renderer existed.
  SetRenderingMode(mRenderingMode);
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

// Called from the DALi render thread.
bool GlViewImpl::OnRenderCallback(const Dali::RenderCallbackInput& input)
{
  if(mTerminateInvoked)
  {
    return true;
  }

  if(input.isTerminated)
  {
    mTerminateInvoked = true;

    // Only the application's own GL resources need releasing, and it has none unless the
    // init callback ran. Skipped as well when there is no context left for the callback to
    // issue GL calls against.
    if(mInitInvoked && input.isNativeApiUsable && mTerminateCallback)
    {
      mTerminateCallback.Invoke();
    }

    // Last statement touching this object. The event thread starts releasing the view as
    // soon as the trigger lands, and it can get there before this returns.
    mEventTrigger->Trigger();
    return true;
  }

  mRenderInfo.GetImplementation().input = &input;

  if(!mInitInvoked)
  {
    mInitInvoked = true;
    if(mInitCallback)
    {
      mInitCallback.Invoke(mRenderInfo);
    }
  }

  if(mRenderFrameCallback)
  {
    // The return value says whether new content was produced. Direct rendering backends
    // draw straight into the window surface, so there is no present step to gate and the
    // value is intentionally discarded here.
    static_cast<void>(mRenderFrameCallback.Invoke(mRenderInfo));
  }

  return true;
}

// Called from the event thread once the render thread has reported the terminate
// invocation back, or directly from Terminate() when it cannot reach the render thread.
void GlViewImpl::OnTerminateCompleted()
{
  // The render thread is done with the callback, so the renderer can be released.
  mRenderer.Reset();

  // Nothing invokes these again, and holding them keeps whatever they point at alive.
  // mRenderCallback is deliberately kept until this object is destroyed: the graphics
  // backend may still be unwinding the invocation that led here.
  mInitCallback        = {};
  mRenderFrameCallback = {};
  mTerminateCallback   = {};

  // mEventTrigger is not released here - this is running from inside its own callback.

  if(mTerminateCompletedCallback)
  {
    Dali::Callback<void()> completed = std::move(mTerminateCompletedCallback);
    completed.Invoke();
  }

  // Last statement: this may well be the reference that was keeping the view alive.
  mSelfReference.Reset();
}

void GlViewImpl::KeepRendering()
{
  Dali::Window window = Dali::Window::Get(Self());
  if(DALI_LIKELY(window))
  {
    // Queues a message and flushes it, so pending changes reach the update thread and a
    // frame is produced even when nothing else would have driven one.
    window.KeepRendering(0.0f);
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
