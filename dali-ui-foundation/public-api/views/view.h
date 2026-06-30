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
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/object/base-handle.h>
#include <functional>
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/types/view-state.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/state-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali-ui-foundation/public-api/views/view-with.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>
#include <functional>
#include <initializer_list>

namespace Dali
{
namespace Ui
{

// Forward declarations
class LayoutManager;
class LayoutTransition;
class RenderEffect;
class UiColor;
class ViewAnimationBridge;
class ViewAnimationSpec;

class ViewImpl;

/**
 * @brief Move-only callback for custom focus navigation.
 *
 * @code
 * // Member function:
 * view.SetFocusNavigationCallback(FocusNavigationCallback::New(this, &MyClass::OnFocusNavigation));
 *
 * // Static function:
 * view.SetFocusNavigationCallback(FocusNavigationCallback::New(&MyFocusNavigationFunc));
 * @endcode
 *
 * @note Signature: View(View currentFocusedView, FocusDirection direction)
 */
using FocusNavigationCallback = Callback<View(View, FocusDirection)>;

// @ANIMATION_CONFIG(View)
/**
 * @brief View is a base UI component class that extends CustomActor.
 *
 * View provides basic UI functionality and can be used as a foundation
 * for creating custom UI components. It inherits all the capabilities
 *
 * View implements the layout system with Measure/Arrange passes.
 */
class DALI_UI_API View : public CustomActor
{
public:
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized View handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  View();

  /**
   * @brief Creates an initialized View.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static View New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] view Handle to copy
   */
  View(const View& view);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  View(View&& rhs) noexcept;

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~View();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  View& operator=(const View& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  View& operator=(View&& rhs) noexcept = default;

public: // Static Methods
  /**
   * @brief Downcasts a handle to View handle.
   *
   * If handle points to a View, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a View or an uninitialized handle
   */
  static View DownCast(BaseHandle handle);

public: // Measure / Arrange API
  /**
   * @brief Measures the view with the given constraints.
   *
   * This method implements caching to avoid redundant calculations.
   * It calls OnMeasure internally (Template Method pattern).
   *
   * @param[in] widthConstraint The width constraint for measurement
   * @param[in] heightConstraint The height constraint for measurement
   * @return The measured size
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @brief Arranges the view within the given bounds.
   *
   * This method calls OnArrange internally (Template Method pattern).
   *
   * @param[in] bounds The bounds to arrange the view in
   * @return The final arranged size
   */
  MeasuredSize Arrange(const LayoutRect& bounds);

  /**
   * @brief Invalidates the measure of this view.
   *
   * This propagates to the parent layout while one exists,
   * until the layout root is reached (no parent Layout).
   */
  void InvalidateMeasure();

  /**
   * @brief Invalidates the arrange of this view.
   */
  void InvalidateArrange();

  /**
   * @brief Gets the measured size from the last Measure() pass.
   *
   * @return The measured size
   */
  MeasuredSize GetMeasuredSize() const;

  /**
   * @brief Sets a custom measure callback for this View.
   *
   * When set, the callback replaces the default measurement behavior
   * during the layout pass. Pass a default-constructed callback to remove.
   *
   * @param[in] callback The measure callback (ownership transferred)
   *
   * @code
   * // Member function
   * view.SetMeasureCallback(MeasureCallback::New(this, &MyClass::OnMeasure));
   *
   * // Static function
   * view.SetMeasureCallback(MeasureCallback::New(&MyMeasure));
   *
   * // Remove callback
   * view.SetMeasureCallback({});
   * @endcode
   */
  void SetMeasureCallback(MeasureCallback callback);

  /**
   * @brief Sets a custom arrange callback for this View.
   *
   * When set, the callback replaces the default arrangement behavior
   * during the layout pass. Pass a default-constructed callback to remove.
   *
   * The callback should arrange children in a LEFT_TO_RIGHT frame by calling
   * View::Arrange() on each non-standalone child, which writes the child's
   * POSITION_X / SIZE_WIDTH onto its Actor. The framework then mirrors
   * direct children horizontally when the View's effective layout direction
   * resolves to RIGHT_TO_LEFT, so callbacks must not apply RTL mirroring
   * themselves.
   *
   * @param[in] callback The arrange callback (ownership transferred)
   *
   * @code
   * // Member function
   * view.SetArrangeCallback(ArrangeCallback::New(this, &MyClass::OnArrange));
   *
   * // Static function
   * view.SetArrangeCallback(ArrangeCallback::New(&MyArrange));
   *
   * // Remove callback
   * view.SetArrangeCallback({});
   * @endcode
   */
  void SetArrangeCallback(ArrangeCallback callback);

  /**
   * @brief Attaches a LayoutTransition to this view.
   *
   * When attached, the framework animates this view's children between
   * layout-pass results: ENTER on add, EXIT on remove (deferred until the
   * EXIT slot finishes), CHANGE on bounds change. Pass an uninitialized
   * handle to detach.
   *
   * @note Replacing the transition at runtime does not interrupt
   * in-flight transitions. Each in-flight ENTER / EXIT / CHANGE keeps the
   * transition handle that started it, finishes on its own timing, and
   * fires its own @c OnFinished. Newly added/removed/moved children
   * dispatch under the new handle from the next layout pass onward. To
   * disable transitions, pass an uninitialized handle: in-flight ones
   * still finish, but no new transitions start.
   *
   * @warning Do NOT call @c SetLayoutTransition from inside a custom
   * @c ArrangeCallback (registered via @c SetArrangeCallback) on this
   * view. The callback runs in the middle of the layout pass while the
   * dispatcher has captured the pre-pass bounds; detaching the transition
   * mid-pass leaves a stale snapshot until the next pass.
   *
   * @param[in] transition The transition to attach (uninitialized to detach)
   */
  void SetLayoutTransition(LayoutTransition transition);

  /**
   * @brief Returns the LayoutTransition currently attached, or an
   * uninitialized handle.
   *
   * @return The attached LayoutTransition handle
   */
  LayoutTransition GetLayoutTransition() const;

  /**
   * @brief Attaches a LayoutManager to this View.
   *
   * After attach, the View's layout pass dispatches to LayoutManager::Measure
   * and LayoutManager::Arrange unless a MeasureCallback / ArrangeCallback is
   * also set (which take priority over the manager).
   *
   * Only one LayoutManager may be attached for the lifetime of a View. Passing
   * a null UniquePtr asserts. Attaching when a LayoutManager is already
   * present asserts. Late attach (after children have been added or the View
   * has been parented) is permitted; the next Measure pass picks up the
   * change because attach calls InvalidateMeasure internally.
   *
   * @param[in] manager The LayoutManager to attach (ownership transferred)
   */
  void AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager);

public: // Properties
  // @ANIMATABLE_MANUAL(BackgroundColor, UiColor)
  // @ANIMATABLE_MANUAL(ShadowBlurRadius, float)
  // @ANIMATABLE_MANUAL(ShadowOpacity, float)
  // @ANIMATABLE_MANUAL(SizeWidth, float)
  // @ANIMATABLE_MANUAL(SizeHeight, float)
  // @ANIMATABLE_MANUAL(PositionX, float)
  // @ANIMATABLE_MANUAL(PositionY, float)

  /**
   * @brief Gets the x scale factor applied to the view.
   *
   * @return The x scale factor
   */
  float GetScaleX() const;

  /**
   * @brief Gets the current x scale factor applied to the view.
   *
   * @return The current x scale factor
   */
  float GetCurrentScaleX() const;

  // @ANIMATABLE(Actor::Property::SCALE_X, float)
  /**
   * @brief Sets the x scale factor applied to the view.
   *
   * @param[in] scaleX The X scale factor to set
   */
  void SetScaleX(float scaleX);

  /**
   * @brief Gets the y scale factor applied to the view.
   *
   * @return The y scale factor
   */
  float GetScaleY() const;

