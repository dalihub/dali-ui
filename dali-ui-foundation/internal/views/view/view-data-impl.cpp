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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/rendering/visual-renderer.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

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

constexpr unsigned int OFF_SCREEN_RENDERING_TYPE_COUNT = 3u;

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

constexpr int INNER_SHADOW_DEPTH_INDEX = DepthIndex::DECORATION - 1;
constexpr int BORDERLINE_DEPTH_INDEX   = DepthIndex::FOREGROUND_EFFECT - 1;

static constexpr uint32_t INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 10);
static constexpr uint32_t BORDERLINE_CORNER_RADIUS_CONSTRAINT_TAG(
  Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 11);

static constexpr uint32_t BORDERLINE_WIDTH_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 12);
static constexpr uint32_t BORDERLINE_COLOR_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 13);
static constexpr uint32_t BORDERLINE_OFFSET_CONSTRAINT_TAG(Dali::Ui::ConstraintTagRanges::UI_CONSTRAINT_TAG_START + 14);

/**
 * @brief Constraint function for Borderline's CornerRadius
 * inputs[0] : View CornerRadius, [1] : View CornerRadiusPolicy, [2] : View size, [3] : Borderline Width, [4] :
 * Borderline Offset
 * @param[out] current InnerShadow's corner radius value.
 * @param[in] inputs Input properties.
 */
static void BorderlineCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  // We just assume below state are applied.
  // - Transform::ORIGIN is CENTER
  // - Transform::ANCHOR_POINT is CENTER
  // - Transform::OFFSET_POLICY are ABSOLUTE
  // - Transform::SIZE_POLICY are RELATIVE
  // - Transform::SIZE is Vector2::ONE
  // - Transform::EXTRA_SIZE is Vector2::ZERO

  Vector4 viewCornerRadius = inputs[0]->GetVector4();

  const int     viewCornerRadiusPolicy = inputs[1]->GetInteger();
  const Vector3 visualSize             = inputs[2]->GetVector3(); // We use VisualSize as ViewSize.

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minViewSize = std::min(visualSize.x, visualSize.y);
    viewCornerRadius *= minViewSize;
  }

  const float borderlineWidth  = inputs[3]->GetFloat();
  const float borderlineOffset = inputs[4]->GetFloat();
  const float expendedRadius   = borderlineWidth * (1.0f + borderlineOffset) * 0.5f;

  // Corner Radius for Borderline is expand about borderlineWidth.

  // Calculate on pixel scale.
  current.x = viewCornerRadius.x < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.x + expendedRadius;
  current.y = viewCornerRadius.y < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.y + expendedRadius;
  current.z = viewCornerRadius.z < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.z + expendedRadius;
  current.w = viewCornerRadius.w < Dali::Math::MACHINE_EPSILON_100 ? 0.0f : viewCornerRadius.w + expendedRadius;

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minVisualSize = std::min(visualSize.x + expendedRadius, visualSize.y + expendedRadius);
    if(DALI_LIKELY(minVisualSize > Math::MACHINE_EPSILON_100))
    {
      current /= minVisualSize;
    }
  }
}

/**
 * @brief Constraint function for InnerShadow's CornerRadius
 * inputs[0] : View CornerRadius, [1] : View CornerRadiusPolicy, [2] : View size, [3] : ExtraSize, [4] : Borderline
 * Width
 * @param[out] current InnerShadow's corner radius value.
 * @param[in] inputs Input properties.
 */
