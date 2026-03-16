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
#include "control-data-impl.h"
#include "control-accessibility-data.h"
#include "control-visual-data.h"

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/object/handle-devel.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/constraint-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/rendering/visual-renderer.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/internal/styling/style-manager-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/controls/control-depth-index-ranges.h>
#include <dali-ui-foundation/public-api/controls/image-view/image-view.h>
#include <dali-ui-foundation/public-api/focus-manager/keyboard-focus-manager.h>
#include <dali-ui-foundation/public-api/toolkit-constraint-tag-ranges.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/public-api/rendering/decorated-visual-renderer.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_DATA");
#endif

constexpr unsigned int OFF_SCREEN_RENDERING_TYPE_COUNT = 3u;

/**
 * Performs actions as requested using the action name.
 * @param[in] object The object on which to perform the action.
 * @param[in] actionName The action to perform.
 * @param[in] attributes The attributes with which to perfrom this action.
 * @return true if action has been accepted by this control
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

bool PerformAccessibilityAction(Ui::Control control, const std::string& actionName, const Property::Map& attributes)
{
  using Dali::Accessibility::ActionType;
  DALI_ASSERT_DEBUG(control);
  DALI_ASSERT_DEBUG(!control.AccessibilityActionSignal().Empty());

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
    bool success = control.AccessibilityActionSignal().Emit({action, Dali::Actor{}});
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed AccessibilityAction: %s, success : %d\n", actionName.c_str(),
                  success);
    return success;
  }

  return false;
}

bool PerformLegacyAccessibilityAction(Ui::Control control, const std::string& actionName)
{
  bool ret = true;
  if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_ACTIVATE))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityActivateSignal().Empty())
    {
      control.AccessibilityActivateSignal().Emit();
    }
    else
    {
      ret = Internal::GetImplementation(control).OnAccessibilityActivated();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_SKIPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityReadingSkippedSignal().Empty())
    {
      control.AccessibilityReadingSkippedSignal().Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_PAUSED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityReadingPausedSignal().Empty())
    {
      control.AccessibilityReadingPausedSignal().Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_RESUMED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityReadingResumedSignal().Empty())
    {
      control.AccessibilityReadingResumedSignal().Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_CANCELLED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityReadingCancelledSignal().Empty())
    {
      control.AccessibilityReadingCancelledSignal().Emit();
    }
  }
  else if(0 == strcmp(actionName.c_str(), ACTION_ACCESSIBILITY_READING_STOPPED))
  {
    // if cast succeeds there is an implementation so no need to check
    if(!control.AccessibilityReadingStoppedSignal().Empty())
    {
      control.AccessibilityReadingStoppedSignal().Emit();
    }
  }
  else
  {
    ret = false;
  }

  if(ret)
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Performed Legacy AccessibilityAction: %s\n", actionName.c_str());
  }
  return ret;
}

bool DoAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Ui::Control control = Ui::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  if(!control.AccessibilityActionSignal().Empty())
  {
    return PerformAccessibilityAction(control, actionName, attributes);
  }

  // Fall back to legacy action is no ActionSignal is connected
  return PerformLegacyAccessibilityAction(control, actionName);
}

bool DoLegacyAccessibilityAction(BaseObject* object, const std::string& actionName, const Property::Map& attributes)
{
  Dali::BaseHandle handle(object);

  Ui::Control control = Ui::Control::DownCast(handle);

  DALI_ASSERT_ALWAYS(control);

  return PerformLegacyAccessibilityAction(control, actionName);
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
static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName,
                            FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool        connected(false);
  Ui::Control control = Ui::Control::DownCast(handle);
  if(control)
  {
    Internal::Control& controlImpl(Internal::GetImplementation(control));
    connected = true;

    if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_EVENT))
    {
      controlImpl.KeyEventSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_GAINED))
    {
      controlImpl.KeyInputFocusGainedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_KEY_INPUT_FOCUS_LOST))
    {
      controlImpl.KeyInputFocusLostSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_TAPPED))
    {
      controlImpl.EnableGestureDetection(GestureType::TAP);
      controlImpl.GetTapGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_PANNED))
    {
      controlImpl.EnableGestureDetection(GestureType::PAN);
      controlImpl.GetPanGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_PINCHED))
    {
      controlImpl.EnableGestureDetection(GestureType::PINCH);
      controlImpl.GetPinchGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_LONG_PRESSED))
    {
      controlImpl.EnableGestureDetection(GestureType::LONG_PRESS);
      controlImpl.GetLongPressGestureDetector().DetectedSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_GET_NAME))
    {
      control.AccessibilityGetNameSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_GET_DESCRIPTION))
    {
      control.AccessibilityGetDescriptionSignal().Connect(tracker, functor);
    }
    else if(0 == strcmp(signalName.c_str(), SIGNAL_DO_GESTURE))
    {
      control.AccessibilityDoGestureSignal().Connect(tracker, functor);
    }
  }
  return connected;
}

/**
 * Creates control through type registry
 */
