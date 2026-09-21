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
#include <dali-ui-foundation/internal/gl-view/gl-view-render-thread.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <algorithm>
#include <chrono>
#include <thread>

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
constexpr uint64_t NANOSECONDS_PER_SECOND = 1000000000ull;
constexpr uint64_t DEFAULT_FRAME_DURATION = NANOSECONDS_PER_SECOND / 60ull;

/**
 * How much depth there is behind the near plane, mirroring what DALi's default camera
 * leaves for a direct backend - content the direct backends do not clip in Z must not be
 * clipped here either. DALi derives the near plane from the scene size and puts the far
 * plane this much further back; the near plane here comes from the buffer instead, which
 * is the view's own space in both cases.
 *
 * @see dali-core CalculateClippingAndZ().
 */
constexpr float DEPTH_RANGE = 4095.0f;

uint64_t GetNanoSeconds()
{
  const auto epoch = std::chrono::steady_clock::now().time_since_epoch();
  return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count());
}

void SleepUntil(uint64_t timeInNanoseconds)
{
  using Clock = std::chrono::steady_clock;
  std::this_thread::sleep_until(Clock::time_point(std::chrono::nanoseconds(timeInNanoseconds)));
}
} // namespace

GlViewRenderThread::GlViewRenderThread(GlViewImpl& owner, Dali::NativeImageQueuePtr queue)
: mOwner(owner),
  mLogFactory(Dali::Adaptor::Get().GetLogFactory()),
  mTraceFactory(Dali::Adaptor::Get().GetTraceFactory()),
  mNativeImageQueue(queue),
  mSurfaceSemaphore(1),
  mFrameDurationNanoseconds(DEFAULT_FRAME_DURATION)
{
  mNativeImageSurface = Dali::NativeImageSurface::New(mNativeImageQueue);
  if(!mNativeImageSurface)
  {
    DALI_LOG_ERROR("GlView: could not create a native image surface, offscreen rendering is unavailable\n");
  }
}

GlViewRenderThread::~GlViewRenderThread() = default;

bool GlViewRenderThread::SetGraphicsConfig(bool depth, bool stencil, int msaa, int glesVersion)
{
  return mNativeImageSurface ? mNativeImageSurface->SetGraphicsConfig(depth, stencil, msaa, glesVersion) : false;
}

void GlViewRenderThread::SetSurfaceSize(const Size& size)
{
  // The caller holds the surface, so the thread is not reading this.
  mSurfaceSize = size;
}

void GlViewRenderThread::SetOnDemandRenderMode(bool onDemand)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mOnDemand = onDemand;
  if(!onDemand && !mThreadPaused)
  {
    mConditionalWait.Notify(lock);
  }
}

void GlViewRenderThread::RenderOnce()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mRenderRequested = true;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::Pause()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mThreadPaused = true;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::Resume()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!mThreadStarted)
  {
    Start();
    mThreadStarted = true;
  }
  mThreadPaused = false;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::Stop()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mInvokeTerminateCallback = true;
  mThreadStopped           = true;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::StopQuietly()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mInvokeTerminateCallback = false;
  mThreadStopped           = true;
  mConditionalWait.Notify(lock);
}

void GlViewRenderThread::AcquireSurface()
{
  mSurfaceSemaphore.Acquire();
}

void GlViewRenderThread::ReleaseSurface()
{
  mSurfaceSemaphore.Release(1);
}

bool GlViewRenderThread::RenderReady(uint64_t& frameDeadline)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);

  while(!mThreadStopped && ((mOnDemand && !mRenderRequested) || mThreadPaused))
  {
    frameDeadline = 0;
    mConditionalWait.Wait(lock);
  }

  mRenderRequested = false;
  return !mThreadStopped;
}

