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

// CLASS HEADER
#include "view-data-impl.h"
#include "view-accessibility-data.h"
#include "view-visual-data.h"
#include "visual-constraint-functions.h"

// EXTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility-devel.h> // LCOV_EXCL_LINE
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/devel-api/size-negotiation/relayout-container.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>  // LCOV_EXCL_LINE
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/object-registry.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/shadow.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/size-negotiated-view-impl.h>
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/internal/common/attachment-container.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/input-event-impl.h>
#include <dali-ui-foundation/internal/layouts/absolute-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/flex-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/grid-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-object.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-direction-utils.h>
#include <dali-ui-foundation/internal/layouts/layout-invalidation-generation.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-object.h>
#include <dali-ui-foundation/internal/layouts/layout-reflow-resolver.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/standalone-bounds-utils.h>
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>
#include <dali-ui-foundation/internal/views/state-effect-target-trait.h>
#include <dali-ui-foundation/internal/views/state-handler-trait.h>
#include <dali-ui-foundation/internal/views/view-state-manager.h>
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/inner-shadow.h>
#include <dali-ui-foundation/internal/views/view/view-gradient-color-binding.h>
#include <dali-ui-foundation/internal/visuals/visual-property-map-helper.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/text/text-utils.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/types/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <algorithm>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace ExtensionView   = Dali::Ui::Extension::View;
namespace IntegrationView = Dali::Ui::Integration::View;

namespace Dali
{
namespace Ui
{
namespace Internal
{

class ViewDataImpl::ScopedSkipChildrenUpdate
{
public:
  explicit ScopedSkipChildrenUpdate(ViewDataImpl& viewDataImpl)
  : mViewDataImpl(viewDataImpl),
    mPrevious(viewDataImpl.mSkipChildrenUpdate)
  {
    mViewDataImpl.mSkipChildrenUpdate = true;
  }

  ~ScopedSkipChildrenUpdate()
  {
    mViewDataImpl.mSkipChildrenUpdate = mPrevious;
  }

  ScopedSkipChildrenUpdate(const ScopedSkipChildrenUpdate&)            = delete;
  ScopedSkipChildrenUpdate& operator=(const ScopedSkipChildrenUpdate&) = delete;

private:
  ViewDataImpl& mViewDataImpl;
  bool          mPrevious;
};

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VIEW_DATA");
#endif

constexpr unsigned int OFF_SCREEN_RENDERING_TYPE_COUNT        = 3u;
constexpr char         BACKGROUND_COLOR_BINDING_ID[]          = "BackgroundColor";
constexpr char         BACKGROUND_GRADIENT_BINDING_ID[]       = "BackgroundGradient";
constexpr char         COLOR_BINDING_ID[]                     = "Color";
constexpr char         ACCESSIBILITY_NAME_BINDING_ID[]        = "Ui.View.AccessibilityName";
constexpr char         ACCESSIBILITY_DESCRIPTION_BINDING_ID[] = "Ui.View.AccessibilityDescription";
constexpr char         INITIAL_HIGHLIGHT_ATTRIBUTE[]          = "initial-a11y-highlight";
constexpr char         COLLECTION_CONTAINER_ATTRIBUTE[]       = "collection_container";
constexpr char         COLLECTION_INDEX_ATTRIBUTE[]           = "collection_index";

const TraitId ACCESSIBILITY_ACTIVATE_CALLBACK_TRAIT_ID                    = TraitId::Alloc();
const TraitId ACCESSIBILITY_ESCAPE_CALLBACK_TRAIT_ID                      = TraitId::Alloc();
const TraitId ACCESSIBILITY_PAN_CALLBACK_TRAIT_ID                         = TraitId::Alloc();
const TraitId ACCESSIBILITY_VALUE_CHANGE_CALLBACK_TRAIT_ID                = TraitId::Alloc();
const TraitId ACCESSIBILITY_SCROLL_TO_CHILD_CALLBACK_TRAIT_ID             = TraitId::Alloc();
const TraitId ACCESSIBILITY_ZOOM_CALLBACK_TRAIT_ID                        = TraitId::Alloc();
const TraitId ACCESSIBILITY_REQUEST_NAME_CALLBACK_TRAIT_ID                = TraitId::Alloc();
const TraitId ACCESSIBILITY_REQUEST_DEFAULT_NAME_CALLBACK_TRAIT_ID        = TraitId::Alloc();
const TraitId ACCESSIBILITY_REQUEST_DESCRIPTION_CALLBACK_TRAIT_ID         = TraitId::Alloc();
const TraitId ACCESSIBILITY_REQUEST_DEFAULT_DESCRIPTION_CALLBACK_TRAIT_ID = TraitId::Alloc();
const TraitId ACCESSIBILITY_REQUEST_VALUE_CALLBACK_TRAIT_ID               = TraitId::Alloc();

template<typename Signature>
class AccessibilityCallbackObject final : public TraitObject
{
public:
  explicit AccessibilityCallbackObject(Callback<Signature> callback)
  : mCallback(std::move(callback))
  {
  }

  template<typename... Args>
  bool Invoke(Args&&... args)
  {
    return mCallback.Invoke(std::forward<Args>(args)...);
  }

protected:
  ~AccessibilityCallbackObject() override = default;

private:
  Callback<Signature> mCallback;
};

template<typename Signature>
void SetAccessibilityCallback(ViewDataImpl& viewDataImpl, TraitId id, Callback<Signature> callback)
{
  if(!callback)
  {
    viewDataImpl.RemoveTrait(id);
    return;
  }

  IntrusivePtr<TraitObject> object(new AccessibilityCallbackObject<Signature>(std::move(callback)));
  viewDataImpl.SetTrait(id, object);
}

bool IsValidAccessibilityRole(Accessibility::Role role)
{
  const uint32_t value = static_cast<uint32_t>(role);
  return value >= Accessibility::ROLE_START_INDEX && value < static_cast<uint32_t>(Accessibility::Role::MAX_COUNT);
}

bool IsValidAccessibilityRelation(Accessibility::RelationType type)
{
  return static_cast<uint32_t>(type) < static_cast<uint32_t>(Accessibility::RelationType::MAX_COUNT);
}

bool IsValidAccessibilityReadingInfo(Accessibility::ReadingInfo info)
{
  return static_cast<uint32_t>(info) < static_cast<uint32_t>(Accessibility::ReadingInfo::MAX_COUNT);
}

Dali::Integration::Accessibility::ReadingInfoType ToIntegrationReadingInfoType(Accessibility::ReadingInfo info)
{
  using IntegrationReadingInfo = Dali::Integration::Accessibility::ReadingInfoType;
  switch(info)
  {
    case Accessibility::ReadingInfo::NAME:
      return IntegrationReadingInfo::NAME;
    case Accessibility::ReadingInfo::ROLE:
      return IntegrationReadingInfo::ROLE;
    case Accessibility::ReadingInfo::DESCRIPTION:
      return IntegrationReadingInfo::DESCRIPTION;
    case Accessibility::ReadingInfo::STATE:
      return IntegrationReadingInfo::STATE;
    case Accessibility::ReadingInfo::MAX_COUNT:
      break;
  }
  return IntegrationReadingInfo::NAME;
}

bool GetStringAttribute(const Property::Map& attributes, StringView key, std::string& value)
{
  const Property::Value* property = attributes.Find(key);
  return property && GetStdString(*property, value);
}

bool GetBooleanAttribute(const Property::Map& attributes, StringView key)
{
  std::string value;
  return GetStringAttribute(attributes, key, value) && value == "true";
}

Vector4 ToVector4(const Insets& insets)
{
  return Vector4(insets.start, insets.end, insets.top, insets.bottom);
}

LayoutCallbacksObject* GetLayoutCallbacksObject(ViewDataImpl& viewDataImpl)
{
  IntrusivePtr<TraitObject> object = viewDataImpl.GetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS);
  return dynamic_cast<LayoutCallbacksObject*>(object.Get());
}

LayoutCallbacksObject* EnsureLayoutCallbacksObject(ViewDataImpl& viewDataImpl)
{
  auto* object = GetLayoutCallbacksObject(viewDataImpl);
  if(!object)
  {
    IntrusivePtr<TraitObject> newObject(new LayoutCallbacksObject());
    object = static_cast<LayoutCallbacksObject*>(newObject.Get());
    viewDataImpl.SetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS, newObject);
  }
  return object;
}

IntrusivePtr<TraitObject> AsTraitObject(BaseHandle traitHandle)
{
  if(!traitHandle)
  {
    return nullptr;
  }

  return IntrusivePtr<TraitObject>(static_cast<TraitObject*>(traitHandle.GetObjectPtr()));
}

template<typename HandleType>
HandleType GetKnownTraitHandle(const ViewDataImpl& viewDataImpl, TraitId id)
{
  IntrusivePtr<TraitObject> object = viewDataImpl.GetTrait(id);
  return object ? HandleType::DownCast(BaseHandle(static_cast<BaseObject*>(object.Get()))) : HandleType();
}

bool IsSelfOrDescendant(View owner, View target)
{
  if(!owner || !target)
  {
    return false;
  }

  const int32_t ownerId  = owner.GetProperty<int32_t>(Actor::Property::ID);
  const int32_t targetId = target.GetProperty<int32_t>(Actor::Property::ID);
  return ownerId == targetId || owner.FindChildById(targetId);
}

StateEffectTargetTrait GetOrCreateStateEffectTargetTrait(ViewDataImpl& viewDataImpl)
{
  StateEffectTargetTrait trait = GetKnownTraitHandle<StateEffectTargetTrait>(viewDataImpl, Integration::ReservedTraitId::STATE_EFFECT_TARGET);
  if(!trait)
  {
    trait = StateEffectTargetTrait::New();
    viewDataImpl.SetTrait(Integration::ReservedTraitId::STATE_EFFECT_TARGET, AsTraitObject(trait));
  }
  return trait;
}

bool GetInsetsFromPropertyValue(const Property::Value& value, Insets& insets)
{
  Vector4 vectorValue;
  if(value.Get(vectorValue))
  {
    insets = Insets(vectorValue.x, vectorValue.y, vectorValue.z, vectorValue.w);
    return true;
  }

  Extents extentsValue;
  if(value.Get(extentsValue))
  {
    insets = Insets(extentsValue);
    return true;
  }

  return false;
}

void ResetStateEffect(ViewDataImpl& viewDataImpl, StateEffect effect)
{
  viewDataImpl.RemoveTrait(Integration::ReservedTraitId::STATE_EFFECT);

  if(effect)
  {
    viewDataImpl.SetTrait(Integration::ReservedTraitId::STATE_EFFECT, AsTraitObject(effect));
  }

  viewDataImpl.RefreshDefaultFocusIndicatorSuppression();
}

View FindStateEffectTarget(View owner, int32_t targetId)
{
  if(!owner || targetId == StateEffectTargetTraitImpl::INVALID_TARGET_ID)
  {
    return View();
  }

  if(owner.GetProperty<int32_t>(Actor::Property::ID) == targetId)
  {
    return owner;
  }

  return View::DownCast(owner.FindChildById(targetId));
}

void ArrangeStandaloneChild(ViewImpl& owner, ViewImpl& childImpl, float parentFullWidth, float parentFullHeight, bool slotUnconsumed)
{
  // Snapshotted BEFORE the re-measures below: the available extent and the slot
  // derivation at the end must be fed the same values, even if the child's own measure
  // producer mutates its scale, margin or requested size mid-pass.
  const StandaloneSlotInputs inputs = SnapshotStandaloneSlotInputs(childImpl);

  const float marginW = static_cast<float>(inputs.margin.start + inputs.margin.end) * inputs.scale;
  const float marginH = static_cast<float>(inputs.margin.top + inputs.margin.bottom) * inputs.scale;

  // The extent this parent makes available to the child: the parent's own final
  // size less the child's margin. It is the constraint BOTH re-measures below use,
  // and for a WRAP_CONTENT / MATCH_PARENT child it is also the one
  // LayoutController::ProcessLayoutRoot derives (parent SIZE - margin) when the
  // same view is driven as a layout root in its own right -- so a standalone root
  // takes a measure cache HIT here rather than re-running its producer.
  //
  // The ARRANGE bounds now converge for EVERY shape, because both paths end on the
  // same DeriveStandaloneRootBounds() call. What is still divergent is the MEASURE
  // constraint, and the divergent term is WHICH parent extent it is taken from:
  // MeasureStandaloneChildren measures a standalone child against the parent's INCOMING
  // constraint (visEffW/visEffH), while this function measures it against the parent's
  // ARRANGED extent (availW/availH). A MATCH axis converges regardless, but only
  // because the re-measure below rewrites the measure cache KEY last, at the extent the
  // child is actually placed in. A FIXED or WRAP axis diverges whenever the two parent
  // extents differ, so an unconsumed pass may re-run the child's producer once here.
  // There is no geometry change (the slot is derived from the snapshot above, not from
  // this re-measure) and no thrash, so unification stays out. The candidates considered
  // and rejected:
  //   - re-measure here against the parent's incoming constraint instead: changes the
  //     constraint every existing standalone producer is run at, even at s == 1;
  //   - measure a FIXED-size root at the parent extent in ProcessLayoutRoot rather than
  //     at its requested size: violates the constraint-priority chain, in which an
  //     explicit requested size outranks the parent extent;
  //   - drive MeasureStandaloneChildren from the parent's own measured size so that
  //     both sites agree: a MATCH_PARENT parent measures to min * s, so its standalone
  //     children would be measured against that and collapse.
  const float availW = std::max(0.0f, parentFullWidth - marginW);
  const float availH = std::max(0.0f, parentFullHeight - marginH);

  // Both Measure() calls below are arrange-time producers issued by this parent, so
  // each carries an owner scope like every other arrange-owned re-measure (owner ==
  // the arranging parent, threaded in because this is a free function). For the
  // standalone child's OWN measure the owner frame is never the ancestor walk's stop:
  // the walk starts at the measured view's DIRECT parent -- which IS the owner
  // recorded here -- but reaching the owner test requires first getting past the
  // self-standalone early return, which only a standalone view takes, and only
  // standalone children ever get here; the two are contradictory. (Were a producer
  // here to reach out and measure some OTHER, non-standalone child of this parent,
  // that child's walk would meet the owner on its first node -- but the direct-parent
  // arrange-in-progress safety net stops at that very same node, so behaviour is
  // identical either way.) The scopes are kept regardless: they keep every
  // arrange-owned re-measure uniform (nothing has to special-case this site), and
  // their identity is pinned by a white-box owner test rather than by walk behaviour
  // -- UtcDaliLayoutDependencyStandaloneArrangeOwnerIdentityP.
  //
  // The corrective re-measure. A standalone child is excluded from every ancestor's
  // measure accumulation, so the ancestor-invalidation walk deliberately returns
  // early for it (InvalidateAncestorLayoutCachesForMeasureMiss) and the parent keeps
  // serving its measure cache HIT -- which means MeasureStandaloneChildren does not
  // re-run and the slot left behind by an out-of-band View::Measure() would be
  // arranged as-is. The unconsumed bit says exactly that: nothing has consumed the
  // current slot, so re-measure it here, at the extent it is about to be placed in.
  // Skipped when the child is MATCH_PARENT on BOTH axes, because then the measured
  // value is discarded on both axes anyway and the re-measure below already runs at
  // exactly this constraint (the first-party standalone views -- ScrollBar and the
  // focus indicator -- are that shape, so this is a strict no-op for them).
  if(slotUnconsumed && !(inputs.matchWidth && inputs.matchHeight))
  {
    LayoutDependency::ArrangeOwnedMeasureScope ownerScope(&owner);
    childImpl.Measure(availW, availH);
  }

  const MeasuredSize measured = childImpl.GetMeasuredSize();
  const float        childW   = ResolveStandaloneExtent(inputs.matchWidth, availW, measured.width);
  const float        childH   = ResolveStandaloneExtent(inputs.matchHeight, availH, measured.height);

  // A MATCH_PARENT axis is placed at the parent's extent rather than at the measured
  // size, so the child is re-measured against the size it will actually get. The
  // UNCLAMPED extent is used deliberately: ProcessLayoutRoot re-measures a boundary
  // root at the same unclamped value, so the measure cache KEY converges too.
  if(inputs.matchWidth || inputs.matchHeight)
  {
    LayoutDependency::ArrangeOwnedMeasureScope ownerScope(&owner);
    childImpl.Measure(childW, childH);
  }

  // THE shared derivation, the same call LayoutController::ProcessLayoutRoot makes for
  // a boundary view driven as a layout root -- which is what makes the two results
  // converge by construction. It re-resolves the extents from the `measured` SNAPSHOT
  // taken above, so the re-measure in between cannot change them, and it applies the
  // child's own min/max clamp: that clamp is new on this path and is exactly what makes
  // it identical to ProcessLayoutRoot.
  const LayoutRect bounds = DeriveStandaloneRootBounds(inputs, availW, availH, measured);
  childImpl.Arrange(bounds);
}

/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if action has been accepted by this view
 */
constexpr const char* ACTION_ACCESSIBILITY_ACTIVATE  = "activate";
constexpr const char* ACTION_ACCESSIBILITY_ESCAPE    = "escape";
constexpr const char* ACTION_ACCESSIBILITY_INCREMENT = "increment";
constexpr const char* ACTION_ACCESSIBILITY_DECREMENT = "decrement";

// Legacy actions
constexpr const char* ACTION_ACCESSIBILITY_READING_STARTED   = "ReadingStarted";
constexpr const char* ACTION_ACCESSIBILITY_READING_CANCELLED = "ReadingCancelled";
constexpr const char* ACTION_ACCESSIBILITY_READING_PAUSED    = "ReadingPaused";
constexpr const char* ACTION_ACCESSIBILITY_READING_RESUMED   = "ReadingResumed";
constexpr const char* ACTION_ACCESSIBILITY_READING_SKIPPED   = "ReadingSkipped";
constexpr const char* ACTION_ACCESSIBILITY_READING_STOPPED   = "ReadingStopped";

constexpr int INNER_SHADOW_DEPTH_INDEX = Dali::Ui::Integration::DepthIndex::DECORATION - 1;
constexpr int BORDERLINE_DEPTH_INDEX   = Dali::Ui::Integration::DepthIndex::FOREGROUND_EFFECT - 1;

inline bool FloatEqual(float a, float b, float epsilon = 0.001f)
{
  return std::abs(a - b) < epsilon;
}

// THE single place the global UI-scale master switch is read. Every scale that
// enters the layout system does so through one of the two calls to this function
// in ComputeEffectiveScale() -- every other Measure/Arrange site, every
// measure-cache key (mLastMeasureScale) and the actor-side VIEW_EFFECTIVE_SCALE
// push obtain the scale via GetEffectiveScale(), which memoizes what
// ComputeEffectiveScale() returns. So collapsing the system scale to 1.0f here
// is what makes the whole system behave as unscaled, with no change at any call
// site. The scale stored in UiScaleManager is left untouched and is re-applied
// as soon as scaling is re-enabled.
//
// Reading the switch HERE rather than at the top of ComputeEffectiveScale() is
// what keeps the two policies that never consult the system scale -- DISABLED,
// and INHERIT with a parent -- free of the handle's refcount touch, exactly as
// they were before the switch existed.
inline float GetSystemScale()
{
  UiScaleManager manager = UiScaleManager::Get();
  return manager.IsScalable() ? manager.GetScale() : 1.0f;
}

// Sanitize size property inputs. RequestedWidth/Height accept any
// non-negative value plus the special values WRAP_CONTENT (-1.0f) and
// MATCH_PARENT (-2.0f). NaN/Inf and other negative values are rejected.
inline bool IsValidRequestedSize(float v)
{
  return std::isfinite(v) && (v >= 0.0f || FloatEqual(v, WRAP_CONTENT) || FloatEqual(v, MATCH_PARENT));
}

// Minimum/Maximum bounds must be finite and non-negative.
inline bool IsValidSizeBound(float v)
{
  return std::isfinite(v) && v >= 0.0f;
}

// A valid arrange input/return LayoutRect: finite x/y (negative allowed),
// finite non-negative width/height.
inline bool IsValidLayoutRect(const LayoutRect& b)
{
  return std::isfinite(b.x) && std::isfinite(b.y) && IsValidSizeBound(b.width) && IsValidSizeBound(b.height);
}

// Equality of two arrange rects, used as the arrange cache KEY comparison.
//
// EXACT, deliberately not an epsilon compare (LayoutRect has no operator==, so
// this is spelled out here). The arrange cache serves a stored rect in place of
// re-running a producer, and the same rect is then written to the actor by
// ApplySelfBoundsIfChanged, whose write suppression is itself an exact `!=`
// test. An epsilon key would let a sub-epsilon slot change hit the cache and
// then be silently applied as the OLD geometry, i.e. a wrong result rather than
// a missed optimisation. Exact matching keeps the two tests in agreement.
//
// NaN-safe by construction: a NaN field compares false against everything,
// including itself, so a NaN never produces a hit. (IsValidLayoutRect rejects
// NaN inputs anyway, but only in DEBUG.)
inline bool SameLayoutRect(const LayoutRect& a, const LayoutRect& b)
{
  return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

// The returned rect is a new trust boundary: a customization hook may return an
// invalid rect. Valid returns are adopted whole; invalid returns fall back to
// the full input rect (no per-field clamp, no min/max reapply).
inline LayoutRect ResolveReturnedBounds(const LayoutRect& inputBounds, const LayoutRect& returnedBounds)
{
  if(IsValidLayoutRect(returnedBounds))
  {
    return returnedBounds;
  }
  DALI_LOG_ERROR("Invalid LayoutRect returned from arrange customization; falling back to input bounds\n");
  DALI_ASSERT_DEBUG(IsValidLayoutRect(returnedBounds));
  return inputBounds;
}

// RAII: sets a flag true for the enclosing scope, restoring on exit (including
// exception/assertion unwind). Used to guard against same-view re-entrant key
// event dispatch. The layout passes use the richer MeasurePassGuard /
// ArrangePassGuard below, which additionally own the pass-local cache state.
struct ScopedTrueFlag
{
  bool& mFlag;
  explicit ScopedTrueFlag(bool& flag)
  : mFlag(flag)
  {
    mFlag = true;
  }
  ~ScopedTrueFlag()
  {
    mFlag = false;
  }
  ScopedTrueFlag(const ScopedTrueFlag&)            = delete;
  ScopedTrueFlag& operator=(const ScopedTrueFlag&) = delete;
};

class ScopedKeyEventDispatch
{
public:
  explicit ScopedKeyEventDispatch(CoreInteractionObject* coreInteractionObject)
  : mCoreInteractionObject(coreInteractionObject)
  {
  }

  ~ScopedKeyEventDispatch()
  {
    if(mCoreInteractionObject)
    {
      mCoreInteractionObject->CancelKeyEventDispatch();
    }
  }

  ScopedKeyEventDispatch(const ScopedKeyEventDispatch&)            = delete;
  ScopedKeyEventDispatch& operator=(const ScopedKeyEventDispatch&) = delete;

private:
  CoreInteractionObject* mCoreInteractionObject{nullptr};
};

static constexpr uint32_t INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 10);
static constexpr uint32_t BORDERLINE_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 11);

static constexpr uint32_t BORDERLINE_WIDTH_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 12);
static constexpr uint32_t BORDERLINE_COLOR_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 13);
static constexpr uint32_t BORDERLINE_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 14);

bool PerformAccessibilityAction(Ui::View view, const Dali::String& actionName)
{
  DALI_ASSERT_DEBUG(view);

  auto& viewImpl = GetImpl(view);
  bool  success  = false;
  if(actionName == ACTION_ACCESSIBILITY_ACTIVATE)
  {
    success = ViewDataImpl::Get(viewImpl).DispatchAccessibilityActivate();
  }
  else if(actionName == ACTION_ACCESSIBILITY_ESCAPE)
  {
    success = ViewDataImpl::Get(viewImpl).DispatchAccessibilityEscape();
  }
  else if(actionName == ACTION_ACCESSIBILITY_INCREMENT)
  {
    success = ViewDataImpl::Get(viewImpl).DispatchAccessibilityValueChange(true);
  }
  else if(actionName == ACTION_ACCESSIBILITY_DECREMENT)
  {
    success = ViewDataImpl::Get(viewImpl).DispatchAccessibilityValueChange(false);
  }
  else
  {
    return false;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: %s, success : %d\n", actionName.CStr(), success);
  return success;
}

bool PerformAccessibilityReadingStatus(Ui::View view, const Dali::String& actionName)
{
  auto&                        viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(GetImpl(view));
  Accessibility::ReadingStatus status;
  if(actionName == ACTION_ACCESSIBILITY_READING_STARTED)
  {
    status = Accessibility::ReadingStatus::STARTED;
  }
  else if(actionName == ACTION_ACCESSIBILITY_READING_SKIPPED)
  {
    status = Accessibility::ReadingStatus::SKIPPED;
  }
  else if(actionName == ACTION_ACCESSIBILITY_READING_PAUSED)
  {
    status = Accessibility::ReadingStatus::PAUSED;
  }
  else if(actionName == ACTION_ACCESSIBILITY_READING_RESUMED)
  {
    status = Accessibility::ReadingStatus::RESUMED;
  }
  else if(actionName == ACTION_ACCESSIBILITY_READING_CANCELLED)
  {
    status = Accessibility::ReadingStatus::CANCELLED;
  }
  else if(actionName == ACTION_ACCESSIBILITY_READING_STOPPED)
  {
    status = Accessibility::ReadingStatus::STOPPED;
  }
  else
  {
    DALI_LOG_RELEASE_INFO("[ReadingStartedTrace][DALiUI] action rejected: unknown action=%s\n", actionName.CStr());
    return false;
  }

  auto& readingStatusSignal = viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingStatusChangedSignal;
  DALI_LOG_RELEASE_INFO("[ReadingStartedTrace][DALiUI] emit begin: action=%s status=%u view=%p connections=%u\n",
                        actionName.CStr(),
                        static_cast<uint32_t>(status),
                        static_cast<void*>(view.GetObjectPtr()),
                        readingStatusSignal.GetConnectionCount());
  readingStatusSignal.Emit(view, status);
  DALI_LOG_RELEASE_INFO("[ReadingStartedTrace][DALiUI] emit end: action=%s status=%u\n",
                        actionName.CStr(),
                        static_cast<uint32_t>(status));
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Changed accessibility reading status: %s\n", actionName.CStr());
  return true;
}

bool DoAccessibilityAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);
  Ui::View         view = Ui::View::DownCast(handle);

  DALI_ASSERT_ALWAYS(view);
  return PerformAccessibilityAction(view, actionName);
}

bool DoAccessibilityReadingStatusAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);
  Ui::View         view = Ui::View::DownCast(handle);

  DALI_LOG_RELEASE_INFO("[ReadingStartedTrace][DALiUI] TypeAction received: action=%s object=%p validView=%d\n",
                        actionName.CStr(),
                        static_cast<void*>(object),
                        view ? 1 : 0);
  DALI_ASSERT_ALWAYS(view);
  return PerformAccessibilityReadingStatus(view, actionName);
}

const char* SIGNAL_KEY_EVENT     = "keyEvent";
const char* SIGNAL_FOCUS_CHANGED = "focusChanged";
const char* SIGNAL_DO_GESTURE    = "doGesture";

/**
 * Connects a callback function with the object's signals.
 * @param[in] object The object providing the signal.
 * @param[in] tracker Used to disconnect the signal.
 * @param[in] signalName The signal to connect to.
 * @param[in] functor A newly allocated FunctorDelegate.
 * @return True if the signal was connected.
 * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible
 * for deleting the unused functor.
 */
static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName,
                            FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool     connected(false);
  Ui::View view = Ui::View::DownCast(handle);
  if(view)
  {
    ViewImpl& viewImpl(GetImpl(view));
    auto&     viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(viewImpl);
    connected              = true;

    if(0 == strcmp(signalName.CStr(), SIGNAL_KEY_EVENT))
    {
      viewImpl.KeyEventSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_FOCUS_CHANGED))
    {
      viewImpl.FocusChangedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_DO_GESTURE))
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityDoGestureSignal.Connect(tracker, functor);
    }
  }
  return connected;
}

/**
 * Creates view through type registry
 */
BaseHandle Create()
{
  ViewImplPtr impl = ViewImpl::New();
  View        handle(*impl);
  return handle;
}
// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN_FULL(Ui::View, Ui::ViewImpl, Dali::CustomActor, Create);

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1(typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal);
SignalConnectorType registerSignal2(typeRegistration, SIGNAL_FOCUS_CHANGED, &DoConnectSignal);
SignalConnectorType registerSignal9(typeRegistration, SIGNAL_DO_GESTURE, &DoConnectSignal);

// === Accessibility Actions === START
TypeAction registerAction1(typeRegistration, ACTION_ACCESSIBILITY_ACTIVATE, &DoAccessibilityAction);
TypeAction registerAction2(typeRegistration, ACTION_ACCESSIBILITY_ESCAPE, &DoAccessibilityAction);
TypeAction registerAction3(typeRegistration, ACTION_ACCESSIBILITY_INCREMENT, &DoAccessibilityAction);
TypeAction registerAction4(typeRegistration, ACTION_ACCESSIBILITY_DECREMENT, &DoAccessibilityAction);
// === Accessibility Actions === END

// === Legacy Accessibility Actions === START
TypeAction registerAction5(typeRegistration, ACTION_ACCESSIBILITY_READING_SKIPPED, &DoAccessibilityReadingStatusAction);
TypeAction registerAction6(typeRegistration, ACTION_ACCESSIBILITY_READING_CANCELLED, &DoAccessibilityReadingStatusAction);
TypeAction registerAction7(typeRegistration, ACTION_ACCESSIBILITY_READING_STOPPED, &DoAccessibilityReadingStatusAction);
TypeAction registerAction8(typeRegistration, ACTION_ACCESSIBILITY_READING_PAUSED, &DoAccessibilityReadingStatusAction);
TypeAction registerAction9(typeRegistration, ACTION_ACCESSIBILITY_READING_RESUMED, &DoAccessibilityReadingStatusAction);
TypeAction registerAction10(typeRegistration, ACTION_ACCESSIBILITY_READING_STARTED, &DoAccessibilityReadingStatusAction);
// === Legacy Accessibility Actions === END

DALI_TYPE_REGISTRATION_END()

Internal::CoreInteractionObject* AsCoreInteractionObject(TraitObject* object)
{
  return object ? dynamic_cast<Internal::CoreInteractionObject*>(object) : nullptr;
}

uint32_t ViewAccessibilityStateToMask(Accessibility::State state)
{
  return 1u << static_cast<uint32_t>(state);
}

Dali::Integration::Accessibility::RelationType ToIntegrationRelationType(Dali::Ui::Accessibility::RelationType relation)
{
  return static_cast<Dali::Integration::Accessibility::RelationType>(relation);
}

/// Field-wise equality for the four per-child layout params value types, used by
/// ViewDataImpl::SetLayoutParams to drop a write that changes nothing.
///
/// Defined here rather than as a public operator== on the params types: those are
/// pimpl value types in the public API, and adding operators to them is an API
/// change this does not need. Every public field is compared, so an addition to
/// any params type has to be mirrored here.
///
/// Floats are compared EXACTLY (`==`), not through FloatEqual. These functions answer
/// "is the stored value the value being written?", which is a question about VALUE
/// IDENTITY, not about whether two numbers are close enough to behave the same. A
/// tolerance here silently drops a real state change:
///  - StackLayoutParams::SetWeight and FlexLayoutParams::SetFlexGrow document 0 as a
///    distinct MODE ("measured normally" vs "sized entirely from the weight/grow
///    proportion"), so 0 -> 0.0005 is a mode switch that a 0.001 epsilon would eat;
///  - AbsoluteLayoutParams bounds are PROPORTIONS of the parent under the
///    *_PROPORTIONAL flags, so their whole meaningful range is 0..1 and 0.001 is a
///    tenth of a percent of the parent -- pixels on any real surface.
/// The property setters further down this file do use FloatEqual, and correctly: they
/// compare pixel-space sizes against a stored pixel-space size. Nothing about their
/// choice transfers here.
bool IsSameLayoutParams(const AbsoluteLayoutParams& lhs, const AbsoluteLayoutParams& rhs)
{
  const LayoutRect lhsBounds = lhs.GetBounds();
  const LayoutRect rhsBounds = rhs.GetBounds();
  return lhsBounds.x == rhsBounds.x &&
         lhsBounds.y == rhsBounds.y &&
         lhsBounds.width == rhsBounds.width &&
         lhsBounds.height == rhsBounds.height &&
         lhs.GetFlags() == rhs.GetFlags();
}

bool IsSameLayoutParams(const FlexLayoutParams& lhs, const FlexLayoutParams& rhs)
{
  return lhs.GetFlexGrow() == rhs.GetFlexGrow() &&
         lhs.GetFlexShrink() == rhs.GetFlexShrink() &&
         lhs.GetFlexBasis() == rhs.GetFlexBasis() &&
         lhs.GetAlignSelf() == rhs.GetAlignSelf();
}

bool IsSameLayoutParams(const GridLayoutParams& lhs, const GridLayoutParams& rhs)
{
  return lhs.GetRow() == rhs.GetRow() &&
         lhs.GetColumn() == rhs.GetColumn() &&
         lhs.GetRowSpan() == rhs.GetRowSpan() &&
         lhs.GetColumnSpan() == rhs.GetColumnSpan() &&
         lhs.GetHorizontalAlignment() == rhs.GetHorizontalAlignment() &&
         lhs.GetVerticalAlignment() == rhs.GetVerticalAlignment();
}

bool IsSameLayoutParams(const StackLayoutParams& lhs, const StackLayoutParams& rhs)
{
  return lhs.GetWeight() == rhs.GetWeight() &&
         lhs.GetAlignment() == rhs.GetAlignment();
}

/// How many Measure()/Arrange() passes -- on ANY view -- are currently on the stack.
///
/// Maintained by MeasurePassGuard / ArrangePassGuard below, and read by
/// InvalidateMeasure() / InvalidateArrange() for one purpose: while it is non-zero,
/// the invalidation propagation generation short-circuit is DISABLED and every
/// invalidation walks its ancestor chain in full.
///
/// That is not conservatism, it is required. The generation says "the walk this call would
/// make has already been made and its registration is still pending", which is a
/// statement about the layout ROOT. It is not a statement about the intermediate
/// ancestors, and mid-pass the walk does more than register: it sets each ancestor's
/// dirty bit and POISONS any ancestor whose pass is currently running, which is what
/// stops that ancestor publishing a cache entry over a subtree that has just changed
/// underneath it. An ancestor can start its pass AFTER an earlier walk in the same
/// generation consumed that ancestor's dirty, so a later short-circuited invalidation would
/// leave it un-poisoned. Skipping the walk is only safe when no pass is running.
///
/// Thread-local, matching LayoutDependency's owner stack and for the same reason: a
/// layout pass and every nested Measure()/Arrange() it issues run synchronously on the
/// same (event) thread, so a per-thread counter is the accurate description of "is a
/// pass on MY stack". A shared counter would additionally fail in the dangerous
/// direction if a pass ever ran off-thread -- reading zero while a pass is live is
/// what re-enables the short-circuit mid-pass.
thread_local uint32_t gActiveLayoutPassDepth = 0u;

} // unnamed namespace

/**
 * RAII transaction guard for one Measure() pass on a single view.
 *
 * Entry establishes the pass-local state:
 *  - mMeasureInProgress marks this view's Measure() as being on the stack, so a
 *    re-entrant call on the SAME view is detected in release builds too.
 *  - mMeasurePassPoisoned is cleared here rather than at publish time, so poison
 *    is strictly PER-PASS instead of latching across passes. The cache-hit test
 *    runs before the guard is constructed, so a poison raised by the previous
 *    pass still forces this pass to miss.
 *  - mMeasureDirty is CONSUMED here: this pass exists precisely to service the
 *    invalidation that raised it. Consuming at entry (rather than clearing at
 *    publish) is what makes a re-invalidation raised DURING the pass survive it:
 *    the flag goes false(entry) -> true(re-invalidation) and the conditional
 *    publish then declines to cache, so the next Measure() misses and recomputes
 *    the post-invalidation value instead of pinning the pre-invalidation one.
 *  - mMeasureCacheValid is cleared so nothing can reuse a half-produced result
 *    while the pass is running.
 *  - mArrangeCacheValid is cleared because a full Measure can change this view's
 *    measured size, which is an input to its own arrangement; an arrange result
 *    produced against the previous measurement must not survive it
 *    (plan34 3.1, measure invariant 8).
 *
 * Entry sets mMeasureCacheValid = false, so a pass left without reaching its
 * publish point (exception, assertion unwind, early return) is never treated as
 * a valid cached result: the next Measure() simply misses and recomputes. That
 * abnormal exit also leaves mMeasureDirty consumed. No stale result can be
 * served (the invalid cache forces the recompute), and any later invalidation
 * still propagates and registers because InvalidateMeasure() has no
 * already-dirty short-circuit. The only thing lost versus re-arming dirty is the
 * standalone-view scene-reconnect self-registration (OnViewSceneConnection's
 * isDirty branch); it only matters after an exception aborts a pass, and a
 * standalone view is re-driven by its parent's MeasureStandaloneChildren and
 * re-invalidated by any reparent, so the abort recovers. Exit only
 * restores mMeasureInProgress; it must NOT re-arm mMeasureDirty, because the
 * guard cannot also register the view with the LayoutController, so a re-armed
 * view would sit dirty with no pass scheduled to consume it until some
 * unrelated invalidation happened to arrive. The destructor must stay
 * non-throwing: it can run during unwinding, so it must not call into
 * LayoutController (whose accessors assert).
 */
struct ViewDataImpl::MeasurePassGuard
{
  explicit MeasurePassGuard(ViewDataImpl& data)
  : mData(data)
  {
    ++gActiveLayoutPassDepth;
    mData.mMeasureInProgress   = true;
    mData.mMeasureDirty        = false;
    mData.mMeasurePassPoisoned = false;
    mData.mMeasureCacheValid   = false;
    mData.mArrangeCacheValid   = false;
  }

