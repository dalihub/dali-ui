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
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/dali-utility.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/clickable-trait.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layout-params.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/view-accessibility-enums.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
class Actor;
class Any;
class Constraint;

namespace Ui
{
class RenderEffect;

namespace Visual
{
class Base;
}

// Forward declarations
class UiColor;

namespace Integration
{
class ViewImpl;
}

#include "view.autogen.h"
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
  // Typedefs

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
   * @brief Gets the desired size of the view after measurement.
   *
   * @return The desired size
   */
  MeasuredSize GetDesiredSize() const;

  /**
   * @brief Checks if the measure is valid.
   *
   * @return True if measure is valid
   */
  bool IsMeasureValid() const;

  /**
   * @brief Checks if the arrange is valid.
   *
   * @return True if arrange is valid
   */
  bool IsArrangeValid() const;

public: // Properties
  // @CHAIN_START(View)
  /**
   * @brief Sets the width of the View.
   *
   * @param[in] width The width to set
   */
  View& SetSizeWidth(float width);

  /**
   * @brief Gets the width of the View.
   *
   * @return The width of the View
   */
  float GetSizeWidth() const;

  /**
   * @brief Sets the height of the View.
   *
   * @param[in] height The height to set
   */
  View& SetSizeHeight(float height);

  /**
   * @brief Gets the height of the View.
   *
   * @return The height of the View
   */
  float GetSizeHeight() const;

  /**
   * @brief Sets the X position of the View.
   *
   * @param[in] x The X position to set
   */
  View& SetPositionX(float x);

  /**
   * @brief Gets the X position of the View.
   *
   * @return The X position of the View
   */
  float GetPositionX() const;

  /**
   * @brief Sets the Y position of the View.
   *
   * @param[in] y The Y position to set
   */
  View& SetPositionY(float y);

  /**
   * @brief Gets the Y position of the View.
   *
   * @return The Y position of the View
   */
  float GetPositionY() const;

  /**
   * @brief Sets the parent origin of the View.
   *
   * @param[in] point The parent origin to set
   */
  View& SetParentOrigin(const Vector3& point);

  /**
   * @brief Gets the parent origin of the View.
   *
   * @return The parent origin of the View
   */
  Vector3 GetParentOrigin() const;

  /**
   * @brief Sets the pivot point of the View.
   *
   * @param[in] point The pivot point to set
   */
  View& SetPivotPoint(const Vector3& point);

  /**
   * @brief Gets the pivot point of the View.
   *
   * @return The pivot point of the View
   */
  Vector3 GetPivotPoint() const;

  /**
   * @brief Sets the layout width.
   *
   * Use LayoutDimension::WrapContent for content-based sizing,
   * LayoutDimension::MatchParent to fill parent, or a positive value for fixed size.
   *
   * @param[in] width The layout width
   */
  View& SetLayoutWidth(float width);

  /**
   * @brief Gets the layout width.
   *
   * @return The layout width
   */
  float GetLayoutWidth() const;

  /**
   * @brief Sets the layout height.
   *
   * @param[in] height The layout height
   */
  View& SetLayoutHeight(float height);

  /**
   * @brief Gets the layout height.
   *
   * @return The layout height
   */
  float GetLayoutHeight() const;

  /**
   * @brief Sets the minimum width.
   *
   * @param[in] width The minimum width
   */
  View& SetMinimumWidth(float width);

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
  View& SetMinimumHeight(float height);

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
   */
  View& SetMaximumWidth(float width);

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
   */
  View& SetMaximumHeight(float height);

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
  View& SetViewMargin(const Extents& margin);

  /**
   * @brief Gets the view margin.
   *
   * @return The view margin
   */
  Extents GetViewMargin() const;

  /**
   * @brief Sets the view padding.
   *
   * @param[in] padding The padding to set
   */
  View& SetViewPadding(const Extents& padding);

  /**
   * @brief Gets the view padding.
   *
   * @return The view padding
   */
  Extents GetViewPadding() const;

