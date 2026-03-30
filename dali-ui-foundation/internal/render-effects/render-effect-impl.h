#ifndef DALI_UI_INTERNAL_RENDER_EFFECT_H
#define DALI_UI_INTERNAL_RENDER_EFFECT_H

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

// EXTERNAL INCLUDE
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/signals/connection-tracker.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
class RenderEffectImpl;

namespace Internal
{
using RenderEffectImplPtr = IntrusivePtr<RenderEffectImpl>;

class RenderEffectImpl : public BaseObject, public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::Ui::RenderEffect::Activate
   */
  void Activate();

  /**
   * @copydoc Dali::Ui::RenderEffect::Deactivate
   */
  void Deactivate();

  /**
   * @copydoc Dali::Ui::RenderEffect::Refresh
   */
  void Refresh();

  /**
   * @copydoc Dali::Ui::RenderEffect::IsActivated
   */
  bool IsActivated() const;

public:
  /**
   * @brief Gets the value of render pass tag for render effects.
   * @return The render pass tag for render effects.
   */
  static uint32_t GetRenderPassTag();

  /**
   * @brief Sets owner View. Applies effect on the owner.
   * @note Activates render effect on default.
   * @param[in] view The owner view to apply RenderEffect.
   */
  void SetOwnerView(Ui::View view);

  /**
   * @brief Clears owner View.
   */
  void ClearOwnerView();

  /**
   * @brief Set shader constants of target renderer. Without an explicit call, it will pull off BACKGROUND property of
   * the owner view.
   * @param[in] map may include corner radius, corner radius policy, and squareness
   */
  void SetCornerConstants(const Property::Map& map);

  /**
   * @brief Retrieves OffScreenRenderableType of this RenderEffect.
   *
   * @return OffScreenRenderableType for this RenderEffect.
   */
  virtual OffScreenRenderable::Type GetOffScreenRenderableType() const = 0;

  /**
   * @brief Retrieves the off-screen RenderTasks associated with the RenderEffect.
   * This method returns the internal RenderTasks held by the RenderEffect. This tasks are
   * used for off-screen rendering, and the system will assign order index to each
   * tasks based on the render order.
   *
   * RenderEffect with a non-NONE OffScreenRenderableType should override this method to
   * provide their render tasks.
   *
   * @param[out] tasks A list of RenderTasks to be populated with the RenderEffect's forward
   * or backward off-screen RenderTask.
   * @param[in] isForward Indicates whether to retrieve forward (true) or backward (false)
   * RenderTasks.
   */
  virtual void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) = 0;

protected:
  /**
   * @copydoc Dali::Ui::RenderEffect::RenderEffect
   */
  RenderEffectImpl();

  /**
   * @copydoc Dali::Ui::RenderEffect::~RenderEffect
   */
  virtual ~RenderEffectImpl() override;

  RenderEffectImpl(const RenderEffectImpl&)            = delete;
  RenderEffectImpl(RenderEffectImpl&&)                 = delete;
  RenderEffectImpl& operator=(RenderEffectImpl&&)      = delete; // no move()
  RenderEffectImpl& operator=(const RenderEffectImpl&) = delete; // no copy()

  /**
   * @brief Second-phase Initialization
   */
  void Initialize();

  /**
   * @brief Get target renderer
   * On internal Activate(), the renderer draws our visual effect and is added to our Owner view.
   * @return mRenderer
   */
  Renderer GetTargetRenderer() const;

  /**
   * @brief The final size of the owner after resizing or relayouts.
   * @return mTargetSize
   */
  Vector2 GetTargetSize() const;

  /**
   * @brief Get Owner view. It could be return empty handle if owner view is not set, or destroyed.
   * @return mOwnerView
   */
  Ui::View GetOwnerView() const;

  /**
   * @brief Get scene holder of owner view.
   * @return mPlacementSceneHolder
   */
  Dali::Integration::SceneHolder GetSceneHolder() const;

  /// For sub classes
protected:
  /**
   * @brief Initializes sub classes effect
   */
  virtual void OnInitialize() = 0;

  /**
   * @brief Activates sub classes effect on ownerView
   */
  virtual void OnActivate() = 0;

  /**
   * @brief Deactivates sub classes effect
   */
  virtual void OnDeactivate() = 0;

  /**
   * @brief Redraws effect without deactivation
   * @note This activates effect if not activated yet.
   */
  virtual void OnRefresh() = 0;

private:
  /**
   * @brief Check whether it is possible to activate effect or not.
   *        It will check various status, e.g. the view's visibility.
   * @note This API don't consider mIsActivated
   */
  bool IsActivateValid() const;

  /**
   * @brief Calculates a valid target size for texture.
   * Invalid cases include: zero vector, minus numbers or large numbers(larger than the maximum).
   */
  void UpdateTargetSize();

  /**
   * @brief Callback when the visibility of the actor is changed.
   * @param[in] actor The actor
   * @param[in] visible Whether this actor is visible or not.
   */
  void OnViewInheritedVisibilityChanged(Actor actor, bool visible);

private:
  Dali::Renderer mRenderer; // An additional renderer for mOwnerView

  std::vector<Constraint> mAnimationConstraints; // For corner animation on owner view.

  Dali::WeakHandle<Dali::Ui::View>           mOwnerView;            ///< Weakhandle of owner view.
  WeakHandle<Dali::Integration::SceneHolder> mPlacementSceneHolder; ///< Weakhandle of scene

  Vector2 mTargetSize; // The final size of mOwnerView

  bool mIsActivated : 1;
};
} // namespace Internal

inline Ui::Internal::RenderEffectImpl& GetImplementation(Ui::RenderEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Ui::Internal::RenderEffectImpl&>(handle);
}

inline const Ui::Internal::RenderEffectImpl& GetImplementation(const Ui::RenderEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Ui::Internal::RenderEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
#endif // DALI_UI_INTERNAL_RENDER_EFFECT_H
