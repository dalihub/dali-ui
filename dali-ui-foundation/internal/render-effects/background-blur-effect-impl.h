#ifndef DALI_UI_INTERNAL_BACKGROUND_BLUR_EFFECT_H
#define DALI_UI_INTERNAL_BACKGROUND_BLUR_EFFECT_H

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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/rendering/frame-buffer.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/render-effects/gaussian-blur-algorithm.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/public-api/render-effects/background-blur-effect.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class BackgroundBlurEffectImpl;
using BackgroundBlurEffectImplPtr = IntrusivePtr<BackgroundBlurEffectImpl>;

class BackgroundBlurEffectImpl : public RenderEffectImpl
{
public:
  /**
   * @brief Creates an initialized BlurEffect implementation, using default settings. As default, blur radius is set to
   * 10u.
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffectImplPtr New();

  /**
   * @brief Creates an initialized BlurEffect implementation.
   *
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffectImplPtr New(uint32_t blurRadius);

  /**
   * @copydoc Ui::Internal::RenderEffectImpl::GetOffScreenRenderableType
   */
  OffScreenRenderable::Type GetOffScreenRenderableType() const override;

  /**
   * @copydoc Ui::Internal::RenderEffectImpl::GetOffScreenRenderTasks
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc Ui::BackgroundBlurEffect::SetBlurOnce
   */
  void SetBlurOnce(bool blurOnce);

  /**
   * @copydoc Ui::BackgroundBlurEffect::GetBlurOnce
   */
  bool GetBlurOnce() const;

  /**
   * @copydoc Ui::BackgroundBlurEffect::SetBlurRadius
   */
  void SetBlurRadius(uint32_t blurRadius);

  /**
   * @copydoc Ui::BackgroundBlurEffect::GetBlurRadius
   */
  uint32_t GetBlurRadius() const;

  /**
   * @copydoc Ui::BackgroundBlurEffect::SetBlurDownscaleFactor
   */
  void SetBlurDownscaleFactor(float downscaleFactor);

  /**
   * @copydoc Ui::BackgroundBlurEffect::GetBlurDownscaleFactor
   */
  float GetBlurDownscaleFactor() const;

  /**
   * @copydoc Ui::BackgroundBlurEffect::AddBlurStrengthAnimation
   */
  void AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod,
                                float fromValue, float toValue);

  /**
   * @copydoc Ui::BackgroundBlurEffect::AddBlurOpacityAnimation
   */
  void AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod,
                               float fromValue, float toValue);

  /**
   * @copydoc Ui::BackgroundBlurEffect::FinishedSignal
   */
  Dali::Ui::BackgroundBlurEffect::FinishedSignalType& FinishedSignal();

  /**
   * @copydoc Ui::BackgroundBlurEffect::SetSourceActor
   */
  void SetSourceActor(Dali::Actor sourceActor);

  /**
   * @copydoc Ui::BackgroundBlurEffect::SetStopperActor
   */
  void SetStopperActor(Dali::Actor stopperActor);

protected:
  /**
   * @brief Creates an uninitialized blur effect implementation
   */
  BackgroundBlurEffectImpl();

  /**
   * @brief Creates an uninitialized blur effect implementation
   * @param[in] blurRadius The radius of Gaussian kernel.
   */
  BackgroundBlurEffectImpl(uint32_t blurRadius);

  /**
   * @brief Destructor
   */
  virtual ~BackgroundBlurEffectImpl();

  /**
   * @brief Initializes blur effect
   */
  void OnInitialize() override;

  /**
   * @brief Activates blur effect
   */
  void OnActivate() override;

  /**
   * @brief Dectivates blur effect
   */
  void OnDeactivate() override;

  /**
   * @brief Redraws effect without deactivation
   */
  void OnRefresh() override;

private:
  // Inner functions
  /**
   * @brief Sets frame buffers to draw blurred output.
   * @param[in] downsampledSize Downsampled size for performance.
   */
  void CreateFrameBuffers(const ImageDimensions downsampledSize);

  /**
   * @brief Removes and destroys local frame buffers.
   */
  void DestroyFrameBuffers();

  /**
   * @brief Sets blur render tasks.
   * Requires initialized buffers, source actors, and source cameras.
   * @param[in] sceneHolder SceneHolder of source view
   * @param[in] sourceView Input source view
   */
  void CreateRenderTasks(Dali::Integration::SceneHolder sceneHolder, const Ui::View sourceView);

  /**
   * @brief Removes and destroys local render tasks.
   */
  void DestroyRenderTasks();

  /**
   * @brief Apply render tasks source actor, and some other options.
   * @param[in] renderTask Target render task to change source actor and exclusiveness
   * @param[in] sourceView Input source view
   */
  void ApplyRenderTaskSourceActor(RenderTask sourceRenderTask, const Ui::View sourceView);

  /**
   * @brief Emits render finished signal of the effect,
   * when mBlurOnce is true and finished signal of the last render task(mVerticalBlurTask) is emitted.
   * @param[in] renderTask that emits source signal.
   */
  void OnRenderFinished(Dali::RenderTask& renderTask);

  /**
   * @brief Calculate valid downscale factor and blur radius by given mBlurRadius and mDownscaleFactor.
   * It will change internal values, downscaled blur radius, and skip blur
   */
  void UpdateDownscaledBlurRadius();

  BackgroundBlurEffectImpl(const BackgroundBlurEffectImpl&)            = delete;
  BackgroundBlurEffectImpl(BackgroundBlurEffectImpl&&)                 = delete;
  BackgroundBlurEffectImpl& operator=(BackgroundBlurEffectImpl&&)      = delete; // no move()
  BackgroundBlurEffectImpl& operator=(const BackgroundBlurEffectImpl&) = delete; // no copy()

public:
  Dali::Ui::BackgroundBlurEffect::FinishedSignalType mFinishedSignal; // Emits when blur once is enabled

private:
  // Camera actors
  CameraActor mCamera;
  CameraActor mRenderDownsampledCamera;

  // Resource
  FrameBuffer mInputBackgroundFrameBuffer; // Input. Background. What to blur.

  Actor       mInternalRoot;
  Actor       mHorizontalBlurActor;
  RenderTask  mHorizontalBlurTask;
  FrameBuffer mTemporaryFrameBuffer;
  Actor       mVerticalBlurActor;
  RenderTask  mVerticalBlurTask;

  FrameBuffer mBlurredOutputFrameBuffer;
  RenderTask  mSourceRenderTask;

  // Variables
  float    mDownscaleFactor;
  uint32_t mBlurRadius;

  Dali::WeakHandle<Dali::Actor> mUserSourceActor;  ///< Weakhandle of source actor from user.
  Dali::WeakHandle<Dali::Actor> mUserStopperActor; ///< Weakhandle of stopper actor from user.

  float    mInternalDownscaleFactor;
  uint32_t mInternalBlurRadius;

  uint32_t mDownscaledBlurRadius;

  bool mSkipBlur : 1;
  bool mBlurOnce : 1;
};
} // namespace Internal

inline Ui::Internal::BackgroundBlurEffectImpl& GetImplementation(Ui::BackgroundBlurEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Ui::Internal::BackgroundBlurEffectImpl&>(handle);
}

inline const Ui::Internal::BackgroundBlurEffectImpl& GetImplementation(const Ui::BackgroundBlurEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Ui::Internal::BackgroundBlurEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_BACKGROUND_BLUR_EFFECT_H
