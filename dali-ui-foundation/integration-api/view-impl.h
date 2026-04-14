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
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/object/type-info.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-trait-interface.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-state.h>
#include <dali-ui-foundation/public-api/view-focus-enums.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
class Layout;
class LayoutManager; // Used by LayoutImpl
class UiColor;
class ViewAccessible;

namespace Internal
{
class LayoutCallbacksImpl;
class ViewDataImpl;
} //namespace Internal

namespace Integration
{

class ViewImpl;
using ViewImplPtr = IntrusivePtr<ViewImpl>;

/**
 * @brief This is the internal implementation class for View.
 *
 * ViewImpl extends CustomActorImpl to provide basic UI functionality.
 * It implements the p-impl pattern and provides the actual implementation
 * for the View public API.
 *
 * View can optionally have a LayoutManager to manage child layout.
 * When a LayoutManager is set, the View can manage children and delegate
 * layout calculations to the LayoutManager.
 *
 * @see Dali::Ui::View
 */
class DALI_UI_API ViewImpl : public CustomActorImpl, public ConnectionTrackerInterface
{
public:
  class Extension; ///< Forward declare future extension interface

  // Creation & Destruction

  /**
   * @brief Child data structure for layout calculations.
   *
   * Stores information about each child including the View handle,
   * measured size, and arranged bounds.
   */
  struct ChildData
  {
    Ui::View     view;           ///< Handle to the child view
    MeasuredSize measuredSize;   ///< Size from Measure pass
    LayoutRect   arrangedBounds; ///< Bounds from Arrange pass
  };

  /**
   * @brief Container type for children.
   */
  using ChildContainer = std::vector<ChildData>;

  /**
   * @brief Creates a new View.
   */
  static ViewImplPtr New();

protected:
  /**
   * @brief Destructor.
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ViewImpl();

  // Construction

  /**
   * @brief View constructor.
   */
  ViewImpl();

public: // From Ui::Internal::View
  /**
   * @copydoc Ui::Internal::View::OnInitialize
   */
  virtual void OnInitialize();

  /**
   * @copydoc Ui::Internal::View::OnSceneConnection
   * When this View is a layout root and is connected to a window, registers
   * with LayoutController so that measure/arrange runs even if invalidation
   * occurred before the view was added to the window.
   */
  void OnSceneConnection(int depth) override;

  /**
   * @brief Override to separate dali-ui layout from DALi size negotiation.
   * When this View has a LayoutManager, size/position are driven by dali-ui
   * LayoutController; we no-op. Otherwise delegate to View.
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Ui::Internal::View::OnFocusGained
   */
  virtual void OnFocusGained();

  /**
   * @copydoc Ui::Internal::View::OnFocusLost
   */
  virtual void OnFocusLost();

  /**
   * @copydoc Ui::Internal::View::OnKeyEvent
   */
  virtual bool OnKeyEvent(const Dali::KeyEvent& event);

public: // API (size, position, parent origin, pivot)
  /**
   * @copydoc Dali::Ui::View::GetScaleX
   */
  float GetScaleX() const;

  /**
   * @copydoc Dali::Ui::View::SetScaleX
   */
  void SetScaleX(float scaleX);

  /**
   * @copydoc Dali::Ui::View::GetScaleY
   */
  float GetScaleY() const;

  /**
   * @copydoc Dali::Ui::View::SetScaleY
   */
  void SetScaleY(float scaleY);

  /**
   * @copydoc Dali::Ui::View::IsVisible
   */
  bool IsVisible() const;

  /**
   * @copydoc Dali::Ui::View::SetVisibility
   */
  void SetVisibility(bool visibility);

  /**
   * @copydoc Dali::Ui::View::GetOpacity
   */
  float GetOpacity() const;

  /**
   * @copydoc Dali::Ui::View::SetOpacity
   */
  void SetOpacity(float opacity);

  /**
   * @copydoc Dali::Ui::View::GetSize
   */
  MeasuredSize GetSize() const;

  /**
   * @copydoc Dali::Ui::View::GetPositionX
   */
  float GetPositionX() const;

  /**
   * @copydoc Dali::Ui::View::SetPositionX
   */
  void SetPositionX(float x);

  /**
   * @copydoc Dali::Ui::View::GetPositionY
   */
  float GetPositionY() const;

  /**
   * @copydoc Dali::Ui::View::SetPositionY
   */
  void SetPositionY(float y);

  /**
   * @copydoc Dali::Ui::View::GetParentOrigin
   */
  Vector3 GetParentOrigin() const;

  /**
   * @copydoc Dali::Ui::View::SetParentOrigin
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @copydoc Dali::Ui::View::GetPivot
   */
  Vector3 GetPivot() const;