BaseHandle Create()
{
  return Internal::Control::New();
}
// Setup signals and actions using the type-registry.
DALI_TYPE_REGISTRATION_BEGIN(Control, CustomActor, Create);

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
const PropertyRegistration Control::Impl::PROPERTY_1(typeRegistration,  "styleName",                      Ui::Control::Property::STYLE_NAME,                            Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_4(typeRegistration,  "keyInputFocus",                  Ui::Control::Property::KEY_INPUT_FOCUS,                       Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_5(typeRegistration,  "background",                     Ui::Control::Property::BACKGROUND,                            Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_6(typeRegistration,  "margin",                         Ui::Control::Property::MARGIN,                                Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_7(typeRegistration,  "padding",                        Ui::Control::Property::PADDING,                               Property::EXTENTS, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_11(typeRegistration, "leftFocusableActorId",           Ui::Control::Property::LEFT_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_12(typeRegistration, "rightFocusableActorId",          Ui::Control::Property::RIGHT_FOCUSABLE_ACTOR_ID,         Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_13(typeRegistration, "upFocusableActorId",             Ui::Control::Property::UP_FOCUSABLE_ACTOR_ID,            Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_14(typeRegistration, "downFocusableActorId",           Ui::Control::Property::DOWN_FOCUSABLE_ACTOR_ID,          Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_15(typeRegistration, "shadow",                         Ui::Control::Property::SHADOW,                           Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_16(typeRegistration, "accessibilityName",              Ui::Control::Property::ACCESSIBILITY_NAME,               Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_17(typeRegistration, "accessibilityDescription",       Ui::Control::Property::ACCESSIBILITY_DESCRIPTION,        Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_18(typeRegistration, "accessibilityTranslationDomain", Ui::Control::Property::ACCESSIBILITY_TRANSLATION_DOMAIN, Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_19(typeRegistration, "accessibilityRole",              Ui::Control::Property::ACCESSIBILITY_ROLE,               Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_20(typeRegistration, "accessibilityHighlightable",     Ui::Control::Property::ACCESSIBILITY_HIGHLIGHTABLE,      Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_21(typeRegistration, "accessibilityAttributes",        Ui::Control::Property::ACCESSIBILITY_ATTRIBUTES,         Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_22(typeRegistration, "dispatchKeyEvents",              Ui::Control::Property::DISPATCH_KEY_EVENTS,              Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_23(typeRegistration, "accessibilityHidden",            Ui::Control::Property::ACCESSIBILITY_HIDDEN,             Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_24(typeRegistration, "clockwiseFocusableActorId",      Ui::Control::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID,     Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_25(typeRegistration, "counterClockwiseFocusableActorId", Ui::Control::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID, Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_26(typeRegistration, "automationId",                   Ui::Control::Property::AUTOMATION_ID,                    Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_27(typeRegistration, "accessibilityValue",             Ui::Control::Property::ACCESSIBILITY_VALUE,              Property::STRING,  &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_28(typeRegistration, "accessibilityScrollable",        Ui::Control::Property::ACCESSIBILITY_SCROLLABLE,         Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_29(typeRegistration, "accessibilityStates",            Ui::Control::Property::ACCESSIBILITY_STATES,             Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_30(typeRegistration, "accessibilityIsModal",           Ui::Control::Property::ACCESSIBILITY_IS_MODAL,           Property::BOOLEAN, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_31(typeRegistration, "offScreenRendering",             Ui::Control::Property::OFFSCREEN_RENDERING,              Property::INTEGER, &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_32(typeRegistration, "innerShadow",                    Ui::Control::Property::INNER_SHADOW,                     Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);
const PropertyRegistration Control::Impl::PROPERTY_33(typeRegistration, "borderline",                     Ui::Control::Property::BORDERLINE,                       Property::MAP,     &Control::Impl::SetProperty, &Control::Impl::GetProperty);

const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_1(typeRegistration, "viewCornerRadius",       Ui::Control::Property::CORNER_RADIUS,        Property::VECTOR4, &Control::Impl::SetProperty, nullptr);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_2(typeRegistration, "viewCornerRadiusPolicy", Ui::Control::Property::CORNER_RADIUS_POLICY, Property::Value(static_cast<int>(Ui::Visual::Transform::Policy::ABSOLUTE)), &Control::Impl::SetProperty, nullptr); ///< Make animatable, for constarint-input
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_3(typeRegistration, "viewCornerSquareness",   Ui::Control::Property::CORNER_SQUARENESS,    Property::VECTOR4, &Control::Impl::SetProperty, nullptr);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_4(typeRegistration, "viewBorderlineWidth",    Ui::Control::Property::BORDERLINE_WIDTH,     Property::FLOAT,   &Control::Impl::SetProperty, nullptr);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_5(typeRegistration, "viewBorderlineColor",    Ui::Control::Property::BORDERLINE_COLOR,     Property::Value(Color::BLACK), &Control::Impl::SetProperty, nullptr);
const AnimatablePropertyRegistration Control::Impl::ANIMATABLE_PROPERTY_6(typeRegistration, "viewBorderlineOffset",   Ui::Control::Property::BORDERLINE_OFFSET,    Property::FLOAT,   &Control::Impl::SetProperty, nullptr);

// clang-format on

Control::Impl::Impl(Control& controlImpl)
: mControlImpl(controlImpl),
  mAccessibilityData(nullptr),
  mVisualData(nullptr),
  mLeftFocusableActorId(-1),
  mRightFocusableActorId(-1),
  mUpFocusableActorId(-1),
  mDownFocusableActorId(-1),
  mClockwiseFocusableActorId(-1),
  mCounterClockwiseFocusableActorId(-1),
  mStyleName(""),
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
  mOffScreenRenderingType(Ui::Control::OffScreenRenderingType::NONE),
  mInputMethodContext(),
  mIdleCallback(nullptr),
  mFlags(Control::ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
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

Control::Impl::~Impl()
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
    // Removes the callback from the callback manager in case the control is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
  }
}

void Control::Impl::InitializeVisualData()
{
  mVisualData = std::make_unique<Control::Impl::VisualData>(*this);
}

Control::Impl& Control::Impl::Get(Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

const Control::Impl& Control::Impl::Get(const Internal::Control& internalControl)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  return *internalControl.mImpl;
}

// Gesture Detection Methods
void Control::Impl::PinchDetected(Actor actor, const PinchGesture& pinch)
{
  mControlImpl.OnPinch(pinch);
}

void Control::Impl::PanDetected(Actor actor, const PanGesture& pan)
{
  mControlImpl.OnPan(pan);
}

void Control::Impl::TapDetected(Actor actor, const TapGesture& tap)
{
  mControlImpl.OnTap(tap);
}

void Control::Impl::LongPressDetected(Actor actor, const LongPressGesture& longPress)
{
  mControlImpl.OnLongPress(longPress);
}

void Control::Impl::ResourceReady()
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  // Emit signal if all enabled visuals registered by the control are ready or there are no visuals.
  if(DALI_LIKELY(mVisualData) && mVisualData->IsResourceReady())
  {
    EmitResourceReadySignal();
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, depthIndex);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled);
  }
}

void Control::Impl::RegisterVisual(Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->RegisterVisual(index, visual, enabled, depthIndex);
  }
}

void Control::Impl::UnregisterVisual(Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(index);
  }
}

Ui::Visual::Base Control::Impl::GetVisual(Property::Index index) const
{
  return Ui::Visual::Base(GetVisualImplPtr(index));
}

Ui::Internal::Visual::Base* Control::Impl::GetVisualImplPtr(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualImplPtr(index);
  }
  return nullptr;
}

bool Control::Impl::IsResourceReady() const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsResourceReady();
  }
  return true;
}

void Control::Impl::OnSceneConnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::OnSceneConnection number of registered visuals(%d)\n",
                mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mControlImpl.Self();
    mVisualData->ConnectScene(self);
  }

  if(mOffScreenRenderingImpl) // mOffScreenRenderingType != NONE
  {
    mOffScreenRenderingImpl->SetOwnerControl(Ui::Control(mControlImpl.GetOwner()));
  }
}

void Control::Impl::OnSceneDisconnection()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Control::OnSceneDisconnection number of registered visuals(%d)\n",
                mVisualData ? mVisualData->mVisuals.Size() : 0u);

  if(DALI_LIKELY(mVisualData))
  {
    Actor self = mControlImpl.Self();
    mVisualData->ClearScene(self);
  }

  if(mOffScreenRenderingImpl)
  {
    mOffScreenRenderingImpl->ClearOwnerControl();
  }
}

void Control::Impl::EnableCornerPropertiesOverridden(Ui::Visual::Base& visual, bool enable,
                                                     Dali::Constraint cornerRadiusConstraint)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableCornerPropertiesOverridden(visual, enable, cornerRadiusConstraint);
  }
}

void Control::Impl::EnableVisual(Property::Index index, bool enable)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->EnableVisual(index, enable);
  }
}

bool Control::Impl::IsVisualEnabled(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->IsVisualEnabled(index);
  }
  return false;
}

Ui::Visual::ResourceStatus Control::Impl::GetVisualResourceStatus(Property::Index index) const
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualResourceStatus(index);
  }
  return Ui::Visual::ResourceStatus::READY;
}

void Control::Impl::DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                             const Dali::Property::Value& attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoAction(visualIndex, actionId, attributes);
  }
}

void Control::Impl::DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                      const Dali::Any& attributes)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->DoActionExtension(visualIndex, actionId, attributes);
  }
}

void Control::Impl::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  Ui::Control control = Ui::Control::DownCast(BaseHandle(object));

  if(control)
  {
    Control& controlImpl(GetImplementation(control));

    switch(index)
    {
      case Ui::Control::Property::STYLE_NAME:
      {
        controlImpl.SetStyleName(value.Get<std::string>());
        break;
      }

      case Ui::Control::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mLeftFocusableActorId = focusId;
        }
      }
      break;

      case Ui::Control::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mRightFocusableActorId = focusId;
        }
      }
      break;

      case Ui::Control::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mUpFocusableActorId = focusId;
        }
      }
      break;

      case Ui::Control::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mDownFocusableActorId = focusId;
        }
      }
      break;

      case Ui::Control::Property::KEY_INPUT_FOCUS:
      {
        if(value.Get<bool>())
        {
          controlImpl.SetKeyInputFocus();
        }
        else
        {
          controlImpl.ClearKeyInputFocus();
        }
        break;
      }

      case Ui::Control::Property::BACKGROUND:
      {
        std::string          url;
        Vector4              color;
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.SetBackground(*map);
        }
        else if(value.Get(url))
        {
          if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
          {
            // don't know the size to load
            Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(url, ImageDimensions());
            if(visual)
            {
              controlImpl.mImpl->mVisualData->RegisterVisual(Ui::Control::Property::BACKGROUND, visual,
                                                             DepthIndex::BACKGROUND);
              controlImpl.mImpl->EnableCornerPropertiesOverridden(visual, true);
            }
          }
        }
        else if(value.Get(color))
        {
          controlImpl.SetBackgroundColor(color);
        }
        else
        {
          // The background is an empty property map, so we should clear the background
          controlImpl.ClearBackground();
        }
        break;
      }

      case Ui::Control::Property::MARGIN:
      {
        Extents margin;
        if(value.Get(margin))
        {
          controlImpl.mImpl->SetMargin(margin);
        }
        break;
      }

      case Ui::Control::Property::PADDING:
      {
        Extents padding;
        if(value.Get(padding))
        {
          controlImpl.mImpl->SetPadding(padding);
        }
        break;
      }

      case Ui::Control::Property::SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the shadow
          controlImpl.mImpl->ClearShadow();
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_NAME:
      {
        std::string name;
        if(value.Get(name))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !name.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.name = std::move(name);
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_DESCRIPTION:
      {
        std::string text;
        if(value.Get(text))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !text.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.description = std::move(text);
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_ROLE:
      {
        int32_t role;
        if(value.Get(role))
        {
          controlImpl.mImpl->mAccessibilityRole = role;
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        bool highlightable;
        if(value.Get(highlightable))
        {
          controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isHighlightable =
            highlightable ? TriStateProperty::TRUE : TriStateProperty::FALSE;
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const Property::Map* map = value.GetMap();
        if(map)
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !map->Empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.extraAttributes = *map;
          }
        }
        break;
      }

      case Ui::Control::Property::DISPATCH_KEY_EVENTS:
      {
        bool dispatch;
        if(value.Get(dispatch))
        {
          controlImpl.mImpl->mDispatchKeyEvents = dispatch;
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_HIDDEN:
      {
        bool hidden;
        if(value.Get(hidden))
        {
          const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
          const bool  originalHidden =
            DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
          if(originalHidden != hidden)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isHidden = hidden;

            auto accessible = controlImpl.GetAccessibleObject();
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
      case Ui::Control::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mClockwiseFocusableActorId = focusId;
        }
        break;
      }
      case Ui::Control::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        int focusId;
        if(value.Get(focusId))
        {
          controlImpl.mImpl->mCounterClockwiseFocusableActorId = focusId;
        }
        break;
      }

      case Ui::Control::Property::AUTOMATION_ID:
      {
        std::string automationId;
        if(value.Get(automationId))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !automationId.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.automationId =
              std::move(automationId);
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_VALUE:
      {
        std::string accessibilityValue;
        if(value.Get(accessibilityValue))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || !accessibilityValue.empty())
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.value = std::move(accessibilityValue);
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_SCROLLABLE:
      {
        bool isScrollable;
        if(value.Get(isScrollable))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || isScrollable)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isScrollable = isScrollable;
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_STATES:
      {
        int32_t states;
        if(value.Get(states))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) ||
             states != static_cast<int32_t>(AccessibilityData::GetDefaultControlAccessibilityStates().GetRawData32()))
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.states =
              AccessibilityStates{static_cast<uint32_t>(states)};
          }
        }
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_IS_MODAL:
      {
        bool isModal;
        if(value.Get(isModal))
        {
          if(DALI_LIKELY(controlImpl.mImpl->GetAccessibilityData()) || isModal)
          {
            controlImpl.mImpl->GetOrCreateAccessibilityData().mAccessibilityProps.isModal = isModal;
          }
        }
        break;
      }

      case Ui::Control::Property::OFFSCREEN_RENDERING:
      {
        int32_t offscreenRenderingType;
        if(value.Get(offscreenRenderingType))
        {
          controlImpl.mImpl->SetOffScreenRendering(offscreenRenderingType);
        }
        break;
      }

      case Ui::Control::Property::INNER_SHADOW:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetInnerShadow(*map);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          controlImpl.mImpl->ClearInnerShadow();
        }
        break;
      }

      case Ui::Control::Property::BORDERLINE:
      {
        const Property::Map* map = value.GetMap();
        if(map && !map->Empty())
        {
          controlImpl.mImpl->SetBorderline(*map, true);
        }
        else
        {
          // The shadow is an empty property map, so we should clear the inner shadow
          controlImpl.mImpl->ClearBorderline();
        }
        break;
      }

      case Ui::Control::Property::CORNER_RADIUS:
      {
        float radiusFloat = 0.0f;
        if(value.Get(radiusFloat))
        {
          control.SetProperty(Ui::Control::Property::CORNER_RADIUS,
                              Vector4(radiusFloat, radiusFloat, radiusFloat, radiusFloat));
          break;
        }

        Vector4 radius;
        if(value.Get(radius))
        {
          if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
          {
            controlImpl.mImpl->mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::CORNER_RADIUS,
                                                                            false);
          }
          controlImpl.mImpl->UpdateCornerRadius();
        }
        break;
      }

      case Ui::Control::Property::CORNER_RADIUS_POLICY:
      {
        int policy;
        if(value.Get(policy))
        {
          if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
          {
            controlImpl.mImpl->mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::CORNER_RADIUS_POLICY,
                                                                            false);
          }
          controlImpl.mImpl->UpdateCornerRadius();
        }
        break;
      }

      case Ui::Control::Property::CORNER_SQUARENESS:
      {
        float squarenessFloat = 0.0f;
        if(value.Get(squarenessFloat))
        {
          control.SetProperty(Ui::Control::Property::CORNER_SQUARENESS,
                              Vector4(squarenessFloat, squarenessFloat, squarenessFloat, squarenessFloat));
          break;
        }

        Vector4 squareness;
        if(value.Get(squareness))
        {
          if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
          {
            controlImpl.mImpl->mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::CORNER_SQUARENESS,
                                                                            false);
          }
          controlImpl.mImpl->UpdateCornerRadius();
        }
        break;
      }

      case Ui::Control::Property::BORDERLINE_WIDTH:
      {
        float width;
        if(value.Get(width))
        {
          controlImpl.mImpl->UpdateBorderline();
        }
        break;
      }

      case Ui::Control::Property::BORDERLINE_COLOR:
      {
        Vector4 color;
        if(value.Get(color))
        {
          controlImpl.mImpl->UpdateBorderline();
        }
        break;
      }

      case Ui::Control::Property::BORDERLINE_OFFSET:
      {
        float offset;
        if(value.Get(offset))
        {
          controlImpl.mImpl->UpdateBorderline();
        }
        break;
      }
    }
  }
}