static void InnerShadowCornerRadiusConstraint(Vector4& current, const PropertyInputContainer& inputs)
{
  // We just assume below state are applied.
  // - Transform::ORIGIN is CENTER
  // - Transform::ANCHOR_POINT is CENTER
  // - Transform::OFFSET_POLICY are ABSOLUTE
  // - Transform::SIZE_POLICY are RELATIVE
  // - Transform::SIZE is Vector2::ONE
  // - Visual::BORDERLINE_OFFSET is 1.0f

  Vector4 viewCornerRadius = inputs[0]->GetVector4();

  const int     viewCornerRadiusPolicy = inputs[1]->GetInteger();
  const Vector3 visualSize             = inputs[2]->GetVector3(); // We use VisualSize as ViewSize.

  Vector2 extraSize = inputs[3]->GetVector2();

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minViewSize = std::min(visualSize.x, visualSize.y);
    viewCornerRadius *= minViewSize;
  }

  const float borderlineWidth = inputs[4]->GetFloat();

  // Corner Radius for Innershadow is expand about borderlineWidth.

  // Calculate on pixel scale.
  current.x = viewCornerRadius.x + borderlineWidth;
  current.y = viewCornerRadius.y + borderlineWidth;
  current.z = viewCornerRadius.z + borderlineWidth;
  current.w = viewCornerRadius.w + borderlineWidth;

  if(viewCornerRadiusPolicy == Ui::Visual::Transform::Policy::RELATIVE)
  {
    const float minVisualSize = std::min(visualSize.x + extraSize.x, visualSize.y + extraSize.y);
    if(DALI_LIKELY(minVisualSize > Math::MACHINE_EPSILON_100))
    {
      current /= minVisualSize;
    }
  }
}

bool PerformAccessibilityAction(Ui::View view, const Dali::String& actionName, const Property::Map& attributes)
{
  using Dali::Accessibility::ActionType;
  DALI_ASSERT_DEBUG(view);

  auto& viewImpl     = Integration::GetImpl(view);
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
  auto& viewImpl     = Integration::GetImpl(view);
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
      ret = Integration::GetImpl(view).OnAccessibilityActivated();
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
  auto&    viewImpl     = Integration::GetImpl(view);
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

const char* SIGNAL_KEY_EVENT              = "keyEvent";
const char* SIGNAL_KEY_INPUT_FOCUS_GAINED = "keyInputFocusGained";
const char* SIGNAL_KEY_INPUT_FOCUS_LOST   = "keyInputFocusLost";
const char* SIGNAL_TAPPED                 = "tapped";
const char* SIGNAL_PANNED                 = "panned";
const char* SIGNAL_PINCHED                = "pinched";
const char* SIGNAL_LONG_PRESSED           = "longPressed";
const char* SIGNAL_GET_NAME               = "getName";
const char* SIGNAL_GET_DESCRIPTION        = "getDescription";
const char* SIGNAL_DO_GESTURE             = "doGesture";

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
    Integration::ViewImpl& viewImpl(Integration::GetImpl(view));
    auto&                  viewDataImpl = Dali::Ui::Internal::ViewDataImpl::Get(viewImpl);
    connected                           = true;

    if(0 == strcmp(signalName.CStr(), SIGNAL_KEY_EVENT))
    {
      viewImpl.KeyEventSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_KEY_INPUT_FOCUS_GAINED))
    {
      viewImpl.KeyInputFocusGainedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_KEY_INPUT_FOCUS_LOST))
    {
      viewImpl.KeyInputFocusLostSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_TAPPED))
    {
      viewImpl.EnableGestureDetection(GestureType::TAP);
      viewImpl.GetTapGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_PANNED))
    {
      viewImpl.EnableGestureDetection(GestureType::PAN);
      viewImpl.GetPanGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_PINCHED))
    {
      viewImpl.EnableGestureDetection(GestureType::PINCH);
      viewImpl.GetPinchGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.CStr(), SIGNAL_LONG_PRESSED))
    {
      viewImpl.EnableGestureDetection(GestureType::LONG_PRESS);
      viewImpl.GetLongPressGestureDetector().DetectedSignal().Connect(tracker, functor);
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
  Integration::ViewImplPtr impl = Integration::ViewImpl::New();
  View                     handle(*impl);
  return handle;
}
// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(View, CustomActor, Create);

// Note: Properties are registered separately below.

