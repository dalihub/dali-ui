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
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/gl-view/gl-view-backend.h>
#include <dali-ui-foundation/internal/gl-view/gl-view-render-thread.h>
#include <dali-ui-foundation/public-api/views/gl/gl-view-offscreen-config.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
/**
 * @brief Runs the application's GL code on a thread of its own, into a buffer.
 *
 * The application draws into a NativeImageQueue through a NativeImageSurface, and
 * what it drew becomes a texture that a quad renderer on the view composites. So
 * the two sides are only coupled by the queue: the application's thread produces
 * frames at its own rate and DALi shows the newest one it finds.
 *
 * Costs the memory of the queue's buffers, and puts the application's content one
 * frame behind the UI drawn around it.
 */
class GlViewOffscreenBackend : public GlViewBackend
{
public:
  /**
   * @brief Constructor.
   *
   * @param[in] config Describes the buffer the application renders into
   */
  explicit GlViewOffscreenBackend(const GlViewOffscreenConfig& config);

  /**
   * @brief Destructor.
   */
  ~GlViewOffscreenBackend() override;

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
   * @copydoc GlViewBackend::OnArrange
   */
  void OnArrange(const Size& size) override;

  /**
   * @copydoc GlViewBackend::OnVisibilityChanged
   */
  void OnVisibilityChanged(bool visible) override;

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
   * @brief Builds the quad renderer that composites the queue's texture.
   *
   * @return The renderer, or an uninitialized handle if there is no queue
   */
  Dali::Renderer CreateCompositeRenderer();

  /**
   * @brief Starts or pauses the thread, according to whether there is anything to draw.
   */
  void UpdateThreadState();

  /**
   * @brief Waits for the thread to end, if it ever started.
   */
  void JoinThread();

private:
  const GlViewOffscreenConfig mConfig;

  GlViewImpl* mOwner{nullptr};

  Dali::NativeImageQueuePtr     mNativeImageQueue;
  UniquePtr<GlViewRenderThread> mRenderThread;
  Dali::Renderer                mRenderer;

  Size mSize;
  bool mVisible{false};
  bool mThreadRunning{false};
};

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
