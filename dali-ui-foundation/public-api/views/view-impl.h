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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/extents.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/types/view-state.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/state-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
class Layout;
class LayoutManager;
class LayoutTransition;
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
   * @brief Flags for the constructor.
   */
  enum ViewBehaviour
  {
    VIEW_BEHAVIOUR_DEFAULT = 0,                                     ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                                                    ///< but doesn't receive event callbacks.
    DISABLE_VISUALS = 1 << (CustomActorImpl::ACTOR_FLAG_COUNT + 0), ///< True if view should not use visuals

    LAST_VIEW_BEHAVIOUR_FLAG
  };

public: // ABI-frozen virtual API
  // ============================================================
  // WARNING: Do NOT add, reorder, remove, or change signatures.
  // ============================================================

  /**
   * @copydoc Dali::Ui::View::IsResourceReady
   */
  virtual bool IsResourceReady() const;

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
  View::StateChangedSignalType& StateChangedSignal();

  /**
   * @copydoc Ui::View::LayoutFinishedSignal()
   */
  View::LayoutFinishedSignalType& LayoutFinishedSignal();

  /**
   * @copydoc Ui::View::GetEffectiveLayoutDirection()
   */
  Dali::LayoutDirection::Type GetEffectiveLayoutDirection() const;

  /**
   * @copydoc Ui::View::SetRequestedX()
   */
  void SetRequestedX(float x);

  /**
   * @copydoc Ui::View::SetRequestedY()
   */
  void SetRequestedY(float y);

  /**
   * @copydoc Ui::View::GetRequestedX()
   */
  float GetRequestedX() const;

  /**
   * @copydoc Ui::View::GetRequestedY()
   */
  float GetRequestedY() const;

  /**
   * @copydoc Ui::View::SetBackgroundColor()
   */
  void SetBackgroundColor(const UiColor& color);

  /**
   * @copydoc Ui::View::GetBackgroundColor()
   */
  UiColor GetBackgroundColor();

  /**
   * @copydoc Ui::View::SetBackgroundImage()
   */
  void SetBackgroundImage(const Dali::String& url);

  /**
   * @copydoc Ui::View::SetBackgroundGradient()
   */
  void SetBackgroundGradient(const Gradient::Base& gradient);

  /**
   * @copydoc Ui::View::GetColor()
   */
  UiColor GetColor() const;

  /**
   * @copydoc Ui::View::SetColor()
   */
  void SetColor(const UiColor& color);

  /**
   * @copydoc Ui::View::GetCurrentColor()
   */
  UiColor GetCurrentColor() const;

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
   * @copydoc Ui::View::ClearBackground()
   */
  void ClearBackground();

  /**
   * @copydoc Ui::View::SetShadow()
   */
  void SetShadow(const Shadow& shadow);

  /**
   * @copydoc Ui::View::SetShadow(const ShadowStack&)
   */
  void SetShadow(const ShadowStack& shadowStack);

  /**
   * @brief Clears all shadows from this View.
   */
  void ClearShadow();

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
   * @brief Ensures this View has an interaction trait and returns it.
   */
  Ui::InteractiveTrait EnsureInteractiveTrait();

  /**
   * @copydoc Ui::View::IsInteractive()
   */
  bool IsInteractive() const;

  /**
   * @copydoc Ui::View::SetStateEffect()
   */
  void SetStateEffect(StateEffect effect);

  /**
   * @copydoc Ui::View::SetStateEffectTarget()
   */
  void SetStateEffectTarget(View target);

  /**
   * @copydoc Ui::View::GetStateEffectTarget()
   */
  View GetStateEffectTarget() const;

  /**
   * @brief Ensures this View has a selectable trait and returns it.
   */
  Ui::SelectableTrait EnsureSelectableTrait();

  /**
   * @copydoc Ui::View::IsSelectable()
   */
  bool IsSelectable() const;

  /**
   * @brief Ensures this View has a group-selectable trait and returns it.
   */
  Ui::GroupSelectableTrait EnsureGroupSelectableTrait();

  /**
   * @copydoc Ui::View::IsGroupSelectable()
   */
  bool IsGroupSelectable() const;

  /**
   * @copydoc Ui::View::SetAttachment()
   */
  DALI_INTERNAL void SetAttachment(AttachmentId id, UniqueAny attachment);

  /**
   * @copydoc Ui::View::RemoveAttachment()
   */
  bool RemoveAttachment(AttachmentId id);

  /**
   * @brief Gets a raw attachment.
   *
   * @param[in] id The key to identify the attachment
   * @return Pointer to the attachment, or nullptr if missing
   */
  DALI_INTERNAL UniqueAny* GetAttachment(AttachmentId id);

  /**
   * @brief Gets a const raw attachment.
   *
   * @param[in] id The key to identify the attachment
   * @return Pointer to the attachment, or nullptr if missing
   */
  DALI_INTERNAL const UniqueAny* GetAttachment(AttachmentId id) const;

  /**
   * @brief Detaches a raw attachment.
   *
   * @param[in] id The key to identify the attachment
   * @return The stored attachment, or an empty UniqueAny if missing
   */
  DALI_INTERNAL UniqueAny DetachAttachment(AttachmentId id);

  // VisualBase

  /**
   * @copydoc Ui::View::AddVisual()
   */
  bool AddVisual(Dali::Ui::VisualBase visualBase, Dali::Ui::Visual::ContainerRangeType containerRangeType);

  /**
   * @copydoc Ui::View::RemoveVisual()
   */
  void RemoveVisual(Dali::Ui::VisualBase visualBase);

  /**
   * @copydoc Ui::View::GetVisualCount()
   */
  uint32_t GetVisualCount(Dali::Ui::Visual::ContainerRangeType containerRangeType) const;

  /**
   * @copydoc Ui::View::GetVisualAt()
   */
  Dali::Ui::VisualBase GetVisualAt(Dali::Ui::Visual::ContainerRangeType containerRangeType, uint32_t siblingOrder) const;

  // Measure / Arrange

  /**
   * @copydoc Ui::View::Measure()
   */
  MeasuredSize Measure(float widthConstraint, float heightConstraint);

  /**
   * @copydoc Ui::View::Arrange()
   */
  LayoutRect Arrange(const LayoutRect& bounds);

  /**
   * @copydoc Ui::View::InvalidateMeasure()
   */
  void InvalidateMeasure();

  // UiScale API

  /**
   * @brief Sets the UiScale participation policy for this view.
   * @param[in] policy The UiScalePolicy to apply
   */
  void SetUiScalePolicy(UiScalePolicy policy);

  /**
   * @brief Gets the UiScale participation policy of this view.
   * @return The current UiScalePolicy
   */
  UiScalePolicy GetUiScalePolicy() const;

  /**
   * @brief Gets the effective scale for this view.
   * Lazily computed on first call after invalidation; valid after first Measure.
   * @return The effective scale factor
   */
  float GetEffectiveScale() const;

  /**
   * @copydoc Ui::View::InvalidateArrange()
   */
  void InvalidateArrange();

  /**
   * @copydoc Ui::View::GetMeasuredSize()
   */
  MeasuredSize GetMeasuredSize() const;

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
  void SetMargin(const Insets& margin);

  /**
   * @copydoc Ui::View::GetMargin()
   */
  Insets GetMargin() const;

  /**
   * @copydoc Ui::View::SetPadding()
   */
  void SetPadding(const Insets& padding);

  /**
   * @copydoc Ui::View::GetPadding()
   */
  Insets GetPadding() const;

  /**
   * @copydoc Ui::View::SetLayoutMode()
   */
  void SetLayoutMode(Ui::LayoutMode mode);

  /**
   * @copydoc Ui::View::GetLayoutMode()
   */
  Ui::LayoutMode GetLayoutMode() const;

  // Layout Callbacks

  /**
   * @copydoc Ui::View::SetMeasureCallback()
   */
  void SetMeasureCallback(MeasureCallback callback);

  /**
   * @copydoc Ui::View::SetArrangeCallback()
   */
  void SetArrangeCallback(ArrangeCallback callback);

  // Layout Manager

  /**
   * @copydoc Ui::View::AttachLayoutManager()
   */
  void AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager);

  /**
   * @copydoc Ui::View::SetLayoutTransition()
   */
  void SetLayoutTransition(LayoutTransition transition);

  /**
   * @copydoc Ui::View::GetLayoutTransition()
   */
  LayoutTransition GetLayoutTransition() const;

  /**
   * @brief Returns the bounds last applied during the layout pass.
   *
   * Internal helper used by the layout transition dispatcher to compare
   * pre/post bounds for the CHANGE slot. Returns a zero rect if the view
   * has never been arranged.
   *
   * @return The most recent arranged bounds
   */
  LayoutRect GetArrangedBounds() const;

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
   * @copydoc Ui::View::RemoveAllChildren(RemovePolicy)
   */
  void RemoveAllChildren(Ui::RemovePolicy policy);

  /**
   * @copydoc Ui::View::Remove(Ui::View, RemovePolicy)
   */
  void Remove(Ui::View child, Ui::RemovePolicy policy);

  /**
   * @copydoc Ui::View::GetChildViewCount()
   * @note Number of children in the internal logical child list (mChildren),
   * excluding in-flight EXIT ghosts.
   */
  uint32_t GetChildViewCount() const;

  /**
   * @copydoc Ui::View::GetChildViewAt()
   * @note Child at @p index in the internal logical child list (mChildren),
   * or an empty handle if out of range.
   */
  Ui::View GetChildViewAt(uint32_t index) const;

  /**
   * @copydoc Ui::View::IndexOfChildView()
   * @note Logical index of @p view in mChildren, or -1 if not a logical child.
   */
  int32_t IndexOfChildView(Ui::View view) const;

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

  // Focus Navigation

  /**
   * @brief Sets a callback for focus navigation within this view's children.
   *
   * When set, the callback takes priority over the OnFocusNavigationRequested()
   * virtual method. The focus manager calls RequestFocusNavigation() which
   * dispatches to the callback if set, otherwise to the virtual method.
   *
   * @param[in] callback The focus navigation callback (move-only, ownership transferred)
   */
  void SetFocusNavigationCallback(Callback<View(View, FocusDirection)> callback);

  // Signals

  /**
   * @copydoc Ui::View::KeyEventSignal()
   */
  Ui::View::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Ui::View::FocusChangedSignal()
   */
  Ui::View::FocusChangedSignalType& FocusChangedSignal();

  /// @cond internal
  DALI_INTERNAL void NotifyFocusChanged(bool focused);
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
   * @brief Called after the actor has been initialized.
   */
  virtual void OnInitialize();

  /**
   * @brief Called during measure pass. Override to implement custom measurement.
   * @param[in] widthConstraint  Available visual (scale-applied) width, or WRAP_CONTENT / MATCH_PARENT.
   * @param[in] heightConstraint Available visual (scale-applied) height, or WRAP_CONTENT / MATCH_PARENT.
   * @return Measured visual (scale-applied) size.
   */
  virtual MeasuredSize OnMeasure(float widthConstraint, float heightConstraint);

  /**
   * @brief Called during arrange pass. Override to implement custom arrangement.
   *
   * Return the view's final self bounds (parent-local, pre-RTL logical). The
   * framework validates the returned rect and applies its x/y/width/height to
   * the self actor; do NOT call self geometry setters. Default echoes @p bounds.
   */
  virtual LayoutRect OnArrange(const LayoutRect& bounds);

  /**
   * @brief Filters a key event before it is emitted through KeyEventSignal().
   *
   * Override this to consume key events that must be handled before
   * application key-event listeners and the default key-event handler.
   *
   * @param[in] event The key event.
   * @return True if the event is consumed.
   */
  virtual bool FilterKeyEvent(const Dali::KeyEvent& event);

  /**
   * @brief Called when a key event is received.
   * @param[in] event The key event
   * @return True if the event is consumed
   */
  virtual bool OnKeyEvent(const Dali::KeyEvent& event);

  /**
   * @brief Called when the view's focus state changes. Override to add
   *        custom focus handling. Call the base class at the end of the
   *        override to preserve default state management.
   * @param[in] focused True if the view gained focus, false if lost
   */
  virtual void OnFocusChanged(bool focused);

  /**
   * @brief Called when the focus manager requests the next focusable view within this container.
   *
   * Override to provide custom focus navigation logic. If a callback is set via
   * SetFocusNavigationCallback(), the callback takes priority and this method is not called.
   *
   * @param[in] currentFocusedView The current focused view
   * @param[in] direction The direction to move the focus towards
   * @return The next focusable view or an empty handle if no view can be focused
   */
  virtual View OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction);

  /**
   * @brief Called when focus is requested on this view via RequestFocus().
   *
   * For layout-capable views (a Layout, or any View with a LayoutManager attached
   * or a Measure / Arrange callback set) the default implementation delegates focus
   * to the first focusable child, falling back to Self(). For all other views it
   * returns Self() if this view is focusable, enabled, and visible.
   *
   * @return The view that should receive focus, or an empty handle if focus cannot be accepted
   */
  virtual View OnFocusRequested();

  /**
   * @brief Called when the effective scale changes.
   *
   * Subclasses (Label, InputField) override this to update scale-dependent
   * content such as font sizes.
   *
   * @param[in] newScale The new effective scale factor
   */
  virtual void OnEffectiveScaleChanged(float newScale)
  {
  }

  // ============================================================
  // protected: Framework overrides (CustomActorImpl)
  // ============================================================

  /**
   * @copydoc Dali::RefObject::OnDestroy()
   */
  void OnDestroy() override;

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
   * @note Valid only inside OffScreenRenderingFinishedSignal() with REFRESH_ONCE type.
   */
  Dali::Texture GetOffScreenRenderingOutput() const;

  /**
   * @brief Registers a color binding for theme-aware color updates.
   *
   * A binding ID represents a stable callback slot. If a binding with the same
   * ID already exists, this method updates only the bound color; it does not
   * replace the existing callback. Call ClearBinding() before reusing an ID with
   * a different target object or setter.
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
    if(!UpdateColorBindingInternal(bindingId, color))
    {
      SetColorBindingInternal(bindingId, color, ColorCallback::New(inst, setter));
    }
    (inst->*setter)(color.GetRgba());
  }

  /**
   * @brief Registers a gradient color binding for theme-aware gradient updates.
   *
   * A binding ID represents a stable callback slot. If a binding with the same
   * ID already exists, this method updates only the stored gradient; it does not
   * replace the existing callback. Call ClearGradientColorBinding() before
   * reusing an ID with a different target object or setter.
   *
   * @tparam T        Type of the instance (ViewImpl or a derived class)
   * @param[in] bindingId  Caller-defined identifier for this gradient binding
   * @param[in] gradient   The gradient to apply
   * @param[in] inst       The object whose @a setter will be used as the callback
   * @param[in] setter     Member function called both immediately and on theme change
   */
  template<typename T>
  void SetColorBinding(StringView bindingId, const Gradient::Base& gradient, T* inst, void (T::*setter)(const Gradient::Base&))
  {
    if(!UpdateColorBindingInternal(bindingId, gradient))
    {
      SetColorBindingInternal(bindingId, gradient, Callback<void(const Gradient::Base&)>::New(inst, setter));
    }
    (inst->*setter)(gradient);
  }

  /**
   * @brief Gets the LayoutManager, if attached.
   *
   * Returned pointer is non-owning. Ownership remains with the View's trait.
   *
   * @return Pointer to the LayoutManager, or nullptr if not attached
   */
  LayoutManager* GetLayoutManager() const;

  // ============================================================
  // private
  // ============================================================

private:
  friend class Internal::ViewDataImpl; ///< Pimpl body

  Internal::ViewDataImpl& GetViewDataImpl() const;

private:
  ViewImpl(const ViewImpl&)            = delete;
  ViewImpl(ViewImpl&&)                 = delete;
  ViewImpl& operator=(const ViewImpl&) = delete;
  ViewImpl& operator=(ViewImpl&&)      = delete;

  /**
   * @brief Updates or clears an existing color binding.
   * @return True when no callback registration is needed; false when the caller should register a new callback.
   */
  bool UpdateColorBindingInternal(StringView bindingId, const UiColor& color);
  void SetColorBindingInternal(StringView bindingId, const UiColor& color, ColorCallback callback);

  /**
   * @brief Updates or clears an existing gradient color binding.
   * @return True when no callback registration is needed; false when the caller should register a new callback.
   */
  bool UpdateColorBindingInternal(StringView bindingId, const Gradient::Base& gradient);
  void SetColorBindingInternal(StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback);
  void ClearGradientColorBinding(StringView bindingId);

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