SignalConnectorType registerSignal1(typeRegistration, SIGNAL_KEY_EVENT, &DoConnectSignal);
SignalConnectorType registerSignal2(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_GAINED, &DoConnectSignal);
SignalConnectorType registerSignal3(typeRegistration, SIGNAL_KEY_INPUT_FOCUS_LOST, &DoConnectSignal);
SignalConnectorType registerSignal4(typeRegistration, SIGNAL_TAPPED, &DoConnectSignal);
SignalConnectorType registerSignal5(typeRegistration, SIGNAL_PANNED, &DoConnectSignal);
SignalConnectorType registerSignal6(typeRegistration, SIGNAL_PINCHED, &DoConnectSignal);
SignalConnectorType registerSignal7(typeRegistration, SIGNAL_LONG_PRESSED, &DoConnectSignal);
SignalConnectorType registerSignal8(typeRegistration, SIGNAL_GET_NAME, &DoConnectSignal);
SignalConnectorType registerSignal9(typeRegistration, SIGNAL_GET_DESCRIPTION, &DoConnectSignal);
SignalConnectorType registerSignal10(typeRegistration, SIGNAL_DO_GESTURE, &DoConnectSignal);

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

} // unnamed namespace

// clang-format off
// Properties registered without macro to use specific member variables.
const PropertyRegistration ViewDataImpl::PROPERTY_4(typeRegistration,  "keyInputFocus",                  Ui::View::Property::KEY_INPUT_FOCUS,                       Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_5(typeRegistration,  "background",                     Ui::View::Property::BACKGROUND,                            Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_6(typeRegistration,  "margin",                         Ui::View::Property::MARGIN,                                Property::EXTENTS, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_7(typeRegistration,  "padding",                        Ui::View::Property::PADDING,                               Property::EXTENTS, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_11(typeRegistration, "leftFocusableActorId",           Ui::View::Property::LEFT_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_12(typeRegistration, "rightFocusableActorId",          Ui::View::Property::RIGHT_FOCUSABLE_ACTOR_ID,         Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_13(typeRegistration, "upFocusableActorId",             Ui::View::Property::UP_FOCUSABLE_ACTOR_ID,            Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_14(typeRegistration, "downFocusableActorId",           Ui::View::Property::DOWN_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_15(typeRegistration, "shadow",                         Ui::View::Property::SHADOW,                           Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_16(typeRegistration, "accessibilityName",              Ui::View::Property::ACCESSIBILITY_NAME,               Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_17(typeRegistration, "accessibilityDescription",       Ui::View::Property::ACCESSIBILITY_DESCRIPTION,        Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_18(typeRegistration, "accessibilityTranslationDomain", Ui::View::Property::ACCESSIBILITY_TRANSLATION_DOMAIN, Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_19(typeRegistration, "accessibilityRole",              Ui::View::Property::ACCESSIBILITY_ROLE,               Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_20(typeRegistration, "accessibilityHighlightable",     Ui::View::Property::ACCESSIBILITY_HIGHLIGHTABLE,      Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_21(typeRegistration, "accessibilityAttributes",        Ui::View::Property::ACCESSIBILITY_ATTRIBUTES,         Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_22(typeRegistration, "dispatchKeyEvents",              Ui::View::Property::DISPATCH_KEY_EVENTS,              Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_23(typeRegistration, "accessibilityHidden",            Ui::View::Property::ACCESSIBILITY_HIDDEN,             Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_24(typeRegistration, "clockwiseFocusableActorId",      Ui::View::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID,     Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_25(typeRegistration, "counterClockwiseFocusableActorId", Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID, Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_26(typeRegistration, "automationId",                   Ui::View::Property::AUTOMATION_ID,                    Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_27(typeRegistration, "accessibilityValue",             Ui::View::Property::ACCESSIBILITY_VALUE,              Property::STRING,  &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_28(typeRegistration, "accessibilityScrollable",        Ui::View::Property::ACCESSIBILITY_SCROLLABLE,         Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_29(typeRegistration, "accessibilityStates",            Ui::View::Property::ACCESSIBILITY_STATES,             Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_30(typeRegistration, "accessibilityIsModal",           Ui::View::Property::ACCESSIBILITY_IS_MODAL,           Property::BOOLEAN, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_31(typeRegistration, "offScreenRendering",             Ui::View::Property::OFFSCREEN_RENDERING,              Property::INTEGER, &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_32(typeRegistration, "innerShadow",                    Ui::View::Property::INNER_SHADOW,                     Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);
const PropertyRegistration ViewDataImpl::PROPERTY_33(typeRegistration, "borderline",                     Ui::View::Property::BORDERLINE,                       Property::MAP,     &ViewDataImpl::SetProperty, &ViewDataImpl::GetProperty);

const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_1(typeRegistration, "viewCornerRadius",       Ui::View::Property::CORNER_RADIUS,        Property::VECTOR4, &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_2(typeRegistration, "viewCornerRadiusPolicy", Ui::View::Property::CORNER_RADIUS_POLICY, Property::Value(static_cast<int>(Ui::Visual::Transform::Policy::ABSOLUTE)), &ViewDataImpl::SetProperty, nullptr); ///< Make animatable, for constarint-input
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_3(typeRegistration, "viewCornerSquareness",   Ui::View::Property::CORNER_SQUARENESS,    Property::VECTOR4, &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_4(typeRegistration, "viewBorderlineWidth",    Ui::View::Property::BORDERLINE_WIDTH,     Property::FLOAT,   &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_5(typeRegistration, "viewBorderlineColor",    Ui::View::Property::BORDERLINE_COLOR,     Property::Value(Color::BLACK), &ViewDataImpl::SetProperty, nullptr);
const AnimatablePropertyRegistration ViewDataImpl::ANIMATABLE_PROPERTY_6(typeRegistration, "viewBorderlineOffset",   Ui::View::Property::BORDERLINE_OFFSET,    Property::FLOAT,   &ViewDataImpl::SetProperty, nullptr);

// clang-format on

ViewDataImpl::ViewDataImpl(Integration::ViewImpl& viewImpl)
: mViewImpl(viewImpl),
  mAccessibilityData(nullptr),
  mVisualData(nullptr),
  mLeftFocusableActorId(-1),
  mRightFocusableActorId(-1),
  mUpFocusableActorId(-1),
  mDownFocusableActorId(-1),
  mClockwiseFocusableActorId(-1),
  mCounterClockwiseFocusableActorId(-1),
  mBackgroundColor(Color::TRANSPARENT),
  mRenderEffect(nullptr),
  mStartingPinchScale(nullptr),
  mMargin(0, 0, 0, 0),
  mPadding(0, 0, 0, 0),
  mSize(0, 0),
  mKeyEventSignal(),
  mKeyInputFocusGainedSignal(),
  mKeyInputFocusLostSignal(),
  mResourceReadySignal(),
  mPinchGestureDetector(),
  mPanGestureDetector(),
  mTapGestureDetector(),
  mLongPressGestureDetector(),
  mOffScreenRenderingImpl(nullptr),
  mOffScreenRenderingType(Ui::View::OffScreenRenderingType::NONE),
  mInputMethodContext(),
  mIdleCallback(nullptr),
  mFlags(Integration::ViewImpl::ViewBehaviour(Integration::ViewImpl::VIEW_BEHAVIOUR_DEFAULT)),
  mAccessibilityRole{static_cast<int32_t>(AccessibilityRole::NONE)},
  mIsKeyboardNavigationSupported(false),
  mIsKeyboardFocusGroup(false),
  mIsEmittingResourceReadySignal(false),
  mIdleCallbackRegistered(false),
  mDispatchKeyEvents(true),
  mAccessibleCreatable(true),
  mProcessorRegistered(false)
{
}

ViewDataImpl::~ViewDataImpl()
{
  if(mVisualData)
  {
    mVisualData->ClearVisuals();
  }

  // All gesture detectors will be destroyed so no need to disconnect.
  delete mStartingPinchScale;

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

void ViewDataImpl::InitializeVisualData()
{
  mVisualData = std::make_unique<ViewDataImpl::VisualData>(*this);
}

ViewDataImpl& ViewDataImpl::Get(Integration::ViewImpl& viewImpl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return viewImpl.GetViewDataImpl();
}

const ViewDataImpl& ViewDataImpl::Get(const Integration::ViewImpl& viewImpl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return viewImpl.GetViewDataImpl();
}

// Gesture Detection Methods
void ViewDataImpl::PinchDetected(Actor actor, const PinchGesture& pinch)
{
  mViewImpl.OnPinch(pinch);
}

void ViewDataImpl::PanDetected(Actor actor, const PanGesture& pan)
{
  mViewImpl.OnPan(pan);
}

void ViewDataImpl::TapDetected(Actor actor, const TapGesture& tap)
{
  mViewImpl.OnTap(tap);
}

void ViewDataImpl::LongPressDetected(Actor actor, const LongPressGesture& longPress)
{
  mViewImpl.OnLongPress(longPress);
}

void ViewDataImpl::ResourceReady()
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Emit signal if all enabled visuals registered by the view are ready or there are no visuals.
  if(DALI_LIKELY(mVisualData) && mVisualData->IsResourceReady())
  {
    EmitResourceReadySignal();
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, depthIndex);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled);
  }
}

void ViewDataImpl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex)
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

Ui::Visual::Base ViewDataImpl::GetVisual(Property::Index index) const
{
  return Ui::Visual::Base(GetVisualImplPtr(index));
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

void ViewDataImpl::EnableCornerPropertiesOverridden(Ui::Visual::Base& visual, bool enable,
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

void ViewDataImpl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  Ui::View view = Ui::View::DownCast(BaseHandle(object));

  if(view)
  {
    Integration::ViewImpl& viewImpl(Integration::GetImpl(view));

    switch(index)
    {
      case Ui::View::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mLeftFocusableActorId = focusId;
        }
      }
      break;

      case Ui::View::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mRightFocusableActorId = focusId;
        }
      }
      break;

      case Ui::View::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mUpFocusableActorId = focusId;
        }
      }
      break;

      case Ui::View::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mDownFocusableActorId = focusId;
        }
      }
      break;

      case Ui::View::Property::KEY_INPUT_FOCUS:
      {
        if(value.Get<bool>())
        {
          viewImpl.SetKeyInputFocus();
        }
        else
        {
          viewImpl.ClearKeyInputFocus();
        }
        break;
      }

      case Ui::View::Property::BACKGROUND:
      {
        std::string          url;
        Vector4              color;
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          viewImpl.SetBackground(*map);
        }
        else if(GetStdString(value, url))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
          {
            // don't know the size to load
            Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(url, ImageDimensions());
            if(visual)
            {
              viewImpl.GetViewDataImpl().mVisualData->RegisterVisual(Ui::View::Property::BACKGROUND, visual,
                                                                     DepthIndex::BACKGROUND);
              viewImpl.GetViewDataImpl().EnableCornerPropertiesOverridden(visual, true);
            }
          }
        }
        else if(value.Get(color))
        {
          viewImpl.SetBackgroundColor(UiColor(color));
        }
        else
        {
          // The background is an empty property map, so we should clear the background
          viewImpl.ClearBackground();
        }
        break;
      }

      case Ui::View::Property::MARGIN:
      {
        Extents margin;
        if(value.Get(margin))
        {
          viewImpl.GetViewDataImpl().SetMargin(margin);
        }
        break;
      }

      case Ui::View::Property::PADDING:
      {
        Extents padding;
        if(value.Get(padding))
        {
          viewImpl.GetViewDataImpl().SetPadding(padding);
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
          // The shadow is an empty property map, so we should clear the shadow
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

            auto accessible = viewImpl.GetAccessibleObject();
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
      case Ui::View::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mClockwiseFocusableActorId = focusId;
        }
        break;
      }
      case Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          viewImpl.GetViewDataImpl().mCounterClockwiseFocusableActorId = focusId;
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

      case Ui::View::Property::ACCESSIBILITY_STATES:
      {
        int32_t states;
        if(value.Get(states))
        {
          if(DALI_LIKELY(viewImpl.GetViewDataImpl().GetAccessibilityData()) ||
             states != static_cast<int32_t>(AccessibilityData::GetDefaultViewAccessibilityStates().GetRawData32()))
          {
            viewImpl.GetViewDataImpl().GetOrCreateAccessibilityData().mAccessibilityProps.states =
              AccessibilityStates{static_cast<uint32_t>(states)};
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
    }
  }
}

Property::Value ViewDataImpl::GetProperty(BaseObject* object, Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  Property::Value value;

  Ui::View view = Ui::View::DownCast(BaseHandle(object));

  if(view)
  {
    Integration::ViewImpl& viewImpl(Integration::GetImpl(view));

    switch(index)
    {
      case Ui::View::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mLeftFocusableActorId;
        break;
      }

      case Ui::View::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mRightFocusableActorId;
        break;
      }

      case Ui::View::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mUpFocusableActorId;
        break;
      }

      case Ui::View::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mDownFocusableActorId;
        break;
      }

      case Ui::View::Property::KEY_INPUT_FOCUS:
      {
        value = viewImpl.HasKeyInputFocus();
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
        value = viewImpl.GetViewDataImpl().GetMargin();
        break;
      }

      case Ui::View::Property::PADDING:
      {
        value = viewImpl.GetViewDataImpl().GetPadding();
        break;
      }

      case Ui::View::Property::SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(viewImpl.GetViewDataImpl().mVisualData))
        {
          Ui::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::SHADOW);
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

      case Ui::View::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mClockwiseFocusableActorId;
        break;
      }

      case Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = viewImpl.GetViewDataImpl().mCounterClockwiseFocusableActorId;
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

      case Ui::View::Property::ACCESSIBILITY_STATES:
      {
        const auto* accessibilityData = viewImpl.GetViewDataImpl().GetAccessibilityData();
        value                         = static_cast<int32_t>((DALI_LIKELY(accessibilityData)
                                                                ? accessibilityData->mAccessibilityProps.states
                                                                : AccessibilityData::GetDefaultViewAccessibilityStates())
                                                               .GetRawData32());
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
          Ui::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::INNER_SHADOW);
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
          Ui::Visual::Base visual = viewImpl.GetViewDataImpl().mVisualData->GetVisual(Ui::View::Property::BORDERLINE);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
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

void ViewDataImpl::SetMargin(Extents margin)
{
  mViewImpl.GetViewDataImpl().mMargin = margin;

  // Trigger a size negotiation request that may be needed when setting a margin.
  mViewImpl.RelayoutRequestToView();
}

Extents ViewDataImpl::GetMargin() const
{
  return mViewImpl.GetViewDataImpl().mMargin;
}

void ViewDataImpl::SetPadding(Extents padding)
{
  mViewImpl.GetViewDataImpl().mPadding = padding;

  // Trigger a size negotiation request that may be needed when setting a padding.
  mViewImpl.RelayoutRequestToView();
}

Extents ViewDataImpl::GetPadding() const
{
  return mViewImpl.GetViewDataImpl().mPadding;
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
    consumed = mInputMethodContext.FilterEventKey(event);
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

void ViewDataImpl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  GetOrCreateAccessibilityData().SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes ViewDataImpl::GetAccessibilityReadingInfoType() const
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
  auto bridge = Accessibility::Bridge::GetCurrentBridge();
  return DALI_LIKELY(bridge) ? !!bridge->GetAccessible(mViewImpl.Self()) : false;
}

void ViewDataImpl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool ViewDataImpl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
}

void ViewDataImpl::EmitAccessibilityStateChanged(Accessibility::State state, int newValue)
{
  Dali::CustomActor handle(mViewImpl.GetOwner());
  auto              bridge = Accessibility::Bridge::GetCurrentBridge();
  if(DALI_LIKELY(bridge))
  {
    if(state == Accessibility::State::SHOWING)
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
    auto accessible = dynamic_cast<Accessibility::ActorAccessible*>(Accessibility::Accessible::Get(handle));
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

void ViewDataImpl::SetShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("shadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::SHADOW, visual, DepthIndex::BACKGROUND_EFFECT);
      EnableCornerPropertiesOverridden(visual, true);

      mViewImpl.RelayoutRequestToView();
    }
  }
}

void ViewDataImpl::ClearShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mViewImpl.RelayoutRequestToView();
  }
}

void ViewDataImpl::SetInnerShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("innerShadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::INNER_SHADOW, visual, INNER_SHADOW_DEPTH_INDEX);

      Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

      auto visualCornerRadiusProperty = visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_RADIUS, false);
      auto visualBorderlineProperty   = visualImpl.GetPropertyObject(DevelVisual::Property::BORDERLINE_WIDTH);

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

      mViewImpl.RelayoutRequestToView();
    }
  }
}

void ViewDataImpl::ClearInnerShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::INNER_SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mViewImpl.RelayoutRequestToView();
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
        previousVisualImplPtr->DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY, map);

        // Trigger borderline relative constraints once
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_WIDTH, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_COLOR, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::View::Property::BORDERLINE_OFFSET, false);
        return;
      }
    }
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("borderline");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::View::Property::BORDERLINE, visual, BORDERLINE_DEPTH_INDEX);

      // Create constraint only if we set Borderline property as DevelView::BORDERLINE_XXX.
      if(!forciblyCreate)
      {
        Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

        auto visualCornerRadiusProperty    = visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_RADIUS, false);
        auto visualBorderlineWidthProperty = visualImpl.GetPropertyObject(DevelVisual::Property::BORDERLINE_WIDTH);

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

          auto visualBorderlineColorProperty  = visualImpl.GetPropertyObject(DevelVisual::Property::BORDERLINE_COLOR);
          auto visualBorderlineOffsetProperty = visualImpl.GetPropertyObject(DevelVisual::Property::BORDERLINE_OFFSET);

          if(DALI_LIKELY(visualBorderlineColorProperty.propertyIndex != Property::INVALID_INDEX &&
                         visualBorderlineColorProperty.object) &&
             DALI_LIKELY(visualBorderlineOffsetProperty.propertyIndex != Property::INVALID_INDEX &&
                         visualBorderlineOffsetProperty.object))
          {
            auto borderlineWidthConstraint = Constraint::New<float>(
              visualBorderlineWidthProperty.object, visualBorderlineWidthProperty.propertyIndex, EqualToConstraint());
            borderlineWidthConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_WIDTH));
            auto borderlineColorConstraint = Constraint::New<Vector4>(
              visualBorderlineColorProperty.object, visualBorderlineColorProperty.propertyIndex, EqualToConstraint());
            borderlineColorConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_COLOR));
            auto borderlineOffsetConstraint =
              Constraint::New<float>(visualBorderlineOffsetProperty.object,
                                     visualBorderlineOffsetProperty.propertyIndex, EqualToConstraint());
            borderlineOffsetConstraint.AddSource(Source(handle, Ui::View::Property::BORDERLINE_OFFSET));

            Dali::Integration::ConstraintSetInternalTag(borderlineWidthConstraint, BORDERLINE_WIDTH_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineColorConstraint, BORDERLINE_COLOR_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineOffsetConstraint, BORDERLINE_OFFSET_CONSTRAINT_TAG);

            borderlineWidthConstraint.Apply();
            borderlineColorConstraint.Apply();
            borderlineOffsetConstraint.Apply();

            visualImpl.AddConstraintFeature(borderlineWidthConstraint, {Ui::View::Property::BORDERLINE_WIDTH});
            visualImpl.AddConstraintFeature(borderlineColorConstraint, {Ui::View::Property::BORDERLINE_COLOR});
            visualImpl.AddConstraintFeature(borderlineOffsetConstraint, {Ui::View::Property::BORDERLINE_OFFSET});
          }

          EnableCornerPropertiesOverridden(visual, true, borderlineCornerRadiusConstraint);
        }
      }

      mViewImpl.RelayoutRequestToView();
    }
  }
}

void ViewDataImpl::ClearBorderline()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::View::Property::BORDERLINE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mViewImpl.RelayoutRequestToView();
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

void ViewDataImpl::UpdateVisualProperties(
  const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UpdateVisualProperties(properties);
  }
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
    mViewImpl.RelayoutRequestToView();
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

std::shared_ptr<Ui::ViewAccessible> ViewDataImpl::GetAccessibleObject()
{
  return GetOrCreateAccessibilityData().GetAccessibleObject();
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
  // TODO : Need to make constriant for RenderEffect corner radius update
  if(mRenderEffect || mOffScreenRenderingImpl)
  {
    Actor self = mViewImpl.Self();

    Property::Map map;
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS,
               self.GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS));
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
               self.GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY));
    map.Insert(Ui::DevelVisual::Property::CORNER_SQUARENESS,
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
  map.Insert(Ui::Visual::Property::TYPE, Ui::Visual::Type::COLOR);
  map.Insert(Ui::ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT);
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_WIDTH,
             self.GetProperty<float>(Ui::View::Property::BORDERLINE_WIDTH));
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_COLOR,
             self.GetProperty<Vector4>(Ui::View::Property::BORDERLINE_COLOR));
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_OFFSET,
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
