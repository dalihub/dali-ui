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
#include <dali-ui-foundation/internal/layouts/layout-manager-object.h>
#include <dali-ui-foundation/internal/layouts/layout-reflow-resolver.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
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
  float  childScale = childImpl.GetEffectiveScale();
  Insets margin     = childImpl.GetMargin();
  float  marginW    = static_cast<float>(margin.start + margin.end) * childScale;
  float  marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;

  // The extent this parent makes available to the child: the parent's own final
  // size less the child's margin. It is the constraint BOTH re-measures below use,
  // and for a WRAP_CONTENT / MATCH_PARENT child it is also the one
  // LayoutController::ProcessLayoutRoot derives (parent SIZE - margin) when the
  // same view is driven as a layout root in its own right -- so a standalone root
  // takes a measure cache HIT here rather than re-running its producer. (A FIXED-size
  // standalone root instead uses its requested size in ProcessLayoutRoot, so an
  // unconsumed pass may re-run its producer once here; that value is constraint-
  // independent, so there is no geometry change and no thrash.)
  const float availW = std::max(0.0f, parentFullWidth - marginW);
  const float availH = std::max(0.0f, parentFullHeight - marginH);
  const bool  matchW = childImpl.GetRequestedWidth() == MATCH_PARENT;
  const bool  matchH = childImpl.GetRequestedHeight() == MATCH_PARENT;

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
  if(slotUnconsumed && !(matchW && matchH))
  {
    LayoutDependency::ArrangeOwnedMeasureScope ownerScope(&owner);
    childImpl.Measure(availW, availH);
  }

  MeasuredSize measured = childImpl.GetMeasuredSize();
  float        childW   = matchW ? availW : measured.width;
  float        childH   = matchH ? availH : measured.height;

  // A MATCH_PARENT axis is placed at the parent's extent rather than at the measured
  // size, so the child is re-measured against the size it will actually get.
  if(matchW || matchH)
  {
    LayoutDependency::ArrangeOwnedMeasureScope ownerScope(&owner);
    childImpl.Measure(childW, childH);
  }

  LayoutRect bounds(childImpl.GetRequestedX() * childScale + static_cast<float>(margin.start) * childScale,
                    childImpl.GetRequestedY() * childScale + static_cast<float>(margin.top) * childScale,
                    childW, childH);
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
    mData.mMeasureInProgress   = true;
    mData.mMeasureDirty        = false;
    mData.mMeasurePassPoisoned = false;
    mData.mMeasureCacheValid   = false;
    mData.mArrangeCacheValid   = false;
  }

  ~MeasurePassGuard()
  {
    mData.mMeasureInProgress = false;
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
 * that a reparent / scale-context reset invalidated the logical context WHILE
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
 */
struct ViewDataImpl::ArrangePassGuard
{
  explicit ArrangePassGuard(ViewDataImpl& data)
  : mData(data)
  {
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
  }

  ArrangePassGuard(const ArrangePassGuard&)            = delete;
  ArrangePassGuard& operator=(const ArrangePassGuard&) = delete;

  ViewDataImpl& mData;
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
  mCoreInteractionObject(nullptr),
  mVisualData(nullptr),
  mAttachments(nullptr),
  mFocusNavigationData(nullptr),
  mRenderEffectData(nullptr),
  mResourceReadyData(nullptr),
  mRequestedX(0.0f),
  mRequestedY(0.0f),
  mMeasuredSize{0.0f, 0.0f},
  // Pure cache key; its initial value is never consulted because
  // mMeasureCacheValid starts false.
  mLastMeasureConstraint{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()},
  mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
  mLastArrangeInput{0.0f, 0.0f, 0.0f, 0.0f},
  mMargin(),
  mPadding(),
  mRequestedWidth(WRAP_CONTENT),
  mRequestedHeight(WRAP_CONTENT),
  mLayoutMode(Ui::LayoutMode::DEFAULT),
  mSize(0, 0),
  mLastArrangedRenderEffectSize(0, 0),
  mAccessibilityData(nullptr),
  mAccessibleObjectCreator(nullptr),
  mAccessibilityRole{static_cast<int32_t>(Accessibility::Role::NONE)},
  mSkipChildrenUpdate(false),
  mMeasureCacheValid(false),
  mMeasureDirty(false),
  mMeasureInProgress(false),
  mMeasurePassPoisoned(false),
  mMeasureResultAvailable(false),
  mMeasuredSlotUnconsumed(false),
  mArrangeCacheValid(false),
  mArrangeDirty(false),
  mArrangeInProgress(false),
  mArrangePassPoisoned(false),
  mArrangeCacheBlockedDuringPass(false),
  mArrangeResultAvailable(false),
  mEffectiveScaleValid(false),
  mEffectiveScaleInvalidatedDuringPass(false),
  mKeyEventDispatchInProgress(false),
  mInitialLayoutDone(false),
  mIsFocusGroup(false),
  mDispatchKeyEvents(true),
  mAccessibleCreatable(true),
  mProcessorRegistered(false),
  mFittingModeLayoutFinishedSignalConnected(false),
  mDefaultFocusIndicatorSuppressedByStateEffect(false),
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

  if(!mChildren.Empty())
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
    ResetEffectiveScaleRecursive();
    InvalidateMeasure();
  }
}