  /**
   * @brief Gets the current y scale factor applied to the view.
   *
   * @return The current y scale factor
   */
  float GetCurrentScaleY() const;

  /**
   * @brief Sets the UiScale participation policy for this view.
   *
   * @param[in] policy The UiScalePolicy to apply
   * @return Reference to this View for fluent chaining
   */
  void SetUiScalePolicy(UiScalePolicy policy);

  /**
   * @brief Gets the UiScale participation policy of this view.
   *
   * @return The current UiScalePolicy
   */
  UiScalePolicy GetUiScalePolicy() const;

  // @ANIMATABLE(Actor::Property::SCALE_Y, float)
  /**
   * @brief Sets the y scale factor applied to the view.
   *
   * @param[in] scaleY The Y scale factor to set
   */
  void SetScaleY(float scaleY);

  /**
   * @brief Sets the y scale factor applied to the view.
   *
   * @param[in] scaleX The X scale factor to set
   * @param[in] scaleY The Y scale factor to set
   */
  void SetScale(float scaleX, float scaleY);

  /**
   * @brief Gets the current scale factor applied to the view.
   *
   * @return The current scale factor
   */
  Vector2 GetCurrentScale() const;

  /**
   * @brief Checks if the view is visible.
   *
   * @return True if the view is visible, false otherwise
   */
  bool IsVisible() const;

  /**
   * @brief Sets the visibility of the view.
   *
   * @note Toggling visibility does NOT fire a parent's
   * @c LayoutTransition. ENTER / EXIT dispatch only on
   * @c Actor::Add / @c View::Remove(View, RemovePolicy::ANIMATE_EXIT) —
   * visibility-driven transitions are not supported. To animate a
   * hide/show, drive @c Actor::Property::OPACITY or @c SCALE via your own
   * @c Animation, or call @c Remove(child, RemovePolicy::ANIMATE_EXIT) +
   * @c Add to participate in the layout transition.
   *
   * @param[in] visibility True to make the view visible, false to hide it
   * @return Reference to this View for fluent chaining
   */
  void SetVisibility(bool visibility);

  /**
   * @brief Gets the opacity of the view.
   *
   * @return The opacity value (0.0 to 1.0)
   */
  float GetOpacity() const;

  // @ANIMATABLE(Actor::Property::OPACITY, float)
  /**
   * @brief Sets the opacity of the view.
   *
   * @param[in] opacity The opacity value to set (0.0 to 1.0)
   * @return Reference to this View for fluent chaining
   */
  void SetOpacity(float opacity);

  /**
   * @brief Gets the actual rendered size of this View after layout.
   *
   * This is a read-only result of the layout system's Measure/Arrange passes.
   * To set the desired size, use SetRequestedWidth() / SetRequestedHeight().
   *
   * @return The actual size (width, height) as set by the layout system
   */
  MeasuredSize GetSize() const;

  /**
   * @brief Gets the current rendered size of this View.
   *
   * @return The current rendered size (width, height)
   */
  MeasuredSize GetCurrentSize() const;

  /**
   * @brief Sets the X position requested by the user.
   *
   * This value is the layout input. The parent's Arrange pass uses it
   * (offset by the parent's left padding and this view's left margin) to
   * compute the final rendered position. The Actor::Property::POSITION_X
   * property is updated only by the next Arrange.
   *
   * To drive the rendered position without affecting the layout request
   * (e.g. for scrolling or animation that should not feed back into
   * layout), set Actor::Property::POSITION_X via SetProperty directly.
   *
   * @param[in] x The requested X position
   */
  void SetRequestedPositionX(float x);

  /**
   * @brief Returns the user-requested X position.
   *
   * Independent of layout passes; returns the last value set via
   * SetRequestedPositionX. To read the current rendered X position
   * (Actor::Property::POSITION_X) instead, use GetPositionX().
   *
   * @return The requested X position
   */
  float GetRequestedPositionX() const;

  /**
   * @brief Sets the Y position requested by the user.
   *
   * @param[in] y The requested Y position
   * @see SetRequestedPositionX
   */
  void SetRequestedPositionY(float y);

  /**
   * @brief Returns the user-requested Y position.
   *
   * @return The requested Y position
   * @see GetRequestedPositionX
   */
  float GetRequestedPositionY() const;

  /**
   * @brief Returns the current rendered X position (Actor::Property::POSITION_X).
   *
   * After a layout pass, this is the requested X plus the parent's left
   * padding plus this view's left margin. Direct property writes (e.g.
   * scroll offset animations) may also modify it.
   *
   * @return The current rendered X position
   */
  float GetPositionX() const;

  /**
   * @brief Returns the current rendered Y position (Actor::Property::POSITION_Y).
   *
   * @return The current rendered Y position
   * @see GetPositionX
   */
  float GetPositionY() const;

  /**
   * @brief Returns the current rendered position (Actor::Property::POSITION).
   *
   * @return The current rendered position
   * @see GetPositionX, GetPositionY
   */
  Vector2 GetCurrentPosition() const;

  /**
   * @brief Sets the parent origin of the View.
   *
   * @param[in] point The parent origin to set
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @brief Gets the parent origin of the View.
   *
   * @return The parent origin of the View
   */
  Vector3 GetParentOrigin() const;

  /**
   * @brief Sets the pivot of the View.
   *
   * @param[in] point The pivot to set
   */
  void SetPivot(const Vector3& point);

  /**
   * @brief Gets the pivot of the View.
   *
   * @return The pivot of the View
   */
  Vector3 GetPivot() const;

  /**
   * @brief Sets the requested width.
   *
   * A positive value sets a fixed width.
   * WRAP_CONTENT (-1): sizes to content (default).
   * MATCH_PARENT (-2): fills the parent container.
   *
   * @param[in] width The requested width
   */
  void SetRequestedWidth(float width);

  /**
   * @brief Gets the requested width.
   *
   * @return The requested width
   */
  float GetRequestedWidth() const;

  /**
   * @brief Sets the requested height.
   *
   * A positive value sets a fixed height.
   * WRAP_CONTENT (-1): sizes to content (default).
   * MATCH_PARENT (-2): fills the parent container.
   *
   * @param[in] height The requested height
   */
  void SetRequestedHeight(float height);

  /**
   * @brief Gets the requested height.
   *
   * @return The requested height
   */
  float GetRequestedHeight() const;

  /**
   * @brief Sets the minimum width.
   *
   * @param[in] width The minimum width
   */
  void SetMinimumWidth(float width);

  /**
   * @brief Gets the minimum width.
   *
   * @return The minimum width
   */
  float GetMinimumWidth() const;

  /**
   * @brief Sets the minimum height.
   *
   * @param[in] height The minimum height
   */
  void SetMinimumHeight(float height);

  /**
   * @brief Gets the minimum height.
   *
   * @return The minimum height
   */
  float GetMinimumHeight() const;

  /**
   * @brief Sets the maximum width.
   *
   * @param[in] width The maximum width
   * @note max-wins: when the minimum width exceeds the maximum width, the
   * MAXIMUM wins. Constraints are applied by flooring to the minimum first and
   * then ceiling to the maximum, so the final clamp to the maximum prevails.
   */
  void SetMaximumWidth(float width);

  /**
   * @brief Gets the maximum width.
   *
   * @return The maximum width
   */
  float GetMaximumWidth() const;

  /**
   * @brief Sets the maximum height.
   *
   * @param[in] height The maximum height
   * @note max-wins: when the minimum height exceeds the maximum height, the
   * MAXIMUM wins. Constraints are applied by flooring to the minimum first and
   * then ceiling to the maximum, so the final clamp to the maximum prevails.
   */
  void SetMaximumHeight(float height);

  /**
   * @brief Gets the maximum height.
   *
   * @return The maximum height
   */
  float GetMaximumHeight() const;

  /**
   * @brief Sets the view margin.
   *
   * @param[in] margin The margin to set
   */
  void SetMargin(const Extents& margin);

