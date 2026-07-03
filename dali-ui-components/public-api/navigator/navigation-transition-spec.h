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
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Navigation transition animation specification.
 *
 * A specification may be installed as the Navigator-wide default, a modal
 * transition default, or a per-page override. Each factory receives the
 * animation created by Navigator and appends property animations to it.
 * Factories must not Play(), Stop(), or change the duration of the supplied
 * animation.
 */
struct NavigationTransitionSpec
{
  /**
   * @brief Adds animation steps for one view.
   * @param[in,out] anim The animation created by Navigator
   * @param[in] view The view to animate
   */
  using AnimFactory = std::function<void(Dali::Animation& anim, Ui::View view)>;

  /**
   * @brief Restores a view to its final resting state after a transition.
   * @param[in] view The view to snap
   */
  using SnapFunction = std::function<void(Ui::View view)>;

  AnimFactory enter;    ///< Incoming view for Push or PushModal.
  AnimFactory exit;     ///< Outgoing view for Push or PushModal.
  AnimFactory popEnter; ///< Incoming/revealed view for Pop or PopModal.
  AnimFactory popExit;  ///< Outgoing/removed view for Pop or PopModal.

  /**
   * @brief Snaps an incoming view to its visible final state.
   *
   * If omitted, Navigator sets opacity to 1.0f.
   */
  SnapFunction snapIncoming;

  /**
   * @brief Snaps an outgoing view that remains in the stack to its resting state.
   *
   * If omitted, Navigator leaves the outgoing view untouched.
   */
  SnapFunction snapOutgoing;

  /**
   * @brief Transition duration in seconds.
   *
   * A value of 0.0f uses Navigator's default transition duration.
   */
  float duration{0.0f};
};

} // namespace Ui
} // namespace Dali