UiScalePolicy ViewDataImpl::GetUiScalePolicy() const
{
  return mScalePolicy;
}

float ViewDataImpl::GetEffectiveScale() const
{
  if(mEffectiveScale < 0.0f)
  {
    mEffectiveScale = ComputeEffectiveScale();
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
  mEffectiveScale    = -1.0f;
  mMeasureDirty      = true;
  mMeasureCacheValid = false;
  mArrangeDirty      = true;
  mArrangeCacheValid = false;

  if(mMeasureInProgress)
  {
    mMeasurePassPoisoned = true;
  }
  if(mArrangeInProgress)
  {
    mArrangePassPoisoned = true;
  }

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
  // surfaces as the parent's CHANGE slot. Mirrors the existing
  // OnChildAdd guard for the standalone+transition combination.
  if(IntegrationView::IsLayoutModeStandalone(mViewImpl))
  {
    Ui::View parentView = GetParentView();
    if(parentView && GetImpl(parentView).GetLayoutTransition())
    {
      GetImpl(parentView).InvalidateMeasure();
    }
    RegisterWithLayoutController();
    return;
  }

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    GetImpl(parentLayout).InvalidateMeasure();
    return;
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    GetImpl(parentView).InvalidateMeasure();
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
    GetImpl(parentLayout).InvalidateArrange();
    return;
  }

  // Propagate to parent View (no LayoutManager)
  Ui::View parentView = GetParentView();
  if(parentView)
  {
    GetImpl(parentView).InvalidateArrange();
    return;
  }

  // Reached top of View tree → register with LayoutController
  RegisterWithLayoutController();
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

bool ViewDataImpl::IsInitialLayoutDone() const
{
  return mInitialLayoutDone;
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
    // ResetEffectiveScaleRecursive() sets mEffectiveScale = -1.0f and clears
    // mMeasureCacheValid for every node in the subtree, guaranteeing:
    //   (a) scale is recomputed from the new parent chain on next GetEffectiveScale(), and
    //   (b) the invalid measure cache forces a cache miss in Measure() so all
    //       nodes fully re-measure with the new scale.
    //
    // After this, InvalidateMeasure() marks the direct child dirty and
    // propagates up to the new layout root. Invalidation always propagates and
    // registers now (there is no dirty short-circuit), so this reaches the new
    // root regardless of the child's prior dirty state.
    ViewDataImpl::Get(childImpl).ResetEffectiveScaleRecursive();

    // Invalidate the child's measure cache -- its previous cache was computed
    // under a different parent's constraints and is no longer reliable.
    childImpl.InvalidateMeasure();

    if(childAffectsSelf)
    {
      // Also invalidate this view's chain directly. The child's
      // InvalidateMeasure now always propagates (no dirty short-circuit), so
      // for a non-standalone child this normally reaches us anyway; the
      // explicit self-invalidation is kept as a direct statement that adding a
      // contributing child invalidates this view's own cached measured size,
      // and it is idempotent.
      //
      // For standalone (boundary) children, this fallback is unnecessary: the
      // child's own InvalidateMeasure registers it as a layout root (Phase 2
      // boundary rule), and OnSceneConnection re-registers dirty boundaries
      // that were already dirty when reparented.
      InvalidateMeasure();
    }
    else if(HasLayoutTransition())
    {
      // Standalone child + transition-attached parent: the standalone
      // path above does not dirty self, so this view would not be
      // reached by ProcessLayouts and the dispatcher would never run
      // its CaptureBeforeLayout / StartTransitionsAfterLayout pass for
      // this parent -- meaning the ENTER (and any subsequent CHANGE)
      // would never be dispatched, while the pending-enter set
      // accumulates entries that fire late on the next unrelated
      // dirty event. Force the parent dirty so its dispatcher pass
      // runs in the same layout batch as the standalone child's.
      InvalidateMeasure();
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

      // Invalidate the removed child's measure cache so that it gets
      // re-measured when re-parented to a different container.
      // Note: Actor parent-child relationship is already severed at this
      // point, so child's InvalidateMeasure cannot propagate to us.
      childImpl.InvalidateMeasure();
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
  const bool isDirty = mMeasureDirty || mArrangeDirty;
  if(!GetParentView() || (IntegrationView::IsLayoutModeStandalone(mViewImpl) && isDirty))
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
  EnsureLayoutCallbacksObject(*this)->SetArrangeCallback(std::move(callback));
  InvalidateArrange();
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
  IntrusivePtr<TraitObject> object(new AbsoluteLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const FlexLayoutParams& params)
{
  IntrusivePtr<TraitObject> object(new FlexLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const GridLayoutParams& params)
{
  IntrusivePtr<TraitObject> object(new GridLayoutParamsImpl(params));
  SetTrait(Integration::ReservedTraitId::GRID_LAYOUT_PARAMS, object);
  InvalidateMeasure();
}

void ViewDataImpl::SetLayoutParams(const StackLayoutParams& params)
{
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
  // correction sits on the ARRANGE side on purpose -- arrange has no cache-hit
  // path, so it is reached even on a pass where the parent's measure cache hits and
  // MeasureStandaloneChildren never runs, which is exactly the case an ancestor
  // cache clear could not have fixed here anyway.
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
    if(isDirectParent && nodeData.mArrangeInProgress)
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
    if(nodeData.mArrangeInProgress)
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
    return mMeasuredSize; // {0,0} until a pass completes (mMeasureResultAvailable)
  }

  float s = mViewImpl.GetEffectiveScale();

  // Push effective scale to the actor animatable property so that decoration
  // constraints (corner radius, borderline width) can read it as a scale input.
  // Read back the current actor property value to skip redundant scene-graph writes.
  // This also naturally corrects any value set externally on EFFECTIVE_SCALE.
  if(!Dali::Equals(s, mViewImpl.Self().GetProperty<float>(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX)))
  {
    // SetProperty triggers ViewDataImpl::SetProperty(VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), which:
    //   - updates the actor animatable so decoration constraints re-evaluate, and
    //   - calls UpdateCornerRadius() for active RenderEffect / OffScreenRendering.
    mViewImpl.Self().SetProperty(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, s);
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

  if(mMeasureCacheValid && !mMeasureDirty && !mMeasurePassPoisoned &&
     mLastMeasureConstraint.width >= 0.0f && FloatEqual(mLastMeasureConstraint.width, effNatW) &&
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
  mMeasuredSize.width     = visual.width;
  mMeasuredSize.height    = visual.height;
  mMeasureResultAvailable = true;

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

LayoutRect ViewDataImpl::Arrange(const LayoutRect& bounds)
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

  // Open the arrange transaction. The guard owns mArrangeInProgress /
  // mArrangePassPoisoned / mEffectiveScaleInvalidatedDuringPass / mArrangeCacheValid
  // for this scope.
  ArrangePassGuard pass(*this);

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
  // RIGHT_TO_LEFT. Runs once per Arrange after every OnArrange variant
  // (LayoutManager / ArrangeCallback / default), keeping layout managers
  // direction-agnostic.
  ApplyLayoutDirection(finalBounds.width);

  // Conditional cache publish, mirroring Measure. The logical context is
  // re-validated only when nothing invalidated it DURING this pass (a reparent
  // or a scale-context reset), never unconditionally (plan34 27.22).
  //
  // The input KEY is published only when every premise of this pass still
  // holds at its end: no re-invalidation (mArrangeDirty was consumed at entry,
  // so seeing it true means one arrived mid-pass), no poison, a valid logical
  // context, and a valid measure cache -- an arrange run before this view was
  // ever measured used default/stale measured sizes for its children and must
  // not be frozen into the cache (plan34 11).
  //
  // Nothing reads mArrangeCacheValid / mLastArrangeInput / mEffectiveScaleValid
  // yet: there is no arrange cache-hit path in this increment, so this block is
  // inert bookkeeping. The one live part is that mArrangeDirty is no longer
  // cleared here, so a mid-pass InvalidateArrange() survives its pass.
  //
  // mArrangeCacheBlockedDuringPass is the freshness-only member of this
  // predicate: a cache-only invalidation that arrived mid-pass declines the
  // publish but, unlike a poison, registers NO follow-up (the follow-up branch
  // below deliberately does not test it -- a cache-only invalidation must never
  // turn into a scheduled layout). The ancestor-invalidation walk sets it on an
  // unowned arrange-in-progress ancestor it clears, so this view's arrange cache
  // cannot be re-published over that clear before the pass ends.
  mEffectiveScaleValid = !mEffectiveScaleInvalidatedDuringPass;
  if(!mArrangeDirty && !mArrangePassPoisoned && !mArrangeCacheBlockedDuringPass && mEffectiveScaleValid && mMeasureCacheValid)
  {
    mLastArrangeInput  = bounds;
    mArrangeCacheValid = true;
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

// FORWARD NOTE: the corrective re-measure for an unconsumed standalone slot lives on
// the arrange path, which today always runs. When an arrange cache-HIT path is added,
// an Arrange() that returns before reaching here would silently skip the correction --
// so mMeasuredSlotUnconsumed on any standalone child must then be added to the arrange
// cache-hit predicate (or the direct parent's arrange publish declined) to keep it.
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

  for(auto& childView : mChildren)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }

    Actor child  = childImpl.Self();
    float oldX   = child.GetProperty<float>(Actor::Property::POSITION_X);
    float childW = child.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    child.SetPositionX(parentWidth - oldX - childW);
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

void ViewDataImpl::ResetEffectiveScaleRecursive()
{
  mEffectiveScale = -1.0f;

  // Drop the cached results back to the "never measured" state so every node
  // re-measures with the new scale.
  //
  // mMeasureDirty is explicitly CLEARED here, not left alone: this function
  // resets the view to the "never measured" state, and every caller follows it
  // with InvalidateMeasure() (SetUiScalePolicy, UiScaleManagerImpl::...,
  // OnChildAdded) which propagates up the new ancestor chain and calls
  // RegisterWithLayoutController(). That follow-up call now runs
  // unconditionally, so leaving the flag set would no longer suppress the
  // re-layout -- but the clear is kept so the state matches "never measured".
  //
  // This reproduces the previous sentinel encoding exactly, where this
  // function overwrote the constraint with NaN ("never measured") on top of
  // whatever was there — including the dirty sentinel.
  mMeasureCacheValid   = false;
  mMeasureDirty        = false;
  mArrangeCacheValid   = false;
  mEffectiveScaleValid = false;

  if(mMeasureInProgress)
  {
    mMeasurePassPoisoned = true;
  }
  if(mArrangeInProgress)
  {
    mArrangePassPoisoned                 = true;
    mEffectiveScaleInvalidatedDuringPass = true;
  }

  for(auto& childView : mChildren)
  {
    ViewDataImpl::Get(GetImpl(childView)).ResetEffectiveScaleRecursive();
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
    LayoutController& controller = LayoutController::Get(window);
    controller.RequestLayout(&mViewImpl);

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
  // differ. Exact comparison (not epsilon): the returned rect is authoritative
  // geometry and must equal mArrangedBounds / the actor target exactly.
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
        newChildren.PushBack(std::move(*it));
      }
    }
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
  mViewImpl.InvalidateMeasure();
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
      return;
    }
  }

  mTraits.emplace_back(id, object);
  if(mTraits.back().second)
  {
    mTraits.back().second->OnAttached(id, self);
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
        // We don't need to hold data for it. But need to apply fitting mode now.
        viewImpl.GetViewDataImpl().SizeOrUiScaleChanged();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();
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
            viewImpl.InvalidateMeasure();

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
              GetImpl(parentView).InvalidateMeasure();
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
  if(DALI_LIKELY(mVisualData))
  {
    // Call ApplyFittingMode
    mVisualData->ApplyFittingMode(mSize, false);
  }
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