Property::Value Control::Impl::GetProperty(BaseObject* object, Property::Index index)
{
  DALI_ASSERT_ALWAYS(Stage::IsCoreThread() && "Core is not installed. Might call this API from worker thread?");

  Property::Value value;

  Ui::Control control = Ui::Control::DownCast(BaseHandle(object));

  if(control)
  {
    Control& controlImpl(GetImplementation(control));

    switch(index)
    {
      case Ui::Control::Property::STYLE_NAME:
      {
        value = controlImpl.GetStyleName();
        break;
      }

      case Ui::Control::Property::LEFT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mLeftFocusableActorId;
        break;
      }

      case Ui::Control::Property::RIGHT_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mRightFocusableActorId;
        break;
      }

      case Ui::Control::Property::UP_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mUpFocusableActorId;
        break;
      }

      case Ui::Control::Property::DOWN_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mDownFocusableActorId;
        break;
      }

      case Ui::Control::Property::KEY_INPUT_FOCUS:
      {
        value = controlImpl.HasKeyInputFocus();
        break;
      }

      case Ui::Control::Property::BACKGROUND:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          const Ui::Internal::Visual::Base* visualImplPtr =
            controlImpl.mImpl->mVisualData->GetVisualImplPtr(Ui::Control::Property::BACKGROUND);
          if(visualImplPtr)
          {
            visualImplPtr->CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::Control::Property::MARGIN:
      {
        value = controlImpl.mImpl->GetMargin();
        break;
      }

      case Ui::Control::Property::PADDING:
      {
        value = controlImpl.mImpl->GetPadding();
        break;
      }

      case Ui::Control::Property::SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Ui::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Ui::Control::Property::SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_NAME:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.name : "";
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_DESCRIPTION:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.description : "";
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_ROLE:
      {
        value = controlImpl.mImpl->mAccessibilityRole;
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_HIGHLIGHTABLE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = (DALI_LIKELY(accessibilityData) &&
                 accessibilityData->mAccessibilityProps.isHighlightable == TriStateProperty::TRUE)
                                          ? true
                                          : false;
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_ATTRIBUTES:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value =
          DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.extraAttributes : Property::Map();
        break;
      }

      case Ui::Control::Property::DISPATCH_KEY_EVENTS:
      {
        value = controlImpl.mImpl->mDispatchKeyEvents;
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_HIDDEN:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isHidden : false;
        break;
      }

      case Ui::Control::Property::CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mClockwiseFocusableActorId;
        break;
      }

      case Ui::Control::Property::COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID:
      {
        value = controlImpl.mImpl->mCounterClockwiseFocusableActorId;
        break;
      }

      case Ui::Control::Property::AUTOMATION_ID:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.automationId : "";
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_VALUE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.value : "";
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_SCROLLABLE:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isScrollable : false;
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_STATES:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = static_cast<int32_t>((DALI_LIKELY(accessibilityData)
                                                                ? accessibilityData->mAccessibilityProps.states
                                                                : AccessibilityData::GetDefaultControlAccessibilityStates())
                                                               .GetRawData32());
        break;
      }

      case Ui::Control::Property::ACCESSIBILITY_IS_MODAL:
      {
        const auto* accessibilityData = controlImpl.mImpl->GetAccessibilityData();
        value                         = DALI_LIKELY(accessibilityData) ? accessibilityData->mAccessibilityProps.isModal : false;
        break;
      }

      case Ui::Control::Property::OFFSCREEN_RENDERING:
      {
        value = controlImpl.mImpl->mOffScreenRenderingType;
        break;
      }

      case Ui::Control::Property::INNER_SHADOW:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Ui::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Ui::Control::Property::INNER_SHADOW);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::Control::Property::BORDERLINE:
      {
        Property::Map map;

        if(DALI_LIKELY(controlImpl.mImpl->mVisualData))
        {
          Ui::Visual::Base visual = controlImpl.mImpl->mVisualData->GetVisual(Ui::Control::Property::BORDERLINE);
          if(visual)
          {
            visual.CreatePropertyMap(map);
          }
        }

        value = map;
        break;
      }

      case Ui::Control::Property::CORNER_RADIUS:
      case Ui::Control::Property::CORNER_RADIUS_POLICY:
      case Ui::Control::Property::CORNER_SQUARENESS:
      case Ui::Control::Property::BORDERLINE_WIDTH:
      case Ui::Control::Property::BORDERLINE_COLOR:
      case Ui::Control::Property::BORDERLINE_OFFSET:
      {
        // Do not return property for animatable custom properties.
        // Actual variables of each property will be registered at custom area.
        break;
      }
    }
  }

  return value;
}

