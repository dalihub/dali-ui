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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/window.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-direct-backend.h>
#include <dali-ui-foundation/internal/gl-view/gl-view-offscreen-backend.h>

namespace DALI_NAMESPACE
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

UniquePtr<GlViewBackend> CreateBackend(GlView::BackendMode backendMode, const GlViewOffscreenConfig& offscreenConfig)
{
  if(backendMode == GlView::BackendMode::OFFSCREEN_RENDERING)
  {
    return MakeUnique<GlViewOffscreenBackend>(offscreenConfig);
  }
  return MakeUnique<GlViewDirectBackend>(backendMode);
}
} // namespace

// ---------------------------------------------------------------------------
// Construction & destruction
// ---------------------------------------------------------------------------

GlViewImpl::GlViewImpl(GlView::BackendMode backendMode, const GlViewOffscreenConfig& offscreenConfig)
: ViewImpl(),
  mBackendMode(backendMode),
  mBackend(CreateBackend(backendMode, offscreenConfig))
{
}

GlViewImpl::~GlViewImpl()
{
  // Reaching here means either the terminate sequence completed - it holds a reference to
  // this object until it does - or the application let the last handle go without ever
  // calling Terminate(). Nothing registered by the application is invoked in the latter
  // case, by design: it has given the view up, so its callbacks may no longer be safe to
  // call into. Whatever GL resources they created are left to the graphics context.
  if(!mTerminateRequested && mBackend)
  {
    mBackend->AbandonQuietly();
  }

  mEventTrigger.reset();
}

GlViewImplPtr GlViewImpl::New(GlView::BackendMode backendMode, const GlViewOffscreenConfig& offscreenConfig)
{
  return new GlViewImpl(backendMode, offscreenConfig);
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
  mBackend->BindTextureResources(std::move(textures));
}

void GlViewImpl::SetRenderingMode(GlView::RenderingMode mode)
{
  mRenderingMode = mode;
  mBackend->SetRenderingMode(mode);
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

  mBackend->RenderOnce();
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

  // Created before the terminate is requested: the rendering thread must never reach the
  // terminate invocation and find no trigger to fire.
  mEventTrigger = std::make_unique<EventThreadCallback>(MakeCallback(this, &GlViewImpl::OnTerminateCompleted));

  // The application can be shut down before the rendering side reports back, and nothing
  // reaches the event thread from that point on. This is where the sequence is completed
  // when that happens.
  Dali::LifecycleController::Get().TerminateSignal().Connect(this, &GlViewImpl::OnApplicationTerminate);

  // Keeps this object alive until the rendering thread has reported back, so releasing the
  // last handle right after this call cannot pull the callback out from under it.
  mSelfReference = Self();

  if(!mBackend->StartTerminate())
  {
    // Nothing reached the rendering thread, so the backend was never told to stop and it
    // cannot report back. Stop it here and complete the sequence rather than leaving the
    // caller waiting for a notification that cannot arrive.
    mBackend->AbandonQuietly();
    OnTerminateCompleted();
  }
}

// ---------------------------------------------------------------------------
// For the backends
// ---------------------------------------------------------------------------

Ui::GlViewRenderInfo& GlViewImpl::GetRenderInfo()
{
  return mRenderInfo;
}

void GlViewImpl::InvokeInitCallback()
{
  if(mInitCallback)
  {
    mInitCallback.Invoke(mRenderInfo);
  }
}

bool GlViewImpl::InvokeRenderFrameCallback()
{
  return mRenderFrameCallback ? mRenderFrameCallback.Invoke(mRenderInfo) : false;
}

void GlViewImpl::InvokeTerminateCallback()
{
  if(mTerminateCallback)
  {
    mTerminateCallback.Invoke();
  }
}

void GlViewImpl::NotifyTerminateCompleted()
{
  mEventTrigger->Trigger();
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

// ---------------------------------------------------------------------------
// From ViewImpl
// ---------------------------------------------------------------------------

void GlViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  mBackend->Initialize(*this);

  // Apply a rendering mode that may have been set before the backend was attached.
  SetRenderingMode(mRenderingMode);

  Dali::DevelActor::OnSceneVisibilityChangedSignal(Self()).Connect(this, &GlViewImpl::OnSceneVisibilityChanged);
}

LayoutRect GlViewImpl::OnArrange(const LayoutRect& bounds)
{
  LayoutRect result = ViewImpl::OnArrange(bounds);

  mBackend->OnArrange(Size(bounds.width, bounds.height));

  return result;
}

void GlViewImpl::OnSceneConnection(int depth)
{
  ViewImpl::OnSceneConnection(depth);

  mPlacementWindow = Dali::Window::Get(Self());
  if(mPlacementWindow)
  {
    // The view can be on screen and yet not visible, because its window is hidden.
    mPlacementWindow.VisibilityChangedSignal().Connect(this, &GlViewImpl::OnWindowVisibilityChanged);
  }

  UpdateVisibility();
}

void GlViewImpl::OnSceneDisconnection()
{
  if(mPlacementWindow)
  {
    mPlacementWindow.VisibilityChangedSignal().Disconnect(this, &GlViewImpl::OnWindowVisibilityChanged);
    mPlacementWindow.Reset();
  }

  UpdateVisibility();

  ViewImpl::OnSceneDisconnection();
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

// The application is shutting down: its main loop is done, so the report the rendering
// side was going to send can no longer land. Complete the sequence here instead, so the
// application still hears back.
void GlViewImpl::OnApplicationTerminate()
{
  if(mTerminateCompleted)
  {
    return;
  }

  // Whatever can still be stopped is stopped, and whatever cannot is at least kept away
  // from the application's callbacks. An invocation the rendering side still owes is safe
  // to leave: the backend detaches its callback on the way out.
  mBackend->AbandonQuietly();

  OnTerminateCompleted();
}

// Called from the event thread once the rendering thread has reported the terminate
// invocation back, from Terminate() when it cannot reach that thread, or from the
// application shutdown that leaves it unable to report.
void GlViewImpl::OnTerminateCompleted()
{
  if(mTerminateCompleted)
  {
    return;
  }
  mTerminateCompleted = true;

  Dali::LifecycleController::Get().TerminateSignal().Disconnect(this, &GlViewImpl::OnApplicationTerminate);

  mBackend->OnTerminateCompleted();

  // Nothing invokes these again, and holding them keeps whatever they point at alive.
  mInitCallback        = {};
  mRenderFrameCallback = {};
  mTerminateCallback   = {};

  // mEventTrigger is not released here - this may be running from inside its own callback.

  if(mTerminateCompletedCallback)
  {
    Dali::Callback<void()> completed = std::move(mTerminateCompletedCallback);
    completed.Invoke();
  }

  // Last statement: this may well be the reference that was keeping the view alive.
  mSelfReference.Reset();
}

void GlViewImpl::UpdateVisibility()
{
  // Both have to hold: a view can be visible in the actor tree while the window it
  // is on is hidden, and nothing is produced for a hidden window.
  const bool visible = Dali::DevelActor::IsOnSceneVisible(Self()) &&
                       mPlacementWindow && mPlacementWindow.IsVisible();

  mBackend->OnVisibilityChanged(visible);
}

void GlViewImpl::OnSceneVisibilityChanged(Dali::Actor /*actor*/, bool /*visible*/)
{
  UpdateVisibility();
}

void GlViewImpl::OnWindowVisibilityChanged(Dali::Window /*window*/, bool /*visible*/)
{
  UpdateVisibility();
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
