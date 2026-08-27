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
#include <dali/devel-api/signals/render-callback.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/rendering/renderer.h>
#include <memory>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-render-info-impl.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
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
 * Attaches a Dali::RenderCallback backed Renderer to the view so the application's
 * GL code is injected into the DALi graphics pipeline. The execution mode of the
 * callback follows the requested backend mode.
 *
 * The GL callbacks run on the DALi render thread, and the state they touch is only ever
 * accessed from that thread. The two threads meet at exactly one point: the render thread
 * reports the terminate invocation back through an EventThreadCallback, and everything
 * the render thread was using is released on the event thread once that lands.
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
  static GlViewImplPtr New(GlView::BackendMode backendMode);

  /**
   * @copydoc Dali::Ui::GlView::RegisterGlCallbacks
   */
  void RegisterGlCallbacks(Ui::Callback<void(const GlViewRenderInfo&)> initCallback,
                           Ui::Callback<bool(const GlViewRenderInfo&)> renderFrameCallback,
                           Ui::Callback<void()>                        terminateCallback);

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
  void Terminate(Ui::Callback<void()> onTerminated);

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] backendMode The backend used to execute the application's GL code
   */
  explicit GlViewImpl(GlView::BackendMode backendMode);

  /**
   * @brief A reference-counted object may only be deleted by calling Unreference().
   */
  ~GlViewImpl() override;

private: // From ViewImpl
  /**
   * @copydoc ViewImpl::OnInitialize
   */
  void OnInitialize() override;

private:
  GlViewImpl(const GlViewImpl&)            = delete;
  GlViewImpl(GlViewImpl&&)                 = delete;
  GlViewImpl& operator=(const GlViewImpl&) = delete;
  GlViewImpl& operator=(GlViewImpl&&)      = delete;

  /**
   * @brief Invoked on the DALi render thread for every frame this view is drawn.
   *
   * @param[in] input Rendering context filled in by DALi and the graphics backend
   * @return True to keep the callback registered
   */
  bool OnRenderCallback(const Dali::RenderCallbackInput& input);

  /**
   * @brief Invoked on the event thread once the render thread has reported back.
   *
   * May destroy this object, as it releases the reference taken by Terminate().
   */
  void OnTerminateCompleted();

  /**
   * @brief Requests a frame so pending render or terminate work is processed.
   */
  void KeepRendering();

private:
  const GlView::BackendMode mBackendMode;
  GlView::RenderingMode     mRenderingMode{GlView::RenderingMode::CONTINUOUS};

  UniquePtr<Dali::RenderCallback> mRenderCallback;
  Dali::Renderer                  mRenderer;

  Ui::GlViewRenderInfo mRenderInfo;

  Ui::Callback<void(const GlViewRenderInfo&)> mInitCallback;
  Ui::Callback<bool(const GlViewRenderInfo&)> mRenderFrameCallback;
  Ui::Callback<void()>                        mTerminateCallback;
  Ui::Callback<void()>                        mTerminateCompletedCallback;

  std::unique_ptr<EventThreadCallback> mEventTrigger;

  /// Keeps this object alive from Terminate() until the render thread has reported back.
  Dali::BaseHandle mSelfReference;

  /// Render thread only.
  bool mInitInvoked{false};
  bool mTerminateInvoked{false};

  /// Event thread only.
  bool mTerminateRequested{false};
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