  ~MeasurePassGuard()
  {
    mData.mMeasureInProgress = false;

    // Leaving the OUTERMOST pass ends the invalidation propagation generation, exactly as
    // the controller's drain does. Load-bearing, not tidiness: a pass is the only
    // thing that consumes dirty bits, so a generation record written BEFORE a pass no
    // longer proves "the chain I walked is still marked" AFTER one -- a manual
    // Measure()/Arrange() on an ancestor (both are public API) can consume the whole
    // chain's dirty without any drain. Ending the generation here makes the next
    // invalidation walk in full.
    //
    // "Outermost" is decided by gActiveLayoutPassDepth, which an arrange cache-HIT
    // REPLAY also holds (ReplayPassScope). A replay consumes no dirty of its own, so it
    // never creates the staleness this ends -- but it can HOST a re-entered producer pass
    // that does, and that inner guard would see the depth fall to 1 rather than 0. Every
    // frame that can be outermost therefore ends the generation, and a settled steady
    // state pays at most one extra full ancestor walk on the next invalidation.
    if(--gActiveLayoutPassDepth == 0u)
    {
      LayoutInvalidation::AdvanceGeneration();
    }
  }

  MeasurePassGuard(const MeasurePassGuard&)            = delete;
  MeasurePassGuard& operator=(const MeasurePassGuard&) = delete;

  ViewDataImpl& mData;
};

/**
 * RAII transaction guard for one Arrange() pass on a single view.
 *
 * Mirrors MeasurePassGuard over the arrange axis and additionally clears
 * mEffectiveScaleInvalidatedDuringPass, which is likewise pass-local: it records
 * that a reparent / scale-context reset invalidated the cached effective scale WHILE
 * this pass was running, and mArrangeCacheBlockedDuringPass, the pass-local
 * record of a FRESHNESS-only invalidation (a cache-only ancestor drop, which
 * raises no dirty bit and schedules nothing). The latter blocks this pass's
 * cache publish but must stay invisible to the pure-poison follow-up branch:
 * a cache-only invalidation must never turn into a scheduled layout.
 *
 * As with the measure guard, mArrangeDirty is CONSUMED at entry, so an
 * InvalidateArrange() raised while this pass is running survives it and blocks
 * the conditional publish at the end. The destructor only restores
 * mArrangeInProgress and must not re-arm mArrangeDirty (same reason as
 * MeasurePassGuard) nor throw.
 *
 * The arrange cache-HIT test runs BEFORE this guard is constructed (again
 * mirroring the measure side): entry clears mArrangeCacheValid and consumes the
 * dirty / poison bits, so constructing the guard first would destroy the very
 * cache entry the hit is there to serve, and would hide a poison left by the
 * previous pass.
 */
struct ViewDataImpl::ArrangePassGuard
{
  explicit ArrangePassGuard(ViewDataImpl& data)
  : mData(data)
  {
    ++gActiveLayoutPassDepth;
    mData.mArrangeInProgress                   = true;
    mData.mArrangeDirty                        = false;
    mData.mArrangePassPoisoned                 = false;
    mData.mArrangeCacheBlockedDuringPass       = false;
    mData.mEffectiveScaleInvalidatedDuringPass = false;
    mData.mArrangeCacheValid                   = false;
  }

  ~ArrangePassGuard()
  {
    mData.mArrangeInProgress = false;

    // See MeasurePassGuard: leaving the outermost pass ends the propagation generation,
    // so no generation record can outlive the dirty bits its walk set.
    if(--gActiveLayoutPassDepth == 0u)
    {
      LayoutInvalidation::AdvanceGeneration();
    }
  }

  ArrangePassGuard(const ArrangePassGuard&)            = delete;
  ArrangePassGuard& operator=(const ArrangePassGuard&) = delete;

  ViewDataImpl& mData;
};

/**
 * RAII transaction scope for one arrange cache-HIT replay SUBTREE.
 *
 * Constructed exactly once, at the hit site in ArrangeImpl, and it owns nothing but the
 * thread-local layout-pass depth. Holding that depth open for the whole replay is what
 * puts a replay INSIDE the layout processing window:
 *
 *  - the replay WRITES actor properties (ApplySelfBoundsIfChanged), and every such write
 *    goes Actor::SetPositionX/SetWidth -> Object::SetProperty -> OnPropertySet plus a
 *    synchronous PropertySetSignal emit, so arbitrary application code runs inside it;
 *  - an invalidation raised by that code must therefore be PARKED, not allowed to arm an
 *    idle wake. LayoutController::RequestIdleWakeIfAllowed reads exactly this depth
 *    (ViewDataImpl::IsLayoutPassOnStack). Before this scope existed the hit path ran with
 *    depth 0 and such an invalidation woke the loop from inside layout processing;
 *  - the same depth disables the propagation-generation short-circuit in
 *    InvalidateMeasure/InvalidateArrange, so a mid-replay invalidation walks its ancestor
 *    chain IN FULL and poisons every in-progress ancestor rather than trusting a
 *    registration recorded before the replay began.
 *
 * NOT ArrangePassGuard: that guard's entry consumes mArrangeDirty and clears
 * mArrangeCacheValid -- the very entry this hit exists to serve. A replay must leave the
 * cache, the dirty bits and the poison bits exactly as it found them, so that an
 * invalidation raised mid-replay survives the unwind untouched.
 */
struct ViewDataImpl::ReplayPassScope
{
  ReplayPassScope()
  {
    ++gActiveLayoutPassDepth;
  }

  ~ReplayPassScope()
  {
    // The generation is ended by whichever frame is OUTERMOST, and a replay can be that
    // frame (the controller's arrange step reaches the hit with the measure guard already
    // unwound). A replay of its own consumes no dirty bit and would not need to end the
    // generation -- but a producer pass RE-ENTERED from a property-set observer inside the
    // replay does consume one, and its own guard sees the depth fall to 1, not 0. Ending
    // it here is what keeps "no generation record outlives the dirty bits its walk set"
    // true for that nesting. The cost is at most one extra full ancestor walk on the next
    // invalidation, which is idempotent; the alternative failure mode is a swallowed
    // registration.
    if(--gActiveLayoutPassDepth == 0u)
    {
      LayoutInvalidation::AdvanceGeneration();
    }
  }

  ReplayPassScope(const ReplayPassScope&)            = delete;
  ReplayPassScope& operator=(const ReplayPassScope&) = delete;
};

/**
 * RAII scope for ONE node visited by an arrange cache-HIT replay.
 *
 * Raises mArrangeInProgress for the whole of that node's replay -- its self apply, its
 * child recursion and its LayoutFinished registration -- so the release-mode re-entrancy
 * guard at the top of ArrangeImpl covers a replay exactly as it covers a producer pass.
 * Without it a PropertySetSignal observer woken by the self apply could call Arrange() on
 * the very view being replayed, re-run its producer underneath the replay and publish a
 * result the replay then writes over.
 *
 * mArrangeReplayInProgress rides alongside so the three OWNERSHIP tests that read
 * mArrangeInProgress can tell a replay from a producer; see that member's documentation.
 *
 * SAVE/RESTORE rather than set/clear: the same shape ManualProcessScope and
 * ActiveLayoutFinishedScope use in LayoutController, and it makes the scope correct under
 * a nesting the gate is believed to rule out. The DEBUG assert records that belief -- a
 * descendant whose own arrange pass is running had mArrangeCacheValid cleared by
 * ArrangePassGuard at entry, and CanReplayArrangeSubtreeFromCache refuses any subtree
 * containing such a node -- while release stays correct if it is ever broken.
 */
struct ViewDataImpl::ReplayNodeScope
{
  explicit ReplayNodeScope(ViewDataImpl& data)
  : mData(data),
    mPreviousInProgress(data.mArrangeInProgress),
    mPreviousReplay(data.mArrangeReplayInProgress)
  {
    DALI_ASSERT_DEBUG(!mData.mArrangeInProgress &&
                      "a replay must not visit a view whose own arrange pass is running");
    mData.mArrangeInProgress       = true;
    mData.mArrangeReplayInProgress = true;
  }

  ~ReplayNodeScope()
  {
    mData.mArrangeReplayInProgress = mPreviousReplay;
    mData.mArrangeInProgress       = mPreviousInProgress;
  }

  ReplayNodeScope(const ReplayNodeScope&)            = delete;
  ReplayNodeScope& operator=(const ReplayNodeScope&) = delete;

  ViewDataImpl& mData;
  bool          mPreviousInProgress;
  bool          mPreviousReplay;
};

// clang-format off
// Properties registered without macro to use specific member variables.
const PropertyRegistration ViewDataImpl::PROPERTY_5(typeRegistration,  "background",                     Ui::View::Property::BACKGROUND,                       Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_6(typeRegistration,  "margin",                         Ui::View::Property::MARGIN,                           Property::VECTOR4, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_7(typeRegistration,  "padding",                        Ui::View::Property::PADDING,                          Property::VECTOR4, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_11(typeRegistration, "leftFocusableViewId",           Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_12(typeRegistration, "rightFocusableViewId",          Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID,         Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_13(typeRegistration, "upFocusableViewId",             Ui::View::Property::UP_FOCUSABLE_VIEW_ID,            Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_14(typeRegistration, "downFocusableViewId",           Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_15(typeRegistration, "shadow",                         Ui::View::Property::SHADOW,                           Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_22(typeRegistration, "dispatchKeyEvents",              Ui::View::Property::DISPATCH_KEY_EVENTS,              Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_24(typeRegistration, "clockwiseFocusableViewId",      Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID,     Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_25(typeRegistration, "counterClockwiseFocusableViewId", Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID, Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_31(typeRegistration, "offScreenRendering",             Ui::View::Property::OFFSCREEN_RENDERING,              Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_32(typeRegistration, "innerShadow",                    Ui::View::Property::INNER_SHADOW,                     Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_33(typeRegistration, "borderline",                     Ui::View::Property::BORDERLINE,                       Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_34(typeRegistration, "requestedWidth",                 Ui::View::Property::REQUESTED_WIDTH,                  Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_35(typeRegistration, "requestedHeight",                Ui::View::Property::REQUESTED_HEIGHT,                 Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_36(typeRegistration, "minimumWidth",                   Ui::View::Property::MINIMUM_WIDTH,                    Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_37(typeRegistration, "minimumHeight",                  Ui::View::Property::MINIMUM_HEIGHT,                   Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_38(typeRegistration, "maximumWidth",                   Ui::View::Property::MAXIMUM_WIDTH,                    Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_39(typeRegistration, "maximumHeight",                  Ui::View::Property::MAXIMUM_HEIGHT,                   Property::FLOAT,   &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_40(typeRegistration, "layoutMode",                     Ui::View::Property::LAYOUT_MODE,                      Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_42(typeRegistration, "focusGroup",             Ui::View::Property::FOCUS_GROUP,             Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_43(typeRegistration, "forwardFocusableViewId",  Ui::View::Property::FORWARD_FOCUSABLE_VIEW_ID,  Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_44(typeRegistration, "backwardFocusableViewId", Ui::View::Property::BACKWARD_FOCUSABLE_VIEW_ID, Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);

// Animatable without uniform
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_1(typeRegistration, "viewCornerRadius",       Ui::View::Property::CORNER_RADIUS,        Property::VECTOR4, &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_2(typeRegistration, "viewCornerRadiusPolicy", Ui::View::Property::CORNER_RADIUS_POLICY, Property::Value(static_cast<int>(Ui::Visual::Transform::Policy::ABSOLUTE)), &ViewDataImpl::SetProperty, nullptr); ///< Make animatable, for constarint-input
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_3(typeRegistration, "viewCornerSquareness",   Ui::View::Property::CORNER_SQUARENESS,    Property::VECTOR4, &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_4(typeRegistration, "viewBorderlineWidth",    Ui::View::Property::BORDERLINE_WIDTH,     Property::FLOAT,   &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_5(typeRegistration, "viewBorderlineColor",    Ui::View::Property::BORDERLINE_COLOR,     Property::Value(Color::BLACK), &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_6(typeRegistration, "viewBorderlineOffset",   Ui::View::Property::BORDERLINE_OFFSET,    Property::FLOAT,   &ViewDataImpl::SetProperty, nullptr);

// Animatable with uniform
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_7(typeRegistration, "viewEffectiveScale", VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, Property::Value(1.0f), &ViewDataImpl::SetProperty, nullptr); ///< Make animatable, for use it as uniform

// clang-format on

ViewDataImpl::ViewDataImpl(ViewImpl& viewImpl)
: mViewImpl(viewImpl),
  // mLayoutMode and mRequestedWidth are initialised here, out of their logical
  // group, because -Wreorder requires declaration order and both are parked at the
  // top of the class for packing. See the PACKING notes in view-data-impl.h.
  mLayoutMode(Ui::LayoutMode::DEFAULT),
  mRequestedWidth(WRAP_CONTENT),
  mCoreInteractionObject(nullptr),
  mVisualData(nullptr),
  mAttachments(nullptr),
  mFocusNavigationData(nullptr),
  mRenderEffectData(nullptr),
  mResourceReadyData(nullptr),
  mRequestedX(0.0f),
  mRequestedY(0.0f),
  mMeasuredSize{0.0f, 0.0f},
  // Pure cache keys; their initial values are never consulted because
  // mMeasureCacheValid starts false. NaN is nevertheless the fail-safe choice for
  // both: it compares unequal to everything, including itself, so a predicate that
  // somehow reached them without the validity bit would MISS rather than serve.
  mLastMeasureConstraint{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()},
  mLastMeasureScale(std::numeric_limits<float>::quiet_NaN()),
  mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
  mLastArrangeInput{0.0f, 0.0f, 0.0f, 0.0f},
  // 0 = "never propagated", which no live generation ever equals, so the first
  // invalidation on each axis always walks.
  mMeasurePropagationGeneration(0u),
  mArrangePropagationGeneration(0u),
  mMargin(),
  mPadding(),
  mSize(0, 0),
  mLastArrangedRenderEffectSize(0, 0),
  mRequestedHeight(WRAP_CONTENT),
  mAccessibilityData(nullptr),
  mAccessibleObjectCreator(nullptr),
  mAccessibilityRole{static_cast<int32_t>(Accessibility::Role::NONE)},
  mSkipChildrenUpdate(false),
  mMeasureCacheValid(false),
  mMeasureDirty(false),
  mMeasureInProgress(false),
  mMeasurePassPoisoned(false),
  mMeasuredSlotUnconsumed(false),
  mArrangeCacheValid(false),
  mArrangeDirty(false),
  mArrangeInProgress(false),
  mArrangeReplayInProgress(false),
  mArrangePassPoisoned(false),
  mArrangeCacheBlockedDuringPass(false),
  mArrangeResultAvailable(false),
  // ArrangePolicy::IF_CHANGED is the default for OnArrange, callbacks and managers.
  // These bits record only the ArrangePolicy::ALWAYS opt-out; the active producer's bit is
  // derived whenever producer selection or policy changes.
  mArrangeOverrideAlways(false),
  mArrangeCallbackAlways(false),
  mArrangeProducerAlways(false),
  mEffectiveScaleValid(false),
  mEffectiveScaleActorSynced(false),
  mEffectiveScaleInvalidatedDuringPass(false),
  mInitialLayoutDone(false),
  mInitialEnterSettled(false),
  mIsFocusGroup(false),
  mDispatchKeyEvents(true),
  mAccessibleCreatable(true),
  mProcessorRegistered(false),
  mFittingModeLayoutFinishedSignalConnected(false),
  mDefaultFocusIndicatorSuppressedByStateEffect(false),
  mLayoutDirectionSignalConnected(false),
  mInPassInvalidationWarned(false),
  // Pure cache key; its initial value is never consulted because
  // mArrangeCacheValid starts false.
  mLastArrangeDirection(Dali::LayoutDirection::LEFT_TO_RIGHT),
  mKeyEventDispatchInProgress(false),
  mFlags(ViewImpl::ViewBehaviour(ViewImpl::VIEW_BEHAVIOUR_DEFAULT))
{
}

ViewDataImpl::~ViewDataImpl()
{
  if(mVisualData)
  {
    mVisualData->ClearVisuals();
  }

  if(mProcessorRegistered && Adaptor::IsAvailable())
  {
    // Unregister the processor from the adaptor
    Adaptor::Get().UnregisterProcessorOnce(*this, true);
  }

  if(mResourceReadyData && mResourceReadyData->idleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the view is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mResourceReadyData->idleCallback);
  }
}

void ViewDataImpl::SetBehaviourFlags(ViewImpl::ViewBehaviour behaviourFlags)
{
  mFlags = behaviourFlags;
}

bool ViewDataImpl::AreVisualsEnabled() const
{
  return !(mFlags & Ui::ViewImpl::ViewBehaviour::DISABLE_VISUALS);
}

void ViewDataImpl::Destroy()
{
  auto colorManager = UiColorManager::Get();
  if(colorManager)
  {
    GetImpl(colorManager).ClearBindings(mViewImpl.GetOwner());
  }

  auto localizationManager = UiLocalizationManager::Get();
  if(localizationManager)
  {
    GetImpl(localizationManager).ClearBindings(mViewImpl.GetOwner());
  }

  NotifyTraitsViewDestroying();
  LayoutController::UnregisterFromAll(&mViewImpl);
  ClearRenderEffect();
}

void ViewDataImpl::InitializeVisualData()
{
  mVisualData = std::make_unique<ViewDataImpl::VisualData>(*this);
}

MeasuredSize ViewDataImpl::MeasureDefault(float widthConstraint, float heightConstraint)
{
  float s    = mViewImpl.GetEffectiveScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  float pw = static_cast<float>(mPadding.start + mPadding.end);
  float ph = static_cast<float>(mPadding.top + mPadding.bottom);

  float effectiveWidth  = (mRequestedWidth >= 0) ? mRequestedWidth : natW;
  float effectiveHeight = (mRequestedHeight >= 0) ? mRequestedHeight : natH;

  float contentWidth  = std::max(0.0f, effectiveWidth - pw);
  float contentHeight = std::max(0.0f, effectiveHeight - ph);

  // The branch key is "does this view have a child that CONTRIBUTES to its measured
  // size", not "does it have any child at all". Standalone children are excluded from
  // this accumulation -- the loop below skips them, and View's public contract
  // (view.h) says a standalone child does not affect its parent's measurement --
  // so a view whose ONLY children are standalone accumulates nothing. Keying the branch on
  // mChildren.Empty() sent such a view down the accumulation path with maxRight/maxBottom
  // still 0, skipping the background natural-size path below entirely and measuring
  // 0 x 0 where the same view with NO children measures its background.
  //
  // One O(direct children) pre-scan rather than a flag set by the loop: the branch has to
  // choose its formula BEFORE the loop runs. Each step is a layout-mode read with no
  // property access, and it stops at the first contributing child, so the ordinary case
  // costs one iteration.
  //
  // NOT fixed by adding a parent invalidation anywhere: the value is wrong on the very
  // first measurement, so there is nothing to invalidate.
  bool hasContributingChild = false;
  for(Dali::Vector<View>::ConstIterator it = mChildren.Begin(), end = mChildren.End(); it != end; ++it)
  {
    if(!IntegrationView::IsLayoutModeStandalone(GetImpl(*it)))
    {
      hasContributingChild = true;
      break;
    }
  }

  if(hasContributingChild)
  {
    float                 maxRight  = 0.0f;
    float                 maxBottom = 0.0f;
    std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
    for(auto& childView : childSnapshot)
    {
      ViewImpl& childImpl = GetImpl(childView);
      if(IntegrationView::IsLayoutModeStandalone(childImpl))
      {
        continue;
      }

      float        childScale            = childImpl.GetEffectiveScale();
      Insets       margin                = childImpl.GetMargin();
      float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
      float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
      float        childWidthConstraint  = std::max(0.0f, contentWidth * s - marginW);
      float        childHeightConstraint = std::max(0.0f, contentHeight * s - marginH);
      MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

      float childNatW  = (s > 0.0f) ? childSize.width / s : childSize.width;
      float childNatH  = (s > 0.0f) ? childSize.height / s : childSize.height;
      float childX     = childImpl.GetRequestedX();
      float childY     = childImpl.GetRequestedY();
      float natMarginW = (s > 0.0f) ? marginW / s : marginW;
      float natMarginH = (s > 0.0f) ? marginH / s : marginH;
      maxRight         = std::max(maxRight, childX + natMarginW + childNatW);
      maxBottom        = std::max(maxBottom, childY + natMarginH + childNatH);
    }

    MeasuredSize size;
    if(mRequestedWidth >= 0)
    {
      size.width = mRequestedWidth;
    }
    else if(mRequestedWidth == MATCH_PARENT)
    {
      size.width = GetMinimumWidth();
    }
    else
    {
      size.width = maxRight + pw;
      if(natW >= 0.0f)
      {
        size.width = std::min(size.width, natW);
      }
    }
    if(mRequestedHeight >= 0)
    {
      size.height = mRequestedHeight;
    }
    else if(mRequestedHeight == MATCH_PARENT)
    {
      size.height = GetMinimumHeight();
    }
    else
    {
      size.height = maxBottom + ph;
      if(natH >= 0.0f)
      {
        size.height = std::min(size.height, natH);
      }
    }
    return {size.width * s, size.height * s};
  }

  MeasuredSize size;
  if(mRequestedWidth >= 0)
  {
    size.width = mRequestedWidth;
  }
  else if(mRequestedWidth == MATCH_PARENT)
  {
    size.width = GetMinimumWidth();
  }
  else
  {
    Vector3 naturalSize     = GetBackgroundVisualNaturalSize();
    float   contentNaturalW = std::max(0.0f, naturalSize.width - pw);
    size.width              = contentNaturalW + pw;
    if(natW >= 0.0f)
    {
      size.width = std::min(size.width, natW);
    }
  }
  if(mRequestedHeight >= 0)
  {
    size.height = mRequestedHeight;
  }
  else if(mRequestedHeight == MATCH_PARENT)
  {
    size.height = GetMinimumHeight();
  }
  else
  {
    Vector3 naturalSize     = GetBackgroundVisualNaturalSize();
    float   contentNaturalH = std::max(0.0f, naturalSize.height - ph);
    size.height             = contentNaturalH + ph;
    if(natH >= 0.0f)
    {
      size.height = std::min(size.height, natH);
    }
  }
  return {size.width * s, size.height * s};
}

LayoutRect ViewDataImpl::ArrangeDefault(const LayoutRect& bounds)
{
  // Self geometry is applied centrally in Arrange() via ApplySelfBoundsIfChanged;
  // the default arrange only places regular children and echoes the input bounds.
  if(!mChildren.Empty())
  {
    float s            = mViewImpl.GetEffectiveScale();
    float visPadLeft   = static_cast<float>(mPadding.start) * s;
    float visPadRight  = static_cast<float>(mPadding.end) * s;
    float visPadTop    = static_cast<float>(mPadding.top) * s;
    float visPadBottom = static_cast<float>(mPadding.bottom) * s;

    std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
    for(auto& childView : childSnapshot)
    {
      ViewImpl& childImpl = GetImpl(childView);
      if(IntegrationView::IsLayoutModeStandalone(childImpl))
      {
        continue;
      }

      float        childScale      = childImpl.GetEffectiveScale();
      Insets       margin          = childImpl.GetMargin();
      float        visMarginStart  = static_cast<float>(margin.start) * childScale;
      float        visMarginEnd    = static_cast<float>(margin.end) * childScale;
      float        visMarginTop    = static_cast<float>(margin.top) * childScale;
      float        visMarginBottom = static_cast<float>(margin.bottom) * childScale;
      float        visMarginW      = visMarginStart + visMarginEnd;
      float        visMarginH      = visMarginTop + visMarginBottom;
      MeasuredSize childMeasured   = childImpl.GetMeasuredSize();
      float        childW          = childMeasured.width;
      float        childH          = childMeasured.height;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childW = std::max(0.0f, bounds.width - visPadLeft - visPadRight - visMarginW);
      }
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childH = std::max(0.0f, bounds.height - visPadTop - visPadBottom - visMarginH);
      }
      float childX = visPadLeft + visMarginStart + childImpl.GetRequestedX() * s;
      float childY = visPadTop + visMarginTop + childImpl.GetRequestedY() * s;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        LayoutDependency::ArrangeOwnedMeasureScope ownerScope(&mViewImpl);
        childImpl.Measure(childW, childH);
      }

      childImpl.Arrange(LayoutRect(childX, childY, childW, childH));
    }
  }

  return bounds;
}

bool ViewDataImpl::HandleKeyEventDefault(const Dali::KeyEvent& event)
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    return traitObject->OnKeyEvent(View::DownCast(mViewImpl.Self()), event);
  }
  return false;
}

void ViewDataImpl::FinalizeKeyEventDispatchDefault()
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    traitObject->FinalizeKeyEventDispatch();
  }
}

bool ViewDataImpl::HasIntrinsicHoverHandlingDefault() const
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    return traitObject->HasIntrinsicHoverHandling();
  }
  return false;
}

bool ViewDataImpl::HasIntrinsicTouchHandlingDefault() const
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    return traitObject->HasIntrinsicTouchHandling();
  }
  return false;
}

bool ViewDataImpl::HandleHoverEventDefault(const Dali::HoverEvent& event)
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    return traitObject->OnHoverEvent(event);
  }
  return false;
}

bool ViewDataImpl::HandleTouchEventDefault(const Dali::TouchEvent& event)
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    return traitObject->OnTouchEvent(View::DownCast(mViewImpl.Self()), event);
  }
  return false;
}

void ViewDataImpl::FinalizeTouchEventDispatchDefault(const Dali::TouchEvent& event)
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    traitObject->FinalizeTouchEventDispatch(View::DownCast(mViewImpl.Self()), event);
  }
}

void ViewDataImpl::HandleFocusChangedDefault(bool focused)
{
  InputEvent cause;
  auto       focusManager = Ui::FocusManager::Get();
  if(focusManager)
  {
    cause = GetImpl(focusManager).FocusChangedContext().inputEvent;
  }

  const bool focusIndicated = focusManager && GetImpl(focusManager).FocusChangedContext().focusIndicated;
  SetState(ViewState::FOCUSED + (focusIndicated ? ViewState::FOCUS_INDICATED : ViewState::NORMAL), focused, cause);

  if(auto* traitObject = GetCoreInteractionObject())
  {
    if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
    {
      interactiveTraitImpl->OnFocusedChanged(View::DownCast(mViewImpl.Self()), focused);
    }
  }

  EmitFocusChangedSignal(focused);
}

void ViewDataImpl::RelayoutDefault(const Vector2& size, RelayoutContainer& container)
{
  if(IntegrationView::HasLayoutCapability(mViewImpl) || GetParentLayout() || GetParentView())
  {
    return;
  }

  if((mPadding.start != 0) || (mPadding.end != 0) || (mPadding.top != 0) ||
     (mPadding.bottom != 0) || (mMargin.start != 0) || (mMargin.end != 0) ||
     (mMargin.top != 0) || (mMargin.bottom != 0))
  {
    for(unsigned int i = 0, numChildren = mViewImpl.Self().GetChildCount(); i < numChildren; ++i)
    {
      Actor   child = mViewImpl.Self().GetChildAt(i);
      Vector2 newChildSize(size);

      Insets padding = mPadding;

      Dali::LayoutDirection::Type layoutDirection = mViewImpl.Self().GetEffectiveLayoutDirection();

      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      newChildSize.width  = size.width - (padding.start + padding.end);
      newChildSize.height = size.height - (padding.top + padding.bottom);

      Vector2 childOffset(0.f, 0.f);
      childOffset.x += (mMargin.start + padding.start);
      childOffset.y += (mMargin.top + padding.top);

      child.SetProperty(Actor::Property::POSITION, Vector2(childOffset.x, childOffset.y));

      container.Add(child, newChildSize);
    }
  }

  if(Dali::Integration::Accessibility::IsUp()) // LCOV_EXCL_LINE
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      auto highlightFrame = accessible->GetHighlightActor();
      if(accessible->GetCurrentlyHighlightedActor() == mViewImpl.Self() &&
         highlightFrame.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
      {
        highlightFrame.SetProperty(Actor::Property::SIZE, size);
        container.Add(highlightFrame, size);
      }
    }
  }

  ApplyFittingMode(size, false);
}

const ViewState& ViewDataImpl::GetState() const
{
  return mState;
}

bool ViewDataImpl::IsEffectivelyFocused() const
{
  return ViewStateManager::Get().IsEffectivelyFocused(mViewImpl);
}

View::LayoutFinishedSignalType& ViewDataImpl::LayoutFinishedSignal()
{
  return mLayoutFinishedSignal;
}

View::StateChangedSignalType& ViewDataImpl::StateChangedSignal()
{
  return mStateChangedSignal;
}

Ui::View::ResourceReadySignalType& ViewDataImpl::ResourceReadySignal()
{
  return EnsureResourceReadyData().resourceReadySignal;
}

Ui::View::OffScreenRenderingFinishedSignalType& ViewDataImpl::OffScreenRenderingFinishedSignal()
{
  return EnsureRenderEffectData().offScreenRenderingFinishedSignal;
}

bool ViewDataImpl::HasLayoutFinishedSignalConnections() const
{
  return !mLayoutFinishedSignal.Empty();
}

void ViewDataImpl::EmitLayoutFinishedSignal(const LayoutRect& bounds)
{
  Dali::Ui::View handle(mViewImpl.GetOwner());
  if(handle && !mLayoutFinishedSignal.Empty())
  {
    mLayoutFinishedSignal.Emit(handle, bounds);
  }
}

PendingLayoutTransitionChanges ViewDataImpl::TakePendingLayoutTransitionChanges()
{
  PendingLayoutTransitionChanges changes;
  if(mLayoutTransitionData)
  {
    LayoutTransitionData& transitionData = *mLayoutTransitionData;
    std::swap(changes.enterChildren, transitionData.pendingEnterChildren);
    std::swap(changes.reorderedChildren, transitionData.pendingReorderedChildren);
    changes.hadChildRemoval               = transitionData.hasPendingChildRemoval;
    transitionData.hasPendingChildRemoval = false;
  }
  return changes;
}

Ui::InteractiveTrait ViewDataImpl::EnsureInteractiveTrait()
{
  auto* traitObject = GetCoreInteractionObject();

  if(!traitObject)
  {
    IntrusivePtr<CoreInteractionObject> newTraitObject = new CoreInteractionObject();
    traitObject                                        = newTraitObject.Get();
    SetTrait(Integration::ReservedTraitId::CORE_INTERACTION_TRAITS, newTraitObject);
    AttachInteractiveStateEffect();
  }

  return Ui::InteractiveTrait::New(traitObject);
}

void ViewDataImpl::SetStateEffect(StateEffect effect)
{
  if(!effect)
  {
    effect = StateEffect::None();
  }
  ResetStateEffect(*this, effect);

  if(effect && !effect.IsNone() && IsInteractive())
  {
    GetImpl(effect).OnInteractiveAttached(View::DownCast(mViewImpl.Self()));
  }
  RefreshDefaultFocusIndicatorSuppression();
}

void ViewDataImpl::AttachInteractiveStateEffect()
{
  StateEffect existingEffect = GetKnownTraitHandle<StateEffect>(*this, Integration::ReservedTraitId::STATE_EFFECT);
  if(existingEffect)
  {
    GetImpl(existingEffect).OnInteractiveAttached(View::DownCast(mViewImpl.Self()));
    RefreshDefaultFocusIndicatorSuppression();
    return;
  }

  StateEffect defaultEffect = UiConfig::GetCurrent().GetDefaultStateEffectForInteractive();
  if(defaultEffect && !defaultEffect.IsNone())
  {
    ResetStateEffect(*this, defaultEffect);
    GetImpl(defaultEffect).OnInteractiveAttached(View::DownCast(mViewImpl.Self()));
    RefreshDefaultFocusIndicatorSuppression();
  }
}

bool ViewDataImpl::IsDefaultFocusIndicatorSuppressedByStateEffect() const
{
  return mDefaultFocusIndicatorSuppressedByStateEffect;
}

void ViewDataImpl::RefreshDefaultFocusIndicatorSuppression()
{
  bool        suppress = false;
  StateEffect effect   = GetKnownTraitHandle<StateEffect>(*this, Integration::ReservedTraitId::STATE_EFFECT);
  if(effect && !effect.IsNone())
  {
    suppress = GetImpl(effect).ShouldSuppressDefaultFocusIndicator(View::DownCast(mViewImpl.Self()));
  }

  if(mDefaultFocusIndicatorSuppressedByStateEffect == suppress)
  {
    return;
  }

  mDefaultFocusIndicatorSuppressedByStateEffect = suppress;

  Ui::FocusManager focusManager = Ui::FocusManager::Get();
  if(focusManager)
  {
    Dali::Ui::GetImpl(focusManager).RefreshFocusIndicator(Ui::View::DownCast(mViewImpl.Self()));
  }
}

void ViewDataImpl::InvalidateDefaultFocusIndicatorSuppression(const Integration::StateEffectImpl& effect)
{
  StateEffect current = GetKnownTraitHandle<StateEffect>(*this, Integration::ReservedTraitId::STATE_EFFECT);
  if(!current || current.IsNone() || (&GetImpl(current) != &effect))
  {
    return;
  }

  RefreshDefaultFocusIndicatorSuppression();
}

void ViewDataImpl::SetStateEffectTarget(View target)
{
  View owner = View::DownCast(mViewImpl.Self());
  if(target)
  {
    DALI_ASSERT_ALWAYS(IsSelfOrDescendant(owner, target) && "State effect target must be this View or a descendant");
  }

  StateEffectTargetTrait trait = GetOrCreateStateEffectTargetTrait(*this);
  trait.GetImpl().SetTargetId(target ? target.GetProperty<int32_t>(Actor::Property::ID) : StateEffectTargetTraitImpl::INVALID_TARGET_ID);

  StateEffect effect = GetKnownTraitHandle<StateEffect>(*this, Integration::ReservedTraitId::STATE_EFFECT);
  if(effect && !effect.IsNone())
  {
    GetImpl(effect).OnStateEffectTargetsChanged(owner);
  }
}

View ViewDataImpl::GetStateEffectTarget() const
{
  View                   owner = View::DownCast(mViewImpl.Self());
  StateEffectTargetTrait trait = GetKnownTraitHandle<StateEffectTargetTrait>(*this, Integration::ReservedTraitId::STATE_EFFECT_TARGET);
  if(!trait)
  {
    return owner;
  }

  View target = FindStateEffectTarget(owner, trait.GetImpl().GetTargetId());
  return target ? target : owner;
}

bool ViewDataImpl::IsInteractive() const
{
  auto* traitObject = GetCoreInteractionObject();
  return traitObject && traitObject->GetInteractiveTraitImpl();
}

Ui::SelectableTrait ViewDataImpl::EnsureSelectableTrait()
{
  auto* traitObject = GetCoreInteractionObject();

  if(!traitObject)
  {
    IntrusivePtr<CoreInteractionObject> newTraitObject = new CoreInteractionObject();
    traitObject                                        = newTraitObject.Get();
    SetTrait(Integration::ReservedTraitId::CORE_INTERACTION_TRAITS, newTraitObject);
    AttachInteractiveStateEffect();
  }

  return Ui::SelectableTrait::New(traitObject);
}

bool ViewDataImpl::IsSelectable() const
{
  auto* traitObject = GetCoreInteractionObject();
  return traitObject && traitObject->GetSelectableTraitImpl();
}

Ui::GroupSelectableTrait ViewDataImpl::EnsureGroupSelectableTrait()
{
  auto* traitObject = GetCoreInteractionObject();

  if(!traitObject)
  {
    IntrusivePtr<CoreInteractionObject> newTraitObject = new CoreInteractionObject();
    traitObject                                        = newTraitObject.Get();
    SetTrait(Integration::ReservedTraitId::CORE_INTERACTION_TRAITS, newTraitObject);
    AttachInteractiveStateEffect();
  }

  return Ui::GroupSelectableTrait::New(traitObject);
}

bool ViewDataImpl::IsGroupSelectable() const
{
  auto* traitObject = GetCoreInteractionObject();
  return traitObject && traitObject->GetGroupSelectableTraitImpl();
}

void ViewDataImpl::SetFocusNavigationCallback(FocusNavigationCallback callback)
{
  if(callback || mFocusNavigationData)
  {
    EnsureFocusNavigationData().callback = std::move(callback);
  }
}

FocusNavigationResult ViewDataImpl::RequestFocusNavigation(View currentFocusedView, FocusNavigationContext context)
{
  if(mFocusNavigationData && mFocusNavigationData->callback)
  {
    return mFocusNavigationData->callback.Invoke(currentFocusedView, context);
  }
  return mViewImpl.OnFocusNavigationRequested(currentFocusedView, context);
}

View ViewDataImpl::RequestFocus()
{
  if(mViewImpl.Self().HasAncestorBlockingFocus())
  {
    return View();
  }
  return mViewImpl.OnFocusRequested();
}

View ViewDataImpl::ResolveDefaultFocusRequest()
{
  Ui::View self = Ui::View::DownCast(mViewImpl.Self());
  if(IntegrationView::HasLayoutCapability(mViewImpl))
  {
    if(!self.IsAllowDescendantFocusEnabled())
    {
      return self.IsFocusable() && self.IsEnabled() && self.IsVisible() ? self : View();
    }

    for(auto& child : mChildren)
    {
      if(child && child.IsVisible())
      {
        View resolved = ViewDataImpl::Get(GetImpl(child)).RequestFocus();
        if(resolved)
        {
          return resolved;
        }
      }
    }
  }

  return self.IsFocusable() && self.IsEnabled() && self.IsVisible() ? self : View();
}

bool ViewDataImpl::IsFocusGroup() const
{
  return mIsFocusGroup;
}

void ViewDataImpl::SetAsFocusGroup(bool isFocusGroup)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::FOCUS_GROUP, isFocusGroup);
}

Ui::View::KeyEventSignalType& ViewDataImpl::KeyEventSignal()
{
  return mKeyEventSignal;
}

Ui::View::FocusChangedSignalType& ViewDataImpl::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

bool ViewDataImpl::NotifyKeyEvent(const KeyEvent& event)
{
  Dali::Ui::View handle(mViewImpl.GetOwner());
  if(mViewImpl.FilterKeyEvent(event))
  {
    DALI_LOG_RELEASE_INFO("[KeyEvent] keyCode(%d), state(%s) consumed by View id(%d), name(%s) at View::FilterKeyEvent\n",
                          event.GetKeyCode(),
                          event.GetState() == KeyEvent::DOWN ? "DOWN" : "UP",
                          handle.GetProperty<int32_t>(Dali::Actor::Property::ID),
                          handle.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
    return true;
  }

  DALI_ASSERT_DEBUG(!mKeyEventDispatchInProgress && "Nested key dispatch to the same view is unsupported");
  ScopedTrueFlag         dispatchFlagGuard(mKeyEventDispatchInProgress);
  ScopedKeyEventDispatch dispatchGuard(GetCoreInteractionObject());

  bool consumed = mViewImpl.OnKeyEvent(event);
  if(consumed)
  {
    DALI_LOG_RELEASE_INFO("[KeyEvent] keyCode(%d), state(%s) consumed by View id(%d), name(%s) at View::OnKeyEvent\n",
                          event.GetKeyCode(),
                          event.GetState() == KeyEvent::DOWN ? "DOWN" : "UP",
                          handle.GetProperty<int32_t>(Dali::Actor::Property::ID),
                          handle.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
  }

  if(!mKeyEventSignal.Empty())
  {
    // Any connected callback consuming the event consumes it for all of them.
    const bool signalConsumed = mKeyEventSignal.EmitOr(handle, event);
    if(signalConsumed)
    {
      DALI_LOG_RELEASE_INFO("[KeyEvent] keyCode(%d), state(%s) consumed by View id(%d), name(%s) at View::KeyEventSignal\n",
                            event.GetKeyCode(),
                            event.GetState() == KeyEvent::DOWN ? "DOWN" : "UP",
                            handle.GetProperty<int32_t>(Dali::Actor::Property::ID),
                            handle.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
    }
    consumed = consumed || signalConsumed;
  }

  mViewImpl.OnFinalizeKeyEventDispatch(event);

  return consumed;
}

bool ViewDataImpl::ActivateAccessibilityDefault()
{
  Ui::View   self = Ui::View::DownCast(mViewImpl.Self());
  InputEvent event(InputEventImpl::New(InputEventType::ACCESSIBILITY_ACTIVATION).Get());

  Ui::FocusManager focusManager = Ui::FocusManager::Get();
  const bool       focused      = focusManager && GetImpl(focusManager).SetCurrentFocusView(self, event);

  bool clicked = false;
  if(mCoreInteractionObject)
  {
    clicked = mCoreInteractionObject->OnAccessibilityActivate(self, event);
  }

  return focused || clicked;
}

void ViewDataImpl::SetAccessibilityActivateCallback(Callback<bool(View)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_ACTIVATE_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityActivate()
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_ACTIVATE_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()));
  }

  return mViewImpl.OnAccessibilityActivate();
}

void ViewDataImpl::SetAccessibilityEscapeCallback(Callback<bool(View)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_ESCAPE_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityEscape()
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_ESCAPE_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()));
  }

  return mViewImpl.OnAccessibilityEscape();
}

void ViewDataImpl::SetAccessibilityPanCallback(Callback<bool(View, PanGesture)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_PAN_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityPan(PanGesture gesture)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_PAN_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, PanGesture)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), std::move(gesture));
  }

  return mViewImpl.OnAccessibilityPan(std::move(gesture));
}