  /**
   * @copydoc Dali::Ui::View::SetPivot
   */
  void SetPivot(const Vector3& point);

  /**
   * @copydoc Dali::Ui::View::IsFocusable
   */
  bool IsFocusable() const;

  /**
   * @copydoc Dali::Ui::View::SetFocusable
   */
  void SetFocusable(bool focusable);

  /**
   * @copydoc Dali::Ui::View::IsTouchFocusable
   */
  bool IsTouchFocusable() const;

  /**
   * @copydoc Dali::Ui::View::SetTouchFocusable
   */
  void SetTouchFocusable(bool touchFocusable);

public: // State API
  using StateChangedSignalType = Signal<void(Ui::View, const StateEvent&)>;

  /**
   * @brief Gets the current state of this View.
   *
   * @return The current UiState
   */
  const UiState& GetState() const;

  /**
   * @brief Checks if the view is enabled.
   *
   * Enabled is the default state. A disabled view does not receive user interaction.
   *
   * @return True if enabled
   */
  bool IsEnabled() const;

  /**
   * @brief Sets the enabled state of the view.
   *
   * When disabled, the Disabled state is added and the underlying
   * USER_INTERACTION_ENABLED property is set to false.
   *
   * @param[in] enabled True to enable, false to disable
   */
  void SetEnabled(bool enabled);

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
   * Unlike IsFocused() (which only reflects the view's own state), this method
   * walks up the scene hierarchy and returns true if any ancestor View carries
   * UiState::FOCUSED.
   *
   * @return True if the view itself or at least one ancestor is focused
   */
  bool IsEffectivelyFocused() const;

  /**
   * @brief Returns the state changed signal.
   *
   * Emitted whenever the view's UiState changes. The signal passes
   * the previous state and the new (current) state.
   *
   * @return The StateChangedSignal
   */
  StateChangedSignalType& StateChangedSignal();

  /**
   * @brief Registers a named state-change handler using a member function.
   *
   * The handler is identified by @a id. If a handler with the same id already
   * exists, it is replaced. The handler is automatically removed when @a obj
   * is destroyed.
   *
   * @param[in] id   Unique identifier for this handler
   * @param[in] obj  Object whose member function will be called (must implement ConnectionTrackerInterface)
   * @param[in] func Member function with signature void(View, const StateEvent&)
   */
  template<class X>
  void WhenStateChanged(const Dali::String& id, X* obj, void (X::*func)(Ui::View, const StateEvent&))
  {
    if(obj && func)
    {
      SetNamedStateHandler(id, obj, MakeCallback(obj, func));
    }
  }

  /**
   * @brief Registers a named state-change handler using a callable (e.g. lambda).
   *
   * The handler is identified by @a id. If a handler with the same id already
   * exists, it is replaced. The handler is automatically removed when @a tracker
   * is destroyed.
   *
   * @code
   * WhenStateChanged("default_bg", this, [](View v, const StateEvent& e) {
   *   if(e.Changed(UiState::FOCUSED)) { ... }
   * });
   * @endcode
   *
   * @param[in] id      Unique identifier for this handler
   * @param[in] tracker ConnectionTrackerInterface for automatic lifetime management
   * @param[in] func    Callable with signature void(View, const StateEvent&)
   */
  template<typename F>
  void WhenStateChanged(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, F&& func)
  {
    if(tracker)
    {
      SetNamedStateHandler(id, tracker, new CallbackFunctor2<std::decay_t<F>, Ui::View, const StateEvent&>(std::forward<F>(func)));
    }
  }

  /**
   * @brief Removes a named state-change handler.
   *
   * @param[in] id The handler identifier to remove
   * @return True if a handler was found and removed
   */
  bool UnsetStateHandler(const Dali::String& id);

  /**
   * @brief Removes a named state-change handler only if it is not currently being processed.
   *
   * Safe to call from within the handler itself: if the call originates from the handler
   * identified by @a id, removal is deferred until after that handler returns.
   *
   * @param[in] id The handler identifier to remove
   * @return True if removed, false if currently processing or not found
   */
  bool UnsetStateHandlerWhenNotProcessing(const Dali::String& id);

  /**
   * @brief Updates a state bit in the view's UiState and emits StateChangedSignal.
   *
   * This is a low-level state-bit updater. It does **not** trigger the
   * higher-level logic associated with the state. For example, calling
   * @c SetViewState(DISABLED, true) only sets the DISABLED bit — it does
   * not clear focus or disable touch processing.
   * Use the corresponding public API (e.g. @c SetEnabled()) to perform
   * the full state transition including all side effects.
   *
   * @param[in] state The state to set or clear
   * @param[in] on    True to add the state, false to remove it
   * @param[in] cause Input event that triggered the change; leave default if programmatic
   */
  void SetViewState(UiState state, bool on, InputEvent cause = InputEvent::None());

  /**
   * @brief Called when the view's focus state changes.
   *
   * The caller (e.g. focus manager) is responsible for invoking this method.
   * Updates the Focused state and forwards to the interaction trait if present.
   *
   * @param[in] focused True if the view gained focus
   * @param[in] cause   The input event that triggered the focus change (empty if system-triggered)
   */
  void OnFocusChanged(bool focused, InputEvent cause = InputEvent::None());

  /**
   * @brief Called when the view's enabled state changes externally.
   *
   * The caller is responsible for invoking this method when
   * USER_INTERACTION_ENABLED property changes.
   * Updates the Disabled state accordingly.
   *
   * @param[in] enabled True if the view became enabled
   */
  void OnEnableChanged(bool enabled);

  /**
   * @copydoc Dali::Ui::View::GetBackgroundColor()
   */
  UiColor GetBackgroundColor();

  /**
   * @copydoc Dali::Ui::View::SetBackgroundColor(const UiColor&)
   */
  void SetBackgroundColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::View::GetCornerRadius
   */
  Vector4 GetCornerRadius() const;

  /**
   * @copydoc Dali::Ui::View::SetCornerRadius(const Vector4&)
   */
  void SetCornerRadius(const Vector4& radius);

  /**
   * @copydoc Dali::Ui::View::GetCornerRadiusPolicy
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @copydoc Dali::Ui::View::SetCornerRadiusPolicy
   */
  void SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @copydoc Dali::Ui::View::GetCornerSquareness
   */
  Vector4 GetCornerSquareness() const;

  /**
   * @copydoc Dali::Ui::View::SetCornerSquareness(const Vector4&)
   */
  void SetCornerSquareness(const Vector4& squareness);

  /**
   * @copydoc Dali::Ui::View::GetBorderlineWidth
   */
  float GetBorderlineWidth() const;

  /**
   * @copydoc Dali::Ui::View::SetBorderlineWidth
   */
  void SetBorderlineWidth(float width);

  /**
   * @copydoc Dali::Ui::View::GetBorderlineColor
   */
  UiColor GetBorderlineColor();

  /**
   * @copydoc Dali::Ui::View::SetBorderlineColor
   */
  void SetBorderlineColor(const UiColor& color);

  /**
   * @copydoc Dali::Ui::View::GetBorderlineOffset
   */
  float GetBorderlineOffset() const;

  /**
   * @copydoc Dali::Ui::View::SetBorderlineOffset
   */
  void SetBorderlineOffset(float offset);

  /**
   * @copydoc Dali::Ui::View::GetName
   */
  Dali::String GetName() const;

  /**
   * @copydoc Dali::Ui::View::SetName
   */
  void SetName(const Dali::String& name);

  /**
   * @brief Sets a trait to this View.
   *
   * The trait will share the lifecycle with this View.
   *
   * For traits identified by a user-defined @p TraitId, calling this method with the
   * same id will replace the existing trait after calling OnDetached() on the old one.
   *
   * For the reserved id @c ReservedTraitId::INTERACTION_TRAIT:
   * - The trait must implement @c InteractiveTraitInterface.
   * - It can be set only once for the lifetime of the View; attempting to replace or
   *   remove it is considered a programming error and will trigger an assertion.
   *
   * @throws DaliException If the trait already has an owner
   * @throws DaliException If the id is already registered with other trait
   *
   * @note **Strong Reference**
   * The view will hold the strong reference to the trait after attached.
   *
   * @param[in] id The unique key to identify the trait
   * @param[in] trait The trait object to attach
   */
  void SetTrait(TraitId id, Trait& trait);

  /**
   * @brief Gets a trait from this View.
   * @param[in] id The unique key to identify the trait
   * @return The trait handle
   */
  Trait GetTrait(TraitId id) const;

  /**
   * @brief Removes a trait from this View.
   *
   * For traits identified by a user-defined @p TraitId, this detaches the trait,
   * calls its OnDetached(), and returns true on success.
   *
   * For the reserved id @c ReservedTraitId::INTERACTION_TRAIT, removal is not allowed; an assertion
   * will be triggered and the method will always return false.
   *
   * @param[in] id The unique key to identify the trait
   * @return True if succeeded, false otherwise
   */
  bool RemoveTrait(TraitId id);

  /**
   * @copydoc Ui::View::EnsureInteractiveTrait
   */
  Ui::InteractiveTrait EnsureInteractiveTrait();

  /**
   * @copydoc Ui::View::IsInteractive
   */
  bool IsInteractive() const;

  /**
   * @copydoc Ui::View::EnsureSelectableTrait
   */
  Ui::SelectableTrait EnsureSelectableTrait();

  /**
   * @copydoc Ui::View::IsSelectable
   */
  bool IsSelectable() const;

  /**
   * @copydoc Ui::View::SetInteractionEffect
   */
  void SetInteractionEffect(Trait effect);

public: // Measure / Arrange API
  /**
   * @brief Measures the view with the given constraints.
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @brief Arranges the view within the given bounds.
   */
  MeasuredSize Arrange(const LayoutRect& bounds);

  /**
   * @brief Invalidates the measure of this view and propagates up.
   */
  void InvalidateMeasure();

  /**
   * @brief Invalidates the arrange of this view.
   */
  void InvalidateArrange();

  /**
   * @brief Gets the measured size from the last Measure() pass.
   */
  MeasuredSize GetMeasuredSize() const;

  /**
   * @brief Checks if the measure is valid.
   */
  bool IsMeasureValid() const;

  /**
   * @brief Checks if the arrange is valid.
   */
  bool IsArrangeValid() const;

protected: // Virtual methods for derived classes (Template Method pattern)
  /**
   * @brief Called during measure pass.
   */
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);

