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
#include "canvas-view-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/property-registration-helper.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view-properties.h>
#include <dali-ui-foundation/public-api/visuals/image-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{
BaseHandle Create()
{
  return BaseHandle();
}

// clang-format off
#define CANVAS_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, CanvasViewPropertyIndex, Ui::Internal, CanvasViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(CanvasViewImpl, ViewImpl, Create)

CANVAS_VIEW_PROPERTY_REGISTRATION("viewBox",                      VECTOR2, VIEW_BOX)
CANVAS_VIEW_PROPERTY_REGISTRATION("synchronousLoading",           BOOLEAN, SYNCHRONOUS_LOADING)
CANVAS_VIEW_PROPERTY_REGISTRATION("rasterizationRequestManually", BOOLEAN, RASTERIZATION_REQUEST_MANUALLY)
CANVAS_VIEW_PROPERTY_REGISTRATION("canvasContentVisual",          MAP,     CANVAS_CONTENT_VISUAL)

DALI_TYPE_REGISTRATION_END()
#undef CANVAS_VIEW_PROPERTY_REGISTRATION
// clang-format on
} // namespace

CanvasViewImpl::CanvasViewImpl(const Vector2& viewBox)
: ViewImpl(),
  mCanvasRenderer(CanvasRenderer::New(viewBox)),
  mTexture(),
  mSize(viewBox),
  mRasterizingTask(),
  mContentVisual(),
  mIsSynchronous(true),
  mManualRasterization(false),
  mProcessorRegistered(false),
  mLastCommitRasterized(false)
{
  DALI_LOG_DEBUG_INFO("[%p] Created\n", this);
  if(DALI_UNLIKELY(!mCanvasRenderer))
  {
    DALI_LOG_ERROR("CanvasViewImpl: CanvasRenderer is not supported on this platform.\n");
  }
}

CanvasViewImpl::~CanvasViewImpl()
{
  if(Adaptor::IsAvailable() && mProcessorRegistered)
  {
    if(mRasterizingTask)
    {
      AsyncTaskManager::Get().RemoveTask(mRasterizingTask);
      mRasterizingTask.Reset();
    }
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
  }
}

CanvasViewImplPtr CanvasViewImpl::New(const Vector2& viewBox)
{
  return new CanvasViewImpl(viewBox);
}

// ---------------------------------------------------------------------------
// Property callbacks
// ---------------------------------------------------------------------------

void CanvasViewImpl::SetProperty(Dali::BaseObject* object, Dali::Property::Index index, const Dali::Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    CanvasViewImpl& impl = static_cast<CanvasViewImpl&>(GetImpl(view));
    switch(index)
    {
      case Property::VIEW_BOX:
      {
        Vector2 viewBox;
        if(value.Get(viewBox))
        {
          impl.SetViewBox(viewBox);
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
      case Property::RASTERIZATION_REQUEST_MANUALLY:
      {
        bool manually;
        if(value.Get(manually))
        {
          impl.SetRasterizationRequestManually(manually);
        }
        break;
      }
    }
  }
}

Dali::Property::Value CanvasViewImpl::GetProperty(Dali::BaseObject* object, Dali::Property::Index index)
{
  Dali::Property::Value value;
  Ui::View              view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    CanvasViewImpl& impl = static_cast<CanvasViewImpl&>(GetImpl(view));
    switch(index)
    {
      case Property::VIEW_BOX:
        value = impl.GetViewBox();
        break;
      case Property::SYNCHRONOUS_LOADING:
        value = impl.IsSynchronousLoading();
        break;
      case Property::RASTERIZATION_REQUEST_MANUALLY:
        value = impl.IsRasterizationRequestManually();
        break;
    }
  }
  return value;
}

// ---------------------------------------------------------------------------
// ViewImpl overrides
// ---------------------------------------------------------------------------

void CanvasViewImpl::OnInitialize()
{
  // Call base class initialization
  ViewImpl::OnInitialize();

  // Trigger an initial rasterize so the view shows content as soon as it has a size.
  RequestRasterization();
}

MeasuredSize CanvasViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
}

MeasuredSize CanvasViewImpl::OnArrange(const LayoutRect& bounds)
{
  MeasuredSize result = ViewImpl::OnArrange(bounds);

  const Vector2 newSize(bounds.width, bounds.height);
  if(DALI_LIKELY(mCanvasRenderer) && newSize != mSize && newSize.width > 0.f && newSize.height > 0.f)
  {
    mCanvasRenderer.SetSize(newSize);
    mSize = newSize;
    // Mark as needing re-rasterization; defer to the next Process() cycle
    // so we never touch visuals during the layout pass.
    mLastCommitRasterized = false;
    ScheduleRasterization();
  }
  return result;
}

