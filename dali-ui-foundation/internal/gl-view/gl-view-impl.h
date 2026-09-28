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
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/signals/callback.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-backend.h>
#include <dali-ui-foundation/internal/gl-view/gl-view-render-info-impl.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view-offscreen-config.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class GlViewImpl;
using GlViewImplPtr = IntrusivePtr<GlViewImpl>;

/**
 * @brief Internal implementation class for GlView.
 *
 * Holds what does not depend on how the application's GL code reaches the screen -
 * the registered callbacks, the render info handed to them, and the terminate
 * handshake - and leaves the rest to a GlViewBackend chosen at construction.
 *
 * The callbacks run on a rendering thread, which one depending on the backend, and
 * the state they touch is only ever accessed from there. The two threads meet at
 * exactly one point: the rendering thread reports the terminate invocation back
 * through an EventThreadCallback, and everything it was using is released on the
 * event thread once that lands.
 *
 * @see Dali::Ui::GlView
 */
class DALI_UI_API GlViewImpl : public ViewImpl
{
public:
  /**
   * @brief Creates a new GlViewImpl.
   *
   * @param[in] backendMode The backend used to execute the application's GL code
   */
  static GlViewImplPtr New(GlView::BackendMode backendMode, const GlViewOffscreenConfig& offscreenConfig);

  /**
   * @copydoc Dali::Ui::GlView::RegisterGlCallbacks
   */
  void RegisterGlCallbacks(Dali::Callback<void(const GlViewRenderInfo&)> initCallback,
                           Dali::Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                           Dali::Callback<void()>                        terminateCallback);

  /**
   * @copydoc Dali::Ui::GlView::BindTextureResources
   */
  void BindTextureResources(Dali::Vector<Dali::Texture> textures);

  /**
   * @copydoc Dali::Ui::GlView::SetRenderingMode
   */
  void SetRenderingMode(GlView::RenderingMode mode);

  /**
   * @copydoc Dali::Ui::GlView::GetRenderingMode
   */
  GlView::RenderingMode GetRenderingMode() const;

  /**
   * @copydoc Dali::Ui::GlView::RenderOnce
   */
  void RenderOnce();

  /**
   * @copydoc Dali::Ui::GlView::GetBackendMode
   */
  GlView::BackendMode GetBackendMode() const;

  /**
   * @copydoc Dali::Ui::GlView::Terminate
   */
  void Terminate(Dali::Callback<void()> onTerminated);

public: // Not intended for application developers - used by the backends
  /**
   * @brief Returns the render info the callbacks are handed.
   *
   * @return The render info
   *
   * @note Filled in by the backend on its rendering thread, immediately before the
   *       callbacks that read it.
   */
  Ui::GlViewRenderInfo& GetRenderInfo();

  /**
   * @brief Invokes the application's init callback, if it registered one.
   * @note Rendering thread only.
   */
  void InvokeInitCallback();

  /**
   * @brief Invokes the application's render frame callback, if it registered one.
   *
   * @return What the application reported, or false if it registered nothing
   *
   * @note Rendering thread only.
   */
  bool InvokeRenderFrameCallback();

  /**
   * @brief Invokes the application's terminate callback, if it registered one.
   * @note Rendering thread only.
   */
  void InvokeTerminateCallback();

  /**
   * @brief Reports that the terminate invocation is done, waking the event thread.
   *
   * @note Rendering thread only, and the last statement that may touch this object:
   *       the event thread can start releasing the view before this returns.
   */
  void NotifyTerminateCompleted();

  /**
   * @brief Requests a frame so pending render or terminate work is processed.
   */
  void KeepRendering();

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] backendMode The backend used to execute the application's GL code
   * @param[in] offscreenConfig Describes the offscreen buffer, where one is used
   */
  GlViewImpl(GlView::BackendMode backendMode, const GlViewOffscreenConfig& offscreenConfig);

  /**
   * @brief A reference-counted object may only be deleted by calling Unreference().
   */
  ~GlViewImpl() override;

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc ViewImpl::OnArrange
   */
  LayoutRect OnArrange(const LayoutRect& bounds) override;

  /**
   * @copydoc ViewImpl::OnSceneConnection
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc ViewImpl::OnSceneDisconnection
   */
  void OnSceneDisconnection() override;

private:
  GlViewImpl(const GlViewImpl&)            = delete;
  GlViewImpl(GlViewImpl&&)                 = delete;
  GlViewImpl& operator=(const GlViewImpl&) = delete;
  GlViewImpl& operator=(GlViewImpl&&)      = delete;

  /**
   * @brief Completes the terminate sequence. Does nothing once it has run.
   *
   * May destroy this object, as it releases the reference taken by Terminate().
   */
  void OnTerminateCompleted();

  /**
   * @brief Invoked when the application is shutting down.
   *
   * Completes a terminate the rendering side can no longer report back on.
   */
  void OnApplicationTerminate();

  /**
   * @brief Tells the backend whether the view can currently be seen.
   */
  void UpdateVisibility();

  /**
   * @brief Invoked when the view's on-scene visibility changes.
   */
  void OnSceneVisibilityChanged(Dali::Actor actor, bool visible);

  /**
   * @brief Invoked when the window the view is on changes visibility.
   */
  void OnWindowVisibilityChanged(Dali::Window window, bool visible);

private:
  const GlView::BackendMode mBackendMode;
  GlView::RenderingMode     mRenderingMode{GlView::RenderingMode::CONTINUOUS};

  UniquePtr<GlViewBackend> mBackend;
  Ui::GlViewRenderInfo     mRenderInfo;
  Dali::Window             mPlacementWindow;

  Dali::Callback<void(const GlViewRenderInfo&)> mInitCallback;
  Dali::Callback<bool(const GlViewRenderInfo&)> mRenderFrameCallback;
  Dali::Callback<void()>                        mTerminateCallback;
  Dali::Callback<void()>                        mTerminateCompletedCallback;

  std::unique_ptr<EventThreadCallback> mEventTrigger;

  /// Keeps this object alive from Terminate() until the render thread has reported back.
  Dali::BaseHandle mSelfReference;

  /// Event thread only.
  bool mTerminateRequested{false};
  bool mTerminateCompleted{false};
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