  /**
   * @brief Called during arrange pass.
   */
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

public: // Requested size API
  void  SetRequestedWidth(float width);
  float GetRequestedWidth() const;
  void  SetRequestedHeight(float height);
  float GetRequestedHeight() const;
  void  SetMinimumWidth(float width);
  float GetMinimumWidth() const;
  void  SetMinimumHeight(float height);
  float GetMinimumHeight() const;
  void  SetMaximumWidth(float width);
  float GetMaximumWidth() const;
  void  SetMaximumHeight(float height);
  float GetMaximumHeight() const;

public: // Layout Params API
  /**
   * @copydoc Dali::Ui::View::SetLayoutParams
   */
  void SetLayoutParams(Ui::LayoutParams params);

  /**
   * @brief Retrieves a layout params trait by LayoutParamsType.
   */
  BaseHandle GetLayoutParamsTrait(LayoutParamsType type) const;

public: // Layout Properties API
  void    SetMargin(const Extents& margin);
  Extents GetMargin() const;
  void    SetPadding(const Extents& padding);
  Extents GetPadding() const;

  /**
   * @brief Sets the layout mode of this View.
   *
   * @see Ui::LayoutMode
   */
  void SetLayoutMode(Ui::LayoutMode mode);

  /**
   * @brief Gets the layout mode of this View.
   */
  Ui::LayoutMode GetLayoutMode() const;

