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
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <cmath>
#include <cstring>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/shadow.h>
#include <dali-ui-foundation/integration-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/internal/common/attachment-container.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-object.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-object.h>
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-reflow-resolver.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>
#include <dali-ui-foundation/internal/views/state-effect-target-trait.h>
#include <dali-ui-foundation/internal/views/state-handler-trait.h>
#include <dali-ui-foundation/internal/views/view-state-manager.h>
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/view-gradient-color-binding.h>
#include <dali-ui-foundation/internal/visuals/visual-property-map-helper.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/types/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <algorithm>

using Dali::Integration::GetStdString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace IntegrationView = Dali::Ui::Integration::View;

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_VIEW_DATA");
#endif

constexpr unsigned int OFF_SCREEN_RENDERING_TYPE_COUNT  = 3u;
constexpr char         BACKGROUND_COLOR_BINDING_ID[]    = "BackgroundColor";
constexpr char         BACKGROUND_GRADIENT_BINDING_ID[] = "BackgroundGradient";
constexpr char         COLOR_BINDING_ID[]               = "Color";
constexpr float        MEASURE_CACHE_DIRTY              = -1.0f;

class ScopedSkipChildrenUpdate
{
public:
  explicit ScopedSkipChildrenUpdate(bool& flag)
  : mFlag(flag),
    mPrevious(flag)
  {
    mFlag = true;
  }

  ~ScopedSkipChildrenUpdate()
  {
    mFlag = mPrevious;
  }

  ScopedSkipChildrenUpdate(const ScopedSkipChildrenUpdate&)            = delete;
  ScopedSkipChildrenUpdate& operator=(const ScopedSkipChildrenUpdate&) = delete;

private:
  bool& mFlag;
  bool  mPrevious;
};

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

TraitId ToTraitId(LayoutParamsType type)
{
  switch(type)
  {
    case LayoutParamsType::ABSOLUTE:
      return Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
    case LayoutParamsType::STACK:
      return Integration::ReservedTraitId::STACK_LAYOUT_PARAMS;
    case LayoutParamsType::GRID:
      return Integration::ReservedTraitId::GRID_LAYOUT_PARAMS;
    case LayoutParamsType::FLEX:
      return Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS;
  }
  DALI_ASSERT_ALWAYS(false && "Unknown LayoutParamsType");
  return Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
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

void ArrangeStandaloneChild(ViewImpl& childImpl, float parentFullWidth, float parentFullHeight)
{
  float        childScale = childImpl.GetEffectiveScale();
  Extents      margin     = childImpl.GetMargin();
  float        marginW    = static_cast<float>(margin.start + margin.end) * childScale;
  float        marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;
  MeasuredSize measured   = childImpl.GetMeasuredSize();
  float        childW     = measured.width;
  float        childH     = measured.height;

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

  LayoutRect bounds(childImpl.GetRequestedPositionX() * childScale + static_cast<float>(margin.start) * childScale,
                    childImpl.GetRequestedPositionY() * childScale + static_cast<float>(margin.top) * childScale,
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

static constexpr uint32_t INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 10);
static constexpr uint32_t BORDERLINE_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 11);

static constexpr uint32_t BORDERLINE_WIDTH_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 12);
static constexpr uint32_t BORDERLINE_COLOR_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 13);
static constexpr uint32_t BORDERLINE_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 14);

bool PerformAccessibilityAction(Ui::View view, const Dali::String& actionName, const Property::Map& attributes)
{
  using Dali::Devel::Accessibility::ActionType; // LCOV_EXCL_LINE
  DALI_ASSERT_DEBUG(view);

  auto& viewImpl     = GetImpl(view);
  auto& viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(viewImpl);
  DALI_ASSERT_DEBUG(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityActionSignal.Empty());

  ActionType action = ActionType::MAX_COUNT;
  if(actionName == ACTION_ACCESSIBILITY_ACTIVATE)
  {
    action = ActionType::ACTIVATE;
  }
  else if(actionName == ACTION_ACCESSIBILITY_ESCAPE)
  {
    action = ActionType::ESCAPE;
  }
  else if(actionName == ACTION_ACCESSIBILITY_INCREMENT)
  {
    action = ActionType::INCREMENT;
  }
  else if(actionName == ACTION_ACCESSIBILITY_DECREMENT)
  {
    action = ActionType::DECREMENT;
  }

  if(action != ActionType::MAX_COUNT)
  {
    bool success = viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityActionSignal.Emit({action, Dali::Actor{}});
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: %s, success : %d\n", actionName.CStr(),
                  success);
    return success;
  }

  return false;
}

bool PerformLegacyAccessibilityAction(Ui::View view, const Dali::String& actionName)
{
  auto& viewImpl     = GetImpl(view);
  auto& viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(viewImpl);
  bool  ret          = true;
  if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_ACTIVATE))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityActivateSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityActivateSignal.Emit();
    }
    else
    {
      ret = GetImpl(view).OnAccessibilityActivated();
    }
  }
  else if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_READING_SKIPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingSkippedSignal.Emit();
    }
  }
  else if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_READING_PAUSED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingPausedSignal.Emit();
    }
  }
  else if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_READING_RESUMED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingResumedSignal.Emit();
    }
  }
  else if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_READING_CANCELLED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingCancelledSignal.Emit();
    }
  }
  else if(0 == strcmp(actionName.CStr(), ACTION_ACCESSIBILITY_READING_STOPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal.Empty())
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityReadingStoppedSignal.Emit();
    }
  }
  else
  {
    ret = false;
  }

  if(ret)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed Legacy AccessibilityAction: %s\n", actionName.CStr());
  }
  return ret;
}

bool DoAccessibilityAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Ui::View view         = Ui::View::DownCast(handle);
  auto&    viewImpl     = GetImpl(view);
  auto&    viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(viewImpl);

  DALI_ASSERT_ALWAYS(view);

  if(!viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityActionSignal.Empty())
  {
    return PerformAccessibilityAction(view, actionName, attributes);
  }

  // Fall back to legacy action is no ActionSignal is connected
  return PerformLegacyAccessibilityAction(view, actionName);
}

bool DoLegacyAccessibilityAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Ui::View view = Ui::View::DownCast(handle);

  DALI_ASSERT_ALWAYS(view);

  return PerformLegacyAccessibilityAction(view, actionName);
}

