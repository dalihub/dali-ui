#ifndef DALI_UI_CONTROL_DEVEL_H
#define DALI_UI_CONTROL_DEVEL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/controls/control-accessible.h>
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/controls/control.h>

namespace Dali
{
namespace UI
{
namespace Visual
{
class Base;
}

namespace DevelControl
{
/// @brief AccessibilityActivate signal type.
typedef Signal<void()> AccessibilityActivateSignalType;

/// @brief AccessibilityReadingSkipped signal type.
typedef Signal<void()> AccessibilityReadingSkippedSignalType;

/// @brief AccessibilityReadingPaused signal type.
typedef Signal<void()> AccessibilityReadingPausedSignalType;

/// @brief AccessibilityReadingResumed signal type.
typedef Signal<void()> AccessibilityReadingResumedSignalType;

/// @brief AccessibilityReadingCancelled signal type.
typedef Signal<void()> AccessibilityReadingCancelledSignalType;

/// @brief AccessibilityReadingStopped signal type.
typedef Signal<void()> AccessibilityReadingStoppedSignalType;

/// @brief AccessibilityGetName signal type.
typedef Signal<void(std::string&)> AccessibilityGetNameSignalType;

/// @brief AccessibilityGetDescription signal type.
typedef Signal<void(std::string&)> AccessibilityGetDescriptionSignalType;

/// @brief AccessibilityDoGesture signal type.
typedef Signal<void(std::pair<Dali::Accessibility::GestureInfo, bool>&)> AccessibilityDoGestureSignalType;

/// @brief AccessibilityAction signal type.
typedef Signal<bool(const Dali::Accessibility::ActionInfo&)> AccessibilityActionSignalType;

/// @brief AccessibilityHighlighted signal type.
/// @param bool highlighted true if control is highlighted, false if highlight is removed.
typedef Signal<void(bool)> AccessibilityHighlightedSignalType;

enum OffScreenRenderingType
{
  NONE,
  REFRESH_ONCE,
  REFRESH_ALWAYS
};
constexpr unsigned int OffScreenRenderingTypeCount = 3u;

enum State
{
  NORMAL,
  FOCUSED,
  DISABLED
};

namespace Property
{
enum
{
  STYLE_NAME = Control::Property::STYLE_NAME,
  KEY_INPUT_FOCUS = Control::Property::KEY_INPUT_FOCUS,
  BACKGROUND = Control::Property::BACKGROUND,
  MARGIN = Control::Property::MARGIN,
  PADDING = Control::Property::PADDING,

  /**
   * @brief The current state of the control.
   * @details Name "state", type DevelControl::State ( Property::INTEGER ) or Property::STRING
   *
   * @see DevelControl::State
   */
  STATE = PADDING + 1,

  /**
   * @brief The current sub state of the control.
   * @details Name "subState", type Property::INTEGER or Property::STRING. The enumeration used is dependent on the
   * derived control.
   *
   * @see DevelControl::State
   */
  SUB_STATE = PADDING + 2,

  /**
   * @brief The actor ID of the left focusable control.
   * @details Name "leftFocusableActorId", type Property::INTEGER.
   *
   */
  LEFT_FOCUSABLE_ACTOR_ID = PADDING + 3,

  /**
   * @brief The actor ID of the right focusable control.
   * @details Name "rightFocusableActorId", type Property::INTEGER.
   *
   */
  RIGHT_FOCUSABLE_ACTOR_ID = PADDING + 4,

  /**
   * @brief The actor ID of the up focusable control.
   * @details Name "upFocusableActorId", type Property::INTEGER.
   *
   */
  UP_FOCUSABLE_ACTOR_ID = PADDING + 5,

  /**
   * @brief The actor ID of the down focusable control.
   * @details Name "downFocusableActorId", type Property::INTEGER.
   *
   */
  DOWN_FOCUSABLE_ACTOR_ID = PADDING + 6,

  /**
   * @brief The shadow of the control.
   * @details Name "shadow", type Property::MAP.
   */
  SHADOW = PADDING + 7,

  /**
   * @brief The name of object visible in accessibility tree.
   * @details Name "accessibilityName", type Property::STRING.
   */
  ACCESSIBILITY_NAME,

  /**
   * @brief The description of object visible in accessibility tree.
   * @details Name "accessibilityDescription", type Property::STRING.
   */
  ACCESSIBILITY_DESCRIPTION,

  /**
   * @brief Deprecated. Current translation domain for accessibility clients.
   * @details Name "accessibilityTranslationDomain", type Property::STRING.
   */
  ACCESSIBILITY_TRANSLATION_DOMAIN,