  /**
   * @brief Convenience: returns true if LayoutMode is Standalone.
   *
   * Standalone children are excluded from the parent layout's accumulation,
   * spacing and index calculations. The parent still calls Measure() on them
   * (so size resolution works normally) but during Arrange the parent passes
   * bounds composed of the child's PositionX/Y and measuredSize.
   */
  bool IsLayoutModeStandalone() const;

public: // Parent Layout API
  Ui::Layout   GetParentLayout() const;
  Ui::View     GetParentView() const;
  virtual bool IsLayout() const;

public: // Layout Callback API
  Internal::LayoutCallbacksImpl* GetLayoutCallbacks() const;
  Internal::LayoutCallbacksImpl* EnsureLayoutCallbacks();

public: // Child Management API
  /**
   * @brief Inserts a child at the specified index.
   *
   * Use Actor::Add() to append a child at the end.
   * This method inserts at a specific position in the children list.
   *
   * @param[in] index The index where to insert
   * @param[in] child The child to insert
   */
  void Insert(uint32_t index, Ui::View child);

  /**
   * @brief Removes all children from this view.
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
  Ui::View GetChildAt(uint32_t index) const;

  /**
   * @brief Returns the index of the given child view, or -1 if not found.
   *
   * @param[in] view The child view to find
   * @return Index of the view, or -1 if not a child
   */
  int32_t IndexOfChild(Ui::View view) const;

  /**
   * @brief Adds a list of children (method chaining).
   *
   * @param[in] children The initializer list of View handles to add
   * @return Reference to this for method chaining
   */
  ViewImpl& Contents(std::initializer_list<Ui::View> children);

  /**
   * @brief Gets the children container for layout manager access.
   *
   * @return Reference to the children container
   */
  ChildContainer& GetChildren();

  /**
   * @brief Gets the children container (const version).
   *
   * @return Const reference to the children container
   */
  const ChildContainer& GetChildren() const;

protected:
  /**
   * @brief Applies min/max constraints to the size.
   */
  MeasuredSize ApplyConstraints(const MeasuredSize& size) const;

  /**
   * @brief Registers this layout with the LayoutController for processing.
   *
   * Called when this view is a Layout Root (top of layout hierarchy).
   */
  void RegisterWithLayoutController();

private:
  /**
   * @brief Measures standalone children that may not have been measured by
   * OnMeasure (e.g. leaf views like Label). The measure cache prevents
   * redundant work when OnMeasure already measured them.
   */
  void MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight);