void ViewDataImpl::SetAccessibilityValueChangeCallback(Callback<bool(View, bool)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_VALUE_CHANGE_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityValueChange(bool isIncreased)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_VALUE_CHANGE_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, bool)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), isIncreased);
  }

  return mViewImpl.OnAccessibilityValueChange(isIncreased);
}

void ViewDataImpl::SetAccessibilityScrollToChildCallback(Callback<bool(View, View)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_SCROLL_TO_CHILD_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityScrollToChild(View child)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_SCROLL_TO_CHILD_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, View)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), std::move(child));
  }

  return mViewImpl.OnAccessibilityScrollToChild(std::move(child));
}

void ViewDataImpl::SetAccessibilityZoomCallback(Callback<bool(View)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_ZOOM_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityZoom()
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_ZOOM_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()));
  }

  return mViewImpl.OnAccessibilityZoom();
}

void ViewDataImpl::SetAccessibilityRequestNameCallback(Callback<bool(View, Dali::String&)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_REQUEST_NAME_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityRequestName(Dali::String& value)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_REQUEST_NAME_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, Dali::String&)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), value);
  }

  return mViewImpl.OnAccessibilityRequestName(value);
}

void ViewDataImpl::SetAccessibilityRequestDefaultNameCallback(Callback<bool(View, Dali::String&)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_REQUEST_DEFAULT_NAME_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityRequestDefaultName(Dali::String& value)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_REQUEST_DEFAULT_NAME_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, Dali::String&)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), value);
  }

  return mViewImpl.OnAccessibilityRequestDefaultName(value);
}

void ViewDataImpl::SetAccessibilityRequestDescriptionCallback(Callback<bool(View, Dali::String&)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_REQUEST_DESCRIPTION_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityRequestDescription(Dali::String& value)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_REQUEST_DESCRIPTION_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, Dali::String&)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), value);
  }

  return mViewImpl.OnAccessibilityRequestDescription(value);
}

void ViewDataImpl::SetAccessibilityRequestDefaultDescriptionCallback(Callback<bool(View, Dali::String&)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_REQUEST_DEFAULT_DESCRIPTION_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityRequestDefaultDescription(Dali::String& value)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_REQUEST_DEFAULT_DESCRIPTION_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, Dali::String&)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), value);
  }

  return mViewImpl.OnAccessibilityRequestDefaultDescription(value);
}

void ViewDataImpl::SetAccessibilityRequestValueCallback(Callback<bool(View, Dali::String&)> callback)
{
  SetAccessibilityCallback(*this, ACCESSIBILITY_REQUEST_VALUE_CALLBACK_TRAIT_ID, std::move(callback));
}

bool ViewDataImpl::DispatchAccessibilityRequestValue(Dali::String& value)
{
  IntrusivePtr<TraitObject> object = GetTrait(ACCESSIBILITY_REQUEST_VALUE_CALLBACK_TRAIT_ID);
  if(object)
  {
    auto* callbackObject = static_cast<AccessibilityCallbackObject<bool(View, Dali::String&)>*>(object.Get());
    return callbackObject->Invoke(Ui::View::DownCast(mViewImpl.Self()), value);
  }

  return mViewImpl.OnAccessibilityRequestValue(value);
}

ViewAccessible* ViewDataImpl::CreateAccessibleObject()
{
  Ui::View view(mViewImpl.GetOwner());
  if(mAccessibleObjectCreator)
  {
    return mAccessibleObjectCreator(view);
  }
  return new ViewAccessible(view);
}

void ViewDataImpl::SetRequestedX(float x)
{
  if(!Dali::Equals(mRequestedX, x))
  {
    mRequestedX = x;
    // InvalidateMeasure (not InvalidateArrange): a WRAP_CONTENT parent's
    // OnMeasure reads the child's RequestedX/Y into maxRight/maxBottom,
    // so a position change can affect the parent's measured size. Measure
    // invalidation also marks the chain dirty for Arrange.
    InvalidateMeasure();
  }
}

void ViewDataImpl::SetRequestedY(float y)
{
  if(!Dali::Equals(mRequestedY, y))
  {
    mRequestedY = y;
    InvalidateMeasure();
  }
}

float ViewDataImpl::GetRequestedX() const
{
  return mRequestedX;
}

float ViewDataImpl::GetRequestedY() const
{
  return mRequestedY;
}

void ViewDataImpl::SetUiScalePolicy(UiScalePolicy policy)
{
  if(mScalePolicy != policy)
  {
    mScalePolicy = policy;
    ResetSubtreeScaleAndLayoutCaches();
    InvalidateMeasure();
  }
}

UiScalePolicy ViewDataImpl::GetUiScalePolicy() const
{
  return mScalePolicy;
}

float ViewDataImpl::GetEffectiveScale() const
{
  // mEffectiveScaleValid -- not a value sentinel on mEffectiveScale itself -- is
  // what says whether the cached scale is usable. Every scale-context
  // invalidation clears that bit; this is the only place that sets it, which is
  // exactly what makes "bit true" mean "mEffectiveScale is what
  // ComputeEffectiveScale() would return now".
  if(!mEffectiveScaleValid)
  {
    mEffectiveScale      = ComputeEffectiveScale();
    mEffectiveScaleValid = true;
  }
  return mEffectiveScale;
}

void ViewDataImpl::InvalidateMeasure()
{
  // Invalidation ALWAYS propagates to the layout root and registers there --
  // there is deliberately no "already dirty, so return early" short-circuit.
  //
  // The local dirty flag only records that THIS view has layout work that has
  // not been consumed yet. It is not evidence that the ancestor chain was
  // walked or that a layout root was registered, because:
  //   - Nothing clears a child's dirty except the child's own pass, so a custom
  //     parent that never arranges a dirty child leaves that dirty standing
  //     indefinitely. Short-circuiting on it would swallow every later
  //     invalidation of that child forever.
  //   - The ancestor chain can change under a standing dirty (reparenting), so
  //     the chain that was walked is not necessarily the chain that must be
  //     reached now.
  //   - A re-invalidation raised while a pass is running must still poison that
  //     pass and re-register the follow-up layout (see the in-progress writes
  //     below), which a short-circuit above them would skip.
  //
  // Repeating the walk on an already-dirty subtree is O(depth) work that is
  // idempotent: each level sets its own flags and calls its parent exactly
  // once, and LayoutController::RequestLayout inserts into a pending set, so
  // duplicate registrations coalesce. See plan34 27.5.
  //
  // Drop the cached effective scale: whatever changed may have moved this view's
  // effective scale (a reparent re-roots the INHERIT chain), so the next
  // GetEffectiveScale() must recompute rather than serve the cached value.
  DropCachedEffectiveScale();

  // Both caches go with it: a cached measured size or arranged bounds produced
  // under the previous context is not a result this call may leave standing.
  InvalidateLayoutCaches();

  // ...and, unlike a pure freshness drop, this call also records that there is
  // now unconsumed layout work here. That is exactly the part
  // InvalidateLayoutCaches() must not do on its own.
  mMeasureDirty = true;
  mArrangeDirty = true;

  // Everything ABOVE this point is the local half and always runs. Everything BELOW
  // is the walk to the layout root, which is idempotent and can therefore be skipped
  // while the registration it would make is already pending -- see
  // LayoutInvalidation and mMeasurePropagationGeneration. This is what keeps a batch of
  // invalidations before one layout pass at O(1) each instead of O(depth) each, with
  // two handle DownCasts per level and a Window lookup at the root.
  //
  // Disabled outright while any layout pass is on the stack (gActiveLayoutPassDepth),
  // because mid-pass the walk also poisons in-progress ancestors, which is not
  // something the root's registration stands in for.
  //
  // Every step of the walk below recurses into this internal primitive
  // (ViewDataImpl::Get(...).Invalidate*()) rather than into the public
  // ViewImpl::Invalidate*() entry point, so the walk never re-enters the public
  // API on the framework's own behalf.
  const uint32_t generation = LayoutInvalidation::CurrentGeneration();
  if(gActiveLayoutPassDepth == 0u && mMeasurePropagationGeneration == generation)
  {
    return;
  }
  mMeasurePropagationGeneration = generation;

  // Layout boundary: a standalone view is excluded from its parent's
  // OnMeasure/OnArrange accumulation, so its measure result cannot change
  // the parent's measured size. Stop propagation here and register this view
  // as its own layout root.
  //
  // When the parent has a LayoutTransition attached, also invalidate the
  // parent so its CaptureBeforeLayout / StartTransitionsAfterLayout pass
  // runs in the same layout batch. Combined with the controller's
  // depth-sorted iteration, this lets the parent capture pre-change
  // bounds before the standalone child's own arrange updates them, so
  // a standalone child's RequestedWidth / RequestedHeight change
  // surfaces as the parent's CHANGE slot. This is the SOLE carrier of the
  // standalone+transition hook: OnChildAdded no longer repeats it, because the
  // child's own InvalidateMeasure() on the add always reaches this branch.
  if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    Ui::View parentView = GetParentView();
    if(parentView && GetImpl(parentView).GetLayoutTransition())
    {
      ViewDataImpl::Get(GetImpl(parentView)).InvalidateMeasure();
    }
    RegisterWithLayoutController();
    return;
  }

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    ViewDataImpl::Get(GetImpl(parentLayout)).InvalidateMeasure();
    return;
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    ViewDataImpl::Get(GetImpl(parentView)).InvalidateMeasure();
    return;
  }

  RegisterWithLayoutController();
}

void ViewDataImpl::InvalidateArrange()
{
  // Mirrors InvalidateMeasure: invalidation ALWAYS propagates to the layout
  // root and registers there, with no "already dirty, so return early"
  // short-circuit. mArrangeDirty is cleared only by this view's own arrange
  // pass, so a custom parent that never arranges a dirty child would otherwise
  // pin that dirty forever and swallow every subsequent invalidation.
  //
  // The repeated walk is O(depth) and idempotent -- each level sets its flags
  // and calls its parent once -- and LayoutController::RequestLayout coalesces
  // duplicate registrations in its pending set. See plan34 27.5.
  mArrangeDirty      = true;
  mArrangeCacheValid = false;

  if(mArrangeInProgress)
  {
    mArrangePassPoisoned = true;
  }

  // Propagation coalescing, exactly as in InvalidateMeasure and with the same two
  // conditions -- see the comment there, and mArrangePropagationGeneration for why the
  // measure and arrange records must stay separate.
  const uint32_t generation = LayoutInvalidation::CurrentGeneration();
  if(gActiveLayoutPassDepth == 0u && mArrangePropagationGeneration == generation)
  {
    return;
  }
  mArrangePropagationGeneration = generation;

  // Layout boundary: standalone child's arrange result does not feed back
  // into the parent's arrangement — stop here and self-register.
  if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    RegisterWithLayoutController();
    return;
  }

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    ViewDataImpl::Get(GetImpl(parentLayout)).InvalidateArrange();
    return;
  }

  // Propagate to parent View (no LayoutManager)
  Ui::View parentView = GetParentView();
  if(parentView)
  {
    ViewDataImpl::Get(GetImpl(parentView)).InvalidateArrange();
    return;
  }

  // Reached top of View tree → register with LayoutController
  RegisterWithLayoutController();
}

bool ViewDataImpl::IsLayoutPassOnStack()
{
  return gActiveLayoutPassDepth != 0u;
}

void ViewDataImpl::InvalidateMeasureFromPublicApi()
{
  InvalidateMeasureFromPublicApi("View::InvalidateMeasure");
}

void ViewDataImpl::InvalidateMeasureFromPublicApi(const char* apiName)
{
  if(gActiveLayoutPassDepth != 0u || LayoutInvalidation::IsLayoutFinishedEmitInProgress())
  {
    LogInPassInvalidation(apiName);
  }

  // Always execute the complete invalidation transaction. During layout
  // processing the controller retains the propagated root as pending but
  // suppresses only the idle wake, matching dali-core's relayout policy.
  InvalidateMeasure();
}

void ViewDataImpl::InvalidateArrangeFromPublicApi()
{
  InvalidateArrangeFromPublicApi("View::InvalidateArrange");
}

void ViewDataImpl::InvalidateArrangeFromPublicApi(const char* apiName)
{
  if(gActiveLayoutPassDepth != 0u || LayoutInvalidation::IsLayoutFinishedEmitInProgress())
  {
    LogInPassInvalidation(apiName);
  }

  // See InvalidateMeasureFromPublicApi(): dirtying, cache invalidation,
  // pass-poisoning, ancestor propagation, and root registration all remain
  // intact; only the controller's self-wake is suppressed.
  InvalidateArrange();
}

void ViewDataImpl::RearmLayoutDirtyForAbortedPass()
{
  mMeasureDirty = true;
  mArrangeDirty = true;
}

void ViewDataImpl::LogInPassInvalidation(const char* apiName)
{
  // Per-view latch, deliberately never cleared. A call from inside layout processing is
  // a code defect at a fixed call site, not a runtime condition, so one diagnostic per
  // View says everything the developer needs; repeating it every frame would bury the
  // rest of the log. There is no global cap on top of the latch: a global cap would
  // leave a later offending View undiagnosed, which is the worse failure.
  if(mInPassInvalidationWarned)
  {
    return;
  }
  mInPassInvalidationWarned = true;

  // Identify the view as helpfully as the handle allows. The latch means this runs at
  // most once per View, so neither property read is on any hot path. Self() can still
  // hand back an EMPTY handle (a derived constructor invalidating before the
  // CustomActor exists), and reading a property off an empty handle aborts, so the
  // handle test comes first and "View" is the last-resort label.
  Dali::CustomActor self = mViewImpl.Self();
  Dali::String      viewName;
  if(self)
  {
    viewName = self.GetProperty<Dali::String>(Dali::Actor::Property::NAME);
    if(viewName.Empty())
    {
      viewName = self.GetTypeName();
    }
  }
  const char* name = viewName.Empty() ? "View" : viewName.CStr();

  // Which half of the layout processing window was open. A pass on the stack shadows
  // the emit half: an emit that re-entered a pass is reported as the pass it is in.
  const char* context = gActiveLayoutPassDepth != 0u
                          ? "while a Measure/Arrange pass is running"
                          : "from a LayoutFinished signal handler";

  DALI_LOG_ERROR(
    "%s() called on '%s' %s. The requested layout work was retained rather than "
    "discarded, but layout processing does not request another idle ProcessEvents cycle "
    "for work it produces itself. The work remains pending and LayoutFinished remains "
    "deferred until a later independently triggered ProcessEvents cycle services it. "
    "Avoid unconditional invalidation from layout callbacks.\n",
    apiName,
    name,
    context);
}

MeasuredSize ViewDataImpl::GetMeasuredSize() const
{
  return mMeasuredSize;
}

UiColor ViewDataImpl::GetBackgroundColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(mViewImpl.Self(), BACKGROUND_COLOR_BINDING_ID, outColor))
  {
    return outColor;
  }

  Property::Map    backgroundMap = mViewImpl.Self().GetProperty<Property::Map>(Ui::View::Property::BACKGROUND);
  Property::Value* typeValue     = backgroundMap.Find(Ui::VisualBasePropertyIndex::TYPE);
  int              type          = static_cast<int>(Ui::Integration::InternalVisualType::COLOR);
  if(typeValue && typeValue->Get(type) && type != static_cast<int>(Ui::Integration::InternalVisualType::COLOR))
  {
    return UiColor();
  }

  Property::Value* colorValue = backgroundMap.Find(Ui::VisualBasePropertyIndex::MIX_COLOR);
  Vector4          color;
  return colorValue && colorValue->Get(color) ? UiColor(color) : UiColor();
}

void ViewDataImpl::SetBackgroundColor(const UiColor& color)
{
  ClearGradientColorBinding(BACKGROUND_GRADIENT_BINDING_ID);
  if(!UpdateColorBindingInternal(BACKGROUND_COLOR_BINDING_ID, color))
  {
    SetColorBindingInternal(BACKGROUND_COLOR_BINDING_ID, color, ColorCallback::New(this, &ViewDataImpl::SetBackgroundColorInternal));
  }
  SetBackgroundColorInternal(color.GetRgba());
}

void ViewDataImpl::SetBackgroundImage(const Dali::String& url)
{
  if(url.Empty())
  {
    ClearBackground();
    return;
  }

  ClearBackgroundBinding();
  SetBackground(Internal::CreateImageVisualPropertyMap(url));
}

void ViewDataImpl::SetBackgroundGradient(const Gradient::Base& gradient)
{
  if(gradient.GetType() == Gradient::Type::NONE || gradient.GetStopNodes().Count() < 2u)
  {
    ClearBackground();
    return;
  }

  UiColorManager::Get().ClearBinding(mViewImpl.Self(), BACKGROUND_COLOR_BINDING_ID);
  if(!UpdateColorBindingInternal(BACKGROUND_GRADIENT_BINDING_ID, gradient))
  {
    SetColorBindingInternal(BACKGROUND_GRADIENT_BINDING_ID, gradient,
                            Callback<void(const Gradient::Base&)>::New(this, &ViewDataImpl::SetBackgroundGradientInternal));
  }
  SetBackgroundGradientInternal(gradient);
}

UiColor ViewDataImpl::GetColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(mViewImpl.Self(), COLOR_BINDING_ID, outColor))
  {
    return outColor;
  }
  return UiColor(mViewImpl.Self().GetProperty<Vector4>(Actor::Property::COLOR));
}

void ViewDataImpl::SetColor(const UiColor& color)
{
  if(!UpdateColorBindingInternal(COLOR_BINDING_ID, color))
  {
    SetColorBindingInternal(COLOR_BINDING_ID, color, ColorCallback::New(this, &ViewDataImpl::SetColorInternal));
  }
  SetColorInternal(color.GetRgba());
}

UiColor ViewDataImpl::GetCurrentColor() const
{
  return UiColor(mViewImpl.Self().GetCurrentProperty<Vector4>(Actor::Property::COLOR));
}

Vector4 ViewDataImpl::GetCornerRadius() const
{
  return mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);
}

void ViewDataImpl::SetCornerRadius(const Vector4& radius)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::CORNER_RADIUS, radius);
}

CornerRadiusPolicy ViewDataImpl::GetCornerRadiusPolicy() const
{
  return static_cast<CornerRadiusPolicy>(mViewImpl.Self().GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY));
}

void ViewDataImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::CORNER_RADIUS_POLICY, static_cast<int>(policy));
}

Vector4 ViewDataImpl::GetCornerSquareness() const
{
  return mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS);
}

void ViewDataImpl::SetCornerSquareness(const Vector4& squareness)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::CORNER_SQUARENESS, squareness);
}

float ViewDataImpl::GetBorderlineWidth() const
{
  return mViewImpl.Self().GetProperty<float>(Ui::View::Property::BORDERLINE_WIDTH);
}

void ViewDataImpl::SetBorderlineWidth(float width)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::BORDERLINE_WIDTH, width);
}

UiColor ViewDataImpl::GetBorderlineColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(mViewImpl.Self(), "BorderlineColor", outColor))
  {
    return outColor;
  }
  return mViewImpl.Self().GetProperty<Vector4>(Ui::View::Property::BORDERLINE_COLOR);
}

void ViewDataImpl::SetBorderlineColor(const UiColor& color)
{
  if(!UpdateColorBindingInternal("BorderlineColor", color))
  {
    SetColorBindingInternal("BorderlineColor", color, ColorCallback::New(this, &ViewDataImpl::SetBorderlineColorInternal));
  }
  SetBorderlineColorInternal(color.GetRgba());
}

float ViewDataImpl::GetBorderlineOffset() const
{
  return mViewImpl.Self().GetProperty<float>(Ui::View::Property::BORDERLINE_OFFSET);
}

void ViewDataImpl::SetBorderlineOffset(float offset)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::BORDERLINE_OFFSET, offset);
}

void ViewDataImpl::ClearBackground()
{
  UnregisterVisual(Ui::View::Property::BACKGROUND);
  ClearBackgroundBinding();

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
  {
    sizeNegotiatedViewImpl->RelayoutRequest();
  }
}

void ViewDataImpl::SetShadow(const Shadow& shadow)
{
  if(shadow == Shadow::None())
  {
    ClearShadow();
    return;
  }

  SetShadow(Dali::Ui::Extension::Shadow::CreatePropertyMap(shadow));
}

void ViewDataImpl::SetShadow(const ShadowStack& shadowStack)
{
  ClearShadow();
  const uint32_t shadowCount = shadowStack.GetShadowCount();
  for(uint32_t index = 0u; index < shadowCount; ++index)
  {
    AppendShadow(shadowStack.GetShadowAt(index));
  }
}

void ViewDataImpl::SetRequestedWidth(float width)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::REQUESTED_WIDTH, width);
}

float ViewDataImpl::GetRequestedWidth() const
{
  return mRequestedWidth;
}

void ViewDataImpl::SetRequestedHeight(float height)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::REQUESTED_HEIGHT, height);
}

float ViewDataImpl::GetRequestedHeight() const
{
  return mRequestedHeight;
}

void ViewDataImpl::SetMinimumWidth(float width)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MINIMUM_WIDTH, width);
}

float ViewDataImpl::GetMinimumWidth() const
{
  return mSizeConstraints ? mSizeConstraints->minWidth : 0.0f;
}

void ViewDataImpl::SetMinimumHeight(float height)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MINIMUM_HEIGHT, height);
}

float ViewDataImpl::GetMinimumHeight() const
{
  return mSizeConstraints ? mSizeConstraints->minHeight : 0.0f;
}

void ViewDataImpl::SetMaximumWidth(float width)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MAXIMUM_WIDTH, width);
}

float ViewDataImpl::GetMaximumWidth() const
{
  return mSizeConstraints ? mSizeConstraints->maxWidth : std::numeric_limits<float>::max();
}

void ViewDataImpl::SetMaximumHeight(float height)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MAXIMUM_HEIGHT, height);
}

float ViewDataImpl::GetMaximumHeight() const
{
  return mSizeConstraints ? mSizeConstraints->maxHeight : std::numeric_limits<float>::max();
}

void ViewDataImpl::SetMargin(const Insets& margin)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MARGIN, ToVector4(margin));
}

Insets ViewDataImpl::GetMargin() const
{
  return mMargin;
}

void ViewDataImpl::SetPadding(const Insets& padding)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::PADDING, ToVector4(padding));
}

Insets ViewDataImpl::GetPadding() const
{
  return mPadding;
}

void ViewDataImpl::SetLayoutMode(Ui::LayoutMode mode)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::LAYOUT_MODE, static_cast<int>(mode));
}

Ui::LayoutMode ViewDataImpl::GetLayoutMode() const
{
  return mLayoutMode;
}

void ViewDataImpl::SetLayoutTransition(LayoutTransition transition)
{
  // Detach: drop any pending ENTER / REORDER / REMOVE markers. Records
  // are only produced while a transition is attached (see OnChildAdd /
  // OnChildOrderChanged / Remove); a previously attached
  // transition could have left entries that we want to discard now so
  // a later re-attach does not surface them as a stale cause on the
  // next pass. In particular hasPendingChildRemoval is
  // consumed only when StartTransitionsForView runs (which requires an
  // attached transition), so without this clear the marker would
  // survive a detach -> layout pass -> reattach cycle and tag the next
  // unrelated CHANGE as SIBLING_REMOVED.
  if(!transition)
  {
    mLayoutTransitionData.reset();
    // Symmetric with the direct markers above: drop any inherited-ENTER
    // candidates this view owns in the dispatcher, so a detach -> reattach
    // cycle does not surface a stale ENTER for a grand-child added under the
    // old transition. The records live in the per-window dispatcher; an
    // off-window view's records were already dropped by its scene-disconnect
    // cleanup (OnViewDestroyed).
    Window window = Window::Get(mViewImpl.Self());
    if(window)
    {
      LayoutController::Get(window).ClearPendingInheritedEnters(&mViewImpl);
    }
    return;
  }

  LayoutTransitionData& transitionData = EnsureLayoutTransitionData();
  transitionData.transition            = transition;

  // Attach: seed any pre-existing children as pending ENTER candidates,
  // but only when this view has not yet completed its initial layout
  // pass. OnChildAdd only inserts into pendingEnterChildren when a
  // transition is already attached, so the order
  //   parent.Add(child); parent.SetLayoutTransition(transition);
  // would otherwise leave the child out of the pending set entirely.
  // Without this seed, the dispatcher's first pass would neither
  // dispatch ENTER nor settle a declarative ENTER spec onto the child,
  // leaving e.g. a child pre-set to opacity = 0 permanently invisible.
  //
  // Restricted to !mInitialLayoutDone so re-attaching a transition on a
  // view that has already been on screen does not retroactively classify
  // its already-visible children as initial-mount candidates.
  if(!mInitialLayoutDone)
  {
    for(auto& childView : mChildren)
    {
      transitionData.pendingEnterChildren.insert(&GetImpl(childView));
    }
  }
}

LayoutTransition ViewDataImpl::GetLayoutTransition() const
{
  return mLayoutTransitionData ? mLayoutTransitionData->transition : LayoutTransition();
}

LayoutRect ViewDataImpl::GetArrangedBounds() const
{
  return mArrangedBounds;
}

bool ViewDataImpl::HasArrangeResult() const
{
  return mArrangeResultAvailable;
}

bool ViewDataImpl::IsInitialLayoutDone() const
{
  return mInitialLayoutDone;
}

bool ViewDataImpl::IsInitialEnterSettled() const
{
  return mInitialEnterSettled;
}

void ViewDataImpl::MarkInitialEnterSettled()
{
  mInitialEnterSettled = true;
}

uint32_t ViewDataImpl::GetChildViewCount() const
{
  return static_cast<uint32_t>(mChildren.Count());
}

Ui::View ViewDataImpl::GetChildViewAt(uint32_t index) const
{
  if(index < mChildren.Count())
  {
    return mChildren[index];
  }
  return Ui::View();
}

Dali::Vector<Ui::View>& ViewDataImpl::GetChildren()
{
  return mChildren;
}

const Dali::Vector<Ui::View>& ViewDataImpl::GetChildren() const
{
  return mChildren;
}

int32_t ViewDataImpl::IndexOfChildView(Ui::View view) const
{
  if(!view)
  {
    return -1;
  }
  for(size_t i = 0; i < mChildren.Count(); ++i)
  {
    if(mChildren[i] == view)
    {
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

void ViewDataImpl::RemoveAll(Ui::RemovePolicy policy)
{
  // Operate on the actor children (consistent with the inherited
  // GetChildCount / GetChildAt). Snapshot up front: with ANIMATE_EXIT the
  // EXIT-ing View children stay attached as ghosts, so iterating the live
  // actor list would revisit them and never terminate. Each removal is routed
  // through the per-child Remove, which already handles this view's own EXIT
  // slot, an inherited SUBTREE-scope EXIT owner, and the in-flight-ghost
  // guard; OnChildRemove keeps mChildren in sync for the immediate paths.
  Actor              self = mViewImpl.Self();
  std::vector<Actor> snapshot;
  const uint32_t     count = self.GetChildCount();
  snapshot.reserve(count);
  for(uint32_t i = 0u; i < count; ++i)
  {
    snapshot.push_back(self.GetChildAt(i));
  }

  for(auto& childActor : snapshot)
  {
    Ui::View childView = Ui::View::DownCast(childActor);
    if(childView)
    {
      // View child: EXIT-aware removal honouring @p policy.
      Remove(childView, policy);
    }
    else
    {
      // Non-View actor child: no EXIT transition applies, unparent now.
      self.Remove(childActor);
    }
  }
}

void ViewDataImpl::Remove(Ui::View child, Ui::RemovePolicy policy)
{
  if(!child)
  {
    return;
  }

  const bool animateExit = (policy == Ui::RemovePolicy::ANIMATE_EXIT);

  // ANIMATE_EXIT only: if a LayoutTransition with an EXIT slot (spec OR
  // animator) is attached, hand the child off to the layout transition
  // dispatcher so the EXIT animation can play. IMMEDIATE (and the
  // no-EXIT-slot case) falls through to the immediate unparent below.
  Ui::LayoutTransition transition = GetLayoutTransition();
  bool                 deferred   = false;
  if(animateExit && transition)
  {
    auto&      impl      = Internal::GetImpl(transition);
    const bool hasExitFx = static_cast<bool>(impl.GetExitVisualSpec()) || impl.HasExitAnimator() || impl.HasActiveExitBoundsEffect();
    if(hasExitFx)
    {
      Actor  self   = mViewImpl.Self();
      Window window = Window::Get(self);
      if(window)
      {
        // Remove the child from this view's layout-tracking list and
        // invalidate so siblings flow into the freed slot during the next
        // layout pass. The child's Actor stays under this Actor so the
        // dispatcher can animate it before unparenting.
        ViewImpl& childImpl = GetImpl(child);
        auto      it        = std::find(mChildren.begin(), mChildren.end(), child);
        if(it != mChildren.end())
        {
          mChildren.Erase(it);
          LayoutTransitionData& transitionData = *mLayoutTransitionData;
          transitionData.pendingEnterChildren.erase(&childImpl);
          // Same rationale as the immediate-remove path's OnChildRemove:
          // a stale raw ViewImpl* in the reorder set could outlive its
          // child after deferred-remove EXIT and cause a future heap-
          // reused address to be misclassified as REORDERED. Erase
          // per-child here (not full clear) so the cause of any
          // siblings still pending reorder is preserved.
          transitionData.pendingReorderedChildren.erase(&childImpl);
          // Mark sibling removal so the dispatcher tags this pass's CHANGE
          // dispatches on the remaining siblings as SIBLING_REMOVED. Set
          // only when a transition is attached to avoid leaving stale
          // marker state on views without transitions.
          transitionData.hasPendingChildRemoval = true;
          InvalidateMeasure();

          // Only schedule the EXIT transition when @p child was actually a
          // tracked child. Calling Remove on a non-child must not fire
          // any DALi layout-transition lifecycle / animation; without this
          // guard a misuse would leave a ghost animation that fires
          // OnStart / OnFinished and races with
          // the actor's real parent.
          LayoutController::Get(window).ScheduleLayoutExit(&mViewImpl, child);
          deferred = true;
        }
      }
    }
  }

  if(!deferred)
  {
    // Guard against re-removing a child that is currently an EXIT ghost
    // under this view. Ghost detection: actor parent is still Self() (the
    // deferred-remove keeps the actor attached) AND the child has already
    // been removed from the logical children list (mChildren). Without
    // this guard, the second Remove bypasses the dispatcher
    // duplicate-EXIT guard and synchronously unparents the ghost, which
    // triggers OnSceneDisconnection -> CancelPendingExit/CancelActiveAnimator
    // and silently cancels the in-flight EXIT (no OnFinished, no fade).
    // The same applies when the parent's LayoutTransition has been replaced
    // or cleared between the first and second Remove -- the second
    // call cannot enter the deferred branch but the ghost is still in
    // flight under its original transition.
    if(child.GetParent() == mViewImpl.Self() &&
       std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end())
    {
      return;
    }

    Actor      selfActor      = mViewImpl.Self();
    Window     window         = Window::Get(selfActor);
    auto       it             = std::find(mChildren.begin(), mChildren.end(), child);
    const bool isCurrentChild = (it != mChildren.end());

    // Inherited (SUBTREE-scope) EXIT: this view does not handle EXIT through
    // its own transition (otherwise the deferred branch above would have run).
    // Walk up to the closest ancestor SUBTREE owner that carries an EXIT
    // effect; if found, defer the child to that owner. The actor stays under
    // this view -- the ghost's direct/visual parent -- while the owner's
    // transition drives the EXIT effect (INV-GHOST-UNDER-DIRECT-PARENT). The
    // closest-owner / standalone-boundary rules are enforced inside the
    // resolver, so a child claimed by a closer (non-SUBTREE or non-EXIT)
    // transition is not stolen by an ancestor.
    // ANIMATE_EXIT only: inherited (SUBTREE-scope) EXIT defer. IMMEDIATE skips
    // this and unparents synchronously below.
    if(animateExit && window && isCurrentChild)
    {
      ViewImpl* owner = Internal::FindGoverningSubtreeOwner(&mViewImpl, Internal::ReflowSlot::EXIT);
      if(owner)
      {
        ViewImpl& childImpl = GetImpl(child);
        mChildren.Erase(it);
        if(mLayoutTransitionData)
        {
          mLayoutTransitionData->pendingEnterChildren.erase(&childImpl);
          mLayoutTransitionData->pendingReorderedChildren.erase(&childImpl);
        }
        // Remaining siblings under THIS direct parent reflow into the freed
        // slot; tag their CHANGE as SIBLING_REMOVED on the next pass -- but only
        // when THIS view owns a transition to consume the marker. For an
        // inherited EXIT this view may have no transition, and the marker --
        // consumed only by a transition-bearing view's layout pass -- would
        // never be cleared and would mis-tag a future CHANGE if it later gains
        // one.
        if(transition)
        {
          mLayoutTransitionData->hasPendingChildRemoval = true;
        }
        InvalidateMeasure();
        LayoutController::Get(window).ScheduleLayoutExit(&mViewImpl, child, owner);
        return;
      }
    }

    // Mark sibling removal for the next CHANGE pass when a transition is
    // attached (without an EXIT slot) AND we have a window. The
    // remaining children may reflow and should be tagged with
    // SIBLING_REMOVED. Skip the marker when no transition is attached,
    // or when no window is available -- without a window the marker
    // cannot be consumed by the dispatcher in this pass (no layout
    // pass runs), so it would leak across a later add-to-window event
    // and mis-tag the first layout pass's CHANGE as SIBLING_REMOVED.
    if(transition && window && isCurrentChild)
    {
      mLayoutTransitionData->hasPendingChildRemoval = true;
    }
    selfActor.Remove(child);
  }
}

uint32_t ViewDataImpl::ComputeLogicalChildIndex(const Actor& child) const
{
  Actor          self            = mViewImpl.Self();
  const uint32_t actorChildCount = self.GetChildCount();

  // Fast path: Actor::Add appends, so the new child is the last actor child
  // and every logical sibling precedes it -- the logical index is simply the
  // current logical child count. This is the overwhelmingly common add path
  // and stays O(1).
  if(actorChildCount > 0u && self.GetChildAt(actorChildCount - 1u) == child)
  {
    return static_cast<uint32_t>(mChildren.Count());
  }

  // General path (a fresh Actor::InsertAbove / InsertBelow): dali-core has
  // already placed the child at its FINAL actor position before notifying us,
  // so the logical index is the number of actor children before it that are
  // logical children of this view. The predicate mirrors
  // OnChildOrderChanged's rebuild filter -- a View that is present in
  // mChildren -- so it skips non-View actor children (never tracked) and
  // in-flight EXIT ghosts (actor-parented but erased from mChildren).
  uint32_t logicalIndex = 0u;
  for(uint32_t i = 0u; i < actorChildCount; ++i)
  {
    Actor actorChild = self.GetChildAt(i);
    if(actorChild == child)
    {
      break;
    }

    Ui::View siblingView = Ui::View::DownCast(actorChild);
    if(siblingView && std::find(mChildren.begin(), mChildren.end(), siblingView) != mChildren.end())
    {
      ++logicalIndex;
    }
  }

  // If @p child is not an actor child at all (defensive -- this hook is only
  // reached from Actor::Add / InsertAbove / InsertBelow after the child is in
  // place), the loop counts every logical child and the result is
  // mChildren.Count(), i.e. a plain append.
  return logicalIndex;
}

void ViewDataImpl::Raise(Ui::LayoutOrderPolicy policy)
{
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.Raise();
      return;
    }
  }
  self.Raise();
}

void ViewDataImpl::Lower(Ui::LayoutOrderPolicy policy)
{
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.Lower();
      return;
    }
  }
  self.Lower();
}