void Control::Impl::SetMargin(Extents margin)
{
  mControlImpl.mImpl->mMargin = margin;

  // Trigger a size negotiation request that may be needed when setting a margin.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetMargin() const
{
  return mControlImpl.mImpl->mMargin;
}

void Control::Impl::SetPadding(Extents padding)
{
  mControlImpl.mImpl->mPadding = padding;

  // Trigger a size negotiation request that may be needed when setting a padding.
  mControlImpl.RelayoutRequest();
}

Extents Control::Impl::GetPadding() const
{
  return mControlImpl.mImpl->mPadding;
}

Ui::Control::VisualEventSignalType& Control::Impl::VisualEventSignal()
{
  DALI_ASSERT_ALWAYS(mVisualData && "Visual Disabled control cannot use VisualEventSignal!!");
  return mVisualData->VisualEventSignal();
}

void Control::Impl::SetInputMethodContext(InputMethodContext& inputMethodContext)
{
  mInputMethodContext = inputMethodContext;
}

bool Control::Impl::FilterKeyEvent(const KeyEvent& event)
{
  bool consumed(false);

  if(mInputMethodContext)
  {
    consumed = mInputMethodContext.FilterEventKey(event);
  }
  return consumed;
}

Control::Impl::AccessibilityData& Control::Impl::GetOrCreateAccessibilityData()
{
  if(DALI_UNLIKELY(!mAccessibilityData))
  {
    // Create only 1 times.
    mAccessibilityData = std::make_unique<AccessibilityData>(mControlImpl);
  }
  DALI_ASSERT_DEBUG(mAccessibilityData && "AccessibilityData not created!");
  return *mAccessibilityData;
}

Control::Impl::AccessibilityData* Control::Impl::GetAccessibilityData() const
{
  return mAccessibilityData.get();
}

void Control::Impl::AppendAccessibilityAttribute(const std::string& key, const std::string value)
{
  GetOrCreateAccessibilityData().AppendAccessibilityAttribute(key, value);
}

void Control::Impl::RemoveAccessibilityAttribute(const std::string& key)
{
  auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->RemoveAccessibilityAttribute(key);
  }
}