  /**
   * @brief Arranges standalone children that may not have been arranged by
   * OnArrange (e.g. leaf views like Label). Uses ArrangeStandaloneChild
   * helper; the arrange-valid flag prevents redundant work.
   */
  void ArrangeStandaloneChildren(const LayoutRect& bounds);

  // Not copyable or movable
  ViewImpl(const ViewImpl&)            = delete;
  ViewImpl(ViewImpl&&)                 = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&)      = delete;

  void SetBackgroundColorInternal(const Vector4& color);
  void SetBorderlineColorInternal(const Vector4& color);
  void OnChildOrderChanged(Actor orderChangedChild);

  /**
   * @brief Internal helper to register a named state handler via StateHandlerTrait.
   *
   * Lazily creates the StateHandlerTrait on first call.
   */
  void SetNamedStateHandler(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);

private:
  std::vector<std::pair<TraitId, Trait>> mTraits;
  InteractiveTraitInterface*             mInteractiveTrait;

  // State
  UiState                mState;
  StateChangedSignalType mStateChangedSignal;

  // Requested size (WRAP_CONTENT = -1.0f, MATCH_PARENT = -2.0f)
  float mRequestedWidth;
  float mRequestedHeight;

  // Requested position (used when parent is not a layout)
  float mRequestedPositionX;
  float mRequestedPositionY;
  float mMinimumWidth;
  float mMinimumHeight;
  float mMaximumWidth;
  float mMaximumHeight;

  // Layout Properties Data
  Extents        mMargin;
  Extents        mPadding;
  Ui::LayoutMode mLayoutMode{Ui::LayoutMode::DEFAULT};

  // Measure/Arrange State (cache-based)
  // mLastMeasuredConstraint.width < 0 means no valid measure cache
  MeasuredSize mMeasuredSize;
  MeasuredSize mLastMeasuredConstraint;
  LayoutRect   mArrangedBounds;
  bool         mArrangeValid;

  // Children (synchronized with Actor hierarchy via OnChildAdd/OnChildRemove)
  ChildContainer mChildren;
  bool           mUpdatingChildren{false}; ///< Guard to prevent redundant mChildren updates in OnChildAdd/OnChildRemove

  // From control-impl.h