  /**
   * @brief Role being performed in accessibility hierarchy.
   * @details Name "accessibilityRole", type Property::INTEGER.
   * @note It gets integer value of AccessibilityRole enum then interprets to Dali::Accessibility::Role when requested
   * by AT-SPI. Note that setting Dali::Accessibility::Role value is still accepted for backward compatibility but not
   * preferred.
   * @see Dali::Accessibility::Role
   */
  ACCESSIBILITY_ROLE,

  /**
   * @brief Mark of able to highlight object.
   * @details Name "accessibilityHighlightable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_HIGHLIGHTABLE,

  /**
   * @brief Set of accessibility attributes describing object in accessibility hierarchy
   * @details Name "accessibilityAttributes", type Property::MAP
   */
  ACCESSIBILITY_ATTRIBUTES,

  /**
   * @brief Whether a Control and its descendants can emit key signals.
   * @details Name "dispatchKeyEvents", type Property::BOOLEAN
   * @note If a control's dispatchKeyEvents is set to false, then it's children will not emit a key event signal either.
   */
  DISPATCH_KEY_EVENTS,

  /**
   * @brief Marks the object as invisible to AT-SPI clients.
   * @details Name "accessibilityHidden", type Property::BOOLEAN.
   * @note The representative Accessible object will not appear in the AT-SPI tree.
   */
  ACCESSIBILITY_HIDDEN,

  /**
   * @brief The actor ID of the clockwise focusable control.
   * @details Name "clockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief The actor ID of the conter-clockwise focusable control.
   * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
   *
   */
  COUNTER_CLOCKWISE_FOCUSABLE_ACTOR_ID,

  /**
   * @brief Identifier that allows the automation framework to find and interact with this element.
   * @details Name "automationId", type Property::STRING.
   * @note This is a string identifier (compared to @c Actor::Property::ID which is an integer).
   * It will also appear in the AT-SPI tree under the key "automationId".
   */
  AUTOMATION_ID,

  /**
   * @brief The accessibility value represented by the control. For example, "60%" for a slider object.
   * @details Name "accessibilityValue", type Property::STRING.
   */
  ACCESSIBILITY_VALUE,

  /**
   * @brief Indicates the accessibility services treat the control as scrollable.
   * @details Name "accessibilityScrollable", type Property::BOOLEAN.
   */
  ACCESSIBILITY_SCROLLABLE,

  /**
   * @brief Bitset integer of AccessibilityState which describes the current state of a control.
   * @details Name "accessibilityStates", type Property::INTEGER.
   */
  ACCESSIBILITY_STATES,

  /**
   * @brief Indicates the accessibility services treat the controla as modal.
   * @details Name "accessibilityIsModal", type Property::BOOLEAN.
   */
  ACCESSIBILITY_IS_MODAL,

  /**
   * @brief Whether to draw on offscreen of not.
   * @details Name "offscreenRendering", type Property::INTEGER.
   * @note Default is false.
   */
  OFFSCREEN_RENDERING,

  /**
   * @brief The inner shadow of the control. The visual will use DepthIndex::Ranges::DECORATION - 2
   * @details Name "innerShadow", type Property::MAP.
   */
  INNER_SHADOW,

  /**
   * @brief The inset borderline of the control. The visual will use DepthIndex::Ranges::DECORATION - 1
   * @details Name "borderline", type Property::MAP.
   */
  BORDERLINE,