  /**
   * @brief Gets the view margin.
   *
   * @return The view margin
   */
  Extents GetMargin() const;

  /**
   * @brief Sets the view padding.
   *
   * @param[in] padding The padding to set
   */
  void SetPadding(const Extents& padding);

  /**
   * @brief Gets the view padding.
   *
   * @return The view padding
   */
  Extents GetPadding() const;

  /**
   * @brief Sets the layout mode of this View.
   *
   * LayoutMode::DEFAULT (default) lets the parent container's layout system manage
   * this View's size and position normally.
   *
   * LayoutMode::STANDALONE excludes this View from the parent's accumulation,
   * spacing and index calculations. The View's size is still measured normally
   * (so MATCH_PARENT, WRAP_CONTENT and explicit RequestedWidth/Height all work),
   * but its position is taken from SetPositionX/SetPositionY instead of being
   * decided by the parent layout. This is useful for floating overlays, drag
   * previews, tooltips and absolute positioning inside any LayoutManager.
   *
   * Standalone children ignore the parent's padding entirely. Their measured
   * size is the parent's full inner size minus the child's own margin, and
   * their final position is SetPositionX/SetPositionY plus the child's own
   * margin in the parent's coordinate space. This keeps size and position
   * consistent: a Standalone child with MATCH_PARENT fills the parent edge
   * to edge regardless of parent padding, and any margin set on the child
   * shifts it inward in both axes.
   *
   * @param[in] mode The layout mode to set
   */
  void SetLayoutMode(LayoutMode mode);

  /**
   * @brief Gets the layout mode of this View.
   *
   * @return The layout mode
   */
  LayoutMode GetLayoutMode() const;

  /**
   * @brief Gets whether the actor should be focusable by keyboard navigation.
   *
   * @return True if it is focusable
   */
  bool IsFocusable() const;

  /**
   * @brief Sets whether the actor should be focusable by keyboard navigation.
   *
   * @param[in] focusable True if it is focusable
   */
  void SetFocusable(bool focusable);

  /**
   * @brief Gets whether the actor should be focused when touched.
   *
   * @return True if it is focused when touched
   */
  bool IsTouchFocusable() const;

  /**
   * @brief Sets whether the actor should be focused when touched.
   *
   * @param[in] touchFocusable True if it gets focused when touched
   */
  void SetTouchFocusable(bool touchFocusable);

  /**
   * @brief Sets whether descendant focus is blocked.
   *
   * When blocked, none of this View's descendants can receive keyboard focus.
   * Focus requests on descendants will be rejected.
   *
   * @param[in] blocked True to block descendant focus
   */
  void SetDescendantFocusBlocked(bool blocked);

  /**
   * @brief Gets whether descendant focus is blocked.
   *
   * @return True if descendant focus is blocked
   */
  bool IsDescendantFocusBlocked() const;

  /**
   * @brief Checks whether any ancestor has descendant focus blocked.
   *
   * Traverses the parent chain on every call to determine if any ancestor
   * has set SetDescendantFocusBlocked to true.
   *
   * @return true if an ancestor blocks descendant focus, false otherwise.
   */
  bool HasAncestorBlockingFocus() const;

  /**
   * @brief Sets the left focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating left
   */
  void SetLeftFocusableView(View view);

  /**
   * @brief Sets the right focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating right
   */
  void SetRightFocusableView(View view);

  /**
   * @brief Sets the up focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating up
   */
  void SetUpFocusableView(View view);

  /**
   * @brief Sets the down focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating down
   */
  void SetDownFocusableView(View view);

  /**
   * @brief Sets the clockwise focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating clockwise
   */
  void SetClockwiseFocusableView(View view);

  /**
   * @brief Sets the counter-clockwise focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating counter-clockwise
   */
  void SetCounterClockwiseFocusableView(View view);

  /**
   * @brief Sets the forward focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating forward
   */
  void SetForwardFocusableView(View view);

  /**
   * @brief Sets the backward focusable View for keyboard navigation.
   *
   * @param[in] view The View to focus when navigating backward
   */
  void SetBackwardFocusableView(View view);

  /**
   * @brief Sets a callback for focus navigation within this view's children.
   *
   * When set, this callback is invoked by the focus manager to determine the next
   * focusable view. It takes priority over the OnFocusNavigationRequested() virtual method.
   *
   * @param[in] callback The focus navigation callback (move-only, ownership transferred)
   */
  void SetFocusNavigationCallback(FocusNavigationCallback callback);

  /**
   * @brief Gets the background color.
   *
   * @return The background color
   */
  UiColor GetBackgroundColor();

  /**
   * @brief Sets the background color.
   *
   * If the UiColor has a color ID, it is resolved from the current
   * theme and a binding is registered so the color is automatically
   * refreshed when the theme changes.
   *
   * If the UiColor has direct RGBA values, it is applied immediately
   * and any previous ID-based binding for this property is removed.
   *
   * @param[in] color The UiColor to apply
   */
  void SetBackgroundColor(const UiColor& color);

  /**
   * @brief Sets a single shadow for this View.
   *
   * This replaces all shadows previously set by SetShadow(). Pass
   * Shadow::None() to clear all shadows from this View.
   *
   * @param[in] shadow The shadow value to apply
   */
  void SetShadow(const Shadow& shadow);

  /**
   * @brief Replaces all shadows for this View.
   *
   * @param[in] shadowStack The ordered shadow stack to apply
   */
  void SetShadow(const ShadowStack& shadowStack);

  /**
   * @brief Gets the color.
   *
   * @return The color
   */
  UiColor GetColor() const;

  /**
   * @brief Sets the color.
   *
   * @param[in] color The color to set
   * @return Reference to this View for fluent chaining
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Gets the current color.
   *
   * @return The current color
   */
  UiColor GetCurrentColor() const;

  /**
   * @brief Checks if the view is enabled.
   *
   * @return True if the view is enabled (default), false if disabled
   */
  bool IsEnabled() const;

  /**
   * @brief Sets whether the view is enabled.
   *
   * A disabled view does not receive user interaction. Setting this to false
   * adds the Disabled state and emits StateChangedSignal.
   *
   * @param[in] enabled True to enable, false to disable
   */
  void SetEnabled(bool enabled);

  /**
   * @brief Gets the corner radius of the view.
   *
   * @return The corner radius as Vector4 (topLeft, topRight, bottomRight, bottomLeft)
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Sets a uniform corner radius for all four corners.
   *
   * @param[in] radius The corner radius to apply to all corners
   */
  void SetCornerRadius(float radius);

  /**
   * @brief Sets individual corner radii for all four corners.
   *
   * @param[in] topLeft     The radius for the top-left corner
   * @param[in] topRight    The radius for the top-right corner
   * @param[in] bottomRight The radius for the bottom-right corner
   * @param[in] bottomLeft  The radius for the bottom-left corner
   */
  void SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);

  // @ANIMATABLE(View::Property::CORNER_RADIUS, Vector4)
  /**
   * @brief Sets corner radii from a Vector4.
   *
   * @param[in] radius Corner radii as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  void SetCornerRadius(const Vector4& radius);

  /**
   * @brief Gets the corner radius policy.
   *
   * @return The corner radius policy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @brief Sets the corner radius policy.
   *
   * @param[in] policy ABSOLUTE for world-unit values (default), RELATIVE for percentage [0.0, 0.5] of the shorter side
   */
  void SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @brief Shortcut to set the corner radius policy to RELATIVE.
   *
   * Equivalent to SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE).
   */
  void SetCornerRadiusPolicyRelative();

  /**
   * @brief Returns true if the corner radius policy is RELATIVE.
   *
   * @return True if the policy is RELATIVE, false if ABSOLUTE
   */
  bool IsCornerRadiusPolicyRelative() const;

  /**
   * @brief Gets the corner squareness of the view.
   *
   * @return The corner squareness as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  Vector4 GetCornerSquareness() const;

  /**
   * @brief Sets a uniform corner squareness for all four corners.
   *
   * @param[in] squareness The squareness value to apply to all corners
   */
  void SetCornerSquareness(float squareness);