public:
  /**
   * @brief Sets the background with a property map.
   *
   * @param[in] map The background property map
   */
  void SetBackground(const Property::Map& map);

  /**
   * @copydoc Dali::Ui::View::ClearBackground
   */
  void ClearBackground();

  /**
   * @copydoc Dali::Ui::View::SetRenderEffect
   */
  void SetRenderEffect(Ui::RenderEffect effect);

  /**
   * @copydoc Dali::Ui::View::GetRenderEffect
   */
  RenderEffect GetRenderEffect() const;

  /**
   * @copydoc Dali::Ui::View::ClearRenderEffect
   */
  void ClearRenderEffect();

  /**
   * @brief Called when resources of view are ready. this api does not request relayout.
   */
  void SetResourceReady();

  /**
   * @brief Retrieves the internal data implementation of the view.
   *
   * @return Reference to the internal data implementation
   */
  Internal::ViewDataImpl& GetViewDataImpl() const;

  /**
   * @brief Retrieves SourceActor of the OffScreenRenderable.
   *
   * @return SourceActor of the OffScreenRenderable.
   */
  virtual Dali::Actor GetOffScreenRenderableSourceActor();

  /**
   * @brief Retrieves whether the OffScreen RenderTasks is exclusive or not.
   * The SourceActor of an OffScreen RenderTask can also become the SourceActor of another Actor's OffScreen RenderTask.
   * To draw the SourceActor multitimes, the exclusive information is required.
   *
   * @return True if the RenderTask is exclusive.
   */
  virtual bool IsOffScreenRenderTaskExclusive();

  // Accessibility

  /**
   * @brief Gets the Accessible object that represents this view.
   *
   * This method calls CreateAccessibleObject() if CreateAccessible is true.
   *
   * @return The Accessible object
   *
   * @see CreateAccessibleObject()
   */
  std::shared_ptr<Ui::ViewAccessible> GetAccessibleObject();

  // Gesture Detection

  /**
   * @brief Allows deriving classes to enable any of the gesture detectors that are available.
   *
   * Gesture detection can be enabled one at a time or in bitwise format as shown:
   * @code
   * EnableGestureDetection(GestureType::Value(GestureType::PINCH | GestureType::TAP | GestureType::PAN));
   * @endcode
   * @param[in] type The gesture type(s) to enable
   */
  void EnableGestureDetection(GestureType::Value type);

  /**
   * @brief Allows deriving classes to disable any of the gesture detectors.
   *
   * Like EnableGestureDetection, this can also be called using bitwise or.
   * @param[in] type The gesture type(s) to disable
   * @see EnableGetureDetection
   */
  void DisableGestureDetection(GestureType::Value type);

  /**
   * @brief If deriving classes wish to fine tune pinch gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pinch gesture detector
   * @pre Pinch detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune pan gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The pan gesture detector
   * @pre Pan detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune tap gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The tap gesture detector
   * @pre Tap detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief If deriving classes wish to fine tune long press gesture
   * detection, then they can access the gesture detector through this
   * API and modify the detection.
   *
   * @return The long press gesture detector
   * @pre Long press detection should have been enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Keyboard Navigation

  /**
   * @brief Sets whether this view supports two dimensional
   * keyboard navigation (i.e. whether it knows how to handle the
   * keyboard focus movement between its child actors).
   *
   * The view doesn't support it by default.
   * @param[in] isSupported Whether this view supports two dimensional keyboard navigation
   */
  void SetKeyboardNavigationSupport(bool isSupported);

  /**
   * @brief Gets whether this view supports two dimensional keyboard navigation.
   *
   * @return true if this view supports two dimensional keyboard navigation
   */
  bool IsKeyboardNavigationSupported();

  // Key Input

  /**
   * @copydoc Ui::View::SetKeyInputFocus()
   */
  void SetKeyInputFocus();

  /**
   * @copydoc Ui::View::HasKeyInputFocus()
   */
  bool HasKeyInputFocus();

  /**
   * @copydoc Ui::View::ClearKeyInputFocus()
   */
  void ClearKeyInputFocus();

  // Keyboard Focus

  /**
   * @brief Sets whether this view is a focus group for keyboard navigation.
   *
   * (i.e. the scope of keyboard focus movement
   * can be limited to its child actors). The view is not a focus group by default.
   * @param[in] isFocusGroup Whether this view is set as a focus group for keyboard navigation
   */
  void SetAsKeyboardFocusGroup(bool isFocusGroup);

  /**
   * @brief Gets whether this view is a focus group for keyboard navigation.
   *
   * @return true if this view is set as a focus group for keyboard navigation
   */
  bool IsKeyboardFocusGroup();

  /**
   * @brief Get texture output of offscreen rendering.
   * @note Valid only if call this API inside of OffScreenRenderingFinishedSignal()
   *       signal, and OffScreenRenderingType::RENDER_ONCE
   */
  Dali::Texture GetOffScreenRenderingOutput() const;

  /// @cond internal
  /**
   * @brief Called by the FocusManager.
   */
  DALI_INTERNAL void KeyboardEnter();
  /// @endcond

  // Signals

  /**
   * @copydoc Dali::Ui::View::KeyEventSignal()
   */
  Ui::View::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Dali::Ui::View::FocusChangedSignal()
   */
  Ui::View::FocusChangedSignalType& FocusChangedSignal();

  /// @cond internal
  /**
   * @brief Called by the KeyInputFocusManager to emit key event signals.
   *
   * @param[in] event The key event
   * @return True if the event was consumed
   */
  DALI_INTERNAL bool EmitKeyEventSignal(const KeyEvent& event);
  /// @endcond

protected: // For derived classes to call
  /**
   * @brief Registers a color binding (if not yet registered) and updates the tracked color,
   *        or clears the binding if @a color has no color ID.
   *
   * @tparam T        Type of the instance (ViewImpl or a derived class)
   * @param[in] bindingId  Caller-defined identifier for this binding (e.g. "BackgroundColor")
   * @param[in] color      The UiColor to apply
   * @param[in] inst       The object whose @a setter will be used as the theme-change callback
   * @param[in] setter     Member function called both immediately and on theme change
   */
  template<typename T>
  void SetColorBinding(StringView bindingId, const UiColor& color, T* inst, void (T::*setter)(const Vector4&))
  {
    auto manager = UiColorManager::Get();
    if(color.HasColorId())
    {
      if(!manager.HasBinding(Self(), bindingId))
      {
        manager.RegisterBinding(Self(), bindingId, ColorCallback::New(inst, setter));
      }
      manager.SetBindingColor(Self(), bindingId, color);
    }
    else
    {
      manager.ClearBinding(Self(), bindingId);
    }
    (inst->*setter)(color.Resolve());
  }

  /**
   * @brief Emits FocusChanged signal.
   *
   * Should be called last by the view after it acts on the Input Focus change.
   *
   * @param[in] focusGained True if gained, False if lost
   */
  void EmitFocusChangedSignal(bool focusGained);