const char* SIGNAL_KEY_EVENT       = "keyEvent";
const char* SIGNAL_FOCUS_CHANGED   = "focusChanged";
const char* SIGNAL_GET_NAME        = "getName";
const char* SIGNAL_GET_DESCRIPTION = "getDescription";
const char* SIGNAL_DO_GESTURE      = "doGesture";

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
    else if(0 == strcmp(signalName.CStr(), SIGNAL_GET_NAME))
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityGetNameSignal.Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_GET_DESCRIPTION))
    {
      viewDataImpl.GetOrCreateAccessibilityData().mAccessibilityGetDescriptionSignal.Connect(tracker, functor);
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
DALI_TYPE_REGISTRATION_BEGIN(View, CustomActor, Create);

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1(typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal);
SignalConnectorType registerSignal2(typeRegistration, SIGNAL_FOCUS_CHANGED, &DoConnectSignal);
SignalConnectorType registerSignal7(typeRegistration, SIGNAL_GET_NAME, &DoConnectSignal);
SignalConnectorType registerSignal8(typeRegistration, SIGNAL_GET_DESCRIPTION, &DoConnectSignal);
SignalConnectorType registerSignal9(typeRegistration, SIGNAL_DO_GESTURE, &DoConnectSignal);

// === Accessibility Actions === START
TypeAction registerAction1(typeRegistration, ACTION_ACCESSIBILITY_ACTIVATE, &DoAccessibilityAction);
TypeAction registerAction2(typeRegistration, ACTION_ACCESSIBILITY_ESCAPE, &DoAccessibilityAction);
TypeAction registerAction3(typeRegistration, ACTION_ACCESSIBILITY_INCREMENT, &DoAccessibilityAction);
TypeAction registerAction4(typeRegistration, ACTION_ACCESSIBILITY_DECREMENT, &DoAccessibilityAction);
// === Accessibility Actions === END

// === Legacy Accessibility Actions === START
TypeAction registerAction5(typeRegistration, ACTION_ACCESSIBILITY_READING_SKIPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction6(typeRegistration, ACTION_ACCESSIBILITY_READING_CANCELLED, &DoLegacyAccessibilityAction);
TypeAction registerAction7(typeRegistration, ACTION_ACCESSIBILITY_READING_STOPPED, &DoLegacyAccessibilityAction);
TypeAction registerAction8(typeRegistration, ACTION_ACCESSIBILITY_READING_PAUSED, &DoLegacyAccessibilityAction);
TypeAction registerAction9(typeRegistration, ACTION_ACCESSIBILITY_READING_RESUMED, &DoLegacyAccessibilityAction);
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

// clang-format off
// Properties registered without macro to use specific member variables.
const PropertyRegistration ViewDataImpl::PROPERTY_5(typeRegistration,  "background",                     Ui::View::Property::BACKGROUND,                       Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_6(typeRegistration,  "margin",                         Ui::View::Property::MARGIN,                           Property::EXTENTS, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_7(typeRegistration,  "padding",                        Ui::View::Property::PADDING,                          Property::EXTENTS, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_11(typeRegistration, "leftFocusableViewId",           Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_12(typeRegistration, "rightFocusableViewId",          Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID,         Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_13(typeRegistration, "upFocusableViewId",             Ui::View::Property::UP_FOCUSABLE_VIEW_ID,            Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_14(typeRegistration, "downFocusableViewId",           Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_15(typeRegistration, "shadow",                         Ui::View::Property::SHADOW,                           Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_16(typeRegistration, "accessibilityName",              Ui::View::Property::ACCESSIBILITY_NAME,               Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_17(typeRegistration, "accessibilityDescription",       Ui::View::Property::ACCESSIBILITY_DESCRIPTION,        Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_18(typeRegistration, "accessibilityTranslationDomain", Ui::View::Property::ACCESSIBILITY_TRANSLATION_DOMAIN, Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_19(typeRegistration, "accessibilityRole",              Ui::View::Property::ACCESSIBILITY_ROLE,               Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_20(typeRegistration, "accessibilityHighlightable",     Ui::View::Property::ACCESSIBILITY_HIGHLIGHTABLE,      Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_21(typeRegistration, "accessibilityAttributes",        Ui::View::Property::ACCESSIBILITY_ATTRIBUTES,         Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_22(typeRegistration, "dispatchKeyEvents",              Ui::View::Property::DISPATCH_KEY_EVENTS,              Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_23(typeRegistration, "accessibilityHidden",            Ui::View::Property::ACCESSIBILITY_HIDDEN,             Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_24(typeRegistration, "clockwiseFocusableViewId",      Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID,     Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_25(typeRegistration, "counterClockwiseFocusableViewId", Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID, Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_26(typeRegistration, "automationId",                   Ui::View::Property::AUTOMATION_ID,                    Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_27(typeRegistration, "accessibilityValue",             Ui::View::Property::ACCESSIBILITY_VALUE,              Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_28(typeRegistration, "accessibilityScrollable",        Ui::View::Property::ACCESSIBILITY_SCROLLABLE,         Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_30(typeRegistration, "accessibilityIsModal",           Ui::View::Property::ACCESSIBILITY_IS_MODAL,           Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
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
  mInputMethodContext(),
  mRenderEffect(nullptr),
  mOffScreenRenderingImpl(nullptr),
  mOffScreenRenderingType(Ui::View::OffScreenRenderingType::NONE),
  mIdleCallback(nullptr),
  mRequestedPositionX(0.0f),
  mRequestedPositionY(0.0f),
  mMeasuredSize{0.0f, 0.0f},
  // NaN is distinct from MEASURE_CACHE_DIRTY (-1.0f), so the first
  // InvalidateMeasure still propagates. See view-impl.cpp for details.
  mLastMeasuredConstraint{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()},
  mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
  mMargin(),
  mPadding(),
  mRequestedWidth(WRAP_CONTENT),
  mRequestedHeight(WRAP_CONTENT),
  mLayoutMode(Ui::LayoutMode::DEFAULT),
  mSize(0, 0),
  mAccessibilityData(nullptr),
  mAccessibilityRole{static_cast<int32_t>(Accessibility::Role::NONE)},
  mSkipChildrenUpdate(false),
  mArrangeDirty(false),
  mPendingChildRemovalForLayoutTransition(false),
  mInitialLayoutDone(false),
  mIsFocusGroup(false),
  mIsEmittingResourceReadySignal(false),
  mIdleCallbackRegistered(false),
  mDispatchKeyEvents(true),
  mAccessibleCreatable(true),
  mProcessorRegistered(false),
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

  if(mIdleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the view is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
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
      Extents      margin                = childImpl.GetMargin();
      float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
      float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
      float        childWidthConstraint  = std::max(0.0f, contentWidth * s - marginW);
      float        childHeightConstraint = std::max(0.0f, contentHeight * s - marginH);
      MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

      float childNatW  = (s > 0.0f) ? childSize.width / s : childSize.width;
      float childNatH  = (s > 0.0f) ? childSize.height / s : childSize.height;
      float childX     = childImpl.GetRequestedPositionX();
      float childY     = childImpl.GetRequestedPositionY();
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
    Vector3 naturalSize     = mViewImpl.Self().GetNaturalSize();
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
    Vector3 naturalSize     = mViewImpl.Self().GetNaturalSize();
    float   contentNaturalH = std::max(0.0f, naturalSize.height - ph);
    size.height             = contentNaturalH + ph;
    if(natH >= 0.0f)
    {
      size.height = std::min(size.height, natH);
    }
  }
  return {size.width * s, size.height * s};
}

MeasuredSize ViewDataImpl::ArrangeDefault(const LayoutRect& bounds)
{
  Actor self = mViewImpl.Self();
  self.SetPositionX(bounds.x);
  self.SetPositionY(bounds.y);
  self.SetWidth(bounds.width);
  self.SetHeight(bounds.height);

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
      Extents      margin          = childImpl.GetMargin();
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
      float childX = visPadLeft + visMarginStart + childImpl.GetRequestedPositionX() * s;
      float childY = visPadTop + visMarginTop + childImpl.GetRequestedPositionY() * s;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childW, childH);
      }

      childImpl.Arrange(LayoutRect(childX, childY, childW, childH));
    }
  }

  return {bounds.width, bounds.height};
}

bool ViewDataImpl::HandleKeyEventDefault(const Dali::KeyEvent& event)
{
  if(auto* traitObject = GetCoreInteractionObject())
  {
    if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
    {
      return interactiveTraitImpl->OnKeyEvent(View::DownCast(mViewImpl.Self()), event);
    }
  }
  return false;
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

      Extents padding = mPadding;

      Dali::LayoutDirection::Type layoutDirection = GetEffectiveLayoutDirection();

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

  ApplyFittingMode(size);
}

const ViewState& ViewDataImpl::GetState() const
{
  return mState;
}

bool ViewDataImpl::IsEffectivelyEnabled() const
{
  return ViewStateManager::Get().IsEffectivelyEnabled(mViewImpl);
}

bool ViewDataImpl::IsEffectivelyFocused() const
{
  return ViewStateManager::Get().IsEffectivelyFocused(mViewImpl);
}

ViewImpl::LayoutFinishedSignalType& ViewDataImpl::LayoutFinishedSignal()
{
  return mLayoutFinishedSignal;
}

ViewImpl::StateChangedSignalType& ViewDataImpl::StateChangedSignal()
{
  return mStateChangedSignal;
}

Ui::View::ResourceReadySignalType& ViewDataImpl::ResourceReadySignal()
{
  return mResourceReadySignal;
}

Ui::View::OffScreenRenderingFinishedSignalType& ViewDataImpl::OffScreenRenderingFinishedSignal()
{
  return mOffScreenRenderingFinishedSignal;
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
  std::swap(changes.enterChildren, mPendingEnterChildren);
  std::swap(changes.reorderedChildren, mPendingReorderedChildren);
  changes.hadChildRemoval                 = mPendingChildRemovalForLayoutTransition;
  mPendingChildRemovalForLayoutTransition = false;
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

void ViewDataImpl::SetFocusNavigationCallback(Callback<View(View, FocusDirection)> callback)
{
  if(callback || mFocusNavigationData)
  {
    EnsureFocusNavigationData().callback = std::move(callback);
  }
}

View ViewDataImpl::RequestFocusNavigation(View currentFocusedView, FocusDirection direction)
{
  if(mFocusNavigationData && mFocusNavigationData->callback)
  {
    return mFocusNavigationData->callback.Invoke(currentFocusedView, direction);
  }
  return mViewImpl.OnFocusNavigationRequested(currentFocusedView, direction);
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
  bool           consumed = FilterKeyEvent(event);

  if(!consumed && !mKeyEventSignal.Empty())
  {
    consumed = mKeyEventSignal.Emit(handle, event);
  }

  if(!consumed)
  {
    consumed = mViewImpl.OnKeyEvent(event);
  }

  return consumed;
}

bool ViewDataImpl::ActivateAccessibilityDefault()
{
  return Ui::FocusManager::Get().SetCurrentFocusView(Ui::View::DownCast(mViewImpl.Self()));
}

ViewAccessible* ViewDataImpl::CreateDefaultAccessibleObject()
{
  return new ViewAccessible(mViewImpl.Self());
}

Dali::LayoutDirection::Type ViewDataImpl::GetEffectiveLayoutDirection() const
{
  return static_cast<Dali::LayoutDirection::Type>(mViewImpl.Self().GetProperty<int>(Actor::Property::LAYOUT_DIRECTION));
}

void ViewDataImpl::SetRequestedPositionX(float x)
{
  if(!Dali::Equals(mRequestedPositionX, x))
  {
    mRequestedPositionX = x;
    // InvalidateMeasure (not InvalidateArrange): a WRAP_CONTENT parent's
    // OnMeasure reads the child's RequestedPosition into maxRight/maxBottom,
    // so a position change can affect the parent's measured size. Measure
    // invalidation also marks the chain dirty for Arrange.
    InvalidateMeasure();
  }
}

void ViewDataImpl::SetRequestedPositionY(float y)
{
  if(!Dali::Equals(mRequestedPositionY, y))
  {
    mRequestedPositionY = y;
    InvalidateMeasure();
  }
}

float ViewDataImpl::GetRequestedPositionX() const
{
  return mRequestedPositionX;
}

float ViewDataImpl::GetRequestedPositionY() const
{
  return mRequestedPositionY;
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
  // Early-exit guard: if already dirty, the ancestor chain has already been
  // invalidated and the layout root has been registered, so re-propagation is
  // redundant. "Never measured" state (NaN) still requires propagation because
  // the chain has never been walked from this view.
  //
  // NOTE: Callers that change the view's ancestor-chain structure
  // (e.g. reparenting) must not rely on this view's InvalidateMeasure to reach
  // the new parent chain — the guard may skip propagation. Instead, the
  // reparenting site should invalidate the new parent directly. See
  // OnChildAdd / OnChildRemove where this is done.
  if(Dali::Equals(mLastMeasuredConstraint.width, MEASURE_CACHE_DIRTY))
  {
    return;
  }

  mEffectiveScale                = -1.0f;
  mLastMeasuredConstraint.width  = MEASURE_CACHE_DIRTY;
  mLastMeasuredConstraint.height = MEASURE_CACHE_DIRTY;
  mArrangeDirty                  = true;

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
  // Early-exit guard mirrors InvalidateMeasure: if already dirty, the
  // ancestor chain has already been invalidated. Only the Dirty state
  // short-circuits; NeverArranged (mArrangeDirty=false, but never arranged)
  // still propagates on its first invalidation.
  if(mArrangeDirty)
  {
    return;
  }

  mArrangeDirty = true;

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
  mViewImpl.RelayoutRequest();
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

void ViewDataImpl::SetMargin(const Extents& margin)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::MARGIN, margin);
}

Extents ViewDataImpl::GetMargin() const
{
  return mMargin;
}

void ViewDataImpl::SetPadding(const Extents& padding)
{
  mViewImpl.Self().SetProperty(Ui::View::Property::PADDING, padding);
}

Extents ViewDataImpl::GetPadding() const
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
  mLayoutTransition = transition;
  // Detach: drop any pending ENTER / REORDER / REMOVE markers. Records
  // are only produced while a transition is attached (see OnChildAdd /
  // Insert / OnChildOrderChanged / Remove); a previously attached
  // transition could have left entries that we want to discard now so
  // a later re-attach does not surface them as a stale cause on the
  // next pass. In particular mPendingChildRemovalForLayoutTransition is
  // consumed only when StartTransitionsForView runs (which requires an
  // attached transition), so without this clear the marker would
  // survive a detach -> layout pass -> reattach cycle and tag the next
  // unrelated CHANGE as SIBLING_REMOVED.
  if(!transition)
  {
    mPendingEnterChildren.clear();
    mPendingReorderedChildren.clear();
    mPendingChildRemovalForLayoutTransition = false;
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

  // Attach: seed any pre-existing children as pending ENTER candidates,
  // but only when this view has not yet completed its initial layout
  // pass. OnChildAdd only inserts into mPendingEnterChildren when a
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
      mPendingEnterChildren.insert(&GetImpl(childView));
    }
  }
}

LayoutTransition ViewDataImpl::GetLayoutTransition() const
{
  return mLayoutTransition;
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

void ViewDataImpl::Insert(uint32_t index, Ui::View child)
{
  if(!child)
  {
    return;
  }

  // Adding to the Actor tree triggers OnChildAdd on this ViewImpl, which is
  // the single source of truth for registering the child in mChildren and
  // for invalidating the new parent chain. Insert only takes additional
  // responsibility for positioning the child at the requested index.
  mViewImpl.Self().Add(child);

  if(index >= mChildren.Count())
  {
    // OnChildAdd push_back'd the child at the end; target index is end.
    return;
  }

  // Fast path: when this was a fresh add, OnChildAdd push_back'd the child,
  // so it is at the tail of mChildren. Avoid an O(N) scan in that case.
  IntegrationView::ChildContainer::Iterator it;
  if(mChildren.Count() > 0 && *(mChildren.End() - 1) == child)
  {
    it = mChildren.End() - 1;
  }
  else
  {
    it = std::find(mChildren.Begin(), mChildren.End(), child);
    if(it == mChildren.End())
    {
      // OnChildAdd did not register this child (e.g. non-View actor). Nothing
      // to reorder.
      return;
    }
  }

  const size_t currentIdx = static_cast<size_t>(std::distance(mChildren.Begin(), it));
  if(currentIdx == index)
  {
    return;
  }

  Ui::View moved = std::move(*it);
  mChildren.Erase(it);
  mChildren.Insert(mChildren.Begin() + index, std::move(moved));

  // Tag every logical child so the layout transition dispatcher reports
  // CHANGE cause as LayoutChangeCause::REORDERED for both the moved child and the
  // siblings whose indices shifted as a result. dali-core's
  // OnChildOrderChanged fires only on actor-tree sibling order changes;
  // Insert() touches the logical (mChildren) order alone, so this is the
  // only place that records the reorder for the CHANGE classifier.
  // Matches OnChildOrderChanged's full-list tagging so a logical reorder
  // and an actor-tree reorder produce the same cause classification.
  // Skip the record when no transition is attached — the dispatcher
  // would never consume it, and stale raw pointers could outlive the
  // child without any global cleanup.
  if(mLayoutTransition)
  {
    for(auto& childView : mChildren)
    {
      mPendingReorderedChildren.insert(&GetImpl(childView));
    }
  }

  // mChildren order affects layout output (e.g. LinearLayout visual order,
  // GridLayout cell assignment). When the child was already under this view
  // (Self().Add is a no-op in that case), OnChildAdd does not fire, so this
  // is the only invalidation point for the reorder. When the child was a
  // fresh add, self is already dirty from OnChildAdd and the guard makes
  // this a no-op.
  InvalidateMeasure();
}

void ViewDataImpl::RemoveAllChildren(Ui::RemovePolicy policy)
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
  Ui::LayoutTransition transition = mLayoutTransition;
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
          mPendingEnterChildren.erase(&childImpl);
          // Same rationale as the immediate-remove path's OnChildRemove:
          // a stale raw ViewImpl* in the reorder set could outlive its
          // child after deferred-remove EXIT and cause a future heap-
          // reused address to be misclassified as REORDERED. Erase
          // per-child here (not full clear) so the cause of any
          // siblings still pending reorder is preserved.
          mPendingReorderedChildren.erase(&childImpl);
          // Mark sibling removal so the dispatcher tags this pass's CHANGE
          // dispatches on the remaining siblings as SIBLING_REMOVED. Set
          // only when a transition is attached to avoid leaving stale
          // marker state on views without transitions.
          mPendingChildRemovalForLayoutTransition = true;
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
        mPendingEnterChildren.erase(&childImpl);
        mPendingReorderedChildren.erase(&childImpl);
        // Remaining siblings under THIS direct parent reflow into the freed
        // slot; tag their CHANGE as SIBLING_REMOVED on the next pass -- but only
        // when THIS view owns a transition to consume the marker. For an
        // inherited EXIT this view may have no transition, and the marker --
        // consumed only by a transition-bearing view's layout pass -- would
        // never be cleared and would mis-tag a future CHANGE if it later gains
        // one.
        if(transition)
        {
          mPendingChildRemovalForLayoutTransition = true;
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
      mPendingChildRemovalForLayoutTransition = true;
    }
    selfActor.Remove(child);
  }
}

void ViewDataImpl::Raise(Ui::LayoutOrderPolicy policy)
{
  Actor self = mViewImpl.Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
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
    mChildren.PushBack(view);

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
        if(!mLayoutTransition)
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
    if(mLayoutTransition)
    {
      mPendingEnterChildren.insert(&childImpl);
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
    // ResetEffectiveScaleRecursive() sets mEffectiveScale = -1.0f and
    // mLastMeasuredConstraint = NaN for every node in the subtree, guaranteeing:
    //   (a) scale is recomputed from the new parent chain on next GetEffectiveScale(), and
    //   (b) the NaN constraint forces a cache miss in Measure() so all nodes
    //       fully re-measure with the new scale.
    //
    // After this, InvalidateMeasure() transitions the direct child from NaN to
    // DIRTY (the guard passes because NaN != DIRTY per IEEE 754) and propagates
    // up to the new layout root.
    ViewDataImpl::Get(childImpl).ResetEffectiveScaleRecursive();

    // Invalidate the child's measure cache -- its previous cache was computed
    // under a different parent's constraints and is no longer reliable.
    childImpl.InvalidateMeasure();

    if(childAffectsSelf)
    {
      // Also invalidate this view's chain directly. The child's
      // InvalidateMeasure may early-exit via the dirty guard if the child was
      // already invalidated under its previous parent (reparenting of a dirty
      // child), in which case the new parent chain would not be reached.
      // Calling InvalidateMeasure on self guarantees the new ancestor chain
      // is marked and the new layout root is registered. If self is already
      // dirty, the guard makes this a no-op.
      //
      // For standalone (boundary) children, this fallback is unnecessary: the
      // child's own InvalidateMeasure registers it as a layout root (Phase 2
      // boundary rule), and OnSceneConnection re-registers dirty boundaries
      // that were already dirty when reparented.
      InvalidateMeasure();
    }
    else if(mLayoutTransition)
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
      // Same for the reorder marker: Insert() / OnChildOrderChanged keep
      // raw ViewImpl* pointers in mPendingReorderedChildren which must
      // not survive the child's removal -- otherwise a heap-reused address
      // could mis-classify a future child as REORDERED.
      mPendingEnterChildren.erase(&childImpl);
      mPendingReorderedChildren.erase(&childImpl);

      // Record sibling removal so the next CHANGE pass tags remaining
      // siblings as SIBLING_REMOVED. This covers paths that reach
      // OnChildRemove without going through View::Remove's marker-
      // setting branch (e.g. inherited Actor::Remove called directly on
      // the view actor). Same window guard as View::Remove -- without
      // a window the marker cannot be consumed in this pass and would
      // leak across a later add-to-window event. Setting the marker
      // here is idempotent with View::Remove's own setter.
      if(mLayoutTransition && Window::Get(mViewImpl.Self()))
      {
        mPendingChildRemovalForLayoutTransition = true;
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
  const bool isDirty = Dali::Equals(mLastMeasuredConstraint.width, MEASURE_CACHE_DIRTY) ||
                       mArrangeDirty;
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

      if(!enabled && mViewImpl.Self() == Dali::Ui::FocusManager::Get().GetCurrentFocusView())
      {
        Dali::Ui::FocusManager::Get().ClearFocus();
      }

      IntegrationView::SetState(mViewImpl, ViewState::DISABLED, !enabled);

      if(auto* traitObject = GetCoreInteractionObject())
      {
        if(auto* interactiveTraitImpl = traitObject->GetInteractiveTraitImpl())
        {
          interactiveTraitImpl->OnEnabledChanged(View::DownCast(mViewImpl.Self()), enabled);
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
  if(mRenderEffect)
  {
    mRenderEffect->GetOffScreenRenderTasks(tasks, isForward);
  }
  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->GetOffScreenRenderTasks(tasks, isForward);
  }
}

Dali::Texture ViewDataImpl::GetOffScreenRenderingOutput() const
{
  if(mOffScreenRenderingType != Ui::View::OffScreenRenderingType::REFRESH_ONCE)
  {
    DALI_LOG_ERROR(
      "Precondition unsatisfied: Set property OFFSCREEN_RENDERING to OffScreenRenderingType::REFRESH_ONCE\n");
    return Dali::Texture();
  }
  return mOffScreenRenderingImpl->GetTexture();
}

Vector3 ViewDataImpl::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ViewImpl::GetNaturalSize for %s\n",
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

BaseHandle ViewDataImpl::GetLayoutParams(LayoutParamsType type) const
{
  IntrusivePtr<TraitObject> object     = GetTrait(ToTraitId(type));
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? BaseHandle(baseObject) : BaseHandle();
}

void ViewDataImpl::SetLayoutParams(Ui::LayoutParams params)
{
  auto& paramsImpl = static_cast<LayoutParamsImpl&>(params.GetBaseObject());
  SetTrait(paramsImpl.GetTraitId(), AsTraitObject(params));
  InvalidateMeasure();
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

    mRenderEffect = object;
  }
  else
  {
    mRenderEffect.Reset();
  }
}

RenderEffect ViewDataImpl::GetRenderEffect() const
{
  return RenderEffect(mRenderEffect.Get());
}

void ViewDataImpl::ClearRenderEffect()
{
  if(mRenderEffect)
  {
    RenderEffectImplPtr effectImpl = std::move(mRenderEffect);

    // Reset handle first to avoid circular reference
    mRenderEffect.Reset();

    effectImpl->ClearOwnerView();
  }
}

MeasuredSize ViewDataImpl::Measure(float visualW, float visualH)
{
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

  if(mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mLastMeasuredConstraint.width, effNatW) &&
     FloatEqual(mLastMeasuredConstraint.height, effNatH))
  {
    return mMeasuredSize;
  }

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
  visual                         = ApplyConstraints(visual);
  mMeasuredSize.width            = visual.width;
  mMeasuredSize.height           = visual.height;
  mLastMeasuredConstraint.width  = effNatW;
  mLastMeasuredConstraint.height = effNatH;

  // Ensure standalone children are measured even when OnMeasure (e.g. in
  // leaf views like Label) does not iterate children. The measure cache
  // prevents redundant work when OnMeasure already measured them.
  MeasureStandaloneChildren(effVisW, effVisH);

  return mMeasuredSize;
}

MeasuredSize ViewDataImpl::Arrange(const LayoutRect& bounds)
{
  MeasuredSize arrangedSize;
  if(auto* callback = GetArrangeCallback())
  {
    arrangedSize = DispatchArrangeWithCallback(callback, bounds);
  }
  else if(auto* manager = mViewImpl.GetLayoutManager())
  {
    arrangedSize = DispatchArrangeWithLayoutManager(manager, bounds);
  }
  else
  {
    arrangedSize = mViewImpl.OnArrange(bounds);
  }
  mArrangedBounds = bounds;
  mArrangeDirty   = false;

  // Ensure standalone children are arranged even when OnArrange (e.g. in
  // leaf views like Label) does not iterate children.
  ArrangeStandaloneChildren(bounds);

  // Mirror direct children when the effective layout direction resolves to
  // RIGHT_TO_LEFT. Runs once per Arrange after every OnArrange variant
  // (LayoutManager / ArrangeCallback / default), keeping layout managers
  // direction-agnostic.
  ApplyLayoutDirection(bounds.width);

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

  return arrangedSize;
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
    float   childScale = childImpl.GetEffectiveScale();
    Extents margin     = childImpl.GetMargin();
    float   visMarginW = static_cast<float>(margin.start + margin.end) * childScale;
    float   visMarginH = static_cast<float>(margin.top + margin.bottom) * childScale;
    float   childVisW  = std::max(0.0f, visEffW - visMarginW);
    float   childVisH  = std::max(0.0f, visEffH - visMarginH);
    childImpl.Measure(childVisW, childVisH);
  }
}

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
    ArrangeStandaloneChild(childImpl, bounds.width, bounds.height);
  }
}

void ViewDataImpl::ApplyLayoutDirection(float parentWidth)
{
  if(mViewImpl.GetEffectiveLayoutDirection() != Dali::LayoutDirection::RIGHT_TO_LEFT)
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
    return UiScaleManager::Get().GetScale();
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

  // Root: inherit from UiScaleManager
  return UiScaleManager::Get().GetScale();
}

void ViewDataImpl::ResetEffectiveScaleRecursive()
{
  mEffectiveScale = -1.0f;

  // Reset measure cache to the NaN "never measured" initial state (not to
  // MEASURE_CACHE_DIRTY = -1.0f). This forces a cache miss in Measure() so
  // every node re-measures with the new scale.
  //
  // NaN is required here (not DIRTY) because the caller follows this with
  // InvalidateMeasure() on the root. DIRTY would trigger InvalidateMeasure's
  // early-exit guard on the root and skip RegisterWithLayoutController().
  // NaN != DIRTY, so the guard does not fire; Dali::Equals(NaN, DIRTY) is
  // false per IEEE 754.
  mLastMeasuredConstraint.width  = std::numeric_limits<float>::quiet_NaN();
  mLastMeasuredConstraint.height = std::numeric_limits<float>::quiet_NaN();

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

  Extents padding = mViewImpl.GetPadding();
  float   visPadW = static_cast<float>(padding.start + padding.end) * s;
  float   visPadH = static_cast<float>(padding.top + padding.bottom) * s;

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

MeasuredSize ViewDataImpl::DispatchArrangeWithLayoutManager(LayoutManager* manager, const LayoutRect& visualBounds)
{
  Actor self = mViewImpl.Self();
  self.SetPositionX(visualBounds.x);
  self.SetPositionY(visualBounds.y);
  self.SetWidth(visualBounds.width);
  self.SetHeight(visualBounds.height);

  float   s       = mViewImpl.GetEffectiveScale();
  Extents padding = mViewImpl.GetPadding();

  LayoutRect visContentBounds;
  visContentBounds.x      = static_cast<float>(padding.start) * s;
  visContentBounds.y      = static_cast<float>(padding.top) * s;
  visContentBounds.width  = std::max(0.0f, visualBounds.width - static_cast<float>(padding.start + padding.end) * s);
  visContentBounds.height = std::max(0.0f, visualBounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  manager->Arrange(&mViewImpl, visContentBounds);

  return {visualBounds.width, visualBounds.height};
}

MeasuredSize ViewDataImpl::DispatchArrangeWithCallback(ArrangeCallback* callback, const LayoutRect& visualBounds)
{
  Actor self = mViewImpl.Self();
  self.SetPositionX(visualBounds.x);
  self.SetPositionY(visualBounds.y);
  self.SetWidth(visualBounds.width);
  self.SetHeight(visualBounds.height);
  Ui::View view = Ui::View::DownCast(self);
  return callback->Invoke(view, visualBounds);
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
      auto parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
      if(parent && !accessible->GetStates()[Dali::Integration::Accessibility::State::MANAGES_DESCENDANTS]) // LCOV_EXCL_LINE
      {
        parent->EmitActiveDescendantChanged(accessible.Get());
      }
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
  if(mLayoutTransition)
  {
    for(auto& childView : mChildren)
    {
      mPendingReorderedChildren.insert(&GetImpl(childView));
    }
  }

  // A logical child-order change can alter the measured size (e.g. a wrap
  // layout where line-breaking depends on child order), so invalidate measure
  // — not just arrange — mirroring the Insert/MoveChild reorder path.
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

void ViewDataImpl::SetState(ViewState state, bool on, InputEvent cause)
{
  // NOTE Orthogonal state constraint: Disabled is mutually exclusive with Focused and Pressed.
  // Clear them immediately rather than waiting for potentially late system events.

  // NOTE that when the view is focused and user sets `view.SetEnabled(false)`,
  // the event squence will be: "Focused out" -> "Enabled changed".

  ViewState prev = mState;
  if(on)
  {
    mState = mState + state;

    // NOTE Handle orthogonal state constraint
    // When DISABLED added,
    // - PRESSED needs to be cleaned immediately
    // - FOCUSED should have gone already (ASSERT(!mState.Contains(FOCUSED)))
    // When PSEUDO_DISABLED added,
    // - PRESSED needs to be cleaned immediately
    // - FOCUSED can exist
    if(state.IsAnyDisabled())
    {
      mState = mState - ViewState::PRESSED;
    }
  }
  else
  {
    // NOTE Handle orthogonal state constraint
    // This is the case that the focus has gone because it turned disabled.
    // (but disabled state hasn't dispatched yet)
    // -> Immediately update states at once.
    mState = mState - state;

    if(state.Contains(ViewState::FOCUSED))
    {
      mState = mState - ViewState::FOCUS_INDICATED;
    }

    if(state.Contains(ViewState::FOCUSED) && !mViewImpl.Self().IsEnabled())
    {
      mState = mState - ViewState::PRESSED + ViewState::DISABLED;
    }
  }

  if(mState != prev)
  {
    if(prev.Contains(ViewState::PRESSED) && !mState.Contains(ViewState::PRESSED) && mState.IsAnyDisabled())
    {
      cause = cause ? cause.WithCancellation() : InputEvent::Programmatic().WithCancellation();
    }

    ViewStateManager::Get().NotifyStateChanged(View::DownCast(mViewImpl.Self()), prev, mState, cause);
  }
}

void ViewDataImpl::SetNamedStateHandler(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
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

bool ViewDataImpl::UnsetStateHandler(const Dali::String& id)
{
  auto* existing = dynamic_cast<StateHandlerTrait*>(GetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT).Get());
  if(!existing)
  {
    return false;
  }

  return existing->Unset(id.CStr());
}

bool ViewDataImpl::UnsetStateHandlerWhenNotProcessing(const Dali::String& id)
{
  auto* existing = dynamic_cast<StateHandlerTrait*>(GetTrait(Integration::ReservedTraitId::STATE_HANDLER_TRAIT).Get());
  if(!existing)
  {
    return false;
  }

  return existing->UnsetWhenNotProcessing(id.CStr());
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

  if(mOffScreenRenderingImpl) // mOffScreenRenderingType != NONE
  {
    mOffScreenRenderingImpl->SetOwnerView(Ui::View(mViewImpl.GetOwner()));
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

  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->ClearOwnerView();
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
        Extents margin;
        if(value.Get(margin))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(dataImpl.mMargin != margin)
          {
            dataImpl.mMargin = margin;
            viewImpl.InvalidateMeasure();
          }
        }
        break;
      }

      case Ui::View::Property::PADDING:
      {
        Extents padding;
        if(value.Get(padding))
        {
          ViewDataImpl& dataImpl = viewImpl.GetViewDataImpl();
          if(dataImpl.mPadding != padding)
          {
            dataImpl.mPadding = padding;
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

      case Ui::View::Property::ACCESSIBILITY_NAME:
      {
        std::string name;
        if(GetStdString(value, name))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || !name.empty())
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.name = std::move(name);
          }
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string text;
        if(GetStdString(value, text))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || !text.empty())
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.description = std::move(text);
          }
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_ROLE:
      {
        int32_t role;
        if(value.Get(role))
        {
          viewImpl.GetViewDataImpl().mAccessibilityRole = role;
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if(value.Get(highlightable))
        {
          viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.isHighlightable =
            highlightable ? TriStateProperty::TRUE : TriStateProperty::FALSE;
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if(map)
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || !map->Empty())
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.extraAttributes = *map;
          }
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

      case Ui::View::Property::ACCESSIBILITY_HIDDEN:
      {
        bool hidden;
        if(value.Get(hidden))
        {
          const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
          const bool  originalHidden =
            DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
          if(originalHidden != hidden)
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.isHidden = hidden;

            auto accessible = viewImpl.GetViewDataImpl().GetAccessibleObject();
            if(DALI_LIKELY(accessible))
            {
              auto* parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
              if(parent)
              {
                parent->OnChildrenChanged();
              }
            }
          }
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

      case Ui::View::Property::AUTOMATION_ID:
      {
        std::string automationId;
        if(GetStdString(value, automationId))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || !automationId.empty())
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.automationId =
              std::move(automationId);
          }
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_VALUE:
      {
        std::string accessibilityValue;
        if(GetStdString(value, accessibilityValue))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || !accessibilityValue.empty())
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.value = std::move(accessibilityValue);
          }
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_SCROLLABLE:
      {
        bool isScrollable;
        if(value.Get(isScrollable))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || isScrollable)
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.isScrollable = isScrollable;
          }
        }
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_IS_MODAL:
      {
        bool isModal;
        if(value.Get(isModal))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) || isModal)
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.isModal = isModal;
          }
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
        value = viewImpl.GetMargin();
        break;
      }

      case Ui::View::Property::PADDING:
      {
        value = viewImpl.GetPadding();
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

      case Ui::View::Property::ACCESSIBILITY_NAME:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = ToPropertyValue(DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.name : "");
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_DESCRIPTION:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = ToPropertyValue(DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.description : "");
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_ROLE:
      {
        value = viewImpl.GetViewDataImpl().mAccessibilityRole;
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = (DALI_LIKELY(accessibilityData) &&
                 accessibilityData->mAccessibilityProps.isHighlightable == TriStateProperty::TRUE)
                                          ? true
                                          : false;
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value =
          DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.extraAttributes : Property::Map();
        break;
      }

      case Ui::View::Property::DISPATCH_KEY_EVENTS:
      {
        value = viewImpl.GetViewDataImpl().mDispatchKeyEvents;
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_HIDDEN:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
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

      case Ui::View::Property::AUTOMATION_ID:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = ToPropertyValue(DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.automationId : "");
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_VALUE:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = ToPropertyValue(DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.value : "");
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_SCROLLABLE:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isScrollable : false;
        break;
      }

      case Ui::View::Property::ACCESSIBILITY_IS_MODAL:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isModal : false;
        break;
      }

      case Ui::View::Property::OFFSCREEN_RENDERING:
      {
        value = viewImpl.GetViewDataImpl().mOffScreenRenderingType;
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

void ViewDataImpl::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  mInputMethodContext = inputMethodContext;
}

bool ViewDataImpl::FilterKeyEvent(const KeyEvent& event)
{
  bool consumed(false);

  if(mInputMethodContext)
  {
    consumed = Dali::Integration::InputMethodContext::FilterEventKey(mInputMethodContext, event);
  }
  return consumed;
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
  SetAccessibilityStates(GetAccessibilityStates() | ViewAccessibilityStateToMask(state));
}

void ViewDataImpl::RemoveAccessibilityState(Accessibility::State state)
{
  SetAccessibilityStates(GetAccessibilityStates() & ~ViewAccessibilityStateToMask(state));
}

void ViewDataImpl::ClearAccessibilityStates()
{
  SetAccessibilityStates(0u);
}

bool ViewDataImpl::HasAccessibilityState(Accessibility::State state) const
{
  return (GetAccessibilityStates() & ViewAccessibilityStateToMask(state)) != 0u;
}

void ViewDataImpl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool ViewDataImpl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
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

void ViewDataImpl::ApplyFittingMode(const Vector2& size)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ApplyFittingMode(size);
  }
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

      mViewImpl.RelayoutRequest();
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

      mViewImpl.RelayoutRequest();
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
    mShadowVisualObjects.push_back(visual);
    mViewImpl.RelayoutRequest();
  }
}

void ViewDataImpl::ClearShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::SHADOW);
  }

  for(auto& visualObject : mShadowVisualObjects)
  {
    RemoveVisualObject(visualObject);
  }
  mShadowVisualObjects.clear();

  mViewImpl.RelayoutRequest();
}

void ViewDataImpl::SetInnerShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Integration::Visual::Base visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
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

      mViewImpl.RelayoutRequest();
    }
  }
}

void ViewDataImpl::ClearInnerShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::INNER_SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mViewImpl.RelayoutRequest();
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

      mViewImpl.RelayoutRequest();
    }
  }
}

