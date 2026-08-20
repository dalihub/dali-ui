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
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-callback.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/inner-shadow.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/types/view-state.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/state-event.h>
#include <dali-ui-foundation/public-api/views/view-accessibility-types.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali-ui-foundation/public-api/views/view-with.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
class AbsoluteLayoutParams;
class FlexLayoutParams;
class GridLayoutParams;
class StackLayoutParams;
class LayoutManager;
class LayoutTransition;
class RenderEffect;
class UiColor;
class ViewAnimationBridge;
class ViewAnimationSpec;

class ViewImpl;

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
   * @return The final arranged bounds (parent-local, pre-RTL logical)
   */
  LayoutRect Arrange(const LayoutRect& bounds);

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
  // @ANIMATABLE_MANUAL(BackgroundGradientStartOffset, float)
  // @ANIMATABLE_MANUAL(ShadowBlurRadius, float)
  // @ANIMATABLE_MANUAL(ShadowOpacity, float)
  // @ANIMATABLE_MANUAL(Size, Vector3)
  // @ANIMATABLE_MANUAL(SizeWidth, float)
  // @ANIMATABLE_MANUAL(SizeHeight, float)
  // @ANIMATABLE_MANUAL(Position, Vector3)
  // @ANIMATABLE_MANUAL(PositionX, float)
  // @ANIMATABLE_MANUAL(PositionY, float)
  // @ANIMATABLE_MANUAL(Scale, Vector3)
  // @ANIMATABLE_MANUAL(ScaleX, float)
  // @ANIMATABLE_MANUAL(ScaleY, float)
  // @ANIMATABLE_MANUAL(Color, Vector4)
  // @ANIMATABLE_MANUAL(Opacity, float)

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
   * layout), use Dali::Ui::Extension::View::SetPositionX(view, x).
   *
   * @param[in] x The requested X position
   */
  void SetRequestedX(float x);

  /**
   * @brief Returns the user-requested X position.
   *
   * Independent of layout passes; returns the last value set via
   * SetRequestedX. To read the current rendered X position
   * (Actor::Property::POSITION_X) instead, use GetPositionX().
   *
   * @return The requested X position
   */
  float GetRequestedX() const;

  /**
   * @brief Sets the Y position requested by the user.
   *
   * @param[in] y The requested Y position
   * @see SetRequestedX
   */
  void SetRequestedY(float y);

  /**
   * @brief Returns the user-requested Y position.
   *
   * @return The requested Y position
   * @see GetRequestedX
   */
  float GetRequestedY() const;

  /**
   * @brief Sets the requested width.
   *
   * A positive value sets a fixed width.
   * WRAP_CONTENT (-1): sizes to content (default).
   * MATCH_PARENT (-2): fills the parent container.
   *
   * @note The raw Dali::Actor size setters (SetSize/SetWidth/SetHeight) are
   * unavailable on View: a View's rendered geometry is owned by the layout
   * system and would be overwritten on the next Arrange. Use this and
   * SetRequestedHeight() for layout-aware sizing; to drive the rendered size
   * directly (e.g. for scrolling or animation that must not feed back into
   * layout), use Dali::Ui::Extension::View::SetSizeWidth(view, width).
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
  void SetMargin(const Insets& margin);

  /**
   * @brief Sets the view margin for each edge.
   *
   * @param[in] start The start margin
   * @param[in] end The end margin
   * @param[in] top The top margin
   * @param[in] bottom The bottom margin
   */
  void SetMargin(float start, float end, float top, float bottom);

  /**
   * @brief Sets the horizontal and vertical view margin.
   *
   * @param[in] horizontal The start and end margin
   * @param[in] vertical The top and bottom margin
   */
  void SetMargin(float horizontal, float vertical);

  /**
   * @brief Sets the same view margin for all edges.
   *
   * @param[in] uniform The margin for all edges
   */
  void SetMargin(float uniform);

  /**
   * @brief Sets the start view margin.
   */
  void SetStartMargin(float margin);

  /**
   * @brief Sets the end view margin.
   */
  void SetEndMargin(float margin);

  /**
   * @brief Sets the top view margin.
   */
  void SetTopMargin(float margin);

  /**
   * @brief Sets the bottom view margin.
   */
  void SetBottomMargin(float margin);

  /**
   * @brief Gets the view margin.
   *
   * @return The view margin
   */
  Insets GetMargin() const;

  /**
   * @brief Sets the view padding.
   *
   * @param[in] padding The padding to set
   */
  void SetPadding(const Insets& padding);

  /**
   * @brief Sets the view padding for each edge.
   *
   * @param[in] start The start padding
   * @param[in] end The end padding
   * @param[in] top The top padding
   * @param[in] bottom The bottom padding
   */
  void SetPadding(float start, float end, float top, float bottom);

  /**
   * @brief Sets the horizontal and vertical view padding.
   *
   * @param[in] horizontal The start and end padding
   * @param[in] vertical The top and bottom padding
   */
  void SetPadding(float horizontal, float vertical);

  /**
   * @brief Sets the same view padding for all edges.
   *
   * @param[in] uniform The padding for all edges
   */
  void SetPadding(float uniform);

  /**
   * @brief Sets the start view padding.
   */
  void SetStartPadding(float padding);

  /**
   * @brief Sets the end view padding.
   */
  void SetEndPadding(float padding);

  /**
   * @brief Sets the top view padding.
   */
  void SetTopPadding(float padding);

  /**
   * @brief Sets the bottom view padding.
   */
  void SetBottomPadding(float padding);

  /**
   * @brief Gets the view padding.
   *
   * @return The view padding
   */
  Insets GetPadding() const;

  // The Dali::Actor geometry setters below are deleted on View because a View's
  // rendered geometry (size and position) is owned by the layout system: every
  // Measure/Arrange pass writes the Actor POSITION_* and SIZE_* properties
  // directly (see OnArrange), so any value set through them would be silently
  // overwritten on the next layout pass. They are kept private so the
  // invoke-method generator, which scans only public members, does not emit
  // wrappers that would call the deleted functions. Public callers use the
  // layout-aware API (SetRequestedWidth/Height, SetRequestedX/Y) or the
  // Dali::Ui::Extension geometry setters instead.
