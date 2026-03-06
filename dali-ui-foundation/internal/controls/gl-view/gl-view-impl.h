#ifndef DALI_UI_INTERNAL_GL_VIEW_H
#define DALI_UI_INTERNAL_GL_VIEW_H

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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/native-image-queue.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/public-api/rendering/shader.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/controls/gl-view/gl-view-interface-impl.h>
#include <dali-ui-foundation/internal/controls/gl-view/gl-view-render-thread.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali-ui-foundation/public-api/controls/gl-view/gl-view.h>

namespace Dali::Ui
{
class GlView;

namespace Internal
{
class GlView : public Dali::Ui::Internal::GlViewImpl
{
protected:
  virtual ~GlView();

public:
  /**
   * @copydoc Dali::Ui::GlView::New()
   */
  static Dali::Ui::GlView New(Dali::Ui::GlView::ColorFormat colorFormat);

  /**
   * Construct a new GlView.
   */
  GlView(Dali::Ui::GlView::ColorFormat colorFormat);

  /**
   * @copydoc Dali::Ui::GlView::RegisterGlCallbacks()
   */
  void RegisterGlCallbacks(CallbackBase* initCallback, CallbackBase* renderFrameCallback,
                           CallbackBase* terminateCallback) override;

  /**
   * @copydoc Dali::Ui::GlView::SetResizeCallback()
   */
  void SetResizeCallback(CallbackBase* resizeCallback) override;

  /**
   * @copydoc Dali::Ui::GlView::SetGraphicsConfig()
   */
  bool SetGraphicsConfig(bool depth, bool stencil, int msaa, Dali::Ui::GlView::GraphicsApiVersion version) override;

  /**
   * @copydoc Dali::Ui::GlView::SetRenderingMode()
   */
  void SetRenderingMode(Dali::Ui::GlView::RenderingMode mode) override;

  /**
   * @copydoc Dali::Ui::GlView::GetRenderingMode()
   */
  Dali::Ui::GlView::RenderingMode GetRenderingMode() const override
  {
    return mRenderingMode;
  }

  /**
   * @copydoc Dali::Ui::GlView::RenderOnce()
   */
  void RenderOnce() override;

  /**
   * @copydoc Dali::Ui::GlView::BindTextureResources()
   */
  void BindTextureResources(Dali::Vector<Dali::Texture> textures) override;

  /**
   * @copydoc Dali::Ui::GlView::Terminate()
   */
  void Terminate() override;

private: // From Control
  /**
   * @copydoc Ui::Control::OnInitialize()
   */
  virtual void OnInitialize() override;

  /**
   * @copydoc Ui::Control::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Ui::Control::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Ui::Control::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

private:
  // Undefined copy constructor and assignment operators
  GlView(const GlView& GlView);
  GlView& operator=(const GlView& GlView);

  /**
   * Callback when the inherited visibility of the GlView is changed
   */
  void OnControlInheritedVisibilityChanged(Dali::Actor actor, bool visible);

  /**
   * Callback when the visibility of the window is changed
   */
  void OnWindowVisibilityChanged(Dali::Window window, bool visible);

  /**
   * Creates the geometry for texturing.
   */
  Dali::Geometry CreateTexturedQuad();

  /**
   * Adds renderer to Actor.
   */
  void AddRenderer();

  /**
   * Creates shader for rendering.
   */
  Dali::Shader CreateShader();

  /**
   * @brief Gets the NativeImageQueues ColorFormat with the GlView's ColorFormat.
   * @param[in] colorFormat the color format of the GlView.
   * @return The color format of NativeImageSourceQueue
   */
  Dali::NativeImageQueue::ColorFormat GetColorFormat(Dali::Ui::GlView::ColorFormat format);

private:
  std::unique_ptr<GlViewRenderThread> mRenderThread;
  Dali::NativeImageQueuePtr mNativeImageQueue;
  Dali::Ui::GlView::RenderingMode mRenderingMode{Ui::GlView::RenderingMode::CONTINUOUS};
  Dali::Ui::GlView::ColorFormat mColorFormat{Ui::GlView::ColorFormat::RGB888};
  bool mDepth{false};
  bool mStencil{false};
  int mMSAA{0};
};

} // namespace Internal

} // namespace Dali::Ui

#endif // DALI_UI_INTERNAL_GL_VIEW_H
