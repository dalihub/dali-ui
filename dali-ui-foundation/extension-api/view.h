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
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/signals/callback.h>

#include <type_traits>
#include <utility>

namespace Dali
{
namespace Ui
{
class ViewImpl;

namespace Extension
{
namespace View
{
namespace Internal
{
/**
 * @cond INTERNAL
 */
/**
 * @brief Registers or replaces a named observer using an owned callback.
 *
 * This function is an ABI bridge for the public SetNamedStateObserver()
 * templates and is not intended to be called directly.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] id Unique identifier for this observer
 * @param[in] tracker Connection tracker for automatic lifetime management
 * @param[in] callback Callback to invoke with void(View, const StateEvent&)
 */
DALI_UI_API void SetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);
/**
 * @endcond
 */
} // namespace Internal

/**
 * @brief Updates a state bit in the view's ViewState and emits StateChangedSignal.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] stateToChange The state to set or clear
 * @param[in] on True to add the state, false to remove it
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState stateToChange, bool on, InputEvent cause = InputEvent::Programmatic());

/**
 * @brief Clears and sets states as a single state change notification.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] statesToClear The states to clear
 * @param[in] statesToSet The states to set
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState statesToClear, ViewState statesToSet, InputEvent cause = InputEvent::Programmatic());

/**
 * @brief Registers or replaces a named state observer using a member function.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] id Unique identifier for this observer
 * @param[in] object Object whose member function will be called
 * @param[in] function Member function with signature void(View, const StateEvent&)
 * @warning Do not replace an observer from inside the callback registered with
 * the same @a id.
 */
template<class X>
void SetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id, X* object, void (X::*function)(Ui::View, const StateEvent&))
{
  if(object && function)
  {
    Internal::SetNamedStateObserver(viewImpl, id, object, MakeCallback(object, function));
  }
}

/**
 * @brief Registers or replaces a named state observer using a callable.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] id Unique identifier for this observer
 * @param[in] tracker Connection tracker for automatic lifetime management
 * @param[in] function Callable with signature void(View, const StateEvent&)
 * @warning Do not replace an observer from inside the callback registered with
 * the same @a id.
 */
template<typename F>
void SetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, F&& function)
{
  if(tracker)
  {
    Internal::SetNamedStateObserver(viewImpl, id, tracker, new CallbackFunctor2<std::decay_t<F>, Ui::View, const StateEvent&>(std::forward<F>(function)));
  }
}

/**
 * @brief Removes a named state observer.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] id The observer identifier to remove
 * @return True if the observer was found and removed
 * @warning Do not call this function from the callback registered with the
 * same @a id. Use UnsetNamedStateObserverIfNotExecuting() in code that may run
 * from that callback.
 */
DALI_UI_API bool UnsetNamedStateObserver(ViewImpl& viewImpl, const Dali::String& id);

/**
 * @brief Removes a named state observer unless that same observer's callback
 * is currently executing.
 *
 * This function may remove a different observer while state observers are
 * being dispatched. Only execution of the callback registered with the same
 * @a id prevents removal.
 *
 * @param[in] viewImpl The view implementation
 * @param[in] id The observer identifier to remove
 * @return True if the observer was removed; false if it was not found or its
 * callback is currently executing
 */
DALI_UI_API bool UnsetNamedStateObserverIfNotExecuting(ViewImpl& viewImpl, const Dali::String& id);

/**
 * @brief Sets a per-instance accessibility activation callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityActivate() for
 * accessibility activation requested through the View action. The callback
 * return value is the final action result; returning false does not fall back
 * to the virtual method. Pass an empty callback to restore virtual dispatch.
 *
 * The callback may replace or clear itself while it is executing. A callback
 * created from a member function does not extend the lifetime of its target
 * object, so the target must outlive the registration or clear it first.
 *
 * @param[in] view The View whose activation behavior is customized
 * @param[in] callback Callback invoked with the View being activated. Return
 * true if the accessibility activation was handled successfully; return false
 * otherwise. Pass an empty callback to restore virtual dispatch.
 */
DALI_UI_API void SetAccessibilityActivateCallback(Ui::View view, Callback<bool(Ui::View)> callback);

/**
 * @brief Sets a per-instance accessibility escape callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityEscape(). Its return
 * value is final. Pass an empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose escape behavior is customized
 * @param[in] callback Callback invoked with the View being dismissed
 */
DALI_UI_API void SetAccessibilityEscapeCallback(Ui::View view, Callback<bool(Ui::View)> callback);

/**
 * @brief Sets a per-instance accessibility pan callback.
 *
 * When dispatched through the View accessibility path, the callback replaces
 * ViewImpl::OnAccessibilityPan(). Its return value is final. Pass an empty
 * callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose pan behavior is customized
 * @param[in] callback Callback invoked with the View and pan gesture
 */
DALI_UI_API void SetAccessibilityPanCallback(Ui::View view, Callback<bool(Ui::View, PanGesture)> callback);

/**
 * @brief Sets a per-instance accessibility value-change callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityValueChange(). Its
 * return value is final. Pass an empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose value-change behavior is customized
 * @param[in] callback Callback invoked with the View and increase flag
 */
DALI_UI_API void SetAccessibilityValueChangeCallback(Ui::View view, Callback<bool(Ui::View, bool)> callback);

/**
 * @brief Sets a per-instance accessibility scroll-to-child callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityScrollToChild(). Its
 * return value is final. Pass an empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The scrollable View whose behavior is customized
 * @param[in] callback Callback invoked with the View and child to reveal
 */
DALI_UI_API void SetAccessibilityScrollToChildCallback(Ui::View view, Callback<bool(Ui::View, Ui::View)> callback);

/**
 * @brief Sets a per-instance accessibility zoom callback.
 *
 * When dispatched through the View accessibility path, the callback replaces
 * ViewImpl::OnAccessibilityZoom(). Its return value is final. Pass an empty
 * callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose zoom behavior is customized
 * @param[in] callback Callback invoked with the View being zoomed
 */
DALI_UI_API void SetAccessibilityZoomCallback(Ui::View view, Callback<bool(Ui::View)> callback);

/**
 * @brief Sets a per-instance dynamic accessibility name callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityRequestName(). A
 * true return accepts the output value, including an intentionally empty value;
 * false continues the existing framework fallback. Pass an empty callback to
 * restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose accessibility name is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestNameCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance default accessibility name callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityRequestDefaultName().
 * Its return value keeps the existing default-name fallback contract. Pass an
 * empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose default accessibility name is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDefaultNameCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance dynamic accessibility description callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityRequestDescription().
 * A true return accepts the output value, including an intentionally empty
 * value; false continues the existing framework fallback. Pass an empty
 * callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose accessibility description is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDescriptionCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance default accessibility description callback.
 *
 * When set, the callback replaces
 * ViewImpl::OnAccessibilityRequestDefaultDescription(). Its return value keeps
 * the existing default-description fallback contract. Pass an empty callback
 * to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose default accessibility description is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDefaultDescriptionCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance dynamic accessibility value callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityRequestValue(). A
 * true return accepts the output value, including an intentionally empty value;
 * false continues the existing framework fallback. Pass an empty callback to
 * restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @param[in] view The View whose accessibility value is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestValueCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Requests accessibility highlight for the given View.
 *
 * If another object is highlighted, the accessibility highlight moves to the
 * given View. When the highlight changes, accessibility clients are notified
 * of the highlighted state change.
 *
 * This request is intended for a View in a stable accessibility tree. A
 * concurrent accessibility context change, such as a newly showing page or
 * modal, may cause an accessibility client to choose a different highlight.
 *
 * @param[in] view The View to request accessibility highlight for
 * @return true if the View has the accessibility highlight, false otherwise
 */
DALI_UI_API bool GrabAccessibilityHighlight(Ui::View view);

/**
 * @brief Clears accessibility highlight from the given View.
 *
 * @param[in] view The View to clear accessibility highlight from
 * @return true if the accessibility highlight was cleared, false otherwise
 */
DALI_UI_API bool ClearAccessibilityHighlight(Ui::View view);

/**
 * @brief Explicitly notifies accessibility clients that a View's value changed.
 *
 * This can be used when the current value must be announced again without
 * changing the value stored on the View.
 *
 * @param[in] view The View whose value-change event should be emitted
 * @return True if the View had an accessible object to notify
 */
DALI_UI_API bool NotifyAccessibilityValueChanged(Ui::View view);

/**
 * @brief Explicitly notifies accessibility clients that a View's showing state changed.
 *
 * @param[in] view The View whose showing-state event should be emitted
 * @param[in] showing The new showing state
 * @return True if the View had an accessible object to notify
 */
DALI_UI_API bool NotifyAccessibilityShowingChanged(Ui::View view, bool showing);

/**
 * @brief Sets the rendered X position of the view.
 *
 * The raw Dali::Actor geometry setters (SetPosition, SetSize, SetPositionX/Y/Z,
 * SetWidth, SetHeight, SetDepth) are deleted on the public View handle because a
 * View's rendered geometry is owned by the layout system. These extension-api
 * free functions give custom-view / component authors a sanctioned way to drive
 * the rendered position and size of a View handle directly.
 *
 * @param[in] view The view to modify
 * @param[in] x The rendered X position
 * @warning Writes the Actor render property POSITION_X directly and bypasses the
 * layout request; for a layout-managed child the next Arrange pass overwrites it.
 * Use View::SetRequestedX/Y and View::SetRequestedWidth/Height for
 * layout-aware placement and sizing.
 */
DALI_UI_API void SetPositionX(Ui::View view, float x);

/**
 * @brief Sets the rendered Y position of the view.
 *
 * @param[in] view The view to modify
 * @param[in] y The rendered Y position
 * @warning Writes the Actor render property POSITION_Y directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetPositionY(Ui::View view, float y);

/**
 * @brief Sets the rendered width of the view.
 *
 * @param[in] view The view to modify
 * @param[in] width The rendered width
 * @warning Writes the Actor render property SIZE_WIDTH directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeWidth(Ui::View view, float width);

/**
 * @brief Sets the rendered height of the view.
 *
 * @param[in] view The view to modify
 * @param[in] height The rendered height
 * @warning Writes the Actor render property SIZE_HEIGHT directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeHeight(Ui::View view, float height);

} // namespace View
} // namespace Extension
} // namespace Ui
} // namespace Dali