private:
  void SetSize(const Vector3& size)         = delete;
  void SetWidth(float width)                = delete;
  void SetHeight(float height)              = delete;
  void SetDepth(float depth)                = delete;
  void SetPosition(const Vector3& position) = delete;
  void SetPositionX(float x)                = delete;
  void SetPositionY(float y)                = delete;
  void SetPositionZ(float z)                = delete;

public:
  /**
   * @brief Sets the layout mode of this View.
   *
   * LayoutMode::DEFAULT (default) lets the parent container's layout system manage
   * this View's size and position normally.
   *
   * LayoutMode::STANDALONE excludes this View from the parent's accumulation,
   * spacing and index calculations. The View's size is still measured normally
   * (so MATCH_PARENT, WRAP_CONTENT and explicit RequestedWidth/Height all work),
   * but its position is taken from SetRequestedX/SetRequestedY instead of being
   * decided by the parent layout. This is useful for floating overlays, drag
   * previews, tooltips and absolute positioning inside any LayoutManager.
   *
   * Standalone children ignore the parent's padding entirely. Their measured
   * size is the parent's full inner size minus the child's own margin, and
   * their final position is SetRequestedX/SetRequestedY plus the child's own
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
   * The callback target must remain alive until the callback is replaced or
   * cleared. Pass an empty callback to restore the View's virtual policy.
   * The callback must return a result instead of changing focus directly.
   *
   * @param[in] callback The focus navigation callback (move-only, ownership transferred)
   */
  void SetFocusNavigationCallback(FocusNavigationCallback callback);

  /**
   * @brief Sets a solid color background.
   *
   * This replaces any image or gradient background previously set on this View.
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
   * @brief Gets the background color.
   *
   * If the background color was set from a token UiColor, the original token
   * color is returned.
   *
   * @return The background color
   */
  UiColor GetBackgroundColor();

  /**
   * @brief Sets an image background from a resource URL.
   *
   * This replaces any color or gradient background previously set on this View.
   *
   * @param[in] url The image resource URL
   */
  void SetBackgroundImage(const Dali::String& url);

  /**
   * @brief Sets a gradient background.
   *
   * This replaces any color or image background previously set on this View.
   *
   * @param[in] gradient The gradient value used to configure the background
   */
  void SetBackgroundGradient(const Gradient::Base& gradient);

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
   * The View stores an independent copy of the parameters and invalidates its
   * measure cache. Changes made to @p params after this call do not affect the
   * View.
   *
   * @param[in] params The layout parameters to attach to this View
   *
   * @code
   * auto params = AbsoluteLayoutParams::New();
   * params.SetBounds(LayoutRect(10, 20, 100, 200));
   * params.SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
   * child.SetLayoutParams(params);
   * @endcode
   */
  void SetLayoutParams(const AbsoluteLayoutParams& params);
  void SetLayoutParams(const FlexLayoutParams& params);
  void SetLayoutParams(const GridLayoutParams& params);
  void SetLayoutParams(const StackLayoutParams& params);

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
   * @brief Removes all children from this View immediately.
   *
   * Equivalent to @c RemoveAllChildren(RemovePolicy::IMMEDIATE): unparents every
   * child now, running no EXIT transition. A child already leaving via a prior
   * @c Remove(child, RemovePolicy::ANIMATE_EXIT) finishes its EXIT rather than
   * being force-unparented (see the @p policy overload). Mirrors the inherited
   * one-argument @c Actor::Remove.
   */
  void RemoveAllChildren();

  /**
   * @brief Removes all children from this View, choosing whether to run the
   * attached LayoutTransition's EXIT slot first.
   *
   * Removes each child of this View. With @c RemovePolicy::ANIMATE_EXIT and a
   * configured EXIT spec, animator, or bounds effect (this view's own slot, or
   * an ancestor SUBTREE-scope owner's), each View child is kept in the actor
   * tree as a "ghost" until its EXIT animation finishes and is then unparented;
   * children with no EXIT slot are unparented immediately.
   * @c RemovePolicy::IMMEDIATE unparents now.
   *
   * @note Non-View actor children (which can only be attached via the
   * integration API @c Dali::Ui::Integration::View::AddActorChild) are always
   * unparented immediately, regardless of @p policy.
   *
   * @note A child that is already leaving via a prior
   * @c Remove(child, RemovePolicy::ANIMATE_EXIT) — an in-flight EXIT ghost — is
   * left to finish its EXIT and is NOT force-unparented, even under
   * @c RemovePolicy::IMMEDIATE (matching per-child @c Remove(child, RemovePolicy)).
   *
   * @param[in] policy Whether to animate the EXIT transition or unparent immediately
   */
  void RemoveAllChildren(RemovePolicy policy);

  /**
   * @brief Number of View children in this view's LOGICAL (layout) child list.
   *
   * The logical child list is the set of @c View children the layout enumerates
   * in @c OnMeasure / @c OnArrange. It EXCLUDES in-flight EXIT ghosts (a child
   * removed via @c Remove(child, RemovePolicy::ANIMATE_EXIT) that is still
   * attached to the actor tree until its EXIT animation finishes) and EXCLUDES
   * non-View actor children (attached via the integration API
   * @c Dali::Ui::Integration::View::AddActorChild). It INCLUDES
   * @c LayoutMode::STANDALONE children: they are View children of this view,
   * they are simply not laid out by this view's own measure/arrange.
   *
   * This is distinct from the inherited actor-tree @c Dali::Actor::GetChildCount,
   * which counts every attached actor including EXIT ghosts and non-View actors.
   *
   * @return The number of logical View children
   */
  uint32_t GetChildViewCount() const;

  /**
   * @brief The View child at @p index in this view's LOGICAL (layout) child list.
   *
   * Returns a @c Ui::View directly (no @c DownCast required at the call site).
   * The index space is the logical child list described in
   * @c GetChildViewCount: EXIT ghosts and non-View actor children are excluded,
   * @c LayoutMode::STANDALONE children are included. This is distinct from the
   * inherited actor-tree @c Dali::Actor::GetChildAt, whose index space includes
   * ghosts and non-View actors.
   *
   * @param[in] index The logical index, in [0, GetChildViewCount())
   * @return The logical View child at @p index, or an empty handle if out of range
   */
  View GetChildViewAt(uint32_t index) const;

  /**
   * @brief The logical index of @p childView in this view's LOGICAL child list.
   *
   * @param[in] childView The child to locate
   * @return The logical index of @p childView, or -1 if it is not a (logical)
   * child of this view (e.g. it is an in-flight EXIT ghost, a non-View actor,
   * or not a child at all)
   *
   * @note Inverse of @c GetChildViewAt: for any logical child @p x the invariant
   * @c GetChildViewAt(IndexOfChildView(x)) == x holds.
   */
  int32_t IndexOfChildView(View childView) const;

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
   * @note During the EXIT animation the child is still attached to the
   * actor tree (so the inherited @c Dali::Actor::GetChildCount /
   * @c Dali::Actor::GetChildAt still count and return it) but is logically
   * absent from this view's layout child list. Re-adding the SAME child to
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

  // Re-expose the inherited one-argument immediate remove; the
  // Remove(View, RemovePolicy) overload would otherwise hide it (C++ name hiding).
  using Dali::Actor::Remove;

  /**
   * @brief Copies the attached layout parameters into @p params.
   *
   * @param[out] params Receives an independent copy of the attached parameters;
   *                    left unchanged if none of the requested type are attached
   * @return true if parameters of the requested type are attached; otherwise
   *         false, leaving @p params unchanged
   */
  bool TryGetLayoutParams(AbsoluteLayoutParams& params) const;
  bool TryGetLayoutParams(FlexLayoutParams& params) const;
  bool TryGetLayoutParams(GridLayoutParams& params) const;
  bool TryGetLayoutParams(StackLayoutParams& params) const;

