#ifndef DALI_UI_VIEW_DATA_IMPL_H
#define DALI_UI_VIEW_DATA_IMPL_H

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
#include <dali/devel-api/adaptor-framework/accessibility-devel.h> // LCOV_EXCL_LINE
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>  // LCOV_EXCL_LINE
#include <dali/integration-api/processor-interface.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property-notification.h>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/render-effects/offscreen-rendering-impl.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <dali-ui-foundation/public-api/views/view-accessibility-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali/integration-api/debug.h>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class SizeNegotiatedViewImpl;
} // namespace Integration

namespace Internal
{
class InteractiveTraitImpl;
class CoreInteractionObject;

/// @brief Type-level animatable property index for effective UI scale.
/// Defined here (not in the public View::Property enum) to keep it internal.
/// Value matches Dali::Ui::View::ANIMATABLE_PROPERTY_START_INDEX + 500,
/// @warning Please change this value if view.h add new enum as Dali::Ui::View::ANIMATABLE_PROPERTY_START_INDEX + 500
inline constexpr Property::Index VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX = View::ANIMATABLE_PROPERTY_START_INDEX + 500;

class AttachmentContainer;

namespace Visual
{
class Base;
}

enum class TriStateProperty
{
  AUTO = 0,
  TRUE,
  FALSE
};

/**
 * @brief Layout-transition changes accumulated since the preceding layout pass.
 *
 * The layout transition dispatcher consumes this state once per pass to
 * determine ENTER and CHANGE causes.
 */
struct PendingLayoutTransitionChanges
{
  std::unordered_set<ViewImpl*> enterChildren;
  std::unordered_set<ViewImpl*> reorderedChildren;
  bool                          hadChildRemoval{false};
};

/**
 * @brief Holds the Implementation for the internal view class
 */
class ViewDataImpl : public ConnectionTracker, public Dali::Integration::Processor
{
private:
  friend class ::Dali::Ui::ViewImpl;
  friend class ::Dali::Ui::Integration::SizeNegotiatedViewImpl;
  friend std::string DumpView(const ::Dali::Ui::ViewImpl& view);

  class AccessibilityData;
  class VisualData;

public:
  using AccessibleObjectCreator = ViewAccessible* (*)(Dali::Ui::View);

  /**
   * @brief Retrieves the implementation of the internal view class.
   * @param[in] viewImpl A ref to the view whose internal implementation is required
   * @return The internal implementation
   */
  static ViewDataImpl& Get(ViewImpl& viewImpl);

  static const ViewDataImpl& Get(const ViewImpl& viewImpl);

  /**
   * @brief Constructor.
   * @param[in] viewImpl The view which owns this implementation
   */
  ViewDataImpl(ViewImpl& viewImpl);

  /**
   * @brief Destructor.
   */
  ~ViewDataImpl();

  bool AreVisualsEnabled() const;

  MeasuredSize Measure(float visualWidth, float visualHeight);
  LayoutRect   Arrange(const LayoutRect& bounds);

  /**
   * @brief Arranges this view as a root driven by LayoutController.
   *
   * Kept separate from Arrange() so the arrange cache can distinguish the
   * framework-owned self pass of a STANDALONE boundary from an application calling
   * the public View::Arrange() with arbitrary bounds. The two calls share the same
   * implementation; only the direct-parent cache ownership decision differs.
   *
   * @param[in] bounds The bounds derived by LayoutController::ProcessLayoutRoot
   * @return The final arranged bounds
   */
  LayoutRect ArrangeAsLayoutRoot(const LayoutRect& bounds);

  const ViewState&                            GetState() const;
  bool                                        IsEffectivelyFocused() const;
  View::LayoutFinishedSignalType&             LayoutFinishedSignal();
  View::StateChangedSignalType&               StateChangedSignal();
  View::ResourceReadySignalType&              ResourceReadySignal();
  View::OffScreenRenderingFinishedSignalType& OffScreenRenderingFinishedSignal();
  bool                                        HasLayoutFinishedSignalConnections() const;
  void                                        EmitLayoutFinishedSignal(const LayoutRect& bounds);
  PendingLayoutTransitionChanges              TakePendingLayoutTransitionChanges();

  InteractiveTrait     EnsureInteractiveTrait();
  void                 SetStateEffect(StateEffect effect);
  void                 AttachInteractiveStateEffect();
  bool                 IsDefaultFocusIndicatorSuppressedByStateEffect() const;
  void                 RefreshDefaultFocusIndicatorSuppression();
  void                 InvalidateDefaultFocusIndicatorSuppression(const Integration::StateEffectImpl& effect);
  void                 SetStateEffectTarget(View target);
  View                 GetStateEffectTarget() const;
  bool                 IsInteractive() const;
  SelectableTrait      EnsureSelectableTrait();
  bool                 IsSelectable() const;
  GroupSelectableTrait EnsureGroupSelectableTrait();
  bool                 IsGroupSelectable() const;

  UiColor            GetBackgroundColor() const;
  void               SetBackgroundColor(const UiColor& color);
  void               SetBackgroundImage(const Dali::String& url);
  void               SetBackgroundGradient(const Gradient::Base& gradient);
  UiColor            GetColor() const;
  void               SetColor(const UiColor& color);
  UiColor            GetCurrentColor() const;
  Vector4            GetCornerRadius() const;
  void               SetCornerRadius(const Vector4& radius);
  CornerRadiusPolicy GetCornerRadiusPolicy() const;
  void               SetCornerRadiusPolicy(CornerRadiusPolicy policy);
  Vector4            GetCornerSquareness() const;
  void               SetCornerSquareness(const Vector4& squareness);
  float              GetBorderlineWidth() const;
  void               SetBorderlineWidth(float width);
  UiColor            GetBorderlineColor() const;
  void               SetBorderlineColor(const UiColor& color);
  float              GetBorderlineOffset() const;
  void               SetBorderlineOffset(float offset);
  void               ClearBackground();
  void               SetShadow(const Shadow& shadow);
  void               SetShadow(const ShadowStack& shadowStack);

  void                          SetFocusNavigationCallback(FocusNavigationCallback callback);
  FocusNavigationResult         RequestFocusNavigation(View currentFocusedView, FocusNavigationContext context);
  View                          RequestFocus();
  bool                          IsFocusGroup() const;
  void                          SetAsFocusGroup(bool isFocusGroup);
  View::KeyEventSignalType&     KeyEventSignal();
  View::FocusChangedSignalType& FocusChangedSignal();
  bool                          NotifyKeyEvent(const KeyEvent& event);

  void          SetRequestedX(float x);
  void          SetRequestedY(float y);
  float         GetRequestedX() const;
  float         GetRequestedY() const;
  void          SetUiScalePolicy(UiScalePolicy policy);
  UiScalePolicy GetUiScalePolicy() const;
  float         GetEffectiveScale() const;
  /**
   * @brief Records and propagates a measure invalidation under the uniform layout
   * processing policy.
   *
   * Every origin -- public API, property setter, tree mutation, resource callback, or
   * framework walk -- performs the full local invalidation, ancestor propagation and
   * layout-root registration. The target controller decides only whether that pending
   * registration may arm an idle wake: work raised during layout processing is parked,
   * while an out-of-processing request arms one coalesced outstanding wake.
   */
  void InvalidateMeasure();

  /**
   * @brief InvalidateMeasure() for the one caller that already holds the parent.
   *
   * Identical in every effect to InvalidateMeasure() -- same local invalidation, same
   * generation short-circuit, same standalone boundary, same propagation -- but the
   * parent's data is passed in instead of being re-derived from the actor tree. Valid only
   * where @p parentData IS this view's parent when the call is made: OnChildAdded, which
   * dali-core runs after it has already parented the child.
   *
   * A re-entrant reparent inside OnChildAdded -- NotifyChildReparented runs application
   * code, before the subtree cache reset -- is tolerated rather than mis-propagated. The
   * nested add invalidates the live chain through its own call to this entry; the outer
   * call then walks the captured @p parentData, which is at worst a conservative extra
   * invalidation of the OLD chain -- every step an idempotent cache drop plus a coalesced
   * registration, with the generation stamp collapsing any overlap where the two chains
   * meet. An invalidation can be repeated harmlessly; it can never be missed.
   *
   * @param[in] parentData The layout data of this view's parent
   */
  void InvalidateMeasureFromParentAdd(ViewDataImpl& parentData);

  /**
   * @brief The arrange-axis counterpart of InvalidateMeasure().
   */
  void InvalidateArrange();

  /**
   * @brief The public-API entry point for measure invalidation and its diagnostic.
   *
   * ViewImpl::InvalidateMeasure() calls this wrapper so a direct application call made
   * during layout processing can be diagnosed. It must still perform exactly the same
   * full invalidation, propagation and root registration as InvalidateMeasure(). The
   * public/internal route is not a scheduling-policy boundary: only the target
   * controller's processing context decides whether the pending work arms an idle wake
   * or remains parked.
   */
  void InvalidateMeasureFromPublicApi();

  /**
   * @brief The public-API entry point for arrange invalidation and its diagnostic.
   *
   * The arrange-axis twin of InvalidateMeasureFromPublicApi().
   */
  void InvalidateArrangeFromPublicApi();

  /// @copydoc ViewDataImpl::InvalidateMeasureFromPublicApi()
  /// @param[in] apiName The public entry point to name in the in-pass diagnostic, for
  /// callers that are not ViewImpl::InvalidateMeasure(). LayoutManager's owner-invalidation
  /// helpers are such a caller: they are a public API in their own right, and reporting
  /// them as "View::InvalidateMeasure" sends the reader to the wrong call site.
  void InvalidateMeasureFromPublicApi(const char* apiName);

  /// @copydoc ViewDataImpl::InvalidateArrangeFromPublicApi()
  /// @param[in] apiName The public entry point to name in the in-pass diagnostic.
  void InvalidateArrangeFromPublicApi(const char* apiName);

  /**
   * @brief Re-arms the dirty bits of a layout pass that was abandoned by an exception.
   *
   * MeasurePassGuard / ArrangePassGuard CONSUME mMeasureDirty / mArrangeDirty at ENTRY,
   * because a pass that runs to completion is what services them. A pass abandoned
   * mid-flight serviced nothing, so the record has to come back: the view would otherwise
   * be "clean but never laid out", and the one consumer that reads the bits outside a pass
   * -- OnViewSceneConnection's standalone isDirty self-registration -- would decline to
   * re-register a reconnecting standalone root that genuinely has work pending.
   *
   * Deliberately NOT InvalidateMeasure(): that walks the ancestor chain and registers at
   * whatever root it reaches (not necessarily the one being restored), and it asks the
   * controller for an idle wake. The only caller is LayoutController's batch rollback,
   * which re-registers THIS root by hand and must not wake the loop for a pass that has
   * just thrown.
   *
   * The cache-valid bits are left alone on purpose: the abandoned pass already cleared
   * them at entry, and that is exactly the state that forces the retry to recompute.
   */
  void RearmLayoutDirtyForAbortedPass();