  /**
   * @brief Sets the horizontal alignment within parent layout.
   *
   * @param[in] alignment The horizontal alignment
   */
  View& SetHorizontalAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the horizontal alignment.
   *
   * @return The horizontal alignment
   */
  LayoutAlignment GetHorizontalAlignment() const;

  /**
   * @brief Sets the vertical alignment within parent layout.
   *
   * @param[in] alignment The vertical alignment
   */
  View& SetVerticalAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the vertical alignment.
   *
   * @return The vertical alignment
   */
  LayoutAlignment GetVerticalAlignment() const;

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
  View& SetFocusable(bool focusable);

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
  View& SetTouchFocusable(bool touchFocusable);

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
  View& SetBackgroundColor(const UiColor& color);

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the clickable interaction role to this View and optionally configures it.
   *
   * A View can have at most one interaction trait for its lifetime; attaching clickable
   * succeeds only if no other interaction trait is set. If the View already has a
   * ClickableTrait (e.g. from a previous AsClickable call), the existing trait is
   * used and the configure callback is invoked with it.
   *
   * The callback is invoked in the caller's translation unit, so no std::function
   * crosses the library ABI boundary; this preserves ABI stability across toolchains.
   *
   * @param[in] configure Optional callback to configure the ClickableTrait (e.g. connect signals).
   *                     Can be null or omitted to only attach the trait.
   * @return Reference to this View for fluent chaining
   */
  View& AsClickable(std::function<void(ClickableTrait&)> configure = nullptr)
  {
    ClickableTrait trait = GetOrAttachClickableTrait();
    if(configure && trait)
    {
      configure(trait);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the clickable trait and connects a member function to the Clicked signal.
   *
   * Equivalent to AsClickable() then ClickedSignal().Connect(obj, func).
   * @param[in] obj Object that implements ConnectionTrackerInterface (e.g. ConnectionTracker subclass); used for
   * automatic disconnection
   * @param[in] func Member function with signature bool (View, const InputEvent&)
   * @return Reference to this View for fluent chaining
   */
  template<class X>
  View& AsClickable(X* obj, bool (X::*func)(View, const InputEvent&))
  {
    ClickableTrait trait = GetOrAttachClickableTrait();
    if(trait && obj && func)
    {
      trait.ClickedSignal().Connect(obj, func);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the clickable trait and connects a callable to the Clicked signal.
   *
   * Equivalent to AsClickable() then ClickedSignal().Connect(connectionTracker, func).
   * @param[in] connectionTracker Used for automatic disconnection when the tracker is destroyed
   * @param[in] func Callable with signature bool (View, const InputEvent&) (e.g. lambda)
   * @return Reference to this View for fluent chaining
   */
  template<typename F>
  View& AsClickable(Dali::ConnectionTrackerInterface* connectionTracker, F&& func)
  {
    ClickableTrait trait = GetOrAttachClickableTrait();
    if(trait && connectionTracker)
    {
      trait.ClickedSignal().Connect(connectionTracker, std::forward<F>(func));
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Assigns this View instance to a target variable.
   * This method is useful for capturing a reference to a View created within
   * a declarative UI tree for later use.
   */
  View& As(View& self)
  {
    self = static_cast<View&>(*this);
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Executes a custom action on this View instance.
   * Use this method to perform additional initialization or logic on a View
   * without breaking the declarative method chaining.
   * @param[in] action A function or lambda to be executed with this instance.
   */
  View& With(std::function<void(View&)> action)
  {
    if(action)
    {
      action(*this);
    }
    return *this;
  }

  /**
   * @brief Sets layout parameters on this View.
   *
   * The params handle is stored on the View as-is, and the View's
   * measure cache is invalidated.
   *
   * @param[in] params The layout parameters to attach to this View
   * @return Reference to this View for fluent chaining
   *
   * @code
   * child.SetLayoutParams(
   *   AbsoluteLayoutParams::New()
   *     .SetBounds(LayoutRect(10, 20, 100, 200))
   *     .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
   * @endcode
   */
  View& SetLayoutParams(LayoutParams params);

  // @CHAIN_END

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
    return T::DownCast(GetLayoutParamsTrait(T::GetLayoutParamsType()));
  }

public: // Clickable role accessors (non-chaining)
  /**
   * @brief Ensures this View has a clickable interaction trait and returns it.
   *
   * If no interaction trait is set, a ClickableTrait is attached and returned.
   * If a ClickableTrait is already attached, it is returned. If a different
   * interaction trait is set, an assertion may fire and an empty handle is returned.
   *
   * @return ClickableTrait handle, or an uninitialized handle on error
   */
  ClickableTrait GetOrAttachClickableTrait();

  /**
   * @brief Returns the clickable interaction trait if this View has one.
   *
   * Use this in non-fluent code paths to obtain the trait after AsClickable(), or when
   * the View was made clickable by other means (e.g. a View that attaches the trait).
   *
   * @return ClickableTrait handle if this View has a clickable trait; otherwise an uninitialized handle
   */
  ClickableTrait GetClickableTrait() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The View implementation
   */
  explicit DALI_UI_API View(Integration::ViewImpl& implementation);

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
  BaseHandle GetLayoutParamsTrait(LayoutParamsType type) const;

  // From Control.h
  // TODO: Refactoring

public:
  /**
   * @brief Enumeration for the start and end property ranges for View.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX =
      PROPERTY_REGISTRATION_START_INDEX,                         ///< Start index is used by the property registration macro. @SINCE_1_0.0
    VIEW_PROPERTY_START_INDEX = PROPERTY_START_INDEX,            ///< Start index of View properties. @SINCE_1_0.0
    VIEW_PROPERTY_END_INDEX   = VIEW_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices. @SINCE_1_0.0
  };

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
   * @brief Enumeration for the instance of properties belonging to the View class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the View class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief Receives key events to the View.
       * @details Name "keyInputFocus", type Property::BOOLEAN.
       * @see Ui::View::SetKeyInputFocus()
       * @SINCE_1_0.0
       */
      KEY_INPUT_FOCUS = PROPERTY_START_INDEX,

      /**
       * @brief The background of the View.
       *
       * @details Name "background", type Property::MAP or Dali::String for URL or Property::VECTOR4 for Color.
       * @SINCE_1_1.3
       */
      BACKGROUND,

      /**
       * @brief The outer space around the View.
       * @details Name "margin", type Property::EXTENTS.
       * @SINCE_1_2.62
       * @note Margin property is to be supported by Layout algorithms and containers in future.
       */
      MARGIN,

      /**
       * @brief The inner space of the View.
       * @details Name "padding", type Property::EXTENTS.
       * @SINCE_1_2.62
       */
      PADDING,

      /**
       * @brief The actor ID of the left focusable View.
       * @details Name "leftFocusableActorId", type Property::INTEGER.
       */
      LEFT_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the right focusable View.
       * @details Name "rightFocusableActorId", type Property::INTEGER.
       */
      RIGHT_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the up focusable View.
       * @details Name "upFocusableActorId", type Property::INTEGER.
       */
      UP_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the down focusable View.
       * @details Name "downFocusableActorId", type Property::INTEGER.
       */
      DOWN_FOCUSABLE_ACTOR_ID,

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
       * @brief The actor ID of the clockwise focusable View.
       * @details Name "clockwiseFocusableActorId", type Property::INTEGER.
       */
      CLOCKWISE_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the conter-clockwise focusable View.
       * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
       */
      COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID,

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
       * @brief The radius for the rounded corners of the View.
       * @details Name "viewCornerRadius", type Property::VECTOR4 or Property::FLOAT
       * @note By default, it is Vector::ZERO.
       * @note Applies to specific visuals inside the View.
       * @note Only Property::Vector4 can be animated.
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_RADIUS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,

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
      BORDERLINE_OFFSET
    };
  };

  /**
   * @brief Describes the direction to move the keyboard focus towards.
   * @SINCE_1_0.0
   */
  struct KeyboardFocus
  {
    /**
     * @brief Keyboard focus direction.
     * @SINCE_1_0.0
     */
    enum Direction
    {
      LEFT,              ///< Move keyboard focus towards the left direction @SINCE_1_0.0
      RIGHT,             ///< Move keyboard focus towards the right direction @SINCE_1_0.0
      UP,                ///< Move keyboard focus towards the up direction @SINCE_1_0.0
      DOWN,              ///< Move keyboard focus towards the down direction @SINCE_1_0.0
      PAGE_UP,           ///< Move keyboard focus towards the previous page direction @SINCE_1_2.14
      PAGE_DOWN,         ///< Move keyboard focus towards the next page direction @SINCE_1_2.14
      FORWARD,           ///< Move keyboard focus towards the forward direction @SINCE_2_1.10
      BACKWARD,          ///< Move keyboard focus towards the backward direction @SINCE_2_1.10
      CLOCKWISE,         ///< Move keyboard focus towards the clockwise direction @SINCE_2_1.14
      COUNTER_CLOCKWISE, ///< Move keyboard focus towards the counter clockwise direction @SINCE_2_1.14
    };

    /**
     * @brief Keyboard focus device.
     * @SINCE_2_5.5
     */
    enum Device
    {
      UNKNOWN,     ///< Unknown device. @SINCE_2_5.5
      KEYBOARD,    ///< A regular keyboard, numberpad or attached buttons. @SINCE_2_5.5
      MOUSE,       ///< A mouse, trackball or touchpad relative motion device. @SINCE_2_5.5
      TOUCH,       ///< A touchscreen with fingers or stylus. @SINCE_2_5.5
      PEN,         ///< A special pen device. @SINCE_2_5.5
      POINTER,     ///< A pointing device based on laser, infrared or similar technology. @SINCE_2_5.5
      GAMEPAD,     ///< A gamepad Viewler or joystick. @SINCE_2_5.5
      WHEEL,       ///< A mouse device. @SINCE_2_5.5
      PROGRAMMATIC ///< Set to API call, not device. @SINCE_2_5.5
    };
  };

  // Typedefs

  /// @brief Key Event signal type. @SINCE_1_0.0
  typedef Signal<bool(View, const KeyEvent&)> KeyEventSignalType;

  /// @brief Key InputFocusType signal type. @SINCE_1_0.0
  typedef Signal<void(View)> KeyInputFocusSignalType;

  /// @brief ResourceReady signal type. @SINCE_1_2.60
  typedef Signal<void(View)> ResourceReadySignalType;

  /// @brief Offscreen rendering finished signal type. @SINCE_2_4.33
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

  /// @brief AccessibilityDoGesture signal type.
  typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

  /// @brief AccessibilityAction signal type.
  typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

  /// @brief AccessibilityHighlighted signal type.
  /// @param bool highlighted true if View is highlighted, false if highlight is removed.
  typedef Signal<void(bool)> AccessibilityHighlightedSignalType;

  /// @brief Visual Event signal type.
  typedef Signal<void(View, Dali::Property::Index, Dali::Property::Index)> VisualEventSignalType;

public: // Creation & Destruction
  /**
   * @brief Additional View behaviour flags for the View constructor.
   * @note TODO : Currunt code is hard-coded. We Should sync type values as
   * CustomActorImpl::ActorFlag and Internal::View::ViewBehaviour in future.
   * @SINCE_2_1.8
   */
  enum ViewBehaviour
  {
    VIEW_BEHAVIOUR_DEFAULT = 0,                          ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                                         ///< but doesn't receive event callbacks. @SINCE_1_2_10
    DISABLE_SIZE_NEGOTIATION = 1 << (0 + 0),             ///< True if View does not need size negotiation, i.e. it can be skipped
                                                         ///< in the algorithm @SINCE_1_0.0
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT = 1 << (4 + 1), ///< True if needs to support keyboard navigation @SINCE_1_0.0
    DISABLE_STYLE_CHANGE_SIGNALS =
      1 << (4 + 2),                 ///< True if View should not monitor style change signals @SINCE_1_2_10
    DISABLE_VISUALS = 1 << (4 + 3), ///< True if View should not use visuals @SINCE_2_3.6
  };

public:
  // Key Input

  /**
   * @brief This sets the View to receive key events.
   *
   * The key event can originate from a virtual or physical keyboard.
   * @SINCE_1_0.0
   * @pre The View has been initialized.
   * @pre The View should be on the stage before setting keyboard focus.
   */
  void SetKeyInputFocus();

  /**
   * @brief Quries whether the View has key input focus.
   *
   * @SINCE_1_0.0
   * @return true if this View has keyboard input focus
   * @pre The View has been initialized.
   * @pre The View should be on the stage before setting keyboard focus.
   * @note The View can be set to have the focus and still not receive all the key events if another View has over
   * ridden it. As the key input focus mechanism works like a stack, the top most View receives all the key events,
   * and passes on the unhandled events to the Views below in the stack. A View in the stack will regain key input
   * focus when there are no more Views above it in the focus stack. To query for the View which is on top of the
   * focus stack use Dali::Ui::KeyInputFocusManager::GetCurrentKeyboardFocusActor().
   */
  bool HasKeyInputFocus();

  /**
   * @brief Once an actor is Set to receive key input focus this function is called to stop it receiving key events.
   *
   * A check is performed to ensure it was previously set, if this check fails then nothing is done.
   * @SINCE_1_0.0
   * @pre The Actor has been initialized.
   */
  void ClearKeyInputFocus();

  // Gesture Detection

  /**
   * @brief Retrieves the pinch gesture detector of the View.
   *
   * @SINCE_1_0.0
   * @return The pinch gesture detector
   * @note Will return an empty handle if the View does not handle the gesture itself.
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief Retrieves the pan gesture detector of the View.
   *
   * @SINCE_1_0.0
   * @return The pan gesture detector
   * @note Will return an empty handle if the View does not handle the gesture itself.
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief Retrieves the tap gesture detector of the View.
   *
   * @SINCE_1_0.0
   * @return The tap gesture detector
   * @note Will return an empty handle if the View does not handle the gesture itself.
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief Retrieves the long press gesture detector of the View.
   *
   * @SINCE_1_0.0
   * @return The long press gesture detector
   * @note Will return an empty handle if the View does not handle the gesture itself.
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  /**
   * @brief Clears the background.
   * @SINCE_1_0.0
   */
  void ClearBackground();

  /**
   * @brief Sets RenderEffect to this View.
   *
   * @SINCE_2_3.25
   * @param[in] effect RenderEffect to add.
   *
   * @note Every effect inherits RenderEffect.
   */
  void SetRenderEffect(Ui::RenderEffect effect);

  /*
   * @brief Retrieves View's render effect
   * @SINCE_2_4.1
   * @return RenderEffect set on this View
   */
  Ui::RenderEffect GetRenderEffect() const;

  /**
   * @brief Clears RenderEffect of this View, if exists.
   * @SINCE_2_3.25
   */
  void ClearRenderEffect();

  // Renderers

  /**
   * @brief Helper method for rendering an image with custom shader.
   * @param[in] vertexSrc The custom vertex shader.
   * @param[in] fragmentSrc The custom fragment shader.
   * @return A newly created renderer.
   * @SINCE_2_6.28
   */
  static Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc);

  /**
   * @brief Helper method for rendering an image with custom shader.
   * @param[in] vertexSrc The custom vertex shader.
   * @param[in] fragmentSrc The custom fragment shader.
   * @param[in] hints Shader hints for optimization.
   * @param[in] shaderName Name of the shader for debugging.
   * @param[in] gridSize The number of grid sub-divisions required.
   * @return A newly created renderer.
   * @SINCE_2_6.28
   */
  static Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc,
                                       Dali::Shader::Hint::Value hints, const std::string& shaderName,
                                       Dali::Uint16Pair gridSize);

  /**
   * @brief Helper method for setting the first texture passed to a renderer.
   * @param[in] renderer The renderer using the texture.
   * @param[in] texture The texture to set.
   * @SINCE_2_6.28
   */
  static void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture);

  /**
   * @brief Helper method for setting the first texture passed to a renderer.
   * @param[in] renderer The renderer using the texture.
   * @param[in] frameBuffer A frame buffer color texture attached.
   * @SINCE_2_6.28
   */
  static void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer);