  /**
   * @brief Sets individual corner squareness values for all four corners.
   *
   * @param[in] topLeft     The squareness for the top-left corner
   * @param[in] topRight    The squareness for the top-right corner
   * @param[in] bottomRight The squareness for the bottom-right corner
   * @param[in] bottomLeft  The squareness for the bottom-left corner
   */
  void SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft);

  // @ANIMATABLE(View::Property::CORNER_SQUARENESS, Vector4)
  /**
   * @brief Sets corner squareness from a Vector4.
   *
   * @param[in] squareness Squareness values as Vector4 (x=topLeft, y=topRight, z=bottomRight, w=bottomLeft)
   */
  void SetCornerSquareness(const Vector4& squareness);

  /**
   * @brief Gets the borderline width of the view.
   *
   * @return The borderline width
   */
  float GetBorderlineWidth() const;

  // @ANIMATABLE(View::Property::BORDERLINE_WIDTH, float)
  /**
   * @brief Sets the borderline width of the view.
   *
   * @param[in] width The borderline width to set
   */
  void SetBorderlineWidth(float width);

  /**
   * @brief Gets the borderline color of the view.
   *
   * @return The borderline color
   */
  UiColor GetBorderlineColor();

  // @ANIMATABLE(View::Property::BORDERLINE_COLOR, UiColor)
  /**
   * @brief Sets the borderline color of the view.
   *
   * If the UiColor has a color ID, it is resolved from the current
   * theme and a binding is registered so the color is automatically
   * refreshed when the theme changes.
   *
   * @param[in] color The UiColor to apply
   */
  void SetBorderlineColor(const UiColor& color);

  /**
   * @brief Gets the borderline offset of the view.
   *
   * @return The borderline offset
   */
  float GetBorderlineOffset() const;

  // @ANIMATABLE(View::Property::BORDERLINE_OFFSET, float)
  /**
   * @brief Sets the borderline offset of the view.
   *
   * @param[in] offset The borderline offset to set
   */
  void SetBorderlineOffset(float offset);

  /**
   * @brief Gets the name of the view.
   *
   * @return The name
   */
  Dali::String GetName() const;

  /**
   * @brief Sets the name of this view.
   *
   * @param[in] name The name to set
   */
  void SetName(const Dali::String& name);

  /**
   * @brief Sets the layout direction of this view explicitly.
   *
   * Overrides inherited direction from the parent. To revert to inheritance,
   * use ClearLayoutDirection().
   *
   * @param[in] direction The layout direction to set (LEFT_TO_RIGHT or RIGHT_TO_LEFT)
   * @return Reference to this View for fluent chaining
   * @note RTL mirror: when the effective layout direction resolves to
   * RIGHT_TO_LEFT, every non-standalone child's X position is mirrored about
   * the parent width after arrange (newX = parentWidth - oldX - childWidth),
   * regardless of the layout manager. The only opt-out is placing a child in
   * LayoutMode STANDALONE, which is never mirrored.
   */
  void SetLayoutDirection(Dali::LayoutDirection::Type direction);

  /**
   * @brief Clears the explicitly set layout direction.
   *
   * After calling this, the view inherits its layout direction from the parent.
   */
  void ClearLayoutDirection();

  /**
   * @brief Returns true if this view is inheriting layout direction from its parent.
   *
   * This is true by default. It becomes false only when SetLayoutDirection() has been
   * called to set an explicit direction, and is restored to true by ClearLayoutDirection().
   *
   * @return True if inheriting, false if an explicit direction has been set
   */
  bool IsLayoutDirectionInherited() const;

  /**
   * @brief Returns the effective layout direction applied to this view.
   *
   * Always returns the resolved direction (LEFT_TO_RIGHT or RIGHT_TO_LEFT),
   * taking inheritance into account.
   *
   * @return The effective layout direction
   */
  Dali::LayoutDirection::Type GetEffectiveLayoutDirection() const;

  /**
   * @brief Returns true if this view and all its View ancestors are enabled.
   *
   * Unlike IsEnabled(), which only reflects the view's own state, this method
   * walks up the scene hierarchy and returns false if any ancestor View carries
   * ViewState::DISABLED.
   *
   * @return True if neither the view nor any ancestor is disabled
   */
  bool IsEffectivelyEnabled() const;

  /**
   * @brief Returns true if this view or any of its View ancestors is focused.
   *
   * Unlike the Focused bit in GetState(), this method walks up the scene
   * hierarchy and returns true if any ancestor View carries ViewState::FOCUSED.
   *
   * @return True if the view itself or at least one ancestor is focused
   */
  bool IsEffectivelyFocused() const;

  DALI_UI_VIEW_WITH(View)

  /**
   * @brief Attaches the interaction trait to this View and returns it.
   *
   * If this View already has an InteractiveTrait, the existing trait is reused.
   *
   * @return InteractiveTrait handle
   */
  InteractiveTrait AsInteractive();
  /**
   * @brief Attaches the selectable trait to this View.
   *
   * Selectable implies Interactive. Calling this method also enables this View's
   * interactive behavior, including click handling, pressed state handling, and
   * the default StateEffect for interactive Views when no explicit StateEffect
   * has been assigned.
   *
   * A View can have at most one selectable trait. If the View already has a
   * SelectableTrait (e.g. from a previous AsSelectable call), the existing trait is reused.
   *
   * @return SelectableTrait handle
   */
  SelectableTrait AsSelectable();

  /**
   * @brief Attaches the group-selectable trait to this View.
   *
   * GroupSelectable implies Selectable implies Interactive. Calling this method
   * also enables this View's selectable behavior (a boolean selected state and
   * SelectionChangedSignal) and its interactive behavior (click handling, pressed
   * state, the default StateEffect for interactive Views).
   *
   * A View can have at most one group-selectable trait. If the View already has a
   * GroupSelectableTrait, the existing trait is reused. Membership in a SelectionGroup
   * is declarative: an on-scene View with no group name auto-joins its parent View's
   * group, or GroupSelectableTrait::SetGroupName(name) joins a named group.
   *
   * @return GroupSelectableTrait handle
   */
  GroupSelectableTrait AsGroupSelectable();

  /**
   * @brief Sets layout parameters on this View.
   *
   * The params handle is stored on the View as-is, and the View's
   * measure cache is invalidated.
   *
   * @param[in] params The layout parameters to attach to this View
   * @code
   * auto params = AbsoluteLayoutParams::New();
   * params.SetBounds(LayoutRect(10, 20, 100, 200));
   * params.SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
   * child.SetLayoutParams(params);
   * @endcode
   */
  void SetLayoutParams(LayoutParams params);
  /**
   * @brief Adds a list of children to this View in a declarative way.
   * This method allows for a hierarchical UI tree construction by passing
   * a brace-enclosed initializer list of View objects.
   * @param[in] children The initializer list containing child View handles to be added.
   */
  void AddChildren(std::initializer_list<View> children)
  {
    for(const auto& child : children)
    {
      Add(child);
    }
  }
  /**
   * @brief Adds a list of visuals to this View in a declarative way.
   * This method allows for append visuals construction by passing
   * a brace-enclosed initializer list of VisualBase objects.
   *
   * Assert if too many visuals are added, or duplicated VisualBase added.
   * Please use AddVisual() API if you need to control error cases.
   *
   * @param[in] containerRangeType The range of visuals to be added.
   * @param[in] visuals The initializer list containing VisualBase handles to be added.
   */
  void AddVisuals(Dali::Ui::Visual::ContainerRangeType containerRangeType, std::initializer_list<Dali::Ui::VisualBase> visuals)
  {
    for(const auto& visual : visuals)
    {
      bool added = AddVisual(visual, containerRangeType);
      DALI_ASSERT_ALWAYS(added && "Too many visuals are added by declarative method, or try to add duplicated VisualBase!");
    }
  }
  /**
   * @brief Sets an attachment from uniquely owned data.
   *
   * The data ownership is moved into this View. If another
   * attachment already exists for @p id, it is destroyed and replaced. The
   * stored attachment is destroyed when it is removed, replaced, or when the
   * View implementation is destroyed.
   *
   * @note This typed attachment API stores Dali::UniquePtr<T> with its default
   * deleter. UniquePtr types with custom deleters are not supported by this
   * overload.
   *
   * @note Use a stable namespace-scope type for attachment data. Local types and
   * anonymous-namespace types should not be used when the attachment may be
   * retrieved from another translation unit, because they may not have a stable
   * cross-translation-unit type identity.
   *
   * @param[in] id The key to identify the attachment
   * @param[in] data The data whose ownership is transferred to this View
   */
  template<typename T>
  void SetAttachment(AttachmentId id, Dali::UniquePtr<T> data)
  {
    DALI_ASSERT_ALWAYS(data && "SetAttachment requires non-null data");
    View::SetAttachment(id, UniqueAny(Dali::Move(data)));
  }

  /**
   * @brief Removes an attachment.
   *
   * Removing an attachment destroys the stored UniqueAny and its owned value.
   *
   * @param[in] id The key to identify the attachment
   * @return True if an attachment was removed
   */
  bool RemoveAttachment(AttachmentId id);

  /**
   * @brief Gets an attachment value.
   *
   * The returned pointer is owned by this View. The caller must not delete it
   * and must not use it after the attachment is removed, replaced, or the View
   * implementation is destroyed.
   *
   * @param[in] id The key to identify the attachment
   * @return Pointer to the stored value, or nullptr if missing or type mismatched
   */
  template<typename T>
  T* GetAttachment(AttachmentId id)
  {
    using StoredType = Dali::UniquePtr<T>;

    UniqueAny*  attachment = GetAttachmentInternal(id);
    StoredType* data       = attachment ? attachment->Get<StoredType>() : nullptr;
    return data ? data->Get() : nullptr;
  }

  /**
   * @brief Gets a const attachment value.
   *
   * The returned pointer is owned by this View. The caller must not delete it
   * and must not use it after the attachment is removed, replaced, or the View
   * implementation is destroyed.
   *
   * @param[in] id The key to identify the attachment
   * @return Pointer to the stored value, or nullptr if missing or type mismatched
   */
  template<typename T>
  const T* GetAttachment(AttachmentId id) const
  {
    using StoredType = Dali::UniquePtr<T>;

    const UniqueAny*  attachment = GetAttachmentInternal(id);
    const StoredType* data       = attachment ? attachment->Get<StoredType>() : nullptr;
    return data ? data->Get() : nullptr;
  }

  /**
   * @brief Detaches an attachment value.
   *
   * If the stored value type matches @p T, the attachment is removed from this
   * View and ownership of the value is transferred to the returned UniquePtr.
   * If the attachment is missing or the type does not match, this View is left
   * unchanged and nullptr is returned.
   *
   * @note This typed attachment API detaches Dali::UniquePtr<T> with its
   * default deleter. UniquePtr types with custom deleters are not supported by
   * this overload.
   *
   * @param[in] id The key to identify the attachment
   * @return The detached value, or nullptr if missing or type mismatched
   */
  template<typename T>
  Dali::UniquePtr<T> DetachAttachment(AttachmentId id)
  {
    using StoredType = Dali::UniquePtr<T>;

    UniqueAny*  storedAttachment = GetAttachmentInternal(id);
    StoredType* storedData       = storedAttachment ? storedAttachment->Get<StoredType>() : nullptr;
    if(!storedData)
    {
      return Dali::UniquePtr<T>();
    }

    UniqueAny                   attachment = DetachAttachmentInternal(id);
    Dali::UniquePtr<StoredType> data       = attachment.Detach<StoredType>();
    return data.Get() ? Dali::Move(*data) : Dali::UniquePtr<T>();
  }

  /**
   * @brief Inserts a child at the specified index.
   *
   * @param[in] index The index where to insert
   * @param[in] child The child to insert
   */
  void Insert(uint32_t index, View child);

  /**
   * @brief Removes all children from this View.
   *
   * If a LayoutTransition with an EXIT slot is attached, every child is
   * handed off to the dispatcher (same semantics as calling
   * @c Remove(child, RemovePolicy::ANIMATE_EXIT) on each child
   * individually): the child is dropped from the layout-tracking list
   * immediately, the actor stays attached during the EXIT animation, and
   * is unparented when the animation finishes. With no EXIT slot, every
   * child is unparented synchronously.
   */
  void RemoveAllChildren();

  /**
   * @brief Removes @p child from this View, choosing whether to run the
   * attached LayoutTransition's EXIT slot first.
   *
   * This is the symmetric counterpart of the inherited @c Actor::Add: just
   * as adding a child auto-dispatches ENTER, calling @c Remove with
   * @c RemovePolicy::ANIMATE_EXIT dispatches EXIT. The policy argument is
   * mandatory by design: the inherited one-argument @c Actor::Remove(Actor)
   * remains available (re-exposed via @c using below) and always performs an
   * immediate, EXIT-free unparent, identical for both @c View-typed and
   * @c Actor-typed handles. There is deliberately NO one-argument
   * @c View::Remove(View) overload, so a bare @c Remove(child) call never
   * diverges by static handle type.
   *
   * With @c RemovePolicy::ANIMATE_EXIT and a configured EXIT spec or animator
   * (this view's own slot, or an ancestor SUBTREE-scope owner's), the child is
   * kept in the actor tree as a "ghost" until the EXIT animation finishes and
   * is then unparented automatically. With no EXIT slot it is unparented
   * immediately. @c RemovePolicy::IMMEDIATE always unparents now, skipping both
   * the own and inherited EXIT effects (see @c RemovePolicy).
   *
   * @note During the EXIT animation the child is logically absent from
   * this view's child list (@c GetChildCount / @c GetChildAt skip it)
   * but still attached to the actor tree. Re-adding the SAME child to
   * the SAME parent in this state via @c View::Insert or inherited
   * @c Actor::Add is silently ignored — the EXIT continues, and the
   * actor is unparented when the animation finishes. To cancel an
   * in-flight EXIT, reparent the child to a DIFFERENT parent: the
   * dispatcher auto-cancels the EXIT, restores interaction state, and
   * triggers ENTER under the new parent.
   *
   * @note When the EXIT spec/animator is "empty" (no entries, or all
   * entries with zero duration+delay), the child is unparented
   * immediately and no lifecycle callbacks fire — the same as if no EXIT
   * slot were configured at all.
   *
   * @param[in] child  The child view to remove
   * @param[in] policy Whether to animate the EXIT transition or unparent immediately
   */
  void Remove(View child, RemovePolicy policy);

  /**
   * @brief Gets the number of child views.
   *
   * @return The child count
   */
  uint32_t GetChildCount() const;

  /**
   * @brief Gets the child view at the specified index.
   *
   * @param[in] index The child index
   * @return The child view at the index
   */
  View GetChildAt(uint32_t index) const;

  /**
   * @brief Returns the index of the given child view, or -1 if not found.
   *
   * @param[in] view The child view to find
   * @return Index of the view, or -1 if not a child
   */
  int32_t IndexOfChild(View view) const;
  using Dali::Actor::Lower;
  using Dali::Actor::LowerBelow;
  using Dali::Actor::LowerToBottom;
  using Dali::Actor::Raise;
  using Dali::Actor::RaiseAbove;
  using Dali::Actor::RaiseToTop;
  // Re-expose the inherited one-argument immediate remove; the
  // Remove(View, RemovePolicy) overload would otherwise hide it (C++ name hiding).
  using Dali::Actor::Remove;

  /**
   * @brief Raises this view one step above its immediate next sibling in the
   * parent's children list.
   *
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   */
  void Raise(LayoutOrderPolicy policy);

  /**
   * @brief Lowers this view one step below its immediate previous sibling in the
   * parent's children list.
   *
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   */
  void Lower(LayoutOrderPolicy policy);

  /**
   * @brief Raises this view to the top of its sibling list.
   *
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   */
  void RaiseToTop(LayoutOrderPolicy policy);

  /**
   * @brief Lowers this view to the bottom of its sibling list.
   *
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   */
  void LowerToBottom(LayoutOrderPolicy policy);

  /**
   * @brief Raises this view above the given target sibling.
   *
   * @param[in] target The target sibling view to raise above.
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   * @pre The target view is a sibling.
   */
  void RaiseAbove(View target, LayoutOrderPolicy policy);

  /**
   * @brief Lowers this view below the given target sibling.
   *
   * @param[in] target The target sibling view to lower below.
   * @param[in] policy UPDATE also reorders the parent layout's children;
   *                   PRESERVE keeps layout order unchanged and affects only visual z-order.
   * @pre The View has been parented.
   * @pre The target view is a sibling.
   */
  void LowerBelow(View target, LayoutOrderPolicy policy);

  /**
   * @brief Retrieves the layout parameters of a specific type attached to this View.
   *
   * Returns the stored handle. Modifying the returned handle directly
   * changes the internal data. Call InvalidateMeasure() afterwards if
   * the layout needs to be recalculated.
   *
   * @tparam T The concrete LayoutParams type (e.g. AbsoluteLayoutParams, FlexLayoutParams).
   *           T must provide static GetLayoutParamsType() and static DownCast(BaseHandle).
   * @return A valid handle if the params are attached, or an uninitialized handle
   *
   * @code
   * auto params = view.GetLayoutParams<AbsoluteLayoutParams>();
   * if (params)
   * {
   *   params.SetWidth(200.0f);
   *   view.InvalidateMeasure();
   * }
   * @endcode
   */
  template<typename T>
  T GetLayoutParams() const
  {
    return T::DownCast(GetLayoutParamsInternal(T::GetLayoutParamsType()));
  }