void ViewDataImpl::ClearBorderline()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::BORDERLINE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mViewImpl.RelayoutRequest();
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
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable())) ///< Avoid resource ready callback during shutting down
  {
    if(!mIsEmittingResourceReadySignal)
    {
      // Guard against calls to emit the signal during the callback
      mIsEmittingResourceReadySignal = true;

      // If the signal handler changes visual, it may become ready during this call & therefore this method will
      // get called again recursively. If so, mIdleCallbackRegistered is set below, and we act on it after that
      // secondary invocation has completed by notifying in an Idle callback to prevent further recursion.
      Dali::Ui::View handle(mViewImpl.GetOwner());
      mResourceReadySignal.Emit(handle);

      mIsEmittingResourceReadySignal = false;
    }
    else
    {
      if(!mIdleCallbackRegistered)
      {
        mIdleCallbackRegistered = true;

        // Add idler to emit the signal again
        if(!mIdleCallback)
        {
          // The callback manager takes the ownership of the callback object.
          mIdleCallback = MakeCallback(this, &ViewDataImpl::OnIdleCallback);
          if(DALI_UNLIKELY(!Adaptor::Get().AddIdle(mIdleCallback, true)))
          {
            DALI_LOG_ERROR("Fail to add idle callback for view resource ready. Skip this callback.\n");
            mIdleCallback           = nullptr;
            mIdleCallbackRegistered = false;
          }
        }
      }
    }
  }
}