void Control::Impl::ClearAccessibilityAttributes()
{
  auto* accessibilityData = GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    accessibilityData->ClearAccessibilityAttributes();
  }
}

void Control::Impl::SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types)
{
  GetOrCreateAccessibilityData().SetAccessibilityReadingInfoType(types);
}

Dali::Accessibility::ReadingInfoTypes Control::Impl::GetAccessibilityReadingInfoType() const
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

bool Control::Impl::IsAccessibleCreated() const
{
  auto bridge = Accessibility::Bridge::GetCurrentBridge();
  return DALI_LIKELY(bridge) ? !!bridge->GetAccessible(mControlImpl.Self()) : false;
}

void Control::Impl::EnableCreateAccessible(bool enable)
{
  mAccessibleCreatable = enable;
}

bool Control::Impl::IsCreateAccessibleEnabled() const
{
  return mAccessibleCreatable;
}

void Control::Impl::EmitAccessibilityStateChanged(Accessibility::State state, int newValue)
{
  Dali::CustomActor handle(mControlImpl.GetOwner());
  auto              bridge = Accessibility::Bridge::GetCurrentBridge();
  if(DALI_LIKELY(bridge))
  {
    if(state == Accessibility::State::SHOWING)
    {
      bool isModal = ControlAccessible::IsModal(handle);
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

void Control::Impl::ApplyFittingMode(const Vector2& size)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ApplyFittingMode(size);
  }
}

void Control::Impl::SetShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("shadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::Control::Property::SHADOW, visual, DepthIndex::BACKGROUND_EFFECT);
      EnableCornerPropertiesOverridden(visual, true);

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::Control::Property::SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

void Control::Impl::SetInnerShadow(const Property::Map& map)
{
  if(DALI_LIKELY(mVisualData))
  {
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("innerShadow");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::Control::Property::INNER_SHADOW, visual, INNER_SHADOW_DEPTH_INDEX);

      Ui::Internal::Visual::Base& visualImpl = Ui::GetImplementation(visual);

      auto visualCornerRadiusProperty = visualImpl.GetPropertyObject(DevelVisual::Property::CORNER_RADIUS, false);
      auto visualBorderlineProperty   = visualImpl.GetPropertyObject(DevelVisual::Property::BORDERLINE_WIDTH);

      if(DALI_LIKELY(visualCornerRadiusProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualCornerRadiusProperty.object) &&
         DALI_LIKELY(visualBorderlineProperty.propertyIndex != Property::INVALID_INDEX &&
                     visualBorderlineProperty.object))
      {
        Dali::CustomActor handle(mControlImpl.GetOwner());

        auto innerShadowCornerRadiusConstraint =
          Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex,
                                   InnerShadowCornerRadiusConstraint);
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::CORNER_RADIUS));
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::CORNER_RADIUS_POLICY));
        innerShadowCornerRadiusConstraint.AddSource(Source(handle, Dali::Actor::Property::SIZE));
        innerShadowCornerRadiusConstraint.AddSource(LocalSource(Dali::VisualRenderer::Property::EXTRA_SIZE));
        innerShadowCornerRadiusConstraint.AddSource(
          LocalSource(Dali::DecoratedVisualRenderer::Property::BORDERLINE_WIDTH));

        Dali::Integration::ConstraintSetInternalTag(innerShadowCornerRadiusConstraint,
                                                    INNER_SHADOW_CORNER_RADIUS_CONSTRAINT_TAG);

        EnableCornerPropertiesOverridden(visual, true, innerShadowCornerRadiusConstraint);
      }

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearInnerShadow()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::Control::Property::INNER_SHADOW);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