void ViewDataImpl::RaiseToTop(Ui::LayoutOrderPolicy policy)
{
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.RaiseToTop();
      return;
    }
  }
  self.RaiseToTop();
}

void ViewDataImpl::LowerToBottom(Ui::LayoutOrderPolicy policy)
{
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.LowerToBottom();
      return;
    }
  }
  self.LowerToBottom();
}

void ViewDataImpl::RaiseAbove(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  if(!target)
  {
    return;
  }
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.RaiseAbove(target);
      return;
    }
  }
  self.RaiseAbove(target);
}

void ViewDataImpl::LowerBelow(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  if(!target)
  {
    return;
  }
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(ViewDataImpl::Get(GetImpl(parent)));
      self.LowerBelow(target);
      return;
    }
  }
  self.LowerBelow(target);
}

void ViewDataImpl::OnChildAdded(Actor& child, bool allowNonViewChild)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    // Place the child at the logical position matching its actor position.
    // A fresh Actor::InsertAbove / InsertBelow emits no ChildOrderChangedSignal
    // (the child had no previous order), so this hook is the only chance to
    // keep both orders in sync; Actor::Add takes the O(1) append fast path.
    const uint32_t logicalIndex = ComputeLogicalChildIndex(child);
    if(logicalIndex >= mChildren.Count())
    {
      mChildren.PushBack(view);
    }
    else
    {
      mChildren.Insert(mChildren.Begin() + logicalIndex, view);
    }

    ViewImpl& childImpl = GetImpl(view);

    // If this child still has an in-flight transition under an old parent
    // (reparent during EXIT), cancel it before we mark the child for
    // ENTER under this view. Otherwise the orphan callback / animation
    // would keep driving the actor against the old parent's coord system.
    {
      Actor  self   = mViewImpl.Self();
      Window window = Window::Get(self);
      if(window)
      {
        auto& controller = LayoutController::Get(window);
        controller.NotifyChildReparented(&childImpl);

        // Inherited (SUBTREE-scope) ENTER: when THIS view has no transition of
        // its own, a child added here is not recorded for direct ENTER (the
        // gate below requires this view's transition). Notify the dispatcher so
        // it can walk up to the closest ancestor SUBTREE owner with an ENTER
        // effect and register an inherited-ENTER candidate. When this view HAS
        // a transition it is the closest owner and the direct path below claims
        // the child, so the inherited walk is skipped here.
        if(!HasLayoutTransition())
        {
          controller.NotifyChildAdded(&mViewImpl, view);
        }
      }
    }

    // Mark this child for ENTER-slot dispatch only when this view has a
    // LayoutTransition attached at the time of the add. Recording
    // unconditionally would (a) accumulate stale entries while no
    // transition is attached, ready to mis-fire as ENTER once a future
    // SetLayoutTransition + unrelated layout pass runs, and (b) keep
    // raw ViewImpl* pointers alive in the set after the child is
    // destroyed (no global cleanup hook today). Recording at the event
    // time matches the semantic that ENTER is for "child added under a
    // transition-bearing parent".
    if(HasLayoutTransition())
    {
      mLayoutTransitionData->pendingEnterChildren.insert(&childImpl);
    }

    // Standalone children do not contribute to this view's OnMeasure/OnArrange
    // accumulation, so adding one does not invalidate this view's cached
    // measured size or arranged bounds. Skip self-invalidation in that case.
    const bool childAffectsSelf = !IntegrationView::IsLayoutModeStandalone(childImpl);

    // Reset the effective scale cache for the entire subtree being added.
    // The child may be arriving from a parent with a different UiScalePolicy
    // context (e.g. reparented from DISABLED to INHERIT), so every descendant's
    // cached mEffectiveScale is potentially stale and must be recomputed.
    //
    // InvalidateMeasure() alone is not sufficient: it only resets
    // mEffectiveScale on the direct child and propagates *upward*. Descendants
    // retain their old cached values and use them in the next Measure() call,
    // producing incorrect font sizes, paddings, and decorations even though
    // the layout container size updates correctly.
    //
    // ResetSubtreeScaleAndLayoutCaches() clears the effective-scale sync bit
    // (mEffectiveScaleValid) and both layout caches (mMeasureCacheValid and
    // mArrangeCacheValid) for every node in the subtree, guaranteeing:
    //   (a) scale is recomputed from the new parent chain on next GetEffectiveScale(), and
    //   (b) the invalid measure cache forces a cache miss in Measure() so all
    //       nodes fully re-measure with the new scale.
    //
    // After this, InvalidateMeasure() marks the direct child dirty and
    // propagates up to the new layout root. Invalidation always propagates and
    // registers now (there is no dirty short-circuit), so this reaches the new
    // root regardless of the child's prior dirty state.
    ViewDataImpl::Get(childImpl).ResetSubtreeScaleAndLayoutCaches();

    // Retract the moved child's arrange RESULT record, not only its caches.
    // mArrangedBounds is a parent-local rect, so under a NEW parent the record
    // describes nothing; yet mArrangeResultAvailable would otherwise stay true
    // forever, and it is the bit three consumers key on:
    //  - the arrange cache-HIT gate treats any result-holding child as part of
    //    the replay set and requires its cache to be valid -- a child this
    //    parent's producer never arranges can never revalidate, so ONE
    //    reparented-in child would keep a producer that ignores its children
    //    (a Label, a leaf-style OnArrange) out of the hit PERMANENTLY;
    //  - the replay itself would re-apply the OLD parent's rect;
    //  - the RTL resolver would mirror the OLD parent's logical x against the
    //    NEW parent's width.
    // Clearing it makes the child "never arranged" again, which is exactly what
    // it is in this parent's coordinate space: the gate and the replay skip it,
    // and the first pass in which any producer arranges it re-establishes the
    // record. Direct child only, NOT recursive -- descendants' rects are
    // relative to the child and stay meaningful -- and deliberately NOT inside
    // ResetSubtreeScaleAndLayoutCaches(), whose other callers (scale changes,
    // scene reconnection) move no view between parents.
    ViewDataImpl::Get(childImpl).mArrangeResultAvailable = false;

    // And retract the one-shot ENTER settle latch, for the same reason and in the same
    // breath. A reparent is the ONLY way the transition that governs this child -- and
    // therefore the ENTER spec SettleInitialEnter baked onto it -- can change, so a
    // latch set under the old parent must not suppress the settle the NEW parent's spec
    // needs. Scoped to the direct child exactly like the retraction above, and for the
    // same reason: this is the node whose governing transition this call moves.
    ViewDataImpl::Get(childImpl).mInitialEnterSettled = false;

    // Invalidate the child's measure cache -- its previous cache was computed
    // under a different parent's constraints and is no longer reliable.
    // Through the internal primitive, not ViewImpl::InvalidateMeasure(): this is
    // a framework-internal consistency invalidation, not an application call.
    ViewDataImpl::Get(childImpl).InvalidateMeasure();

    // No self-invalidation here for a CONTRIBUTING child, and none is needed: the child's
    // own InvalidateMeasure() above reaches this view and does the whole job.
    //
    // That is a guarantee, not a likelihood. The generation short-circuit
    // (InvalidateMeasure's `mMeasurePropagationGeneration == generation` return) is the
    // only thing that could stop the walk short, and ResetSubtreeScaleAndLayoutCaches()
    // above zeroed the whole added subtree's propagation records -- 0 is the "never
    // propagated" sentinel that no live generation ever equals (the counter starts at 1
    // and skips 0 on wrap). The child's walk therefore always runs in full, and dali-core
    // has already parented the child (Actor::SetParent precedes OnChildAdd), so
    // GetParentLayout()/GetParentView() resolve to THIS view. When the walk arrives here
    // it drops this view's cached scale, retracts BOTH layout caches, poisons an
    // in-progress pass and raises both dirty bits before any short-circuit is even
    // consulted -- everything the removed call did.
    //
    // UtcDaliViewAddConfiguredOffSceneChildReMeasuresParentP and
    // UtcDaliViewReparentChildInSameBatchReMeasuresNewParentChainP pin it.
    if(!childAffectsSelf)
    {
      // Adding a standalone child changes neither this view's measured size nor its
      // arranged bounds -- but it DOES add work to this view's ARRANGE pass:
      // ArrangeStandaloneChildren must place the new child, and that placement lives
      // on the arrange path only. Without a signal here, an arrange cache entry
      // published for a child set that did not include this child stays live, and
      // the arrange cache HIT would serve it and skip the placement entirely.
      //
      // The predicate's !HasUnconsumedStandaloneChild() term does NOT cover this
      // case: mMeasuredSlotUnconsumed is false until the child's first measure
      // publish, so a freshly added, never-measured standalone child does not raise
      // it. The add-side invalidation is what closes the gap.
      //
      // InvalidateArrange() rather than InvalidateMeasure() is the exact signal: the
      // measured size genuinely is unaffected (see the comment above), while
      // InvalidateArrange retracts the arrange cache entry, marks the arrange dirty
      // and registers a pass -- respecting the standalone boundary rule on the way
      // up. It is idempotent, and the transition branch below is strictly stronger,
      // so the two compose.
      InvalidateArrange();

      // No parent InvalidateMeasure() for the standalone+transition combination either.
      // The child's InvalidateMeasure() above reaches its own boundary branch, which
      // carries exactly this hook: a standalone view whose parent has a LayoutTransition
      // attached invalidates that parent's measure before self-registering, so the
      // dispatcher's CaptureBeforeLayout / StartTransitionsAfterLayout pass runs in the
      // same layout batch as the child's. Same predicate (GetLayoutTransition() and
      // HasLayoutTransition() read the same member) and same action (the internal
      // primitive on this very view), and it is reachable for the same reason as the
      // contributing case above: the propagation record was zeroed a few lines up, so the
      // walk cannot be short-circuited before it gets there.
      // UtcDaliLayoutTransitionAddStandaloneChildToTransitionParentDispatchesEnterP
      // pins it.
    }
  }
  else
  {
    if(allowNonViewChild)
    {
      // Permitted via IntegrationView::AddActorChild: skip the View-only check
      // and do not record this child in mChildren (it is excluded from layout).
      return;
    }
    DALI_ASSERT_ALWAYS(false && "View could only have child as View class!");
  }
}

void ViewDataImpl::OnChildRemoved(Actor& child)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    auto it = std::find(mChildren.begin(), mChildren.end(), view);
    if(it != mChildren.end())
    {
      ViewImpl& childImpl = GetImpl(view);

      // Standalone children are excluded from this view's OnMeasure/OnArrange
      // accumulation, so removing one does not change this view's measured
      // size or arranged bounds. Skip self-invalidation in that case to avoid
      // an unnecessary parent chain walk.
      const bool childWasAffectingSelf = !IntegrationView::IsLayoutModeStandalone(childImpl);

      // If the child was added and removed within the same frame (before
      // any layout pass consumed the pending-enter set), drop the ENTER
      // marker so the dispatcher does not fire on a no-longer-present view.
      // Same for the reorder marker: OnChildOrderChanged keeps
      // raw ViewImpl* pointers in pendingReorderedChildren which must
      // not survive the child's removal -- otherwise a heap-reused address
      // could mis-classify a future child as REORDERED.
      if(mLayoutTransitionData)
      {
        mLayoutTransitionData->pendingEnterChildren.erase(&childImpl);
        mLayoutTransitionData->pendingReorderedChildren.erase(&childImpl);
      }

      // Record sibling removal so the next CHANGE pass tags remaining
      // siblings as SIBLING_REMOVED. This covers paths that reach
      // OnChildRemove without going through View::Remove's marker-
      // setting branch (e.g. inherited Actor::Remove called directly on
      // the view actor). Same window guard as View::Remove -- without
      // a window the marker cannot be consumed in this pass and would
      // leak across a later add-to-window event. Setting the marker
      // here is idempotent with View::Remove's own setter.
      if(HasLayoutTransition() && Window::Get(mViewImpl.Self()))
      {
        mLayoutTransitionData->hasPendingChildRemoval = true;
      }

      // Removal re-roots the removed subtree's effective-scale chain: an INHERIT
      // node's ComputeEffectiveScale walks its parent chain, and severing this
      // link re-roots that chain (ultimately at UiScaleManager's global scale).
      // So the whole removed subtree's cached effective scale is now stale. The
      // InvalidateMeasure() below drops it on the direct child ONLY and
      // propagates upward, never into the subtree -- the mirror of OnChildAdded's
      // recursive reset. Without this, a subtree removed from a scale-DISABLED
      // parent and re-added under a plain Actor / Window (which fires no
      // ViewDataImpl::OnChildAdded) renders torn: the root at the new scale, its
      // descendants at the old one.
      ViewDataImpl::Get(childImpl).ResetSubtreeScaleAndLayoutCaches();

      // Invalidate the removed child's measure cache so that it gets
      // re-measured when re-parented to a different container.
      // Note: Actor parent-child relationship is already severed at this
      // point, so child's InvalidateMeasure cannot propagate to us.
      // Through the internal primitive, not ViewImpl::InvalidateMeasure(): this
      // is a framework-internal consistency invalidation, not an application call.
      ViewDataImpl::Get(childImpl).InvalidateMeasure();
      mChildren.Erase(it);

      if(childWasAffectingSelf)
      {
        InvalidateMeasure();
      }
    }
  }
}

void ViewDataImpl::OnViewSceneConnection()
{
  OnSceneConnection();

  if(auto* traitObject = GetCoreInteractionObject())
  {
    if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
    {
      interactiveTraitImpl->OnSceneConnection(View::DownCast(mViewImpl.Self()));
    }
  }

  Actor self(mViewImpl.Self());
  int   clippingMode = ClippingMode::DISABLED;
  if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode) &&
     clippingMode == ClippingMode::CLIP_CHILDREN &&
     (DALI_UNLIKELY(!mVisualData) || mVisualData->mVisuals.Empty()) &&
     self.GetRendererCount() == 0u)
  {
    mViewImpl.SetBackgroundColor(Color::TRANSPARENT);
  }

  // Register as a layout root if this view is:
  //   (a) the top of the view tree (no parent view), OR
  //   (b) a standalone (boundary) view whose invalidation does not propagate
  //       to its parent. Such a view must self-register so its pending layout
  //       work is processed by the LayoutController on the current window.
  //
  // The boundary case matters when a view becomes dirty while off-scene
  // (RegisterWithLayoutController silently no-ops without a window). Once
  // connected to a scene here, it must register so the pending state is
  // picked up in the new window's controller.
  const bool isLayoutRoot = !GetParentView();

  // Off-scene scale gap. While this view is a detached layout root it is not in
  // UiScaleManager's root set, so a global SetScale() cannot reach it -- its
  // cached effective scale, and the whole subtree's for INHERIT chains, may be
  // stale (computed against the scale in force before it was detached). On
  // (re)connection re-derive the subtree's effective scale so the layout pass
  // registered below measures and arranges at the CURRENT scale. On a first-ever
  // connection this is a cheap no-op (the caches are empty and the scale merely
  // recomputes to the same value). This is the mirror, for the Window/Actor
  // remove-then-add path, of OnChildRemoved's recursive invalidation for the
  // View reparent path.
  if(isLayoutRoot)
  {
    ResetSubtreeScaleAndLayoutCaches();
  }

  const bool isDirty = mMeasureDirty || mArrangeDirty;
  if(isLayoutRoot || (IntegrationView::IsLayoutModeStandalone(mViewImpl) && isDirty))
  {
    RegisterWithLayoutController();
  }
  else if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    // Standalone but clean: no layout pass needed, but must be tracked by
    // UiScaleManager so future scale changes reach this view. This covers the
    // case where a standalone view was unregistered on scene-disconnection and
    // reconnects without any pending dirty work.
    Window window = Window::Get(mViewImpl.Self());
    if(window)
    {
      UiScaleManager::Get().RegisterLayoutRoot(View::DownCast(mViewImpl.Self()));
    }
  }
}

void ViewDataImpl::OnViewSceneDisconnection()
{
  // When a view leaves the scene (including being removed from a parent or
  // when its window is destroyed), remove any pending LayoutController
  // registration. Otherwise the controller would carry a stale entry whose
  // parent-chain is no longer in this window, and the next layout pass
  // would process a view that is effectively orphaned. The view's destructor
  // already calls UnregisterFromAll as a last resort, but doing it here
  // avoids stale pending work between disconnect and destruction.
  LayoutController::UnregisterFromAll(&mViewImpl);

  if(auto* traitObject = GetCoreInteractionObject())
  {
    if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
    {
      interactiveTraitImpl->OnSceneDisconnection(View::DownCast(mViewImpl.Self()));
    }
  }

  OnSceneDisconnection();

  // Remove from UiScaleManager if this view was registered as a layout root.
  // Two cases match the registration paths in RegisterWithLayoutController:
  //   (a) tree root: no parent view and no parent layout
  //   (b) standalone: boundary views self-register regardless of their parent
  if((!GetParentView() && !GetParentLayout()) || IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    UiScaleManager::Get().UnregisterLayoutRoot(View::DownCast(mViewImpl.Self()));
  }
}

void ViewDataImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  // If the clipping mode has been set, we may need to create a renderer.
  // Only do this if we are already on-stage as the OnSceneConnection will handle the off-stage clipping controls.
  switch(index)
  {
    case Actor::Property::CLIPPING_MODE:
    {
      if(mViewImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        Actor self(mViewImpl.Self());
        int   clippingMode = ClippingMode::DISABLED;
        if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode) &&
           clippingMode == ClippingMode::CLIP_CHILDREN &&
           (DALI_UNLIKELY(!mVisualData) || mVisualData->mVisuals.Empty()) &&
           self.GetRendererCount() == 0u)
        {
          mViewImpl.SetBackgroundColor(Color::TRANSPARENT);
        }
      }
      break;
    }
    case Actor::Property::ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      View       self    = View::DownCast(mViewImpl.Self());

      if(!enabled && self == Dali::Ui::FocusManager::Get().GetCurrentFocusView())
      {
        Dali::Ui::FocusManager::Get().ClearFocus();
      }

      ExtensionView::SetState(mViewImpl, ViewState::DISABLED, !enabled);

      if(auto* traitObject = GetCoreInteractionObject())
      {
        if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
        {
          interactiveTraitImpl->OnEnabledChanged(self, enabled);
        }
      }
      break;
    }
    // The layout-direction WRITE interception: this is what covers a mid-tree View
    // that is not a layout root and therefore holds no signal hook of its own.
    // LAYOUT_DIRECTION does reach here even though it is a core DEFAULT property --
    // Object::SetProperty invokes this virtual for those too -- and every documented
    // way of moving the direction lands on one of these three indices
    // (Actor::SetLayoutDirection routes through SetProperty on the first).
    // DevelActor::Property::LAYOUT_DIRECTION is an alias of the same index, so it is
    // deliberately not listed a second time.
    case Dali::Actor::Property::LAYOUT_DIRECTION:
    case Dali::DevelActor::Property::LAYOUT_DIRECTION_LEGACY:
    case Dali::DevelActor::Property::INHERIT_LAYOUT_DIRECTION_LEGACY:
    {
      // A hooked layout root has already been covered: the signal fires inside the
      // core inherit walk, BEFORE OnPropertySet, whenever the resolved direction
      // actually moved. Skip the second walk.
      if(mLayoutDirectionSignalConnected)
      {
        break;
      }

      // Redundant-write guard. mLastArrangeDirection is the direction the last
      // published arrange used; at this point the resolved value is already the
      // new one, so equality means nothing moved -- and if THIS view's resolved
      // direction did not move, no inheriting descendant's did either.
      if(mArrangeCacheValid &&
         mLastArrangeDirection == mViewImpl.Self().GetEffectiveLayoutDirection())
      {
        break;
      }

      InvalidateSubtreeLayoutForDirectionChange();
      InvalidateMeasure();
      break;
    }
  }
}

void ViewDataImpl::OnSizeSet(const Vector3& targetSize)
{
  mSize = Vector2(targetSize);

  // Notify that size or UiScale changed
  SizeOrUiScaleChanged();
}

void ViewDataImpl::OnSizeAnimation(Animation& animation)
{
  // @todo size negotiate background to new size, animate as well?

  // TODO : Could we clear animation constraint when size animation stopped?
  CreateAnimationConstraints(animation.GetBaseObject(), Dali::Actor::Property::SIZE);
}

void ViewDataImpl::OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Animation::State state)
{
  if(state == Animation::State::PLAYING)
  {
    CreateAnimationConstraints(animation.GetBaseObject(), index);
  }
  else if(state == Animation::State::STOPPED)
  {
    ClearAnimationConstraints(animation.GetBaseObject(), index);
  }
}

void ViewDataImpl::OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied)
{
  if(applied)
  {
    CreateAnimationConstraints(constraint.GetBaseObject(), index);
  }
  else
  {
    ClearAnimationConstraints(constraint.GetBaseObject(), index);
  }
}

void ViewDataImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(!mRenderEffectData)
  {
    return;
  }

  if(mRenderEffectData->renderEffect)
  {
    mRenderEffectData->renderEffect->GetOffScreenRenderTasks(tasks, isForward);
  }
  if(mRenderEffectData->offScreenRendering)
  {
    mRenderEffectData->offScreenRendering->GetOffScreenRenderTasks(tasks, isForward);
  }
}

Dali::Texture ViewDataImpl::GetOffScreenRenderingOutput() const
{
  if(!mRenderEffectData ||
     mRenderEffectData->offScreenRenderingType != Ui::View::OffScreenRenderingType::REFRESH_ONCE)
  {
    DALI_LOG_ERROR(
      "Precondition unsatisfied: Set property OFFSCREEN_RENDERING to OffScreenRenderingType::REFRESH_ONCE\n");
    return Dali::Texture();
  }
  return mRenderEffectData->offScreenRendering->GetTexture();
}

Vector3 ViewDataImpl::GetBackgroundVisualNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ViewDataImpl::GetBackgroundVisualNaturalSize for %s\n",
                mViewImpl.Self().GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
  Ui::Internal::Visual::Base* visualImplPtr = GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr)
  {
    Vector2 naturalSize;
    visualImplPtr->GetNaturalSize(naturalSize);
    naturalSize.width += (mPadding.start + mPadding.end);
    naturalSize.height += (mPadding.top + mPadding.bottom);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

void ViewDataImpl::SetMeasureCallback(MeasureCallback callback)
{
  EnsureLayoutCallbacksObject(*this)->SetMeasureCallback(std::move(callback));
  InvalidateMeasure();
}

void ViewDataImpl::SetArrangeCallback(ArrangeCallback callback)
{
  // The one-argument API uses the framework default policy.
  SetArrangeCallback(std::move(callback), ArrangePolicy::IF_CHANGED);
}

void ViewDataImpl::SetArrangeCallback(ArrangeCallback callback, ArrangePolicy policy)
{
  EnsureLayoutCallbacksObject(*this)->SetArrangeCallback(std::move(callback));

  mArrangeCallbackAlways = (policy == ArrangePolicy::ALWAYS);
  RefreshArrangeProducerPolicy();

  // Pair the policy change with invalidation so a view that has already
  // published a cache entry cannot be served from it under the new policy.
  InvalidateArrange();
}

void ViewDataImpl::SetArrangePolicy(ArrangePolicy policy)
{
  // Value guard, matching the manager side, which already refuses to notify its owner for
  // a policy that does not move (LayoutManager::SetArrangePolicy ->
  // LayoutManager::Impl::SetArrangePolicy returns false). Re-selecting the policy a view
  // already has retracts nothing and schedules nothing. Safe to return early: the derived
  // bit is refreshed by every path that can change WHICH producer is active -- the two
  // mutators and the public trait swap funnel (OnTraitChanged) -- so it is never stale in
  // a way a redundant policy write would have repaired.
  const bool overrideAlways = (policy == ArrangePolicy::ALWAYS);
  if(overrideAlways == mArrangeOverrideAlways)
  {
    return;
  }
  mArrangeOverrideAlways = overrideAlways;

  // The cache predicate reads only the EFFECTIVE producer policy, and this override is
  // the third-ranked input to it (callback > LayoutManager > OnArrange). While a callback
  // or a manager is installed, changing the override moves nothing the predicate can see,
  // so compare the derived bit across the refresh and invalidate only on a real move.
  const bool producerAlwaysBefore = mArrangeProducerAlways;

  // Safe with no handle: both lookups it performs are plain trait-table reads
  // (ViewDataImpl::GetTrait walks mTraits), so a derived implementation may select
  // its policy from its constructor.
  RefreshArrangeProducerPolicy();

  // Invalidate only when there is a published entry that the new policy could make
  // unsafe to serve. Before the CustomActor handle exists, InvalidateArrange() cannot
  // walk to a layout root; at that point mArrangeCacheValid is necessarily false.
  // InvalidateArrange() walks to the layout root, and the FIRST thing that walk does
  // on a handle-less view is GetParentLayout() -> mViewImpl.Self().GetParent(), where
  // Self() hands back an empty Actor and dali-core's GetImplementation(Actor&) aborts
  // on `DALI_ASSERT_ALWAYS(actor && "Actor handle is empty")`. (The later
  // RegisterWithLayoutController() -> Window::Get(self) would abort for the same
  // reason, but the walk never reaches it.)
  //
  // Skipping it there is sound, not merely convenient: mArrangeCacheValid is false
  // until a pass publishes, so at construction there is provably nothing to serve.
  // A later policy change on a settled view still invalidates, which is the case
  // the guard exists to keep.
  if(mArrangeProducerAlways != producerAlwaysBefore && mArrangeCacheValid)
  {
    InvalidateArrange();
  }
}

ArrangePolicy ViewDataImpl::GetArrangePolicy() const
{
  return mArrangeOverrideAlways ? ArrangePolicy::ALWAYS : ArrangePolicy::IF_CHANGED;
}

void ViewDataImpl::OnLayoutManagerArrangePolicyChanged()
{
  // The manager owns the policy, while the hot cache predicate keeps a derived bit
  // on its View. Keep those two pieces of state synchronized for policy changes made
  // after the manager has been attached.
  //
  // The manager side has already dropped a same-value write, so what reaches here is
  // always a real change to the MANAGER's policy -- but not necessarily to the EFFECTIVE
  // one: an installed ArrangeCallback outranks the manager, so a policy change on an
  // INACTIVE manager must retract nothing.
  const bool producerAlwaysBefore = mArrangeProducerAlways;

  RefreshArrangeProducerPolicy();

  // Gated exactly as in SetArrangePolicy(), and for the same two reasons: with no
  // published entry there is provably nothing the new policy could make unsafe to
  // serve, and a manager attached before the CustomActor handle exists must be able
  // to change its policy without InvalidateArrange() walking off an empty Self().
  if(mArrangeProducerAlways != producerAlwaysBefore && mArrangeCacheValid)
  {
    InvalidateArrange();
  }
}

void ViewDataImpl::RefreshArrangeProducerPolicy()
{
  // Mirrors the producer dispatch order in Arrange(): callback > LayoutManager >
  // OnArrange. The trait lookups below (GetArrangeCallback / GetLayoutManager) are
  // why this is computed HERE, at the rare mutation points, and not inside the
  // per-pass hit predicate.
  if(GetArrangeCallback() != nullptr)
  {
    mArrangeProducerAlways = mArrangeCallbackAlways;
  }
  else if(LayoutManager* manager = mViewImpl.GetLayoutManager())
  {
    // The manager is the active producer, so its own policy replaces the view's
    // OnArrange policy. Manager policy is stored outside the frozen virtual API.
    mArrangeProducerAlways = (manager->GetArrangePolicy() == ArrangePolicy::ALWAYS);
  }
  else
  {
    mArrangeProducerAlways = mArrangeOverrideAlways;
  }
}

MeasureCallback* ViewDataImpl::GetMeasureCallback()
{
  auto* object = GetLayoutCallbacksObject(*this);
  return object ? object->GetMeasureCallback() : nullptr;
}

ArrangeCallback* ViewDataImpl::GetArrangeCallback()
{
  auto* object = GetLayoutCallbacksObject(*this);
  return object ? object->GetArrangeCallback() : nullptr;
}

void ViewDataImpl::AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager)
{
  DALI_ASSERT_ALWAYS(manager && "AttachLayoutManager requires a non-null LayoutManager.");
  DALI_ASSERT_ALWAYS(!HasLayoutManager() && "LayoutManager already set. Cannot replace an existing LayoutManager.");

  IntrusivePtr<TraitObject> object(new LayoutManagerObject(std::move(manager)));
  IntegrationView::SetTrait(mViewImpl, Integration::ReservedTraitId::LAYOUT_MANAGER, object);

  // Wire the back-pointer that lets the manager invalidate US from its own setters
  // (LayoutManager::InvalidateOwnerMeasure / InvalidateOwnerArrange). Without it a
  // manager's private state -- a stack orientation, a grid's row definitions --
  // could change with nothing to retract the measure/arrange result computed against
  // the old value, and the arrange cache would keep serving that result. Read back
  // through GetLayoutManager() rather than kept from the argument, which was moved
  // into the trait object above.
  //
  // One-shot: the assert above makes attach a single transition, so there is no
  // detach to clear this on. The pointee also strictly outlives the pointer -- this
  // view owns the manager through the trait -- so the raw pointer cannot dangle.
  if(LayoutManager* attached = mViewImpl.GetLayoutManager())
  {
    attached->SetOwnerView(&mViewImpl);
  }

  // The producer may have just changed from OnArrange to the manager (it does unless
  // an ArrangeCallback outranks both), so the derived bit has to follow. The assert
  // above makes this a one-shot transition: a manager can never be replaced or
  // detached, so there is no reverse edge to mirror.
  RefreshArrangeProducerPolicy();

  InvalidateMeasure();
}

LayoutManager* ViewDataImpl::GetLayoutManager() const
{
  IntrusivePtr<TraitObject> object        = IntegrationView::GetTrait(mViewImpl, Integration::ReservedTraitId::LAYOUT_MANAGER);
  auto*                     managerObject = object ? static_cast<LayoutManagerObject*>(object.Get()) : nullptr;
  return managerObject ? managerObject->GetLayoutManager() : nullptr;
}

bool ViewDataImpl::HasLayoutManager() const
{
  return GetLayoutManager() != nullptr;
}

bool ViewDataImpl::HasLayoutCallback() const
{
  auto* object = GetLayoutCallbacksObject(const_cast<ViewDataImpl&>(*this));
  return object && (object->GetMeasureCallback() || object->GetArrangeCallback());
}