  // Resources

  /**
   * @brief Query if all resources required by a View are loaded and ready.
   *
   * Most resources are only loaded when the View is placed on stage.
   * @SINCE_1_2.60
   * @return true if the resources are loaded and ready, false otherwise
   */
  bool IsResourceReady() const;

  /**
   * @brief Get the loading state of the visual resource.
   *
   * @SINCE_1_3_5
   * @param[in] index The Property index of the visual
   * @return Return the loading status (PREPARING, READY and FAILED) of visual resource
   */
  Visual::ResourceStatus GetVisualResourceStatus(const Dali::Property::Index index);

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(View view, const KeyEvent& event);
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The View has been initialized.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted when the View gets Key Input Focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( View view );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The View has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @brief This signal is emitted when the View loses Key Input Focus.
   *
   * This could be due to it being gained by another View or Actor or just cleared from
   * this View as no longer required.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( View view );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The View has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusLostSignal();

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
   * @SINCE_1_2.60
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
   * @SINCE_2_4.33
   * @return The signal to connect to
   * @pre The View has been initialized.
   * @note This signal is emitted when the offscreen rendering task is completed.
   * @note This signal is only emitted when OffScreenRenderingType is set to RENDER_ONCE.
   */
  OffScreenRenderingFinishedSignalType& OffScreenRenderingFinishedSignal();

public: // Visual Management
  /**
   * @brief Register a visual by Property Index.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   *
   * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to
   * connect/disconnect registered visual to stage. Use below API with enabled set to false if derived class wishes to
   * View when visual is staged.
   * @note depth-index be used to Dali::Ui::DepthIndex::Ranges::AUTO_INDEX.
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual);

  /**
   * @brief Register a visual by Property Index with a depth index.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to
   * DepthIndex::Ranges::AUTO_INDEX, the actual depth-index of visual will be determind automatically (Use previous
   * visuals depth-index, or placed on top of all other visuals.) Otherwise, the visual's depth-index is set to clamped
   * value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
   *
   * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to
   * connect/disconnect registered visual to stage. Use below API with enabled set to false if derived class wishes to
   * View when visual is staged.
   *
   * @see Visual::Base::GetDepthIndex()
   * @see Visual::Base::SetDepthIndex()
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, int depthIndex);

  /**
   * @brief Register a visual by Property Index with the option of enabling/disabling it.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   * @param[in] enabled false if derived class wants to View when visual is set on stage.
   *
   * @note depth-index be used to Dali::Ui::DepthIndex::Ranges::AUTO_INDEX.
   *
   * @see EnableVisual()
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled);

  /**
   * @brief Register a visual by Property Index with a depth index with the option of enabling/disabling it.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   * @param[in] enabled false if derived class wants to View when visual is set on stage.
   * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to
   * DepthIndex::Ranges::AUTO_INDEX, the actual depth-index of visual will be determind automatically (Use previous
   * visuals depth-index, or placed on top of all other visuals.) Otherwise, the visual's depth-index is set to clamped
   * value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
   *
   * @see EnableVisual()
   * @see Visual::Base::GetDepthIndex()
   * @see Visual::Base::SetDepthIndex()
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @brief Erase the entry matching the given index from the list of registered visuals
   *
   * @param[in] index The Property index of the visual, used to reference visual
   */
  void UnregisterVisual(Dali::Property::Index index);

