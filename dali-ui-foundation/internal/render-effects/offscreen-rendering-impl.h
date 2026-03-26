#ifndef DALI_UI_INTERNAL_OFFSCREEN_RENDERING_IMPL
#define DALI_UI_INTERNAL_OFFSCREEN_RENDERING_IMPL

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
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * @brief This effect renders all children(including self) at off screen framebuffer.
 * This effect is handleless, so initiate an instance through setting View::Property::OFFSCREEN_RENDERING
 * The instance will be saved internally, thus outer modification is impossible.
 */
class OffScreenRenderingImpl : public RenderEffectImpl
{
public:
  /**
   * @brief Constructor
   * @param[in] type Defines whether effect is enabled or not, and its refresh rate
   */
  OffScreenRenderingImpl(Ui::View::OffScreenRenderingType type);

  /**
   * @brief Set OffScreenRenderingType explicitly
   * @param[in] type Defines whether effect is enabled or not, and its refresh rate
   */
  void SetType(Ui::View::OffScreenRenderingType type);

  // @copydoc Dali::Ui::Internal::RenderEffectImpl::GetOffScreenRenderableType
  OffScreenRenderable::Type GetOffScreenRenderableType() const override;

  // @copydoc Dali::Ui::Internal::RenderEffectImpl::GetOffScreenRenderTasks
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @brief Get rendered output. Only valid when type is set to OffScreenRenderingType::RENDER_ONCE
   * @return mTexture, the output
   */
  Dali::Texture GetTexture() const;

protected:
  // @copydoc Dali::Ui::Internal::RenderEffectImpl::OnInitialize
  void OnInitialize() override;

  // @copydoc Dali::Ui::Internal::RenderEffectImpl::OnActivate
  void OnActivate() override;

  // @copydoc Dali::Ui::Internal::RenderEffectImpl::OnDeactivate
  void OnDeactivate() override;

  // @copydoc Dali::Ui::Internal::RenderEffectImpl::OnRefresh
  void OnRefresh() override;

private:
  /**
   * @brief Initializes offscreen buffer to draw subtree
   */
  void CreateFrameBuffer();

  /**
   * @brief Destroys offscreen buffer
   */
  void DestroyFrameBuffer();

  /**
   * @brief Initializes render task for offscreen rendering
   */
  void CreateRenderTask();

  /**
   * @brief Destroys render task for offscreen rendering
   */
  void DestroyRenderTask();

  /**
   * @brief Saves texture after render once is finished.
   */
  void OnRenderFinished(Dali::RenderTask& task);

private:
  RenderTask                       mRenderTask;
  CameraActor                      mCamera;
  FrameBuffer                      mFrameBuffer;
  Dali::Texture                    mTexture; // saved when render once
  Ui::View::OffScreenRenderingType mType;
};
} // namespace Internal
} // namespace Ui
} // namespace Dali
#endif // DALI_UI_INTERNAL_OFFSCREEN_RENDERING_IMPL