void ViewDataImpl::SetLayoutParams(const AbsoluteLayoutParams& params)
{
  // Value guard, as on the property setters: a re-write of the params already in
  // place has nothing to retract and nothing to schedule. Motivating recurrence:
  // ScrollBarImpl::SetVBarBounds re-writes its bar's AbsoluteLayoutParams on every
  // bar position/size update, and today only that function's own hand-rolled 0.01f
  // epsilon test -- commented "avoid infinite relayout loop" -- stops the write from
  // re-arming layout every pass. That check belongs on this side too.
  AbsoluteLayoutParams current;
  if(TryGetLayoutParams(current) && IsSameLayoutParams(current, params))
  {
    return;
  }

  IntrusivePtr<TraitObject> object(new AbsoluteLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const FlexLayoutParams& params)
{
  FlexLayoutParams current;
  if(TryGetLayoutParams(current) && IsSameLayoutParams(current, params))
  {
    return;
  }

  IntrusivePtr<TraitObject> object(new FlexLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const GridLayoutParams& params)
{
  GridLayoutParams current;
  if(TryGetLayoutParams(current) && IsSameLayoutParams(current, params))
  {
    return;
  }

  IntrusivePtr<TraitObject> object(new GridLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::GRID_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const StackLayoutParams& params)
{
  StackLayoutParams current;
  if(TryGetLayoutParams(current) && IsSameLayoutParams(current, params))
  {
    return;
  }

  IntrusivePtr<TraitObject> object(new StackLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::STACK_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

bool ViewDataImpl::TryGetLayoutParams(AbsoluteLayoutParams& params) const
{
  IntrusivePtr<TraitObject> object = GetTrait(Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS);
  if(!object)
  {
    return false;
  }
  auto* impl = dynamic_cast<AbsoluteLayoutParamsImpl*>(object.Get());
  DALI_ASSERT_ALWAYS(impl && "ABSOLUTE_LAYOUT_PARAMS trait must be an AbsoluteLayoutParamsImpl");
  params.SetBounds(impl->GetBounds()).SetFlags(impl->GetFlags());
  return true;
}

bool ViewDataImpl::TryGetLayoutParams(FlexLayoutParams& params) const
{
  IntrusivePtr<TraitObject> object = GetTrait(Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS);
  if(!object)
  {
    return false;
  }
  auto* impl = dynamic_cast<FlexLayoutParamsImpl*>(object.Get());
  DALI_ASSERT_ALWAYS(impl && "FLEX_LAYOUT_PARAMS trait must be a FlexLayoutParamsImpl");
  params.SetFlexGrow(impl->GetFlexGrow()).SetFlexShrink(impl->GetFlexShrink()).SetFlexBasis(impl->GetFlexBasis()).SetAlignSelf(impl->GetAlignSelf());
  return true;
}

bool ViewDataImpl::TryGetLayoutParams(GridLayoutParams& params) const
{
  IntrusivePtr<TraitObject> object = GetTrait(Integration::ReservedTraitId::GRID_LAYOUT_PARAMS);
  if(!object)
  {
    return false;
  }
  auto* impl = dynamic_cast<GridLayoutParamsImpl*>(object.Get());
  DALI_ASSERT_ALWAYS(impl && "GRID_LAYOUT_PARAMS trait must be a GridLayoutParamsImpl");
  params.SetRow(impl->GetRow()).SetColumn(impl->GetColumn()).SetRowSpan(impl->GetRowSpan()).SetColumnSpan(impl->GetColumnSpan()).SetHorizontalAlignment(impl->GetHorizontalAlignment()).SetVerticalAlignment(impl->GetVerticalAlignment());
  return true;
}

bool ViewDataImpl::TryGetLayoutParams(StackLayoutParams& params) const
{
  IntrusivePtr<TraitObject> object = GetTrait(Integration::ReservedTraitId::STACK_LAYOUT_PARAMS);
  if(!object)
  {
    return false;
  }
  auto* impl = dynamic_cast<StackLayoutParamsImpl*>(object.Get());
  DALI_ASSERT_ALWAYS(impl && "STACK_LAYOUT_PARAMS trait must be a StackLayoutParamsImpl");
  params.SetWeight(impl->GetWeight()).SetAlignment(impl->GetAlignment());
  return true;
}

void ViewDataImpl::SetRenderEffect(Ui::RenderEffect effect)
{
  ClearRenderEffect();

  if(effect)
  {
    RenderEffectImpl* object = dynamic_cast<RenderEffectImpl*>(effect.GetObjectPtr());
    DALI_ASSERT_ALWAYS(object && "Given render effect is not valid.");

    Dali::Ui::View ownerView(mViewImpl.GetOwner());
    object->SetOwnerView(ownerView);

    EnsureRenderEffectData().renderEffect = object;
  }
}

RenderEffect ViewDataImpl::GetRenderEffect() const
{
  return RenderEffect(mRenderEffectData ? mRenderEffectData->renderEffect.Get() : nullptr);
}

void ViewDataImpl::ClearRenderEffect()
{
  if(mRenderEffectData && mRenderEffectData->renderEffect)
  {
    RenderEffectImplPtr effectImpl = std::move(mRenderEffectData->renderEffect);

    // Reset handle first to avoid circular reference
    mRenderEffectData->renderEffect.Reset();

    effectImpl->ClearOwnerView();
  }
}

void ViewDataImpl::BlockArrangeCachePublishDuringPass()
{
  // Setting this outside a pass would latch a block that nothing clears until the
  // NEXT arrange entry (ArrangePassGuard clears it there), silently declining that
  // unrelated pass's publish. The only caller is the ancestor-invalidation walk,
  // which tests mArrangeInProgress first.
  DALI_ASSERT_DEBUG(mArrangeInProgress &&
                    "mArrangeCacheBlockedDuringPass is pass-local; only set it while arranging");
  mArrangeCacheBlockedDuringPass = true;
}

/**
 * Drop the ANCESTOR measure/arrange cache entries after a confirmed full
 * Measure() miss on this view.
 *
 * Why it is needed: a completed Measure() republishes this view's measured
 * slot UNCONDITIONALLY (see the "RESULT is published unconditionally" block in
 * Measure()), and every ancestor consumes that stored slot while arranging
 * (ArrangeDefault reads childImpl.GetMeasuredSize(), as do all five layout
 * managers). No producer on the MEASURE side reads a stored slot -- each
 * consumes the value Measure() returns -- so an ancestor's measured result is a
 * deterministic function of its own constraint, and re-running it reproduces
 * the same value. What is NOT reproducible is the slot: if an ancestor serves a
 * measure cache HIT it never re-measures this view, and then arranges it from
 * whatever the out-of-band Measure() left behind. Invalidating only the
 * ancestor ARRANGE cache would not help; the arrange would re-run and re-read
 * the same corrupted slot.
 *
 * This is therefore a CACHE-ONLY invalidation: it clears cache-valid bits and
 * nothing else. It deliberately does NOT touch mMeasureDirty / mArrangeDirty,
 * does not call InvalidateMeasure()/InvalidateArrange(), and does not
 * RegisterWithLayoutController(), so it cannot schedule a layout pass and
 * therefore cannot spin. It also does not touch mMeasurePassPoisoned /
 * mArrangePassPoisoned (which would trip the one-shot follow-up registration at
 * the end of a pass), mEffectiveScaleValid, or mEffectiveScale.
 *
 * Ownership is tested two ways, and the ascent stops at whichever applies
 * first: an explicit owner scope (LayoutDependency::Top(), pushed around every
 * arrange-time re-measure and around the RecyclerView/ItemsLayouter boundary),
 * plus the (a) measure-in-progress bit, which covers the plain top-down measure
 * recursion -- that recursion pushes no scope, so the bit is its only stop.
 * Iterative, never recursive; the actor tree is finite and acyclic and each
 * node is visited at most once, so this is O(depth) with no re-entry.
 */
void ViewDataImpl::InvalidateAncestorLayoutCachesForMeasureMiss()
{
  // Self is a standalone view: it is excluded from every ancestor's
  // OnMeasure/OnArrange ACCUMULATION (MeasureDefault and ArrangeDefault both skip
  // standalone children), so re-measuring it changes no ancestor's measured value
  // and its ancestors need no invalidation from here. The boundary stop in
  // InvalidateMeasure() / InvalidateArrange() is analogous, though those also do
  // transition / controller-registration work this cache-only walk omits.
  //
  // COMPLETE, not a partial stop. The one ancestor that does consume this view's
  // stored slot is ArrangeStandaloneChild(), and it no longer depends on an
  // ancestor cache clear to be correct: the publish at the end of every measure
  // pass marks the slot unconsumed (mMeasuredSlotUnconsumed), and the parent's next
  // arrange re-measures this view against its own extent before placing it. That
  // correction sits on the ARRANGE side on purpose -- it is reached even on a pass
  // where the parent's measure cache hits and MeasureStandaloneChildren never runs,
  // which is exactly the case an ancestor cache clear could not have fixed here
  // anyway. The arrange cache-HIT path does not open a hole in this: the parent's
  // hit predicate tests !HasUnconsumedStandaloneChild(), so a parent holding such a
  // child always misses and always reaches ArrangeStandaloneChildren.
  if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    return;
  }

  // (c) The explicit owner boundary. A producer that deliberately issues a
  // Measure() on a descendant while running its own pass pushes an owner frame
  // around exactly that call (LayoutDependency::ArrangeOwnedMeasureScope /
  // RecyclerLayoutOwnerScope), so it already accounts for the result and needs
  // no invalidation -- and neither does anything above it.
  //
  // Only the INNERMOST frame is consulted: every nested owner pushes its own
  // frame, and a producer only ever owns the measurements it issues directly, so
  // an enclosing frame can never be the right stop for a measurement issued
  // under an inner one. A null frame (or a null owner in it) simply means "no
  // owner", which leaves the other stops to do their work.
  const LayoutDependency::Frame* const frame = LayoutDependency::Top();
  const ViewImpl* const                owner = frame ? frame->owner : nullptr;

  // Only the DIRECT parent can be the owner of an arrange-time re-measure (every
  // owned arrange site re-measures one of its own children), which is what makes
  // the safety-net stop below precise: it fires for the re-measuring parent but not
  // for a distant arrange-in-progress ancestor (that distant node is the Δ1 case,
  // and must be cleared).
  bool isDirectParent = true;

  // GetParentView() DownCasts the actor parent, which also covers Ui::Layout
  // (Layout derives from View). A non-View parent (e.g. the window's root
  // layer) yields an empty handle and ends the ascent.
  Ui::View node = GetParentView();
  while(node)
  {
    ViewImpl&     nodeImpl = GetImpl(node);
    ViewDataImpl& nodeData = ViewDataImpl::Get(nodeImpl);

    // The owner test precedes (a), the safety-net stop and the clears on purpose:
    // the explicit owner must never be cleared. It is load-bearing for the RECYCLER
    // scope, whose owner (the recycler) is NOT arrange-in-progress and so would not
    // be caught by the direct-parent safety-net stop below.
    if(&nodeImpl == owner)
    {
      break;
    }

    // (a) The ancestor is running its own MEASURE pass, so it owns this
    // measurement -- typically this Measure() is a step of that pass's own
    // top-down recursion, and the ancestor's conditional publish decides its own
    // cache when its pass ends. Stop here.
    //
    // This stop stays load-bearing even now that owner scopes exist: the plain
    // top-down measure recursion pushes NO scope (a parent measuring its own
    // children inside OnMeasure is not an out-of-band measure), so removing this
    // would strand every node of every ordinary measure pass.
    //
    // Read no more into it than "the owner is mid-measure": an ancestor that has
    // ALREADY finished an earlier sibling has published that sibling's subtree,
    // so this stop does not cover a producer that reaches out of its own subtree
    // to measure such a node. No first-party code does that (producers measure
    // only their own direct children).
    if(nodeData.mMeasureInProgress)
    {
      break;
    }

    // Safety net for arrange-time re-measures. An ARRANGING DIRECT parent owns the
    // re-measure of its own child whether or not it pushed an ArrangeOwnedMeasureScope,
    // so the walk stops here exactly as it stops at an explicit owner. First-party
    // arrange sites also push a scope (the owner break above fires first for them);
    // this stop is what protects THIRD-PARTY LayoutManager / View subclasses, which
    // cannot push a scope because the scope header is internal -- they get the same
    // protection with no opt-in. Kept DIRECT-parent-only on purpose: a DISTANT
    // arrange-in-progress ancestor is the legitimate out-of-band case this walk exists
    // to clear (Δ1), so it must fall through to the clears below rather than stop here.
    //
    // A REPLAYING parent is excluded: a cache-hit replay runs no producer and issues no
    // Measure(), so it can never be the owner of one. Its own subtree gate has already
    // certified the descendants, and the cache-clear below is what retracts that
    // certification when an unowned measure rewrites a slot underneath it.
    if(isDirectParent && nodeData.mArrangeInProgress && !nodeData.mArrangeReplayInProgress)
    {
      break;
    }
    isDirectParent = false;

    nodeData.mMeasureCacheValid = false;
    nodeData.mArrangeCacheValid = false;

    // The ancestor is mid-ARRANGE and unowned, so it has already consumed this
    // view's previous slot: the cache clear just above must survive to the end of
    // that pass instead of being overwritten by its own publish. Declining the
    // publish is all that is needed -- this is a cache-only invalidation, so it
    // must not poison the pass and must not register a follow-up layout.
    //
    // ...and a REPLAYING ancestor has no publish to block: the replay leaves
    // mArrangeCacheValid exactly as it found it. Setting the bit there would latch a
    // block that only the NEXT real ArrangePassGuard entry clears, silently declining
    // that unrelated pass's publish.
    if(nodeData.mArrangeInProgress && !nodeData.mArrangeReplayInProgress)
    {
      nodeData.BlockArrangeCachePublishDuringPass();
    }

    // The ancestor is itself a standalone view: its measured size does not feed
    // its own parent's accumulation, so nothing ABOVE it can go stale because of
    // this view; stop. (Its cache was cleared just above; that clear is consumed
    // only if its own parent later re-measures it via MeasureStandaloneChildren,
    // i.e. on that parent's measure MISS. Its own slot needs nothing from this
    // walk: its parent's ArrangeStandaloneChild corrects it from the unconsumed
    // bit -- see the self-standalone early return at the top of this function.)
    if(IntegrationView::IsLayoutModeStandalone(nodeImpl))
    {
      break;
    }

    node = nodeData.GetParentView();
  }
}

/**
 * Drop the DIRECT PARENT's arrange cache entry before an UNOWNED arrange pass
 * rewrites this view's arrange records.
 *
 * Why it is needed: Arrange() publishes mArrangedBounds and (on a clean pass)
 * mLastArrangeInput unconditionally, and an ancestor's cache HIT replays this
 * view FROM those records -- CanReplayArrangeSubtreeFromCache deliberately skips
 * the cache-KEY comparison for descendants on the premise that each descendant's
 * recorded slot is the one the ancestor's own producer chain handed it. A public
 * out-of-band Arrange() breaks that premise: after it, the parent's HIT would
 * keep replaying the foreign bounds while a forced MISS would re-run the
 * producer and restore the parent-derived slot -- the hit/miss divergence
 * View::Arrange()'s contract rules out. This is the arrange-side twin of
 * InvalidateAncestorLayoutCachesForMeasureMiss(), with two deliberate
 * asymmetries:
 *
 *  - ARRANGE caches only. An arrange rewrites no measured slot, so every
 *    ancestor's MEASURE entry stays exactly as reproducible as it was.
 *  - ONE node, not a chain. The measure hit predicate is node-local, so the
 *    measure walk must clear every level itself; the arrange hit gate is
 *    RECURSIVE (it re-tests mArrangeCacheValid at every descendant it would
 *    elide), so one cleared parent already refuses every ancestor's hit, and
 *    the misses that follow re-publish level by level.
 *
 * Cache-only, like the measure walk: no dirty, no poison, no registration --
 * this must never schedule (or spin) a layout pass. The next pass that reaches
 * the parent simply misses once and re-hands every child its parent-derived
 * slot, which is exactly the pre-cache behaviour.
 */
void ViewDataImpl::InvalidateParentArrangeCacheForOutOfBandArrange(bool frameworkLayoutRootPass)
{
  // A standalone view is its own layout root, but that alone does NOT make every
  // Arrange() owned: an application can call the public View::Arrange() with
  // arbitrary bounds, and the parent's next MISS would replace them with the slot
  // ArrangeStandaloneChild derives. Only LayoutController's root entry point proves
  // that this pass used the framework derivation.
  //
  // And that derivation converging with the parent's is now a CALL-GRAPH fact rather
  // than a coincidence of two hand-written expressions: ProcessLayoutRoot and
  // ArrangeStandaloneChild both end on Internal::DeriveStandaloneRootBounds(), so the
  // framework root pass hands the view exactly the bounds the parent's next miss
  // would. An application calling the public View::Arrange() carries no such
  // ownership, which is why the exemption is gated on the root entry point and not on
  // the layout mode alone.
  if(frameworkLayoutRootPass && IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    return;
  }

  // Only a RECYCLER frame owns Arrange calls. An ARRANGE frame is deliberately a
  // Measure-only scope (ArrangeOwnedMeasureScope): treating its mere presence as
  // arrange ownership would let a nested, unrelated public Arrange() retain a stale
  // parent entry.
  const LayoutDependency::Frame* const frame = LayoutDependency::Top();
  if(frame != nullptr && frame->owner != nullptr && frame->kind == LayoutDependency::OwnerKind::RECYCLER)
  {
    return;
  }

  Ui::View parentView = GetParentView();
  if(!parentView)
  {
    return; // A layout root: there is no parent entry to go stale.
  }

  ViewDataImpl& parentData = ViewDataImpl::Get(GetImpl(parentView));

  // The direct parent is mid-arrange: this call came from its own producer
  // chain (the normal recursion, every layout manager, or a third-party
  // producer arranging its own child). Whatever a producer does inside its own
  // pass IS its output -- a re-run would reproduce it -- so the parent's
  // about-to-publish entry stays a faithful replay premise.
  //
  // A REPLAYING parent does NOT qualify. "Whatever a producer does inside its own pass IS
  // its output" is a statement about a producer; a replay runs none, so a foreign
  // Arrange() reaching a descendant mid-replay really does invalidate the premise the
  // replay is built on -- CanReplayArrangeSubtreeFromCache assumes every descendant's
  // recorded slot was written by THIS chain.
  if(parentData.mArrangeInProgress && !parentData.mArrangeReplayInProgress)
  {
    return;
  }

  parentData.mArrangeCacheValid = false;
}

MeasuredSize ViewDataImpl::Measure(float visualW, float visualH)
{
  // Same-view re-entrancy is guarded in RELEASE, not just DEBUG: Measure() is
  // reachable from untrusted customization (MeasureCallback / OnMeasure /
  // LayoutManager) and from the public, nestable LayoutController::ProcessLayouts(),
  // so an app can re-enter a view that is mid-pass. Running the producer again
  // here would recurse without bound and would let the inner pass publish a
  // result the outer pass then overwrites. Instead: poison the outer pass (so it
  // cannot serve a cache hit afterwards) and hand back the last COMPLETED result.
  // This check precedes the actor read and the cache-hit test on purpose - a hit
  // served to a re-entrant call would hide the poison.
  if(mMeasureInProgress)
  {
    mMeasurePassPoisoned = true;
    DALI_LOG_ERROR("Re-entrant Measure() on the same View; returning last completed result\n");
    return mMeasuredSize; // {0,0} until a pass completes
  }

  float s = mViewImpl.GetEffectiveScale();

  // Push effective scale to the actor animatable property so that decoration
  // constraints (corner radius, borderline width) can read it as a scale input.
  //
  // GATED by mEffectiveScaleActorSynced, the sync bit for that ACTOR-side copy --
  // the second half of the pair whose first half, mEffectiveScaleValid, is the sync
  // bit for mEffectiveScale itself. While the bit is true the property is known to
  // hold `s`, so the read-back below is skipped. That read sits ABOVE the cache-hit
  // test (it has to: `s` is an input to the KEY computed just after it), so before
  // the gate it ran on every HIT as well -- the one actor property access left on
  // the settled measure path.
  //
  // The gate is BEHAVIOUR-NEUTRAL rather than merely cheaper, because every way the
  // property can stop matching `s` from the event side clears the bit:
  //   - the cached scale moving: DropCachedEffectiveScale() clears BOTH bits in one
  //     breath, which is also why this bit stores no value of its own. Every
  //     scale-context change routes through it (InvalidateMeasure,
  //     ResetSubtreeScaleAndLayoutCaches), so the same subtree-wide invalidation
  //     the arrange cache relies on for Corollary C covers this bit too.
  //   - anyone writing the property: it is a REGISTERED animatable property with a
  //     set function (ANIMATABLE_PROPERTY_7 at the top of this file), so every
  //     event-side write -- the push below included -- is routed by dali-core
  //     through ViewDataImpl::SetProperty, which clears the bit. An external clobber
  //     is therefore still corrected by the very next Measure(), cache HIT or MISS,
  //     exactly as it was before the gate.
  // The bit is set AFTER the write for precisely that reason: the push re-enters
  // ViewDataImpl::SetProperty and clears it first.
  //
  // Known gap, deliberately left open: an Animation targeting this property updates
  // the event-side cached value through Object::NotifyPropertyAnimation, which does
  // NOT run the registered setter, so the bit survives. Animating a framework-owned
  // property was never supported (the push below has always fought it).
  if(!mEffectiveScaleActorSynced)
  {
    // Read back the current actor property value to skip redundant scene-graph writes.
    // This also naturally corrects any value set externally on EFFECTIVE_SCALE.
    if(!Dali::Equals(s, mViewImpl.Self().GetProperty<float>(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX)))
    {
      // SetProperty triggers ViewDataImpl::SetProperty(VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), which:
      //   - updates the actor animatable so decoration constraints re-evaluate, and
      //   - calls UpdateCornerRadius() for active RenderEffect / OffScreenRendering.
      mViewImpl.Self().SetProperty(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, s);
    }
    mEffectiveScaleActorSynced = true;
  }

  float natW = (visualW > 0.f) ? visualW / s : visualW;
  float natH = (visualH > 0.f) ? visualH / s : visualH;

  // Ensure constraints respect this view's min/max bounds so that
  // OnMeasure (and therefore child measurements) see the effective
  // available space. Without this, children would be measured against
  // the original (smaller) constraint, then ApplyConstraints would
  // enlarge the result — leaving children incorrectly sized.
  float effNatW = std::min(std::max(natW, GetMinimumWidth()), GetMaximumWidth());
  float effNatH = std::min(std::max(natH, GetMinimumHeight()), GetMaximumHeight());

  // The effective scale is a KEY term here, where the ARRANGE cache instead relies on
  // invalidation plus a DEBUG assert (Corollary C). The asymmetry is deliberate:
  //
  //  - As a KEY, a missed invalidation degrades to a MISS -- one recomputed
  //    measurement -- and can never produce a measured size computed for a different
  //    scale. That is the same reasoning mLastArrangeDirection applies to
  //    arrange x direction, and `s` is already in hand here (it is read above the
  //    predicate because the constraint normalisation needs it), so the term costs one
  //    float compare and no extra property access.
  //  - Under the CURRENT invariant this term cannot fire: every path that moves the
  //    cached scale pairs DropCachedEffectiveScale() with InvalidateLayoutCaches(),
  //    which clears mMeasureCacheValid first. It is therefore defence in depth, and it
  //    exists so that a future unpaired caller degrades to a miss instead of serving a
  //    result produced at the old scale.
  //
  // Compared EXACTLY rather than with FloatEqual: this is a copy of the very
  // computation the producer was run under, with no arithmetic in between, so an
  // epsilon compare would only widen the key -- it would let a sub-epsilon scale change
  // serve a result computed for a different producer input. The constraint terms below
  // do need the tolerance: they arrive through a /s normalisation and a min/max clamp.
  // NaN (the constructed value) equals nothing, so the never-measured state fails safe.
  //
  // Cost order: after the three bit tests, which are cheaper and reject more often;
  // before the FloatEqual calls, which are not.
  //
  // The width term is a plain FloatEqual: the published key is the clamped
  // effective natural constraint, which min/max clamping (both validated non-negative)
  // keeps >= 0, and the NaN the constructor plants is unreachable behind
  // mMeasureCacheValid. The old `>= 0.0f` pre-test could therefore never reject anything
  // FloatEqual would have accepted.
  if(mMeasureCacheValid && !mMeasureDirty && !mMeasurePassPoisoned &&
     mLastMeasureScale == s &&
     FloatEqual(mLastMeasureConstraint.width, effNatW) &&
     FloatEqual(mLastMeasureConstraint.height, effNatH))
  {
    return mMeasuredSize;
  }

  // Miss: open the measure transaction. The guard owns mMeasureInProgress /
  // mMeasurePassPoisoned / mMeasureCacheValid for this scope; leaving it with
  // mMeasureCacheValid still false (no publish reached) makes the next Measure
  // miss and recompute.
  MeasurePassGuard pass(*this);

  // This pass will rewrite the measured slot that every ancestor arranges from,
  // so any ancestor cache entry that was produced against the previous slot has
  // to stop being servable. Runs only on a confirmed full miss (after the
  // re-entrancy return and after the cache-hit return) and only once the guard
  // is open, so an ancestor that owns this measurement is recognised by its own
  // in-progress flag and the walk stops there.
  InvalidateAncestorLayoutCachesForMeasureMiss();

  // OnMeasure receives and returns visual (scale-applied) sizes, consistent with OnArrange.
  float        effVisW = (effNatW >= 0.f) ? effNatW * s : effNatW;
  float        effVisH = (effNatH >= 0.f) ? effNatH * s : effNatH;
  MeasuredSize visual;
  if(auto* callback = GetMeasureCallback())
  {
    Ui::View view = Ui::View::DownCast(mViewImpl.Self());
    visual        = callback->Invoke(view, effVisW, effVisH);
  }
  else if(auto* manager = mViewImpl.GetLayoutManager())
  {
    visual = DispatchMeasureWithLayoutManager(manager, effVisW, effVisH);
  }
  else
  {
    visual = mViewImpl.OnMeasure(effVisW, effVisH);
  }
  visual = ApplyConstraints(visual);

  // The RESULT is published unconditionally: mMeasuredSize is "the last
  // completed measurement" and is read outside the cache path (GetMeasuredSize(),
  // and every layout manager placing a non-MATCH_PARENT child during Arrange),
  // so it must reflect the work this pass just did regardless of cache state.
  mMeasuredSize.width  = visual.width;
  mMeasuredSize.height = visual.height;

  // The freshly written slot has not been consumed by this view's parent yet.
  // Set unconditionally, exactly like the publish above: only the standalone
  // path reads it (ArrangeStandaloneChild), and testing the layout mode here
  // would cost more than the store it would save.
  mMeasuredSlotUnconsumed = true;

  // The cache KEY is published CONDITIONALLY. mMeasureDirty was consumed at pass
  // entry, so seeing it true here means an InvalidateMeasure() arrived while the
  // producer was running: the value just computed is already known to be stale,
  // and caching it would pin it until some later, unrelated invalidation. Same
  // for a poisoned pass (re-entrancy / scale-context reset). Declining to
  // publish leaves mMeasureCacheValid false, so the next Measure() misses and
  // recomputes with the new state.
  if(!mMeasureDirty && !mMeasurePassPoisoned)
  {
    mLastMeasureConstraint.width  = effNatW;
    mLastMeasureConstraint.height = effNatH;
    mLastMeasureScale             = s;
    mMeasureCacheValid            = true;
  }
  else if(mMeasurePassPoisoned && !mMeasureDirty)
  {
    // Pure poison: the pass was invalidated by something that did not go
    // through InvalidateMeasure() on this view (re-entrancy, scale-context
    // reset), so nothing propagated to the layout root or registered a
    // follow-up. Schedule exactly one. A declarative mid-pass re-dirty
    // (mMeasureDirty true) already propagated and registered inside
    // InvalidateMeasure(), so it must NOT be registered a second time here.
    //
    // Caveat: a producer that re-enters its own on-scene Measure() on every
    // pass turns this one-shot follow-up into a per-frame layout loop. As with
    // the LayoutFinished-slot re-invalidation case (see LayoutController), this
    // app-level non-convergence is deliberately not capped here; the re-entry is
    // logged above on every occurrence. No first-party producer does this.
    InvalidateMeasure();
  }

  // Ensure standalone children are measured even when OnMeasure (e.g. in
  // leaf views like Label) does not iterate children. The measure cache
  // prevents redundant work when OnMeasure already measured them.
  MeasureStandaloneChildren(effVisW, effVisH);

  return mMeasuredSize;
}

bool ViewDataImpl::HasUnconsumedStandaloneChild() const
{
  // Const iteration: this is a pure query on the arrange cache-hit path and must
  // not touch any child state. Dali::Vector<View>::ConstIterator yields a
  // const View&, which resolves to the const GetImpl / const ViewDataImpl::Get
  // overloads, so constness is carried all the way to the two reads below.
  for(Dali::Vector<View>::ConstIterator it = mChildren.Begin(), end = mChildren.End(); it != end; ++it)
  {
    const ViewImpl&     childImpl = GetImpl(*it);
    const ViewDataImpl& childData = ViewDataImpl::Get(childImpl);

    // Selective term first. mMeasuredSlotUnconsumed is set unconditionally at every
    // measure publish and cleared only by the parent's own MeasureStandaloneChildren
    // / ArrangeStandaloneChildren, both of which skip non-standalone children -- so
    // it is TRUE for every regular child in the steady state and rejects nothing.
    // IsLayoutModeStandalone is the term that actually decides, so it runs first and
    // the bit only qualifies the few standalone children. Both operands are
    // side-effect-free reads, so the order is a pure cost choice.
    if(IntegrationView::IsLayoutModeStandalone(childImpl) && childData.mMeasuredSlotUnconsumed)
    {
      return true;
    }
  }
  return false;
}

bool ViewDataImpl::CanServeArrangeFromCache(const LayoutRect& bounds) const
{
  // The NODE-LOCAL half of the arrange cache-HIT predicate; see the term list in
  // Arrange(), which is the only caller that supplies a candidate `bounds`. Factored
  // out so the subtree gate below can re-state the same terms for a descendant
  // without duplicating them, and so the one line a future increment might edit is
  // in one place.
  //
  // Cost-ordered, and HasUnconsumedStandaloneChild() is deliberately LAST: it is the
  // only O(direct children) term here, every other one being a bit test, a four-float
  // compare or a single cached-member read on the actor. Every cheaper term therefore
  // gets its chance to reject before the scan runs.
  return mArrangeCacheValid &&
         !mArrangeProducerAlways &&
         !mArrangeDirty &&
         !mArrangePassPoisoned &&
         !mArrangeCacheBlockedDuringPass &&
         SameLayoutRect(mLastArrangeInput, bounds) &&
         mLastArrangeDirection == mViewImpl.Self().GetEffectiveLayoutDirection() &&
         !HasUnconsumedStandaloneChild();
}

bool ViewDataImpl::CanReplayArrangeSubtreeFromCache() const
{
  // Read-only, and it visits exactly the nodes ReplayArrangeSubtreeFromCache would:
  // the two walks must agree, or the gate would clear a node the replay then writes
  // (or refuse on a node the replay never touches).
  for(Dali::Vector<View>::ConstIterator it = mChildren.Begin(), end = mChildren.End(); it != end; ++it)
  {
    const ViewImpl&     childImpl = GetImpl(*it);
    const ViewDataImpl& childData = ViewDataImpl::Get(childImpl);

    // Not arranged by this view's producer in the pass that published the entry, so
    // the replay does not touch it and it constrains nothing. This is the shape of a
    // Label's children: LabelImpl::OnArrange returns its bounds and never delegates,
    // so its children hold no arrange result and a MISS leaves them alone too.
    //
    // This is sound only because no child can JOIN the set after the entry was
    // published: adding a contributing child invalidates this view's measure, and
    // adding a standalone child invalidates this view's arrange (OnChildAdded). So a
    // live entry implies the child set is the one the producer last arranged.
    if(!childData.mArrangeResultAvailable)
    {
      continue;
    }

    // The node-local terms of CanServeArrangeFromCache, minus two:
    //
    //  - the cache KEY (SameLayoutRect). A descendant has no candidate `bounds` to
    //    key against, and it does not need one: with this view's own key matched and
    //    its producer ArrangePolicy::IF_CHANGED, the producer would hand each child the same slot it
    //    handed it last time -- the slot that child recorded as mLastArrangeInput and
    //    resolved into the mArrangedBounds the replay is about to apply. The key
    //    match is implied by the parent's key match plus policy, which is why policy
    //    is required at every node and not only at the top.
    //
    //    That "recorded by THIS chain's producers" premise is ENFORCED, not
    //    assumed: an unowned out-of-band Arrange() on any descendant rewrites
    //    those records and, in the same breath, retracts its direct parent's
    //    entry (InvalidateParentArrangeCacheForOutOfBandArrange) -- which this
    //    recursive walk then sees as an invalid node and refuses the whole hit.
    //    So a hit can never replay a record this chain did not write.
    //  - mChildren.Empty(), which is what this increment removes.
    //
    // !mArrangeProducerAlways is the term that carries the implication above.
    // mArrangeCacheValid is what catches a STANDALONE descendant whose invalidation
    // stopped at its own boundary and never reached this view; the dirty / poison /
    // blocked bits beside it are defence in depth in exactly the sense the node-local
    // list describes -- every writer that raises one clears mArrangeCacheValid in the
    // same breath, so they are implied today and are tested so that the hit stays
    // correct if that pairing is ever broken.
    // !HasUnconsumedStandaloneChild() is I4 restored for descendants: it is
    // O(direct children), so only a per-node evaluation covers a subtree. It is the
    // one term here that decides on its own -- a standalone view is its own layout
    // root, so it can end a pass with a live entry AND a slot its parent has not
    // consumed.
    // The direction read is Actor::GetEffectiveLayoutDirection() on the child's own
    // actor -- the single resolved-direction read the framework uses everywhere.
    //
    // Ordered as in CanServeArrangeFromCache, and for the same reason: the
    // O(direct children) scan goes last so every bit test gets to reject first.
    if(!(childData.mArrangeCacheValid &&
         !childData.mArrangeProducerAlways &&
         !childData.mArrangeDirty &&
         !childData.mArrangePassPoisoned &&
         !childData.mArrangeCacheBlockedDuringPass &&
         childData.mLastArrangeDirection == childData.mViewImpl.Self().GetEffectiveLayoutDirection() &&
         !childData.HasUnconsumedStandaloneChild()))
    {
      return false;
    }

    if(!childData.CanReplayArrangeSubtreeFromCache())
    {
      return false;
    }
  }

  return true;
}

void ViewDataImpl::ReplayArrangeSubtreeFromCache(bool mirrorUnderParentRtl, float parentArrangedWidth)
{
  // Corollary C, checked live and per node: a valid arrange cache implies a valid
  // cached effective scale, because every scale-context reset clears both for the WHOLE
  // subtree (ResetSubtreeScaleAndLayoutCaches). The replay skips the
  // GetEffectiveScale() the MISS path performs at every level, so this assert is
  // what makes the reliance visible. DEBUG-only, like every other first-party layout
  // invariant on this hot path. It runs BEFORE the node scope below on purpose: it
  // reads nothing the scope owns, and throwing out of it leaves no flag to restore.
  DALI_ASSERT_DEBUG(mEffectiveScaleValid);

  // A valid cache is published only at the end of a pass that also published
  // mArrangedBounds, so a result must exist to serve. For a descendant this is the
  // gate's own filter, restated as an invariant.
  DALI_ASSERT_DEBUG(mArrangeResultAvailable);

  // Open this node's slice of the replay transaction BEFORE the first actor write. Every
  // write below can reach application code through OnPropertySet / PropertySetSignal, and
  // this is what makes that code see the same same-view re-entrancy guard a producer pass
  // would give it. Scoped to the end of the function so it also covers the recursion and
  // the LayoutFinished registration; unwound by RAII, so a DEBUG assert thrown deeper in
  // the subtree restores every level on the way out.
  ReplayNodeScope node(*this);

  // Snapshot before applying: ApplySelfBoundsIfChanged takes its argument by const
  // reference, so passing mArrangedBounds directly would hand it an alias of the very
  // member it reconciles against. The copy is also what keeps mArrangedBounds LOGICAL
  // while the value applied below may be its physical resolution.
  const LayoutRect cached = mArrangedBounds;

  // 1. Self reconciliation -- the SAME call the MISS path ends on, and the ONE actor
  //    write this node performs. NOT skippable: it is an UNCONDITIONAL per-pass
  //    reconciliation, not a one-time apply, and it is what restores geometry clobbered
  //    outside layout (Extension::SetPositionX, a transition frame). Its exact `!=` write
  //    suppression makes the settled case free -- four property reads and no scene-graph
  //    write -- and that is now true for a right-to-left subtree too, because the mirror
  //    is folded in here instead of being re-applied afterwards by the parent.
  //
  //    mArrangedBounds itself stays LOGICAL (invariant: the arranged rect and Arrange()'s
  //    return value are parent-local logical, and the transition dispatcher's
  //    VisualBoundsOf re-derives the physical x from it through the same MirrorX). Only
  //    `applied` carries the physical x, and only as far as the actor.
  //
  //    The MISS path also applies the INPUT bounds provisionally before running the
  //    producer. That write is observable only to the producer, which is elided, so
  //    the single apply of the final bounds is equivalent.
  LayoutRect applied = cached;
  if(mirrorUnderParentRtl)
  {
    applied.x = MirrorX(parentArrangedWidth, cached.x, cached.width);
  }
  ApplySelfBoundsIfChanged(applied);

  // 2. Descendants, in mChildren order -- the order ArrangeDefault's snapshot preserves,
  //    and the order every layout manager iterates.
  //
  //    A SNAPSHOT, like every other child traversal in this file. The apply above is an
  //    actor property write, which reaches OnPropertySet and a synchronous
  //    PropertySetSignal emit and hence application code that may unparent a child --
  //    dali-core erases from its container BEFORE notifying (ActorParentImpl::Remove), so
  //    an index loop that re-read Count() would shift every following sibling down by one
  //    and silently skip the last one. The snapshot also keeps each child alive across the
  //    recursive call. Guarded by Empty() so a leaf, which is the common hit shape, pays
  //    no allocation.
  //
  //    A child removed DURING the walk is still visited from the snapshot, exactly as
  //    ArrangeDefault's and ArrangeStandaloneChildren's snapshots visit theirs. A REPARENT
  //    is fully covered: OnChildAdded clears the moved child's mArrangeResultAvailable, so
  //    the test below skips it and no old-parent rect is applied under a new parent. A
  //    plain removal leaves one write on a detached actor, which its next add re-derives.
  //
  //    Standalone children are NOT filtered out of the walk: the MISS path reaches them
  //    through ArrangeStandaloneChildren -> ArrangeStandaloneChild -> childImpl.Arrange(),
  //    which likewise ends at the child's own mArrangedBounds. They are excluded only from
  //    the MIRROR, exactly as ApplyLayoutDirection excludes them.
  if(!mChildren.Empty())
  {
    // THIS node's own resolved direction decides the mirror for its direct children --
    // the same read, on the same actor, that ApplyLayoutDirection performs on the MISS
    // path. Read once per node rather than once per child.
    const bool selfIsRightToLeft =
      (mViewImpl.Self().GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT);

    std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
    for(auto& childView : childSnapshot)
    {
      ViewImpl&     childImpl = GetImpl(childView);
      ViewDataImpl& childData = ViewDataImpl::Get(childImpl);

      // Not arranged by this view's producer in the pass that published the entry, so
      // there is no parent-owned logical result to re-apply and none to mirror. The MISS
      // path leaves such a child untouched for the same reason (ApplyLayoutDirection's
      // mArrangeResultAvailable skip).
      if(!childData.mArrangeResultAvailable)
      {
        continue;
      }

      // The MISS path's mirror-eligibility rule, evaluated by the parent because only the
      // parent knows the direction and the width it mirrors about.
      const bool mirrorChild =
        selfIsRightToLeft && !IntegrationView::IsLayoutModeStandalone(childImpl);

      childData.ReplayArrangeSubtreeFromCache(mirrorChild, cached.width);
    }
  }

  // Already true on any path that could reach a hit (the cache was published by a
  // completed pass, which sets this). Kept unconditional so the flag's meaning stays
  // "at least one arrange pass has completed", independent of hit/miss.
  mInitialLayoutDone = true;

  // LayoutFinished semantics are pass-based, not work-based: a subscriber is told its
  // view was arranged in this pass, and being replayed from cache IS an arrange of
  // that view. Emitting it per visited node is what keeps the controller's
  // arrangedViews set identical to the one a MISS would produce.
  if(HasLayoutFinishedSignalConnections())
  {
    LayoutController::NotifyViewArranged(&mViewImpl);
  }
}

LayoutRect ViewDataImpl::Arrange(const LayoutRect& bounds)
{
  return ArrangeImpl(bounds, false);
}

LayoutRect ViewDataImpl::ArrangeAsLayoutRoot(const LayoutRect& bounds)
{
  return ArrangeImpl(bounds, true);
}

LayoutRect ViewDataImpl::ArrangeImpl(const LayoutRect& bounds, bool frameworkLayoutRootPass)
{
  // Validate first-party layout invariants in DEBUG only: this runs on the
  // per-frame, per-view layout hot path (including deep child recursion), so a
  // release-active assert/throw here would turn a manager/measure glitch into an
  // app-terminating exception. The untrusted customization RETURN is still
  // guarded in release by ResolveReturnedBounds (log + fall back to input).
  DALI_ASSERT_DEBUG(IsValidLayoutRect(bounds));

  // Same-view re-entrancy is guarded in RELEASE, not just DEBUG (this used to be
  // a debug-only assertion, i.e. undefined behaviour in release). An arrange
  // customization can call Arrange() on its own view, directly or through the
  // public, nestable LayoutController::ProcessLayouts(). Re-running the producer
  // would recurse without bound and corrupt the outer pass's self geometry, so:
  // poison the outer pass and return the last COMPLETED bounds, falling back to
  // the caller's input while no pass has completed yet.
  if(mArrangeInProgress)
  {
    mArrangePassPoisoned = true;
    DALI_LOG_ERROR("Re-entrant Arrange() on the same View\n");
    return mArrangeResultAvailable ? mArrangedBounds : bounds;
  }

  // ---------------------------------------------------------------------------
  // Arrange cache HIT.
  //
  // Placed here, ABOVE ArrangePassGuard, for the same reason the measure hit sits
  // above MeasurePassGuard: the guard clears mArrangeCacheValid (and consumes
  // mArrangeDirty / the poison bits) at entry, so this is the last point at which
  // the previous pass's cache state can still be read. Constructing the guard and
  // then returning would also destroy the very cache entry being served.
  //
  // A HIT IS NOT A PRUNE. Skipping this view's producer also skips the child
  // recursion it would have driven, and that recursion does more than recompute:
  // every level ends in ApplySelfBoundsIfChanged, an UNCONDITIONAL per-pass
  // reconciliation that silently REPAIRS any external actor-geometry write below it
  // (the sanctioned Extension::SetPositionX escape hatch used by ScrollView /
  // RecyclerView, and the layout transition dispatcher). It also mirrors under RTL
  // and registers every arranged view for LayoutFinished. Dropping all of that for a
  // subtree would break the promise View::Arrange documents -- "the arranged geometry
  // is reconciled either way, so the outcome is the same" -- and the clobber channel
  // cannot be closed by bookkeeping, because Ui::View publicly derives from
  // Dali::Actor and any holder can write POSITION_X without going through
  // Extension::.
  //
  // So the hit REPLAYS the settled subtree from cache instead of pruning it:
  // ReplayArrangeSubtreeFromCache walks the same nodes the producer recursion would
  // have reached and performs, per node, exactly the observable work the MISS path
  // performs -- self reconciliation (with the right-to-left mirror folded into that
  // single apply), the LayoutFinished registration -- while eliding only the PRODUCER (and with it two
  // heap allocations, the producer dispatch, ResolveReturnedBounds and the publish
  // gate). What is skipped is recomputation; what is kept is every write and every
  // notification.
  //
  // Eliding a producer is result-identical to the miss it replaces only when its
  // result depends on the input bounds, effective layout direction, effective scale,
  // and state tracked by layout invalidation. The first two are cache-key terms, the
  // third is carried by Corollary C, and the fourth by mArrangeCacheValid.
  // ArrangePolicy::IF_CHANGED assumes that contract by default. A producer outside the
  // envelope -- one that reads ancestor/world geometry or pushes state outside the
  // actor tree -- must explicitly select ArrangePolicy::ALWAYS. See the term list.
  //
  // TWO GATES, in this order:
  //  1. CanServeArrangeFromCache(bounds) -- the NODE-LOCAL predicate below: may THIS
  //     view's producer be elided for THIS input?
  //  2. CanReplayArrangeSubtreeFromCache() -- the recursive SUBTREE gate: may every
  //     descendant the replay would touch have ITS producer elided too? It re-tests
  //     the same node-local terms (minus the key, which is implied -- see the
  //     function) at every node, so policy, dirtiness and the unconsumed-standalone
  //     correction are checked per node rather than only at the top. Skipped
  //     entirely for a childless view, which keeps a leaf's hit exactly as cheap as
  //     it was when the hit was childless-only.
  //
  // Validate-then-replay, not a fused bail-out walk: a walk that gave up half way
  // would already have written cached bounds into part of the subtree, and the MISS
  // that follows does not necessarily revisit every node it wrote (a Label's
  // children, for instance), so a partial replay would not be provably neutral. Two
  // phases keep "hit" atomic.
  //
  // Predicate terms, cost-ordered (cheapest / most selective first):
  //  - mArrangeCacheValid: the entry exists. Cleared by every layout invalidation
  //    and by a full Measure pass on this view, so it carries all the freshness the
  //    hit relies on for its own inputs.
  //  - !mArrangeProducerAlways: the active producer permits unchanged-result reuse.
  //    Producers that read untracked geometry or update an external surface use
  //    ArrangePolicy::ALWAYS and therefore reject this cache hit at every subtree level.
  //  - !mArrangeDirty / !mArrangePassPoisoned / !mArrangeCacheBlockedDuringPass:
  //    defence in depth. Each of these is raised by a writer that also clears
  //    mArrangeCacheValid in the same breath, so they are implied today; testing
  //    them keeps the hit correct if that pairing is ever broken.
  //  - SameLayoutRect(mLastArrangeInput, bounds): the cache KEY. Exact compare,
  //    see SameLayoutRect.
  //  - !HasUnconsumedStandaloneChild(): the corrective re-measure for a standalone
  //    child's slot lives further down this function (ArrangeStandaloneChildren),
  //    so a hit must not skip it. It is O(direct children) only, which is why the
  //    subtree gate re-evaluates it at every node it would elide -- a DESCENDANT
  //    holding an unconsumed slot is invisible from here. The other half of the
  //    standalone story, a child ADDED after the entry was published, is closed on
  //    the add side: OnChildAdded invalidates this view's arrange for a standalone
  //    child (the measured slot of a never-measured child does not raise this term).
  //  - mLastArrangeDirection == GetEffectiveLayoutDirection(): belt and braces for
  //    the direction key. The direction lives in dali-core and can move through
  //    actors dali-ui does not own, so a missed invalidating subtree walk must
  //    degrade to a MISS here, never to a wrongly mirrored arrangement.
  //
  // Deliberately NOT terms:
  //  - the effective scale. A scale change goes through
  //    ResetSubtreeScaleAndLayoutCaches / DropCachedEffectiveScale, both of which
  //    clear the layout caches, so mArrangeCacheValid already implies "scale in
  //    sync" (Corollary C). The DEBUG assert in the body is that implication's
  //    live check rather than a re-test.
  //
  //    The four (axis, input) pairs are handled three different ways, and the choice
  //    is per pair rather than per axis or per input:
  //      measure x scale     -- KEY (mLastMeasureScale). `s` is already read above
  //                             that predicate for the constraint normalisation, so
  //                             the term is one float compare.
  //      arrange x scale     -- invalidation + DEBUG assert (this bullet). Reading it
  //                             here would be a fresh call on a path that needs it for
  //                             nothing else.
  //      arrange x direction -- KEY (mLastArrangeDirection). The direction lives in
  //                             dali-core and can move through actors dali-ui does not
  //                             own, so a missed hook must not mirror the wrong way.
  //      measure x direction -- invalidation (the direction-change subtree walk
  //                             invalidates the MEASURE axis). A key term would put a
  //                             layout-direction read into the per-view, per-pass
  //                             measure predicate to cover a producer shape no
  //                             first-party view has.
  //    The rule behind the split: a KEY where the value is already in hand or the
  //    failure mode is a wrong RESULT; invalidation where reading it would cost a
  //    property access on a hot path and the failure mode is only a stale one.
  //  - mMeasureCacheValid. A full Measure pass clears mArrangeCacheValid from
  //    MeasurePassGuard, so the two are cleared as a pair.
  if(CanServeArrangeFromCache(bounds) &&
     (mChildren.Empty() || CanReplayArrangeSubtreeFromCache()))
  {
    // Snapshot the entry before anything is applied. The replay below reconciles
    // this view's actor against mArrangedBounds, and nothing on this path writes
    // that member -- the copy is what keeps that a local fact instead of a
    // precondition the return value silently depends on.
    const LayoutRect cached = mArrangedBounds;

    // Open the replay TRANSACTION for the whole subtree. See ReplayPassScope: the replay
    // writes actor properties and therefore runs application code, so it has to be inside
    // the layout processing window (park an invalidation raised from it, and make the
    // per-node re-entrancy guard reachable). It is NOT ArrangePassGuard, whose entry would
    // consume mArrangeDirty and clear the very mArrangeCacheValid this hit is serving.
    ReplayPassScope replayPass;

    // The whole hit: this view and every descendant the elided producers would have
    // reached, reconciled in the MISS path's own order. Per-node Corollary C and
    // result-availability asserts live inside it.
    //
    // LOGICAL at the top: this view's own right-to-left mirror is owned by its PARENT,
    // which is not running here -- exactly as on the MISS path, where the producer applies
    // logical bounds and the parent's ApplyLayoutDirection mirrors afterwards. Folding a
    // mirror in here would apply it twice.
    // UtcDaliViewArrangeCacheHitReAppliesLogicalBoundsUnderRtlP pins it.
    ReplayArrangeSubtreeFromCache(false, 0.0f);

    // The cached rect, NOT `bounds`. The publishing pass returned its
    // ResolveReturnedBounds() result, which an arrange customization may have
    // moved away from its input; mArrangedBounds is exactly that value, so
    // returning it makes the hit indistinguishable from re-running the producer.
    return cached;
  }
  // ---------------------------------------------------------------------------

  // Open the arrange transaction. The guard owns mArrangeInProgress /
  // mArrangePassPoisoned / mEffectiveScaleInvalidatedDuringPass / mArrangeCacheValid
  // for this scope.
  ArrangePassGuard pass(*this);

  // This pass will rewrite the arrange records every ancestor's cache HIT
  // replays this view from (mArrangedBounds is published unconditionally
  // below). If no producer above owns this pass -- an out-of-band public
  // Arrange() -- the direct parent's cache entry must stop being servable, or
  // its next HIT would replay the foreign result where a MISS would restore the
  // parent-derived slot. Owned passes (parent mid-arrange, recycler scope, and a
  // framework-owned standalone root pass) return without touching anything.
  InvalidateParentArrangeCacheForOutOfBandArrange(frameworkLayoutRootPass);

  // Establish this view's cached effective scale for the pass, exactly as
  // Measure() does at its own entry. Every arrange producer that touches
  // children reads the effective scale anyway, but the DEFAULT arrange of a
  // CHILDLESS view never does (ArrangeDefault's scale read sits behind its
  // child loop), so without this read the sync bit would still be false at the
  // publish gate below and a settled leaf could never cache. Amortised O(1):
  // the walk stops at the first ancestor whose bit is already live. Return value
  // intentionally discarded -- called for its side effect of establishing the bit.
  (void)mViewImpl.GetEffectiveScale();

  // ApplySelfBoundsIfChanged is an UNCONDITIONAL per-pass reconciliation of the
  // actor's geometry against the arranged bounds, not a one-time apply: it
  // silently repairs any external clobber of POSITION/SIZE -- including the
  // sanctioned first-party scroll writes (ScrollViewImpl/RecyclerViewImpl move the
  // content actor directly via Extension::SetPositionX to bypass layout). The
  // cache HIT above performs this reconciliation itself, with the LOGICAL
  // mArrangedBounds and never a mirrored value (C4-B1 / C4-B2: mirroring stays the
  // parent's job in ApplyLayoutDirection), which is why it returns from the middle
  // of this function instead of the top.
  // UtcDaliViewArrangeRestoresExternallyMovedSelfGeometry{,Rtl}P pin this, and
  // UtcDaliViewArrangeCacheHitStillReconcilesSelfGeometryP pins it for the hit.
  //
  // Phase 1: apply the input bounds as provisional self geometry, so a
  // customization hook that reads back self event-side geometry observes the
  // input (as before this refactor), not stale prior-pass geometry.
  ApplySelfBoundsIfChanged(bounds);

  LayoutRect returnedBounds = bounds;
  if(auto* callback = GetArrangeCallback())
  {
    returnedBounds = DispatchArrangeWithCallback(callback, bounds);
  }
  else if(auto* manager = mViewImpl.GetLayoutManager())
  {
    DispatchArrangeWithLayoutManager(manager, bounds);
    returnedBounds = bounds; // Managers arrange children only; owner final == input.
  }
  else
  {
    returnedBounds = mViewImpl.OnArrange(bounds);
  }

  // Phase 2: adopt the validated returned rect (all four axes) as the
  // authoritative final self geometry. Invalid returns fall back to input.
  const LayoutRect finalBounds = ResolveReturnedBounds(bounds, returnedBounds);
  ApplySelfBoundsIfChanged(finalBounds);

  // The RESULT is published unconditionally: mArrangedBounds is "the last
  // completed arrangement", read outside the cache path (GetArrangedBounds(),
  // the transition dispatcher, the re-entrancy fallback), so it must reflect the
  // work this pass just did. mArrangeDirty was consumed at pass ENTRY, so it is
  // NOT cleared here; see the conditional cache publish after the direction
  // resolver below.
  mArrangedBounds         = finalBounds;
  mArrangeResultAvailable = true; // A completed rect now exists for the re-entrancy fallback.

  // Ensure standalone children are arranged even when OnArrange (e.g. in
  // leaf views like Label) does not iterate children.
  ArrangeStandaloneChildren(finalBounds);

  // Mirror direct children when the effective layout direction resolves to
  // RIGHT_TO_LEFT. Runs once per arrange MISS, after every producer variant
  // (LayoutManager / ArrangeCallback / default OnArrange), keeping layout managers
  // direction-agnostic. A cache HIT returns above, and ReplayArrangeSubtreeFromCache
  // performs this same call, once per node it visits, with that node's cached width
  // -- the mirror is reproduced, not skipped. Children that have no arranged result
  // are ignored on both paths because no logical position belongs to this parent.
  ApplyLayoutDirection(finalBounds.width);

  // Conditional cache publish, mirroring Measure.
  //
  // The input KEY is published only when every premise of this pass still
  // holds at its end: no re-invalidation (mArrangeDirty was consumed at entry,
  // so seeing it true means one arrived mid-pass), no poison, a valid logical
  // context, and a valid measure cache -- an arrange run before this view was
  // ever measured used default/stale measured sizes for its children and must
  // not be frozen into the cache (plan34 11).
  //
  // The cached effective scale is READ here, never written: mEffectiveScaleValid is the
  // effective-scale sync bit, owned solely by GetEffectiveScale() and the
  // invalidation paths. Both logical terms are required and neither implies the
  // other. mEffectiveScaleValid alone would accept a pass whose context was
  // reset mid-flight and then re-validated by a later GetEffectiveScale() -- the
  // work done before that reset used the OLD scale. mEffectiveScaleInvalidatedDuringPass
  // alone would accept a pass that simply never established a context (the bit
  // still false at entry), which is what the GetEffectiveScale() at pass entry
  // rules out for the childless-default case.
  //
  // What this block writes is LIVE: mArrangeCacheValid / mLastArrangeInput /
  // mLastArrangeDirection are exactly what the cache-HIT test at the top of this
  // function reads on the next pass. Declining the publish here is therefore how a
  // pass whose premises did not survive forces the next Arrange() to recompute. It
  // is also what the subtree gate reads on every DESCENDANT it would elide, so
  // declining the publish here withdraws this view from any ancestor's hit too.
  //
  // mArrangeCacheBlockedDuringPass is the freshness-only member of this
  // predicate: a cache-only invalidation that arrived mid-pass declines the
  // publish but, unlike a poison, registers NO follow-up (the follow-up branch
  // below deliberately does not test it -- a cache-only invalidation must never
  // turn into a scheduled layout). The ancestor-invalidation walk sets it on an
  // unowned arrange-in-progress ancestor it clears, so this view's arrange cache
  // cannot be re-published over that clear before the pass ends.
  if(!mArrangeDirty && !mArrangePassPoisoned && !mArrangeCacheBlockedDuringPass && mEffectiveScaleValid && !mEffectiveScaleInvalidatedDuringPass && mMeasureCacheValid)
  {
    mLastArrangeInput     = bounds;
    mLastArrangeDirection = mViewImpl.Self().GetEffectiveLayoutDirection();
    mArrangeCacheValid    = true;
  }
  else if(mArrangePassPoisoned && !mArrangeDirty)
  {
    // Pure poison (re-entrancy / scale-context reset): nothing propagated an
    // invalidation of its own, so no follow-up layout is registered yet.
    // Register exactly one. A mid-pass InvalidateArrange() (mArrangeDirty true)
    // already registered inside that call, so it must not be registered twice.
    //
    // Caveat: an on-scene producer that re-enters its own Arrange() every pass
    // turns this one-shot follow-up into a per-frame loop; like the analogous
    // measure case and the LayoutFinished-slot case (see LayoutController), this
    // app-level non-convergence is deliberately not capped here. No first-party
    // producer does this.
    InvalidateArrange();
  }

  // Mark this view as having completed an arrange pass. Read by the layout
  // transition dispatcher to suppress ENTER on initial mount: the dispatcher
  // records views that were captured before this flag became true and
  // settles their declarative ENTER specs to final values without firing
  // OnStart / OnFinished.
  mInitialLayoutDone = true;

  // Register as a layout-finished candidate with the controller currently
  // running this pass (file-static resolved), only when subscribed. Snapshot
  // and emission happen later on the controller side (RTL correctness + before
  // StartTransitionsAfterLayout overwrites actor props).
  if(HasLayoutFinishedSignalConnections())
  {
    LayoutController::NotifyViewArranged(&mViewImpl);
  }

  return finalBounds;
}

void ViewDataImpl::MeasureStandaloneChildren(float visEffW, float visEffH)
{
  // Snapshot: a child's Measure() may mutate mImpl->mChildren.
  std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
  for(auto& childView : childSnapshot)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(!IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }
    float  childScale = childImpl.GetEffectiveScale();
    Insets margin     = childImpl.GetMargin();
    float  visMarginW = static_cast<float>(margin.start + margin.end) * childScale;
    float  visMarginH = static_cast<float>(margin.top + margin.bottom) * childScale;
    float  childVisW  = std::max(0.0f, visEffW - visMarginW);
    float  childVisH  = std::max(0.0f, visEffH - visMarginH);
    childImpl.Measure(childVisW, childVisH);

    // LOAD-BEARING, not bookkeeping. This measurement IS the parent's consumption of
    // the child's slot, so the corrective re-measure in ArrangeStandaloneChild must
    // not fire for it. Without this clear the publish inside the Measure() above
    // would leave the bit set and every standalone child would be re-measured once
    // more per pass, at the (generally different) arrange extent -- extra producer
    // runs, and a steady-state geometry that follows the arrange constraint instead
    // of the measure constraint.
    ViewDataImpl::Get(childImpl).mMeasuredSlotUnconsumed = false;
  }
}

// The corrective re-measure for an unconsumed standalone slot lives on the arrange
// path, so an Arrange() that returned early on a cache HIT would silently skip it.
// That is why !HasUnconsumedStandaloneChild() is a term of the arrange cache-hit
// predicate: a view with such a child always MISSES and therefore always reaches
// here. The bit is cleared below, so the correction costs at most one declined hit
// per out-of-band Measure().
//
// The term is O(direct children), so it is re-evaluated at every node of
// CanReplayArrangeSubtreeFromCache: a DESCENDANT holding an unconsumed slot refuses
// the ancestor's hit, which is what keeps this reachable under a subtree replay.
// The other way a standalone child can be missed -- being ADDED after the entry was
// published, when its slot bit is still false because it has never been measured --
// is closed on the add side, by the InvalidateArrange() in OnChildAdded.
void ViewDataImpl::ArrangeStandaloneChildren(const LayoutRect& bounds)
{
  // Snapshot: a child's Arrange() may mutate mImpl->mChildren.
  std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
  for(auto& childView : childSnapshot)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(!IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }
    ViewDataImpl& childData = ViewDataImpl::Get(childImpl);
    ArrangeStandaloneChild(mViewImpl, childImpl, bounds.width, bounds.height, childData.mMeasuredSlotUnconsumed);

    // Consumed: the slot has now been read against this parent's arrange extent
    // (and corrected first if it was unconsumed). Cleared unconditionally, so the
    // correction is paid for only after a fresh out-of-band Measure() and the
    // steady state costs nothing.
    childData.mMeasuredSlotUnconsumed = false;
  }
}

void ViewDataImpl::ApplyLayoutDirection(float parentWidth)
{
  if(mViewImpl.Self().GetEffectiveLayoutDirection() != Dali::LayoutDirection::RIGHT_TO_LEFT)
  {
    return;
  }

  // A SNAPSHOT, like every other child traversal in this file (ArrangeDefault,
  // MeasureStandaloneChildren, ArrangeStandaloneChildren, the cache-hit replay). The
  // SetPositionX below is an actor write that reaches OnPropertySet and a synchronous
  // PropertySetSignal emit, and hence application code that may add or remove children.
  // dali-core erases the child from its container BEFORE notifying
  // (ActorParentImpl::Remove), and ViewDataImpl::OnChildRemoved then erases it from
  // mChildren, so an index loop that re-read Count() would shift every following sibling
  // down one position and skip the last one entirely. Re-reading Count() keeps the index
  // in bounds; it does NOT keep the traversal complete, which is what this needs.
  //
  // This is the MISS path only: a cache-hit replay folds the mirror into each child's own
  // self apply and never calls here.
  std::vector<Ui::View> childSnapshot(mChildren.Begin(), mChildren.End());
  for(auto& childView : childSnapshot)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }

    Actor         child     = childImpl.Self();
    ViewDataImpl& childData = ViewDataImpl::Get(childImpl);

    // A producer that does not arrange this child has supplied no logical bounds
    // for the framework to mirror. Reading the actor's current (already physical)
    // position as if it were logical makes repeated identical passes alternate
    // between x and MirrorX(x). Leave the actor untouched until some producer
    // actually arranges the child and publishes a parent-local logical result.
    if(!childData.mArrangeResultAvailable)
    {
      continue;
    }

    // Mirror from the child's LOGICAL arranged bounds, never from the actor. This
    // is a pure function of the result the child published and is therefore
    // idempotent and immune to external actor-position writes.
    const LayoutRect logical  = childData.GetArrangedBounds();
    const float      mirrored = MirrorX(parentWidth, logical.x, logical.width);

    // Read-compare-write, exactly as ApplySelfBoundsIfChanged does it and with the same
    // exact `!=`. The value above is a pure function of the child's published logical
    // bounds and of this parent's arranged width, so a pass that changes nothing about
    // this child computes the value the actor already holds and the write is suppressed.
    // Reconciliation is unaffected: an external clobber still differs from the computed
    // value and is still repaired.
    if(child.GetProperty<float>(Actor::Property::POSITION_X) != mirrored)
    {
      child.SetPositionX(mirrored);
    }
  }
}