  /**
   * @brief Retrieve the visual associated with the given property index.
   *
   * @param[in] index The Property index of the visual.
   * @return The registered visual if exist, otherwise empty handle.
   * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference
   * count.
   */
  Ui::Visual::Base GetVisual(Dali::Property::Index index);

  /**
   * @brief Sets the given visual to be displayed or not when parent staged.
   *
   * @param[in] index The Property index of the visual
   * @param[in] enable flag to set enabled or disabled.
   */
  void EnableVisual(Dali::Property::Index index, bool enable);

  /**
   * @brief Queries if the given visual is to be displayed when parent staged.
   *
   * @param[in] index The Property index of the visual
   * @return bool whether visual is enabled or not
   */
  bool IsVisualEnabled(Dali::Property::Index index);

  /**
   * @brief Perform an action on a visual registered to this View.
   *
   * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
   *
   * @param[in] visualIndex The Property index of the visual.
   * @param[in] actionId The action to perform.  See Visual to find supported actions.
   * @param[in] attributes Optional attributes for the action.
   */
  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                const Dali::Property::Value& attributes);

  /**
   * @brief Perform an action on a visual registered to this View.
   *
   * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
   *
   * @param[in] visualIndex The Property index of the visual.
   * @param[in] actionId The action to perform.  See Visual to find supported actions.
   * @param[in] attributes Optional attributes for the action.
   */
  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                         const Dali::Any& attributes);

  /**
   * @brief Takes corner properties of a View and applies them to a registered visual, overriding the visual's own
   * corner properties.
   *
   * This function is essential for visuals like shadows and borderlines that need to match the View's corner radius.
   * It allows the visual to inherit the View's corner properties (e.g., CORNER_RADIUS, CORNER_RADIUS_POLICY)
   * through an optional constraint. This ensures visual consistency, especially when the View's corners are rounded.
   *
   * This API should be used for each registered visual (e.g., SHADOW, INNER_SHADOW, BORDERLINE) that needs to
   * reflect the View's corner properties. For instance, when adding a shadow visual, this function should be called
   * with the shadow visual and an appropriate corner constraint to ensure the shadow's corners align with the View's
   * corners.
   *
   * @note This function is provided for derived View classes that cannot access
   * View::Impl::EnableCornerPropertiesOverridden directly.
   * @note If @p enable is true, the @p cornerConstraint is applied to link the View's corner properties to the
   * visual's corner radius property. If @p enable is false, any previously applied constraint is removed, and the
   * visual will use its own corner properties.
   *
   * @param[in] visual The registered visual (e.g., shadow, borderline) that will receive the corner properties.
   * @param[in] enable True to apply the corner property override, false to disable it.
   * @param[in] cornerRadiusConstraint An optional Dali::Constraint that defines how the View's
   *                            corner properties (e.g., Ui::View::Property::CORNER_RADIUS)
   *                            are mapped to the visual's corner radius property.
   *                            If empty, a default equality constraint might be used if applicable.
   */
  void EnableCornerPropertiesOverridden(Visual::Base& visual, bool enable,
                                        Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

  /**
   * @brief Set input method context.
   *
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext(InputMethodContext& inputMethodContext);

  /**
   * @brief This signal is emitted when a visual has an event to notify.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( View view, Dali::Property::Index visualIndex, Dali::Property::Index signalId );
   * @endcode
   * @return The signal to connect to
   */
  VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Retrieve the property object associated with the given property index and the visual property key.
   *
   * @param[in] index The Property index of the visual.
   * @param[in] visualPropertyKey The key of the visual's property.
   * @return The Property object
   */
  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

public: // Accessibility
  /**
   * @brief The signal is emmited as a succession of "activate" signal send by accessibility client.
   * @return The signal to connect to
   */
  AccessibilityActivateSignalType& AccessibilityActivateSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was placed in TTS queue but other text with higher priority prevented it from being read.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal();

  /**
   * @brief
   *
   * @return The signal to connect to
   */
  AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal();

  /**
   * @brief
   *
   * @return The signal to connect to
   */
  AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was placed in TTS queue and reading was started but other text with higher priority cancelled it.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was fully read by TTS module.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal();

  /**
   * @brief The signal is emmited when accessibility client asks for object's name.
   *
   * Connected callback should assign name to Dali::String call parameter.
   * Accessibility name can be stored in two ways:
   *     this signal,
   *     ACCESSIBILITY_NAME property,
   * Priority is as above. If none is used, default implementation is provided.
   * @return [description]
   */
  AccessibilityGetNameSignalType& AccessibilityGetNameSignal();

  /**
   * @brief The signal is emmited when accessibility client asks for object's description.
   *
   * Connected callback should assign description to Dali::String call parameter.
   * Accessibility description can be stored in two ways:
   *     this signal,
   *     ACCESSIBILITY_DESCRIPTION property,
   * Priority is as above. If none is used, default implementation is provided.
   * @return signal handler
   */
  AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal();

  /**
   * @brief The signal is emitted when accessibility client call "DoGesture" method via IPC mechanism.
   *
   * This signal allows developers to serve incoming gesture in specific way.
   * @return signal handler
   */
  AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal();

  /**
   * @brief The signal is emitted when accessibility client call "DoAction" or "DoActionName" method via IPC mechanism.
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& AccessibilityActionSignal();

  /**
   * @brief The signal is emitted when accessibility highlight is added or removed from View.
   * @return The signal to connect to
   */
  AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal();

  /**
   * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
   *
   * @param destination Actor object
   * @param relation    enumerated value describing relation
   */
  void AppendAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation);

  /**
   * @brief The method allows removing relation
   *
   * @param destination Actor object
   * @param relation    enumerated value describing relation
   */
  void RemoveAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation);

  /**
   * @brief Returns a collection of Accessible objects related to current object and grouped by relation type.
   *
   * @return collection of relations
   *
   * @see Dali::Accessibility::Accessible::GetRelationSet()
   */
  std::vector<Accessibility::Relation> GetAccessibilityRelations();

  /**
   * @brief The method removes all previously appended relations
   */
  void ClearAccessibilityRelations();

  /**
   * @brief The method allows to add or modify value matched with given key.
   * Modification take place if key was previously set.
   *
   * @param key   Dali::String value
   * @param value Dali::String value
   */
  void AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value);

  /**
   * @brief The method erases key with its value from accessibility attributes
   * @param key Dali::String value
   */
  void RemoveAccessibilityAttribute(const Dali::String& key);

  /**
   * @brief The method clears accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief The method inserts reading information of an accessible object into attributes
   *
   * @param types Reading information types
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @brief The method returns reading information of an accessible object
   *
   * @return Reading information types
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType();

  /**
   * @brief The method erases highlight.
   *
   * @return bool value, false when it is not possible or something went wrong, at the other way true.
   */
  bool ClearAccessibilityHighlight();

  /**
   * @brief The method grabs highlight.
   *
   * @return bool value, false when it is not possible or something went wrong, at the other way true.
   */
  bool GrabAccessibilityHighlight();

  /**
   * @brief The metod presents bitset of View's states.
   *
   * @return Dali::Accessibility::States is vector of enumerated State.
   */
  Dali::Accessibility::States GetAccessibilityStates();

  /**
   * @brief The method force sending notifications about current states to accessibility clients
   *
   * @param states  mask with states expected to broadcast
   * @param recurse flag pointing if notifications of children's state would be sent
   */
  void NotifyAccessibilityStateChange(Dali::Accessibility::States states, bool recurse);

  /**
   * @brief The method to get the View's accessibility created or not.
   *
   * @return True if accessible were created. False otherwise.
   */
  bool IsAccessibleCreated();

  /**
   * @brief The method to set creatable of View's accessibility.
   * @note This method doesn't remove that already created accessible.
   *
   * @param enable True if we allow to create accessible. False otherwise.
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @brief The method to get creatable of View's accessibility.
   *
   * @return True if we allow to create accessible. False otherwise.
   */
  bool IsCreateAccessibleEnabled();

public: // Accessibility Methods
  /**
   * @brief The method to emit accessibility state-changed event to accessibility clients
   *
   * @param state  The accessibility state.
   * @param newValue new value to set.
   */
  void EmitAccessibilityStateChanged(Accessibility::State state, int newValue);

public: // Templates for Deriving Classes
  /**
   * @brief Template to allow deriving Views to DownCast handles to deriving handle classes.
   *
   * @tparam     T      The handle class
   * @tparam     I      The implementation class
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in] internal Pointer to the Internal::CustomActor
   */
  template<typename I>
  DALI_INTERNAL void VerifyCustomActorPointer(Dali::Internal::CustomActor* internal)
  {
    // Can have a NULL pointer so we only need to check if the internal implementation is our class
    // when there is a value.
    if(internal)
    {
      DALI_ASSERT_DEBUG(dynamic_cast<I*>(&CustomActor(internal).GetImplementation()));
    }
  }
};

} // namespace Ui

} // namespace Dali
