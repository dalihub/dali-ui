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
#include <dali/devel-api/signals/render-callback.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-backend.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
/**
 * @brief Runs the application's GL code inside the DALi graphics pipeline.
 *
 * A Dali::RenderCallback backed Renderer injects the callbacks at the point in
 * the frame where the view would be drawn, so its output composites with the
 * DALi content around it and goes straight into the window surface. There is no
 * rendering thread of its own: the callbacks run on the DALi render thread.
 *
 * Serves both GlView::BackendMode::DIRECT_RENDERING and
 * GlView::BackendMode::UNSAFE_DIRECT_RENDERING, which differ only in whether the
 * callback gets an isolated EGL context.
 */
class GlViewDirectBackend : public GlViewBackend
{
public:
  /**
   * @brief Constructor.
   *
   * @param[in] backendMode Which of the two direct backends this is
   */
  explicit GlViewDirectBackend(GlView::BackendMode backendMode);

  /**
   * @brief Destructor.
   */
  ~GlViewDirectBackend() override;

private: // From GlViewBackend
  /**
   * @copydoc GlViewBackend::Initialize
   */
  void Initialize(GlViewImpl& owner) override;

  /**
   * @copydoc GlViewBackend::SetRenderingMode
   */
  void SetRenderingMode(GlView::RenderingMode mode) override;

  /**
   * @copydoc GlViewBackend::RenderOnce
   */
  void RenderOnce() override;

  /**
   * @copydoc GlViewBackend::BindTextureResources
   */
  void BindTextureResources(Dali::Vector<Dali::Texture> textures) override;

  /**
   * @copydoc GlViewBackend::StartTerminate
   */
  bool StartTerminate() override;

  /**
   * @copydoc GlViewBackend::AbandonQuietly
   */
  void AbandonQuietly() override;

  /**
   * @copydoc GlViewBackend::OnTerminateCompleted
   */
  void OnTerminateCompleted() override;

private:
  /**
   * @brief Invoked on the DALi render thread for every frame the view is drawn.
   *
   * @param[in] input Rendering context filled in by DALi and the graphics backend
   * @return True to keep the callback registered
   */
  bool OnRenderCallback(const Dali::RenderCallbackInput& input);

private:
  GlViewImpl* mOwner{nullptr};

  /// Shared with the render side, which invokes it until it is done with it.
  Dali::RenderCallbackPtr mRenderCallback;

  Dali::Renderer mRenderer;

  /// Render thread only.
  bool mInitInvoked{false};
  bool mTerminateInvoked{false};
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