void CanvasViewImpl::OnSceneConnection(int depth)
{
  Dali::Window window = Window::Get(Self());
  if(DALI_LIKELY(window))
  {
    mPlacementWindow = window;
  }
  ViewImpl::OnSceneConnection(depth);
}

void CanvasViewImpl::OnSceneDisconnection()
{
  mPlacementWindow.Reset();
  ViewImpl::OnSceneDisconnection();
}

// ---------------------------------------------------------------------------
// Processor
// ---------------------------------------------------------------------------

void CanvasViewImpl::Process(bool /*postProcessor*/)
{
  mProcessorRegistered = false;

  bool rasterizeRequired = false;

  if(DALI_LIKELY(mCanvasRenderer) && mSize.width > 0.f && mSize.height > 0.f)
  {
    const bool forcibleRasterization = (mIsSynchronous && !mLastCommitRasterized);
    const bool canvasChanged         = mCanvasRenderer.IsCanvasChanged();

    rasterizeRequired = forcibleRasterization || canvasChanged;
    if(rasterizeRequired)
    {
      AddRasterizationTask(forcibleRasterization);
    }
  }

  const bool syncFailed = (rasterizeRequired && mIsSynchronous && !mLastCommitRasterized);

  // Keep polling unless in manual mode (and no sync retry is pending).
  if(DALI_LIKELY(mCanvasRenderer) && (!mManualRasterization || syncFailed))
  {
    // Use ScheduleRasterization() directly (not RequestRasterization()) so that the
    // internal polling loop does NOT reset mLastCommitRasterized — that flag is only
    // cleared by the public RequestRasterization() API to force a rasterize on-demand.
    ScheduleRasterization();

    if(syncFailed && Adaptor::IsAvailable())
    {
      Adaptor::Get().RequestProcessEventsOnIdle();
    }
  }
}

// ---------------------------------------------------------------------------
// Internal rasterization helpers
// ---------------------------------------------------------------------------

void CanvasViewImpl::ScheduleRasterization()
{
  if(DALI_LIKELY(mCanvasRenderer) && !mProcessorRegistered && Adaptor::IsAvailable())
  {
    mProcessorRegistered = true;
    Adaptor::Get().RegisterProcessorOnce(*this, true);
  }
}

void CanvasViewImpl::AddRasterizationTask(bool forceProcess)
{
  if(DALI_UNLIKELY(!mCanvasRenderer))
  {
    return;
  }

  if(!mCanvasRenderer.Commit() && !forceProcess)
  {
    return;
  }

  mLastCommitRasterized = false;

  if(mIsSynchronous)
  {
    CanvasViewRasterizingTaskPtr task = new CanvasViewRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasViewImpl::ApplyRasterizedImage));
    task->Process();
    ApplyRasterizedImage(task);
  }
  else
  {
    if(mRasterizingTask)
    {
      AsyncTaskManager::Get().RemoveTask(mRasterizingTask);
      mRasterizingTask.Reset();
    }
    mRasterizingTask = new CanvasViewRasterizingTask(mCanvasRenderer, MakeCallback(this, &CanvasViewImpl::ApplyRasterizedImage));
    AsyncTaskManager::Get().AddTask(mRasterizingTask);
  }
}

void CanvasViewImpl::ApplyRasterizedImage(CanvasViewRasterizingTaskPtr task)
{
  mLastCommitRasterized = task->IsRasterized();
  DALI_LOG_DEBUG_INFO("[%p] Rasterized. Success?[%d]\n", this, mLastCommitRasterized);

  if(mLastCommitRasterized)
  {
    Texture rasterizedTexture = task->GetRasterizedTexture();
    if(rasterizedTexture && rasterizedTexture.GetWidth() != 0 && rasterizedTexture.GetHeight() != 0)
    {
      if(mTexture != rasterizedTexture)
      {
        mTexture = rasterizedTexture;

        // Unregister the old visual before invalidating its URL, then wrap the new
        // texture in a member-owned ImageUrl so the TextureManager entry stays alive
        // for the lifetime of the new visual (the destructor calls RequestRemoveExternalResourceByUrl).
        auto& viewData = Internal::ViewDataImpl::Get(*this);
        viewData.UnregisterVisual(Property::CANVAS_CONTENT_VISUAL);
        mContentVisual.Reset();

        mImageUrl = Dali::Ui::ImageUrl::New(rasterizedTexture, true);

        Dali::Property::Map map;
        map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::IMAGE);
        map.Insert(Ui::ImageVisualPropertyIndex::URL, mImageUrl.GetUrl());

        mContentVisual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
        if(mContentVisual)
        {
          viewData.RegisterVisual(Property::CANVAS_CONTENT_VISUAL, mContentVisual, Dali::Ui::Integration::DepthIndex::CONTENT);
          // ViewDataImpl::Process() (ApplyFittingMode) runs before CanvasViewImpl::Process()
          // in the same post-processor cycle, so the visual is registered too late to be
          // sized by the automatic pass — call it explicitly here.
          viewData.ApplyFittingMode(mSize);
        }
      }
      else
      {
        // Texture unchanged — just keep the window rendering so the frame is presented.
        Dali::Window window = mPlacementWindow.GetHandle();
        if(DALI_LIKELY(window))
        {
          window.KeepRendering(0.0f);
        }
      }
    }
  }

  if(task == mRasterizingTask)
  {
    mRasterizingTask.Reset();
  }

  // If in async mode, re-queue if the canvas changed during the rasterize or if the last attempt failed.
  if(!mIsSynchronous && DALI_LIKELY(mCanvasRenderer) &&
     (!mLastCommitRasterized || (!mManualRasterization && mCanvasRenderer.IsCanvasChanged())))
  {
    AddRasterizationTask(!mLastCommitRasterized);
  }
}

