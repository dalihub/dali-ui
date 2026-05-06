#ifndef DALI_UI_INTERNAL_MASK_EFFECT_H
#define DALI_UI_INTERNAL_MASK_EFFECT_H

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
#include <dali-ui-foundation/public-api/render-effects/mask-effect.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/camera-actor.h>
#include <dali/public-api/adaptor-framework/image-options.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/public-api/rendering/shader.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class MaskEffectImpl;
using MaskEffectImplPtr = IntrusivePtr<MaskEffectImpl>;

class MaskEffectImpl : public RenderEffectImpl
{
public:
  /**
   * @brief Creates an initialized MaskEffect with view, using default settings. The default settings are:
   *
   * maskMode = MaskEffect::MaskMode::ALPHA
   * maskPosition = Vector2(0.f, 0.f)
   * maskScale = Vector2(1.f, 1.f)
   *
   * @param[in] maskView The source View to affect mask.
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffectImplPtr New(Ui::View maskView);

  /**
   * @brief Creates an initialized MaskEffect.
   * @param[in] maskView The source View to affect mask.
   * @param[in] maskMode Defines pixel data type (alpha, luminance) used as the mask source.
   * @param[in] maskPosition The Position of mask source.
   * @param[in] maskScale The Scale of mask source.
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffectImplPtr New(Ui::View maskView, MaskEffect::MaskMode maskMode, Vector2 maskPosition,
                               Vector2 maskScale);

  /**
   * @copydoc Ui::Internal::RenderEffectImpl::GetOffScreenRenderableType
   */
  OffScreenRenderable::Type GetOffScreenRenderableType() const override;

  /**
   * @copydoc Ui::Internal::RenderEffectImpl::GetOffScreenRenderTasks
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc Ui::MaskEffect::SetTargetMaskOnce
   */
  void SetTargetMaskOnce(bool targetMaskOnce);

  /**
   * @copydoc Ui::MaskEffect::GetTargetMaskOnce
   */
  bool GetTargetMaskOnce() const;

  /**
   * @copydoc Ui::MaskEffect::SetSourceMaskOnce
   */
  void SetSourceMaskOnce(bool sourceMaskOnce);

  /**
   * @copydoc Ui::MaskEffect::GetSourceMaskOnce
   */
  bool GetSourceMaskOnce() const;

  /**
   * @brief Reverses target and source roles to alter masking direction
   * @param[in] reverseMaskDirection True if masking direction reversed
   */
  void SetReverseMaskDirection(bool reverseMaskDirection);

protected:
  /**
   * @brief Creates an uninitialized mask effect implementation
   * @param[in] maskView
   */
  MaskEffectImpl(Ui::View maskView);

  /**
   * @brief Creates an uninitialized mask effect implementation
   * @param[in] maskView
   * @param[in] maskMode
   * @param[in] maskPosition
   * @param[in] maskScale
   */
  MaskEffectImpl(Ui::View maskView, MaskEffect::MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale);

  /**
   * @brief Destructor
   */
  virtual ~MaskEffectImpl();

  /**
   * @brief Initializes mask effect
   */
  void OnInitialize() override;

  /**
   * @brief Activates mask effect
   * @note If the mask source actor is an ancestor of the target actor,
   * the target will not be rendered due to scene hierarchy.
   */
  void OnActivate() override;

  /**
   * @brief Dectivates mask effect
   */
  void OnDeactivate() override;

  /**
   * @brief Redraw effect without deactivation
   */
  void OnRefresh() override;

private:
  // inner functions
  /**
   * @brief Sets frame buffers to draw masked output.
   * @param[in] size The size of target.
   */
  void CreateFrameBuffers(const ImageDimensions size);

  /**
   * @brief Sets mask render tasks.
   * @param[in] ownerView Input owner view
   */
  void CreateRenderTasks(Ui::View ownerView);

  /**
   * @brief Reset Mask Data
   */
  void ResetMaskData();

  /**
   * @brief Sets shader constants, mask mode, position, and scale.
   * @param[in] ownerView Input owner view
   */
  void SetShaderConstants(Ui::View ownerView);

  /**
   * @brief Create mask data including RenderTask, Framebuffer, Texture.
   * @param[in] ownerView Input owner view
   */
  void CreateMaskData();

  /**
   * @brief Emits render finished signal of the target,
   * when mTargetMaskOnce is true and finished signal of the target render task(mMaskTargetRenderTask) is emitted.
   * @param[in] renderTask that emits target signal.
   */
  void OnTargetRenderFinished(Dali::RenderTask renderTask);

  /**
   * @brief Emits render finished signal of the source,
   * when mSourceMaskOnce is true and finished signal of the source render task(mMaskSourceRenderTask) is emitted.
   * @param[in] renderTask that emits source signal.
   */
  void OnSourceRenderFinished(Dali::RenderTask renderTask);

private:
  MaskEffectImpl(const MaskEffectImpl&)            = delete;
  MaskEffectImpl(MaskEffectImpl&&)                 = delete;
  MaskEffectImpl& operator=(MaskEffectImpl&&)      = delete; // no move()
  MaskEffectImpl& operator=(const MaskEffectImpl&) = delete; // no copy()

private:
  // Camera actors
  CameraActor mCamera;

  WeakHandle<Ui::View> mMaskView;

  // Resource
  RenderTask  mMaskTargetRenderTask;
  FrameBuffer mMaskTargetFrameBuffer;
  Texture     mMaskTargetTexture;

  RenderTask  mMaskSourceRenderTask;
  FrameBuffer mMaskSourceFrameBuffer;
  Texture     mMaskSourceTexture;

  // Variables
  MaskEffect::MaskMode mMaskMode;
  Vector2              mMaskPosition;
  Vector2              mMaskScale;
  bool                 mTargetMaskOnce : 1;
  bool                 mSourceMaskOnce : 1;
  bool                 mReverseMaskDirection : 1;

  // Cached shader
  thread_local static Dali::Shader gMaskEffectShader;
};
} // namespace Internal

inline Ui::Internal::MaskEffectImpl& GetImplementation(Ui::MaskEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Ui::Internal::MaskEffectImpl&>(handle);
}

inline const Ui::Internal::MaskEffectImpl& GetImplementation(const Ui::MaskEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Ui::Internal::MaskEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_BACKGROUND_MASK_EFFECT_H