protected: // From CustomActorImpl
  /**
   * @copydoc CustomActorImpl::OnSceneDisconnection()
   * @note If overridden, then an up-call to ViewImpl::OnSceneDisconnection MUST be made at the end.
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc CustomActorImpl::OnChildAdd()
   * @note If overridden, then an up-call to ViewImpl::OnChildAdd MUST be made at the end.
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnChildRemove()
   * @note If overridden, then an up-call to ViewImpl::OnChildRemove MUST be made at the end.
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc CustomActorImpl::OnPropertySet()
   * @note If overridden, then an up-call to ViewImpl::OnPropertySet MUST be made at the end.
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc CustomActorImpl::OnSizeSet()
   * @note If overridden, then an up-call to ViewImpl::OnSizeSet MUST be made at the end.
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnSizeAnimation()
   * @note If overridden, then an up-call to ViewImpl::OnSizeAnimation MUST be made at the end.
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc CustomActorImpl::OnAnimateAnimatableProperty()
   * @note If overridden, then an up-call to ViewImpl::OnAnimateAnimatableProperty MUST be made at the end.
   */
  void OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Dali::Animation::State state) override;

  /**
   * @copydoc CustomActorImpl::OnConstraintAnimatableProperty()
   * @note If overridden, then an up-call to ViewImpl::OnConstraintAnimatableProperty MUST be made at the end.
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied) override;

  /**
   * @copydoc CustomActorImpl::GetOffScreenRenderTasks()
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc CustomActorImpl::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc CustomActorImpl::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc CustomActorImpl::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc CustomActorImpl::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc CustomActorImpl::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc CustomActorImpl::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

public:
  void RelayoutRequestToView();

public: // Helpers for deriving classes
  /**
   * @brief Flags for the constructor.
   */
  enum ViewBehaviour
  {
    VIEW_BEHAVIOUR_DEFAULT = 0, ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                ///< but doesn't receive event callbacks.
    NOT_IN_USE_1 = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 0),
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT =
      1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 1), ///< True if needs to support keyboard navigation
    DISABLE_STYLE_CHANGE_SIGNALS = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT +
                                         2), ///< True if view should not monitor style change signals
    DISABLE_VISUALS =
      1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 3), ///< True if view should not use visuals

    LAST_VIEW_BEHAVIOUR_FLAG
  };

  static const int VIEW_BEHAVIOUR_FLAG_COUNT =
    Log<LAST_VIEW_BEHAVIOUR_FLAG - 1>::value + 1; ///< Total count of flags

protected:
  // Construction

  /**
   * @brief View constructor.
   *
   * @param[in] behaviourFlags Behavioural flags from ViewBehaviour enum
   */
  ViewImpl(ViewBehaviour behaviourFlags);

