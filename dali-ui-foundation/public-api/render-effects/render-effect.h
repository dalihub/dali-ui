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
 * Each effect has a single owner View.
 *
 * Used internal at:
 * Ui::View::SetRenderEffect(Ui::RenderEffect effect);
 * Ui::View::ClearRenderEffect();
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
   * @brief Activates effect on owner View
   */
  void Activate();

  /**
   * @brief Deactivates effect
   */
  void Deactivate();

  /**
   * @brief Refreshes effect rendering
   */
  void Refresh();

  /**
   * @brief Get whether this effect activated or not.
   * @return True if effect is activated. False otherwise.
   */
  bool IsActivated();

public: // Not intended for Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Ui::Internal implementation.
   * @param[in] renderEffectImpl The RenderEffect 4implementation.
   */
  explicit DALI_INTERNAL RenderEffect(Internal::RenderEffectImpl* renderEffectImpl);
  ///@endcond
};

} // namespace Ui
} // namespace Dali

#endif // DALI_UI_RENDER_EFFECT_H