void Control::Impl::SetBorderline(const Property::Map& map, bool forciblyCreate)
{
  if(DALI_LIKELY(mVisualData))
  {
    if(!forciblyCreate)
    {
      Ui::Internal::Visual::Base* previousVisualImplPtr =
        mVisualData->GetVisualImplPtr(Ui::Control::Property::BORDERLINE);
      if(previousVisualImplPtr)
      {
        previousVisualImplPtr->DoAction(Ui::DevelVisual::Action::UPDATE_PROPERTY, map);

        // Trigger borderline relative constraints once
        mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::BORDERLINE_WIDTH, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::BORDERLINE_COLOR, false);
        mVisualData->NotifyConstraintPropertyChanged(Ui::Control::Property::BORDERLINE_OFFSET, false);
        return;
      }
    }
    Ui::Visual::Base visual = Ui::VisualFactory::Get().CreateVisual(map);
    visual.SetName("borderline");

    if(visual)
    {
      mVisualData->RegisterVisual(Ui::Control::Property::BORDERLINE, visual, BORDERLINE_DEPTH_INDEX);

      // Create constraint only if we set Borderline property as DevelControl::BORDERLINE_XXX.
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
          Dali::CustomActor handle(mControlImpl.GetOwner());

          auto borderlineCornerRadiusConstraint =
            Constraint::New<Vector4>(visualCornerRadiusProperty.object, visualCornerRadiusProperty.propertyIndex,
                                     BorderlineCornerRadiusConstraint);
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::CORNER_RADIUS));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::CORNER_RADIUS_POLICY));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Dali::Actor::Property::SIZE));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::BORDERLINE_WIDTH));
          borderlineCornerRadiusConstraint.AddSource(Source(handle, Ui::Control::Property::BORDERLINE_OFFSET));

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
            borderlineWidthConstraint.AddSource(Source(handle, Ui::Control::Property::BORDERLINE_WIDTH));
            auto borderlineColorConstraint = Constraint::New<Vector4>(
              visualBorderlineColorProperty.object, visualBorderlineColorProperty.propertyIndex, EqualToConstraint());
            borderlineColorConstraint.AddSource(Source(handle, Ui::Control::Property::BORDERLINE_COLOR));
            auto borderlineOffsetConstraint =
              Constraint::New<float>(visualBorderlineOffsetProperty.object,
                                     visualBorderlineOffsetProperty.propertyIndex, EqualToConstraint());
            borderlineOffsetConstraint.AddSource(Source(handle, Ui::Control::Property::BORDERLINE_OFFSET));

            Dali::Integration::ConstraintSetInternalTag(borderlineWidthConstraint, BORDERLINE_WIDTH_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineColorConstraint, BORDERLINE_COLOR_CONSTRAINT_TAG);
            Dali::Integration::ConstraintSetInternalTag(borderlineOffsetConstraint, BORDERLINE_OFFSET_CONSTRAINT_TAG);

            borderlineWidthConstraint.Apply();
            borderlineColorConstraint.Apply();
            borderlineOffsetConstraint.Apply();

            visualImpl.AddConstraintFeature(borderlineWidthConstraint, {Ui::Control::Property::BORDERLINE_WIDTH});
            visualImpl.AddConstraintFeature(borderlineColorConstraint, {Ui::Control::Property::BORDERLINE_COLOR});
            visualImpl.AddConstraintFeature(borderlineOffsetConstraint, {Ui::Control::Property::BORDERLINE_OFFSET});
          }

          EnableCornerPropertiesOverridden(visual, true, borderlineCornerRadiusConstraint);
        }
      }

      mControlImpl.RelayoutRequest();
    }
  }
}