public: // State API (non-chaining)
  using StateChangedSignalType = Signal<void(View, StateEvent)>;

  /**
   * @brief Gets the current state of this View.
   *
   * @return The current ViewState (may contain multiple combined states)
   */
  const ViewState& GetState() const;

  /**
   * @brief Returns the state changed signal.
   *
   * Emitted whenever the view's ViewState changes.
   *
   * @code
   * view.StateChangedSignal().Connect(tracker, [](View v, StateEvent e) {
   *   if(e.Added(ViewState::FOCUSED)) { ... }
   * });
   * @endcode
   *
   * @return The StateChangedSignal
   */
  StateChangedSignalType& StateChangedSignal();

public: // Trait accessors (non-chaining)
  /**
   * @brief Returns whether this View has an interaction trait attached.
   *
   * @return True if this View has an InteractiveTrait
   */
  bool IsInteractive() const;

  /**
   * @brief Returns whether this View has a selectable trait attached.
   *
   * @return True if this View has a SelectableTrait
   */
  bool IsSelectable() const;

  /**
   * @brief Returns whether this View has a group-selectable trait attached.
   *
   * @return True if this View has a GroupSelectableTrait
   */
  bool IsGroupSelectable() const;

public: // VisualBase (non-chaining)
  /**
   * @brief Add a Dali::Ui::VisualBase to the view.
   *
   * The visual is added to the top of the visuals.
   * If the container cannot add more than maximum count of visuals
   * or the visual is already added, return false and it will be ignored.
   *
   * If input visual already added to another view,
   * visual will be detached from old view and added to this view.
   *
   * @param[in] visualBase The visual to add.
   * @param[in] containerRangeType The range of visuals to be added.
   * @return True if the visual was added successfully, false otherwise.
   */
  bool AddVisual(Dali::Ui::VisualBase visualBase, Dali::Ui::Visual::ContainerRangeType containerRangeType);

  /**
   * @brief Remove a Dali::Ui::VisualBase from the view.
   *
   * @note The VisualBase's SiblingOrder value of all other Dali::Ui::VisualBase
   * who were added at same container with removed visual will be changed automatically.
   *
   * @param[in] visualBase The visual to remove.
   */
  void RemoveVisual(Dali::Ui::VisualBase visualBase);

  /**
   * @brief Get total number of Dali::Ui::VisualBase which we added using AddVisual().
   *
   * @param[in] containerRangeType The range of visuals to get.
   * @return Get the number of visual base.
   */
  uint32_t GetVisualCount(Dali::Ui::Visual::ContainerRangeType containerRangeType) const;

  /**
   * @brief Get a Dali::Ui::VisualBase by sibling order.
   *
   * @param[in] containerRangeType The range of visuals to get.
   * @param[in] siblingOrder The sibling order to get.
   * @return Get visual base by sibling order. Empty handle if not exist.
   */
  Dali::Ui::VisualBase GetVisualAt(Dali::Ui::Visual::ContainerRangeType containerRangeType, uint32_t siblingOrder) const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The View implementation
   */
  explicit DALI_UI_API View(ViewImpl& implementation);

  /**
   * @brief Allows the creation of this View from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API View(Dali::Internal::CustomActor* internal);
  /// @endcond

private:
  /**
   * @brief Retrieves a layout params trait by LayoutParamsType.
   *
   * @param[in] type The layout params type identifier
   * @return The trait as a BaseHandle, or an empty handle if not found
   */
  BaseHandle GetLayoutParamsInternal(LayoutParamsType type) const;