float ViewDataImpl::ComputeEffectiveScale() const
{
  if(mScalePolicy == UiScalePolicy::DISABLED)
  {
    return 1.0f;
  }
  if(mScalePolicy == UiScalePolicy::ENABLED)
  {
    return GetSystemScale();
  }

  // INHERIT: walk up the parent chain (Layout first, consistent with InvalidateMeasure)
  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    return GetImpl(parentLayout).GetEffectiveScale();
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    return GetImpl(parentView).GetEffectiveScale();
  }

  // Root: inherit from UiScaleManager. Every INHERIT chain terminates either
  // here or at a DISABLED/ENABLED ancestor, so the switch read inside
  // GetSystemScale() reaches every node in the tree exactly as a gate at the top
  // of this function would -- an INHERIT node just resolves to 1.0f through its
  // ancestor's already-memoized scale instead of returning it directly.
  return GetSystemScale();
}

void ViewDataImpl::DropCachedEffectiveScale()
{
  // Clearing mEffectiveScaleValid IS the scale drop: it is the sync bit for
  // mEffectiveScale, so the next GetEffectiveScale() on this node recomputes
  // from the (possibly re-rooted) parent chain.
  mEffectiveScaleValid = false;

  // The ACTOR-side copy goes with it. That bit's claim is "the animatable
  // VIEW_EFFECTIVE_SCALE property holds mEffectiveScale", and mEffectiveScale is
  // exactly what has just been retracted -- the recompute may land on a different
  // value, which Measure() must then push. Clearing the two together is what makes
  // the bit mean "in sync with the CURRENT effective scale" rather than "in sync
  // with whatever scale was current when it was set", and it is why this bit needs
  // no invalidation path of its own: every scale-context change already comes
  // through here.
  //
  // What this clear is still REQUIRED for, and what it no longer is:
  //  - the ARRANGE cache: required. Its hit predicate carries no scale term at all,
  //    so Corollary C -- "a valid arrange entry implies the scale has not moved" --
  //    is only true because every caller pairs this with InvalidateLayoutCaches().
  //  - the actor push: required. The bit cleared just above is the ONLY record that
  //    the animatable VIEW_EFFECTIVE_SCALE property is behind, so without this clear
  //    the next Measure() would skip the push and leave decoration constraints on the
  //    old scale.
  //  - the MEASURE cache: no longer required for CORRECTNESS. That cache keys on the
  //    effective scale (mLastMeasureScale), so a caller that forgot the pairing would
  //    take a miss rather than serve a size computed at the old scale. The pairing is
  //    still the contract; the key is the second line of defence, not a licence.
  mEffectiveScaleActorSynced = false;

  if(mArrangeInProgress)
  {
    // The running pass has already arranged part of this view against the OLD
    // scale, so its result must not be published even if a later
    // GetEffectiveScale() re-validates the bit before the pass ends.
    mEffectiveScaleInvalidatedDuringPass = true;
  }
}

void ViewDataImpl::InvalidateLayoutCaches()
{
  mMeasureCacheValid = false;
  mArrangeCacheValid = false;

  // mMeasureDirty is deliberately NOT touched here.
  //
  // Dirty and cache-valid are different bits with different owners. "Cache
  // valid" is a FRESHNESS claim about a stored result and is exactly what an
  // invalidation must retract. "Dirty" is a record that this view has layout
  // work which has not been consumed yet; it is consumed at pass entry
  // (MeasurePassGuard) and re-armed only by InvalidateMeasure(). Clearing it
  // from an invalidation does not invalidate anything -- it DISCARDS pending
  // work.
  //
  // This function used to clear it (a leftover from the era when this reset
  // restored the "never measured" state wholesale). On the recursive path that
  // was wrong: the callers follow the reset with InvalidateMeasure(), which
  // re-arms the node it is called on and propagates UPWARD only, so a
  // DESCENDANT that was dirty before the reset lost that dirty with nothing to
  // give it back -- the one place it is still read (OnViewSceneConnection's
  // standalone `isDirty` self-registration) would then decline to re-register a
  // reconnecting standalone descendant that genuinely had work pending.
  if(mMeasureInProgress)
  {
    mMeasurePassPoisoned = true;
  }
  if(mArrangeInProgress)
  {
    mArrangePassPoisoned = true;
  }
}

void ViewDataImpl::ResetSubtreeScaleAndLayoutCaches()
{
  // The two concerns of a scale-context reset, applied in order: the cached
  // scale itself, then every cached layout result that was derived from it.
  DropCachedEffectiveScale();
  InvalidateLayoutCaches();

  // Retract the invalidation propagation records for the whole subtree.
  //
  // This is THE reparent hook for them, and it has to be here rather than in
  // DropCachedEffectiveScale (which InvalidateMeasure calls on every invalidation and
  // where retracting them would defeat the coalescing entirely). Every path that can
  // move a View's ancestor chain runs through here -- OnChildAdded and OnChildRemoved
  // on the moved child, OnViewSceneConnection on a (re)connecting root -- and a record
  // written against the OLD chain must not authorise skipping a walk that now has a
  // DIFFERENT root to reach.
  //
  // Recursive because the chain change is subtree-wide: a descendant's record names
  // the same old root as the moved node's.
  mMeasurePropagationGeneration = 0u;
  mArrangePropagationGeneration = 0u;

  for(auto& childView : mChildren)
  {
    ViewDataImpl::Get(GetImpl(childView)).ResetSubtreeScaleAndLayoutCaches();
  }
}

void ViewDataImpl::InvalidateSubtreeLayoutForDirectionChange()
{
  // Same recursion SHAPE as ResetSubtreeScaleAndLayoutCaches above, three
  // deliberate differences: no cached effective scale is dropped (a direction
  // change moves no scale), no propagation generation is retracted (an unwritten
  // generation can only cost an extra later ancestor walk, never a missed
  // registration), and the dirty bits are SET here rather than left alone -- the
  // caller of that one follows it with an InvalidateMeasure() that re-arms only the
  // node it is called on, whereas here every affected DESCENDANT genuinely has
  // unconsumed layout work.
  if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    // Scheduling boundary: a standalone view's invalidation does not propagate
    // to its parent, so raise the same full InvalidateMeasure the old per-View
    // handler raised -- it self-registers and nudges a transition-bearing parent.
    InvalidateMeasure();
  }
  else
  {
    InvalidateLayoutCaches();
    mMeasureDirty = true; // SET, never clear: dirty is pending work, and the
    mArrangeDirty = true; // white-box direction tests assert it on the child.
  }

  for(uint32_t i = 0; i < mChildren.Count(); ++i)
  {
    Ui::View child = mChildren[i]; // keep the handle alive across app re-entry
    if(child.GetLayoutDirection() != Dali::LayoutDirection::INHERIT)
    {
      continue; // mirror of the core inherit-walk prune: a locally-set child's
                // resolved direction did not move, nor did its subtree's
    }
    ViewDataImpl::Get(GetImpl(child)).InvalidateSubtreeLayoutForDirectionChange();
  }
}

bool ViewDataImpl::UpdateColorBindingInternal(StringView bindingId, const UiColor& color)
{
  auto manager = UiColorManager::Get();
  if(!color.HasColorId())
  {
    manager.ClearBinding(mViewImpl.Self(), bindingId);
    return true;
  }

  if(manager.HasBinding(mViewImpl.Self(), bindingId))
  {
    manager.SetBindingColor(mViewImpl.Self(), bindingId, color);
    return true;
  }
  return false;
}

void ViewDataImpl::SetColorBindingInternal(StringView bindingId, const UiColor& color, ColorCallback callback)
{
  auto manager = UiColorManager::Get();
  manager.RegisterBinding(mViewImpl.Self(), bindingId, std::move(callback));
  manager.SetBindingColor(mViewImpl.Self(), bindingId, color);
}

bool ViewDataImpl::UpdateColorBindingInternal(StringView bindingId, const Gradient::Base& gradient)
{
  if(!Internal::ViewGradientColorBinding::HasTokenColor(gradient))
  {
    ClearGradientColorBinding(bindingId);
    return true;
  }
  return Internal::ViewGradientColorBinding::Update(mViewImpl, bindingId, gradient);
}

void ViewDataImpl::SetColorBindingInternal(StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback)
{
  if(Internal::ViewGradientColorBinding::Add(mViewImpl, bindingId, gradient, std::move(callback)))
  {
    UiColorManager::Get().ColorTableChangedSignal().Connect(this, &ViewDataImpl::OnColorTableChanged);
  }
}

void ViewDataImpl::OnColorTableChanged()
{
  Internal::ViewGradientColorBinding::ApplyAll(mViewImpl);
}

void ViewDataImpl::ClearGradientColorBinding(StringView bindingId)
{
  if(Internal::ViewGradientColorBinding::Clear(mViewImpl, bindingId))
  {
    UiColorManager::Get().ColorTableChangedSignal().Disconnect(this, &ViewDataImpl::OnColorTableChanged);
  }
}

void ViewDataImpl::ClearBackgroundBinding()
{
  UiColorManager::Get().ClearBinding(mViewImpl.Self(), BACKGROUND_COLOR_BINDING_ID);
  ClearGradientColorBinding(BACKGROUND_GRADIENT_BINDING_ID);
}

void ViewDataImpl::RegisterWithLayoutController()
{
  Actor  self   = mViewImpl.Self();
  Window window = Window::Get(self);

  DALI_LOG_DEBUG_INFO("[ViewImpl] RegisterWithLayoutController: hasWindow=%d\n", window ? 1 : 0);

  if(window)
  {
    // Lazy, once, and only for a view that registers with a LIVE window: an
    // on-scene layout root, or an on-scene standalone boundary. Such a view pays
    // the signal-connection cost (the callback, the signal's first pool block and
    // the BaseSignal itself); nothing else in the tree does, which is most of the
    // per-View memory this series once cost.
    //
    // INSIDE the window check, not above it. InvalidateMeasure falls through to
    // this function for ANY parentless view, so connecting before the check would
    // hook nearly every view in a build-then-add tree -- configure a view, and its
    // first layout property write invalidates it while it still has no parent --
    // and would forfeit the saving entirely.
    //
    // A live window is the EXACT predicate rather than a convenient
    // approximation: it is also the precondition for any layout pass to run at all
    // (LayoutController::Get takes a Window), so a view that cannot be hooked here
    // is a view no pass can reach either.
    //
    // Nothing is lost off-scene. The OnPropertySet interception is
    // window-independent and covers a direction WRITE on any View wherever it
    // happens; and a direction that moved while this subtree was detached cannot
    // outlive reconnection, because OnViewSceneConnection's layout-root path runs
    // ResetSubtreeScaleAndLayoutCaches() -- dropping every cached measure and
    // arrange result in the subtree -- before it registers here. Never
    // disconnected either; see mLayoutDirectionSignalConnected.
    if(!mLayoutDirectionSignalConnected)
    {
      self.LayoutDirectionChangedSignal().Connect(this, &ViewDataImpl::OnLayoutDirectionChanged);
      mLayoutDirectionSignalConnected = true;
    }

    LayoutController& controller = LayoutController::Get(window);
    // The internal registration path: this is the invalidation walk reaching its
    // layout root, not an application asking for a layout.
    controller.RequestLayoutInternal(&mViewImpl);

    // Register as a layout root in UiScaleManager so it gets invalidated when
    // the system scale changes. Duplicate registration is silently ignored.
    UiScaleManager::Get().RegisterLayoutRoot(Ui::View::DownCast(self));
  }
}

MeasuredSize ViewDataImpl::ApplyConstraints(const MeasuredSize& size) const
{
  // size is in visual (scale-applied) units; scale min/max (natural) accordingly.
  float        s           = mViewImpl.GetEffectiveScale();
  MeasuredSize constrained = size;
  constrained.width        = std::max(constrained.width, GetMinimumWidth() * s);
  constrained.height       = std::max(constrained.height, GetMinimumHeight() * s);
  constrained.width        = std::min(constrained.width, GetMaximumWidth() * s);
  constrained.height       = std::min(constrained.height, GetMaximumHeight() * s);
  return constrained;
}

Ui::Layout ViewDataImpl::GetParentLayout() const
{
  Actor parent = mViewImpl.Self().GetParent();
  if(parent)
  {
    return Ui::Layout::DownCast(parent);
  }
  return Ui::Layout();
}

Ui::View ViewDataImpl::GetParentView() const
{
  Actor parent = mViewImpl.Self().GetParent();
  if(parent)
  {
    return Ui::View::DownCast(parent);
  }
  return Ui::View();
}

void ViewDataImpl::SetBackgroundGradientInternal(const Gradient::Base& gradient)
{
  SetBackground(Internal::CreateGradientVisualPropertyMap(gradient));
}

void ViewDataImpl::SetColorInternal(const Vector4& color)
{
  mViewImpl.Self().SetProperty(Actor::Property::COLOR, color);
}

void ViewDataImpl::SetBorderlineColorInternal(const Vector4& color)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::BORDERLINE_COLOR, color);
}

void ViewDataImpl::SetBackgroundColorInternal(const Vector4& color)
{
  Property::Map map = Internal::CreateColorVisualPropertyMap(color);

  Ui::Internal::Visual::Base* visualImplPtr = GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr && visualImplPtr->GetType() == Ui::Integration::InternalVisualType::COLOR)
  {
    // Update background color only
    visualImplPtr->DoAction(Dali::Ui::Integration::Visual::Action::UPDATE_PROPERTY, map);
    return;
  }

  SetBackground(map);
}

MeasuredSize ViewDataImpl::DispatchMeasureWithLayoutManager(LayoutManager* manager, float widthConstraint, float heightConstraint)
{
  float s = mViewImpl.GetEffectiveScale();

  Insets padding = mViewImpl.GetPadding();
  float  visPadW = static_cast<float>(padding.start + padding.end) * s;
  float  visPadH = static_cast<float>(padding.top + padding.bottom) * s;

  float requestedWidth  = mViewImpl.GetRequestedWidth();
  float requestedHeight = mViewImpl.GetRequestedHeight();

  float requestedVisW = (requestedWidth >= 0.f) ? requestedWidth * s : requestedWidth;
  float requestedVisH = (requestedHeight >= 0.f) ? requestedHeight * s : requestedHeight;
  float effectiveVisW = (requestedVisW >= 0.f) ? requestedVisW : widthConstraint;
  float effectiveVisH = (requestedVisH >= 0.f) ? requestedVisH : heightConstraint;
  float contentVisW   = std::max(0.0f, effectiveVisW - visPadW);
  float contentVisH   = std::max(0.0f, effectiveVisH - visPadH);

  MeasuredSize visContent = manager->Measure(&mViewImpl, contentVisW, contentVisH);

  float resultVisW;
  if(requestedVisW >= 0.f)
    resultVisW = requestedVisW;
  else if(requestedWidth == MATCH_PARENT)
    resultVisW = mViewImpl.GetMinimumWidth() * s;
  else
    resultVisW = visContent.width + visPadW;

  float resultVisH;
  if(requestedVisH >= 0.f)
    resultVisH = requestedVisH;
  else if(requestedHeight == MATCH_PARENT)
    resultVisH = mViewImpl.GetMinimumHeight() * s;
  else
    resultVisH = visContent.height + visPadH;

  return MeasuredSize(resultVisW, resultVisH);
}

void ViewDataImpl::DispatchArrangeWithLayoutManager(LayoutManager* manager, const LayoutRect& visualBounds)
{
  // Self geometry is applied centrally in Arrange(); the manager arranges
  // children only within the padding-adjusted content bounds. Owner final == input.
  float  s       = mViewImpl.GetEffectiveScale();
  Insets padding = mViewImpl.GetPadding();

  LayoutRect visContentBounds;
  visContentBounds.x      = static_cast<float>(padding.start) * s;
  visContentBounds.y      = static_cast<float>(padding.top) * s;
  visContentBounds.width  = std::max(0.0f, visualBounds.width - static_cast<float>(padding.start + padding.end) * s);
  visContentBounds.height = std::max(0.0f, visualBounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  manager->Arrange(&mViewImpl, visContentBounds);
}

LayoutRect ViewDataImpl::DispatchArrangeWithCallback(ArrangeCallback* callback, const LayoutRect& visualBounds)
{
  // Self geometry is applied centrally in Arrange(); the callback returns the
  // view's final self bounds.
  Ui::View view = Ui::View::DownCast(mViewImpl.Self());
  return callback->Invoke(view, visualBounds);
}

void ViewDataImpl::ApplySelfBoundsIfChanged(const LayoutRect& bounds)
{
  Actor self = mViewImpl.Self();

  // Read the event-side target property and write only axes that actually
  // differ. Exact comparison (not epsilon): the rect handed in is authoritative
  // geometry -- the MISS path's final bounds, or a cache-hit replay's physical
  // resolution of them (the same rect with x mirrored about the parent width) -- and the
  // actor target must equal it exactly.
  if(self.GetProperty<float>(Actor::Property::POSITION_X) != bounds.x)
  {
    self.SetPositionX(bounds.x);
  }
  if(self.GetProperty<float>(Actor::Property::POSITION_Y) != bounds.y)
  {
    self.SetPositionY(bounds.y);
  }
  if(self.GetProperty<float>(Actor::Property::SIZE_WIDTH) != bounds.width)
  {
    self.SetWidth(bounds.width);
  }
  if(self.GetProperty<float>(Actor::Property::SIZE_HEIGHT) != bounds.height)
  {
    self.SetHeight(bounds.height);
  }

  // The layout engine applies self size via SetWidth/SetHeight, which does not
  // route through Actor::OnSizeSet (only Actor::SetSize does). Drive the same
  // render-effect refresh OnSizeSet would, so render effects (e.g. blur) that
  // read the final layout size refresh for layout-sized views that never
  // receive an explicit SetSize. Fitting mode is intentionally not re-applied
  // here: it is already driven for layout-arranged views by the
  // layout-finished signal, and re-registering it here would apply it twice.
  // Track against a dedicated field rather than mSize: Arrange() can run with
  // provisional/degenerate bounds for views outside real layout measurement
  // (e.g. a plain View given an explicit Actor size but never measured by a
  // layout container), and mSize is load-bearing for Process()/ApplyFittingMode
  // via the OnSizeSet path - clobbering it here desyncs fitting mode sizing.
  const Vector2 newSize(bounds.width, bounds.height);
  if(mLastArrangedRenderEffectSize != newSize)
  {
    mLastArrangedRenderEffectSize = newSize;
    RefreshRenderEffects();
  }
}

void ViewDataImpl::EmitFocusChangedSignal(bool focusGained)
{
  Dali::Ui::View handle = Ui::View::DownCast(mViewImpl.Self());

  if(Dali::Integration::Accessibility::IsUp())
  {
    auto accessible = GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitFocused(focusGained);
    }
  }

  // signals are allocated dynamically when someone connects
  if(!mFocusChangedSignal.Empty())
  {
    mFocusChangedSignal.Emit(handle, focusGained);
  }
}