void GlViewRenderThread::UpdateRenderInfo()
{
  auto& info = mOwner.GetRenderInfo().GetImplementation();

  // Nothing points this at a RenderCallbackInput, so the synthesised values are read.
  info.input = nullptr;

  info.size        = mSurfaceSize;
  info.clippingBox = BoundsInteger(0, 0, static_cast<int32_t>(mSurfaceSize.width), static_cast<int32_t>(mSurfaceSize.height));

  // The view's own pixel space with the origin at its centre, which is the space a
  // direct backend's MVP matrix maps from. Y is flipped so that, as there, moving down
  // the screen means increasing Y.
  const float halfWidth  = mSurfaceSize.width * 0.5f;
  const float halfHeight = mSurfaceSize.height * 0.5f;

  Matrix ortho(false);
  ortho.SetIdentityAndScale(Vector3::ONE);
  if(halfWidth > 0.0f && halfHeight > 0.0f)
  {
    // Z is scaled rather than left at 1 because content in this space is authored
    // against the view's pixel size in all three axes - a cube as wide as the view is
    // just as deep - so a unit range would clip all but a slice of it. The near plane
    // is where DALi's default camera would put it for a scene this size, and the rest
    // of the range lies behind it.
    const float zNear = std::max(mSurfaceSize.width, mSurfaceSize.height);

    float* m = ortho.AsFloat();
    m[0]     = 1.0f / halfWidth;
    m[5]     = -1.0f / halfHeight;

    // Negative, so that increasing Z - towards the viewer, as in DALi - takes the
    // smaller depth value and wins the depth test.
    m[10] = -2.0f / DEPTH_RANGE;
    m[14] = 2.0f * zNear / DEPTH_RANGE - 1.0f;
  }

  info.projection = ortho;
  info.mvp        = ortho;
}

void GlViewRenderThread::Run()
{
  Dali::SetThreadName("GlViewRenderer");
  mLogFactory.InstallLogFunction();
  mTraceFactory.InstallTraceFunction();

  if(!mNativeImageSurface)
  {
    return;
  }

  AcquireSurface();
  mNativeImageSurface->InitializeGraphics();
  ReleaseSurface();

  uint64_t frameDeadline = 0;

  while(RenderReady(frameDeadline))
  {
    const uint64_t frameStartTime = GetNanoSeconds();

    AcquireSurface();
    mNativeImageSurface->PreRender();

    // Only now is a buffer dequeued and the context current, so this is the first
    // point the application's GL code can run.
    if(mNativeImageSurface->CanRender())
    {
      UpdateRenderInfo();

      if(!mInitInvoked)
      {
        mInitInvoked = true;
        mOwner.InvokeInitCallback();
      }

      // Unlike the direct backends there is a present step to gate, so what the
      // application reports about producing new content is acted on.
      if(mOwner.InvokeRenderFrameCallback())
      {
        mNativeImageSurface->PostRender();
      }
    }

    ReleaseSurface();

    if(frameDeadline == 0)
    {
      frameDeadline = frameStartTime + mFrameDurationNanoseconds;
    }
    else
    {
      frameDeadline += mFrameDurationNanoseconds;

      // Skip whole frames rather than trying to catch up, if we fell far behind.
      const uint64_t frameEndTime = GetNanoSeconds();
      while(frameEndTime > frameDeadline + mFrameDurationNanoseconds)
      {
        frameDeadline += mFrameDurationNanoseconds;
      }
    }

    SleepUntil(frameDeadline);
  }

  bool invokeTerminateCallback = false;
  {
    ConditionalWait::ScopedLock lock(mConditionalWait);
    invokeTerminateCallback = mInvokeTerminateCallback;
  }

  // The application has GL resources to release only if its init callback ran, and can
  // only release them where a context is current.
  if(invokeTerminateCallback && mInitInvoked)
  {
    AcquireSurface();
    mNativeImageSurface->PreRender();
    if(mNativeImageSurface->CanRender())
    {
      mOwner.InvokeTerminateCallback();
    }
    ReleaseSurface();
  }

  mNativeImageSurface->TerminateGraphics();

  if(invokeTerminateCallback)
  {
    // Last statement that may touch the owner: the event thread starts releasing the
    // view as soon as the trigger lands. It joins this thread before doing so, which is
    // what keeps the rest of this function's return safe.
    mOwner.NotifyTerminateCompleted();
  }
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
