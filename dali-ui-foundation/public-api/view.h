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
#include <functional>
#include <dali-ui-foundation/public-api/controls/control.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/clickable-trait.h>

namespace Dali
{

namespace UI
{

// Forward declarations
namespace Integration
{
class ViewImpl;
}

#include "view.autogen.h"
/**
 * @brief View is a base UI component class that extends Control.
 *
 * View provides basic UI functionality and can be used as a foundation
 * for creating custom UI components. It inherits all the capabilities
 * of Control including styling, gesture detection, and keyboard navigation.
 *
 * View implements the layout system with Measure/Arrange passes.
 */
class DALI_UI_API View : public UI::Control
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
    if (configure && trait)
    {
      configure(trait);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the clickable trait and connects a member function to the Clicked signal.
   *
   * Equivalent to AsClickable() then GetClickedSignal().Connect(obj, func).
   * @param[in] obj Object that implements ConnectionTrackerInterface (e.g. ConnectionTracker subclass); used for
   * automatic disconnection
   * @param[in] func Member function with signature bool (View, const InputEvent&)
   * @return Reference to this View for fluent chaining
   */
  template <class X>
  View& AsClickable(X* obj, bool (X::*func)(View, const InputEvent&))
  {
    ClickableTrait trait = GetOrAttachClickableTrait();
    if (trait && obj && func)
    {
      trait.GetClickedSignal().Connect(obj, func);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Attaches the clickable trait and connects a callable to the Clicked signal.
   *
   * Equivalent to AsClickable() then GetClickedSignal().Connect(connectionTracker, func).
   * @param[in] connectionTracker Used for automatic disconnection when the tracker is destroyed
   * @param[in] func Callable with signature bool (View, const InputEvent&) (e.g. lambda)
   * @return Reference to this View for fluent chaining
   */
  template <typename F>
  View& AsClickable(Dali::ConnectionTrackerInterface* connectionTracker, F&& func)
  {
    ClickableTrait trait = GetOrAttachClickableTrait();
    if (trait && connectionTracker)
    {
      trait.GetClickedSignal().Connect(connectionTracker, std::forward<F>(func));
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
   * @brief Executesa custom action on this View instance.
   * Use this method to perform additional initialization or logic on a View
   * without breaking the declarative method chaining.
   * @param[in] action A function or lambda to be executed with this instance.
   */
  View& With(std::function<void(View&)> action)
  {
    if (action)
    {
      action(*this);
    }
    return *this;
  }

  // @CHAIN_MANUAL
  /**
   * @brief Sets the background color.
   *
   * @param[in] color The required background color value
   */
  View& BackgroundColor(const Vector4& color)
  {
    SetBackgroundColor(color);
    return *this;
  }

  // @CHAIN_END

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
   * the View was made clickable by other means (e.g. a control that attaches the trait).
   *
   * @return ClickableTrait handle if this View has a clickable trait; otherwise an uninitialized handle
   */
  ClickableTrait GetClickableTrait() const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  explicit DALI_UI_API View(Integration::ViewImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API View(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace UI

} // namespace Dali