  /**
   * @brief Logs, once for this view, that @p apiName was called from inside layout processing.
   *
   * Public because LayoutController::RequestLayout() reaches it through
   * ViewDataImpl::Get(); it is a diagnostic helper, not part of any application-facing
   * surface (this whole class is internal). The warning reports that the work was
   * parked; it must never imply that the invalidation was ignored or discarded.
   *
   * @param[in] apiName The violating entry point, fully qualified as it should appear
   * in the log, e.g. "View::InvalidateMeasure" or "LayoutController::RequestLayout"
   */
  void LogInPassInvalidation(const char* apiName);

  /**
   * @brief Returns whether any view's Measure()/Arrange() pass is on this thread's stack.
   *
   * The pass half of the layout processing window; the emit half is
   * LayoutInvalidation::IsLayoutFinishedEmitInProgress(). Exposed so controller
   * registration can apply the same retained-but-parked wake policy to every origin.
   *
   * @return True while a Measure or Arrange pass is running
   */
  static bool IsLayoutPassOnStack();

  MeasuredSize     GetMeasuredSize() const;
  void             SetRequestedWidth(float width);
  float            GetRequestedWidth() const;
  void             SetRequestedHeight(float height);
  float            GetRequestedHeight() const;
  void             SetMinimumWidth(float width);
  float            GetMinimumWidth() const;
  void             SetMinimumHeight(float height);
  float            GetMinimumHeight() const;
  void             SetMaximumWidth(float width);
  float            GetMaximumWidth() const;
  void             SetMaximumHeight(float height);
  float            GetMaximumHeight() const;
  void             SetMargin(const Insets& margin);
  Insets           GetMargin() const;
  void             SetPadding(const Insets& padding);
  Insets           GetPadding() const;
  void             SetLayoutMode(LayoutMode mode);
  LayoutMode       GetLayoutMode() const;
  void             SetLayoutTransition(LayoutTransition transition);
  LayoutTransition GetLayoutTransition() const;
  LayoutRect       GetArrangedBounds() const;
  bool             HasArrangeResult() const;
  bool             IsInitialLayoutDone() const;
  /// One-shot latch for the layout transition dispatcher's fresh-child ENTER settle.
  /// A child that no producer ever arranges keeps its "fresh" classification forever,
  /// so the settle branch is re-entered on every pass; the settle itself is needed
  /// exactly once, and this records that it has happened. Cleared on reparent
  /// (OnChildAdded), because that is the only way the governing transition -- and
  /// hence the ENTER spec that was baked -- can change.
  bool                      IsInitialEnterSettled() const;
  void                      MarkInitialEnterSettled();
  uint32_t                  GetChildViewCount() const;
  View                      GetChildViewAt(uint32_t index) const;
  Dali::Vector<View>&       GetChildren();
  const Dali::Vector<View>& GetChildren() const;
  int32_t                   IndexOfChildView(View view) const;
  void                      Remove(View child, RemovePolicy policy);
  void                      RemoveAll(RemovePolicy policy);
  void                      Raise(LayoutOrderPolicy policy);
  void                      Lower(LayoutOrderPolicy policy);
  void                      RaiseToTop(LayoutOrderPolicy policy);
  void                      LowerToBottom(LayoutOrderPolicy policy);
  void                      RaiseAbove(View target, LayoutOrderPolicy policy);
  void                      LowerBelow(View target, LayoutOrderPolicy policy);

  void SetMeasureCallback(MeasureCallback callback);
  void SetArrangeCallback(ArrangeCallback callback);
  void SetArrangeCallback(ArrangeCallback callback, ArrangePolicy policy);
  /// Sets this view's OnArrange execution policy. Callable from a constructor
  /// before the CustomActor handle exists because it only invalidates an existing
  /// published cache entry.
  void SetArrangePolicy(ArrangePolicy policy);
  /// Returns this view's OnArrange execution policy -- the value SetArrangePolicy set,
  /// or ArrangePolicy::IF_CHANGED by default. Reflects OnArrange() only and is
  /// independent of any attached LayoutManager or ArrangeCallback.
  ArrangePolicy GetArrangePolicy() const;
  /// Re-derives the active producer policy after the attached LayoutManager changes
  /// its policy, then invalidates the previous arrange result.
  void             OnLayoutManagerArrangePolicyChanged();
  MeasureCallback* GetMeasureCallback();
  ArrangeCallback* GetArrangeCallback();
  void             AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager);
  LayoutManager*   GetLayoutManager() const;
  bool             HasLayoutManager() const;
  bool             HasLayoutCallback() const;

