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
#include <dali/public-api/events/long-press-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/tap-gesture.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <limits>
#include <memory>
#include <unordered_set>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
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

namespace Integration
{
class StateEffectImpl;
}

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

  using StateChangedSignalType   = Signal<void(Ui::View, StateEvent)>;
  using LayoutFinishedSignalType = Signal<void(Ui::View, LayoutRect)>;

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

  static const int VIEW_BEHAVIOUR_FLAG_COUNT = Log<LAST_VIEW_BEHAVIOUR_FLAG - 1>::value + 1; ///< Total count of flags

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
   * @copydoc Ui::View::LayoutFinishedSignal()
   */
  LayoutFinishedSignalType& LayoutFinishedSignal();

  /**
   * @brief Whether LayoutFinishedSignal has at least one connected slot.
   */
  DALI_INTERNAL bool HasLayoutFinishedSignalConnections() const;

  /**
   * @brief Emits LayoutFinishedSignal with @p bounds if still connected.
   */
  DALI_INTERNAL void EmitLayoutFinishedSignal(const LayoutRect& bounds);

  /**
   * @copydoc Ui::View::GetScaleX()
   */
  float GetScaleX() const;

  /**
   * @copydoc Ui::View::GetCurrentScaleX()
   */
  float GetCurrentScaleX() const;

  /**
   * @copydoc Ui::View::SetScaleX()
   */
  void SetScaleX(float scaleX);

  /**
   * @copydoc Ui::View::GetScaleY()
   */
  float GetScaleY() const;

  /**
   * @copydoc Ui::View::GetCurrentScaleY()
   */
  float GetCurrentScaleY() const;

  /**
   * @copydoc Ui::View::SetScaleY()
   */
  void SetScaleY(float scaleY);

  /**
   * @copydoc Ui::View::SetLayoutDirection()
   */
  void SetLayoutDirection(Dali::LayoutDirection::Type direction);

  /**
   * @copydoc Ui::View::ClearLayoutDirection()
   */
  void ClearLayoutDirection();

  /**
   * @copydoc Ui::View::IsLayoutDirectionInherited()
   */
  bool IsLayoutDirectionInherited() const;

  /**
   * @copydoc Ui::View::GetEffectiveLayoutDirection()
   */
  Dali::LayoutDirection::Type GetEffectiveLayoutDirection() const;

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
   * @copydoc Ui::View::GetCurrentSize()
   */
  MeasuredSize GetCurrentSize() const;

  /**
   * @copydoc Ui::View::GetPositionX()
   */
  float GetPositionX() const;

  /**
   * @copydoc Ui::View::GetCurrentPositionX()
   */
  float GetCurrentPositionX() const;

  /**
   * @copydoc Ui::View::GetPositionY()
   */
  float GetPositionY() const;

  /**
   * @copydoc Ui::View::GetCurrentPositionY()
   */
  float GetCurrentPositionY() const;

  /**
   * @copydoc Ui::View::SetRequestedPositionX()
   */
  void SetRequestedPositionX(float x);

  /**
   * @copydoc Ui::View::SetRequestedPositionY()
   */
  void SetRequestedPositionY(float y);

  /**
   * @copydoc Ui::View::GetRequestedPositionX()
   */
  float GetRequestedPositionX() const;

  /**
   * @copydoc Ui::View::GetRequestedPositionY()
   */
  float GetRequestedPositionY() const;

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
   * @copydoc Ui::View::GetName()
   */
  Dali::String GetName() const;

  /**
   * @copydoc Ui::View::SetName()
   */
  void SetName(const Dali::String& name);

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
   * @brief Gets whether the current StateEffect suppresses FocusManager's default focus indicator.
   *
   * This is a cached value updated when the StateEffect changes or when the
   * StateEffect implementation invalidates it.
   *
   * @return True if the current StateEffect suppresses the default focus indicator
   */
  bool IsDefaultFocusIndicatorSuppressedByStateEffect() const;

  /**
   * @brief Recomputes the cached default focus indicator suppression state.
   */
  void RefreshDefaultFocusIndicatorSuppression();

  /**
   * @brief Applies the current or default StateEffect after this View becomes interactive.
   */
  void AttachInteractiveStateEffect();

  /**
   * @brief Recomputes default focus indicator suppression if the given effect is currently attached.
   *
   * @param[in] effect The StateEffect implementation requesting invalidation
   */
  void InvalidateDefaultFocusIndicatorSuppression(const Integration::StateEffectImpl& effect);

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
  MeasuredSize Arrange(const LayoutRect& bounds);

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
   * @brief Returns whether this view has a LayoutManager attached.
   *
   * @return True if a LayoutManager is attached
   */
  bool HasLayoutManager() const;

  /**
   * @brief Returns whether this view has a MeasureCallback or
   *        ArrangeCallback set.
   *
   * Used by HasLayoutCapability to recognize callback-driven custom
   * layout views so that legacy relayout and child-first focus behave
   * consistently with Layout subclasses and LayoutManager attachments.
   *
   * @return True if either callback is set
   */
  bool HasLayoutCallback() const;

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

  /**
   * @brief Atomically retrieves and clears the set of children that were
   * added since the previous layout pass.
   *
   * Internal helper used by the layout transition dispatcher to fire
   * ENTER-slot animations. Must be called once per layout pass; subsequent
   * calls before a new OnChildAdd return an empty set.
   *
   * @return Set of child ViewImpl pointers
   */
  std::unordered_set<ViewImpl*> TakePendingEnterChildren();

  /**
   * @brief Atomically retrieves and clears the set of children whose
   * sibling order changed since the previous layout pass.
   *
   * Internal helper used by the layout transition dispatcher to tag
   * CHANGE-slot dispatches with @c LayoutChangeCause::REORDERED.
   *
   * @return Set of child ViewImpl pointers
   */
  std::unordered_set<ViewImpl*> TakePendingReorderedChildren();

  /**
   * @brief Atomically retrieves and clears the marker that records whether
   * any child was removed via @c View::Remove(child, RemovePolicy::ANIMATE_EXIT)
   * or the internal @c ViewImpl::RemoveAllChildren since the last layout pass.
   *
   * Internal helper used by the layout transition dispatcher to tag
   * CHANGE-slot dispatches on the remaining children with
   * @c LayoutChangeCause::SIBLING_REMOVED.
   *
   * @return @c true if a removal occurred and the marker was cleared
   */
  bool TakePendingChildRemovalForLayoutTransition();

  /**
   * @brief Returns @c true if this view has completed at least one
   * @c Arrange pass.
   *
   * Used by the layout transition dispatcher to distinguish initial-mount
   * children from children added at runtime after the parent has already
   * been arranged. Children present at the parent's first arrange pass are
   * treated as the view's "always there" state and ENTER is suppressed for
   * them by default (the surface has typically not been displayed yet, so
   * the fade-in would be invisible). Apps can opt into firing ENTER for
   * initial-mount children via @c LayoutTransition::SetEnterOnInitialMount.
   */
  bool IsInitialLayoutDone() const;

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

  // Focus

  /**
   * @copydoc Ui::View::SetDescendantFocusBlocked
   */
  void SetDescendantFocusBlocked(bool blocked);

  /**
   * @copydoc Ui::View::IsDescendantFocusBlocked
   */
  bool IsDescendantFocusBlocked() const;

  /**
   * @copydoc Ui::View::HasAncestorBlockingFocus
   */
  bool HasAncestorBlockingFocus() const;

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

  /**
   * @copydoc Ui::View::IsOnScene()
   */
  bool IsOnScene() const;

  /// @cond internal
  DALI_INTERNAL void SetAsFocusGroup(bool isFocusGroup);
  DALI_INTERNAL bool IsFocusGroup() const;
  DALI_INTERNAL bool NotifyKeyEvent(const KeyEvent& event);
  DALI_INTERNAL void NotifyFocusChanged(bool focused);
  DALI_INTERNAL void NotifyFocusChangeCommitted(Ui::View committedFocusableView);
  DALI_INTERNAL View RequestFocusNavigation(View currentFocusedView, FocusDirection direction);
  DALI_INTERNAL View RequestFocus();
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
   */
  virtual MeasuredSize OnArrange(const LayoutRect& bounds);

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
   * @brief Called when this view's chosen focusable view will be focused.
   * @param[in] committedFocusableView The committed focusable view
   */
  virtual void OnFocusChangeCommitted(Ui::View committedFocusableView);

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
   * @brief Gets the measure callback, if set.
   * @return Pointer to the MeasureCallback, or nullptr if not set
   */
  MeasureCallback* GetMeasureCallback();

  /**
   * @brief Gets the arrange callback, if set.
   * @return Pointer to the ArrangeCallback, or nullptr if not set
   */
  ArrangeCallback* GetArrangeCallback();

  /**
   * @brief Gets the LayoutManager, if attached.
   *
   * Returned pointer is non-owning. Ownership remains with the View's trait.
   *
   * @return Pointer to the LayoutManager, or nullptr if not attached
   */
  LayoutManager* GetLayoutManager() const;

  /**
   * @brief Resolves focus by iterating visible children, with self fallback.
   *
   * Used by OnFocusRequested for views that have layout capability (Layout
   * subclasses or any View with a LayoutManager attached).
   */
  View RequestChildFirstFocus();

  /**
   * @brief Returns self if focusable, enabled and visible; otherwise empty.
   */
  View DefaultOnFocusRequested();

  // ============================================================
  // private
  // ============================================================

