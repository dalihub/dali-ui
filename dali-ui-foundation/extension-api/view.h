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
#include <dali-ui-foundation/extension-api/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/views/view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/signals/callback.h>

#include <type_traits>
#include <utility>

namespace DALI_NAMESPACE
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
 * @SINCE_3_0.0
 * @param[in] viewImpl The view implementation
 * @param[in] stateToChange The state to set or clear
 * @param[in] on True to add the state, false to remove it
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState stateToChange, bool on, InputEvent cause = InputEvent::Programmatic());

/**
 * @brief Clears and sets states as a single state change notification.
 *
 * @SINCE_3_0.0
 * @param[in] viewImpl The view implementation
 * @param[in] statesToClear The states to clear
 * @param[in] statesToSet The states to set
 * @param[in] cause Input event that triggered the change; leave default if programmatic
 */
DALI_UI_API void SetState(ViewImpl& viewImpl, ViewState statesToClear, ViewState statesToSet, InputEvent cause = InputEvent::Programmatic());

/**
 * @brief Registers or replaces a named state observer using a member function.
 *
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
 * @param[in] view The View whose zoom behavior is customized
 * @param[in] callback Callback invoked with the View being zoomed
 */
DALI_UI_API void SetAccessibilityZoomCallback(Ui::View view, Callback<bool(Ui::View)> callback);

/**
 * @brief Sets a per-instance default accessibility name callback.
 *
 * When set, the callback replaces ViewImpl::OnAccessibilityRequestDefaultName().
 * Its return value keeps the existing default-name fallback contract. Pass an
 * empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @SINCE_3_0.0
 * @param[in] view The View whose default accessibility name is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDefaultNameCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance default accessibility description callback.
 *
 * When set, the callback replaces
 * ViewImpl::OnAccessibilityRequestDefaultDescription(). Its return value keeps
 * the existing default-description fallback contract. Pass an empty callback
 * to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @SINCE_3_0.0
 * @param[in] view The View whose default accessibility description is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDefaultDescriptionCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

/**
 * @brief Sets a per-instance default accessibility value callback.
 *
 * The callback is called only when the deprecated request-value hook did not
 * handle the request and the explicit accessibility value is empty.
 * When set, it replaces ViewImpl::OnAccessibilityRequestDefaultValue().
 * Returning true accepts the output, including an intentionally empty value.
 * Returning false leaves the accessible value empty; it does not call the
 * virtual hook. Pass an empty callback to restore virtual dispatch.
 * The callback follows the lifetime rules of SetAccessibilityActivateCallback().
 *
 * @SINCE_3_0.0
 * @param[in] view The View whose default accessibility value is customized
 * @param[in] callback Callback invoked with the View and output value
 */
DALI_UI_API void SetAccessibilityRequestDefaultValueCallback(Ui::View view, Callback<bool(Ui::View, Dali::String&)> callback);

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
 * @SINCE_3_0.0
 * @param[in] view The View to request accessibility highlight for
 * @return true if the View has the accessibility highlight, false otherwise
 */
DALI_UI_API bool GrabAccessibilityHighlight(Ui::View view);

/**
 * @brief Clears accessibility highlight from the given View.
 *
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
 * @param[in] view The View whose value-change event should be emitted
 * @return True if the View had an accessible object to notify
 */
DALI_UI_API bool NotifyAccessibilityValueChanged(Ui::View view);

/**
 * @brief Explicitly notifies accessibility clients that a View's showing state changed.
 *
 * @SINCE_3_0.0
 * @param[in] view The View whose showing-state event should be emitted
 * @param[in] showing The new showing state
 * @return True if the View had an accessible object to notify
 */
DALI_UI_API bool NotifyAccessibilityShowingChanged(Ui::View view, bool showing);

/**
 * @brief Sets the rendered X position of the view.
 *
 * The raw Dali::Actor geometry setters (SetPosition, SetSize, SetPositionX/Y/Z,
 * SetSizeWidth, SetSizeHeight, SetSizeDepth) are deleted on the public View
 * handle because a View's rendered geometry is owned by the layout system.
 * These extension-api free functions give custom-view / component authors a
 * sanctioned way to drive the rendered position and size of a View handle
 * directly.
 *
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
 * @param[in] view The view to modify
 * @param[in] y The rendered Y position
 * @warning Writes the Actor render property POSITION_Y directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetPositionY(Ui::View view, float y);

/**
 * @brief Sets the rendered Z position of the view.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to modify
 * @param[in] z The rendered Z position
 * @warning Writes the Actor render property POSITION_Z directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetPositionZ(Ui::View view, float z);

/**
 * @brief Sets the rendered width of the view.
 *
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
 * @param[in] view The view to modify
 * @param[in] height The rendered height
 * @warning Writes the Actor render property SIZE_HEIGHT directly and bypasses
 * layout.
 * @see SetPositionX
 */