public: // State API (non-chaining)
  using StateChangedSignalType   = Signal<void(View, StateEvent)>;
  using LayoutFinishedSignalType = Signal<void(View, LayoutRect)>;

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

  /**
   * @brief Emitted when this View's layout has fully settled.
   *
   * Fires once per dirty-to-settled layout episode of this View's window, after
   * all Measure/Arrange work for the window has drained (same settle point as
   * LayoutController::LayoutFinishedSignal). Emitted during the post-process
   * phase, i.e. AFTER DALi core size negotiation (Relayout); Measure/Arrange
   * still run in the pre-process phase and only the emit is deferred. The
   * callback receives this View and its arranged target as a LayoutRect in
   * PARENT-relative, visual (scale-applied) units. Under RIGHT_TO_LEFT the x is
   * the mirrored (final) position. The bounds are the PRE-transition target,
   * snapshotted during arrange, not intermediate animated values.
   *
   * Recurs: if a slot invalidates layout again, the View is re-arranged and the
   * signal fires again on a later settled pass. Connecting after a layout pass
   * does not replay the previous result.
   *
   * @warning A slot that UNCONDITIONALLY triggers a layout recalculation (e.g.
   * always sets a size/position/layout property, calls a method that invalidates
   * measure/arrange, or adds/removes children) will spin an ENDLESS
   * dirty->settled->emit cycle: each emit re-invalidates layout, which schedules
   * another settled pass that emits again, and so on (the event loop is kept
   * awake via the idle-process request). There is intentionally no iteration cap
   * (as with LayoutController::LayoutFinishedSignal and equivalents in other
   * toolkits). Also note this signal fires whenever the View is (re-)arranged in
   * a settled pass, INCLUDING when its bounds did NOT change (e.g. it was
   * re-arranged only because a sibling or ancestor changed) -- do NOT assume
   * "signal fired" means "this View's geometry changed". Guard any layout-
   * affecting work in the slot behind a real condition, e.g. compare @p bounds
   * against a value you cached from the previous emit and act only on an actual
   * change, or use a one-shot flag.
   *
   * @note Fires only for a View whose own Arrange() runs during the pass. All
   * built-in LayoutManagers and the default arrange route through child.Arrange().
   * A custom parent ArrangeCallback that positions a non-standalone child by
   * writing Actor properties directly, without calling that child's Arrange(),
   * will NOT fire the child's signal. This is not a completion callback for a
   * manual View::Arrange().
   *
   * @return The layout-finished signal
   */
  LayoutFinishedSignalType& LayoutFinishedSignal();

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
  explicit View(ViewImpl& implementation);

  /**
   * @brief Allows the creation of this View from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit View(Dali::Internal::CustomActor* internal);
  /// @endcond

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
       * @details Name "margin", type Property::VECTOR4, ordered as start, end, top, bottom.
       * @note Margin property is to be supported by Layout algorithms and containers in future.
       */
      MARGIN,

      /**
       * @brief The inner space of the View.
       * @details Name "padding", type Property::VECTOR4, ordered as start, end, top, bottom.
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
       * @brief Whether a View and its descendants can emit key signals.
       * @details Name "dispatchKeyEvents", type Property::BOOLEAN
       * @note If a View's dispatchKeyEvents is set to false, then it's children will not emit a key event signal
       * either.
       */
      DISPATCH_KEY_EVENTS,

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
       * @brief Whether to draw on offscreen of not.
       * @details Name "offscreenRendering", type Property::INTEGER.
       * @note Default is false.
       */
      OFFSCREEN_RENDERING,

      /**
       * @brief The inner shadow of the View. The visual will use Dali::Ui::Integration::DepthIndex::Ranges::DECORATION - 2
       * @details Name "innerShadow", type Property::MAP.
       */
      INNER_SHADOW,

      /**
       * @brief The inset borderline of the View. The visual will use Dali::Ui::Integration::DepthIndex::Ranges::DECORATION - 1
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
       * @see Dali::Ui::Integration::Visual::Property::Type::CORNER_RADIUS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_START_INDEX,

      /**
       * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute
       * (in world units).
       * @details Name "viewCornerRadiusPolicy", type Property::INTEGER.
       * @see Policy::Type
       * @see Dali::Ui::Integration::Visual::Property::Type::CORNER_RADIUS_POLICY
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS_POLICY,

      /**
       * @brief The squareness for the rounded corners of the View.
       * @details Name "viewCornerSquareness", type Property::VECTOR4 or Property::FLOAT
       * @note By default, it is Vector::ZERO.
       * @note Applies to specific visuals inside the View.
       * @note Only Property::Vector4 can be animated.
       * @see Dali::Ui::Integration::Visual::Property::Type::CORNER_SQUARENESS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_SQUARENESS,

      /**
       * @brief The width for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineWidth", type Property::FLOAT.
       * @see Dali::Ui::Integration::Visual::Property::Type::BORDERLINE_WIDTH
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_WIDTH,

      /**
       * @brief The color for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineColor", type Property::VECTOR4
       * @see Dali::Ui::Integration::Visual::Property::Type::BORDERLINE_COLOR
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_COLOR,

      /**
       * @brief The offset for the borderline of the View. It will update borderline visual
       * @details Name "viewBorderlineOffset", type Property::FLOAT.
       * @see Dali::Ui::Integration::Visual::Property::Type::BORDERLINE_OFFSET
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

  /// @brief Accessibility reading lifecycle signal type.
  using AccessibilityReadingStatusChangedSignalType = Signal<void(View, Accessibility::ReadingStatus)>;

  /// @brief Accessibility highlight state signal type.
  using AccessibilityHighlightedSignalType = Signal<void(View, bool)>;

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
   * @brief Sets the explicit accessibility name.
   *
   * @param[in] name The accessibility name
   */
  void SetAccessibilityName(StringView name);

  /**
   * @brief Returns the explicit or resolved accessibility name.
   *
   * @return The accessibility name
   */
  Dali::String GetAccessibilityName() const;

  /**
   * @brief Sets the explicit accessibility description.
   *
   * @param[in] description The accessibility description
   */
  void SetAccessibilityDescription(StringView description);

  /**
   * @brief Returns the explicit or resolved accessibility description.
   *
   * @return The accessibility description
   */
  Dali::String GetAccessibilityDescription() const;

  /**
   * @brief Sets the accessibility value.
   *
   * @param[in] value The accessibility value
   */
  void SetAccessibilityValue(StringView value);

  /**
   * @brief Returns the accessibility value.
   *
   * @return The accessibility value
   */
  Dali::String GetAccessibilityValue() const;

  /**
   * @brief Sets the accessibility role.
   *
   * @param[in] role The accessibility role
   */
  void SetAccessibilityRole(Accessibility::Role role);

  /**
   * @brief Returns the accessibility role.
   *
   * @return The accessibility role
   */
  Accessibility::Role GetAccessibilityRole() const;

  /**
   * @brief Sets whether this View is hidden from accessibility clients.
   *
   * @param[in] hidden Whether this View is hidden from accessibility clients
   */
  void SetAccessibilityHidden(bool hidden);

  /**
   * @brief Returns whether this View is hidden from accessibility clients.
   *
   * @return True if this View is hidden from accessibility clients, false otherwise
   */
  bool IsAccessibilityHidden() const;

  /**
   * @brief Overrides whether this View can receive accessibility highlight.
   *
   * @param[in] highlightable Whether this View can receive accessibility highlight
   */
  void SetAccessibilityHighlightable(bool highlightable);

  /**
   * @brief Restores role-based accessibility highlight behavior.
   */
  void ResetAccessibilityHighlightable();

  /**
   * @brief Returns the effective accessibility highlightable state.
   *
   * @return True if this View can receive accessibility highlight, false otherwise
   */
  bool IsAccessibilityHighlightable() const;

  /**
   * @brief Sets whether this View exposes accessibility scrolling behavior.
   *
   * @param[in] scrollable Whether this View exposes accessibility scrolling behavior
   */
  void SetAccessibilityScrollable(bool scrollable);

  /**
   * @brief Returns whether accessibility scrolling behavior is enabled.
   *
   * @return True if accessibility scrolling behavior is enabled, false otherwise
   */
  bool IsAccessibilityScrollable() const;

  /**
   * @brief Sets whether this View is accessibility-modal.
   *
   * @param[in] modal Whether this View is accessibility-modal
   */
  void SetAccessibilityModal(bool modal);

  /**
   * @brief Returns whether this View is accessibility-modal.
   *
   * @return True if this View is accessibility-modal, false otherwise
   */
  bool IsAccessibilityModal() const;

  /**
   * @brief Sets application automation metadata for this View.
   *
   * @param[in] automationId The application automation identifier
   */
  void SetAutomationId(StringView automationId);

  /**
   * @brief Returns application automation metadata for this View.
   *
   * @return The application automation identifier
   */
  Dali::String GetAutomationId() const;

  /**
   * @brief Binds the accessibility name to a resource in the default domain.
   *
   * @param[in] resourceId The localization resource identifier
   */
  void SetTranslatableAccessibilityName(StringView resourceId);

  /**
   * @brief Binds the accessibility name to a resource and domain.
   *
   * @param[in] resourceId The localization resource identifier
   * @param[in] domain The localization domain
   */
  void SetTranslatableAccessibilityName(StringView resourceId, StringView domain);

  /**
   * @brief Returns the resource id bound to the accessibility name.
   *
   * @return The localization resource identifier, or an empty string if no binding exists
   */
  Dali::String GetTranslatableAccessibilityName() const;

  /**
   * @brief Removes the accessibility name localization binding.
   */
  void ClearTranslatableAccessibilityName();

  /**
   * @brief Binds the accessibility description to a resource in the default domain.
   *
   * @param[in] resourceId The localization resource identifier
   */
  void SetTranslatableAccessibilityDescription(StringView resourceId);

  /**
   * @brief Binds the accessibility description to a resource and domain.
   *
   * @param[in] resourceId The localization resource identifier
   * @param[in] domain The localization domain
   */
  void SetTranslatableAccessibilityDescription(StringView resourceId, StringView domain);

  /**
   * @brief Returns the resource id bound to the accessibility description.
   *
   * @return The localization resource identifier, or an empty string if no binding exists
   */
  Dali::String GetTranslatableAccessibilityDescription() const;

  /**
   * @brief Removes the accessibility description localization binding.
   */
  void ClearTranslatableAccessibilityDescription();

  /**
   * @brief Adds an accessibility relation to another View.
   *
   * @param[in] type The accessibility relation type
   * @param[in] target The target View
   */
  void AddAccessibilityRelation(Accessibility::RelationType type, View target);

  /**
   * @brief Removes an accessibility relation to another View.
   *
   * @param[in] type The accessibility relation type
   * @param[in] target The target View
   */
  void RemoveAccessibilityRelation(Accessibility::RelationType type, View target);

  /**
   * @brief Removes all accessibility relations.
   */
  void ClearAccessibilityRelations();

  /**
   * @brief Returns whether the specified accessibility relation exists.
   *
   * @param[in] type The accessibility relation type
   * @param[in] target The target View
   * @return True if the relation exists, false otherwise
   */
  bool HasAccessibilityRelation(Accessibility::RelationType type, View target) const;

  /**
   * @brief Adds information to be announced when this View is read.
   *
   * @param[in] info The accessibility reading information to add
   */
  void AddAccessibilityReadingInfo(Accessibility::ReadingInfo info);

  /**
   * @brief Removes information from the set announced for this View.
   *
   * @param[in] info The accessibility reading information to remove
   */
  void RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo info);

  /**
   * @brief Clears all information announced for this View.
   */
  void ClearAccessibilityReadingInfo();

  /**
   * @brief Returns whether the specified information is announced.
   *
   * @param[in] info The accessibility reading information to query
   * @return True if the information is announced, false otherwise
   */
  bool HasAccessibilityReadingInfo(Accessibility::ReadingInfo info) const;

  /**
   * @brief Adds a code-point-based language span to the accessibility name.
   *
   * @param[in] start The zero-based code-point start index
   * @param[in] length The number of code points in the span
   * @param[in] locale The locale associated with the span
   * @return True if the language span was added, false otherwise
   */
  bool AddAccessibilityNameLanguageSpan(uint32_t start, uint32_t length, StringView locale);

  /**
   * @brief Clears all language spans from the accessibility name.
   */
  void ClearAccessibilityNameLanguageSpans();

  /**
   * @brief Adds a code-point-based language span to the accessibility description.
   *
   * @param[in] start The zero-based code-point start index
   * @param[in] length The number of code points in the span
   * @param[in] locale The locale associated with the span
   * @return True if the language span was added, false otherwise
   */
  bool AddAccessibilityDescriptionLanguageSpan(uint32_t start, uint32_t length, StringView locale);

  /**
   * @brief Clears all language spans from the accessibility description.
   */
  void ClearAccessibilityDescriptionLanguageSpans();

  /**
   * @brief Sets whether initial accessibility highlight is requested.
   *
   * @param[in] request Whether initial accessibility highlight is requested
   */
  void SetRequestInitialAccessibilityHighlight(bool request);

  /**
   * @brief Returns whether initial accessibility highlight is requested.
   *
   * @return True if initial accessibility highlight is requested, false otherwise
   */
  bool IsInitialAccessibilityHighlightRequested() const;

  /**
   * @brief Sets whether this View is an accessibility collection container.
   *
   * @param[in] container Whether this View is an accessibility collection container
   */
  void SetAccessibilityCollectionContainer(bool container);

  /**
   * @brief Returns whether this View is an accessibility collection container.
   *
   * @return True if this View is an accessibility collection container, false otherwise
   */
  bool IsAccessibilityCollectionContainer() const;

  /**
   * @brief Sets this View's zero-based index in an accessibility collection.
   *
   * @param[in] index The zero-based collection index, or -1 to clear it
   */
  void SetAccessibilityCollectionIndex(int32_t index);

  /**
   * @brief Returns the collection index, or -1 when it is not set.
   *
   * @return The zero-based collection index, or -1 when it is not set
   */
  int32_t GetAccessibilityCollectionIndex() const;

  /**
   * @brief Clears this View's accessibility collection index.
   */
  void ClearAccessibilityCollectionIndex();

  /**
   * @brief Adds or replaces a raw accessibility attribute.
   *
   * @param[in] key The accessibility attribute key
   * @param[in] value The accessibility attribute value
   */
  void AppendAccessibilityAttribute(StringView key, StringView value);

  /**
   * @brief Removes a raw accessibility attribute.
   *
   * @param[in] key The accessibility attribute key
   */
  void RemoveAccessibilityAttribute(StringView key);

  /**
   * @brief Removes all raw and typed accessibility attributes.
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief Adds the given accessibility state to this View.
   *
   * @SINCE_2_5.30
   * @param[in] state The accessibility state to enable
   * @see Dali::Ui::Accessibility::State
   */
  void AddAccessibilityState(Accessibility::State state);

  /**
   * @brief Removes the given accessibility state from this View.
   *
   * @SINCE_2_5.30
   * @param[in] state The accessibility state to disable
   * @see Dali::Ui::Accessibility::State
   */
  void RemoveAccessibilityState(Accessibility::State state);

  /**
   * @brief Clears all accessibility states from this View.
   *
   * @SINCE_2_5.30
   */
  void ClearAccessibilityStates();

  /**
   * @brief Notifies accessibility clients that the active descendant changed.
   *
   * This is typically used by container Views (e.g. lists, combo boxes) to
   * indicate which child is currently active (focused or selected).
   *
   * The @p descendant should be a child View of this View. While a deeper
   * descendant (e.g. a grandchild) is also accepted, a direct child is
   * recommended so that assistive technologies can correctly identify the
   * relationship.
   *
   * @SINCE_2_5.34
   * @param[in] descendant The active descendant View, which should be a child
   *                       of this View. An empty handle is allowed and clears
   *                       the current active descendant.
   */
  void NotifyAccessibilityActiveDescendantChanged(View descendant);

  /**
   * @brief Returns whether the given accessibility state is set on this View.
   *
   * @SINCE_2_5.30
   * @param[in] state The accessibility state to query
   * @return True if the state is set
   * @see Dali::Ui::Accessibility::State
   */
  bool HasAccessibilityState(Accessibility::State state) const;

  /**
   * @brief Returns the signal emitted when this View's reading lifecycle changes.
   *
   * @return The accessibility reading status changed signal
   */
  AccessibilityReadingStatusChangedSignalType& AccessibilityReadingStatusChangedSignal();

  /**
   * @brief Returns the signal emitted when accessibility highlight changes.
   *
   * @return The accessibility highlighted signal
   */
  AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal();

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
   * @note This signal is only emitted when OffScreenRenderingType is set to REFRESH_ONCE.
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

  /**
   * @brief Sets the inner shadow for this View.
   *
   * Pass InnerShadow::None() to clear the inner shadow from this View.
   *
   * @param[in] innerShadow The inner shadow value to apply
   */
  void SetInnerShadow(const InnerShadow& innerShadow);

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