public:
  /**
   * @brief OffScreenRenderingType enumeration.
   */
  enum OffScreenRenderingType
  {
    NONE,
    REFRESH_ONCE,
    REFRESH_ALWAYS
  };

  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::VIEW_PROPERTY_START_INDEX,
    PROPERTY_END_INDEX   = Ui::VIEW_PROPERTY_END_INDEX,

    ANIMATABLE_PROPERTY_START_INDEX = Ui::VIEW_ANIMATABLE_PROPERTY_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX   = Ui::VIEW_ANIMATABLE_PROPERTY_END_INDEX,

    ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX = Ui::VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX,
    ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_END_INDEX   = Ui::VIEW_ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_END_INDEX,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the View class.
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the View class.
     */
    enum
    {
      /**
       * @brief The background of the View.
       *
       * @details Name "background", type Property::MAP or Dali::String for URL or Property::VECTOR4 for Color.
       */
      BACKGROUND = PROPERTY_START_INDEX,

      /**
       * @brief The outer space around the View.
       * @details Name "margin", type Property::EXTENTS.
       * @note Margin property is to be supported by Layout algorithms and containers in future.
       */
      MARGIN,

      /**
       * @brief The inner space of the View.
       * @details Name "padding", type Property::EXTENTS.
       */
      PADDING,

      /**
       * @brief The ID of the left focusable View.
       * @details Name "leftFocusableViewId", type Property::INTEGER.
       */
      LEFT_FOCUSABLE_VIEW_ID,

      /**
       * @brief The ID of the right focusable View.
       * @details Name "rightFocusableViewId", type Property::INTEGER.
       */
      RIGHT_FOCUSABLE_VIEW_ID,

      /**
       * @brief The ID of the up focusable View.
       * @details Name "upFocusableViewId", type Property::INTEGER.
       */
      UP_FOCUSABLE_VIEW_ID,

      /**
       * @brief The ID of the down focusable View.
       * @details Name "downFocusableViewId", type Property::INTEGER.
       */
      DOWN_FOCUSABLE_VIEW_ID,

      /**
       * @brief The shadow of the View.
       * @details Name "shadow", type Property::MAP.
       */
      SHADOW,

      /**
       * @brief The name of object visible in accessibility tree.
       * @details Name "accessibilityName", type Property::STRING.
       */
      ACCESSIBILITY_NAME,

      /**
       * @brief The description of object visible in accessibility tree.
       * @details Name "accessibilityDescription", type Property::STRING.
       */
      ACCESSIBILITY_DESCRIPTION,

      /**
       * @brief Deprecated. Current translation domain for accessibility clients.
       * @details Name "accessibilityTranslationDomain", type Property::STRING.
       */
      ACCESSIBILITY_TRANSLATION_DOMAIN,

      /**
       * @brief Role being performed in accessibility hierarchy.
       * @details Name "accessibilityRole", type Property::INTEGER.
       * @note It gets integer value of AccessibilityRole enum then interprets to Dali::Accessibility::Role when
       * requested by AT-SPI. Note that setting Dali::Accessibility::Role value is still accepted for backward
       * compatibility but not preferred.
       * @see Dali::Accessibility::Role
       */
      ACCESSIBILITY_ROLE,

      /**
       * @brief Mark of able to highlight object.
       * @details Name "accessibilityHighlightable", type Property::BOOLEAN.
       */
      ACCESSIBILITY_HIGHLIGHTABLE,

      /**
       * @brief Set of accessibility attributes describing object in accessibility hierarchy
       * @details Name "accessibilityAttributes", type Property::MAP
       */
      ACCESSIBILITY_ATTRIBUTES,

      /**
       * @brief Whether a View and its descendants can emit key signals.
       * @details Name "dispatchKeyEvents", type Property::BOOLEAN
       * @note If a View's dispatchKeyEvents is set to false, then it's children will not emit a key event signal
       * either.
       */
      DISPATCH_KEY_EVENTS,

      /**
       * @brief Marks the object as invisible to AT-SPI clients.
       * @details Name "accessibilityHidden", type Property::BOOLEAN.
       * @note The representative Accessible object will not appear in the AT-SPI tree.
       */
      ACCESSIBILITY_HIDDEN,

      /**
       * @brief The ID of the clockwise focusable View.
       * @details Name "clockwiseFocusableViewId", type Property::INTEGER.
       */
      CLOCKWISE_FOCUSABLE_VIEW_ID,

      /**
       * @brief The ID of the counter-clockwise focusable View.
       * @details Name "counterClockwiseFocusableViewId", type Property::INTEGER.
       */
      COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID,

      /**
       * @brief Identifier that allows the automation framework to find and interact with this element.
       * @details Name "automationId", type Property::STRING.
       * @note This is a string identifier (compared to @c Actor::Property::ID which is an integer).
       * It will also appear in the AT-SPI tree under the key "automationId".
       */
      AUTOMATION_ID,

      /**
       * @brief The accessibility value represented by the View. For example, "60%" for a slider object.
       * @details Name "accessibilityValue", type Property::STRING.
       */
      ACCESSIBILITY_VALUE,

      /**
       * @brief Indicates the accessibility services treat the View as scrollable.
       * @details Name "accessibilityScrollable", type Property::BOOLEAN.
       */
      ACCESSIBILITY_SCROLLABLE,

      /**
       * @brief Bitset integer of AccessibilityState which describes the current state of a View.
       * @details Name "accessibilityStates", type Property::INTEGER.
       */
      ACCESSIBILITY_STATES,

      /**
       * @brief Indicates the accessibility services treat the Viewa as modal.
       * @details Name "accessibilityIsModal", type Property::BOOLEAN.
       */
      ACCESSIBILITY_IS_MODAL,

      /**
       * @brief Whether to draw on offscreen of not.
       * @details Name "offscreenRendering", type Property::INTEGER.
       * @note Default is false.
       */
      OFFSCREEN_RENDERING,

      /**
       * @brief The inner shadow of the View. The visual will use DepthIndex::Ranges::DECORATION - 2
       * @details Name "innerShadow", type Property::MAP.
       */
      INNER_SHADOW,

      /**
       * @brief The inset borderline of the View. The visual will use DepthIndex::Ranges::DECORATION - 1
       * @details Name "borderline", type Property::MAP.
       */
      BORDERLINE,

      /**
       * @brief The width requested by the View for measurement.
       * @details Name "requestedWidth", type Property::FLOAT.
       * @note WRAP_CONTENT (-1.0f) and MATCH_PARENT (-2.0f) are valid special values.
       */
      REQUESTED_WIDTH,

      /**
       * @brief The height requested by the View for measurement.
       * @details Name "requestedHeight", type Property::FLOAT.
       * @note WRAP_CONTENT (-1.0f) and MATCH_PARENT (-2.0f) are valid special values.
       */
      REQUESTED_HEIGHT,

      /**
       * @brief The minimum width applied during measurement.
       * @details Name "minimumWidth", type Property::FLOAT.
       */
      MINIMUM_WIDTH,

      /**
       * @brief The minimum height applied during measurement.
       * @details Name "minimumHeight", type Property::FLOAT.
       */
      MINIMUM_HEIGHT,

      /**
       * @brief The maximum width applied during measurement.
       * @details Name "maximumWidth", type Property::FLOAT.
       * @note max-wins: if the minimum exceeds the maximum, the MAXIMUM wins
       * (measurement floors to the minimum then ceils to the maximum).
       * See SetMaximumWidth().
       */
      MAXIMUM_WIDTH,

      /**
       * @brief The maximum height applied during measurement.
       * @details Name "maximumHeight", type Property::FLOAT.
       */
      MAXIMUM_HEIGHT,

      /**
       * @brief The layout mode that controls how the View participates in its parent's layout.
       * @details Name "layoutMode", type Property::INTEGER (Ui::LayoutMode enum).
       */
      LAYOUT_MODE,

      /**
       * @brief Whether the View acts as a focus group boundary.
       * @details Name "focusGroup", type Property::BOOLEAN.
       */
      FOCUS_GROUP,

      /**
       * @brief The ID of the forward focusable View (used for Tab key navigation).
       * @details Name "forwardFocusableViewId", type Property::INTEGER.
       */
      FORWARD_FOCUSABLE_VIEW_ID,

      /**
       * @brief The ID of the backward focusable View (used for Shift+Tab key navigation).
       * @details Name "backwardFocusableViewId", type Property::INTEGER.
       */
      BACKWARD_FOCUSABLE_VIEW_ID,

      /**
       * @brief The radius for the rounded corners of the View.
       * @details Name "viewCornerRadius", type Property::VECTOR4 or Property::FLOAT
       * @note By default, it is Vector::ZERO.
       * @note Applies to specific visuals inside the View.
       * @note Only Property::Vector4 can be animated.
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_RADIUS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX,

      /**
       * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute
       * (in world units).
       * @details Name "viewCornerRadiusPolicy", type Property::INTEGER.
       * @see Policy::Type
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_RADIUS_POLICY
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS_POLICY,

      /**
       * @brief The squareness for the rounded corners of the View.
       * @details Name "viewCornerSquareness", type Property::VECTOR4 or Property::FLOAT
       * @note By default, it is Vector::ZERO.
       * @note Applies to specific visuals inside the View.
       * @note Only Property::Vector4 can be animated.
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_SQUARENESS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_SQUARENESS,

      /**
       * @brief The width for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineWidth", type Property::FLOAT.
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_WIDTH
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_WIDTH,

      /**
       * @brief The color for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineColor", type Property::VECTOR4
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_COLOR
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_COLOR,

      /**
       * @brief The offset for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineOffset", type Property::FLOAT.
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_OFFSET
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_OFFSET,

    };
  };

  /// @brief Key Event signal type.
  typedef Signal<bool(View, KeyEvent)> KeyEventSignalType;

  /// @brief Focus changed signal type.
  typedef Signal<void(View, bool)> FocusChangedSignalType;

  /// @brief ResourceReady signal type.
  typedef Signal<void(View)> ResourceReadySignalType;

  /// @brief Offscreen rendering finished signal type.
  typedef Signal<void(View)> OffScreenRenderingFinishedSignalType;

  /// @brief AccessibilityActivate signal type.
  typedef Signal<void()> AccessibilityActivateSignalType;

  /// @brief AccessibilityReadingSkipped signal type.
  typedef Signal<void()> AccessibilityReadingSkippedSignalType;

  /// @brief AccessibilityReadingPaused signal type.
  typedef Signal<void()> AccessibilityReadingPausedSignalType;

  /// @brief AccessibilityReadingResumed signal type.
  typedef Signal<void()> AccessibilityReadingResumedSignalType;

  /// @brief AccessibilityReadingCancelled signal type.
  typedef Signal<void()> AccessibilityReadingCancelledSignalType;

  /// @brief AccessibilityReadingStopped signal type.
  typedef Signal<void()> AccessibilityReadingStoppedSignalType;

  /// @brief AccessibilityGetName signal type.
  typedef Signal<void(Dali::String&)> AccessibilityGetNameSignalType;

  /// @brief AccessibilityGetDescription signal type.
  typedef Signal<void(Dali::String&)> AccessibilityGetDescriptionSignalType;

  /// @brief AccessibilityHighlighted signal type.
  /// @param bool highlighted true if View is highlighted, false if highlight is removed.
  typedef Signal<void(bool)> AccessibilityHighlightedSignalType;

  /// @brief Visual Event signal type.
  typedef Signal<void(View, Dali::Property::Index, Dali::Property::Index)> VisualEventSignalType;

  /**
   * @brief Clears the background.
   */
  void ClearBackground();

  /**
   * @brief Sets RenderEffect to this View.
   *
   * @param[in] effect RenderEffect to add.
   *
   * @note Every effect inherits RenderEffect.
   */
  void SetRenderEffect(Ui::RenderEffect effect);

  /**
   * @brief Retrieves View's render effect
   * @return RenderEffect set on this View
   */
  Ui::RenderEffect GetRenderEffect() const;

  /**
   * @brief Clears RenderEffect of this View, if exists.
   */
  void ClearRenderEffect();

  /**
   * @brief Query if all resources required by a View are loaded and ready.
   *
   * Most resources are only loaded when the View is placed on stage.
   * @return true if the resources are loaded and ready, false otherwise
   */
  bool IsResourceReady() const;

  /**
   * @brief Checks whether the view is connected to the scene.
   *
   * @return true if the view is on scene, false otherwise
   */
  bool IsOnScene() const;

  /**
   * @brief This signal is emitted after all resources required by a View are loaded and ready.
   *
   * Most resources are only loaded when the View is placed on stage.
   *
   * If resources are shared between ImageViews, they are cached.
   * In this case, the ResourceReady signal may be sent before there is an object to connect to.
   * To protect against this, IsResourceReady() can be checked first.
   *
   * @code
   *    auto newView = View::New();
   *    newView.SetResource( resourceUrl );
   *    if ( newView.IsResourceReady() )
   *    {
   *       // do something
   *    }
   *    else
   *    {
   *      newView.ResourceReadySignal.Connect( .... )
   *    }
   * @endcode
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( View view );
   * @endcode
   *
   * @return The signal to connect to
   * @note A RelayoutRequest is queued by View before this signal is emitted
   */
  ResourceReadySignalType& ResourceReadySignal();

  /**
   * @brief This signal is emitted when offscreen rendering is finished.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( View view );
   * @endcode
   *
   * @return The signal to connect to
   * @pre The View has been initialized.
   * @note This signal is emitted when the offscreen rendering task is completed.
   * @note This signal is only emitted when OffScreenRenderingType is set to RENDER_ONCE.
   */
  OffScreenRenderingFinishedSignalType& OffScreenRenderingFinishedSignal();

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(View view, KeyEvent event);
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @return The signal to connect to
   * @pre The View has been initialized.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted when the view gets or loses focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(View view, bool focused);
   * @endcode
   * @return The signal to connect to
   * @pre The View has been initialized.
   */
  FocusChangedSignalType& FocusChangedSignal();

public: // Animation
  /**
   * @brief Creates a ViewAnimationBridge for this View.
   *
   * Returns a lightweight, stack-allocated proxy that applies animations
   * directly to the given Animation for this View.
   *
   * @code
   *   auto anim = Animation::New();
   *   view.Animate(anim).Opacity(0.5f, 300_ms);
   *   anim.Play();
   * @endcode
   *
   * @param[in] animation The Animation to apply to
   * @return A ViewAnimationBridge
   */
  ViewAnimationBridge Animate(Animation animation);

  /**
   * @brief Creates a new ViewAnimationSpec.
   *
   * @code
   *   auto spec = View::NewAnimationSpec()
   *     .Opacity(1.0f, 300_ms);
   *   spec.ApplyTo(anim, view);
   * @endcode
   *
   * @return A new ViewAnimationSpec
   */
  static ViewAnimationSpec NewAnimationSpec();

public:
  /**
   * @brief Sets an attachment internally.
   *
   * @param[in] id The key to identify the attachment
   * @param[in] attachment The attachment whose ownership is transferred to this View
   */
  void SetAttachment(AttachmentId id, UniqueAny attachment);

  /**
   * @brief Sets a StateEffect on this View.
   *
   * The effect receives this View's state changes. If this View receives an
   * interactive trait, for example through AsInteractive() or during
   * InteractiveView initialization, this explicitly assigned effect takes
   * priority over the UiConfig default for interactive Views.
   *
   * The effect handle is stored as-is and may be shared with other Views. This
   * method does not clone or snapshot the effect. If an effect type exposes
   * mutable APIs, modifying the effect after assigning it may affect this View
   * and any other Views sharing the same effect object.
   *
   * StateEffect implementations should keep per-View runtime state outside the
   * effect object.
   *
   * Pass StateEffect::None() to explicitly disable state effects for this View.
   *
   * @param[in] effect A StateEffect, or StateEffect::None() to disable state effects
   */
  void SetStateEffect(StateEffect effect);

  /**
   * @brief Sets the target used by state effects.
   *
   * The target must be this View or one of its descendants. StateEffect
   * implementations may use this target instead of the owner View when applying
   * effects for state changes.
   *
   * Pass an empty View to clear the target. If no target is set, the owner View
   * is used.
   *
   * @param[in] target The state effect target View, or an empty View to clear
   */
  void SetStateEffectTarget(View target);

  /**
   * @brief Gets the target used by state effects.
   *
   * If no target has been set, this View is returned.
   *
   * @return The state effect target View
   */
  View GetStateEffectTarget() const;

public: // Templates for Deriving Classes
  /**
   * @brief Template to allow deriving Views to DownCast handles to deriving handle classes.
   *
   * @tparam     T      The handle class
   * @tparam     I      The implementation class
   * @param[in] handle Handle to an object
   * @return Handle to a class T or an uninitialized handle
   * @see DownCast(BaseHandle)
   */
  template<typename T, typename I>
  DALI_INTERNAL static T DownCast(BaseHandle handle)
  {
    T result;

    CustomActor custom = Dali::CustomActor::DownCast(handle);
    if(custom)
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      I* impl = dynamic_cast<I*>(&customImpl);

      if(impl)
      {
        result = T(customImpl.GetOwner());
      }
    }

    return result;
  }

  /**
   * @brief Template to allow deriving Views to verify whether the Internal::CustomActor* is actually an
   * implementation of their class.
   *
   * @tparam     I       The implementation class
   * @param[in] internal Pointer to the Internal::CustomActor
   */
  template<typename I>
  DALI_INTERNAL void VerifyCustomActorPointer(Dali::Internal::CustomActor* internal)
  {
    if(internal)
    {
      DALI_ASSERT_DEBUG(dynamic_cast<I*>(&CustomActor(internal).GetImplementation()));
    }
  }

private:
  UniqueAny*       GetAttachmentInternal(AttachmentId id);
  const UniqueAny* GetAttachmentInternal(AttachmentId id) const;
  UniqueAny        DetachAttachmentInternal(AttachmentId id);
};

} // namespace Ui

} // namespace Dali