DALI_UI_API void SetSizeHeight(Ui::View view, float height);

/**
 * @brief Sets the rendered depth of the view.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to modify
 * @param[in] depth The rendered depth
 * @warning Writes the Actor render property SIZE_DEPTH directly and bypasses
 * layout.
 * @see SetSizeWidth
 */
DALI_UI_API void SetSizeDepth(Ui::View view, float depth);

/**
 * @brief Sets how the view takes part in the UI scale.
 *
 * The policy selects the UI-scale term of the view's effective scale: the term of its
 * parent (Extension::UiScalePolicy::INHERIT, the default; a root takes the UI scale), the UI
 * scale Extension::LayoutScaleManager holds (Extension::UiScalePolicy::ENABLED), or 1.0
 * (Extension::UiScalePolicy::DISABLED, which descendant INHERIT views follow). The window
 * scale multiplies whichever term the policy selects, so it also applies under DISABLED. A
 * new policy drops the cached effective scale of the view's subtree and requests a re-layout;
 * setting the current policy again does nothing.
 *
 * This replaces the deprecated Ui::View::SetUiScalePolicy() and ViewImpl::SetUiScalePolicy(),
 * which take the deprecated Ui::UiScalePolicy: pass the Extension::UiScalePolicy enumerator of
 * the same name, or static_cast a Ui::UiScalePolicy value. Like the rest of the scale
 * configuration, it is an extension API for platform integrators and module providers, and
 * not part of the application-facing public API.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to configure; it must not be empty
 * @param[in] policy The policy to apply
 */
DALI_UI_API void SetUiScalePolicy(Ui::View view, Extension::UiScalePolicy policy);

/**
 * @brief Returns how the view takes part in the UI scale.
 *
 * This replaces the deprecated Ui::View::GetUiScalePolicy() and ViewImpl::GetUiScalePolicy(),
 * which return the same policy as a deprecated Ui::UiScalePolicy.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to query; it must not be empty
 * @return The policy set through SetUiScalePolicy(); Extension::UiScalePolicy::INHERIT by default
 */
DALI_UI_API Extension::UiScalePolicy GetUiScalePolicy(Ui::View view);

/**
 * @brief Returns the effective layout scale of the view.
 *
 * The factor the layout pass multiplies the view's natural lengths by: the window scale
 * of the view's Window times the UI-scale term its UiScalePolicy selects, as
 * Extension::LayoutScaleManager describes; 1.0 while scaling is disabled through
 * Extension::LayoutScaleManager::SetScalable(false). A view in no Window has no window
 * term. The value is computed lazily and cached while the view is on a scene; off a scene
 * it is computed on every call, so it always reflects the current UI scale, switch and
 * window.
 *
 * A custom view multiplies each natural length it supplies -- a size, offset, radius,
 * thickness or spacing -- by this value exactly once; the constraints OnMeasure() receives
 * and the bounds OnArrange() receives already include it. Inside a ViewImpl subclass call
 * it as Ui::Extension::View::GetEffectiveLayoutScale(*this): there an unqualified
 * Extension:: names CustomActorImpl::Extension.
 *
 * This replaces the deprecated ViewImpl::GetEffectiveScale(). It is an extension API for
 * platform integrators and module providers, and not part of the application-facing
 * public API.
 *
 * @SINCE_3_0.0
 * @param[in] viewImpl The view implementation
 * @return The effective layout scale factor
 */
DALI_UI_API float GetEffectiveLayoutScale(const ViewImpl& viewImpl);

