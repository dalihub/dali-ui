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
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class ViewStateManager;
}

namespace Integration
{

/**
 * @brief Base implementation for effects that react to ViewState changes.
 */
class DALI_UI_API StateEffectImpl : public TraitObject
{
protected:
  StateEffectImpl();
  ~StateEffectImpl() override;

  /**
   * @brief Called when the attached View state changes.
   *
   * @param[in] view The View whose state changed
   * @param[in] event The state transition event
   */
  virtual void OnViewStateChanged(View view, const StateEvent& event);

  /**
   * @brief Called when the attached View receives an interactive trait.
   *
   * State effects may be attached before a View has an interactive trait. This
   * hook lets the effect finish interactive-dependent setup without requiring a
   * detach/attach cycle.
   *
   * @param[in] view The View that became interactive
   */
  virtual void OnInteractiveAttached(View view);

  /**
   * @brief Called when the attached View's state effect targets change.
   *
   * @param[in] view The View whose state effect targets changed
   */
  virtual void OnStateEffectTargetsChanged(View view);

  /**
   * @brief Returns whether this effect suppresses FocusManager's default focus indicator for the given owner View.
   *
   * The framework may cache this result. This method is not guaranteed to be
   * called each time FocusManager decides whether to show the default focus
   * indicator, so implementations must not rely on it being queried from the
   * focus hot path.
   *
   * The returned value should describe the current state of this effect for the
   * given owner View and should not mutate observable state. If the result can
   * change after this effect is attached, the implementation must call
   * InvalidateDefaultFocusIndicatorSuppression() when that change may have
   * happened.
   *
   * Examples of values that require invalidation include mutable effect options,
   * View state, StateEffect target changes, child add/remove signals, or other
   * external state observed by the effect. Stable values such as the owner View
   * type normally do not require invalidation after attachment.
   *
   * @param[in] view The owner View this effect is attached to
   * @return True if FocusManager's default focus indicator should be suppressed
   */
  virtual bool ShouldSuppressDefaultFocusIndicator(View view) const;

  /**
   * @brief Invalidates the cached default focus indicator suppression result for an attached View.
   *
   * Call this when the result of ShouldSuppressDefaultFocusIndicator() may have
   * changed after this effect was attached to @p view. If this effect is no
   * longer the StateEffect attached to @p view, this call is ignored.
   *
   * This may refresh FocusManager's default focus indicator if @p view is the
   * current focused View and the global focus indicator state allows the default
   * indicator to be shown.
   *
   * @param[in] view The owner View whose cached suppression result may be stale
   */
  void InvalidateDefaultFocusIndicatorSuppression(View view);

private:
  friend class Internal::ViewStateManager;
  friend class Dali::Ui::ViewImpl;
};

} // namespace Integration

inline DALI_UI_API Integration::StateEffectImpl& GetImpl(StateEffect& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::StateEffectImpl&>(handle);
}

inline DALI_UI_API const Integration::StateEffectImpl& GetImpl(const StateEffect& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::StateEffectImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
