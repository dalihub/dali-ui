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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-focus-enums.h>
#include <dali-ui-foundation/public-api/view-state.h>
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
class ViewDataImpl;
} //namespace Internal

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
  // ============================================================
  // Types
  // ============================================================

  class Extension; ///< Forward declare future extension interface

  /**
   * @brief Child data structure for layout calculations.
   */
  struct ChildData
  {
    Ui::View     view;           ///< Handle to the child view
    MeasuredSize measuredSize;   ///< Size from Measure pass
    LayoutRect   arrangedBounds; ///< Bounds from Arrange pass
  };

  using ChildContainer         = Dali::Vector<ChildData>;
  using StateChangedSignalType = Signal<void(Ui::View, const StateEvent&)>;

  /// @brief AccessibilityDoGesture signal type.
  typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

  /// @brief AccessibilityAction signal type.
  typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

  /**
   * @brief Flags for the constructor.
   */
  enum ViewBehaviour
  {
    VIEW_BEHAVIOUR_DEFAULT = 0, ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                ///< but doesn't receive event callbacks.
    NOT_IN_USE_1                    = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 0),
    REQUIRES_KEY_NAVIGATION_SUPPORT = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 1), ///< True if needs to support key navigation
    DISABLE_VISUALS                 = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 2), ///< True if view should not use visuals

    LAST_VIEW_BEHAVIOUR_FLAG
  };

  static const int VIEW_BEHAVIOUR_FLAG_COUNT = Log<LAST_VIEW_BEHAVIOUR_FLAG - 1>::value + 1; ///< Total count of flags

public: // ABI-frozen virtual API
  // ============================================================
  // WARNING: Do NOT add, reorder, remove, or change signatures.
  // ============================================================

  /**
   * @brief Called after the actor has been initialized.
   */
  virtual void OnInitialize();

  /**
   * @brief Called when a key event is received.
   * @param[in] event The key event
   * @return True if the event is consumed
   */
  virtual bool OnKeyEvent(const Dali::KeyEvent& event);

  /**
   * @copydoc Dali::Ui::View::IsResourceReady
   */
  virtual bool IsResourceReady() const;

  /**
   * @brief Returns whether this view acts as a layout container.
   * @return True if this view has a LayoutManager attached
   */
  virtual bool IsLayout() const;

  /**
   * @brief This method is called when the view is accessibility activated.
   * @return true if this view can perform accessibility activation
   */
  virtual bool OnAccessibilityActivated();

  /**
   * @brief Called when accessibility pan gesture is received.
   * @param[in] gesture The pan gesture
   * @return true if the pan gesture has been consumed by this view
   */
  virtual bool OnAccessibilityPan(PanGesture gesture);

  /**
   * @brief Called for accessibility value change (e.g. slider up/down).
   * @param[in] isIncrease Whether the value should be increased or decreased
   * @return true if the value changed action has been consumed by this view
   */
  virtual bool OnAccessibilityValueChange(bool isIncrease);

  /**
   * @brief Called for accessibility zoom action.
   * @return true if the zoom action has been consumed by this view
   */
  virtual bool OnAccessibilityZoom();

  /**
   * @brief Creates a custom Accessible object for this view.
   *
   * The AT-SPI infrastructure is responsible for destroying the returned object.
   * @return The newly created Accessible object
   * @see GetAccessibleObject()
   */
  virtual ViewAccessible* CreateAccessibleObject();

  /**
   * @brief Gets the next focusable actor in this view towards the given direction.
   *
   * A view needs to override this function in order to support two dimensional key navigation.
   * @param[in] currentFocusedActor The current focused actor
   * @param[in] direction The direction to move the focus towards
   * @param[in] loopEnabled Whether the focus movement should be looped within the view
   * @return The next focusable actor in this view or an empty handle if no actor can be focused
   */
  virtual Actor GetNextFocusableActor(Actor currentFocusedActor, Ui::FocusDirection direction, bool loopEnabled);

  /**
   * @brief Informs this view that its chosen focusable actor will be focused.
   * @param[in] committedFocusableActor The committed focusable actor
   */
  virtual void OnFocusChangeCommitted(Actor committedFocusableActor);

  /**
   * @brief Called when the view has enter pressed on it.
   * @return true if this view supported this action
   */
  virtual bool OnKeyboardEnter();

  /**
   * @brief Retrieves SourceActor of the OffScreenRenderable.
   * @return SourceActor of the OffScreenRenderable.
   */
  virtual Dali::Actor GetOffScreenRenderableSourceActor();

  /**
   * @brief Retrieves whether the OffScreen RenderTasks is exclusive or not.
   * @return True if the RenderTask is exclusive.
   */
  virtual bool IsOffScreenRenderTaskExclusive();

  /**
   * @brief Retrieves the extension for this view.
   * @return The extension if available, NULL otherwise
   */
  virtual Extension* GetViewExtension()
  {
    return NULL;
  }

