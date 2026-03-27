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
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <functional>
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/ui-state.h>
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
   * @brief Gets the measured size from the last Measure() pass.
   *
   * @return The measured size
   */
  MeasuredSize GetMeasuredSize() const;

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
   * @brief Gets the x scale factor applied to the view.
   *
   * @return The x scale factor
   */
  float GetScaleX() const;

  /**
   * @brief Sets the x scale factor applied to the view.
   *
   * @param[in] scaleX The X scale factor to set
   */
  View& SetScaleX(float scaleX);

  /**
   * @brief Gets the y scale factor applied to the view.
   *
   * @return The y scale factor
   */
  float GetScaleY() const;

  /**
   * @brief Sets the y scale factor applied to the view.
   *
   * @param[in] scaleY The Y scale factor to set
   */
  View& SetScaleY(float scaleY);

  /**
   * @brief Sets the y scale factor applied to the view.
   *
   * @param[in] scaleX The X scale factor to set
   * @param[in] scaleY The Y scale factor to set
   */
  View& SetScale(float scaleX, float scaleY);

  /**
   * @brief Checks if the view is visible.
   *
   * @return True if the view is visible, false otherwise
   */
  bool IsVisible() const;

  /**
   * @brief Sets the visibility of the view.
   *
   * @param[in] visibility True to make the view visible, false to hide it
   * @return Reference to this View for fluent chaining
   */
  View& SetVisibility(bool visibility);

  /**
   * @brief Gets the opacity of the view.
   *
   * @return The opacity value (0.0 to 1.0)
   */
  float GetOpacity() const;

  /**
   * @brief Sets the opacity of the view.
   *
   * @param[in] opacity The opacity value to set (0.0 to 1.0)
   * @return Reference to this View for fluent chaining
   */
  View& SetOpacity(float opacity);

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
   * @brief Sets the requested width.
   *
   * A positive value sets a fixed width.
   * WRAP_CONTENT (-1): sizes to content (default).
   * MATCH_PARENT (-2): fills the parent container.
   *
   * @param[in] width The requested width
   */
  View& SetRequestedWidth(float width);

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
  View& SetRequestedHeight(float height);

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

  /*
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
  View& SetEnabled(bool enabled);

  /**
   * @brief Returns true if this view and all its View ancestors are enabled.
   *
   * Unlike IsEnabled(), which only reflects the view's own state, this method
   * walks up the scene hierarchy and returns false if any ancestor View carries
   * UiState::DISABLED.
   *
   * @return True if neither the view nor any ancestor is disabled
   */
  bool IsEffectivelyEnabled() const;

  /**
   * @brief Returns true if this view or any of its View ancestors is focused.
   *
   * Unlike the Focused bit in GetState(), this method walks up the scene
   * hierarchy and returns true if any ancestor View carries UiState::FOCUSED.
   *
   * @return True if the view itself or at least one ancestor is focused
   */
  bool IsEffectivelyFocused() const;

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the interaction trait to this View and optionally configures it.
   *
   * A View can have at most one interaction trait for its lifetime; attaching interactive
   * succeeds only if no other interaction trait is set. If the View already has an
   * InteractiveTrait (e.g. from a previous AsInteractive call), the existing trait is
   * used and the configure callback is invoked with it.
   *
   * The callback is invoked in the caller's translation unit, so no std::function
   * crosses the library ABI boundary; this preserves ABI stability across toolchains.
   *
   * @param[in] configure Optional callback to configure the InteractiveTrait (e.g. connect signals).
   *                     Can be null or omitted to only attach the trait.
   * @return Reference to this View for fluent chaining
   */
  View& AsInteractive(std::function<void(InteractiveTrait&)> configure = nullptr)
  {
    InteractiveTrait trait = EnsureInteractiveTrait();
    if(configure && trait)
    {
      configure(trait);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the selectable trait to this View and optionally configures it.
   *
   * A View can have at most one selectable trait. If the View already has a
   * SelectableTrait (e.g. from a previous AsSelectable call), the existing trait is
   * used and the configure callback is invoked with it.
   *
   * @param[in] configure Optional callback to configure the SelectableTrait (e.g. connect signals).
   *                     Can be null or omitted to only attach the trait.
   * @return Reference to this View for fluent chaining
   */
  View& AsSelectable(std::function<void(SelectableTrait&)> configure = nullptr)
  {
    SelectableTrait trait = EnsureSelectableTrait();
    if(configure && trait)
    {
      configure(trait);
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

  // @CHAIN_MANUAL
  /**
   * @brief Adds a list of children to this View in a declarative way.
   * This method allows for a hierarchical UI tree construction by passing
   * a brace-enclosed initializer list of View objects.
   * @param[in] children The initializer list containing child View handles to be added.
   */
  View& Children(std::initializer_list<View> children)
  {
    for(const auto& child : children)
    {
      Add(child);
    }
    return *this;
  }

  // @CHAIN_END

  /**
   * @brief Inserts a child at the specified index.
   *
   * @param[in] index The index where to insert
   * @param[in] child The child to insert
   */
  void Insert(uint32_t index, View child);

  /**
   * @brief Removes all children from this View.
   */
  void RemoveAllChildren();

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

public: // State API (non-chaining)
  using StateChangedSignalType = Signal<void(View, const StateEvent&)>;

  /**
   * @brief Gets the current state of this View.
   *
   * @return The current UiState (may contain multiple combined states)
   */
  const UiState& GetState() const;

  /**
   * @brief Returns the state changed signal.
   *
   * Emitted whenever the view's UiState changes.
   *
   * @code
   * view.StateChangedSignal().Connect(tracker, [](View v, const StateEvent& e) {
   *   if(e.Added(UiState::FOCUSED)) { ... }
   * });
   * @endcode
   *
   * @return The StateChangedSignal
   */
  StateChangedSignalType& StateChangedSignal();

public: // Trait accessors (non-chaining)
  /**
   * @brief Ensures this View has an interaction trait and returns it.
   *
   * If no interaction trait is set, an InteractiveTrait is attached and returned.
   * If an InteractiveTrait is already attached, it is returned. If a different
   * interaction trait is set, an assertion may fire and an empty handle is returned.
   *
   * @return InteractiveTrait handle, or an uninitialized handle on error
   */
  InteractiveTrait EnsureInteractiveTrait();

  /**
   * @brief Returns whether this View has an interaction trait attached.
   *
   * @return True if this View has an InteractiveTrait
   */
  bool IsInteractive() const;

  /**
   * @brief Ensures this View has a selectable trait and returns it.
   *
   * If no selectable trait is set, a SelectableTrait is attached and returned.
   * If a SelectableTrait is already attached, it is returned. If a different
   * selectable trait type is set, an assertion may fire and an empty handle is returned.
   *
   * @return SelectableTrait handle, or an uninitialized handle on error
   */
  SelectableTrait EnsureSelectableTrait();

  /**
   * @brief Returns whether this View has a selectable trait attached.
   *
   * @return True if this View has a SelectableTrait
   */
  bool IsSelectable() const;

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

  // Background

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

  // Resources

  /**
   * @brief Query if all resources required by a View are loaded and ready.
   *
   * Most resources are only loaded when the View is placed on stage.
   * @SINCE_1_2.60
   * @return true if the resources are loaded and ready, false otherwise
   */
  bool IsResourceReady() const;

  // Signals

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