  /**
   * @brief The radius for the rounded corners of the control.
   * @details Name "viewCornerRadius", type Property::VECTOR4 or Property::FLOAT
   * @note By default, it is Vector::ZERO.
   * @note Applies to specific visuals inside the control.
   * @note Only Property::Vector4 can be animated.
   * @see Dali::UI::DevelVisual::Property::Type::CORNER_RADIUS
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,

  /**
   * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute (in
   * world units).
   * @details Name "viewCornerRadiusPolicy", type Property::INTEGER.
   * @see Policy::Type
   * @see Dali::UI::DevelVisual::Property::Type::CORNER_RADIUS_POLICY
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  CORNER_RADIUS_POLICY,

  /**
   * @brief The squareness for the rounded corners of the control.
   * @details Name "viewCornerSquareness", type Property::VECTOR4 or Property::FLOAT
   * @note By default, it is Vector::ZERO.
   * @note Applies to specific visuals inside the control.
   * @note Only Property::Vector4 can be animated.
   * @see Dali::UI::DevelVisual::Property::Type::CORNER_SQUARENESS
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  CORNER_SQUARENESS,

  /**
   * @brief The width for the borderline of the control. It will update borderline visual
   * @details Name "viewBorderlineWidth", type Property::FLOAT.
   * @see Dali::UI::DevelVisual::Property::Type::BORDERLINE_WIDTH
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  BORDERLINE_WIDTH,

  /**
   * @brief The color for the borderline of the control. It will update borderline visual
   * @details Name "viewBorderlineColor", type Property::VECTOR4
   * @see Dali::UI::DevelVisual::Property::Type::BORDERLINE_COLOR
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  BORDERLINE_COLOR,

  /**
   * @brief The offset for the borderline of the control. It will update borderline visual
   * @details Name "viewBorderlineOffset", type Property::FLOAT.
   * @see Dali::UI::DevelVisual::Property::Type::BORDERLINE_OFFSET
   * @note It will not create UniformMap internally. So this property don't be used at Render phase.
   */
  BORDERLINE_OFFSET,
};

} // namespace Property

/**
 * @brief Register a visual by Property Index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 *
 * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to
 * connect/disconnect registered visual to stage. Use below API with enabled set to false if derived class wishes to
 * control when visual is staged.
 * @note depth-index be used to Dali::UI::DepthIndex::Ranges::AUTO_INDEX.
 */
DALI_UI_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, UI::Visual::Base& visual);

/**
 * @brief Register a visual by Property Index with a depth index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to
 * DepthIndex::Ranges::AUTO_INDEX, the actual depth-index of visual will be determind automatically (Use previous
 * visuals depth-index, or placed on top of all other visuals.) Otherwise, the visual's depth-index is set to clamped
 * value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
 *
 * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to
 * connect/disconnect registered visual to stage. Use below API with enabled set to false if derived class wishes to
 * control when visual is staged.
 *
 * @see Visual::Base::GetDepthIndex()
 * @see Visual::Base::SetDepthIndex()
 */
DALI_UI_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, UI::Visual::Base& visual,
                                int depthIndex);

/**
 * @brief Register a visual by Property Index with the option of enabling/disabling it.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 *
 * @note depth-index be used to Dali::UI::DepthIndex::Ranges::AUTO_INDEX.
 *
 * @see EnableVisual()
 */
DALI_UI_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, UI::Visual::Base& visual,
                                bool enabled);

/**
 * @brief Register a visual by Property Index with a depth index with the option of enabling/disabling it.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 * @param[in] visual The visual to register
 * @param[in] enabled false if derived class wants to control when visual is set on stage.
 * @param[in] depthIndex The visual's depth-index is set to this. If the depth-index is set to
 * DepthIndex::Ranges::AUTO_INDEX, the actual depth-index of visual will be determind automatically (Use previous
 * visuals depth-index, or placed on top of all other visuals.) Otherwise, the visual's depth-index is set to clamped
 * value, between DepthIndex::Ranges::MINIMUM_DEPTH_INDEX and DepthIndex::Ranges::MAXIMUM_DEPTH_INDEX.
 *
 * @see EnableVisual()
 * @see Visual::Base::GetDepthIndex()
 * @see Visual::Base::SetDepthIndex()
 */
DALI_UI_API void RegisterVisual(Internal::Control& control, Dali::Property::Index index, UI::Visual::Base& visual,
                                bool enabled, int depthIndex);

/**
 * @brief Erase the entry matching the given index from the list of registered visuals
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual, used to reference visual
 */