// ---------------------------------------------------------------------------
// Drawable management
// ---------------------------------------------------------------------------

bool CanvasViewImpl::AddDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.AddDrawable(drawable);
  }
  return false;
}

bool CanvasViewImpl::RemoveDrawable(Dali::CanvasRenderer::Drawable& drawable)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.RemoveDrawable(drawable);
  }
  return false;
}

bool CanvasViewImpl::RemoveAllDrawables()
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.RemoveAllDrawables();
  }
  return false;
}

void CanvasViewImpl::SetDropShadow(const Vector4& color, float offsetX, float offsetY, float blurRadius)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.SetDropShadow(color, offsetX, offsetY, blurRadius);
    // The renderer marks the canvas as changed, so automatic mode re-rasterizes on the next
    // Process(). In manual mode the request must be made explicitly.
    if(mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

void CanvasViewImpl::ClearDropShadow()
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.ClearDropShadow();
    if(mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

bool CanvasViewImpl::HasDropShadow() const
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.HasDropShadow();
  }
  return false;
}

void CanvasViewImpl::SetGaussianBlur(float blurRadius)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.SetGaussianBlur(blurRadius);
    if(mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

void CanvasViewImpl::ClearGaussianBlur()
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.ClearGaussianBlur();
    if(mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

bool CanvasViewImpl::HasGaussianBlur() const
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.HasGaussianBlur();
  }
  return false;
}

void CanvasViewImpl::SetEffectAutoPaddingEnabled(bool enable)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.SetEffectAutoPaddingEnabled(enable);
    if(mManualRasterization)
    {
      RequestRasterization();
    }
  }
}

bool CanvasViewImpl::IsEffectAutoPaddingEnabled() const
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.IsEffectAutoPaddingEnabled();
  }
  return false;
}

// ---------------------------------------------------------------------------
// Property getters / setters
// ---------------------------------------------------------------------------

void CanvasViewImpl::SetViewBox(const Vector2& viewBox)
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    mCanvasRenderer.SetViewBox(viewBox);
  }
}

Vector2 CanvasViewImpl::GetViewBox()
{
  if(DALI_LIKELY(mCanvasRenderer))
  {
    return mCanvasRenderer.GetViewBox();
  }
  return Vector2::ZERO;
}

void CanvasViewImpl::SetSynchronousLoading(bool synchronous)
{
  mIsSynchronous = synchronous;
}

bool CanvasViewImpl::IsSynchronousLoading() const
{
  return mIsSynchronous;
}

void CanvasViewImpl::SetRasterizationRequestManually(bool manually)
{
  if(mManualRasterization != manually)
  {
    mManualRasterization = manually;
    if(!mManualRasterization)
    {
      // Switching back to automatic mode — ensure polling resumes.
      RequestRasterization();
      if(Adaptor::IsAvailable())
      {
        Adaptor::Get().RequestProcessEventsOnIdle();
      }
    }
  }
}

bool CanvasViewImpl::IsRasterizationRequestManually() const
{
  return mManualRasterization;
}

void CanvasViewImpl::RequestRasterization()
{
  DALI_LOG_DEBUG_INFO("[%p] Rasterize request\n", this);
  // Reset mLastCommitRasterized so that the next Process() call unconditionally
  // rasterizes, regardless of whether IsCanvasChanged() returns true.
  // This makes the public API a true "force rasterize on next cycle" call.
  mLastCommitRasterized = false;
  ScheduleRasterization();
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