  void          SetLayoutParams(const AbsoluteLayoutParams& params);
  void          SetLayoutParams(const FlexLayoutParams& params);
  void          SetLayoutParams(const GridLayoutParams& params);
  void          SetLayoutParams(const StackLayoutParams& params);
  bool          TryGetLayoutParams(AbsoluteLayoutParams& params) const;
  bool          TryGetLayoutParams(FlexLayoutParams& params) const;
  bool          TryGetLayoutParams(GridLayoutParams& params) const;
  bool          TryGetLayoutParams(StackLayoutParams& params) const;
  void          GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward);
  Dali::Texture GetOffScreenRenderingOutput() const;
  /// Natural size of the background visual plus padding, or ZERO when the view has
  /// no background visual. Not an override and never virtually dispatched;
  /// Actor::GetNaturalSize() reaches it only through SizeNegotiatedViewImpl.
  Vector3      GetBackgroundVisualNaturalSize();
  void         SetRenderEffect(RenderEffect effect);
  RenderEffect GetRenderEffect() const;
  void         ClearRenderEffect();

  /**
   * @brief Drops the cached effective scale and the layout caches of this view and of
   * every descendant.
   *
   * Used by the paths that can move the effective scale of a whole subtree at
   * once -- a UiScalePolicy change, a global UI scale change, and a reparent --
   * where the change re-roots the INHERIT chain, so every descendant's cached
   * scale (and therefore every cached measure/arrange result derived from it)
   * is potentially stale.
   *
   * It is exactly DropCachedEffectiveScale() + InvalidateLayoutCaches() applied
   * to the subtree, plus a retraction of the invalidation propagation records: it
   * raises NO dirty bit and registers nothing with the LayoutController. The callers
   * follow it with InvalidateMeasure(), which propagates upward and enqueues the layout
   * root. Whether that pending work arms an idle wake depends on whether layout
   * processing is active.
   */
  void  ResetSubtreeScaleAndLayoutCaches();
  float ComputeEffectiveScale() const;

  /// @name Layout cache-state accessors (white-box test support)
  /// Plain one-line readers of the layout bookkeeping bits. They exist so the
  /// internal UTC target can assert on cache state directly instead of inferring
  /// it from geometry; this is an internal, non-exported header, so they add no
  /// public API and no ABI surface. Nothing in the library reads them.
  /// @{
  bool IsMeasureCacheValid() const
  {
    return mMeasureCacheValid;
  }
  /// The effective scale the measure cache entry was produced at, and a KEY term of
  /// the measure hit predicate. Meaningful only while IsMeasureCacheValid(): the slot
  /// is shared with the measure propagation record (union).
  float GetLastMeasureScale() const
  {
    return mMeasureKeyOrPropagation.scaleKey;
  }
  bool IsArrangeCacheValid() const
  {
    return mArrangeCacheValid;
  }
  Dali::LayoutDirection::Type GetLastArrangeDirection() const
  {
    return mLastArrangeDirection;
  }
  bool IsMeasureDirty() const
  {
    return mMeasureDirty;
  }
  bool IsArrangeDirty() const
  {
    return mArrangeDirty;
  }
  bool IsEffectiveScaleValid() const
  {
    return mEffectiveScaleValid;
  }
  /// The ACTOR-side half of the scale sync pair: true when the animatable
  /// VIEW_EFFECTIVE_SCALE property is known to hold mEffectiveScale, which is what
  /// lets Measure() skip reading that property on a cache hit.
  bool IsEffectiveScaleActorSynced() const
  {
    return mEffectiveScaleActorSynced;
  }
  /// Returns the active producer's derived execution policy.
  bool ArrangesIfChanged() const
  {
    return !mArrangeProducerAlways;
  }
  /// The generation each axis last propagated its invalidation to a layout root in.
  /// Compared against LayoutInvalidation::CurrentGeneration() to decide whether a further
  /// invalidation may skip the ancestor walk; 0 means never propagated. Meaningful only
  /// while the axis's cache entry is absent (Is*CacheValid() false): the slot is shared
  /// with the axis's cache KEY (union), so publishing an entry overwrites the record and
  /// dropping a standing entry retracts it to 0.
  uint32_t GetMeasurePropagationGeneration() const
  {
    return mMeasureKeyOrPropagation.propagationGeneration;
  }
  uint32_t GetArrangePropagationGeneration() const
  {
    return mArrangeKeyOrPropagation.propagationGeneration;
  }
  /// True once this view has connected the actor layout-direction signal, which only a
  /// LAYOUT ROOT ever does (see RegisterWithLayoutController). A plain child View stays
  /// false and is covered by OnPropertySet plus its root's subtree walk instead.
  bool IsLayoutDirectionSignalConnected() const
  {
    return mLayoutDirectionSignalConnected;
  }
  /// True once this view has logged an in-pass Invalidate*() contract violation; never
  /// cleared. The latch is what makes LogInPassInvalidation() diagnose a defective call
  /// site once instead of every frame, and the log line itself is not observable from a
  /// test (DALI_LOG_ERROR goes to stderr, which the tct harness does not capture), so
  /// this reader is the only way to pin that the guard fired at all.
  bool HasWarnedInPassInvalidation() const
  {
    return mInPassInvalidationWarned;
  }
  /// @}

  View GetParentView() const;
  void EmitFocusChangedSignal(bool focusGained);
  void RegisterWithLayoutController();

  /// THE implementation of a RequestedWidth / RequestedHeight write. Validation,
  /// sentinel snap, change guard, invalidation and the parentless-leaf immediate apply
  /// all live here and nowhere else: the registered property setter (the scripting /
  /// JSON route) and the C++ setter both call in, so the two can never drift.
  /// @{
  void ApplyRequestedWidth(float width);
  void ApplyRequestedHeight(float height);
  /// @}

  /// Replays the two OBSERVABLE tails dali-core runs after a registered property's set
  /// function, for a C++ setter that wrote the member directly instead of going through
  /// Handle::SetProperty. See the definition for why each tail is mandatory.
  ///
  /// @p self is the strong handle the CALLER already made to pin this object for the
  /// whole write. It is an INPUT, not a convenience: dali-core's Object::SetProperty
  /// pins at function entry and emits with that same handle, and both tails below can
  /// re-enter application code that releases the last external reference. Taking the
  /// caller's handle keeps the pin and the emit argument the same object and costs no
  /// second Self() round trip.
  void NotifyPropertySet(Dali::Handle& self, Property::Index index, const Property::Value& value);

  bool UpdateColorBindingInternal(StringView bindingId, const UiColor& color);
  void SetColorBindingInternal(StringView bindingId, const UiColor& color, ColorCallback callback);
  bool UpdateColorBindingInternal(StringView bindingId, const Gradient::Base& gradient);
  void SetColorBindingInternal(StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback);
  void ClearGradientColorBinding(StringView bindingId);
  void ClearBackgroundBinding();
  void SetBackgroundColorInternal(const Vector4& color);
  void SetBackgroundGradientInternal(const Gradient::Base& gradient);
  void SetBorderlineColorInternal(const Vector4& color);
  void SetColorInternal(const Vector4& color);

  /**
   * @brief Returns this impl's private VisualData context, creating it on first use.
   *
   * The context is allocated lazily, so a view that never touches a visual never pays for
   * one. Every caller must gate on AreVisualsEnabled() first: a DISABLE_VISUALS view must
   * never reach this function, because reaching it would allocate the very context that
   * flag exists to suppress.
   *
   * @return The VisualData context of this impl
   */
  VisualData& EnsureVisualData();

  // Trait management (delegated from ViewImpl)

  /**
   * @brief Notifies all traits that the owning View is being destroyed.
   *
   * Must be called while ViewImpl members are still valid (i.e. inside ViewImpl::~ViewImpl body,
   * before `delete mImpl`).
   */
  void NotifyTraitsViewDestroying();

  /**
   * @brief Sets a trait data to the owning View.
   *
   * Lifecycle callbacks (OnAttached, OnDetaching, OnViewDestroying) are invoked
   * automatically.
   *
   * @warning Do not store Actor-derived objects as trait data. Actors are owned by
   * the scene graph and have their own parent-child lifecycle. Storing them here
   * causes ownership conflicts and potential dangling references.
   *
   * @param[in] id The key to identify the trait
   * @param[in] object The object to store
   */
  void SetTrait(TraitId id, IntrusivePtr<TraitObject> object);

  /**
   * @brief Gets a trait data from the owning View.
   *
   * @param[in] id The key to identify the trait
   * @return The stored object, or nullptr if not found
   */
  IntrusivePtr<TraitObject> GetTrait(TraitId id) const;

  /**
   * @brief Removes a trait from the owning View.
   */
  bool RemoveTrait(TraitId id);

  /**
   * @brief Sets an attachment to the owning View.
   *
   * Replaces the existing attachment when @p id is already present.
   */
  void SetAttachment(AttachmentId id, UniqueAny attachment);

  /**
   * @brief Removes an attachment from the owning View.
   *
   * @return True if an attachment was removed
   */
  bool RemoveAttachment(AttachmentId id);

  /**
   * @brief Detaches an attachment from the owning View.
   *
   * @return The stored attachment, or an empty UniqueAny if missing
   */
  UniqueAny DetachAttachment(AttachmentId id);

  /**
   * @brief Gets a raw attachment from the owning View.
   *
   * @return Pointer to the attachment, or nullptr if missing
   */
  UniqueAny* GetAttachment(AttachmentId id);

  /**
   * @brief Gets a const raw attachment from the owning View.
   *
   * @return Pointer to the attachment, or nullptr if missing
   */
  const UniqueAny* GetAttachment(AttachmentId id) const;

  // State management

  /**
   * @brief Updates a state bit and notifies ViewStateManager if the state changed.
   * @param[in] stateToChange The state to set or clear
   * @param[in] on            True to add the state, false to remove it
   * @param[in] cause         Input event that triggered the change
   */
  void SetState(ViewState stateToChange, bool on, InputEvent cause);

  /**
   * @brief Clears and sets states as a single state change notification.
   * @param[in] statesToClear The states to clear
   * @param[in] statesToSet   The states to set
   * @param[in] cause         Input event that triggered the change
   */
  void SetState(ViewState statesToClear, ViewState statesToSet, InputEvent cause);

  /**
   * @brief Registers a named state observer.
   * @param[in] id       Unique identifier for this observer
   * @param[in] tracker  ConnectionTrackerInterface for lifetime management
   * @param[in] callback Callback with signature void(View, const StateEvent&)
   */
  void SetNamedStateObserver(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback);

  /**
   * @brief Removes a named state observer.
   * @param[in] id The observer identifier to remove
   * @return True if an observer was found and removed
   */
  bool UnsetNamedStateObserver(const Dali::String& id);

  /**
   * @brief Removes a named state observer unless its callback is currently executing.
   * @param[in] id The observer identifier to remove
   * @return True if removed; false if currently executing or not found
   */
  bool UnsetNamedStateObserverIfNotExecuting(const Dali::String& id);

  /**
   * @brief Returns the core interaction trait object pointer (may be null).
   */
  Internal::CoreInteractionObject* GetCoreInteractionObject() const;

  /**
   * @brief Called when resources of view are ready.
   */
  void ResourceReady();

  void RegisterVisual(Property::Index index, Integration::Visual::Base& visual);

  void RegisterVisual(Property::Index index, Integration::Visual::Base& visual, int depthIndex);

  void RegisterVisual(Property::Index index, Integration::Visual::Base& visual, bool enabled);

  void RegisterVisual(Property::Index index, Integration::Visual::Base& visual, bool enabled, int depthIndex);

  void UnregisterVisual(Property::Index index);

  Integration::Visual::Base GetVisual(Property::Index index) const;

  /**
   * @brief Get the raw pointer of visual impl.
   * It will be used when we want to get visual infomations without increase reference counts.
   * @note Only for internal usage.
   *
   * @param[in] index Index of parameter
   * @return Raw pointer of visual base implements. nullptr if not exist.
   */
  Visual::Base* GetVisualImplPtr(Property::Index index) const;

  /**
   * @brief Sets the background visual from a property map.
   * @param[in] map The background visual property map
   */
  void SetBackground(const Property::Map& map);

  /**
   * @brief Enables or disables overriding the given visual's corner properties to its view's
   * @param[in] visual A registered visual
   * @param[in] enable flat to set enabled or disabled.
   * @param[in] cornerRadiusConstraint Optional constraint to link the view's corner properties to the visual's.
   */
  void EnableCornerPropertiesOverridden(Integration::Visual::Base& visual, bool enable,
                                        Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

  void EnableVisual(Property::Index index, bool enable);

  bool IsVisualEnabled(Property::Index index) const;

  Ui::Visual::ResourceStatus GetVisualResourceStatus(Property::Index index) const;

  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                const Dali::Property::Value& attributes);

  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                         const Dali::Any& attributes);

  bool AddVisualObject(VisualBase visualBase, Integration::Visual::InternalContainerRangeType internalContainerRangeType);

  /**
   * @brief Adds a shadow visual object.
   * @param[in] visualBase The shadow visual to add
   * @param[in] internalContainerRangeType The range of visuals to be added
   * @return True if the visual was added successfully, false otherwise
   */
  bool AddShadowVisualObject(VisualBase visualBase, Integration::Visual::InternalContainerRangeType internalContainerRangeType);

  void RemoveVisualObject(VisualBase visualBase);

  uint32_t GetVisualObjectCount(Integration::Visual::InternalContainerRangeType internalContainerRangeType) const;

  VisualBase GetVisualObjectAt(Integration::Visual::InternalContainerRangeType internalContainerRangeType, uint32_t siblingOrder) const;

  /**
   * @brief Function used to set view properties.
   * @param[in] object The object whose property to set
   * @param[in] index The index of the property to set
   * @param[in] value The value of the property to set
   */
  static void SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);

  /**
   * @brief Function used to retrieve the value of view properties.
   * @param[in] object The object whose property to get
   * @param[in] index The index of the property to get
   * @return The value of the property
   */
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

  /**
   * @brief Whether the resource is ready
   * @return True if the resource is read.
   */
  bool IsResourceReady() const;

  void OnSceneConnection();

  void OnSceneDisconnection();

  /**
   * @brief Get private AccessibilityData context for this impl. If not created yet, it will create new data.
   * @return The l-value of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData& GetOrCreateAccessibilityData();

  /**
   * @brief Get private AccessibilityData context for this impl.
   * @return The pointer of AccessibilityData context.
   */
  [[nodiscard]] AccessibilityData* GetAccessibilityData() const;

  void SetAccessibilityActivateCallback(Callback<bool(View)> callback);
  bool DispatchAccessibilityActivate();

  void SetAccessibilityEscapeCallback(Callback<bool(View)> callback);
  bool DispatchAccessibilityEscape();

  void SetAccessibilityPanCallback(Callback<bool(View, PanGesture)> callback);
  bool DispatchAccessibilityPan(PanGesture gesture);

  void SetAccessibilityValueChangeCallback(Callback<bool(View, bool)> callback);
  bool DispatchAccessibilityValueChange(bool isIncreased);

  void SetAccessibilityScrollToChildCallback(Callback<bool(View, View)> callback);
  bool DispatchAccessibilityScrollToChild(View child);

  void SetAccessibilityZoomCallback(Callback<bool(View)> callback);
  bool DispatchAccessibilityZoom();

  void SetAccessibilityRequestNameCallback(Callback<bool(View, Dali::String&)> callback);
  bool DispatchAccessibilityRequestName(Dali::String& value);

  void SetAccessibilityRequestDefaultNameCallback(Callback<bool(View, Dali::String&)> callback);
  bool DispatchAccessibilityRequestDefaultName(Dali::String& value);

  void SetAccessibilityRequestDescriptionCallback(Callback<bool(View, Dali::String&)> callback);
  bool DispatchAccessibilityRequestDescription(Dali::String& value);

  void SetAccessibilityRequestDefaultDescriptionCallback(Callback<bool(View, Dali::String&)> callback);
  bool DispatchAccessibilityRequestDefaultDescription(Dali::String& value);

  void SetAccessibilityRequestValueCallback(Callback<bool(View, Dali::String&)> callback);
  bool DispatchAccessibilityRequestValue(Dali::String& value);

  void                SetAccessibilityName(StringView name);
  Dali::String        GetAccessibilityName() const;
  void                SetAccessibilityDescription(StringView description);
  Dali::String        GetAccessibilityDescription() const;
  void                SetAccessibilityValue(StringView value);
  Dali::String        GetAccessibilityValue() const;
  void                SetAccessibilityRole(Accessibility::Role role);
  Accessibility::Role GetAccessibilityRole() const;
  void                SetAccessibilityHidden(bool hidden);
  bool                IsAccessibilityHidden() const;
  void                SetAccessibilityHighlightable(bool highlightable);
  void                ResetAccessibilityHighlightable();
  bool                IsAccessibilityHighlightable() const;
  void                SetAccessibilityScrollable(bool scrollable);
  bool                IsAccessibilityScrollable() const;
  void                SetAccessibilityModal(bool modal);
  bool                IsAccessibilityModal() const;
  void                SetAutomationId(StringView automationId);
  Dali::String        GetAutomationId() const;

  void         SetTranslatableAccessibilityName(StringView resourceId, StringView domain);
  Dali::String GetTranslatableAccessibilityName() const;
  void         ClearTranslatableAccessibilityName();
  void         SetTranslatableAccessibilityDescription(StringView resourceId, StringView domain);
  Dali::String GetTranslatableAccessibilityDescription() const;
  void         ClearTranslatableAccessibilityDescription();

  void AddAccessibilityRelation(Accessibility::RelationType type, View target);
  void RemoveAccessibilityRelation(Accessibility::RelationType type, View target);
  void ClearAccessibilityRelations();
  bool HasAccessibilityRelation(Accessibility::RelationType type, View target) const;

  void AddAccessibilityReadingInfo(Accessibility::ReadingInfo info);
  void RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo info);
  void ClearAccessibilityReadingInfo();
  bool HasAccessibilityReadingInfo(Accessibility::ReadingInfo info) const;

  bool AddAccessibilityNameLanguageSpan(uint32_t start, uint32_t length, StringView locale);
  void ClearAccessibilityNameLanguageSpans();
  bool AddAccessibilityDescriptionLanguageSpan(uint32_t start, uint32_t length, StringView locale);
  void ClearAccessibilityDescriptionLanguageSpans();

  void    SetRequestInitialAccessibilityHighlight(bool request);
  bool    IsInitialAccessibilityHighlightRequested() const;
  void    SetAccessibilityCollectionContainer(bool container);
  bool    IsAccessibilityCollectionContainer() const;
  void    SetAccessibilityCollectionIndex(int32_t index);
  int32_t GetAccessibilityCollectionIndex() const;
  void    ClearAccessibilityCollectionIndex();

  View::AccessibilityReadingStatusChangedSignalType& AccessibilityReadingStatusChangedSignal();
  View::AccessibilityHighlightedSignalType&          AccessibilityHighlightedSignal();

  /**
   * @brief Adds accessibility attribute
   * @param[in] key Attribute name to set
   * @param[in] value Attribute value to set
   *
   * Attribute is added if not existed previously or updated
   * if existed.
   */
  void AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value);

  /**
   * @brief Removes accessibility attribute
   * @param[in] key Attribute name to remove
   *
   * Function does nothing if attribute doesn't exist.
   */
  void RemoveAccessibilityAttribute(const Dali::String& key);

  /**
   * @brief Removes all accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief Sets reading info type attributes
   * @param[in] types info type attributes to set
   *
   * This function sets, which part of object will be read out
   * by screen-reader.
   */
  void SetAccessibilityReadingInfoType(const Dali::Integration::Accessibility::ReadingInfoTypes types); // LCOV_EXCL_LINE

  /**
   * @brief Gets currently active reading info type attributes
   */
  Dali::Integration::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType() const; // LCOV_EXCL_LINE

  View::VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Replaces all shadows with a single shadow described by a property map.
   *
   * This is the View::Property::SHADOW setter path. It clears both the first
   * shadow and any additional shadows, then installs @p map as the first shadow.
   *
   * @param[in] map The shadow property map
   */
  void SetShadow(const Property::Map& map);

  /**
   * @brief Sets only the first shadow visual.
   *
   * The first shadow is registered as View::Property::SHADOW so property lookup
   * and typed shadow animations can target it directly.
   *
   * @param[in] map The shadow property map
   */
  void SetFirstShadow(const Property::Map& map);

  /**
   * @brief Appends a shadow value to the shadow stack.
   *
   * The first appended shadow is installed through SetFirstShadow() so it keeps
   * the View::Property::SHADOW identity used by property lookup and typed
   * shadow animations. Later shadows are appended as container visuals.
   *
   * @param[in] shadow The shadow value to append
   */
  void AppendShadow(const Shadow& shadow);

  /**
   * @brief Clears the first shadow and all additional shadow visuals.
   */
  void ClearShadow();

  /**
   * @brief Sets the inner shadow with a property map.
   * @param[in] map The inner shadow property map
   */
  void SetInnerShadow(const Property::Map& map);

  /**
   * @brief Sets the inner shadow with an InnerShadow value.
   * @param[in] innerShadow The inner shadow value
   */
  void SetInnerShadow(const Ui::InnerShadow& innerShadow);

  /**
   * @brief Clear the inner shadow.
   */
  void ClearInnerShadow();

  /**
   * @brief Registers an inner shadow visual and connects its corner radius.
   * @param[in] visual The inner shadow visual
   */
  void RegisterInnerShadowVisual(Ui::Integration::Visual::Base visual);

  /**
   * @brief Sets the borderline with a property map.
   * @param[in] map The borderline property map
   * @param[in] forciblyCreate Create new visual forcibly, False if we only need to update properties.
   */
  void SetBorderline(const Property::Map& map, bool forciblyCreate);

  /**
   * @brief Clear the borderline.
   */
  void ClearBorderline();

  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

  /**
   * @brief Create constraints to animate animatable properties.
   * @param[in] animationObject BaseObject of Animation or Constraint
   * @param[in] index The animatable property
   */
  void CreateAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index);

  /**
   * @brief Clear animatable constraints
   * @param[in] animationObject BaseObject of Animation or Constraint
   * @param[in] index The animatable property
   */
  void ClearAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index);

  SharedPtr<ViewAccessible> GetAccessibleObject();

  Dali::Vector<Dali::Devel::Accessibility::Relation> GetAccessibilityRelations(); // LCOV_EXCL_LINE

  /**
   * @brief Sets the accessibility states.
   * @param[in] states The accessibility state mask
   */
  void SetAccessibilityStates(uint32_t states);

  /**
   * @brief Gets the accessibility states.
   * @return The accessibility state mask
   */
  uint32_t GetAccessibilityStates() const;

  /**
   * @brief Adds the accessibility state.
   * @param[in] state The state to add
   */
  void AddAccessibilityState(Accessibility::State state);

  /**
   * @brief Removes the accessibility state.
   * @param[in] state The state to remove
   */
  void RemoveAccessibilityState(Accessibility::State state);

  /**
   * @brief Clears all accessibility states.
   */
  void ClearAccessibilityStates();

  /**
   * @brief Notifies accessibility clients that the active descendant changed.
   *
   * The @p descendant should be a child View of this View. While a deeper
   * descendant (e.g. a grandchild) is also accepted, a direct child is
   * recommended so that assistive technologies can correctly identify the
   * relationship.
   *
   * @param[in] descendant The active descendant View, which should be a child
   *                       of this View. An empty handle is allowed and clears
   *                       the current active descendant.
   */
  void NotifyAccessibilityActiveDescendantChanged(View descendant);

  /**
   * @brief Returns whether the accessibility state is set.
   * @param[in] state The state to query
   * @return True if the state is set
   */
  bool HasAccessibilityState(Accessibility::State state) const;

  bool IsAccessibleCreated() const;

  void EnableCreateAccessible(bool enable);

  bool IsCreateAccessibleEnabled() const;

  /**
   * @brief Sets the integration creator used for a custom Accessible object.
   */
  void SetAccessibleObjectCreator(AccessibleObjectCreator creator);

  /**
   * @brief Creates the configured Accessible object or the default one.
   */
  ViewAccessible* CreateAccessibleObject();

  void EmitAccessibilityStateChanged(Dali::Integration::Accessibility::State state, int newValue); // LCOV_EXCL_LINE

  /**
   * @brief Apply fittingMode
   *
   * @param[in] size The size of the view
   * @param[in] isLayoutFinishedUpdate Whether fitting mode is updated after layout has finished
   */
  void ApplyFittingMode(const Vector2& size, bool isLayoutFinishedUpdate = false);

  /**
   * @brief Ensures this view listens to its layout-finished signal for fitting mode update.
   */
  void EnsureFittingModeLayoutFinishedSignalConnected();

  /**
   * @brief Called when this view's layout is finished.
   *
   * @param[in] view The view whose layout is finished
   * @param[in] bounds The arranged bounds of the view
   */
  void OnLayoutFinished(Ui::View view, LayoutRect bounds);

  /**
   * @brief Register processor
   */
  void RegisterProcessorOnce();

  /**
   * Call if mSize or EffectiveScale changed.
   */
  void SizeOrUiScaleChanged();

  /**
   * @brief Refreshes render effects (e.g. blur) that depend on the current self size.
   *
   * Unlike SizeOrUiScaleChanged(), this does not re-register the fitting-mode
   * processor: fitting mode for layout-arranged views is already driven by the
   * layout-finished signal (see EnsureFittingModeLayoutFinishedSignalConnected()),
   * so triggering it again here would apply it twice per layout pass.
   */
  void RefreshRenderEffects();

