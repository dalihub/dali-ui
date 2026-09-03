#ifndef DALI_UI_RENDER_EFFECT_H
#define DALI_UI_RENDER_EFFECT_H

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class RenderEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief
 * RenderEffect is an interface for visual effects.
 *
 * An effect is put on a View by Ui::View::SetRenderEffect() and taken off it by
 * Ui::View::ClearRenderEffect(). Each effect has a single owner View at a time; setting the same
 * effect on another View moves it, leaving the previous owner without an effect.
 *
 * @note RenderEffect is interface class without constructor. Create resource by subclass.
 */
class DALI_UI_API RenderEffect : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized effect.
   */
  RenderEffect() = default;

  /**
   * @brief Destructor.
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RenderEffect() = default;

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object.
   */
  RenderEffect(const RenderEffect& handle);

  /**
   * @brief Activates effect on owner View.
   *
   * @pre The effect must be set on a View by Ui::View::SetRenderEffect() beforehand. This call has
   * no effect while the effect has no owner View.
   *
   * @note Ui::View::SetRenderEffect() activates the effect on its own, so this is only needed to turn
   * an effect back on after Deactivate().
   * @note Activation is also skipped while the owner View is off-scene, not visible, or has zero size.
   * The effect is activated automatically once the View becomes visible again.
   */
  void Activate();

  /**
   * @brief Deactivates effect.
   *
   * The effect stays set on its owner View, so it can be turned back on by Activate().
   * To remove the effect from the View entirely, use Ui::View::ClearRenderEffect().
   */
  void Deactivate();

  /**
   * @brief Refreshes effect rendering.
   *
   * Re-reads the owner View's size and brings the effect back in line with it: the effect is
   * activated if it can be activated now but is not yet, deactivated if it can no longer be
   * activated, and otherwise its offscreen rendering is refreshed in place.
   *
   * @note This is called for you when the owner View is re-arranged, so an application rarely
   * needs it. It has no effect while the effect has no owner View.
   */
  void Refresh();

  /**
   * @brief Retrieves whether this effect is currently activated.
   * @return True if effect is activated. False otherwise.
   */
  bool IsActivated();

public: // Not intended for Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Ui::Internal implementation.
   * @param[in] renderEffectImpl The RenderEffect implementation.
   */
  explicit DALI_INTERNAL RenderEffect(Internal::RenderEffectImpl* renderEffectImpl);
  ///@endcond
};

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_RENDER_EFFECT_H