bool ViewDataImpl::OnIdleCallback()
{
  // Reset the flag
  mIdleCallbackRegistered = false;

  // A visual is ready so view may need relayouting if staged
  if(mViewImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mViewImpl.RelayoutRequest();
  }

  EmitResourceReadySignal();

  if(!mIdleCallbackRegistered)
  {
    // Set the pointer to null as the callback manager deletes the callback after execute it.
    mIdleCallback = nullptr;
  }

  // Repeat idle if mIdleCallbackRegistered become true one more time.
  return mIdleCallbackRegistered;
}

SharedPtr<Ui::ViewAccessible> ViewDataImpl::GetAccessibleObject()
{
  return GetOrCreateAccessibilityData().GetAccessibleObject();
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
      const auto& targets = relation.second;

      Dali::Devel::Accessibility::Relation rel{ToIntegrationRelationType(relation.first), {}}; // LCOV_EXCL_LINE
      std::copy(targets.begin(), targets.end(), std::back_inserter(rel.mTargets));
      result.PushBack(std::move(rel));
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

  // Refresh render effects
  if(mRenderEffect)
  {
    mRenderEffect->Refresh();
  }

  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->Refresh();
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
    if(mOffScreenRenderingImpl)
    {
      auto tempOffscreenRenderingImpl = std::move(mOffScreenRenderingImpl);
      tempOffscreenRenderingImpl->ClearOwnerView();

      if(DALI_LIKELY(mVisualData))
      {
        mVisualData->OffscreenRenderingEnabled(false);
      }
    }
  }
  else if(mOffScreenRenderingType == Ui::View::OffScreenRenderingType::NONE)
  {
    mOffScreenRenderingImpl = std::make_unique<OffScreenRenderingImpl>(newType);
    mOffScreenRenderingImpl->SetOwnerView(handle);

    if(DALI_LIKELY(mVisualData))
    {
      mVisualData->OffscreenRenderingEnabled(true);
    }
  }
  else if(mOffScreenRenderingType != newType)
  {
    mOffScreenRenderingImpl->SetType(newType);
  }
  mOffScreenRenderingType = newType;
}

void ViewDataImpl::UpdateCornerRadius()
{
  if(mRenderEffect || mOffScreenRenderingImpl)
  {
    Actor     self   = mViewImpl.Self();
    const int policy = self.GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY);

    Vector4 cornerRadius = self.GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);

    Property::Map map;
    map.Insert(Ui::Integration::Visual::Property::CORNER_RADIUS, cornerRadius);
    map.Insert(Ui::Integration::Visual::Property::CORNER_RADIUS_POLICY, policy);
    map.Insert(Ui::Integration::Visual::Property::CORNER_SQUARENESS,
               self.GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS));

    if(mRenderEffect)
    {
      mRenderEffect->SetCornerConstants(map);
    }

    if(mOffScreenRenderingImpl)
    {
      mOffScreenRenderingImpl->SetCornerConstants(map);
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
    mVisualData->ApplyFittingMode(mSize);
  }
  mProcessorRegistered = false;
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
