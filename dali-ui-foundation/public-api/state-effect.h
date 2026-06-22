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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class StateEffectImpl;
}

/**
 * @brief Base handle for effects that react to ViewState changes.
 */
class DALI_UI_API StateEffect : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized StateEffect handle.
   */
  StateEffect() = default;

  /**
   * @brief Downcasts a handle to StateEffect.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized StateEffect on success, otherwise empty
   */
  static StateEffect DownCast(BaseHandle handle);

  /**
   * @brief Creates a no-op StateEffect.
   *
   * Use this to explicitly disable state effects on a View or to indicate that
   * no default state effect should be applied.
   *
   * @return A StateEffect that does not react to state changes
   */
  static StateEffect None();

  /**
   * @brief Gets the current default StateEffect for Views that become interactive.
   *
   * This returns UiConfig::GetDefaultStateEffectForInteractive() from the
   * current UiConfig.
   *
   * @pre UiConfig::Apply() must have been called.
   * @return The default StateEffect for interactive Views
   */
  static StateEffect DefaultForInteractive();

  /**
   * @brief Returns whether this effect is a no-op effect created by StateEffect::None().
   *
   * @return True if this effect is StateEffect::None(), otherwise false
   */
  bool IsNone() const;

protected:
  /**
   * @brief Creates a StateEffect handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit StateEffect(Integration::StateEffectImpl* impl);
};

} // namespace Ui
} // namespace Dali