public: // Non-virtual API (safe to reorder / extend)
  /**
   * @brief Creates a new View.
   */
  static ViewImplPtr New();

  /**
   * @brief Second phase initialization.
   */
  void Initialize();

  /**
   * @copydoc Ui::View::GetState()
   */
  const ViewState& GetState() const;

  /**
   * @copydoc Ui::View::IsEnabled()
   */
  bool IsEnabled() const;

  /**
   * @copydoc Ui::View::SetEnabled()
   */
  void SetEnabled(bool enabled);

  /**
   * @copydoc Ui::View::IsEffectivelyEnabled()
   */
  bool IsEffectivelyEnabled() const;

  /**
   * @copydoc Ui::View::IsEffectivelyFocused()
   */
  bool IsEffectivelyFocused() const;

  /**
   * @copydoc Ui::View::StateChangedSignal()
   */
  StateChangedSignalType& StateChangedSignal();

  /**
   * @brief Registers a named state-change handler using a member function.
   * @param[in] id   Unique identifier for this handler
   * @param[in] obj  Object whose member function will be called
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
   * @param[in] id The handler identifier to remove
   * @return True if a handler was found and removed
   */
  bool UnsetStateHandler(const Dali::String& id);

  /**
   * @brief Removes a named state-change handler only if it is not currently being processed.
   * @param[in] id The handler identifier to remove
   * @return True if removed, false if currently processing or not found
   */
  bool UnsetStateHandlerWhenNotProcessing(const Dali::String& id);

  /**
   * @brief Updates a state bit in the view's ViewState and emits StateChangedSignal.
   * @param[in] state The state to set or clear
   * @param[in] on    True to add the state, false to remove it
   * @param[in] cause Input event that triggered the change; leave default if programmatic
   */
  void SetState(ViewState state, bool on, InputEvent cause = InputEvent::None());

  /**
   * @brief Notifies this view that its focus state has changed.
   *
   * Called by focus managers. Invokes the protected virtual OnFocusChanged()
   * and emits the FocusChanged signal.
   *
   * @param[in] focused True if the view gained focus, false if lost
   */
  void NotifyFocusChanged(bool focused);

  /**
   * @copydoc Ui::View::GetScaleX()
   */
  float GetScaleX() const;

  /**
   * @copydoc Ui::View::SetScaleX()
   */
  void SetScaleX(float scaleX);

  /**
   * @copydoc Ui::View::GetScaleY()
   */
  float GetScaleY() const;

  /**
   * @copydoc Ui::View::SetScaleY()
   */
  void SetScaleY(float scaleY);

  /**
   * @copydoc Ui::View::IsVisible()
   */
  bool IsVisible() const;

  /**
   * @copydoc Ui::View::SetVisibility()
   */
  void SetVisibility(bool visibility);

  /**
   * @copydoc Ui::View::GetOpacity()
   */
  float GetOpacity() const;

  /**
   * @copydoc Ui::View::SetOpacity()
   */
  void SetOpacity(float opacity);

  /**
   * @copydoc Ui::View::GetSize()
   */
  MeasuredSize GetSize() const;

  /**
   * @copydoc Ui::View::GetPositionX()
   */
  float GetPositionX() const;

  /**
   * @copydoc Ui::View::SetPositionX()
   */
  void SetPositionX(float x);

  /**
   * @copydoc Ui::View::GetPositionY()
   */
  float GetPositionY() const;

  /**
   * @copydoc Ui::View::SetPositionY()
   */
  void SetPositionY(float y);

  /**
   * @copydoc Ui::View::GetParentOrigin()
   */
  Vector3 GetParentOrigin() const;

  /**
   * @copydoc Ui::View::SetParentOrigin()
   */
  void SetParentOrigin(const Vector3& point);

  /**
   * @copydoc Ui::View::GetPivot()
   */
  Vector3 GetPivot() const;

  /**
   * @copydoc Ui::View::SetPivot()
   */
  void SetPivot(const Vector3& point);

  /**
   * @copydoc Ui::View::IsFocusable()
   */
  bool IsFocusable() const;

  /**
   * @copydoc Ui::View::SetFocusable()
   */
  void SetFocusable(bool focusable);

  /**
   * @copydoc Ui::View::IsTouchFocusable()
   */
  bool IsTouchFocusable() const;

  /**
   * @copydoc Ui::View::SetTouchFocusable()
   */
  void SetTouchFocusable(bool touchFocusable);

  /**
   * @copydoc Ui::View::GetBackgroundColor()
   */
  UiColor GetBackgroundColor();

  /**
   * @copydoc Ui::View::SetBackgroundColor()
   */
  void SetBackgroundColor(const UiColor& color);

  /**
   * @copydoc Ui::View::GetCornerRadius()
   */
  Vector4 GetCornerRadius() const;

  /**
   * @copydoc Ui::View::SetCornerRadius()
   */
  void SetCornerRadius(const Vector4& radius);

  /**
   * @copydoc Ui::View::GetCornerRadiusPolicy()
   */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;

  /**
   * @copydoc Ui::View::SetCornerRadiusPolicy()
   */
  void SetCornerRadiusPolicy(CornerRadiusPolicy policy);

  /**
   * @copydoc Ui::View::GetCornerSquareness()
   */
  Vector4 GetCornerSquareness() const;

  /**
   * @copydoc Ui::View::SetCornerSquareness()
   */
  void SetCornerSquareness(const Vector4& squareness);

  /**
   * @copydoc Ui::View::GetBorderlineWidth()
   */
  float GetBorderlineWidth() const;

  /**
   * @copydoc Ui::View::SetBorderlineWidth()
   */
  void SetBorderlineWidth(float width);

  /**
   * @copydoc Ui::View::GetBorderlineColor()
   */
  UiColor GetBorderlineColor();

  /**
   * @copydoc Ui::View::SetBorderlineColor()
   */
  void SetBorderlineColor(const UiColor& color);

  /**
   * @copydoc Ui::View::GetBorderlineOffset()
   */
  float GetBorderlineOffset() const;

  /**
   * @copydoc Ui::View::SetBorderlineOffset()
   */
  void SetBorderlineOffset(float offset);

  /**
   * @copydoc Ui::View::GetName()
   */
  Dali::String GetName() const;

  /**
   * @copydoc Ui::View::SetName()
   */
  void SetName(const Dali::String& name);

  /**
   * @brief Sets the background with a property map.
   * @param[in] map The background property map
   */
  void SetBackground(const Property::Map& map);

  /**
   * @copydoc Ui::View::ClearBackground()
   */
  void ClearBackground();

  /**
   * @copydoc Ui::View::SetRenderEffect()
   */
  void SetRenderEffect(Ui::RenderEffect effect);

  /**
   * @copydoc Ui::View::GetRenderEffect()
   */
  RenderEffect GetRenderEffect() const;

  /**
   * @copydoc Ui::View::ClearRenderEffect()
   */
  void ClearRenderEffect();

  /**
   * @copydoc Ui::View::SetTrait()
   */
  void SetTrait(TraitId id, Trait& trait);

  /**
   * @copydoc Ui::View::GetTrait()
   */
  Trait GetTrait(TraitId id) const;

  /**
   * @copydoc Ui::View::RemoveTrait()
   */
  bool RemoveTrait(TraitId id);

  /**
   * @copydoc Ui::View::EnsureInteractiveTrait()
   */
  Ui::InteractiveTrait EnsureInteractiveTrait();

  /**
   * @copydoc Ui::View::IsInteractive()
   */
  bool IsInteractive() const;

  /**
   * @copydoc Ui::View::EnsureSelectableTrait()
   */
  Ui::SelectableTrait EnsureSelectableTrait();

  /**
   * @copydoc Ui::View::IsSelectable()
   */
  bool IsSelectable() const;

  /**
   * @copydoc Ui::View::SetInteractionEffect()
   */
  void SetInteractionEffect(Trait effect);

  // Measure / Arrange

  /**
   * @copydoc Ui::View::Measure()
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @copydoc Ui::View::Arrange()
   */
  MeasuredSize Arrange(const LayoutRect& bounds);

  /**
   * @copydoc Ui::View::InvalidateMeasure()
   */
  void InvalidateMeasure();

  /**
   * @copydoc Ui::View::InvalidateArrange()
   */
  void InvalidateArrange();

  /**
   * @copydoc Ui::View::GetMeasuredSize()
   */
  MeasuredSize GetMeasuredSize() const;

  /**
   * @copydoc Ui::View::IsMeasureValid()
   */
  bool IsMeasureValid() const;

  /**
   * @copydoc Ui::View::IsArrangeValid()
   */
  bool IsArrangeValid() const;

  // Requested Size

  /**
   * @copydoc Ui::View::SetRequestedWidth()
   */
  void SetRequestedWidth(float width);

  /**
   * @copydoc Ui::View::GetRequestedWidth()
   */
  float GetRequestedWidth() const;

  /**
   * @copydoc Ui::View::SetRequestedHeight()
   */
  void SetRequestedHeight(float height);

  /**
   * @copydoc Ui::View::GetRequestedHeight()
   */
  float GetRequestedHeight() const;

  /**
   * @copydoc Ui::View::SetMinimumWidth()
   */
  void SetMinimumWidth(float width);

  /**
   * @copydoc Ui::View::GetMinimumWidth()
   */
  float GetMinimumWidth() const;

  /**
   * @copydoc Ui::View::SetMinimumHeight()
   */
  void SetMinimumHeight(float height);

  /**
   * @copydoc Ui::View::GetMinimumHeight()
   */
  float GetMinimumHeight() const;

  /**
   * @copydoc Ui::View::SetMaximumWidth()
   */
  void SetMaximumWidth(float width);

  /**
   * @copydoc Ui::View::GetMaximumWidth()
   */
  float GetMaximumWidth() const;

  /**
   * @copydoc Ui::View::SetMaximumHeight()
   */
  void SetMaximumHeight(float height);

  /**
   * @copydoc Ui::View::GetMaximumHeight()
   */
  float GetMaximumHeight() const;

  // Layout Params

  /**
   * @copydoc Ui::View::SetLayoutParams()
   */
  void SetLayoutParams(Ui::LayoutParams params);

  /**
   * @brief Retrieves a layout params trait by LayoutParamsType.
   * @param[in] type The layout params type
   * @return The layout params handle
   */
  BaseHandle GetLayoutParams(LayoutParamsType type) const;

  // Layout Properties

  /**
   * @copydoc Ui::View::SetMargin()
   */
  void SetMargin(const Extents& margin);

  /**
   * @copydoc Ui::View::GetMargin()
   */
  Extents GetMargin() const;

  /**
   * @copydoc Ui::View::SetPadding()
   */
  void SetPadding(const Extents& padding);

  /**
   * @copydoc Ui::View::GetPadding()
   */
  Extents GetPadding() const;

  /**
   * @copydoc Ui::View::SetLayoutMode()
   */
  void SetLayoutMode(Ui::LayoutMode mode);

  /**
   * @copydoc Ui::View::GetLayoutMode()
   */
  Ui::LayoutMode GetLayoutMode() const;

  /**
   * @brief Convenience: returns true if LayoutMode is Standalone.
   * @return True if the view's layout mode is Standalone
   */
  bool IsLayoutModeStandalone() const;

  // Parent Layout

  /**
   * @brief Gets the parent layout of this view, if any.
   * @return The parent Layout handle, or empty if none
   */
  Ui::Layout GetParentLayout() const;

  /**
   * @brief Gets the parent view in the layout hierarchy.
   * @return The parent View handle, or empty if none
   */
  Ui::View GetParentView() const;

  // Layout Callbacks

  /**
   * @copydoc Ui::View::SetMeasureCallback()
   */
  void SetMeasureCallback(MeasureCallback callback);

  /**
   * @copydoc Ui::View::SetArrangeCallback()
   */
  void SetArrangeCallback(ArrangeCallback callback);

  // Child Management

  /**
   * @copydoc Ui::View::Insert()
   */
  void Insert(uint32_t index, Ui::View child);

  /**
   * @copydoc Ui::View::RemoveAllChildren()
   */
  void RemoveAllChildren();

  /**
   * @copydoc Ui::View::GetChildCount()
   */
  uint32_t GetChildCount() const;

  /**
   * @copydoc Ui::View::GetChildAt()
   */
  Ui::View GetChildAt(uint32_t index) const;

  /**
   * @copydoc Ui::View::IndexOfChild()
   */
  int32_t IndexOfChild(Ui::View view) const;

  /**
   * @copydoc Ui::View::Raise()
   */
  void Raise(Ui::LayoutOrderPolicy policy);

  /**
   * @copydoc Ui::View::Lower()
   */
  void Lower(Ui::LayoutOrderPolicy policy);

  /**
   * @copydoc Ui::View::RaiseToTop()
   */
  void RaiseToTop(Ui::LayoutOrderPolicy policy);

  /**
   * @copydoc Ui::View::LowerToBottom()
   */
  void LowerToBottom(Ui::LayoutOrderPolicy policy);

  /**
   * @copydoc Ui::View::RaiseAbove()
   */
  void RaiseAbove(Ui::View target, Ui::LayoutOrderPolicy policy);

  /**
   * @copydoc Ui::View::LowerBelow()
   */
  void LowerBelow(Ui::View target, Ui::LayoutOrderPolicy policy);

  /**
   * @brief Gets the children container for layout manager access.
   * @return Reference to the children container
   */
  ChildContainer& GetChildren();

  /**
   * @copydoc GetChildren()
   */
  const ChildContainer& GetChildren() const;

  // Accessibility

  /**
   * @brief Gets the Accessible object that represents this view.
   * @return The Accessible object
   * @see CreateAccessibleObject()
   */
  std::shared_ptr<Ui::ViewAccessible> GetAccessibleObject();

  /**
   * @brief Gets the accessibility relations of this view.
   * @return The list of accessibility relations
   */
  Dali::Vector<Accessibility::Relation> GetAccessibilityRelations();

  // Key Navigation & Focus

  /**
   * @brief Sets whether this view supports two dimensional key navigation.
   * @param[in] isSupported True to support key navigation
   */
  void SetKeyNavigationSupport(bool isSupported);

  /**
   * @brief Gets whether this view supports two dimensional key navigation.
   * @return True if key navigation is supported
   */
  bool IsKeyNavigationSupported();

  /**
   * @brief Sets this view to receive key input focus.
   */
  void SetKeyInputFocus();

  /**
   * @brief Checks whether this view has key input focus.
   * @return True if the view has key input focus
   */
  bool HasKeyInputFocus();

  /**
   * @brief Clears key input focus from this view.
   */
  void ClearKeyInputFocus();

  /**
   * @brief Sets whether this view acts as a focus group boundary.
   * @param[in] isFocusGroup True to set as focus group
   */
  void SetAsFocusGroup(bool isFocusGroup);

  /**
   * @brief Gets whether this view acts as a focus group boundary.
   * @return True if the view is a focus group
   */
  bool IsFocusGroup();

  // Signals

  /**
   * @copydoc Ui::View::KeyEventSignal()
   */
  Ui::View::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Ui::View::FocusChangedSignal()
   */
  Ui::View::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc Ui::View::IsOnScene()
   */
  bool IsOnScene() const;

  // Internal data access

  /**
   * @brief Retrieves the internal data implementation of the view.
   * @return Reference to the internal data implementation
   */
  Internal::ViewDataImpl& GetViewDataImpl() const;

  /// @cond internal
  DALI_INTERNAL void KeyboardEnter();
  DALI_INTERNAL bool EmitKeyEventSignal(const KeyEvent& event);
  /// @endcond

