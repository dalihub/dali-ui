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

#ifndef DALI_UI_FOUNDATION_EDGE_EFFECT_H
#define DALI_UI_FOUNDATION_EDGE_EFFECT_H

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Integration
{
class EdgeEffectImpl;
}

/**
 * @brief Controls a visual effect displayed when scrollable content reaches its boundary.
 *
 * EdgeEffect implements a 5-state machine (IDLE → PULL → RECEDE → IDLE and
 * IDLE → ABSORB → RECEDE → IDLE) driven by four control methods:
 *   - OnPull()    — called while the user drags past the edge.
 *   - OnRelease() — called when the user lifts their finger after a drag.
 *   - OnAbsorb()  — called when a fling reaches the edge.
 *   - Finish()    — forces the effect back to IDLE immediately.
 *
 * Each method emits a corresponding signal. Integration implementers can
 * derive custom behaviours from EdgeEffectImpl in integration-api.
 *
 * EdgeEffect is a value type / handle: copying shares the underlying implementation.
 * An uninitialized handle (default-constructed) evaluates to false.
 *
 * Typical usage:
 * @code
 * auto effect = BounceEdgeEffect::New(ScrollDirection::Vertical);
 * effect.SetSource(scrollContent);
 * scrollView.SetStartEdgeEffect(effect);
 * @endcode
 * @SINCE_3_0.0
 */
class DALI_UI_API EdgeEffect : public Dali::BaseHandle
{
public:
  enum class State
  {
    IDLE,
    PULL,
    ABSORB,
    RECEDE,
    PULLDECAY
  };

  using PullSignalType     = Dali::Signal<void(float, float)>; ///< (deltaDistance, displacement)
  using ReleaseSignalType  = Dali::Signal<void()>;
  using AbsorbSignalType   = Dali::Signal<void(float)>; ///< (velocity)
  using FinishedSignalType = Dali::Signal<void()>;

public:
  /**
   * @brief Creates an uninitialized EdgeEffect handle (evaluates to false).
   * @SINCE_3_0.0
   */
  EdgeEffect() = default;

  /**
   * @brief Creates an initialized default EdgeEffect with no visual behaviour.
   * @SINCE_3_0.0
   */
  static EdgeEffect New();

  ~EdgeEffect() = default;

  EdgeEffect(const EdgeEffect&)            = default;
  EdgeEffect(EdgeEffect&&)                 = default;
  EdgeEffect& operator=(const EdgeEffect&) = default;
  EdgeEffect& operator=(EdgeEffect&&)      = default;

  /**
   * @brief Sets the source view whose visual position this effect manipulates.
   * @SINCE_3_0.0
   */
  void SetSource(View source);

  /**
   * @brief Gets the source view.
   * @SINCE_3_0.0
   */
  View GetSource() const;

  /**
   * @brief Returns the current state.
   * @SINCE_3_0.0
   */
  State GetState() const;

  /**
   * @brief Called while the user drags past the edge.
   *
   * @SINCE_3_0.0
   * @param[in] deltaDistance  Incremental drag distance (px) since last call.
   * @param[in] displacement   Total displacement from the edge boundary (px).
   */
  void OnPull(float deltaDistance, float displacement);

  /**
   * @brief Called when the user lifts their finger after a drag.
   * @SINCE_3_0.0
   */
  void OnRelease();

  /**
   * @brief Called when a fling reaches the edge.
   *
   * A keyboard edge bump also calls it, and so emits AbsorbSignal(), with a synthetic velocity
   * that follows the scroll view's effective scale.
   *
   * @SINCE_3_0.0
   * @param[in] velocity  Velocity at impact (px/ms).
   */
  void OnAbsorb(float velocity);

  /**
   * @brief Forces the effect back to IDLE immediately.
   * @SINCE_3_0.0
   */
  void Finish();

  /**
   * @brief Signal emitted each time OnPull() is processed.
   * @SINCE_3_0.0
   */
  PullSignalType& PullSignal();

  /**
   * @brief Signal emitted each time OnRelease() is processed.
   * @SINCE_3_0.0
   */
  ReleaseSignalType& ReleaseSignal();

  /**
   * @brief Signal emitted each time OnAbsorb() is processed.
   * @SINCE_3_0.0
   */
  AbsorbSignalType& AbsorbSignal();

  /**
   * @brief Signal emitted each time Finish() is called.
   * @SINCE_3_0.0
   */
  FinishedSignalType& FinishedSignal();

public: // Not intended for application developers
  /// @cond internal
  explicit EdgeEffect(Integration::EdgeEffectImpl* impl);

  Integration::EdgeEffectImpl&       GetImpl();
  const Integration::EdgeEffectImpl& GetImpl() const;
  /// @endcond
};

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_FOUNDATION_EDGE_EFFECT_H