void ViewDataImpl::OnChildOrderChanged(Actor parent, Actor orderChangedChild)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Actor                           self            = mViewImpl.Self();
  uint32_t                        actorChildCount = self.GetChildCount();
  IntegrationView::ChildContainer newChildren;
  newChildren.Reserve(actorChildCount);

  for(uint32_t i = 0; i < actorChildCount; ++i)
  {
    Ui::View view = Ui::View::DownCast(self.GetChildAt(i));
    if(view)
    {
      auto it = std::find(mChildren.begin(), mChildren.end(), view);
      if(it != mChildren.end())
      {
        // Copied rather than moved out of mChildren, so mChildren still holds the
        // PREVIOUS order for the comparison below. The move-assignment further
        // down releases the old container anyway.
        newChildren.PushBack(*it);
      }
    }
  }

  // dali-core does NOT suppress every reorder signal that moves nothing. RaiseToTop /
  // LowerToBottom do gate their emit on an actual move, but RaiseChildAbove and
  // LowerChildBelow set their `raised` / `lowered` flag OUTSIDE the guard that performs
  // the move (ActorParentImpl::RaiseChildAbove / LowerChildBelow), so asking to raise a
  // child above one it is already above emits all the same. Convergence for the
  // repeat-every-pass callers (RecyclerViewImpl::OnArrange raises its scroll bar to the
  // top on every arrange) therefore rests on THIS guard, not on an external invariant.
  //
  // The guard also catches what dali-core could not suppress in any case: a reorder among
  // the NON-View actor children, which really does move an actor while leaving the
  // View-only sequence rebuilt above identical to mChildren.
  //
  // Skipping the reorder tagging as well as the invalidation is intentional: no
  // View moved, so no View's dispatch should be tagged REORDERED.
  if(newChildren.Count() == mChildren.Count() &&
     std::equal(newChildren.Begin(), newChildren.End(), mChildren.Begin()))
  {
    return;
  }

  mChildren = std::move(newChildren);

  // Tag every child as reordered so the layout transition dispatcher can
  // tag CHANGE-slot dispatches with @c LayoutChangeCause::REORDERED. The
  // dispatcher consumes this set once per layout pass. Skip when no
  // transition is attached so stale records cannot leak across an
  // unrelated SetLayoutTransition + layout pass later, and so raw
  // ViewImpl* pointers do not outlive their owning views without a
  // central cleanup hook.
  if(HasLayoutTransition())
  {
    for(auto& childView : mChildren)
    {
      mLayoutTransitionData->pendingReorderedChildren.insert(&GetImpl(childView));
    }
  }

  // A logical child-order change can alter the measured size (e.g. a wrap
  // layout where line-breaking depends on child order), so invalidate measure
  // — not just arrange — for the whole reorder path.
  // This view's own internal primitive, not mViewImpl.InvalidateMeasure(): an
  // actor-side reorder is a framework-internal event, not an application call.
  InvalidateMeasure();
}

void ViewDataImpl::OnLayoutDirectionChanged(Dali::Actor /* actor */, Dali::LayoutDirection::Type /* type */)
{
  // BOTH axes, via InvalidateMeasure (which raises the arrange dirty too).
  //
  // Arrange alone would be enough for every FIRST-PARTY producer -- the direction is
  // consumed by ApplyLayoutDirection, which mirrors the x of this view's
  // non-standalone children, and no in-library measure producer sizes on it. But
  // GetEffectiveLayoutDirection() is public and OnMeasure() is virtual, so an
  // application's measure producer CAN size on the direction, and the measure cache
  // key has no direction term: invalidating arrange alone would leave such a producer
  // pinned at its pre-change measured size until some unrelated invalidation arrived.
  //
  // Closing that here rather than by contract costs nothing that matters. The
  // alternative -- a direction term in the measure cache KEY -- was rejected because
  // it would put a layout-direction read into the measure HIT predicate, which runs
  // per view per pass; this call is on the direction-CHANGE path, which runs on a
  // locale or explicit direction switch and is idle otherwise. The extra work is one
  // re-measure of the affected subtree per such switch, and in exchange no
  // application has to know that measure and arrange key on different inputs.
  //
  // Core has already filtered this signal down to the actors whose RESOLVED direction
  // actually changed, so there is no value-change guard to repeat here.
  //
  // The subtree walk is this handler's own job now: only a LAYOUT ROOT holds this
  // connection (RegisterWithLayoutController), so the per-View connections that used
  // to deliver the signal to every affected descendant individually are gone.
  // InvalidateSubtreeLayoutForDirectionChange drops the caches and raises the dirty
  // bits down the inherit chain; the InvalidateMeasure() after it is what propagates
  // upward from this node and schedules the pass.
  InvalidateSubtreeLayoutForDirectionChange();
  InvalidateMeasure();
}

// =============================================================================
// Trait Management
// =============================================================================

void ViewDataImpl::NotifyTraitsViewDestroying()
{
  for(auto& iter : mTraits)
  {
    if(iter.second)
    {
      iter.second->OnViewDestroying(&mViewImpl);
    }
  }
}

void ViewDataImpl::SetTrait(TraitId id, IntrusivePtr<TraitObject> object)
{
  Ui::View self = Ui::View::DownCast(mViewImpl.Self());

  if(id == Integration::ReservedTraitId::CORE_INTERACTION_TRAITS)
  {
    if(mCoreInteractionObject)
    {
      DALI_ASSERT_ALWAYS(false && "Core interaction trait object cannot be replaced once set");
      return;
    }
    auto* traitObject = AsCoreInteractionObject(object.Get());
    DALI_ASSERT_ALWAYS(traitObject &&
                       "Trait for ReservedTraitId::CORE_INTERACTION_TRAITS must be a CoreInteractionObject");
    mCoreInteractionObject = traitObject;
  }

  for(auto& entry : mTraits)
  {
    if(entry.first == id)
    {
      if(entry.second == object)
      {
        return;
      }
      if(entry.second)
      {
        entry.second->OnDetaching(id, self);
      }
      entry.second = object;
      if(entry.second)
      {
        entry.second->OnAttached(id, self);
      }
      OnLayoutProducerTraitChanged(id);
      return;
    }
  }

  mTraits.emplace_back(id, object);
  if(mTraits.back().second)
  {
    mTraits.back().second->OnAttached(id, self);
  }
  OnLayoutProducerTraitChanged(id);
}

void ViewDataImpl::OnLayoutProducerTraitChanged(TraitId id)
{
  // Two reserved traits carry a view's layout producers: LAYOUT_SIGNALS holds the
  // ArrangeCallback and LAYOUT_MANAGER the LayoutManager, the first two rungs of the
  // dispatch order in Arrange() (ArrangeCallback > LayoutManager > OnArrange).
  // Swapping either changes WHICH producer runs, so the derived policy bit must not
  // be left describing the one that just went away.
  //
  // The in-library mutators (SetArrangeCallback / AttachLayoutManager) already do this
  // for themselves; this covers the public Integration::View::SetTrait/RemoveTrait
  // surface, which can reach the same trait ids with no other bookkeeping. Nothing in
  // the library relies on it today -- it is here so the derived bit cannot describe
  // a producer that has just been replaced.
  //
  // The in-library mutators do also LAND here once: the first SetArrangeCallback /
  // SetMeasureCallback creates the LAYOUT_SIGNALS trait through
  // EnsureLayoutCallbacksObject -> SetTrait, and AttachLayoutManager installs
  // LAYOUT_MANAGER the same way. On that first-install path the invalidation below is
  // redundant with the mutator's own (and broader: measure, not just arrange) --
  // deliberate over-invalidation on a cold path, accepted for one funnel instead of
  // two exemption lists.
  if(id != Integration::ReservedTraitId::LAYOUT_SIGNALS &&
     id != Integration::ReservedTraitId::LAYOUT_MANAGER)
  {
    return;
  }

  // A LAYOUT_SIGNALS swap replaces the callback and its policy. Reset the policy
  // to ArrangePolicy::IF_CHANGED before deriving the active producer again.
  if(id == Integration::ReservedTraitId::LAYOUT_SIGNALS)
  {
    mArrangeCallbackAlways = false;
  }

  RefreshArrangeProducerPolicy();

  // Pair the change with an invalidation so a view that has already published an entry
  // cannot be served from it under the new producer.
  //
  // The MEASURE axis has to go too, and that is why this is InvalidateMeasure() rather
  // than InvalidateArrange(): both trait ids carry a measure producer as well as an
  // arrange one. LAYOUT_MANAGER supplies LayoutManager::Measure(), and LAYOUT_SIGNALS
  // holds the MeasureCallback and the ArrangeCallback in ONE LayoutCallbacksObject, so
  // removing it removes both. Leaving the measured slot cached is worse than leaving the
  // arranged one: the slot is what every ancestor arranges FROM, so a stale slot
  // survives as geometry after the view's own arrange has been recomputed.
  // InvalidateMeasure() supersedes InvalidateArrange() here -- it clears both cache bits
  // and raises both dirty bits.
  //
  // Gated exactly as in SetArrangePolicy(), and for the same reason: this can run before
  // the CustomActor handle exists (the first SetMeasureCallback/SetArrangeCallback
  // creates the LAYOUT_SIGNALS trait) and the invalidation walk dereferences Self().
  // Neither cache bit can be true without a handle, because only a completed pass sets
  // them, so the gate cannot suppress a needed invalidation.
  if(mMeasureCacheValid || mArrangeCacheValid)
  {
    InvalidateMeasure();
  }
}

IntrusivePtr<TraitObject> ViewDataImpl::GetTrait(TraitId id) const
{
  for(auto& entry : mTraits)
  {
    if(entry.first == id)
    {
      return entry.second;
    }
  }
  return IntrusivePtr<TraitObject>();
}

bool ViewDataImpl::RemoveTrait(TraitId id)
{
  if(id == Integration::ReservedTraitId::CORE_INTERACTION_TRAITS)
  {
    DALI_ASSERT_ALWAYS(false && "Core interaction trait object cannot be removed once set");
    return false;
  }

  for(auto it = mTraits.begin(); it != mTraits.end(); ++it)
  {
    if(it->first == id)
    {
      Ui::View self = Ui::View::DownCast(mViewImpl.Self());
      if(it->second)
      {
        it->second->OnDetaching(id, self);
      }
      mTraits.erase(it);
      OnLayoutProducerTraitChanged(id);
      return true;
    }
  }
  return false;
}

void ViewDataImpl::SetAttachment(AttachmentId id, UniqueAny attachment)
{
  if(!mAttachments)
  {
    mAttachments = std::make_unique<AttachmentContainer>();
  }
  mAttachments->SetAttachment(id, Dali::Move(attachment));
}

bool ViewDataImpl::RemoveAttachment(AttachmentId id)
{
  return mAttachments ? mAttachments->RemoveAttachment(id) : false;
}

UniqueAny ViewDataImpl::DetachAttachment(AttachmentId id)
{
  return mAttachments ? mAttachments->DetachAttachment(id) : UniqueAny();
}

UniqueAny* ViewDataImpl::GetAttachment(AttachmentId id)
{
  return mAttachments ? mAttachments->GetAttachment(id) : nullptr;
}

const UniqueAny* ViewDataImpl::GetAttachment(AttachmentId id) const
{
  return mAttachments ? mAttachments->GetAttachment(id) : nullptr;
}

void ViewDataImpl::SetState(ViewState stateToChange, bool on, InputEvent cause)
{
  SetState(on ? ViewState::NORMAL : stateToChange,
           on ? stateToChange : ViewState::NORMAL,
           cause);
}

void ViewDataImpl::SetState(ViewState statesToClear, ViewState statesToSet, InputEvent cause)
{
  // NOTE Orthogonal state constraint: Disabled is mutually exclusive with Focused and Pressed.
  // Clear them immediately rather than waiting for potentially late system events.

  // NOTE that when the view is focused and user sets `view.SetEnabled(false)`,
  // the event squence will be: "Focused out" -> "Enabled changed".

  ViewState prev = mState;

  mState = mState - statesToClear + statesToSet;

  // NOTE Handle orthogonal state constraint
  // When DISABLED added,
  // - PRESSED needs to be cleaned immediately
  // - FOCUSED should have gone already (ASSERT(!mState.Contains(FOCUSED)))
  // When PSEUDO_DISABLED added,
  // - PRESSED needs to be cleaned immediately
  // - FOCUSED can exist
  if(statesToSet.IsAnyDisabled())
  {
    mState = mState - ViewState::PRESSED - ViewState::HOVERED;
  }

  // NOTE Handle orthogonal state constraint
  // This is the case that the focus has gone because it turned disabled.
  // (but disabled state hasn't dispatched yet)
  // -> Immediately update states at once.
  if(statesToClear.Contains(ViewState::FOCUSED))
  {
    mState = mState - ViewState::FOCUS_INDICATED;

    if(!mViewImpl.Self().IsEnabled())
    {
      mState = mState - ViewState::PRESSED + ViewState::DISABLED;
    }
  }

  if(mState != prev)
  {
    const bool pressedClearedByDisabled = prev.Contains(ViewState::PRESSED) && !mState.Contains(ViewState::PRESSED) && mState.IsAnyDisabled();
    const bool hoveredClearedByDisabled = prev.Contains(ViewState::HOVERED) && !mState.Contains(ViewState::HOVERED) && mState.IsAnyDisabled();

    if(pressedClearedByDisabled || hoveredClearedByDisabled)
    {
      cause = cause ? cause.WithCancellation() : InputEvent::Programmatic().WithCancellation();
    }

    View self = View::DownCast(mViewImpl.Self());
    ViewStateManager::Get().NotifyStateChanged(self, prev, mState, cause);

    if(pressedClearedByDisabled || hoveredClearedByDisabled)
    {
      if(auto* traitObject = GetCoreInteractionObject())
      {
        if(pressedClearedByDisabled)
        {
          traitObject->OnPressedClearedByViewState(self, cause);
        }

        if(hoveredClearedByDisabled)
        {
          traitObject->OnHoveredClearedByViewState(self, cause);
        }
      }
    }
  }
}

void ViewDataImpl::SetNamedStateObserver(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  auto* existing = dynamic_cast<StateHandlerTrait*>(GetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT).Get());

  if(!existing)
  {
    IntrusivePtr<TraitObject> stateHandlerTrait(new StateHandlerTrait());
    existing = static_cast<StateHandlerTrait*>(stateHandlerTrait.Get());
    SetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT, stateHandlerTrait);
  }

  existing->Set(id.CStr(), tracker, callback);
}

bool ViewDataImpl::UnsetNamedStateObserver(const Dali::String& id)
{
  auto* existing = dynamic_cast<StateHandlerTrait*>(GetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT).Get());
  if(!existing)
  {
    return false;
  }

  return existing->Unset(id.CStr());
}

bool ViewDataImpl::UnsetNamedStateObserverIfNotExecuting(const Dali::String& id)
{
  auto* existing = dynamic_cast<StateHandlerTrait*>(GetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT).Get());
  if(!existing)
  {
    return false;
  }

  return existing->UnsetIfNotExecuting(id.CStr());
}

Internal::CoreInteractionObject* ViewDataImpl::GetCoreInteractionObject() const
{
  return mCoreInteractionObject;
}

ViewDataImpl& ViewDataImpl::Get(ViewImpl& viewImpl)
{
  DALI_ASSERT_ALWAYS(Dali::Adaptor::IsEventThread() && "Must be called from the event thread!");

  return viewImpl.GetViewDataImpl();
}

const ViewDataImpl& ViewDataImpl::Get(const ViewImpl& viewImpl)
{
  DALI_ASSERT_ALWAYS(Dali::Adaptor::IsEventThread() && "Must be called from the event thread!");

  return viewImpl.GetViewDataImpl();
}

void ViewDataImpl::ResourceReady()
{
  DALI_ASSERT_ALWAYS(Dali::Adaptor::IsEventThread() && "Must be called from the event thread!");

  // Emit signal if all enabled visuals registered by the view are ready or there are no visuals.
  if(DALI_LIKELY(mVisualData) && mVisualData->IsResourceReady())
  {
    EmitResourceReadySignal();
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Integration::Visual::Base& visual)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Integration::Visual::Base& visual, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, depthIndex);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Integration::Visual::Base& visual, bool enabled)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Integration::Visual::Base& visual, bool enabled, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled, depthIndex);
  }
}

void ViewDataImpl::UnregisterVisual(Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(index);
  }
}

Ui::Integration::Visual::Base ViewDataImpl::GetVisual(Property::Index index) const
{
  return Ui::Integration::Visual::Base(GetVisualImplPtr(index));
}

Ui::Internal::Visual::Base* ViewDataImpl::GetVisualImplPtr(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualImplPtr(index);
  }
  return nullptr;
}

bool ViewDataImpl::IsResourceReady() const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsResourceReady();
  }
  return true;
}

void ViewDataImpl::OnSceneConnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::OnSceneConnection number of registered visuals(%d)\n",
                mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mViewImpl.Self();
    mVisualData->ConnectScene(self);
  }

  if(mRenderEffectData && mRenderEffectData->offScreenRendering)
  {
    mRenderEffectData->offScreenRendering->SetOwnerView(Ui::View(mViewImpl.GetOwner()));
  }
}

void ViewDataImpl::OnSceneDisconnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "View::OnSceneDisconnection number of registered visuals(%d)\n",
                mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mViewImpl.Self();
    mVisualData->ClearScene(self);
  }

  if(mRenderEffectData && mRenderEffectData->offScreenRendering)
  {
    mRenderEffectData->offScreenRendering->ClearOwnerView();
  }
}

void ViewDataImpl::EnableCornerPropertiesOverridden(Ui::Integration::Visual::Base& visual, bool enable,
                                                    Dali::Constraint cornerRadiusConstraint)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
  }
}

void ViewDataImpl::EnableVisual(Property::Index index, bool enable)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableVisual(index, enable);
  }
}

bool ViewDataImpl::IsVisualEnabled(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsVisualEnabled(index);
  }
  return false;
}

Ui::Visual::ResourceStatus ViewDataImpl::GetVisualResourceStatus(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualResourceStatus(index);
  }
  return Ui::Visual::ResourceStatus::READY;
}

void ViewDataImpl::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                            const Dali::Property::Value& attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoAction(visualIndex, actionId, attributes);
  }
}

void ViewDataImpl::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                     const Dali::Any& attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoActionExtension(visualIndex, actionId, attributes);
  }
}

bool ViewDataImpl::AddVisualObject(Dali::Ui::VisualBase visualBase, Dali::Ui::Integration::Visual::InternalContainerRangeType internalContainerRangeType)
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->AddVisualObject(visualBase, internalContainerRangeType);
  }
  return false;
}

bool ViewDataImpl::AddShadowVisualObject(Dali::Ui::VisualBase visualBase, Dali::Ui::Integration::Visual::InternalContainerRangeType internalContainerRangeType)
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->AddShadowVisualObject(visualBase, internalContainerRangeType);
  }
  return false;
}

void ViewDataImpl::RemoveVisualObject(Dali::Ui::VisualBase visualBase)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RemoveVisualObject(visualBase);
  }
}

uint32_t ViewDataImpl::GetVisualObjectCount(Dali::Ui::Integration::Visual::InternalContainerRangeType internalContainerRangeType) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualObjectCount(internalContainerRangeType);
  }
  return 0u;
}

Dali::Ui::VisualBase ViewDataImpl::GetVisualObjectAt(Dali::Ui::Integration::Visual::InternalContainerRangeType internalContainerRangeType, uint32_t siblingOrder) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualObjectAt(internalContainerRangeType, siblingOrder);
  }
  return Dali::Ui::VisualBase();
}

void ViewDataImpl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  DALI_ASSERT_ALWAYS(Dali::Adaptor::IsEventThread() && "Must be called from the event thread!");

  Ui::View view = Ui::View::DownCast(BaseHandle(object));

  if(view)
  {
    ViewImpl& viewImpl(GetImpl(view));

    switch(index)
    {
      case Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().leftId = focusId;
        }
      }
      break;

      case Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().rightId = focusId;
        }
      }
      break;

      case Ui::View::Property::UP_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().upId = focusId;
        }
      }
      break;

      case Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().downId = focusId;
        }
      }
      break;

      case Ui::View::Property::BACKGROUND:
      {
        std::string          url;
        Vector4              color;
        const Property::Map* map = value.GetMap();
        ViewDataImpl::Get(viewImpl).ClearBackgroundBinding();
        if(map && !map->Empty())
        {
          viewImpl.GetViewDataImpl().SetBackground(*map);
        }
        else if(GetStdString(value, url))
        {
          viewImpl.GetViewDataImpl().SetBackground(CreateImageVisualPropertyMap(Dali::String(url.c_str())));
        }
        else if(value.Get(color))
        {
          viewImpl.GetViewDataImpl().SetBackground(CreateColorVisualPropertyMap(color));
        }
        else
        {
          // The background is an empty property map, so unregister the background visual.
          viewImpl.GetViewDataImpl().UnregisterVisual(Ui::View::Property::BACKGROUND);
        }
        break;
      }

      case Ui::View::Property::MARGIN:
      {
        Insets marginInsets;
        if(GetInsetsFromPropertyValue(value, marginInsets))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(dataImpl.mMargin != marginInsets)
          {
            dataImpl.mMargin = marginInsets;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::PADDING:
      {
        Insets paddingInsets;
        if(GetInsetsFromPropertyValue(value, paddingInsets))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(dataImpl.mPadding != paddingInsets)
          {
            dataImpl.mPadding = paddingInsets;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          viewImpl.GetViewDataImpl().SetShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so clear the whole shadow stack.
          viewImpl.GetViewDataImpl().ClearShadow();
        }
        break;
      }

      case Ui::View::Property::DISPATCH_KEY_EVENTS:
      {
        bool dispatch;
        if(value.Get(dispatch))
        {
          viewImpl.GetViewDataImpl().mDispatchKeyEvents = dispatch;
        }
        break;
      }

      case Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().clockwiseId = focusId;
        }
        break;
      }
      case Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().counterClockwiseId = focusId;
        }
        break;
      }
      case Ui::View::Property::FORWARD_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().forwardId = focusId;
        }
        break;
      }
      case Ui::View::Property::BACKWARD_FOCUSABLE_VIEW_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().EnsureFocusNavigationData().backwardId = focusId;
        }
        break;
      }

      case Ui::View::Property::OFFSCREEN_RENDERING:
      {
        int32_t offscreenRenderingType;
        if(value.Get(offscreenRenderingType))
        {
          viewImpl.GetViewDataImpl().SetOffScreenRendering(offscreenRenderingType);
        }
        break;
      }

      case Ui::View::Property::INNER_SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          viewImpl.GetViewDataImpl().SetInnerShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          viewImpl.GetViewDataImpl().ClearInnerShadow();
        }
        break;
      }

      case Ui::View::Property::BORDERLINE:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          viewImpl.GetViewDataImpl().SetBorderline(*map, true);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          viewImpl.GetViewDataImpl().ClearBorderline();
        }
        break;
      }
      case Ui::View::Property::CORNER_RADIUS:
      {
        float radiusFloat = 0.0f;
        if(value.Get(radiusFloat))
        {
          view.SetProperty(Ui::View::Property::CORNER_RADIUS,
                           Vector4(radiusFloat, radiusFloat, radiusFloat, radiusFloat));
          break;
        }

        Vector4 radius;
        if(value.Get(radius))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
          {
            viewImpl.GetViewDataImpl().mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::CORNER_RADIUS,
                                                                                    false);
          }
          viewImpl.GetViewDataImpl().UpdateCornerRadius();
        }
        break;
      }

      case Ui::View::Property::CORNER_RADIUS_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
          {
            viewImpl.GetViewDataImpl().mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::CORNER_RADIUS_POLICY,
                                                                                    false);
          }
          viewImpl.GetViewDataImpl().UpdateCornerRadius();
        }
        break;
      }

      case Ui::View::Property::CORNER_SQUARENESS:
      {
        float squarenessFloat = 0.0f;
        if(value.Get(squarenessFloat))
        {
          view.SetProperty(Ui::View::Property::CORNER_SQUARENESS,
                           Vector4(squarenessFloat, squarenessFloat, squarenessFloat, squarenessFloat));
          break;
        }

        Vector4 squareness;
        if(value.Get(squareness))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
          {
            viewImpl.GetViewDataImpl().mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::CORNER_SQUARENESS,
                                                                                    false);
          }
          viewImpl.GetViewDataImpl().UpdateCornerRadius();
        }
        break;
      }

      case Ui::View::Property::BORDERLINE_WIDTH:
      {
        float width;
        if(value.Get(width))
        {
          viewImpl.GetViewDataImpl().UpdateBorderline();
        }
        break;
      }

      case Ui::View::Property::BORDERLINE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          viewImpl.GetViewDataImpl().UpdateBorderline();
        }
        break;
      }

      case Ui::View::Property::BORDERLINE_OFFSET:
      {
        float offset;
        if(value.Get(offset))
        {
          viewImpl.GetViewDataImpl().UpdateBorderline();
        }
        break;
      }

      case VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX:
      {
        ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();

        // This is the single funnel for EVERY event-side write of the property --
        // the framework's own push from Measure() and any external clobber alike,
        // because dali-core routes a registered animatable property's writes
        // through its set function. So it is the one place that can retract the
        // actor-side sync bit, and retracting it is what keeps Measure()'s
        // corrective push reachable on a cache HIT (see the gate there). Measure()
        // sets the bit AFTER its own SetProperty, so this clear does not fight it.
        dataImpl.mEffectiveScaleActorSynced = false;

        // We don't need to hold data for it. But need to apply fitting mode now.
        dataImpl.SizeOrUiScaleChanged();
        break;
      }

      case Ui::View::Property::REQUESTED_WIDTH:
      {
        float width;
        if(value.Get(width) && IsValidRequestedSize(width))
        {
          // Snap a near-sentinel input to the exact sentinel so downstream
          // exact comparisons (== WRAP_CONTENT / == MATCH_PARENT) are correct.
          if(FloatEqual(width, WRAP_CONTENT))
          {
            width = WRAP_CONTENT;
          }
          else if(FloatEqual(width, MATCH_PARENT))
          {
            width = MATCH_PARENT;
          }
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.mRequestedWidth, width))
          {
            dataImpl.mRequestedWidth = width;
            dataImpl.InvalidateMeasure();
            if(width >= 0 && !dataImpl.GetParentLayout() && !dataImpl.GetParentView() &&
               !Integration::View::HasLayoutCapability(viewImpl) && viewImpl.GetChildViewCount() == 0)
            {
              viewImpl.Self().SetWidth(width);
            }
          }
        }
        break;
      }

      case Ui::View::Property::REQUESTED_HEIGHT:
      {
        float height;
        if(value.Get(height) && IsValidRequestedSize(height))
        {
          // Snap a near-sentinel input to the exact sentinel so downstream
          // exact comparisons (== WRAP_CONTENT / == MATCH_PARENT) are correct.
          if(FloatEqual(height, WRAP_CONTENT))
          {
            height = WRAP_CONTENT;
          }
          else if(FloatEqual(height, MATCH_PARENT))
          {
            height = MATCH_PARENT;
          }
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.mRequestedHeight, height))
          {
            dataImpl.mRequestedHeight = height;
            dataImpl.InvalidateMeasure();
            if(height >= 0 && !dataImpl.GetParentLayout() && !dataImpl.GetParentView() &&
               !Integration::View::HasLayoutCapability(viewImpl) && viewImpl.GetChildViewCount() == 0)
            {
              viewImpl.Self().SetHeight(height);
            }
          }
        }
        break;
      }

      case Ui::View::Property::MINIMUM_WIDTH:
      {
        float width;
        if(value.Get(width) && IsValidSizeBound(width))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.GetMinimumWidth(), width))
          {
            dataImpl.EnsureSizeConstraints().minWidth = width;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::MINIMUM_HEIGHT:
      {
        float height;
        if(value.Get(height) && IsValidSizeBound(height))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.GetMinimumHeight(), height))
          {
            dataImpl.EnsureSizeConstraints().minHeight = height;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::MAXIMUM_WIDTH:
      {
        float width;
        if(value.Get(width) && IsValidSizeBound(width))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.GetMaximumWidth(), width))
          {
            dataImpl.EnsureSizeConstraints().maxWidth = width;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::MAXIMUM_HEIGHT:
      {
        float height;
        if(value.Get(height) && IsValidSizeBound(height))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(!FloatEqual(dataImpl.GetMaximumHeight(), height))
          {
            dataImpl.EnsureSizeConstraints().maxHeight = height;
            dataImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::LAYOUT_MODE:
      {
        int modeValue;
        if(value.Get(modeValue))
        {
          ViewDataImpl&  dataImpl = viewImpl.GetViewDataImpl();
          Ui::LayoutMode mode     = static_cast<Ui::LayoutMode>(modeValue);
          if(dataImpl.mLayoutMode != mode)
          {
            dataImpl.mLayoutMode = mode;
            dataImpl.InvalidateMeasure();

            // A layout-mode transition (DEFAULT <-> STANDALONE) changes
            // whether this view contributes to the parent's measure/arrange.
            // The parent's cached result is now stale in either direction,
            // so explicitly invalidate the parent. This is needed because
            // the self.InvalidateMeasure() above may no longer propagate
            // (e.g. after transitioning to STANDALONE this view becomes a
            // layout boundary and stops propagation).
            Ui::View parentView = dataImpl.GetParentView();
            if(parentView)
            {
              ViewDataImpl::Get(GetImpl(parentView)).InvalidateMeasure();
            }
          }
        }
        break;
      }

      case Ui::View::Property::FOCUS_GROUP:
      {
        bool isFocusGroup;
        if(value.Get(isFocusGroup))
        {
          viewImpl.GetViewDataImpl().mIsFocusGroup = isFocusGroup;
        }
        break;
      }
    }
  }
}

Property::Value ViewDataImpl::GetProperty(BaseObject* object, Property::Index index)
{
  DALI_ASSERT_ALWAYS(Dali::Adaptor::IsEventThread() && "Must be called from the event thread!");

  Property::Value value;

  Ui::View view = Ui::View::DownCast(BaseHandle(object));

  if(view)
  {
    ViewImpl& viewImpl(GetImpl(view));

    switch(index)
    {
      case Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::leftId);
        break;
      }

      case Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::rightId);
        break;
      }

      case Ui::View::Property::UP_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::upId);
        break;
      }

      case Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::downId);
        break;
      }

      case Ui::View::Property::BACKGROUND:
      {
        Property::Map map;

        if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
        {
          const Ui::Internal::Visual::Base* visualImplPtr =
            viewImpl.GetViewDataImpl().mVisualData->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
          if(visualImplPtr)
          {
            visualImplPtr->CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::View::Property::MARGIN:
      {
        value = ToVector4(viewImpl.GetMargin());
        break;
      }

      case Ui::View::Property::PADDING:
      {
        value = ToVector4(viewImpl.GetPadding());
        break;
      }

      case Ui::View::Property::SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
        {
          Ui::Integration::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::View::Property::DISPATCH_KEY_EVENTS:
      {
        value = viewImpl.GetViewDataImpl().mDispatchKeyEvents;
        break;
      }

      case Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::clockwiseId);
        break;
      }

      case Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::counterClockwiseId);
        break;
      }

      case Ui::View::Property::FORWARD_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::forwardId);
        break;
      }

      case Ui::View::Property::BACKWARD_FOCUSABLE_VIEW_ID:
      {
        value = viewImpl.GetViewDataImpl().GetFocusNavigationId(&FocusNavigationData::backwardId);
        break;
      }

      case Ui::View::Property::OFFSCREEN_RENDERING:
      {
        const auto* renderEffectData = viewImpl.GetViewDataImpl().mRenderEffectData.get();
        value                        = renderEffectData ? renderEffectData->offScreenRenderingType : Ui::View::OffScreenRenderingType::NONE;
        break;
      }

      case Ui::View::Property::INNER_SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
        {
          Ui::Integration::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::INNER_SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::View::Property::BORDERLINE:
      {
        Property::Map map;

        if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
        {
          Ui::Integration::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::BORDERLINE);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::View::Property::REQUESTED_WIDTH:
      {
        value = viewImpl.GetViewDataImpl().mRequestedWidth;
        break;
      }

      case Ui::View::Property::REQUESTED_HEIGHT:
      {
        value = viewImpl.GetViewDataImpl().mRequestedHeight;
        break;
      }

      case Ui::View::Property::MINIMUM_WIDTH:
      {
        value = viewImpl.GetViewDataImpl().GetMinimumWidth();
        break;
      }

      case Ui::View::Property::MINIMUM_HEIGHT:
      {
        value = viewImpl.GetViewDataImpl().GetMinimumHeight();
        break;
      }

      case Ui::View::Property::MAXIMUM_WIDTH:
      {
        value = viewImpl.GetViewDataImpl().GetMaximumWidth();
        break;
      }

      case Ui::View::Property::MAXIMUM_HEIGHT:
      {
        value = viewImpl.GetViewDataImpl().GetMaximumHeight();
        break;
      }

      case Ui::View::Property::LAYOUT_MODE:
      {
        value = static_cast<int>(viewImpl.GetViewDataImpl().mLayoutMode);
        break;
      }

      case Ui::View::Property::FOCUS_GROUP:
      {
        value = viewImpl.GetViewDataImpl().mIsFocusGroup;
        break;
      }

      case Ui::View::Property::CORNER_RADIUS:
      case Ui::View::Property::CORNER_RADIUS_POLICY:
      case Ui::View::Property::CORNER_SQUARENESS:
      case Ui::View::Property::BORDERLINE_WIDTH:
      case Ui::View::Property::BORDERLINE_COLOR:
      case Ui::View::Property::BORDERLINE_OFFSET:
      {
        // Do not return property for animatable custom properties.
        // Actual variables of each property will be registered at custom area.
        break;
      }
    }
  }

  return value;
}

Ui::View::VisualEventSignalType& ViewDataImpl::VisualEventSignal()
{
  DALI_ASSERT_ALWAYS(mVisualData && "Visual Disabled view cannot use VisualEventSignal!!");
  return mVisualData->VisualEventSignal();
}

ViewDataImpl::AccessibilityData& ViewDataImpl::GetOrCreateAccessibilityData()
{
  if(DALI_UNLIKELY(!mAccessibilityData))
  {
    // Create only 1 times.
    mAccessibilityData = std::make_unique<AccessibilityData>(mViewImpl);
  }
  DALI_ASSERT_DEBUG(mAccessibilityData && "AccessibilityData not created!");
  return *mAccessibilityData;
}

ViewDataImpl::AccessibilityData* ViewDataImpl::GetAccessibilityData() const
{
  return mAccessibilityData.get();
}

void ViewDataImpl::SetResolvedAccessibilityName(const Dali::String& name)
{
  const std::string value = ToStdString(name);
  if(DALI_UNLIKELY(!GetAccessibilityData()) && value.empty())
  {
    return;
  }

  auto& data = GetOrCreateAccessibilityData();
  if(data.mAccessibilityProps.name != value)
  {
    data.mAccessibilityProps.name = value;
    data.EmitPropertyChanged(Dali::Devel::Accessibility::ObjectPropertyChangeEvent::NAME);
  }
}

void ViewDataImpl::SetAccessibilityName(StringView name)
{
  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.ClearBinding(mViewImpl.Self(), ACCESSIBILITY_NAME_BINDING_ID);
  }

  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.translatableName.clear();
    data->mAccessibilityProps.nameLanguageSpans.clear();
    data->UpdateLanguageSpanAttribute(true);
  }
  SetResolvedAccessibilityName(ToDaliString(ToStdString(name)));
}

Dali::String ViewDataImpl::GetAccessibilityName() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.name : std::string{});
}

void ViewDataImpl::SetResolvedAccessibilityDescription(const Dali::String& description)
{
  const std::string value = ToStdString(description);
  if(DALI_UNLIKELY(!GetAccessibilityData()) && value.empty())
  {
    return;
  }

  auto& data = GetOrCreateAccessibilityData();
  if(data.mAccessibilityProps.description != value)
  {
    data.mAccessibilityProps.description = value;
    data.EmitPropertyChanged(Dali::Devel::Accessibility::ObjectPropertyChangeEvent::DESCRIPTION);
  }
}

void ViewDataImpl::SetAccessibilityDescription(StringView description)
{
  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.ClearBinding(mViewImpl.Self(), ACCESSIBILITY_DESCRIPTION_BINDING_ID);
  }

  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.translatableDescription.clear();
    data->mAccessibilityProps.descriptionLanguageSpans.clear();
    data->UpdateLanguageSpanAttribute(false);
  }
  SetResolvedAccessibilityDescription(ToDaliString(ToStdString(description)));
}

Dali::String ViewDataImpl::GetAccessibilityDescription() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.description : std::string{});
}

void ViewDataImpl::SetAccessibilityValue(StringView value)
{
  const std::string stringValue = ToStdString(value);
  if(DALI_UNLIKELY(!GetAccessibilityData()) && stringValue.empty())
  {
    return;
  }

  auto& data = GetOrCreateAccessibilityData();
  if(data.mAccessibilityProps.value != stringValue)
  {
    data.mAccessibilityProps.value = stringValue;
    data.EmitPropertyChanged(Dali::Devel::Accessibility::ObjectPropertyChangeEvent::VALUE);
  }
}

Dali::String ViewDataImpl::GetAccessibilityValue() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.value : std::string{});
}

void ViewDataImpl::SetAccessibilityRole(Accessibility::Role role)
{
  if(IsValidAccessibilityRole(role))
  {
    mAccessibilityRole = static_cast<int32_t>(role);
  }
}

Accessibility::Role ViewDataImpl::GetAccessibilityRole() const
{
  return static_cast<Accessibility::Role>(mAccessibilityRole);
}

