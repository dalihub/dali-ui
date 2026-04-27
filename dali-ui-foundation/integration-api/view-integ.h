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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/view-state.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

class ViewImpl;

namespace IntegrationView
{

/**
 * @brief Container of child view handles (in add order).
 *
 * Per-child layout state (measured size, arranged bounds) is owned by the
 * child view itself (ViewImpl). Layout managers that need a mutable working
 * buffer (flex-basis/grow/shrink, stack weight distribution, etc.) allocate
 * one locally per Measure/Arrange call.
 */
using ChildContainer = Dali::Vector<Ui::View>;

/**
 * @brief Adds a raw (non-View) Actor as a child of the given View.
 *
 * View::OnChildAdd normally asserts when the added child cannot be DownCast to
 * Ui::View. This Integration helper bypasses that check so internal modules can
 * attach supporting Actors (clipping stencils, render-effect roots, decoration
 * layers, etc.) without violating the View-only invariant for application code.
 *
 * The added Actor is NOT tracked in the View's child container and therefore is
 * excluded from the View's measure/arrange layout pass. The caller is fully
 * responsible for the Actor's size, position, and lifecycle.
 *
 * @param[in] view  The parent View to add to.
 * @param[in] actor The raw Actor to attach as a child.
 */
DALI_UI_API void AddActorChild(Ui::View view, Dali::Actor actor);

/**
 * @brief Sets trait data on a View.
 *
 * If the stored object implements TraitInterface, lifecycle callbacks
 * (OnBeforeAttached, OnAttached, OnDetached, OnViewDestroying) are invoked.
 * Otherwise the object is stored without callbacks.
 *
 * @param[in] viewImpl The view implementation to attach to
 * @param[in] id The key to identify the trait
 * @param[in] handle The object to store
 */
DALI_UI_API void SetTrait(ViewImpl& viewImpl, TraitId id, Dali::BaseHandle handle);

/**
 * @brief Gets trait data from a View.
 *
 * @param[in] viewImpl The view implementation to query
 * @param[in] id The key to identify the trait
 * @return The stored handle, or an empty handle if not found
 */
DALI_UI_API Dali::BaseHandle GetTrait(const ViewImpl& viewImpl, TraitId id);

/**
 * @brief Gets trait data from a View and DownCasts to the specified handle type.
 *
 * @tparam HandleType A handle class that provides a static DownCast(BaseHandle) method
 * @param[in] viewImpl The view implementation to query
 * @param[in] id The key to identify the trait
 * @return The DownCast handle, or an empty handle if not found or cast fails
 */
template<typename HandleType>
HandleType GetTrait(const ViewImpl& viewImpl, TraitId id)
{
  return HandleType::DownCast(GetTrait(viewImpl, id));
}

/**
 * @brief Removes trait data from a View.
 *
 * @param[in] viewImpl The view implementation to modify
 * @param[in] id The key to identify the trait
 * @return true if the trait was found and removed
 */
DALI_UI_API bool RemoveTrait(ViewImpl& viewImpl, TraitId id);

// State management

/**
 * @brief Internal helper for WhenStateChanged templates.
 */
DALI_UI_API void SetNamedStateHandler(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);

/**
 * @brief Updates a state bit in the view's ViewState and emits StateChangedSignal.
 * @param[in] viewImpl The view implementation
 * @param[in] state The state to set or clear
 * @param[in] on    True to add the state, false to remove it
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState state, bool on, InputEvent cause = InputEvent::None());

/**
 * @brief Registers a named state-change handler using a member function.
 * @param[in] viewImpl The view implementation
 * @param[in] id   Unique identifier for this handler
 * @param[in] obj  Object whose member function will be called
 * @param[in] func Member function with signature void(View, const StateEvent&)
 */
template<class X>
void WhenStateChanged(ViewImpl& viewImpl, const Dali::String& id, X* obj, void (X::*func)(Ui::View, const StateEvent&))
{
  if(obj && func)
  {
    SetNamedStateHandler(viewImpl, id, obj, MakeCallback(obj, func));
  }
}

/**
 * @brief Registers a named state-change handler using a callable (e.g. lambda).
 * @param[in] viewImpl The view implementation
 * @param[in] id      Unique identifier for this handler
 * @param[in] tracker ConnectionTrackerInterface for automatic lifetime management
 * @param[in] func    Callable with signature void(View, const StateEvent&)
 */
template<typename F>
void WhenStateChanged(ViewImpl& viewImpl, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, F&& func)
{
  if(tracker)
  {
    SetNamedStateHandler(viewImpl, id, tracker, new CallbackFunctor2<std::decay_t<F>, Ui::View, const StateEvent&>(std::forward<F>(func)));
  }
}

/**
 * @brief Removes a named state-change handler.
 * @param[in] viewImpl The view implementation
 * @param[in] id The handler identifier to remove
 * @return True if a handler was found and removed
 */
DALI_UI_API bool UnsetStateHandler(ViewImpl& viewImpl, const Dali::String& id);

/**
 * @brief Removes a named state-change handler only if it is not currently being processed.
 * @param[in] viewImpl The view implementation
 * @param[in] id The handler identifier to remove
 * @return True if removed, false if currently processing or not found
 */
DALI_UI_API bool UnsetStateHandlerWhenNotProcessing(ViewImpl& viewImpl, const Dali::String& id);

// View-handle overloads for state management

/**
 * @copydoc SetState(ViewImpl&, ViewState, bool, InputEvent)
 */
DALI_UI_API void SetState(Ui::View view, ViewState state, bool on, InputEvent cause = InputEvent::None());

/**
 * @brief Internal helper for WhenStateChanged templates (View overload).
 */
DALI_UI_API void SetNamedStateHandler(Ui::View view, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);

/**
 * @brief Registers a named state-change handler using a member function.
 * @param[in] view The view handle
 * @param[in] id   Unique identifier for this handler
 * @param[in] obj  Object whose member function will be called
 * @param[in] func Member function with signature void(View, const StateEvent&)
 */
template<class X>
void WhenStateChanged(Ui::View view, const Dali::String& id, X* obj, void (X::*func)(Ui::View, const StateEvent&))
{
  if(obj && func)
  {
    SetNamedStateHandler(view, id, obj, MakeCallback(obj, func));
  }
}

/**
 * @brief Registers a named state-change handler using a callable (e.g. lambda).
 * @param[in] view    The view handle
 * @param[in] id      Unique identifier for this handler
 * @param[in] tracker ConnectionTrackerInterface for automatic lifetime management
 * @param[in] func    Callable with signature void(View, const StateEvent&)
 */
template<typename F>
void WhenStateChanged(Ui::View view, const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, F&& func)
{
  if(tracker)
  {
    SetNamedStateHandler(view, id, tracker, new CallbackFunctor2<std::decay_t<F>, Ui::View, const StateEvent&>(std::forward<F>(func)));
  }
}

/**
 * @copydoc UnsetStateHandler(ViewImpl&, const Dali::String&)
 */
DALI_UI_API bool UnsetStateHandler(Ui::View view, const Dali::String& id);

/**
 * @copydoc UnsetStateHandlerWhenNotProcessing(ViewImpl&, const Dali::String&)
 */
DALI_UI_API bool UnsetStateHandlerWhenNotProcessing(Ui::View view, const Dali::String& id);

// Visual property helpers

/**
 * @brief Returns a Property handle for animating a visual sub-property.
 *
 * Uses ViewDataImpl::GetVisualProperty internally. If the visual for the
 * given @a index has not been created yet, the returned Property has
 * Property::INVALID_INDEX.
 *
 * @param[in] view              The view to query.
 * @param[in] index             The property index that owns the visual (e.g. View::Property::BACKGROUND).
 * @param[in] visualPropertyKey The visual sub-property key (e.g. ColorVisual::Property::MIX_COLOR).
 * @return A Property targeting the visual sub-property, or an invalid Property.
 */
DALI_UI_API Dali::Property GetVisualProperty(Ui::View view, Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

// Layout helpers

/**
 * @brief Convenience: returns true if LayoutMode is Standalone.
 * @param[in] viewImpl The view implementation to query
 * @return True if the view's layout mode is Standalone
 */
DALI_UI_API bool IsLayoutModeStandalone(const ViewImpl& viewImpl);

/**
 * @brief Gets the children container from a view.
 * @param[in] viewImpl The view implementation
 * @return Reference to the children container
 */
DALI_UI_API ChildContainer& GetChildren(ViewImpl& viewImpl);

/**
 * @copydoc GetChildren(ViewImpl&)
 */
DALI_UI_API const ChildContainer& GetChildren(const ViewImpl& viewImpl);

/**
 * @brief Returns whether this view is a Layout container.
 * @param[in] viewImpl The view implementation to query
 * @return True if the view is a Layout (i.e. Ui::Layout::DownCast succeeds)
 */
DALI_UI_API bool IsLayout(ViewImpl& viewImpl);

} // namespace IntegrationView
} // namespace Ui
} // namespace Dali
