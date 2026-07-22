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
 * @param[in] state The state to set or clear
 * @param[in] on True to add the state, false to remove it
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState state, bool on, InputEvent cause = InputEvent::Programmatic());

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
DALI_UI_API bool GrabAccessibilityHighlight(View view);

/**
 * @brief Clears accessibility highlight from the given View.
 *
 * @param[in] view The View to clear accessibility highlight from
 * @return true if the accessibility highlight was cleared, false otherwise
 */
DALI_UI_API bool ClearAccessibilityHighlight(View view);

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
DALI_UI_API void SetPositionX(View view, float x);

/**
 * @brief Sets the rendered Y position of the view.
 *
 * @param[in] view The view to modify
 * @param[in] y The rendered Y position
 * @warning Writes the Actor render property POSITION_Y directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetPositionY(View view, float y);

/**
 * @brief Sets the rendered width of the view.
 *
 * @param[in] view The view to modify
 * @param[in] width The rendered width
 * @warning Writes the Actor render property SIZE_WIDTH directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeWidth(View view, float width);

/**
 * @brief Sets the rendered height of the view.
 *
 * @param[in] view The view to modify
 * @param[in] height The rendered height
 * @warning Writes the Actor render property SIZE_HEIGHT directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeHeight(View view, float height);

} // namespace Extension
} // namespace Ui
} // namespace Dali