void ViewDataImpl::SetAccessibilityHidden(bool hidden)
{
  const auto* data           = GetAccessibilityData();
  const bool  originalHidden = DALI_LIKELY(data) ? data->mAccessibilityProps.isHidden : false;
  if(originalHidden == hidden)
  {
    return;
  }

  GetOrCreateAccessibilityData().mAccessibilityProps.isHidden = hidden;
  auto accessible                                             = GetOrCreateAccessibilityData().GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    auto* parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
    if(parent)
    {
      parent->OnChildrenChanged();
    }
  }
}

bool ViewDataImpl::IsAccessibilityHidden() const
{
  const auto* data = GetAccessibilityData();
  return DALI_LIKELY(data) && data->mAccessibilityProps.isHidden;
}

void ViewDataImpl::SetAccessibilityHighlightable(bool highlightable)
{
  GetOrCreateAccessibilityData().mAccessibilityProps.isHighlightable =
    highlightable ? TriStateProperty::TRUE : TriStateProperty::FALSE;
}

void ViewDataImpl::ResetAccessibilityHighlightable()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.isHighlightable = TriStateProperty::AUTO;
  }
}

bool ViewDataImpl::IsAccessibilityHighlightable() const
{
  const auto* data  = GetAccessibilityData();
  const auto  value = DALI_LIKELY(data) ? data->mAccessibilityProps.isHighlightable : TriStateProperty::AUTO;
  if(value == TriStateProperty::TRUE)
  {
    return true;
  }
  if(value == TriStateProperty::FALSE)
  {
    return false;
  }
  return GetAccessibilityRole() != Accessibility::Role::NONE;
}

void ViewDataImpl::SetAccessibilityScrollable(bool scrollable)
{
  if(DALI_LIKELY(GetAccessibilityData()) || scrollable)
  {
    GetOrCreateAccessibilityData().mAccessibilityProps.isScrollable = scrollable;
  }
}

bool ViewDataImpl::IsAccessibilityScrollable() const
{
  const auto* data = GetAccessibilityData();
  return DALI_LIKELY(data) && data->mAccessibilityProps.isScrollable;
}

void ViewDataImpl::SetAccessibilityModal(bool modal)
{
  if(DALI_LIKELY(GetAccessibilityData()) || modal)
  {
    GetOrCreateAccessibilityData().mAccessibilityProps.isModal = modal;
  }
}

bool ViewDataImpl::IsAccessibilityModal() const
{
  const auto* data = GetAccessibilityData();
  return DALI_LIKELY(data) && data->mAccessibilityProps.isModal;
}

void ViewDataImpl::SetAutomationId(StringView automationId)
{
  const std::string value = ToStdString(automationId);
  if(DALI_LIKELY(GetAccessibilityData()) || !value.empty())
  {
    GetOrCreateAccessibilityData().mAccessibilityProps.automationId = value;
  }
}

Dali::String ViewDataImpl::GetAutomationId() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.automationId : std::string{});
}

void ViewDataImpl::ApplyLocalizedAccessibilityName(BaseHandle, const Dali::String& name)
{
  SetResolvedAccessibilityName(name);
}

void ViewDataImpl::SetTranslatableAccessibilityName(StringView resourceId, StringView domain)
{
  if(resourceId.Empty())
  {
    ClearTranslatableAccessibilityName();
    return;
  }

  auto& data                                = GetOrCreateAccessibilityData();
  data.mAccessibilityProps.translatableName = ToStdString(resourceId);
  data.mAccessibilityProps.nameLanguageSpans.clear();
  data.UpdateLanguageSpanAttribute(true);

  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.SetBindingResource(mViewImpl.Self(),
                               ACCESSIBILITY_NAME_BINDING_ID,
                               resourceId,
                               domain,
                               LocalizedStringCallback::New(this, &ViewDataImpl::ApplyLocalizedAccessibilityName));
  }
  else
  {
    SetResolvedAccessibilityName(ToDaliString(ToStdString(resourceId)));
  }
}

Dali::String ViewDataImpl::GetTranslatableAccessibilityName() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.translatableName : std::string{});
}

void ViewDataImpl::ClearTranslatableAccessibilityName()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.translatableName.clear();
  }
  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.ClearBinding(mViewImpl.Self(), ACCESSIBILITY_NAME_BINDING_ID);
  }
}

void ViewDataImpl::ApplyLocalizedAccessibilityDescription(BaseHandle, const Dali::String& description)
{
  SetResolvedAccessibilityDescription(description);
}

void ViewDataImpl::SetTranslatableAccessibilityDescription(StringView resourceId, StringView domain)
{
  if(resourceId.Empty())
  {
    ClearTranslatableAccessibilityDescription();
    return;
  }

  auto& data                                       = GetOrCreateAccessibilityData();
  data.mAccessibilityProps.translatableDescription = ToStdString(resourceId);
  data.mAccessibilityProps.descriptionLanguageSpans.clear();
  data.UpdateLanguageSpanAttribute(false);

  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.SetBindingResource(mViewImpl.Self(),
                               ACCESSIBILITY_DESCRIPTION_BINDING_ID,
                               resourceId,
                               domain,
                               LocalizedStringCallback::New(this, &ViewDataImpl::ApplyLocalizedAccessibilityDescription));
  }
  else
  {
    SetResolvedAccessibilityDescription(ToDaliString(ToStdString(resourceId)));
  }
}

Dali::String ViewDataImpl::GetTranslatableAccessibilityDescription() const
{
  const auto* data = GetAccessibilityData();
  return ToDaliString(DALI_LIKELY(data) ? data->mAccessibilityProps.translatableDescription : std::string{});
}

void ViewDataImpl::ClearTranslatableAccessibilityDescription()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.translatableDescription.clear();
  }
  auto manager = UiLocalizationManager::Get();
  if(manager)
  {
    manager.ClearBinding(mViewImpl.Self(), ACCESSIBILITY_DESCRIPTION_BINDING_ID);
  }
}

void ViewDataImpl::AddAccessibilityRelation(Accessibility::RelationType type, View target)
{
  if(!IsValidAccessibilityRelation(type) || !target)
  {
    return;
  }

  auto& targets = GetOrCreateAccessibilityData().mAccessibilityProps.relations[type];
  for(auto iterator = targets.begin(); iterator != targets.end();)
  {
    View current = iterator->GetHandle();
    if(!current)
    {
      iterator = targets.erase(iterator);
    }
    else
    {
      if(current == target)
      {
        return;
      }
      ++iterator;
    }
  }
  targets.emplace_back(target);
}

void ViewDataImpl::RemoveAccessibilityRelation(Accessibility::RelationType type, View target)
{
  auto* data = GetAccessibilityData();
  if(DALI_UNLIKELY(!data) || !IsValidAccessibilityRelation(type) || !target)
  {
    return;
  }

  auto relation = data->mAccessibilityProps.relations.find(type);
  if(relation == data->mAccessibilityProps.relations.end())
  {
    return;
  }
  auto& targets = relation->second;
  targets.erase(std::remove_if(targets.begin(), targets.end(), [&target](const WeakHandle<View>& weakTarget)
  {
    const View current = weakTarget.GetHandle();
    return !current || current == target;
  }),
                targets.end());
  if(targets.empty())
  {
    data->mAccessibilityProps.relations.erase(relation);
  }
}

void ViewDataImpl::ClearAccessibilityRelations()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.relations.clear();
  }
}

bool ViewDataImpl::HasAccessibilityRelation(Accessibility::RelationType type, View target) const
{
  const auto* data = GetAccessibilityData();
  if(DALI_UNLIKELY(!data) || !IsValidAccessibilityRelation(type) || !target)
  {
    return false;
  }
  auto relation = data->mAccessibilityProps.relations.find(type);
  if(relation == data->mAccessibilityProps.relations.end())
  {
    return false;
  }
  return std::any_of(relation->second.begin(), relation->second.end(), [&target](const WeakHandle<View>& weakTarget)
  {
    return weakTarget.GetHandle() == target;
  });
}

void ViewDataImpl::AddAccessibilityReadingInfo(Accessibility::ReadingInfo info)
{
  if(!IsValidAccessibilityReadingInfo(info))
  {
    return;
  }
  auto types                                = GetAccessibilityReadingInfoType();
  types[ToIntegrationReadingInfoType(info)] = true;
  SetAccessibilityReadingInfoType(types);
}

void ViewDataImpl::RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo info)
{
  if(!IsValidAccessibilityReadingInfo(info))
  {
    return;
  }
  auto types                                = GetAccessibilityReadingInfoType();
  types[ToIntegrationReadingInfoType(info)] = false;
  SetAccessibilityReadingInfoType(types);
}

void ViewDataImpl::ClearAccessibilityReadingInfo()
{
  SetAccessibilityReadingInfoType({});
}

bool ViewDataImpl::HasAccessibilityReadingInfo(Accessibility::ReadingInfo info) const
{
  return IsValidAccessibilityReadingInfo(info) && GetAccessibilityReadingInfoType()[ToIntegrationReadingInfoType(info)];
}

bool ViewDataImpl::AddAccessibilityNameLanguageSpan(uint32_t start, uint32_t length, StringView locale)
{
  const std::string localeValue = ToStdString(locale);
  auto*             data        = GetAccessibilityData();
  if(DALI_UNLIKELY(!data) || length == 0u || localeValue.empty() || start > std::numeric_limits<uint32_t>::max() - length)
  {
    return false;
  }

  const uint32_t end = start + length;
  if(end > Text::Utf8ToUtf32Length(ToDaliString(data->mAccessibilityProps.name)))
  {
    return false;
  }
  auto& spans = data->mAccessibilityProps.nameLanguageSpans;
  if(std::any_of(spans.begin(), spans.end(), [start, end](const auto& span)
  {
    return start < span.start + span.length && span.start < end;
  }))
  {
    return false;
  }
  spans.push_back({start, length, localeValue});
  std::sort(spans.begin(), spans.end(), [](const auto& lhs, const auto& rhs)
  { return lhs.start < rhs.start; });
  data->UpdateLanguageSpanAttribute(true);
  return true;
}

void ViewDataImpl::ClearAccessibilityNameLanguageSpans()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.nameLanguageSpans.clear();
    data->UpdateLanguageSpanAttribute(true);
  }
}

bool ViewDataImpl::AddAccessibilityDescriptionLanguageSpan(uint32_t start, uint32_t length, StringView locale)
{
  const std::string localeValue = ToStdString(locale);
  auto*             data        = GetAccessibilityData();
  if(DALI_UNLIKELY(!data) || length == 0u || localeValue.empty() || start > std::numeric_limits<uint32_t>::max() - length)
  {
    return false;
  }

  const uint32_t end = start + length;
  if(end > Text::Utf8ToUtf32Length(ToDaliString(data->mAccessibilityProps.description)))
  {
    return false;
  }
  auto& spans = data->mAccessibilityProps.descriptionLanguageSpans;
  if(std::any_of(spans.begin(), spans.end(), [start, end](const auto& span)
  {
    return start < span.start + span.length && span.start < end;
  }))
  {
    return false;
  }
  spans.push_back({start, length, localeValue});
  std::sort(spans.begin(), spans.end(), [](const auto& lhs, const auto& rhs)
  { return lhs.start < rhs.start; });
  data->UpdateLanguageSpanAttribute(false);
  return true;
}

void ViewDataImpl::ClearAccessibilityDescriptionLanguageSpans()
{
  if(auto* data = GetAccessibilityData())
  {
    data->mAccessibilityProps.descriptionLanguageSpans.clear();
    data->UpdateLanguageSpanAttribute(false);
  }
}

void ViewDataImpl::SetRequestInitialAccessibilityHighlight(bool request)
{
  if(request)
  {
    AppendAccessibilityAttribute(INITIAL_HIGHLIGHT_ATTRIBUTE, "true");
  }
  else
  {
    RemoveAccessibilityAttribute(INITIAL_HIGHLIGHT_ATTRIBUTE);
  }
}

bool ViewDataImpl::IsInitialAccessibilityHighlightRequested() const
{
  const auto* data = GetAccessibilityData();
  return DALI_LIKELY(data) && GetBooleanAttribute(data->mAccessibilityProps.extraAttributes, INITIAL_HIGHLIGHT_ATTRIBUTE);
}

void ViewDataImpl::SetAccessibilityCollectionContainer(bool container)
{
  if(container)
  {
    AppendAccessibilityAttribute(COLLECTION_CONTAINER_ATTRIBUTE, "true");
  }
  else
  {
    RemoveAccessibilityAttribute(COLLECTION_CONTAINER_ATTRIBUTE);
  }
}

bool ViewDataImpl::IsAccessibilityCollectionContainer() const
{
  const auto* data = GetAccessibilityData();
  return DALI_LIKELY(data) && GetBooleanAttribute(data->mAccessibilityProps.extraAttributes, COLLECTION_CONTAINER_ATTRIBUTE);
}

void ViewDataImpl::SetAccessibilityCollectionIndex(int32_t index)
{
  if(index < 0)
  {
    ClearAccessibilityCollectionIndex();
  }
  else
  {
    AppendAccessibilityAttribute(COLLECTION_INDEX_ATTRIBUTE, ToDaliString(std::to_string(index)));
  }
}

int32_t ViewDataImpl::GetAccessibilityCollectionIndex() const
{
  const auto* data = GetAccessibilityData();
  std::string value;
  if(DALI_UNLIKELY(!data) || !GetStringAttribute(data->mAccessibilityProps.extraAttributes, COLLECTION_INDEX_ATTRIBUTE, value))
  {
    return -1;
  }
  char* parsedEnd = nullptr;
  long  parsed    = std::strtol(value.c_str(), &parsedEnd, 10);
  if(parsedEnd != value.c_str() + value.size() || parsed < 0 || parsed > std::numeric_limits<int32_t>::max())
  {
    return -1;
  }
  return static_cast<int32_t>(parsed);
}

void ViewDataImpl::ClearAccessibilityCollectionIndex()
{
  RemoveAccessibilityAttribute(COLLECTION_INDEX_ATTRIBUTE);
}

View::AccessibilityReadingStatusChangedSignalType& ViewDataImpl::AccessibilityReadingStatusChangedSignal()
{
  return GetOrCreateAccessibilityData().mAccessibilityReadingStatusChangedSignal;
}

View::AccessibilityHighlightedSignalType& ViewDataImpl::AccessibilityHighlightedSignal()
{
  return GetOrCreateAccessibilityData().mAccessibilityHighlightedSignal;
}

void ViewDataImpl::AppendAccessibilityAttribute(const Dali::String& key, const Dali::String& value)
{
  GetOrCreateAccessibilityData().AppendAccessibilityAttribute(key, value);
}

void ViewDataImpl::RemoveAccessibilityAttribute(const Dali::String& key)
{
  auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->RemoveAccessibilityAttribute(key);
  }
}

void ViewDataImpl::ClearAccessibilityAttributes()
{
  auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->ClearAccessibilityAttributes();
  }
}

void ViewDataImpl::SetAccessibilityReadingInfoType(const Dali::Integration::Accessibility::ReadingInfoTypes types)
{
  GetOrCreateAccessibilityData().SetAccessibilityReadingInfoType(types);
}

Dali::Integration::Accessibility::ReadingInfoTypes ViewDataImpl::GetAccessibilityReadingInfoType() const
{
  const auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    return accessibilityData->GetAccessibilityReadingInfoType();
  }
  else
  {
    // Return default ReadingInfoTypes
    return AccessibilityData::GetDefaultReadingInfoTypes();
  }
}

bool ViewDataImpl::IsAccessibleCreated() const
{
  auto bridge = Dali::Integration::Accessibility::Bridge::GetCurrentBridge(); // LCOV_EXCL_LINE
  return DALI_LIKELY(bridge) ? !!bridge->GetAccessible(mViewImpl.Self()) : false;
}

void ViewDataImpl::SetAccessibilityStates(uint32_t states)
{
  const auto defaultStates = AccessibilityData::GetDefaultViewAccessibilityStates();
  if(DALI_LIKELY(GetAccessibilityData()) || states != defaultStates)
  {
    GetOrCreateAccessibilityData().mAccessibilityProps.states = states;
  }

  auto accessible = GetAccessibleObject();
  if(DALI_LIKELY(accessible))
  {
    accessible->OnStatePropertySet(states);
  }
}

uint32_t ViewDataImpl::GetAccessibilityStates() const
{
  const auto* accessibilityData = GetAccessibilityData();
  return DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.states : AccessibilityData::GetDefaultViewAccessibilityStates();
}

void ViewDataImpl::AddAccessibilityState(Accessibility::State state)
{
  if(static_cast<uint32_t>(state) < static_cast<uint32_t>(Accessibility::State::MAX_COUNT))
  {
    SetAccessibilityStates(GetAccessibilityStates() | ViewAccessibilityStateToMask(state));
  }
}

void ViewDataImpl::RemoveAccessibilityState(Accessibility::State state)
{
  if(static_cast<uint32_t>(state) < static_cast<uint32_t>(Accessibility::State::MAX_COUNT))
  {
    SetAccessibilityStates(GetAccessibilityStates() & ~ViewAccessibilityStateToMask(state));
  }
}

void ViewDataImpl::ClearAccessibilityStates()
{
  SetAccessibilityStates(0u);
}

bool ViewDataImpl::HasAccessibilityState(Accessibility::State state) const
{
  return static_cast<uint32_t>(state) < static_cast<uint32_t>(Accessibility::State::MAX_COUNT) &&
         (GetAccessibilityStates() & ViewAccessibilityStateToMask(state)) != 0u;
}

void ViewDataImpl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool ViewDataImpl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
}

void ViewDataImpl::SetAccessibleObjectCreator(AccessibleObjectCreator creator)
{
  mAccessibleObjectCreator = creator;
}

void ViewDataImpl::EmitAccessibilityStateChanged(Dali::Integration::Accessibility::State state, int newValue)
{
  Dali::CustomActor handle(mViewImpl.GetOwner());
  auto              bridge = Dali::Integration::Accessibility::Bridge::GetCurrentBridge(); // LCOV_EXCL_LINE
  if(DALI_LIKELY(bridge))
  {
    if(state == Dali::Integration::Accessibility::State::SHOWING) // LCOV_EXCL_LINE
    {
      bool isModal = ViewAccessible::IsModal(handle);
      if(isModal)
      {
        if(newValue == 1)
        {
          bridge->RegisterDefaultLabel(handle);
        }
        else
        {
          bridge->UnregisterDefaultLabel(handle);
        }
      }
    }
  }

  if(bridge && bridge->IsUp())
  {
    auto accessible = dynamic_cast<Dali::Accessibility::ActorAccessible*>(Dali::Accessibility::Accessible::Get(handle)); // LCOV_EXCL_LINE
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitStateChanged(state, newValue, 0);
    }
  }
}

void ViewDataImpl::ApplyFittingMode(const Vector2& size, bool isLayoutFinishedUpdate)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ApplyFittingMode(size, isLayoutFinishedUpdate);
  }
}

void ViewDataImpl::EnsureFittingModeLayoutFinishedSignalConnected()
{
  if(!mFittingModeLayoutFinishedSignalConnected)
  {
    Ui::View handle = Ui::View::DownCast(mViewImpl.Self());
    if(handle)
    {
      handle.LayoutFinishedSignal().Connect(this, &ViewDataImpl::OnLayoutFinished);
      mFittingModeLayoutFinishedSignalConnected = true;
    }
  }
}

void ViewDataImpl::OnLayoutFinished(Ui::View view, LayoutRect bounds)
{
  ApplyFittingMode(Vector2(bounds.width, bounds.height), true);
}

void ViewDataImpl::SetBackground(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Integration::Visual::Base visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
    visual.SetName("background");

    if(visual)
    {
      // Ignore corner radius for offscreen case.
      Ui::GetImplementation(visual).CornerRadiusIgnoredAtOffscreenRendering(true);
      mVisualData->RegisterVisual(Ui::View::Property::BACKGROUND, visual, Dali::Ui::Integration::DepthIndex::BACKGROUND);
      EnableCornerPropertiesOverridden(visual, true);

      if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
      {
        sizeNegotiatedViewImpl->RelayoutRequest();
      }
    }
  }
}

void ViewDataImpl::SetShadow(const Property::Map& map)
{
  ClearShadow();
  SetFirstShadow(map);
}

void ViewDataImpl::SetFirstShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Integration::Visual::Base visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
    visual.SetName("shadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::SHADOW, visual, Dali::Ui::Integration::DepthIndex::BACKGROUND_EFFECT);
      EnableCornerPropertiesOverridden(visual, true);

      if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
      {
        sizeNegotiatedViewImpl->RelayoutRequest();
      }
    }
  }
}

void ViewDataImpl::AppendShadow(const Dali::Ui::Shadow& shadow)
{
  if(!GetVisualImplPtr(Ui::View::Property::SHADOW))
  {
    // Keep the first shadow as the View::Property::SHADOW visual. That makes
    // property lookup and shadow blur/opacity animations target the primary
    // shadow directly, while later shadows can live in the visual container.
    SetFirstShadow(Extension::Shadow::CreatePropertyMap(shadow));
    return;
  }

  ColorVisual visual = Extension::Shadow::CreateVisual(shadow);
  visual.SetName("shadow");
  if(AddShadowVisualObject(visual, Ui::Integration::Visual::InternalContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND))
  {
    if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
    {
      sizeNegotiatedViewImpl->RelayoutRequest();
    }
  }
}

void ViewDataImpl::ClearShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::SHADOW);
    mVisualData->RemoveBoxShadowVisualObjects();
  }

  if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
  {
    sizeNegotiatedViewImpl->RelayoutRequest();
  }
}

void ViewDataImpl::SetInnerShadow(const Property::Map& map)
{
  RegisterInnerShadowVisual(Ui::Integration::VisualFactory::Get().CreateVisual(map));
}

void ViewDataImpl::SetInnerShadow(const Ui::InnerShadow& innerShadow)
{
  if(innerShadow == Ui::InnerShadow::None())
  {
    ClearInnerShadow();
    return;
  }

  SetInnerShadow(Dali::Ui::Internal::InnerShadow::CreatePropertyMap(innerShadow));
}

void ViewDataImpl::RegisterInnerShadowVisual(Ui::Integration::Visual::Base visual)
{
  if(DALI_LIKELY(mVisualData))
  {
    visual.SetName("innerShadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::INNER_SHADOW, visual, INNER_SHADOW_DEPTH_INDEX);

      Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

      auto visualCornerRadiusProperty = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::CORNER_RADIUS, false);
      auto visualBorderlineProperty   = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::BORDERLINE_WIDTH);

      if(DALI_LIKELY(visualCornerRadiusProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualCornerRadiusProperty.object) &&
         DALI_LIKELY(visualBorderlineProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualBorderlineProperty.object))
      {
        Dali::CustomActor handle(mViewImpl.GetOwner());

        auto innerShadowCornerRadiusConstraint =
          Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex,
                                   InnerShadowCornerRadiusConstraint);
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::CORNER_RADIUS));
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::CORNER_RADIUS_POLICY));
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Dali::Actor::Property::SIZE));
        innerShadowCornerRadiusConstraint.AddSource(LocalSource(Dali::VisualRenderer::Property::EXTRA_SIZE));
        innerShadowCornerRadiusConstraint.AddSource(
          LocalSource(Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH));

        Dali::Integration::ConstraintSetInternalTag(innerShadowCornerRadiusConstraint,
                                                    INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG);

        EnableCornerPropertiesOverridden(visual, true, innerShadowCornerRadiusConstraint);
      }

      if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
      {
        sizeNegotiatedViewImpl->RelayoutRequest();
      }
    }
  }
}

void ViewDataImpl::ClearInnerShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::INNER_SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
    {
      sizeNegotiatedViewImpl->RelayoutRequest();
    }
  }
}

void ViewDataImpl::SetBorderline(const Property::Map& map, bool forciblyCreate)
{
  if(DALI_LIKELY(mVisualData))
  {
    if(!forciblyCreate)
    {
      Ui::Internal::Visual::Base* previousVisualImplPtr =
        mVisualData->GetVisualImplPtr(Ui::View::Property::BORDERLINE);
      if(previousVisualImplPtr)
      {
        previousVisualImplPtr->DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, map);

        // Trigger borderline relative constraints once
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_WIDTH, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_COLOR, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_OFFSET, false);
        return;
      }
    }
    Ui::Integration::Visual::Base visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
    visual.SetName("borderline");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::BORDERLINE, visual, BORDERLINE_DEPTH_INDEX);

      // Create constraint only if we set Borderline property as DevelView::BORDERLINE_XXX.
      if(!forciblyCreate)
      {
        Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

        auto visualCornerRadiusProperty    = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::CORNER_RADIUS, false);
        auto visualBorderlineWidthProperty = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::BORDERLINE_WIDTH);

        if(DALI_LIKELY(visualCornerRadiusProperty.propertyIndex != Property::INVALID_INDEX &&
                       visualCornerRadiusProperty.object) &&
           DALI_LIKELY(visualBorderlineWidthProperty.propertyIndex != Property::INVALID_INDEX &&
                       visualBorderlineWidthProperty.object))
        {
          Dali::CustomActor handle(mViewImpl.GetOwner());

          auto borderlineCornerRadiusConstraint =
            Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex,
                                     BorderlineCornerRadiusConstraint);
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::CORNER_RADIUS));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::CORNER_RADIUS_POLICY));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Dali::Actor::Property::SIZE));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_WIDTH));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_OFFSET));

          Dali::Integration::ConstraintSetInternalTag(borderlineCornerRadiusConstraint,
                                                      BORDERLINE_CORNER_RADIUS_CONSTRAINT_TAG);

          auto visualBorderlineColorProperty  = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::BORDERLINE_COLOR);
          auto visualBorderlineOffsetProperty = visualImpl.GetPropertyObject(Dali::Ui::Integration::Visual::Property::BORDERLINE_OFFSET);

          if(DALI_LIKELY(visualBorderlineColorProperty.propertyIndex != Property::INVALID_INDEX &&
                         visualBorderlineColorProperty.object) &&
             DALI_LIKELY(visualBorderlineOffsetProperty.propertyIndex != Property::INVALID_INDEX &&
                         visualBorderlineOffsetProperty.object))
          {
            auto borderlineWidthConstraint = Constraint::New<float>(
              visualBorderlineWidthProperty.object, visualBorderlineWidthProperty.propertyIndex, Dali::EqualToConstraint());
            borderlineWidthConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_WIDTH));
            auto borderlineColorConstraint = Constraint::New<Vector4>(
              visualBorderlineColorProperty.object, visualBorderlineColorProperty.propertyIndex, Dali::EqualToConstraint());
            borderlineColorConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_COLOR));
            auto borderlineOffsetConstraint =
              Constraint::New<float>(visualBorderlineOffsetProperty.object,
                                     visualBorderlineOffsetProperty.propertyIndex, Dali::EqualToConstraint());
            borderlineOffsetConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_OFFSET));

            Dali::Integration::ConstraintSetInternalTag(borderlineWidthConstraint, BORDERLINE_WIDTH_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineColorConstraint, BORDERLINE_COLOR_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineOffsetConstraint, BORDERLINE_OFFSET_CONSTRAINT_TAG);

            borderlineWidthConstraint.Apply();
            borderlineColorConstraint.Apply();
            borderlineOffsetConstraint.Apply();

            visualImpl.AddConstraintFeature(borderlineWidthConstraint,
                                            {Ui::View::Property::BORDERLINE_WIDTH, VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX});
            visualImpl.AddConstraintFeature(borderlineColorConstraint, {Ui::View::Property::BORDERLINE_COLOR});
            visualImpl.AddConstraintFeature(borderlineOffsetConstraint, {Ui::View::Property::BORDERLINE_OFFSET});
          }

          EnableCornerPropertiesOverridden(visual, true, borderlineCornerRadiusConstraint);
        }
      }

      if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
      {
        sizeNegotiatedViewImpl->RelayoutRequest();
      }
    }
  }
}

void ViewDataImpl::ClearBorderline()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::BORDERLINE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
    {
      sizeNegotiatedViewImpl->RelayoutRequest();
    }
  }
}

Dali::Property ViewDataImpl::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualProperty(index, visualPropertyKey);
  }
  Dali::Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void ViewDataImpl::EmitResourceReadySignal()
{
  if(!mResourceReadyData || !Dali::Adaptor::IsAvailable()) ///< Avoid resource ready callback during shutting down
  {
    return;
  }

  ResourceReadyData& resourceReadyData = *mResourceReadyData;
  if(!resourceReadyData.isEmittingResourceReadySignal)
  {
    // Guard against calls to emit the signal during the callback
    resourceReadyData.isEmittingResourceReadySignal = true;

    // If the signal handler changes visual, it may become ready during this call & therefore this method will
    // get called again recursively. If so, idleCallbackRegistered is set below, and we act on it after that
    // secondary invocation has completed by notifying in an Idle callback to prevent further recursion.
    Dali::Ui::View handle(mViewImpl.GetOwner());
    resourceReadyData.resourceReadySignal.Emit(handle);

    resourceReadyData.isEmittingResourceReadySignal = false;
  }
  else if(!resourceReadyData.idleCallbackRegistered)
  {
    resourceReadyData.idleCallbackRegistered = true;

    // Add idler to emit the signal again
    if(!resourceReadyData.idleCallback)
    {
      // The callback manager takes the ownership of the callback object.
      resourceReadyData.idleCallback = MakeCallback(this, &ViewDataImpl::OnIdleCallback);
      if(DALI_UNLIKELY(!Adaptor::Get().AddIdle(resourceReadyData.idleCallback, true)))
      {
        DALI_LOG_ERROR("Fail to add idle callback for view resource ready. Skip this callback.\n");
        resourceReadyData.idleCallback           = nullptr;
        resourceReadyData.idleCallbackRegistered = false;
      }
    }
  }
}

bool ViewDataImpl::OnIdleCallback()
{
  if(!mResourceReadyData)
  {
    return false;
  }

  ResourceReadyData& resourceReadyData = *mResourceReadyData;

  // Reset the flag
  resourceReadyData.idleCallbackRegistered = false;

  // A visual is ready so view may need relayouting if staged
  if(mViewImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    if(Integration::SizeNegotiatedViewImpl* sizeNegotiatedViewImpl = dynamic_cast<Integration::SizeNegotiatedViewImpl*>(&mViewImpl))
    {
      sizeNegotiatedViewImpl->RelayoutRequest();
    }
  }

  EmitResourceReadySignal();

  if(!resourceReadyData.idleCallbackRegistered)
  {
    // Set the pointer to null as the callback manager deletes the callback after execute it.
    resourceReadyData.idleCallback = nullptr;
  }

  // Repeat idle if idleCallbackRegistered becomes true one more time.
  return resourceReadyData.idleCallbackRegistered;
}

SharedPtr<Ui::ViewAccessible> ViewDataImpl::GetAccessibleObject()
{
  return GetOrCreateAccessibilityData().GetAccessibleObject();
}

void ViewDataImpl::NotifyAccessibilityActiveDescendantChanged(View descendant)
{
  if(!descendant || !Dali::Integration::Accessibility::IsUp())
  {
    return;
  }

  auto source = GetAccessibleObject();
  auto target = ViewDataImpl::Get(Ui::GetImpl(descendant)).GetAccessibleObject();
  if(source && target)
  {
    source->EmitActiveDescendantChanged(target.Get());
  }
}

Dali::Vector<Dali::Devel::Accessibility::Relation> ViewDataImpl::GetAccessibilityRelations()
{
  Dali::Vector<Dali::Devel::Accessibility::Relation> result;

  const auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for(const auto& relation : relations)
    {
      Dali::Devel::Accessibility::Relation rel{ToIntegrationRelationType(relation.first), {}}; // LCOV_EXCL_LINE
      for(const auto& weakTarget : relation.second)
      {
        View target = weakTarget.GetHandle();
        if(target)
        {
          if(auto* targetAccessible = Dali::Accessibility::Accessible::Get(target))
          {
            rel.mTargets.push_back(targetAccessible);
          }
        }
      }
      if(!rel.mTargets.empty())
      {
        result.PushBack(std::move(rel));
      }
    }
  }

  return result;
}

void ViewDataImpl::RegisterProcessorOnce()
{
  if(DALI_LIKELY(mVisualData))
  {
    if(!mProcessorRegistered)
    {
      Adaptor::Get().RegisterProcessorOnce(*this, true);
      mProcessorRegistered = true;
    }
  }
}

void ViewDataImpl::SizeOrUiScaleChanged()
{
  // Apply fitting mode at post process.r
  RegisterProcessorOnce();

  RefreshRenderEffects();
}

void ViewDataImpl::RefreshRenderEffects()
{
  if(mRenderEffectData && mRenderEffectData->renderEffect)
  {
    mRenderEffectData->renderEffect->Refresh();
  }

  if(mRenderEffectData && mRenderEffectData->offScreenRendering)
  {
    mRenderEffectData->offScreenRendering->Refresh();
  }
}

void ViewDataImpl::SetOffScreenRendering(int32_t offScreenRenderingType)
{
  // Validate input
  {
    constexpr int32_t count = static_cast<int32_t>(OFF_SCREEN_RENDERING_TYPE_COUNT);
    if(0 > offScreenRenderingType || offScreenRenderingType >= count)
    {
      DALI_LOG_ERROR("Failed to set offscreen rendering. Type index is out of bound.\n");
      return;
    }
  }

  Ui::View::OffScreenRenderingType newType =
    static_cast<Ui::View::OffScreenRenderingType>(offScreenRenderingType);

  Dali::Ui::View handle(mViewImpl.GetOwner());

  if(newType == Ui::View::OffScreenRenderingType::NONE)
  {
    if(mRenderEffectData && mRenderEffectData->offScreenRendering)
    {
      auto tempOffscreenRenderingImpl = std::move(mRenderEffectData->offScreenRendering);
      tempOffscreenRenderingImpl->ClearOwnerView();

      if(DALI_LIKELY(mVisualData))
      {
        mVisualData->OffscreenRenderingEnabled(false);
      }
    }
  }
  else
  {
    RenderEffectData& renderEffectData = EnsureRenderEffectData();
    if(renderEffectData.offScreenRenderingType == Ui::View::OffScreenRenderingType::NONE)
    {
      renderEffectData.offScreenRendering = std::make_unique<OffScreenRenderingImpl>(newType);
      renderEffectData.offScreenRendering->SetOwnerView(handle);

      if(DALI_LIKELY(mVisualData))
      {
        mVisualData->OffscreenRenderingEnabled(true);
      }
    }
    else if(renderEffectData.offScreenRenderingType != newType)
    {
      renderEffectData.offScreenRendering->SetType(newType);
    }
    renderEffectData.offScreenRenderingType = newType;
    return;
  }

  if(mRenderEffectData)
  {
    mRenderEffectData->offScreenRenderingType = newType;
  }
}

void ViewDataImpl::UpdateCornerRadius()
{
  if(mRenderEffectData && (mRenderEffectData->renderEffect || mRenderEffectData->offScreenRendering))
  {
    Actor     self   = mViewImpl.Self();
    const int policy = self.GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY);

    Vector4 cornerRadius = self.GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);

    Property::Map map;
    map.Insert(Ui::Integration::Visual::Property::CORNER_RADIUS, cornerRadius);
    map.Insert(Ui::Integration::Visual::Property::CORNER_RADIUS_POLICY, policy);
    map.Insert(Ui::Integration::Visual::Property::CORNER_SQUARENESS,
               self.GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS));

    if(mRenderEffectData->renderEffect)
    {
      mRenderEffectData->renderEffect->SetCornerConstants(map);
    }

    if(mRenderEffectData->offScreenRendering)
    {
      mRenderEffectData->offScreenRendering->SetCornerConstants(map);
    }
  }
}

void ViewDataImpl::UpdateBorderline()
{
  Actor self = mViewImpl.Self();

  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::Integration::InternalVisualType::COLOR);
  map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, Color::TRANSPARENT);
  // Scale natural-pixel width to visual pixels for the initial visual creation.
  map.Insert(Ui::Integration::Visual::Property::BORDERLINE_WIDTH,
             self.GetProperty<float>(Ui::View::Property::BORDERLINE_WIDTH));
  map.Insert(Ui::Integration::Visual::Property::BORDERLINE_COLOR,
             self.GetProperty<Vector4>(Ui::View::Property::BORDERLINE_COLOR));
  map.Insert(Ui::Integration::Visual::Property::BORDERLINE_OFFSET,
             self.GetProperty<float>(Ui::View::Property::BORDERLINE_OFFSET));

  SetBorderline(map, false);
}

void ViewDataImpl::CreateAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    if(index == Ui::View::Property::BORDERLINE_WIDTH || index == Ui::View::Property::BORDERLINE_COLOR ||
       index == Ui::View::Property::BORDERLINE_OFFSET)
    {
      Ui::Internal::Visual::Base* previousVisualImplPtr =
        mVisualData->GetVisualImplPtr(Ui::View::Property::BORDERLINE);
      if(!previousVisualImplPtr)
      {
        // Create visual and constraint for borderline first.
        UpdateBorderline();
      }
    }
    mVisualData->CreateAnimationConstraints(animationObject, index);
  }
}

void ViewDataImpl::ClearAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ClearAnimationConstraints(animationObject, index);
  }
}

void ViewDataImpl::Process(bool postProcessor)
{
  // Consume the registration BEFORE doing the work, never after.
  //
  // RegisterProcessorOnce() is guarded by this very flag, so as long as it stays
  // set the guard swallows every new request. ApplyFittingMode() runs arbitrary
  // visual code and can itself raise one (a fitting apply that resizes a visual
  // whose readiness or size feeds back into this view), and with the flag cleared
  // afterwards that request was silently dropped: nothing re-registered, and the
  // fitting stayed one step behind until some unrelated size or scale change came
  // along. Clearing first makes the re-request re-register for the next pass.
  //
  // There is deliberately no persistent "pending logical size" to go with it: the
  // re-registered run reads mSize at the time it runs, so it uses the CURRENT size
  // rather than replaying whatever size was current when the request was raised.
  mProcessorRegistered = false;

  if(DALI_LIKELY(mVisualData))
  {
    // Call ApplyFittingMode
    mVisualData->ApplyFittingMode(mSize, false);
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