void Control::Impl::ClearBorderline()
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UnregisterVisual(Ui::Control::Property::BORDERLINE);

    // Trigger a size negotiation request that may be needed when unregistering a visual.
    mControlImpl.RelayoutRequest();
  }
}

Dali::Property Control::Impl::GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey)
{
  if(DALI_LIKELY(mVisualData))
  {
    return mVisualData->GetVisualProperty(index, visualPropertyKey);
  }
  Dali::Handle handle;
  return Dali::Property(handle, Property::INVALID_INDEX);
}

void Control::Impl::UpdateVisualProperties(
  const std::vector<std::pair<Dali::Property::Index, Dali::Property::Map>>& properties)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->UpdateVisualProperties(properties);
  }
}

void Control::Impl::EmitResourceReadySignal()
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
      Dali::Ui::Control handle(mControlImpl.GetOwner());
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
          mIdleCallback = MakeCallback(this, &Control::Impl::OnIdleCallback);
          if(DALI_UNLIKELY(!Adaptor::Get().AddIdle(mIdleCallback, true)))
          {
            DALI_LOG_ERROR("Fail to add idle callback for control resource ready. Skip this callback.\n");
            mIdleCallback           = nullptr;
            mIdleCallbackRegistered = false;
          }
        }
      }
    }
  }
}

