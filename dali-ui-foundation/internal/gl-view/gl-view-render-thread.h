#pragma once

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

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/native-image-queue.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <dali/devel-api/threading/semaphore.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <dali/integration-api/adaptor-framework/native-image-surface.h>
#include <dali/integration-api/adaptor-framework/trace-factory-interface.h>
#include <dali/public-api/math/vector2.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class GlViewImpl;

/**
 * @brief The rendering thread behind GlView's offscreen backend.
 *
 * Drives a NativeImageSurface, which is where the application's GL code runs: the
 * thread makes the surface current, invokes the registered callbacks through its
 * owner, and hands the finished frame back to the queue the surface draws into.
 *
 * Paced at its own rate rather than DALi's, which is the point of the backend - the
 * application's GL code neither waits for the UI nor holds it up.
 */
class GlViewRenderThread : public Dali::Thread
{
public:
  /**
   * @brief Constructor.
   *
   * @param[in] owner The view whose callbacks this thread invokes
   * @param[in] queue The queue the surface renders into
   */
  GlViewRenderThread(GlViewImpl& owner, Dali::NativeImageQueuePtr queue);

  /**
   * @brief Destructor.
   */
  ~GlViewRenderThread() override;

  /**
   * @brief Applies the requested graphics configuration to the surface.
   *
   * @param[in] depth Whether a depth buffer is wanted
   * @param[in] stencil Whether a stencil buffer is wanted
   * @param[in] msaa Samples per pixel, or 0 to disable multisampling
   * @param[in] glesVersion The GLES version, as 20 or 30
   * @return True if the platform can provide such a surface
   *
   * @note Must be called before the thread is started. A false return means nothing will
   *       ever be produced, so the caller has no reason to keep this thread.
   */
  bool SetGraphicsConfig(bool depth, bool stencil, int msaa, int glesVersion);

  /**
   * @brief Records the size the queue was resized to.
   *
   * @param[in] size The new surface size
   *
   * @note Call between AcquireSurface() and ReleaseSurface(), so the thread is not
   *       rendering into the buffers being replaced.
   */
  void SetSurfaceSize(const Size& size);

  /**
   * @brief Sets whether frames are produced continuously or on request.
   *
   * @param[in] onDemand True to render only when RenderOnce() asks
   */
  void SetOnDemandRenderMode(bool onDemand);

  /**
   * @brief Asks for one frame while in on-demand mode.
   */
  void RenderOnce();

  /**
   * @brief Stops producing frames, without ending the thread.
   */
  void Pause();

  /**
   * @brief Resumes producing frames, starting the thread if it has not run yet.
   */
  void Resume();

  /**
   * @brief Stops the thread, running the application's terminate callback first.
   *
   * @note The thread reports back through GlViewImpl::NotifyTerminateCompleted()
   *       once the callback has run.
   */
  void Stop();

  /**
   * @brief Stops the thread without invoking any application callback.
   */
  void StopQuietly();

  /**
   * @brief Takes the surface, so the queue can be resized without racing the thread.
   */
  void AcquireSurface();

  /**
   * @brief Releases the surface taken by AcquireSurface().
   */
  void ReleaseSurface();

protected:
  /**
   * @copydoc Dali::Thread::Run
   */
  void Run() override;

private:
  GlViewRenderThread(const GlViewRenderThread&)            = delete;
  GlViewRenderThread& operator=(const GlViewRenderThread&) = delete;

  /**
   * @brief Waits until there is a reason to produce the next frame.
   *
   * @param[in,out] frameDeadline The deadline the previous frame was paced to, in
   *                              nanoseconds. Cleared where the thread had to wait, as
   *                              the cadence it described no longer holds
   * @return False if the thread should end
   */
  bool RenderReady(uint64_t& frameDeadline);

  /**
   * @brief Fills in the render info the application's callbacks read.
   *
   * There is no render task drawing this view, so the values that describe where it
   * sits on screen are built here instead: an orthographic projection over the
   * view's own pixel space, which is the space the direct backends hand over too.
   */
  void UpdateRenderInfo();

private:
  GlViewImpl& mOwner;

  const Dali::LogFactoryInterface&   mLogFactory;
  const Dali::TraceFactoryInterface& mTraceFactory;

  Dali::NativeImageQueuePtr   mNativeImageQueue;
  Dali::NativeImageSurfacePtr mNativeImageSurface;

  /// Serialises the queue's buffers against the event thread resizing them. Binary, so
  /// a Release() without a matching Acquire() throws rather than lifting the exclusion.
  Semaphore<1> mSurfaceSemaphore;

  Dali::ConditionalWait mConditionalWait;

  Size mSurfaceSize{1.0f, 1.0f};

  /// Guarded by mConditionalWait.
  bool mThreadStarted{false};
  bool mThreadStopped{false};
  bool mThreadPaused{false};
  bool mRenderRequested{false};
  bool mOnDemand{false};
  bool mInvokeTerminateCallback{false};

  /// Thread only.
  bool mInitInvoked{false};

  uint64_t mFrameDurationNanoseconds;
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