private:
  friend class Internal::ViewDataImpl; ///< Pimpl body

  Internal::ViewDataImpl& GetViewDataImpl() const;
  Ui::Layout              GetParentLayout() const;
  Ui::View                GetParentView() const;
  void                    EmitFocusChangedSignal(bool focusGained);
  MeasuredSize            ApplyConstraints(const MeasuredSize& size) const;
  void                    RegisterWithLayoutController();
  void                    MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight);
  void                    ArrangeStandaloneChildren(const LayoutRect& bounds);
  void                    ApplyLayoutDirection(float parentWidth);
  float                   ComputeEffectiveScale() const;
  MeasuredSize            DispatchMeasureWithLayoutManager(LayoutManager* manager,
                                                           float          widthConstraint,
                                                           float          heightConstraint);
  MeasuredSize            DispatchArrangeWithLayoutManager(LayoutManager* manager, const LayoutRect& bounds);
  MeasuredSize            DispatchArrangeWithCallback(ArrangeCallback* callback, const LayoutRect& bounds);

public:
  /**
   * @brief Recursively resets the effective-scale cache and measure cache for
   *        this view and all its descendants.
   * Called by UiScaleManagerImpl::SetScale() so that the entire subtree
   * re-evaluates its effective scale on the next Measure pass.
   */
  void ResetEffectiveScaleRecursive();

  /**
   * @brief Called when the effective scale changes.
   * Subclasses (Label, InputField) override this to update font size scale.
   * @param[in] newScale The new effective scale factor
   */
  virtual void OnEffectiveScaleChanged(float newScale)
  {
  }

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
  void OnColorTableChanged();
  void ClearGradientColorBinding(StringView bindingId);
  void ClearBackgroundBinding();
  void SetBackgroundColorInternal(const Vector4& color);
  void SetBackgroundGradientInternal(const Gradient::Base& gradient);
  void SetBorderlineColorInternal(const Vector4& color);
  void SetColorInternal(const Vector4& color);
  void OnChildOrderChanged(Actor parent, Actor orderChangedChild);

  // UiScale
  UiScalePolicy mScalePolicy{UiScalePolicy::INHERIT}; // 1 byte
  mutable float mEffectiveScale{-1.0f};               // -1 = uncomputed sentinel

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