protected: // From processor-interface
  void Process(bool postProcessor) override;

  std::string_view GetProcessorName() const override
  {
    return "ViewDataImpl";
  }

private:
  void SetResolvedAccessibilityName(const Dali::String& name);
  void SetResolvedAccessibilityDescription(const Dali::String& description);
  void ApplyLocalizedAccessibilityName(BaseHandle target, const Dali::String& name);
  void ApplyLocalizedAccessibilityDescription(BaseHandle target, const Dali::String& description);

  class ScopedSkipChildrenUpdate;

  void SetBehaviourFlags(ViewImpl::ViewBehaviour behaviourFlags);
  void Destroy();

  MeasuredSize MeasureDefault(float widthConstraint, float heightConstraint);
  LayoutRect   ArrangeDefault(const LayoutRect& bounds);
  bool         HandleKeyEventDefault(const Dali::KeyEvent& event);
  void         FinalizeKeyEventDispatchDefault();
  bool         HasIntrinsicHoverHandlingDefault() const;
  bool         HandleHoverEventDefault(const Dali::HoverEvent& event);
  bool         HasIntrinsicTouchHandlingDefault() const;
  bool         HandleTouchEventDefault(const Dali::TouchEvent& event);
  void         FinalizeTouchEventDispatchDefault(const Dali::TouchEvent& event);
  void         HandleFocusChangedDefault(bool focused);
  void         RelayoutDefault(const Vector2& size, RelayoutContainer& container);
  View         ResolveDefaultFocusRequest();
  bool         ActivateAccessibilityDefault();
  uint32_t     ComputeLogicalChildIndex(const Actor& child) const;
  void         OnChildAdded(Actor& child, bool allowNonViewChild);
  void         OnChildRemoved(Actor& child);
  void         OnViewSceneConnection();
  void         OnViewSceneDisconnection();
  void         OnPropertySet(Property::Index index, const Property::Value& propertyValue);
  void         OnSizeSet(const Vector3& targetSize);
  void         OnSizeAnimation(Animation& animation);
  void         OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Animation::State state);
  void         OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied);
  void         OnChildOrderChanged(Actor parent, Actor orderChangedChild);

  /**
   * @brief Drops the layout caches and raises both dirty bits on this view and on
   * every descendant whose RESOLVED layout direction moved with it.
   *
   * Per node: InvalidateLayoutCaches() plus SETTING mMeasureDirty and mArrangeDirty
   * (never clearing them -- dirty is pending work, and the white-box direction tests
   * assert it on the child). The recursion runs over mChildren and PRUNES any child
   * whose GetLayoutDirection() is not LayoutDirection::INHERIT: that is the exact
   * mirror of dali-core's inherit walk
   * (ActorParentImpl::InheritLayoutDirectionRecursively stops at an actor with
   * mInheritLayoutDirection false), so a child with a direction of its own -- and
   * therefore a subtree whose resolved direction did not move -- keeps its caches.
   *
   * A STANDALONE node takes a full InvalidateMeasure() instead: its invalidation
   * does not propagate to its parent, so it must self-register exactly as it did
   * when every View held its own signal connection.
   *
   * Does NOT touch the effective-scale state and does NOT retract the invalidation
   * propagation generations (contrast ResetSubtreeScaleAndLayoutCaches, which does
   * both): a direction change moves no scale, and leaving the generations unwritten
   * can only cost an extra later ancestor walk, never a missed registration.
   */
  void InvalidateSubtreeLayoutForDirectionChange();

  /**
   * @brief Invalidates this view's LAYOUT, and its affected descendants', after its
   * effective layout direction changed.
   *
   * Connected LAZILY and only by a view that registers with a LIVE WINDOW -- an
   * on-scene layout root, or an on-scene standalone boundary -- in
   * RegisterWithLayoutController(), to the actor's layout-direction-changed signal,
   * which dali-core emits on exactly the set of actors whose RESOLVED direction
   * changed. One connection per on-scene layout root is enough because this handler
   * WALKS THE SUBTREE itself (InvalidateSubtreeLayoutForDirectionChange), pruning at
   * children that hold a direction of their own. A plain child View therefore pays for
   * no connection at all, and a change set on a non-View ancestor -- an intermediate
   * Layer, or the window's root layer -- still reaches the root that sits below it. A
   * direction write on a mid-tree View that is NOT a layout root is covered by
   * OnPropertySet, which raises the same walk.
   *
   * An OFF-SCENE view holds no connection and needs none: no layout pass can run
   * without a window, and a direction that moved while the subtree was detached cannot
   * survive reconnection, because OnViewSceneConnection's layout-root path runs
   * ResetSubtreeScaleAndLayoutCaches() before it registers.
   *
   * A hooked STANDALONE descendant is walked twice on an ancestor change -- once by the
   * ancestor's walk and once by its own hook (core emits on every affected actor) --
   * so a chain of k hooked standalone views costs O(k^2) idempotent invalidation work
   * per direction change. Accepted: over-invalidation only, on a rare event.
   *
   * Invalidates the MEASURE axis (which raises the arrange dirty with it), not
   * arrange alone. Arrange alone would be exactly correct for every first-party
   * producer -- the direction is consumed by ApplyLayoutDirection, and no in-library
   * measure producer reads it (verified across the layout managers and components;
   * text views resolve direction inside their own signal handlers) -- but
   * GetEffectiveLayoutDirection() is public and OnMeasure() is virtual, so an
   * APPLICATION's measure producer can size on it, and the measure cache key has no
   * direction term. That is now a SPECIFIC claim rather than a blanket one: the key is
   * mLastMeasureConstraint plus the scale key (mMeasureKeyOrPropagation.scaleKey), so it does carry the effective
   * scale, and the layout direction is the one producer input it deliberately leaves
   * out. Invalidating measure here is what lets that just work instead of becoming a
   * contract the application has to know.
   *
   * The rejected alternative was a direction term in the measure cache KEY: that puts
   * a layout-direction read into the measure HIT predicate, which runs per view per
   * pass, whereas this handler runs only on an actual direction change. The cost paid
   * here is one re-measure of the affected subtree per locale / direction switch.
   *
   * The ARRANGE cache keeps its own recorded direction (mLastArrangeDirection) as a
   * key term regardless. That is belt and braces for a different failure: the
   * direction lives in dali-core and can be moved through actors dali-ui does not
   * own, so a missed signal must degrade to a cache MISS, never to an arrangement
   * mirrored the wrong way round.
   *
   * @param[in] actor The actor whose resolved layout direction changed (this view)
   * @param[in] type The new resolved layout direction
   */
  void OnLayoutDirectionChanged(Dali::Actor actor, Dali::LayoutDirection::Type type);

  /**
   * @brief Drops the ANCESTOR measure/arrange cache entries after this view has
   * taken a full Measure() miss, up to the nearest layout dependency boundary.
   *
   * A completed Measure() rewrites this view's stored measured size, which every
   * ancestor consumes while arranging (ArrangeDefault and the five layout
   * managers all read the stored slot). When that Measure() did not originate
   * from an ancestor's own pass -- an external View::Measure(), or a measure
   * issued from an unrelated view's producer -- the ancestors' cached results
   * were produced against the PREVIOUS slot, so an ancestor cache hit on the
   * next pass would skip re-measuring this view and then arrange it from the
   * overwritten slot.
   *
   * Cache-only: no dirty bit is raised and nothing is registered with the
   * LayoutController, so this can never schedule (or spin) a layout pass. See
   * the definition in view-data-impl.cpp for the stop conditions.
   *
   * Standalone views are excluded outright (an early return): no ancestor's
   * measured value is a function of a standalone child's slot, so there is
   * nothing for this walk to invalidate. Their slot is corrected on the ARRANGE
   * side instead -- mMeasuredSlotUnconsumed plus the corrective re-measure in
   * ArrangeStandaloneChild -- which is reached whenever the parent arranges,
   * rather than only on an ancestor's measure miss. The arrange cache-HIT path
   * keeps that reachable by testing HasUnconsumedStandaloneChild(): a parent with
   * an unconsumed standalone child cannot hit.
   */
  void InvalidateAncestorLayoutCachesForMeasureMiss();

  /**
   * @brief Drops the DIRECT PARENT's arrange cache entry when this view is about to
   * take an arrange pass that no producer above it owns.
   *
   * The arrange-side counterpart of InvalidateAncestorLayoutCachesForMeasureMiss(),
   * closing the symmetric hole: a completed Arrange() rewrites this view's
   * mArrangedBounds and the input key (mArrangeKeyOrPropagation.inputKey) unconditionally, and an ancestor's cache
   * HIT replays descendants FROM those records on the premise that they were written
   * by that ancestor's own producer chain (see CanReplayArrangeSubtreeFromCache,
   * which skips the cache-KEY comparison for descendants on exactly that premise).
   * An out-of-band public Arrange() breaks the premise: the parent's cached entry
   * would replay the foreign bounds, while a forced miss would re-run the producer
   * and hand back the parent-derived slot -- a hit/miss divergence that violates the
   * contract documented on View::Arrange().
   *
   * ONE node is enough, unlike the measure walk's chain: the measure hit predicate
   * is node-local, but the arrange hit gate is RECURSIVE -- every ancestor re-tests
   * mArrangeCacheValid at every descendant it would elide, so a single cleared
   * parent refuses every ancestor's hit above it, and the misses that follow
   * re-publish level by level (self-healing).
   *
   * Cache-only, exactly like the measure walk: no dirty bit, no registration, so it
   * can never schedule (or spin) a layout pass.
   *
   * OWNED arranges are excluded, because whatever a producer does inside its own
   * pass IS that producer's output and a re-run would reproduce it:
   *  - the direct parent is arrange-in-progress (the normal recursion, every layout
   *    manager, and any third-party producer arranging its own child);
   *  - a RecyclerLayoutOwnerScope is open (the RecyclerView/ItemsLayouter cycle,
   *    whose item geometry the recycler owns under its own invalidation contract);
   *  - this is the framework-owned layout-root pass of a STANDALONE view:
   *    ProcessLayoutRoot derives the same requested-position / measured-extent bounds
   *    as ArrangeStandaloneChild, so the parent's replay premise survives. A public
   *    Arrange() on that same standalone view is NOT excluded -- arbitrary bounds
   *    break the premise exactly as they do for a normal child.
   *
   * @param[in] frameworkLayoutRootPass True only for ArrangeAsLayoutRoot()
   */
  void InvalidateParentArrangeCacheForOutOfBandArrange(bool frameworkLayoutRootPass);

  /**
   * @brief Shared implementation for public/parent Arrange and LayoutController's
   * root Arrange entry point.
   */
  LayoutRect ArrangeImpl(const LayoutRect& bounds, bool frameworkLayoutRootPass);

  /**
   * @brief Declines this view's arrange cache publish for the CURRENT pass because a
   * cache-ONLY invalidation reached it mid-pass.
   *
   * Unlike a logical invalidation, it enqueues no pending work at all: a cache-only
   * invalidation must never become either a parked request or a wake-armed request.
   * Pass-local by construction (ArrangePassGuard clears the bit at pass entry, the
   * publish gate reads it at pass exit), hence the precondition that an arrange pass is
   * actually running.
   */
  void BlockArrangeCachePublishDuringPass();

  /**
   * @brief Recomputes mArrangeProducerAlways from the active producer's policy.
   *
   * Mirrors the producer dispatch order ArrangeCallback > LayoutManager > OnArrange.
   * The result is cached at producer mutation points so the arrange cache predicate
   * only needs one bit test.
   */
  void RefreshArrangeProducerPolicy();

  /**
   * @brief Re-derives layout producer state after a reserved layout trait was added,
   * replaced or removed.
   *
   * A no-op unless @p id is ReservedTraitId::LAYOUT_SIGNALS (the LayoutCallbacksObject,
   * which holds BOTH the MeasureCallback and the ArrangeCallback) or
   * ReservedTraitId::LAYOUT_MANAGER (the LayoutManager, which supplies both Measure()
   * and Arrange()) -- the only two traits that change WHICH producer a layout pass
   * dispatches to. Both therefore carry a producer on BOTH axes, which is why this
   * covers measure as well as arrange: it re-derives the arrange execution policy, and
   * it retracts the cached results the outgoing producer published. For LAYOUT_SIGNALS
   * it also clears the stored callback policy, since that policy belonged to the
   * callback object being replaced.
   *
   * Defensive: it exists for the public Integration::View::SetTrait/RemoveTrait surface,
   * which can reach those ids without going through
   * SetMeasureCallback()/SetArrangeCallback()/AttachLayoutManager().
   *
   * @param[in] id The trait that changed
   */
  void OnLayoutProducerTraitChanged(TraitId id);

  /**
   * @brief Whether any DIRECT child is a standalone view whose freshly measured
   * slot this view has not consumed yet.
   *
   * A term of the arrange cache-HIT predicate, and the reason the forward note on
   * ArrangeStandaloneChildren (see view-data-impl.cpp) exists: the corrective
   * re-measure for an unconsumed standalone slot lives on the ARRANGE path, so an
   * Arrange() that returns early on a cache hit would silently skip it. Declining
   * the hit while such a child exists keeps the correction reachable; the very
   * next (missing) pass consumes the slot and clears the bit, so this can decline
   * at most one pass per out-of-band Measure().
   *
   * Cost-ordered on purpose, and the SELECTIVE term goes first:
   * mMeasuredSlotUnconsumed is set unconditionally at every measure publish and is
   * cleared only by the two standalone loops, so it is TRUE for every regular child
   * in the steady state and decides nothing. IsLayoutModeStandalone is the term that
   * actually rejects, so it is tested first and the bit only qualifies the few
   * standalone children. O(direct children), no recursion.
   *
   * @note This is NOT the guard for a NEVER-MEASURED standalone child.
   * mMeasuredSlotUnconsumed is initialised false and is raised only at a measure
   * publish, so a standalone child that was just added and has not been measured yet
   * leaves this query FALSE. What keeps that child reachable is the ARRANGE
   * invalidation ViewDataImpl::OnChildAdded issues on the standalone-child path: it
   * retracts the cache entry that was published for the older child set. This query
   * covers only the other half -- an already measured standalone child whose fresh
   * slot the parent has not consumed yet.
   *
   * @note O(direct children) is exactly the scope of this query, so on its own it
   * says nothing about a DESCENDANT holding an unconsumed slot. What extends the
   * claim to a subtree is CanReplayArrangeSubtreeFromCache(), which evaluates this
   * same term at every node it would elide and refuses the whole hit if any node
   * fails it.
   *
   * @return True when at least one direct child is standalone AND has an
   *         unconsumed measured slot
   */
  bool HasUnconsumedStandaloneChild() const;

  /**
   * @brief Drops this view's cached effective scale.
   *
   * Single concern: the CACHED SCALE only. Clears BOTH of its sync bits -- the
   * one that says the cached value is usable (mEffectiveScaleValid), so the next
   * GetEffectiveScale() recomputes from the (possibly re-rooted) parent chain,
   * and the one that says the ACTOR already holds that value
   * (mEffectiveScaleActorSynced), whose claim names the very value being
   * retracted -- and records the drop when it lands inside a running arrange pass
   * so that pass declines to publish a result produced against the old scale.
   *
   * It touches no cache and no dirty bit: whether dropping the scale must also
   * drop cached layout results is the caller's decision, not this function's.
   *
   * @warning A scale change DOES invalidate this view's arranged result (the
   * arrangement is scale-applied). The invariant the arrange cache relies on --
   * "mArrangeCacheValid is true only while the effective scale is unchanged
   * since publish", which lets the arrange cache-HIT predicate omit a scale term
   * (and assert mEffectiveScaleValid instead) -- holds ONLY because every caller
   * that drops the scale ALSO calls InvalidateLayoutCaches() on the same view.
   * Do NOT add a freshness-only caller of this alone: it would leave a valid
   * arrange cache computed against the old scale, served as a hit with no test
   * to catch it. Pair the two, or use ResetSubtreeScaleAndLayoutCaches() which
   * does.
   *
   * The MEASURE cache is the one exception, and only because it carries a scale
   * KEY of its own (mMeasureKeyOrPropagation.scaleKey): an unpaired caller would cost it a miss
   * rather than a wrong measured size. That does not license the unpaired call --
   * the arrange cache and the actor-side push below both still depend on the
   * pairing -- it just means the measure side has a second line of defence.
   */
  void DropCachedEffectiveScale();

  /**
   * @brief Drops this view's cached measure and arrange results.
   *
   * Single concern: the CACHED RESULTS only. Both caches go together because a
   * measured size is an input to this view's own arrangement, so an arrange
   * result cannot outlive the measurement it was produced against.
   *
   * Deliberately does NOT touch mMeasureDirty. Dirty means "this view has
   * layout work that has not been consumed yet"; it is consumed at pass entry
   * (MeasurePassGuard) and is not a freshness bit, so clearing it here would
   * silently DISCARD pending work rather than invalidate a stale result. That
   * matters on the recursive path: the callers' follow-up InvalidateMeasure()
   * only re-arms the node it is called on and its ANCESTORS, so a descendant
   * whose dirty was cleared here would never get it back.
   */
  void InvalidateLayoutCaches();

  /**
   * @brief The NODE-LOCAL half of the arrange cache-HIT predicate.
   *
   * "May THIS view's arrange producer be elided for THIS input?" -- the entry exists
   * and is fresh, the producer uses ArrangePolicy::IF_CHANGED, the input matches the cache
   * KEY, the
   * effective layout direction matches, and no direct standalone child is holding an
   * unconsumed measured slot. The full, cost-ordered rationale for each term is in
   * ViewDataImpl::Arrange, which is the only caller.
   *
   * Says nothing about descendants. CanReplayArrangeSubtreeFromCache() is the other
   * half, and a hit requires both.
   *
   * @param[in] bounds The candidate arrange input
   * @return True when this view's producer may be elided for @p bounds
   */
  bool CanServeArrangeFromCache(const LayoutRect& bounds) const;

  /**
   * @brief Whether every node strictly BELOW this one may have its producer elided.
   *
   * The recursive half of the arrange cache-HIT gate. Read-only and side-effect-free:
   * it is phase one of a validate-then-replay hit, so that "hit" stays atomic. A
   * fused walk that bailed out half way would already have written cached bounds into
   * part of the subtree, and the MISS that followed would not necessarily revisit
   * every node it wrote.
   *
   * Per node it re-tests the node-local terms of CanServeArrangeFromCache() MINUS the
   * cache KEY -- a descendant has no candidate bounds, and does not need one: with
   * this view's own key matched and every producer using ArrangePolicy::IF_CHANGED, each
   * producer hands its
   * children the same slots as last pass, which is exactly what those children
   * resolved into the arranged bounds the replay applies. Children with no arrange
   * result are skipped, because the replay does not visit them either (a Label's
   * children, for instance).
   *
   * Cost is one read-only walk, paid only by a node whose own cache is already live
   * -- which implies no non-standalone descendant is dirty, since dirtiness
   * propagates upward. A childless view never enters it at all.
   *
   * @return True when the whole subtree below this view may be replayed from cache
   */
  bool CanReplayArrangeSubtreeFromCache() const;

  /**
   * @brief Serves the arrange cache for this view and its settled subtree.
   *
   * Phase two of the hit: a pre-order walk that performs, per node, exactly the
   * observable work an arrange MISS performs -- reconcile the actor against the node's
   * cached arranged bounds (mirrored for a child of a right-to-left parent, see below),
   * recurse into the children that hold an arrange result, mark the initial layout done
   * and register for LayoutFinished -- while eliding only the PRODUCER.
   *
   * It is NOT a prune. Skipping the subtree would drop the per-level reconciliation
   * that repairs actor geometry written outside layout, which View::Arrange documents
   * as a promise ("the arranged geometry is reconciled either way").
   *
   * The right-to-left mirror is FUSED into each node's own single self apply rather than
   * being a separate parent-side pass, which is the one place the replay's shape differs
   * from ApplyLayoutDirection on the MISS path. Mirroring afterwards would write POSITION_X
   * twice per child per pass -- once logical, once mirrored -- and each write emits a
   * synchronous PropertySetSignal, so a SETTLED right-to-left subtree was never the
   * write-free replay the hit is supposed to be. mArrangedBounds stays LOGICAL either way:
   * only the value handed to ApplySelfBoundsIfChanged is resolved to physical.
   *
   * @param[in] mirrorUnderParentRtl True when the caller is this view's parent AND that
   *            parent's effective layout direction resolves to RIGHT_TO_LEFT AND this view
   *            is not a standalone child -- the same three conditions ApplyLayoutDirection
   *            applies on the MISS path. FALSE at the top-level hit, whose own mirror
   *            belongs to a parent that is not running.
   * @param[in] parentArrangedWidth The caller's cached arranged width, i.e. the value the
   *            MISS path passes to ApplyLayoutDirection. Read only when
   *            @p mirrorUnderParentRtl is true.
   *
   * @pre CanServeArrangeFromCache() holds for this view and, unless it is childless,
   *      CanReplayArrangeSubtreeFromCache() does too.
   * @pre A ReplayPassScope is on the stack (constructed at the hit site).
   */
  void ReplayArrangeSubtreeFromCache(bool mirrorUnderParentRtl, float parentArrangedWidth);

  MeasuredSize ApplyConstraints(const MeasuredSize& size) const;
  void         MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight);
  void         ArrangeStandaloneChildren(const LayoutRect& bounds);
  void         ApplyLayoutDirection(float parentWidth);
  MeasuredSize DispatchMeasureWithLayoutManager(LayoutManager* manager, float widthConstraint, float heightConstraint);
  void         DispatchArrangeWithLayoutManager(LayoutManager* manager, const LayoutRect& bounds);
  LayoutRect   DispatchArrangeWithCallback(ArrangeCallback* callback, const LayoutRect& bounds);
  void         ApplySelfBoundsIfChanged(const LayoutRect& bounds);
  void         OnColorTableChanged();

  /**
   * @brief Emits the resource ready signal.
   */
  void EmitResourceReadySignal();
  /**
   * @brief Callbacks called on idle.
   *
   * @return True if we need to call this idle callback one more time.
   */
  bool OnIdleCallback();

  /**
   * Set off-screen rendering.
   * @param[in] offScreenRenderingType enum OffScreenRenderingType
   * @note When offscreen rendering is on, changing visual's depth index may not apply instantaneously. Turn it off and
   * on again.
   */
  void SetOffScreenRendering(int32_t offScreenRenderingType);

  /**
   * Notify to this view's corner radius changed.
   */
  void UpdateCornerRadius();

  /**
   * Notify to this view's borderline changed.
   */
  void UpdateBorderline();