/**
 * @brief Raises the view one step above its next sibling without changing its parent's layout
 * order.
 *
 * A Ui::View keeps two orders of its children. The actor order (Dali::Actor::GetChildAt(),
 * Dali::DevelActor::Property::SIBLING_ORDER) decides the draw and hit-test order. The layout
 * order (Ui::View::GetChildViewAt(), Ui::View::IndexOfChildView()) is the order in which the
 * parent's layout measures and arranges its children. A reorder made through the inherited
 * Dali::Actor calls changes the actor order and makes the layout order follow it. This function
 * changes the actor order exactly like Dali::Actor::Raise() and leaves the layout order as it is,
 * so it requests no measure or layout pass. When the parent is not a Ui::View it is the same as
 * Dali::Actor::Raise(); without a parent it does nothing, like Dali::Actor::Raise().
 *
 * The layout order keeps this state until the next call on the same parent that makes it follow
 * the actor order again. That call undoes the effect of every earlier ...PreservingLayout() call
 * on the parent. When it changes the layout order, the parent re-measures; with a
 * LayoutTransition attached to the parent itself, the CHANGE transitions of the children that
 * move then report LayoutChangeCause::REORDERED, while a newly inserted child keeps its ENTER
 * handling. These calls make the layout order follow the actor order:
 * - Dali::Actor::Raise(), Lower(), RaiseToTop(), LowerToBottom(), RaiseAbove() and LowerBelow()
 *   on a child, and a write of Dali::DevelActor::Property::SIBLING_ORDER, whenever dali-core
 *   reports them through Dali::DevelActor::ChildOrderChangedSignal(). RaiseAbove() and
 *   LowerBelow() report whenever the target is a sibling and the child is not already the
 *   top-most (bottom-most) child, even when the child does not move.
 * - Dali::Actor::InsertAbove() and InsertBelow() of a child the parent already has, relative to
 *   another child of the parent. They always report, even when the child stays where it is, so
 *   re-inserting a child at its current position makes the layout order follow the actor order.
 * - Dali::Actor::InsertAbove() and InsertBelow() of a child the parent does not have yet (a new
 *   view, or one taken from another parent), when the child does not end up as the last child.
 *   This also applies to such an insert made from a Dali::DevelActor::ChildOrderChangedSignal()
 *   callback while a ...PreservingLayout() call on one of the parent's children is running, which
 *   undoes that call's effect at once.
 * These calls leave the layout order as it is:
 * - Raise(), RaiseToTop() and RaiseAbove() on the top-most child, Lower(), LowerToBottom() and
 *   LowerBelow() on the bottom-most child, RaiseAbove() and LowerBelow() with a target that is not
 *   a sibling, and InsertAbove() and InsertBelow() with a target that is not a child of the
 *   parent: dali-core changes and reports nothing.
 * - Dali::Actor::Add(), and an InsertAbove() or InsertBelow() of a new child that leaves it as the
 *   last child: the child is appended to both orders.
 * - Removing a child (Dali::Actor::Remove(), Ui::View::Remove() and the like): the child leaves
 *   the layout order and the other children keep theirs.
 *
 * A reorder of the parent's children made from inside a callback connected to the parent's
 * Dali::DevelActor::ChildOrderChangedSignal() is not reported, because a signal is not emitted
 * again while it is being emitted. Whether the layout order still follows such a reorder depends
 * on the order in which the handlers connected to that signal run and on which call emitted the
 * signal, so do not rely on either outcome: make such reorders outside the callback.
 *
 * While the two orders differ, Dali::Actor::GetChildAt() and SIBLING_ORDER report the actor order,
 * and Ui::View::GetChildViewAt() and Ui::View::IndexOfChildView() the layout order. The focus
 * finder and the accessibility child list read the actor order. This function emits
 * Dali::DevelActor::ChildOrderChangedSignal() as Dali::Actor::Raise() does.
 * Dali::Actor::Property::DEPTH_INDEX is independent of both orders: it changes only the draw and
 * hit-test order among siblings.
 *
 * This replaces the deprecated Ui::View::Raise(LayoutOrderPolicy) and
 * ViewImpl::Raise(Ui::LayoutOrderPolicy) called with Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to raise; it must not be empty
 * @see Dali::Actor::Raise()
 */
DALI_UI_API void RaisePreservingLayout(Ui::View view);

/**
 * @brief Lowers the view one step below its previous sibling without changing its parent's
 * layout order.
 *
 * Changes the actor order exactly like Dali::Actor::Lower() and leaves the parent's layout order
 * as it is; see RaisePreservingLayout() for the two orders, how long the effect lasts and what
 * differs while they differ.
 *
 * When the two orders differ, a later Dali::Actor::InsertAbove() or InsertBelow() on the parent
 * makes its layout order follow the actor order again, as does a reorder of one of its children
 * made with the inherited Dali::Actor calls; only a new child that ends up as the last child is
 * appended without that. RaisePreservingLayout() lists the exact cases.
 *
 * This replaces the deprecated Ui::View::Lower(LayoutOrderPolicy) and
 * ViewImpl::Lower(Ui::LayoutOrderPolicy) called with Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to lower; it must not be empty
 * @see Dali::Actor::Lower()
 */
DALI_UI_API void LowerPreservingLayout(Ui::View view);