DALI_UI_API void UnregisterVisual(Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Retrieve the visual associated with the given property index.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual.
 * @return The registered visual if exist, otherwise empty handle.
 * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference
 * count.
 */
DALI_UI_API UI::Visual::Base GetVisual(const Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Sets the given visual to be displayed or not when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @param[in] enable flag to set enabled or disabled.
 */
DALI_UI_API void EnableVisual(Internal::Control& control, Dali::Property::Index index, bool enable);

/**
 * @brief Queries if the given visual is to be displayed when parent staged.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual
 * @return bool whether visual is enabled or not
 */
DALI_UI_API bool IsVisualEnabled(const Internal::Control& control, Dali::Property::Index index);

/**
 * @brief Perform an action on a visual registered to this control.
 *
 * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
 *
 * @param[in] control The control.
 * @param[in] visualIndex The Property index of the visual.
 * @param[in] actionId The action to perform.  See Visual to find supported actions.
 * @param[in] attributes Optional attributes for the action.
 */
DALI_UI_API void DoAction(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                          const Dali::Property::Value& attributes);

/**
 * @brief Perform an action on a visual registered to this control.
 *
 * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
 *
 * @param[in] control The control.
 * @param[in] visualIndex The Property index of the visual.
 * @param[in] actionId The action to perform.  See Visual to find supported actions.
 * @param[in] attributes Optional attributes for the action.
 */
DALI_UI_API void DoActionExtension(Control& control, Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                                   const Dali::Any& attributes);

/**
 * @brief Takes corner properties of a control and applies them to a registered visual, overriding the visual's own
 * corner properties.
 *
 * This function is essential for visuals like shadows and borderlines that need to match the control's corner radius.
 * It allows the visual to inherit the control's corner properties (e.g., CORNER_RADIUS, CORNER_RADIUS_POLICY)
 * through an optional constraint. This ensures visual consistency, especially when the control's corners are rounded.
 *
 * This API should be used for each registered visual (e.g., SHADOW, INNER_SHADOW, BORDERLINE) that needs to
 * reflect the control's corner properties. For instance, when adding a shadow visual, this function should be called
 * with the shadow visual and an appropriate corner constraint to ensure the shadow's corners align with the control's
 * corners.
 *
 * @note This function is provided for derived control classes that cannot access
 * Control::Impl::EnableCornerPropertiesOverridden directly.
 * @note If @p enable is true, the @p cornerConstraint is applied to link the control's corner properties to the
 * visual's corner radius property. If @p enable is false, any previously applied constraint is removed, and the visual
 * will use its own corner properties.
 *
 * @param[in] control The control whose corner properties are to be used.
 * @param[in] visual The registered visual (e.g., shadow, borderline) that will receive the corner properties.
 * @param[in] enable True to apply the corner property override, false to disable it.
 * @param[in] cornerRadiusConstraint An optional Dali::Constraint that defines how the control's
 *                            corner properties (e.g., UI::DevelControl::Property::CORNER_RADIUS)
 *                            are mapped to the visual's corner radius property.
 *                            If empty, a default equality constraint might be used if applicable.
 */
DALI_UI_API void EnableCornerPropertiesOverridden(Internal::Control& control, Visual::Base& visual, bool enable,
                                                  Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

/**
 * @brief Set input method context.
 *
 * @param[in] control The control.
 * @param[in] inputMethodContext The input method context.
 */
DALI_UI_API void SetInputMethodContext(Internal::Control& control, InputMethodContext& inputMethodContext);

/**
 * @brief Visual Event signal type
 */
using VisualEventSignalType = Signal<void(Control, Dali::Property::Index, Dali::Property::Index)>;

/**
 * @brief This signal is emitted when a visual has an event to notify.
 *
 * A callback of the following type may be connected:
 * @code
 *   void YourCallbackName( Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId );
 * @endcode
 * @return The signal to connect to
 */
DALI_UI_API VisualEventSignalType& VisualEventSignal(Control control);

/**
 * @brief Retrieve the property object associated with the given property index and the visual property key.
 *
 * @param[in] control The control
 * @param[in] index The Property index of the visual.
 * @param[in] visualPropertyKey The key of the visual's property.
 * @return The Property object
 */
DALI_UI_API Dali::Property GetVisualProperty(Control control, Dali::Property::Index index,
                                             Dali::Property::Key visualPropertyKey);

/**
 * @brief The signal is emmited as a succession of "activate" signal send by accessibility client.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityActivateSignalType& AccessibilityActivateSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue but other text with higher priority prevented it from being read.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal(UI::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal(UI::Control control);

/**
 * @brief
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was placed in TTS queue and reading was started but other text with higher priority cancelled it.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal(UI::Control control);

/**
 * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
 * was fully read by TTS module.
 *
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal(UI::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's name.
 *
 * Connected callback should assign name to std::string call parameter.
 * Accessibility name can be stored in two ways:
 *     this signal,
 *     ACCESSIBILITY_NAME property,
 * Priority is as above. If none is used, default implementation is provided.
 * @return [description]
 */
DALI_UI_API AccessibilityGetNameSignalType& AccessibilityGetNameSignal(UI::Control control);

/**
 * @brief The signal is emmited when accessibility client asks for object's description.
 *
 * Connected callback should assign description to std::string call parameter.
 * Accessibility description can be stored in two ways:
 *     this signal,
 *     ACCESSIBILITY_DESCRIPTION property,
 * Priority is as above. If none is used, default implementation is provided.
 * @return signal handler
 */
DALI_UI_API AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoGesture" method via IPC mechanism.
 *
 * This signal allows developers to serve incoming gesture in specific way.
 * @return signal handler
 */
DALI_UI_API AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility client call "DoAction" or "DoActionName" method via IPC mechanism.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityActionSignalType& AccessibilityActionSignal(UI::Control control);

/**
 * @brief The signal is emitted when accessibility highlight is added or removed from control.
 * @return The signal to connect to
 */
DALI_UI_API AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal(UI::Control control);

/**
 * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_UI_API void AppendAccessibilityRelation(UI::Control control, Dali::Actor destination,
                                             Dali::Accessibility::RelationType relation);

/**
 * @brief The method allows removing relation
 *
 * @param control object to append attribute to
 * @param destination Actor object
 * @param relation    enumerated value describing relation
 */
DALI_UI_API void RemoveAccessibilityRelation(UI::Control control, Dali::Actor destination,
                                             Dali::Accessibility::RelationType relation);

/**
 * @brief Returns a collection of Accessible objects related to current object and grouped by relation type.
 *
 * @param control object to query
 * @return collection of relations
 *
 * @see Dali::Accessibility::Accessible::GetRelationSet()
 */
DALI_UI_API std::vector<Accessibility::Relation> GetAccessibilityRelations(UI::Control control);

/**
 * @brief The method removes all previously appended relations
 *
 * @param control object to append attribute to
 */
DALI_UI_API void ClearAccessibilityRelations(UI::Control control);

/**
 * @brief The method allows to add or modify value matched with given key.
 * Modification take place if key was previously set.
 *
 * @param control object to append attribute to
 * @param key   std::string value
 * @param value std::string value
 */
DALI_UI_API void AppendAccessibilityAttribute(UI::Control control, const std::string& key, const std::string& value);

/**
 * @brief The method erases key with its value from accessibility attributes
 * @param control object to append attribute to
 * @param key std::string value
 */
DALI_UI_API void RemoveAccessibilityAttribute(UI::Control control, const std::string& key);

/**
 * @brief The method clears accessibility attributes
 *
 * @param control object to append attribute to
 */
DALI_UI_API void ClearAccessibilityAttributes(UI::Control control);

/**
 * @brief The method inserts reading information of an accessible object into attributes
 *
 * @param control object to append attribute to
 * @param types Reading information types
 */
DALI_UI_API void SetAccessibilityReadingInfoType(UI::Control control,
                                                 const Dali::Accessibility::ReadingInfoTypes types);

/**
 * @brief The method returns reading information of an accessible object
 *
 * @param control object to append attribute to
 * @return Reading information types
 */
DALI_UI_API Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType(UI::Control control);

/**
 * @brief The method erases highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_UI_API bool ClearAccessibilityHighlight(UI::Control control);

/**
 * @brief The method grabs highlight.
 *
 * @param control object to append attribute to
 * @return bool value, false when it is not possible or something went wrong, at the other way true.
 */
DALI_UI_API bool GrabAccessibilityHighlight(UI::Control control);

/**
 * @brief The metod presents bitset of control's states.
 *
 * @param control object to append attribute to
 * @return Dali::Accessibility::States is vector of enumerated State.
 */
DALI_UI_API Dali::Accessibility::States GetAccessibilityStates(UI::Control control);

/**
 * @brief The method force sending notifications about current states to accessibility clients
 *
 * @param control object to append attribute to
 * @param states  mask with states expected to broadcast
 * @param recurse flag pointing if notifications of children's state would be sent
 */
DALI_UI_API void NotifyAccessibilityStateChange(UI::Control control, Dali::Accessibility::States states, bool recurse);

/**
 * @brief The method to get the control's accessibility created or not.
 *
 * @param[in] control object to append attribute to
 * @return True if accessible were created. False otherwise.
 */
DALI_UI_API bool IsAccessibleCreated(UI::Control control);

/**
 * @brief The method to set creatable of control's accessibility.
 * @note This method doesn't remove that already created accessible.
 *
 * @param[in] control object to append attribute to
 * @param[in] enable True if we allow to create accessible. False otherwise.
 */
DALI_UI_API void EnableCreateAccessible(UI::Control control, bool enable);

/**
 * @brief The method to get creatable of control's accessibility.
 *
 * @param[in] control object to append attribute to
 * @return True if we allow to create accessible. False otherwise.
 */
DALI_UI_API bool IsCreateAccessibleEnabled(UI::Control control);

/**
 * @brief The method to emit accessibility state-changed event to accessibility clients
 *
 * @param actor The actor that has accessibility state change.
 * @param state  The accessibility state.
 * @param newValue new value to set.
 */
DALI_UI_API void EmitAccessibilityStateChanged(Dali::Actor actor, Dali::Accessibility::State state, int newValue);

} // namespace DevelControl

} // namespace UI

} // namespace Dali

#endif // DALI_UI_CONTROL_DEVEL_H