private:
  using TraitEntries = std::vector<std::pair<TraitId, IntrusivePtr<TraitObject>>>;

  /// RAII transaction guards for a single Measure() / Arrange() PRODUCER pass on this
  /// view. Defined in view-data-impl.cpp. Entry CONSUMES the dirty bit and clears the
  /// cache-valid bit; exit restores only the in-progress bit and never re-arms dirty
  /// (recovery from an abandoned pass is the LayoutController's, not the guard's --
  /// see LayoutControllerImpl::PendingBatchRollbackScope).
  struct MeasurePassGuard;
  struct ArrangePassGuard;

  /// RAII scopes for an arrange cache-HIT REPLAY. Defined in view-data-impl.cpp.
  /// ReplayPassScope is constructed ONCE, at the hit site, and holds the layout-pass
  /// depth open across the whole subtree; ReplayNodeScope marks one visited node as
  /// arrange-in-progress for the duration of that node's applies and recursion.
  /// Neither consumes a dirty bit or a cache entry: a replay SERVES an entry rather
  /// than producing one, which is exactly why the pass guards above cannot be reused.
  struct ReplayPassScope;
  struct ReplayNodeScope;

  struct SizeConstraints
  {
    float minWidth  = 0.0f;
    float minHeight = 0.0f;
    float maxWidth  = std::numeric_limits<float>::max();
    float maxHeight = std::numeric_limits<float>::max();
  };

  struct FocusNavigationData
  {
    int                     leftId             = -1;
    int                     rightId            = -1;
    int                     upId               = -1;
    int                     downId             = -1;
    int                     clockwiseId        = -1;
    int                     counterClockwiseId = -1;
    int                     forwardId          = -1;
    int                     backwardId         = -1;
    FocusNavigationCallback callback;
  };

  struct RenderEffectData
  {
    // Public effect set through View::SetRenderEffect().
    RenderEffectImplPtr renderEffect;

    // Unlike renderEffect, this handleless effect is created only by the OFFSCREEN_RENDERING property.
    std::unique_ptr<OffScreenRenderingImpl>    offScreenRendering;
    View::OffScreenRenderingType               offScreenRenderingType{View::OffScreenRenderingType::NONE};
    View::OffScreenRenderingFinishedSignalType offScreenRenderingFinishedSignal;
  };

  struct ResourceReadyData
  {
    View::ResourceReadySignalType resourceReadySignal;
    CallbackBase*                 idleCallback{nullptr};
    bool                          isEmittingResourceReadySignal{false};
    bool                          idleCallbackRegistered{false};
  };

  struct LayoutTransitionData
  {
    LayoutTransition              transition;
    std::unordered_set<ViewImpl*> pendingEnterChildren;
    std::unordered_set<ViewImpl*> pendingReorderedChildren;
    bool                          hasPendingChildRemoval{false};
  };

  SizeConstraints& EnsureSizeConstraints()
  {
    if(!mSizeConstraints)
    {
      mSizeConstraints = std::make_unique<SizeConstraints>();
    }
    return *mSizeConstraints;
  }

  FocusNavigationData& EnsureFocusNavigationData()
  {
    if(!mFocusNavigationData)
    {
      mFocusNavigationData = std::make_unique<FocusNavigationData>();
    }
    return *mFocusNavigationData;
  }

  RenderEffectData& EnsureRenderEffectData()
  {
    if(!mRenderEffectData)
    {
      mRenderEffectData = std::make_unique<RenderEffectData>();
    }
    return *mRenderEffectData;
  }

  ResourceReadyData& EnsureResourceReadyData()
  {
    if(!mResourceReadyData)
    {
      mResourceReadyData = std::make_unique<ResourceReadyData>();
    }
    return *mResourceReadyData;
  }

  LayoutTransitionData& EnsureLayoutTransitionData()
  {
    if(!mLayoutTransitionData)
    {
      mLayoutTransitionData = std::make_unique<LayoutTransitionData>();
    }
    return *mLayoutTransitionData;
  }

  bool HasLayoutTransition() const
  {
    return mLayoutTransitionData && mLayoutTransitionData->transition;
  }

  int GetFocusNavigationId(int FocusNavigationData::* field) const
  {
    return mFocusNavigationData ? mFocusNavigationData.get()->*field : -1;
  }

  /// Retire a standing cache entry on one axis: clear the valid bit AND retract the
  /// propagation record that shares its slot (see the KEY/record unions below). These
  /// are the ONLY correct way to take m*CacheValid from true to false -- writing the
  /// bit directly would leave stale KEY bits readable as a generation. No-op when the
  /// entry is already absent, which is what preserves a live record (and with it the
  /// invalidation batch coalescing) across repeated invalidations.
  /// @{
  void DropMeasureCacheEntry()
  {
    if(mMeasureCacheValid)
    {
      mMeasureCacheValid                             = false;
      mMeasureKeyOrPropagation.propagationGeneration = 0u;
    }
  }
  void DropArrangeCacheEntry()
  {
    if(mArrangeCacheValid)
    {
      mArrangeCacheValid                             = false;
      mArrangeKeyOrPropagation.propagationGeneration = 0u;
    }
  }
  /// @}

  ViewImpl&                            mViewImpl;
  ViewState                            mState;
  UiScalePolicy                        mScalePolicy{UiScalePolicy::INHERIT};
  LayoutMode                           mLayoutMode;           ///< Layout mode of the view. PACKING: parked here, next to the other one-byte policy enum, because it fills the pad mScalePolicy would otherwise leave; its logical home is the layout group further down, which cross-references it.
  mutable float                        mEffectiveScale{1.0f}; ///< Cached effective scale. Carries NO validity sentinel of its own; whether this value is usable is recorded by mEffectiveScaleValid. Mutable because the lazy (re)compute happens inside the const GetEffectiveScale().
  float                                mRequestedWidth;       ///< Requested width (WRAP_CONTENT = -1.0f, MATCH_PARENT = -2.0f). PACKING: parked here to fill the 4-byte pad in front of mTraits; it belongs logically with mRequestedHeight, which sits in the layout group further down.
  TraitEntries                         mTraits;
  Internal::CoreInteractionObject*     mCoreInteractionObject;
  std::unique_ptr<VisualData>          mVisualData; ///< Visual context, allocated on first use by EnsureVisualData(). Null means EITHER visuals are disabled (DISABLE_VISUALS) OR no visual has been touched yet -- read paths answer both the same way.
  std::unique_ptr<AttachmentContainer> mAttachments;
  std::unique_ptr<FocusNavigationData> mFocusNavigationData;
  std::unique_ptr<RenderEffectData>    mRenderEffectData;
  std::unique_ptr<ResourceReadyData>   mResourceReadyData;
  View::StateChangedSignalType         mStateChangedSignal;
  View::KeyEventSignalType             mKeyEventSignal;
  View::FocusChangedSignalType         mFocusChangedSignal;
  View::LayoutFinishedSignalType       mLayoutFinishedSignal;

  float        mRequestedX;
  float        mRequestedY;
  MeasuredSize mMeasuredSize;          ///< Last completed measure result. Always readable (GetMeasuredSize() and layout managers consume it during Arrange regardless of cache state); mMeasureCacheValid only governs whether the KEY below may serve a cache hit.
  MeasuredSize mLastMeasureConstraint; ///< Pure cache KEY: the effective natural constraint the cached mMeasuredSize was produced for. Carries no dirty/never-measured sentinel meaning; validity lives in mMeasureCacheValid / mMeasureDirty.
  /// @name Cache KEY / propagation record slots
  /// Each axis overlays its pure cache KEY with its invalidation propagation record,
  /// because their lifetimes never overlap. The discriminator is the axis's cache-valid
  /// bit:
  ///  - while m*CacheValid is TRUE, the KEY member is active. A standing entry means
  ///    the pending work any record described has been consumed, so the record is dead.
  ///  - while m*CacheValid is FALSE, the propagation record is active: the generation
  ///    in which this view's last Invalidate*() walked its ancestor chain to a layout
  ///    root and registered it. While a record still equals
  ///    LayoutInvalidation::CurrentGeneration(), that registration is known to be live
  ///    and not yet processed, so a further invalidation on the SAME axis may skip the
  ///    walk entirely. 0 = never propagated (AdvanceGeneration skips 0 on wrap).
  ///
  /// The overlay is sound ONLY under one write rule: every TRUE -> FALSE edge of the
  /// valid bit retracts the record to 0 in the same operation (Drop*CacheEntry() is
  /// the sole such edge). Without it the stale KEY bits would be read back as a
  /// generation, and a pattern like 1.0f (0x3F800000) can eventually EQUAL the live
  /// generation and silently skip a walk. Retracting on the EDGE only -- never on a
  /// re-invalidation of an already-absent entry -- is what keeps the batch coalescing
  /// intact: repeated invalidations while the entry stays absent leave a live record
  /// untouched.
  ///
  /// The two axes are separate and must not be merged. An arrange walk marks the
  /// ancestors' arrange dirty but leaves their MEASURE caches valid, so an
  /// InvalidateMeasure() that skipped its walk on the strength of an arrange record
  /// would leave every ancestor's measure hitting -- and an ancestor measure hit does
  /// not re-measure its children, so this view's new measured size would never be
  /// computed at all.
  /// @{
  union MeasureKeyOrPropagation
  {
    MeasureKeyOrPropagation()
    : propagationGeneration(0u)
    {
    }
    float    scaleKey; ///< Pure cache KEY: the effective scale the cached mMeasuredSize was produced at. Compared EXACTLY, not with FloatEqual, because it is a straight copy of the same GetEffectiveScale() value with no arithmetic between publish and compare -- unlike mLastMeasureConstraint, which reaches the predicate through a /s normalisation and a min/max clamp and therefore needs the tolerance.
    uint32_t propagationGeneration;
  };
  union ArrangeKeyOrPropagation
  {
    ArrangeKeyOrPropagation()
    : propagationGeneration(0u)
    {
    }
    LayoutRect inputKey; ///< Pure cache KEY: the input bounds mArrangedBounds was produced for.
    uint32_t   propagationGeneration;
  };
  MeasureKeyOrPropagation mMeasureKeyOrPropagation;
  LayoutRect              mArrangedBounds;
  ArrangeKeyOrPropagation mArrangeKeyOrPropagation;
  /// @}

  /// @name Layout inputs
  /// Three members of this group live elsewhere purely for PACKING, each next to a
  /// hole it fills: mLayoutMode and mRequestedWidth at the top of the class (beside
  /// mScalePolicy and mEffectiveScale), and mLastArrangeDirection at the end of the
  /// bit-field run below. Nothing but the byte layout distinguishes them from the
  /// members here.
  /// @{
  Insets                           mMargin;                       ///< Layout margin
  Insets                           mPadding;                      ///< Layout padding
  Vector2                          mSize;                         ///< The size of the view
  Vector2                          mLastArrangedRenderEffectSize; ///< Self size last seen by ApplySelfBoundsIfChanged, used only to dedupe render-effect refresh (kept separate from mSize, which Process()/ApplyFittingMode rely on)
  float                            mRequestedHeight;              ///< Requested height (WRAP_CONTENT = -1.0f, MATCH_PARENT = -2.0f). PACKING: placed here rather than beside mRequestedWidth so that mSizeConstraints below still lands 8-aligned with no hole in front of it.
  std::unique_ptr<SizeConstraints> mSizeConstraints;              ///< Lazy-allocated measurement min/max bounds (natural units).
  /// @}
  Dali::Vector<View>                    mChildren; ///< Synchronized with Actor hierarchy via OnChildAdd/OnChildRemove.
  std::unique_ptr<LayoutTransitionData> mLayoutTransitionData;

  std::unique_ptr<AccessibilityData> mAccessibilityData;
  AccessibleObjectCreator            mAccessibleObjectCreator;
  int32_t                            mAccessibilityRole : Dali::Log<static_cast<uint32_t>(Accessibility::Role::MAX_COUNT)>::value + 2; ///< Frequently touched accessibility-related value kept here to avoid AccessibilityData creation.

  bool         mSkipChildrenUpdate : 1;
  bool         mMeasureCacheValid : 1;                            ///< True when mLastMeasureConstraint + the scale key + mMeasuredSize hold a usable cache entry. Cleared ONLY via DropMeasureCacheEntry(), which also retracts the record sharing the KEY slot.
  bool         mMeasureDirty : 1;                                 ///< True when invalidated since the last measure.
  bool         mMeasureInProgress : 1;                            ///< True while this view's own Measure() is on the stack.
  bool         mMeasurePassPoisoned : 1;                          ///< True when an invalidation arrived while this view's measure pass was running.
  bool         mMeasuredSlotUnconsumed : 1;                       ///< True while the measured size published by the last completed measure pass has not been consumed by this view's parent. Set unconditionally at the publish; cleared by the parent in MeasureStandaloneChildren / ArrangeStandaloneChildren. Read only on the standalone path: it tells ArrangeStandaloneChild that the slot may be the leftover of an out-of-band Measure() and must be re-measured against the parent's extent before it is placed.
  bool         mArrangeCacheValid : 1;                            ///< True when The input key + mArrangedBounds hold a usable cache entry. Cleared ONLY via DropArrangeCacheEntry(), which also retracts the record sharing the KEY slot.
  bool         mArrangeDirty : 1;                                 ///< True when invalidated since the last arrange.
  bool         mArrangeInProgress : 1;                            ///< True while this view's own Arrange() is on the stack; guards same-view re-entrancy.
  bool         mArrangeReplayInProgress : 1;                      ///< True while this view is being visited by an arrange cache-HIT REPLAY (ReplayNodeScope), which also raises mArrangeInProgress so the re-entrancy guard in ArrangeImpl covers a replay too. The discriminator exists because three OWNERSHIP tests read mArrangeInProgress as "a producer pass below me owns this work" -- InvalidateAncestorLayoutCachesForMeasureMiss's direct-parent stop and its cache-publish block, and InvalidateParentArrangeCacheForOutOfBandArrange's parent stop. A replay runs no producer and owns nothing, so those three must see through it or an out-of-band Measure/Arrange raised from a property-set observer mid-replay would leave a stale ancestor entry standing.
  bool         mArrangePassPoisoned : 1;                          ///< True when an invalidation arrived while this view's arrange pass was running.
  bool         mArrangeCacheBlockedDuringPass : 1;                ///< True when a cache-ONLY invalidation arrived while this view's arrange pass was running. Declines the cache publish without poisoning the pass and enqueues no layout work (parked or wake-armed). Set by InvalidateAncestorLayoutCachesForMeasureMiss on an unowned arrange-in-progress ancestor; see BlockArrangeCachePublishDuringPass.
  bool         mArrangeResultAvailable : 1;                       ///< True once at least one arrange pass has published a result into mArrangedBounds.
  bool         mArrangeOverrideAlways : 1;                        ///< True when this view's OnArrange() uses ArrangePolicy::ALWAYS. Default FALSE (ArrangePolicy::IF_CHANGED).
  bool         mArrangeCallbackAlways : 1;                        ///< True when the installed ArrangeCallback uses ArrangePolicy::ALWAYS. The one-argument overload resets it to FALSE.
  bool         mArrangeProducerAlways : 1;                        ///< Derived from the active producer and read by the arrange cache predicate. Default FALSE (ArrangePolicy::IF_CHANGED).
  mutable bool mEffectiveScaleValid : 1;                          ///< THE sync bit for mEffectiveScale: true exactly when mEffectiveScale equals what ComputeEffectiveScale() would return now. Set by the lazy compute in the const GetEffectiveScale() (hence mutable), cleared by every scale-context invalidation.
  bool         mEffectiveScaleActorSynced : 1;                    ///< THE sync bit for the ACTOR-side copy of the scale (the animatable VIEW_EFFECTIVE_SCALE property): true exactly when that property is known to hold mEffectiveScale. The second half of the pair whose first half is mEffectiveScaleValid -- that one says the CACHED scale is usable, this one says the ACTOR already has it. Set by Measure()'s push (after the write, which re-enters the clear below), cleared by DropCachedEffectiveScale() (the value it names has been retracted) and by ViewDataImpl::SetProperty for that index, which is the single funnel every event-side write of the property passes through. Default false, so the first Measure() always pushes.
  bool         mEffectiveScaleInvalidatedDuringPass : 1;          ///< True when the cached effective scale was invalidated while an arrange pass was running.
  bool         mInitialLayoutDone : 1;                            ///< True after this view has completed at least one arrange pass; used by the dispatcher to suppress ENTER on initial mount
  bool         mInitialEnterSettled : 1;                          ///< True once LayoutTransitionDispatcher::SettleInitialEnter has actually BAKED an ENTER spec onto this view. The fresh-child settle branch is re-entered on every pass for a child no producer ever arranges (freshChild stays true forever), and each entry would otherwise allocate a 0-duration Animation and re-bake. Cleared by OnChildAdded, the only path that can change which transition governs this view.
  bool         mIsFocusGroup : 1;                                 ///< Stores whether the view is a focus group.
  bool         mDispatchKeyEvents : 1;                            ///< Whether the actor emits key event signals
  bool         mAccessibleCreatable : 1;                          ///< Whether we can create new accessible or not.
  bool         mProcessorRegistered : 1;                          ///< Whether the processor is registered.
  bool         mFittingModeLayoutFinishedSignalConnected : 1;     ///< Whether layout-finished signal is connected for fitting mode update.
  bool         mDefaultFocusIndicatorSuppressedByStateEffect : 1; ///< Whether the current StateEffect suppresses the default focus indicator.
  bool         mLayoutDirectionSignalConnected : 1;               ///< True once this view registered with the LayoutController on a live window (an on-scene layout root or on-scene standalone boundary) and connected the actor layout-direction signal; never cleared -- the claim "core emits on this actor" survives reparenting and scene disconnection, which is what keeps OnPropertySet's short-circuit sound.
  bool         mInPassInvalidationWarned : 1;                     ///< Latched once this View has logged an in-pass Invalidate*() contract violation; never cleared.
  bool         mChildOrderSignalConnected : 1;                    ///< True once this view connected the actor child-order signal. Made lazily, at the first tracked (View) child add, because that is the earliest moment a reorder of a tracked child becomes possible: every dali-core emit site moves an actor that is ALREADY a child, and a fresh Add / InsertAbove emits nothing. Never cleared -- the claim "core emits on this actor" survives the last child leaving.

  /// Pure cache KEY: the effective layout direction mArrangedBounds was produced under.
  /// Valid only while mArrangeCacheValid is true. Recorded as a KEY because the direction
  /// lives in dali-core and can be moved through actors dali-ui does not own, so a missed
  /// invalidation must degrade to "no cache hit" and never to a wrongly mirrored
  /// arrangement. The choice is per (axis, input) pair, not per input: measure x scale is
  /// also a KEY (the measure scale key, and `s` is already in hand there), while arrange x
  /// scale relies on invalidation plus a DEBUG assert (reading the scale here would be a
  /// fresh call the path needs for nothing else) and measure x direction relies on
  /// invalidation (the direction-change subtree walk), because a direction term would put
  /// a layout-direction read into the per-view, per-pass measure predicate. The invalidation
  /// this KEY backs up is now the RECURSIVE subtree walk driven by a layout root's lazy
  /// signal hook and by OnPropertySet, not a per-View signal connection, so it covers a
  /// strictly wider set of failure modes than before.
  ///
  /// PACKING: a 2-bit field, not the 4-byte enum it used to be. LayoutDirection::Type has
  /// three enumerators, but only LEFT_TO_RIGHT and RIGHT_TO_LEFT are ever STORED here --
  /// the only writer copies GetEffectiveLayoutDirection(), whose documented dali-core
  /// invariant is to resolve INHERIT and never return it. Even a contract-violating
  /// write that smuggled INHERIT into the resolved slot still fits (an unsigned 2-bit
  /// field holds 0..3), and a truncated value can only fail the equality below -- a
  /// cache MISS -- never fabricate a HIT. Placed at the END of the run rather than
  /// inside it so that a compiler which does not merge allocation units across a change
  /// of field type opens ONE new unit here instead of splitting the bool run in two.
  /// If a target compiler treats this 2-bit enum field as SIGNED, widen it to `: 3`
  /// (the stored values are 0 and 1 either way; the width is about staying lossless
  /// for the full enumerator range).
  Dali::LayoutDirection::Type mLastArrangeDirection : 2;

  /// A whole bool, not a bit-field: ScopedTrueFlag binds a `bool&`, which a bit-field
  /// cannot provide. It sits between the bit-field run above and mFlags below; a
  /// non-bit-field member always starts at the next whole byte, so this placement
  /// costs exactly one byte.
  bool mKeyEventDispatchInProgress; ///< True while this view's key event dispatch is on the stack; guards unsupported same-view re-entrancy.

  static constexpr uint32_t VIEW_BEHAVIOUR_FLAG_COUNT = Dali::Log<static_cast<uint32_t>(ViewImpl::LAST_VIEW_BEHAVIOUR_FLAG)>::value + 1;
  ViewImpl::ViewBehaviour   mFlags : VIEW_BEHAVIOUR_FLAG_COUNT; ///< Flags passed in from constructor.

  // Property registrations access private methods and data of ViewImpl and ViewDataImpl.
  static const PropertyRegistration           PROPERTY_1;
  static const PropertyRegistration           PROPERTY_2;
  static const PropertyRegistration           PROPERTY_3;
  static const PropertyRegistration           PROPERTY_5;
  static const PropertyRegistration           PROPERTY_6;
  static const PropertyRegistration           PROPERTY_7;
  static const PropertyRegistration           PROPERTY_8;
  static const PropertyRegistration           PROPERTY_9;
  static const PropertyRegistration           PROPERTY_10;
  static const PropertyRegistration           PROPERTY_11;
  static const PropertyRegistration           PROPERTY_12;
  static const PropertyRegistration           PROPERTY_13;
  static const PropertyRegistration           PROPERTY_14;
  static const PropertyRegistration           PROPERTY_15;
  static const PropertyRegistration           PROPERTY_22;
  static const PropertyRegistration           PROPERTY_24;
  static const PropertyRegistration           PROPERTY_25;
  static const PropertyRegistration           PROPERTY_31;
  static const PropertyRegistration           PROPERTY_32;
  static const PropertyRegistration           PROPERTY_33;
  static const PropertyRegistration           PROPERTY_34;
  static const PropertyRegistration           PROPERTY_35;
  static const PropertyRegistration           PROPERTY_36;
  static const PropertyRegistration           PROPERTY_37;
  static const PropertyRegistration           PROPERTY_38;
  static const PropertyRegistration           PROPERTY_39;
  static const PropertyRegistration           PROPERTY_40;
  static const PropertyRegistration           PROPERTY_42;
  static const PropertyRegistration           PROPERTY_43;
  static const PropertyRegistration           PROPERTY_44;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_1;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_2;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_3;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_4;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_5;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_6;
  static const AnimatablePropertyRegistration ANIMATABLE_PROPERTY_7;
};

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_VIEW_DATA_IMPL_H