protected:
  // ============================================================
  // protected: ABI-frozen virtual API
  // WARNING: Do NOT add, reorder, remove, or change signatures.
  // ============================================================

  /**
   * @brief Destructor. A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ViewImpl();

  /**
   * @brief Called during measure pass. Override to implement custom measurement.
   */
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);

  /**
   * @brief Called during arrange pass. Override to implement custom arrangement.
   */
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

  /**
   * @brief Called when the view's focus state changes. Override to add
   *        custom focus handling. Call the base class at the end of the
   *        override to preserve default state management.
   * @param[in] focused True if the view gained focus, false if lost
   */
  virtual void OnFocusChanged(bool focused);

  // ============================================================
  // protected: Framework overrides (CustomActorImpl)
  // ============================================================

  /**
   * @copydoc Dali::CustomActorImpl::OnSceneConnection()
   */
  void OnSceneConnection(int depth) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnSceneDisconnection()
   */
  void OnSceneDisconnection() override;

  /**
   * @copydoc Dali::CustomActorImpl::OnRelayout()
   */
  void OnRelayout(const Vector2& size, RelayoutContainer& container) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnChildAdd()
   */
  void OnChildAdd(Actor& child) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnChildRemove()
   */
  void OnChildRemove(Actor& child) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnPropertySet()
   */
  void OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnSizeSet()
   */
  void OnSizeSet(const Vector3& targetSize) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnSizeAnimation()
   */
  void OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnAnimateAnimatableProperty()
   */
  void OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Dali::Animation::State state) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnConstraintAnimatableProperty()
   */
  void OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied) override;

  /**
   * @copydoc Dali::CustomActorImpl::GetOffScreenRenderTasks()
   */
  void GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnSetResizePolicy()
   */
  void OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension) override;

  /**
   * @copydoc Dali::CustomActorImpl::GetNaturalSize()
   */
  Vector3 GetNaturalSize() override;

  /**
   * @copydoc Dali::CustomActorImpl::CalculateChildSize()
   */
  float CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension) override;

  /**
   * @copydoc Dali::CustomActorImpl::GetHeightForWidth()
   */
  float GetHeightForWidth(float width) override;

  /**
   * @copydoc Dali::CustomActorImpl::GetWidthForHeight()
   */
  float GetWidthForHeight(float height) override;

  /**
   * @copydoc Dali::CustomActorImpl::RelayoutDependentOnChildren()
   */
  bool RelayoutDependentOnChildren(Dimension::Type dimension = Dimension::ALL_DIMENSIONS) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnCalculateRelayoutSize()
   */
  void OnCalculateRelayoutSize(Dimension::Type dimension) override;

  /**
   * @copydoc Dali::CustomActorImpl::OnLayoutNegotiated()
   */
  void OnLayoutNegotiated(float size, Dimension::Type dimension) override;

  /**
   * @copydoc Dali::ConnectionTrackerInterface::SignalConnected()
   */
  void SignalConnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @copydoc Dali::ConnectionTrackerInterface::SignalDisconnected()
   */
  void SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  // ============================================================
  // protected: Construction & helpers
  // ============================================================

  /**
   * @brief Default constructor.
   */
  ViewImpl();

  /**
   * @brief Constructor with behaviour flags.
   * @param[in] behaviourFlags Behavioural flags from ViewBehaviour enum
   */
  ViewImpl(ViewBehaviour behaviourFlags);

  /**
   * @brief Gets texture output of offscreen rendering.
   * @return The offscreen rendering output texture
   * @note Valid only inside OffScreenRenderingFinishedSignal() with RENDER_ONCE type.
   */
  Dali::Texture GetOffScreenRenderingOutput() const;

  /**
   * @brief Marks this view's resources as ready (does not request relayout).
   */
  void SetResourceReady();

  /**
   * @brief Registers a color binding for theme-aware color updates.
   *
   * @tparam T        Type of the instance (ViewImpl or a derived class)
   * @param[in] bindingId  Caller-defined identifier for this binding
   * @param[in] color      The UiColor to apply
   * @param[in] inst       The object whose @a setter will be used as the callback
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
   * @brief Gets the measure callback, if set.
   * @return Pointer to the MeasureCallback, or nullptr if not set
   */
  MeasureCallback* GetMeasureCallback();

  /**
   * @brief Gets the arrange callback, if set.
   * @return Pointer to the ArrangeCallback, or nullptr if not set
   */
  ArrangeCallback* GetArrangeCallback();

  // ============================================================
  // private
  // ============================================================

private:
  friend class Internal::ViewDataImpl; ///< Pimpl body

  void         EmitFocusChangedSignal(bool focusGained);
  MeasuredSize ApplyConstraints(const MeasuredSize& size) const;
  void         RegisterWithLayoutController();
  void         MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight);
  void         ArrangeStandaloneChildren(const LayoutRect& bounds);

  ViewImpl(const ViewImpl&)            = delete;
  ViewImpl(ViewImpl&&)                 = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&)      = delete;

  void SetBackgroundColorInternal(const Vector4& color);
  void SetBorderlineColorInternal(const Vector4& color);
  void OnChildOrderChanged(Actor orderChangedChild);
  void SetNamedStateHandler(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);

  Internal::ViewDataImpl* mImpl;
};

// Helpers for public-api forwarding methods

inline DALI_UI_API ViewImpl& GetImpl(Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  Dali::RefObject& handle = view.GetImplementation();

  return static_cast<ViewImpl&>(handle);
}

inline DALI_UI_API const ViewImpl& GetImpl(const Ui::View& view)
{
  DALI_ASSERT_ALWAYS(view);

  const Dali::RefObject& handle = view.GetImplementation();

  return static_cast<const ViewImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