bool Control::Impl::OnIdleCallback()
{
  // Reset the flag
  mIdleCallbackRegistered = false;

  // A visual is ready so control may need relayouting if staged
  if(mControlImpl.Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    mControlImpl.RelayoutRequest();
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

std::shared_ptr<Ui::ControlAccessible> Control::Impl::GetAccessibleObject()
{
  return GetOrCreateAccessibilityData().GetAccessibleObject();
}

void Control::Impl::RegisterProcessorOnce()
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

void Control::Impl::SetOffScreenRendering(int32_t offScreenRenderingType)
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

  Ui::Control::OffScreenRenderingType newType =
    static_cast<Ui::Control::OffScreenRenderingType>(offScreenRenderingType);

  Dali::Ui::Control handle(mControlImpl.GetOwner());

  if(newType == Ui::Control::OffScreenRenderingType::NONE)
  {
    if(mOffScreenRenderingImpl)
    {
      auto tempOffscreenRenderingImpl = std::move(mOffScreenRenderingImpl);
      tempOffscreenRenderingImpl->ClearOwnerControl();

      if(DALI_LIKELY(mVisualData))
      {
        mVisualData->OffscreenRenderingEnabled(false);
      }
    }
  }
  else if(mOffScreenRenderingType == Ui::Control::OffScreenRenderingType::NONE)
  {
    mOffScreenRenderingImpl = std::make_unique<OffScreenRenderingImpl>(newType);
    mOffScreenRenderingImpl->SetOwnerControl(handle);

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

void Control::Impl::UpdateCornerRadius()
{
  // TODO : Need to make constriant for RenderEffect corner radius update
  if(mRenderEffect || mOffScreenRenderingImpl)
  {
    Actor self = mControlImpl.Self();

    Property::Map map;
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS,
               self.GetProperty<Vector4>(Ui::Control::Property::CORNER_RADIUS));
    map.Insert(Ui::DevelVisual::Property::CORNER_RADIUS_POLICY,
               self.GetProperty<int>(Ui::Control::Property::CORNER_RADIUS_POLICY));
    map.Insert(Ui::DevelVisual::Property::CORNER_SQUARENESS,
               self.GetProperty<Vector4>(Ui::Control::Property::CORNER_SQUARENESS));

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

void Control::Impl::UpdateBorderline()
{
  Actor self = mControlImpl.Self();

  Property::Map map;
  map.Insert(Ui::Visual::Property::TYPE, Ui::Visual::Type::COLOR);
  map.Insert(Ui::ColorVisual::Property::MIX_COLOR, Color::TRANSPARENT);
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_WIDTH,
             self.GetProperty<float>(Ui::Control::Property::BORDERLINE_WIDTH));
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_COLOR,
             self.GetProperty<Vector4>(Ui::Control::Property::BORDERLINE_COLOR));
  map.Insert(Ui::DevelVisual::Property::BORDERLINE_OFFSET,
             self.GetProperty<float>(Ui::Control::Property::BORDERLINE_OFFSET));

  SetBorderline(map, false);
}

void Control::Impl::CreateAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    if(index == Ui::Control::Property::BORDERLINE_WIDTH || index == Ui::Control::Property::BORDERLINE_COLOR ||
       index == Ui::Control::Property::BORDERLINE_OFFSET)
    {
      Ui::Internal::Visual::Base* previousVisualImplPtr =
        mVisualData->GetVisualImplPtr(Ui::Control::Property::BORDERLINE);
      if(!previousVisualImplPtr)
      {
        // Create visual and constraint for borderline first.
        UpdateBorderline();
      }
    }
    mVisualData->CreateAnimationConstraints(animationObject, index);
  }
}

void Control::Impl::ClearAnimationConstraints(const Dali::BaseObject& animationObject, Property::Index index)
{
  if(DALI_LIKELY(mVisualData))
  {
    mVisualData->ClearAnimationConstraints(animationObject, index);
  }
}

void Control::Impl::Process(bool postProcessor)
{
  if(DALI_LIKELY(mVisualData))
  {
    // Call ApplyFittingMode
    mVisualData->ApplyFittingMode(mSize);
  }
  mProcessorRegistered = false;
}

void Control::OnApplyDefaultStyle()
{
  // Default implementation does nothing
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