public: // API for derived classes to override
  // Lifecycle

  /**
   * @brief Second phase initialization.
   */
  void Initialize();

  /**
   * @copydoc Dali::Ui::View::IsResourceReady
   */
  virtual bool IsResourceReady() const;

  /**
   * @copydoc Dali::Ui::View::IsOnScene
   */
  bool IsOnScene() const;

  // Accessibility

  /**
   * @brief This method is called when the view is accessibility activated.
   *
   * Derived classes should override this to perform custom accessibility activation.
   * @return true if this view can perform accessibility activation
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond the accessibility
   * pan gesture.
   *
   * @param[in] gesture The pan gesture
   * @return true if the pan gesture has been consumed by this view
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility up and down action (i.e. value change of slider control).
   *
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this view
   */
  virtual bool OnAccessibilityValueChange(bool isIncrease);

  /**
   * @brief This method should be overridden by deriving classes when they wish to respond
   * the accessibility zoom action.
   *
   * @return true if the zoom action has been consumed by this view
   */
  virtual bool OnAccessibilityZoom();

  /**
   * @brief This method should be overridden by deriving classes when they wish to be
   * represented by a custom Accessible object implementation.
   *
   * The AT-SPI infrastructure is responsible for destroying the returned object.
   *
   * Currently, this method is called at most once in a given View's lifetime, when
   * GetAccessibleObject() is called for the first time. A future version of the
   * AT-SPI infrastructure, however, may delete the Accessible object and request a new
   * one to be created (by calling this method) multiple times, for example during
   * scene connection and disconnection.
   *
   * @return The newly created Accessible object
   *
   * @see GetAccessibleObject()
   */
  virtual ViewAccessible* CreateAccessibleObject();

  // Keyboard focus

  /**
   * @brief Gets the next keyboard focusable actor in this view towards the given direction.
   *
   * A view needs to override this function in order to support two dimensional keyboard navigation.
   * @param[in] currentFocusedActor The current focused actor
   * @param[in] direction The direction to move the focus towards
   * @param[in] loopEnabled Whether the focus movement should be looped within the view
   * @return The next keyboard focusable actor in this view or an empty handle if no actor can be focused
   */
  virtual Actor GetNextKeyboardFocusableActor(Actor              currentFocusedActor,
                                              Ui::FocusDirection direction, bool loopEnabled);

  /**
   * @brief Informs this view that its chosen focusable actor will be focused.
   *
   * This allows the application to perform any actions if wishes
   * before the focus is actually moved to the chosen actor.
   *
   * @param[in] committedFocusableActor The committed focusable actor
   */
  virtual void OnKeyboardFocusChangeCommitted(Actor committedFocusableActor);

  /**
   * @brief This method is called when the view has enter pressed on it.
   *
   * Derived classes should override this to perform custom actions.
   * @return true if this view supported this action
   */
  virtual bool OnKeyboardEnter();

  // Gestures

  /**
   * @brief Called whenever a pinch gesture is detected on this view.
   *
   * This can be overridden by deriving classes when pinch detection
   * is enabled.  The default behaviour is to scale the view by the
   * pinch scale.
   *
   * @param[in] pinch The pinch gesture
   * @note If overridden, then the default behavior will not occur.
   * @note Pinch detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPinch(const PinchGesture& pinch);

  /**
   * @brief Called whenever a pan gesture is detected on this view.
   *
   * This should be overridden by deriving classes when pan detection
   * is enabled.
   *
   * @param[in] pan The pan gesture
   * @note There is no default behavior with panning.
   * @note Pan detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnPan(const PanGesture& pan);

  /**
   * @brief Called whenever a tap gesture is detected on this view.
   *
   * This should be overridden by deriving classes when tap detection
   * is enabled.
   *
   * @param[in] tap The tap gesture
   * @note There is no default behavior with a tap.
   * @note Tap detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnTap(const TapGesture& tap);

  /**
   * @brief Called whenever a long press gesture is detected on this view.
   *
   * This should be overridden by deriving classes when long press
   * detection is enabled.
   *
   * @param[in] longPress The long press gesture
   * @note There is no default behaviour associated with a long press.
   * @note Long press detection should be enabled via EnableGestureDetection().
   * @see EnableGestureDetection
   */
  virtual void OnLongPress(const LongPressGesture& longPress);

  // From ConnectionTrackerInterface

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  void SignalConnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  void SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @brief Retrieves the extension for this view.
   *
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetViewExtension()
  {
    return NULL;
  }

  /**
   * @brief Update visual properties.
   * @param[in] properties Property list to be used to update visual properties of this View.
   */
  virtual void OnUpdateVisualProperties(
    const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
  {
  }

public:
  class DALI_INTERNAL ViewDataImpl; // Class declaration is public so we can internally add devel API's to the Views Impl

private:
  Internal::ViewDataImpl* mImpl;

  // From view.h

public:
  /// @brief AccessibilityDoGesture signal type.
  typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

  /// @brief AccessibilityAction signal type.
  typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

  std::vector<Accessibility::Relation> GetAccessibilityRelations();
};

// Helpers for public-api forwarding methods

inline Integration::ViewImpl& GetImpl(Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<Integration::ViewImpl&>(handle);
}

inline const Integration::ViewImpl& GetImpl(const Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const Integration::ViewImpl&>(handle);
}

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
 * @brief Arranges a standalone MATCH_PARENT child within its parent.
 *
 * Standalone children ignore parent padding. MATCH_PARENT axes are
 * expanded to the full parent size minus the child's own margin, then
 * re-measured with the final size before arranging.
 *
 * @param[in] childImpl  The standalone child implementation
 * @param[in,out] childData  Child data (measuredSize / arrangedBounds updated)
 * @param[in] parentFullWidth  Parent's full width (content + padding)
 * @param[in] parentFullHeight Parent's full height (content + padding)
 */
inline void ArrangeStandaloneChild(ViewImpl& childImpl, ViewImpl::ChildData& childData,
                                   float parentFullWidth, float parentFullHeight)
{
  Extents margin  = childImpl.GetMargin();
  float   marginW = static_cast<float>(margin.start + margin.end);
  float   marginH = static_cast<float>(margin.top + margin.bottom);
  float   childW  = childData.measuredSize.width;
  float   childH  = childData.measuredSize.height;

  if(childImpl.GetRequestedWidth() == MATCH_PARENT)
  {
    childW = std::max(0.0f, parentFullWidth - marginW);
  }
  if(childImpl.GetRequestedHeight() == MATCH_PARENT)
  {
    childH = std::max(0.0f, parentFullHeight - marginH);
  }
  if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
  {
    childImpl.Measure(childW, childH);
  }

  LayoutRect bounds(childImpl.GetPositionX() + static_cast<float>(margin.start),
                    childImpl.GetPositionY() + static_cast<float>(margin.top),
                    childW, childH);
  childImpl.Arrange(bounds);
  childData.arrangedBounds = bounds;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
