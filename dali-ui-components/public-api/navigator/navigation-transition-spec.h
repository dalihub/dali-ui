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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class NavigationTransitionSpecImpl;
}

/**
 * @brief Navigation transition animation specification.
 *
 * A specification may be installed as the Navigator-wide default, a modal
 * transition default, or a per-page override. Animator signal callbacks
 * receive the animation created by Navigator and append property animations
 * to it. They must not Play(), Stop(), or change the duration of the supplied
 * animation. Transition callbacks must not mutate the Navigator's page or modal
 * stacks; re-entrant navigation requests are ignored while callbacks run.
 *
 * The handle is reference counted. Copies share the same callbacks and
 * duration.
 */
class DALI_UI_COMPONENTS_API NavigationTransitionSpec : public BaseHandle
{
public:
  /**
   * @brief Signal used to append animation steps for one view.
   *
   * Parameters are the animation created by Navigator and the view to animate.
   */
  using AnimatorSignalType = Signal<void(Dali::Animation&, Ui::View)>;

  /**
   * @brief Signal used to restore one view to its final resting state.
   */
  using SnapSignalType = Signal<void(Ui::View)>;

  /**
   * @brief Creates an uninitialized handle.
   */
  NavigationTransitionSpec();

  /**
   * @brief Creates an initialized transition specification.
   * @return A new transition specification
   */
  static NavigationTransitionSpec New();

  NavigationTransitionSpec(const NavigationTransitionSpec& other);
  NavigationTransitionSpec(NavigationTransitionSpec&& rhs) noexcept;
  ~NavigationTransitionSpec();
  NavigationTransitionSpec& operator=(const NavigationTransitionSpec& other);
  NavigationTransitionSpec& operator=(NavigationTransitionSpec&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to NavigationTransitionSpec.
   * @param[in] handle The handle to downcast
   * @return A transition specification, or an uninitialized handle
   */
  static NavigationTransitionSpec DownCast(BaseHandle handle);

  /// @brief Incoming view for Push or PushModal.
  AnimatorSignalType& EnterSignal();
  /// @brief Outgoing view for Push or PushModal.
  AnimatorSignalType& ExitSignal();
  /// @brief Incoming/revealed view for Pop or PopModal.
  AnimatorSignalType& PopEnterSignal();
  /// @brief Outgoing/removed view for Pop or PopModal.
  AnimatorSignalType& PopExitSignal();

  /**
   * @brief Snaps an incoming view to its visible final state.
   *
   * If no callback is connected, Navigator sets opacity to 1.0f.
   */
  SnapSignalType& SnapIncomingSignal();

  /**
   * @brief Snaps an outgoing view that remains in the stack to its resting state.
   *
   * If no callback is connected, Navigator leaves the outgoing view untouched.
   */
  SnapSignalType& SnapOutgoingSignal();

  /**
   * @brief Sets the transition duration in seconds.
   *
   * A value less than or equal to 0.0f uses Navigator's default duration.
   * @param[in] duration The duration in seconds
   */
  void SetDuration(float duration);

  /**
   * @brief Gets the transition duration in seconds.
   * @return The configured duration, or 0.0f when Navigator's default is used
   */
  float GetDuration() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL NavigationTransitionSpec(Integration::NavigationTransitionSpecImpl* implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