/**
 * @brief Raises the view to the top of its siblings without changing its parent's layout order.
 *
 * Changes the actor order exactly like Dali::Actor::RaiseToTop() and leaves the parent's layout
 * order as it is; see RaisePreservingLayout() for the two orders, how long the effect lasts and
 * what differs while they differ.
 *
 * When the two orders differ, a later Dali::Actor::InsertAbove() or InsertBelow() on the parent
 * makes its layout order follow the actor order again, as does a reorder of one of its children
 * made with the inherited Dali::Actor calls; only a new child that ends up as the last child is
 * appended without that. RaisePreservingLayout() lists the exact cases.
 *
 * This replaces the deprecated Ui::View::RaiseToTop(LayoutOrderPolicy) and
 * ViewImpl::RaiseToTop(Ui::LayoutOrderPolicy) called with Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to raise; it must not be empty
 * @see Dali::Actor::RaiseToTop()
 */
DALI_UI_API void RaiseToTopPreservingLayout(Ui::View view);

/**
 * @brief Lowers the view to the bottom of its siblings without changing its parent's layout
 * order.
 *
 * Changes the actor order exactly like Dali::Actor::LowerToBottom() and leaves the parent's
 * layout order as it is; see RaisePreservingLayout() for the two orders, how long the effect
 * lasts and what differs while they differ.
 *
 * When the two orders differ, a later Dali::Actor::InsertAbove() or InsertBelow() on the parent
 * makes its layout order follow the actor order again, as does a reorder of one of its children
 * made with the inherited Dali::Actor calls; only a new child that ends up as the last child is
 * appended without that. RaisePreservingLayout() lists the exact cases.
 *
 * This replaces the deprecated Ui::View::LowerToBottom(LayoutOrderPolicy) and
 * ViewImpl::LowerToBottom(Ui::LayoutOrderPolicy) called with Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to lower; it must not be empty
 * @see Dali::Actor::LowerToBottom()
 */
DALI_UI_API void LowerToBottomPreservingLayout(Ui::View view);

/**
 * @brief Raises the view directly above @p target without changing its parent's layout order.
 *
 * Changes the actor order exactly like Dali::Actor::RaiseAbove() and leaves the parent's layout
 * order as it is; see RaisePreservingLayout() for the two orders, how long the effect lasts and
 * what differs while they differ. The view moves only when it is below @p target; otherwise it
 * stays where it is. Unlike Dali::Actor::RaiseAbove(), an empty @p target is ignored.
 *
 * When the two orders differ, a later Dali::Actor::InsertAbove() or InsertBelow() on the parent
 * makes its layout order follow the actor order again, as does a reorder of one of its children
 * made with the inherited Dali::Actor calls; only a new child that ends up as the last child is
 * appended without that. RaisePreservingLayout() lists the exact cases.
 *
 * This replaces the deprecated Ui::View::RaiseAbove(View, LayoutOrderPolicy) and
 * ViewImpl::RaiseAbove(Ui::View, Ui::LayoutOrderPolicy) called with
 * Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to raise; it must not be empty
 * @param[in] target The sibling to raise the view above; an empty handle does nothing
 * @see Dali::Actor::RaiseAbove()
 */
DALI_UI_API void RaiseAbovePreservingLayout(Ui::View view, Ui::View target);

/**
 * @brief Lowers the view directly below @p target without changing its parent's layout order.
 *
 * Changes the actor order exactly like Dali::Actor::LowerBelow() and leaves the parent's layout
 * order as it is; see RaisePreservingLayout() for the two orders, how long the effect lasts and
 * what differs while they differ. The view moves only when it is above @p target; otherwise it
 * stays where it is. Unlike Dali::Actor::LowerBelow(), an empty @p target is ignored.
 *
 * When the two orders differ, a later Dali::Actor::InsertAbove() or InsertBelow() on the parent
 * makes its layout order follow the actor order again, as does a reorder of one of its children
 * made with the inherited Dali::Actor calls; only a new child that ends up as the last child is
 * appended without that. RaisePreservingLayout() lists the exact cases.
 *
 * This replaces the deprecated Ui::View::LowerBelow(View, LayoutOrderPolicy) and
 * ViewImpl::LowerBelow(Ui::View, Ui::LayoutOrderPolicy) called with
 * Ui::LayoutOrderPolicy::PRESERVE.
 *
 * @SINCE_3_0.0
 * @param[in] view The view to lower; it must not be empty
 * @param[in] target The sibling to lower the view below; an empty handle does nothing
 * @see Dali::Actor::LowerBelow()
 */
DALI_UI_API void LowerBelowPreservingLayout(Ui::View view, Ui::View target);

} // namespace View
} // namespace Extension
} // namespace Ui
} //namespace DALI_NAMESPACE
