#ifndef DALI_UI_CONTROL_H
#define DALI_UI_CONTROL_H

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
#include <dali-ui-foundation/public-api/controls/control-accessibility-enums.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali/devel-api/adaptor-framework/accessibility-bridge.h>
#include <dali/devel-api/adaptor-framework/input-method-context.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
class Actor;
class Any;
class Constraint;

namespace Ui
{
class RenderEffect;

namespace Visual
{
class Base;
}

// Forward declarations.

namespace Internal
{
class Control;
}
/**
 * @addtogroup dali_toolkit_controls
 * @{
 */

/**
 * @brief Control is the base class for all controls.
 *
 * The implementation of the control must be supplied; see Internal::Control for more details.
 * @SINCE_1_0.0
 * @see Internal::Control
 *
 * Signals
 * | %Signal Name           | Method                                              |
 * |------------------------|-----------------------------------------------------|
 * | keyEvent               | @ref KeyEventSignal()                               |
 * | keyInputFocusGained    | @ref KeyInputFocusGainedSignal()                    |
 * | keyInputFocusLost      | @ref KeyInputFocusLostSignal()                      |
 * | resourceReady          | @ref ResourceReadySignal()                          |
 * | offscreenRenderingFinished | @ref OffScreenRenderingFinishedSignal()       |
 * | tapped                 | @ref GetTapGestureDetector().DetectedSignal()       |
 * | panned                 | @ref GetPanGestureDetector().DetectedSignal()       |
 * | pinched                | @ref GetPinchGestureDetector().DetectedSignal()     |
 * | longPressed            | @ref GetLongPressGestureDetector().DetectedSignal() |
 *
 * Actions
 * | %Action Name           | %Control method called                             |
 * |------------------------|----------------------------------------------------|
 * | accessibilityActivated | %OnAccessibilityActivated()                        |
 */
class DALI_UI_API Control : public CustomActor
{
public:
  /**
   * @brief Enumeration for the start and end property ranges for control.
   * @SINCE_1_0.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX =
      PROPERTY_REGISTRATION_START_INDEX,                               ///< Start index is used by the property registration macro. @SINCE_1_0.0
    CONTROL_PROPERTY_START_INDEX = PROPERTY_START_INDEX,               ///< Start index of Control properties. @SINCE_1_0.0
    CONTROL_PROPERTY_END_INDEX   = CONTROL_PROPERTY_START_INDEX + 1000 ///< Reserving 1000 property indices. @SINCE_1_0.0
  };

  /**
   * @brief OffScreenRenderingType enumeration.
   */
  enum OffScreenRenderingType
  {
    NONE,
    REFRESH_ONCE,
    REFRESH_ALWAYS
  };
  /**
   * @brief Enumeration for the instance of properties belonging to the Control class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the Control class.
     * @SINCE_1_0.0
     */
    enum
    {
      /**
       * @brief Receives key events to the control.
       * @details Name "keyInputFocus", type Property::BOOLEAN.
       * @see Ui::Control::SetKeyInputFocus()
       * @SINCE_1_0.0
       */
      KEY_INPUT_FOCUS = PROPERTY_START_INDEX,

      /**
       * @brief The background of the control.
       *
       * @details Name "background", type Property::MAP or std::string for URL or Property::VECTOR4 for Color.
       * @SINCE_1_1.3
       */
      BACKGROUND,

      /**
       * @brief The outer space around the control.
       * @details Name "margin", type Property::EXTENTS.
       * @SINCE_1_2.62
       * @note Margin property is to be supported by Layout algorithms and containers in future.
       */
      MARGIN,

      /**
       * @brief The inner space of the control.
       * @details Name "padding", type Property::EXTENTS.
       * @SINCE_1_2.62
       */
      PADDING,

      /**
       * @brief The actor ID of the left focusable control.
       * @details Name "leftFocusableActorId", type Property::INTEGER.
       */
      LEFT_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the right focusable control.
       * @details Name "rightFocusableActorId", type Property::INTEGER.
       */
      RIGHT_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the up focusable control.
       * @details Name "upFocusableActorId", type Property::INTEGER.
       */
      UP_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the down focusable control.
       * @details Name "downFocusableActorId", type Property::INTEGER.
       */
      DOWN_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The shadow of the control.
       * @details Name "shadow", type Property::MAP.
       */
      SHADOW,

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
       * @note It gets integer value of AccessibilityRole enum then interprets to Dali::Accessibility::Role when
       * requested by AT-SPI. Note that setting Dali::Accessibility::Role value is still accepted for backward
       * compatibility but not preferred.
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
       * @note If a control's dispatchKeyEvents is set to false, then it's children will not emit a key event signal
       * either.
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
       */
      CLOCKWISE_FOCUSABLE_ACTOR_ID,

      /**
       * @brief The actor ID of the conter-clockwise focusable control.
       * @details Name "counterClockwiseFocusableActorId", type Property::INTEGER.
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
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_RADIUS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS = ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX,

      /**
       * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute
       * (in world units).
       * @details Name "viewCornerRadiusPolicy", type Property::INTEGER.
       * @see Policy::Type
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_RADIUS_POLICY
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_RADIUS_POLICY,

      /**
       * @brief The squareness for the rounded corners of the control.
       * @details Name "viewCornerSquareness", type Property::VECTOR4 or Property::FLOAT
       * @note By default, it is Vector::ZERO.
       * @note Applies to specific visuals inside the control.
       * @note Only Property::Vector4 can be animated.
       * @see Dali::Ui::DevelVisual::Property::Type::CORNER_SQUARENESS
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      CORNER_SQUARENESS,

      /**
       * @brief The width for the borderline of the control. It will update borderline visual
       * @details Name "viewBorderlineWidth", type Property::FLOAT.
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_WIDTH
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_WIDTH,

      /**
       * @brief The color for the borderline of the control. It will update borderline visual
       * @details Name "viewBorderlineColor", type Property::VECTOR4
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_COLOR
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_COLOR,

      /**
       * @brief The offset for the borderline of the control. It will update borderline visual
       * @details Name "viewBorderlineOffset", type Property::FLOAT.
       * @see Dali::Ui::DevelVisual::Property::Type::BORDERLINE_OFFSET
       * @note It will not create UniformMap internally. So this property don't be used at Render phase.
       */
      BORDERLINE_OFFSET
    };
  };

  /**
   * @brief Describes the direction to move the keyboard focus towards.
   * @SINCE_1_0.0
   */
  struct KeyboardFocus
  {
    /**
     * @brief Keyboard focus direction.
     * @SINCE_1_0.0
     */
    enum Direction
    {
      LEFT,              ///< Move keyboard focus towards the left direction @SINCE_1_0.0
      RIGHT,             ///< Move keyboard focus towards the right direction @SINCE_1_0.0
      UP,                ///< Move keyboard focus towards the up direction @SINCE_1_0.0
      DOWN,              ///< Move keyboard focus towards the down direction @SINCE_1_0.0
      PAGE_UP,           ///< Move keyboard focus towards the previous page direction @SINCE_1_2.14
      PAGE_DOWN,         ///< Move keyboard focus towards the next page direction @SINCE_1_2.14
      FORWARD,           ///< Move keyboard focus towards the forward direction @SINCE_2_1.10
      BACKWARD,          ///< Move keyboard focus towards the backward direction @SINCE_2_1.10
      CLOCKWISE,         ///< Move keyboard focus towards the clockwise direction @SINCE_2_1.14
      COUNTER_CLOCKWISE, ///< Move keyboard focus towards the counter clockwise direction @SINCE_2_1.14
    };

    /**
     * @brief Keyboard focus device.
     * @SINCE_2_5.5
     */
    enum Device
    {
      UNKNOWN,     ///< Unknown device. @SINCE_2_5.5
      KEYBOARD,    ///< A regular keyboard, numberpad or attached buttons. @SINCE_2_5.5
      MOUSE,       ///< A mouse, trackball or touchpad relative motion device. @SINCE_2_5.5
      TOUCH,       ///< A touchscreen with fingers or stylus. @SINCE_2_5.5
      PEN,         ///< A special pen device. @SINCE_2_5.5
      POINTER,     ///< A pointing device based on laser, infrared or similar technology. @SINCE_2_5.5
      GAMEPAD,     ///< A gamepad controller or joystick. @SINCE_2_5.5
      WHEEL,       ///< A mouse device. @SINCE_2_5.5
      PROGRAMMATIC ///< Set to API call, not device. @SINCE_2_5.5
    };
  };

  // Typedefs

  /// @brief Key Event signal type. @SINCE_1_0.0
  typedef Signal<bool(Control, const KeyEvent&)> KeyEventSignalType;

  /// @brief Key InputFocusType signal type. @SINCE_1_0.0
  typedef Signal<void(Control)> KeyInputFocusSignalType;

  /// @brief ResourceReady signal type. @SINCE_1_2.60
  typedef Signal<void(Control)> ResourceReadySignalType;

  /// @brief Offscreen rendering finished signal type. @SINCE_2_4.33
  typedef Signal<void(Control)> OffScreenRenderingFinishedSignalType;

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

  /// @brief Visual Event signal type.
  typedef Signal<void(Control, Dali::Property::Index, Dali::Property::Index)> VisualEventSignalType;

public: // Creation & Destruction
  /**
   * @brief Additional control behaviour flags for the control constructor.
   * @note TODO : Currunt code is hard-coded. We Should sync type values as
   * CustomActorImpl::ActorFlag and Internal::Control::ControlBehaviour in future.
   * @SINCE_2_1.8
   */
  enum ControlBehaviour
  {
    CONTROL_BEHAVIOUR_DEFAULT = 0,                       ///< Default behaviour: Size negotiation is enabled & listens to Style Change signal,
                                                         ///< but doesn't receive event callbacks. @SINCE_1_2_10
    DISABLE_SIZE_NEGOTIATION = 1 << (0 + 0),             ///< True if control does not need size negotiation, i.e. it can be skipped
                                                         ///< in the algorithm @SINCE_1_0.0
    REQUIRES_KEYBOARD_NAVIGATION_SUPPORT = 1 << (4 + 1), ///< True if needs to support keyboard navigation @SINCE_1_0.0
    DISABLE_STYLE_CHANGE_SIGNALS =
      1 << (4 + 2),                 ///< True if control should not monitor style change signals @SINCE_1_2_10
    DISABLE_VISUALS = 1 << (4 + 3), ///< True if control should not use visuals @SINCE_2_3.6
  };

  /**
   * @brief Creates a new instance of a Control.
   *
   * @SINCE_1_0.0
   * @return A handle to a new Control
   */
  static Control New();

  /**
   * @brief Creates a new instance of a Control with additional behaviour.
   *
   * @SINCE_2_1.8
   * @param[in] additionalBehaviour Additional control behaviour
   * @return A handle to a new Control
   */
  static Control New(ControlBehaviour additionalBehaviour);

  /**
   * @brief Creates an uninitialized Control handle.
   *
   * Only derived versions can be instantiated.  Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   * @SINCE_1_0.0
   */
  Control();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @SINCE_1_0.0
   * @param[in] uiControl Handle to copy
   */
  Control(const Control& uiControl);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.23
   * @param[in] rhs Handle to move
   */
  Control(Control&& rhs) noexcept;

  /**
   * @brief Dali::Control is intended as a base class.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~Control();

public: // operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @SINCE_1_0.0
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  Control& operator=(const Control& handle);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.23
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  Control& operator=(Control&& rhs) noexcept;

public:
  /**
   * @brief Downcasts a handle to Control handle.
   *
   * If handle points to a Control, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return A handle to a Control or an uninitialized handle
   */
  static Control DownCast(BaseHandle handle);

  // Key Input

  /**
   * @brief This sets the control to receive key events.
   *
   * The key event can originate from a virtual or physical keyboard.
   * @SINCE_1_0.0
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   */
  void SetKeyInputFocus();

  /**
   * @brief Quries whether the control has key input focus.
   *
   * @SINCE_1_0.0
   * @return true if this control has keyboard input focus
   * @pre The Control has been initialized.
   * @pre The Control should be on the stage before setting keyboard focus.
   * @note The control can be set to have the focus and still not receive all the key events if another control has over
   * ridden it. As the key input focus mechanism works like a stack, the top most control receives all the key events,
   * and passes on the unhandled events to the controls below in the stack. A control in the stack will regain key input
   * focus when there are no more controls above it in the focus stack. To query for the control which is on top of the
   * focus stack use Dali::Ui::KeyInputFocusManager::GetCurrentKeyboardFocusActor().
   */
  bool HasKeyInputFocus();

  /**
   * @brief Once an actor is Set to receive key input focus this function is called to stop it receiving key events.
   *
   * A check is performed to ensure it was previously set, if this check fails then nothing is done.
   * @SINCE_1_0.0
   * @pre The Actor has been initialized.
   */
  void ClearKeyInputFocus();

  // Gesture Detection

  /**
   * @brief Retrieves the pinch gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The pinch gesture detector
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  PinchGestureDetector GetPinchGestureDetector() const;

  /**
   * @brief Retrieves the pan gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The pan gesture detector
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  PanGestureDetector GetPanGestureDetector() const;

  /**
   * @brief Retrieves the tap gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The tap gesture detector
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  TapGestureDetector GetTapGestureDetector() const;

  /**
   * @brief Retrieves the long press gesture detector of the control.
   *
   * @SINCE_1_0.0
   * @return The long press gesture detector
   * @note Will return an empty handle if the control does not handle the gesture itself.
   */
  LongPressGestureDetector GetLongPressGestureDetector() const;

  // Background

  /**
   * @brief Sets the background color of the control.
   *
   * @SINCE_1_0.0
   * @param[in] color The required background color of the control
   *
   * @note If SetBackgroundImage is called later, this background color is removed.
   *
   * @note The background color fully blends with the actor color.
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * @brief Clears the background.
   * @SINCE_1_0.0
   */
  void ClearBackground();

  /**
   * @brief Sets RenderEffect to this control.
   *
   * @SINCE_2_3.25
   * @param[in] effect RenderEffect to add.
   *
   * @note Every effect inherits RenderEffect.
   */
  void SetRenderEffect(Ui::RenderEffect effect);

  /*
   * @brief Retrieves control's render effect
   * @SINCE_2_4.1
   * @return RenderEffect set on this control
   */
  Ui::RenderEffect GetRenderEffect() const;

  /**
   * @brief Clears RenderEffect of this control, if exists.
   * @SINCE_2_3.25
   */
  void ClearRenderEffect();

  // Renderers

  /**
   * @brief Helper method for rendering an image with custom shader.
   * @param[in] vertexSrc The custom vertex shader.
   * @param[in] fragmentSrc The custom fragment shader.
   * @return A newly created renderer.
   * @SINCE_2_6.28
   */
  static Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc);

  /**
   * @brief Helper method for rendering an image with custom shader.
   * @param[in] vertexSrc The custom vertex shader.
   * @param[in] fragmentSrc The custom fragment shader.
   * @param[in] hints Shader hints for optimization.
   * @param[in] shaderName Name of the shader for debugging.
   * @param[in] gridSize The number of grid sub-divisions required.
   * @return A newly created renderer.
   * @SINCE_2_6.28
   */
  static Dali::Renderer CreateRenderer(std::string_view vertexSrc, std::string_view fragmentSrc,
                                       Dali::Shader::Hint::Value hints, const std::string& shaderName,
                                       Dali::Uint16Pair gridSize);

  /**
   * @brief Helper method for setting the first texture passed to a renderer.
   * @param[in] renderer The renderer using the texture.
   * @param[in] texture The texture to set.
   * @SINCE_2_6.28
   */
  static void SetRendererTexture(Dali::Renderer renderer, Dali::Texture texture);

  /**
   * @brief Helper method for setting the first texture passed to a renderer.
   * @param[in] renderer The renderer using the texture.
   * @param[in] frameBuffer A frame buffer color texture attached.
   * @SINCE_2_6.28
   */
  static void SetRendererTexture(Dali::Renderer renderer, Dali::FrameBuffer frameBuffer);

  // Resources

  /**
   * @brief Query if all resources required by a control are loaded and ready.
   *
   * Most resources are only loaded when the control is placed on stage.
   * @SINCE_1_2.60
   * @return true if the resources are loaded and ready, false otherwise
   */
  bool IsResourceReady() const;

  /**
   * @brief Get the loading state of the visual resource.
   *
   * @SINCE_1_3_5
   * @param[in] index The Property index of the visual
   * @return Return the loading status (PREPARING, READY and FAILED) of visual resource
   */
  Visual::ResourceStatus GetVisualResourceStatus(const Dali::Property::Index index);

  // Signals

  /**
   * @brief This signal is emitted when key event is received.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName(Control control, const KeyEvent& event);
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Control has been initialized.
   */
  KeyEventSignalType& KeyEventSignal();

  /**
   * @brief This signal is emitted when the control gets Key Input Focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Control control );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Control has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusGainedSignal();

  /**
   * @brief This signal is emitted when the control loses Key Input Focus.
   *
   * This could be due to it being gained by another Control or Actor or just cleared from
   * this control as no longer required.
   *
   * A callback of the following type may be connected:
   * @code
   *   bool YourCallbackName( Control control );
   * @endcode
   * The return value of True, indicates that the event should be consumed.
   * Otherwise the signal will be emitted on the next parent of the actor.
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Control has been initialized.
   */
  KeyInputFocusSignalType& KeyInputFocusLostSignal();

  /**
   * @brief This signal is emitted after all resources required by a control are loaded and ready.
   *
   * Most resources are only loaded when the control is placed on stage.
   *
   * If resources are shared between ImageViews, they are cached.
   * In this case, the ResourceReady signal may be sent before there is an object to connect to.
   * To protect against this, IsResourceReady() can be checked first.
   *
   * @code
   *    auto newControl = Control::New();
   *    newControl.SetResource( resourceUrl );
   *    if ( newControl.IsResourceReady() )
   *    {
   *       // do something
   *    }
   *    else
   *    {
   *      newControl.ResourceReadySignal.Connect( .... )
   *    }
   * @endcode
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Control control );
   * @endcode
   *
   * @SINCE_1_2.60
   * @return The signal to connect to
   * @note A RelayoutRequest is queued by Control before this signal is emitted
   */
  ResourceReadySignalType& ResourceReadySignal();

  /**
   * @brief This signal is emitted when offscreen rendering is finished.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Control control );
   * @endcode
   *
   * @SINCE_2_4.33
   * @return The signal to connect to
   * @pre The Control has been initialized.
   * @note This signal is emitted when the offscreen rendering task is completed.
   * @note This signal is only emitted when OffScreenRenderingType is set to RENDER_ONCE.
   */
  OffScreenRenderingFinishedSignalType& OffScreenRenderingFinishedSignal();

public: // Visual Management
  /**
   * @brief Register a visual by Property Index.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   *
   * @note Derived class should not call visual.SetOnScene(actor). It is the responsibility of the base class to
   * connect/disconnect registered visual to stage. Use below API with enabled set to false if derived class wishes to
   * control when visual is staged.
   * @note depth-index be used to Dali::Ui::DepthIndex::Ranges::AUTO_INDEX.
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual);

  /**
   * @brief Register a visual by Property Index with a depth index.
   *
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
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, int depthIndex);

  /**
   * @brief Register a visual by Property Index with the option of enabling/disabling it.
   *
   * @param[in] index The Property index of the visual, used to reference visual
   * @param[in] visual The visual to register
   * @param[in] enabled false if derived class wants to control when visual is set on stage.
   *
   * @note depth-index be used to Dali::Ui::DepthIndex::Ranges::AUTO_INDEX.
   *
   * @see EnableVisual()
   */
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled);

  /**
   * @brief Register a visual by Property Index with a depth index with the option of enabling/disabling it.
   *
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
  void RegisterVisual(Dali::Property::Index index, Ui::Visual::Base& visual, bool enabled, int depthIndex);

  /**
   * @brief Erase the entry matching the given index from the list of registered visuals
   *
   * @param[in] index The Property index of the visual, used to reference visual
   */
  void UnregisterVisual(Dali::Property::Index index);

  /**
   * @brief Retrieve the visual associated with the given property index.
   *
   * @param[in] index The Property index of the visual.
   * @return The registered visual if exist, otherwise empty handle.
   * @note For managing object life-cycle, do not store the returned visual as a member which increments its reference
   * count.
   */
  Ui::Visual::Base GetVisual(Dali::Property::Index index);

  /**
   * @brief Sets the given visual to be displayed or not when parent staged.
   *
   * @param[in] index The Property index of the visual
   * @param[in] enable flag to set enabled or disabled.
   */
  void EnableVisual(Dali::Property::Index index, bool enable);

  /**
   * @brief Queries if the given visual is to be displayed when parent staged.
   *
   * @param[in] index The Property index of the visual
   * @return bool whether visual is enabled or not
   */
  bool IsVisualEnabled(Dali::Property::Index index);

  /**
   * @brief Perform an action on a visual registered to this control.
   *
   * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
   *
   * @param[in] visualIndex The Property index of the visual.
   * @param[in] actionId The action to perform.  See Visual to find supported actions.
   * @param[in] attributes Optional attributes for the action.
   */
  void DoAction(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
                const Dali::Property::Value& attributes);

  /**
   * @brief Perform an action on a visual registered to this control.
   *
   * Visuals will have actions, this API is used to perform one of these actions with the given attributes.
   *
   * @param[in] visualIndex The Property index of the visual.
   * @param[in] actionId The action to perform.  See Visual to find supported actions.
   * @param[in] attributes Optional attributes for the action.
   */
  void DoActionExtension(Dali::Property::Index visualIndex, Dali::Property::Index actionId,
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
   * visual's corner radius property. If @p enable is false, any previously applied constraint is removed, and the
   * visual will use its own corner properties.
   *
   * @param[in] visual The registered visual (e.g., shadow, borderline) that will receive the corner properties.
   * @param[in] enable True to apply the corner property override, false to disable it.
   * @param[in] cornerRadiusConstraint An optional Dali::Constraint that defines how the control's
   *                            corner properties (e.g., Ui::Control::Property::CORNER_RADIUS)
   *                            are mapped to the visual's corner radius property.
   *                            If empty, a default equality constraint might be used if applicable.
   */
  void EnableCornerPropertiesOverridden(Visual::Base& visual, bool enable,
                                        Dali::Constraint cornerRadiusConstraint = Dali::Constraint());

  /**
   * @brief Set input method context.
   *
   * @param[in] inputMethodContext The input method context.
   */
  void SetInputMethodContext(InputMethodContext& inputMethodContext);

  /**
   * @brief This signal is emitted when a visual has an event to notify.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Control control, Dali::Property::Index visualIndex, Dali::Property::Index signalId );
   * @endcode
   * @return The signal to connect to
   */
  VisualEventSignalType& VisualEventSignal();

  /**
   * @brief Retrieve the property object associated with the given property index and the visual property key.
   *
   * @param[in] index The Property index of the visual.
   * @param[in] visualPropertyKey The key of the visual's property.
   * @return The Property object
   */
  Dali::Property GetVisualProperty(Dali::Property::Index index, Dali::Property::Key visualPropertyKey);

public: // Accessibility
  /**
   * @brief The signal is emmited as a succession of "activate" signal send by accessibility client.
   * @return The signal to connect to
   */
  AccessibilityActivateSignalType& AccessibilityActivateSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was placed in TTS queue but other text with higher priority prevented it from being read.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingSkippedSignalType& AccessibilityReadingSkippedSignal();

  /**
   * @brief
   *
   * @return The signal to connect to
   */
  AccessibilityReadingPausedSignalType& AccessibilityReadingPausedSignal();

  /**
   * @brief
   *
   * @return The signal to connect to
   */
  AccessibilityReadingResumedSignalType& AccessibilityReadingResumedSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was placed in TTS queue and reading was started but other text with higher priority cancelled it.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingCancelledSignalType& AccessibilityReadingCancelledSignal();

  /**
   * @brief The signal is emmited when text send via Dali::Accessibility::Bridge::Say
   * was fully read by TTS module.
   *
   * @return The signal to connect to
   */
  AccessibilityReadingStoppedSignalType& AccessibilityReadingStoppedSignal();

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
  AccessibilityGetNameSignalType& AccessibilityGetNameSignal();

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
  AccessibilityGetDescriptionSignalType& AccessibilityGetDescriptionSignal();

  /**
   * @brief The signal is emitted when accessibility client call "DoGesture" method via IPC mechanism.
   *
   * This signal allows developers to serve incoming gesture in specific way.
   * @return signal handler
   */
  AccessibilityDoGestureSignalType& AccessibilityDoGestureSignal();

  /**
   * @brief The signal is emitted when accessibility client call "DoAction" or "DoActionName" method via IPC mechanism.
   * @return The signal to connect to
   */
  AccessibilityActionSignalType& AccessibilityActionSignal();

  /**
   * @brief The signal is emitted when accessibility highlight is added or removed from control.
   * @return The signal to connect to
   */
  AccessibilityHighlightedSignalType& AccessibilityHighlightedSignal();

  /**
   * @brief The method allows connection with other actor with usage of concrete accessibility relation type.
   *
   * @param destination Actor object
   * @param relation    enumerated value describing relation
   */
  void AppendAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation);

  /**
   * @brief The method allows removing relation
   *
   * @param destination Actor object
   * @param relation    enumerated value describing relation
   */
  void RemoveAccessibilityRelation(Dali::Actor destination, Dali::Accessibility::RelationType relation);

  /**
   * @brief Returns a collection of Accessible objects related to current object and grouped by relation type.
   *
   * @return collection of relations
   *
   * @see Dali::Accessibility::Accessible::GetRelationSet()
   */
  std::vector<Accessibility::Relation> GetAccessibilityRelations();

  /**
   * @brief The method removes all previously appended relations
   */
  void ClearAccessibilityRelations();

  /**
   * @brief The method allows to add or modify value matched with given key.
   * Modification take place if key was previously set.
   *
   * @param key   std::string value
   * @param value std::string value
   */
  void AppendAccessibilityAttribute(const std::string& key, const std::string& value);

  /**
   * @brief The method erases key with its value from accessibility attributes
   * @param key std::string value
   */
  void RemoveAccessibilityAttribute(const std::string& key);

  /**
   * @brief The method clears accessibility attributes
   */
  void ClearAccessibilityAttributes();

  /**
   * @brief The method inserts reading information of an accessible object into attributes
   *
   * @param types Reading information types
   */
  void SetAccessibilityReadingInfoType(const Dali::Accessibility::ReadingInfoTypes types);

  /**
   * @brief The method returns reading information of an accessible object
   *
   * @return Reading information types
   */
  Dali::Accessibility::ReadingInfoTypes GetAccessibilityReadingInfoType();

  /**
   * @brief The method erases highlight.
   *
   * @return bool value, false when it is not possible or something went wrong, at the other way true.
   */
  bool ClearAccessibilityHighlight();

  /**
   * @brief The method grabs highlight.
   *
   * @return bool value, false when it is not possible or something went wrong, at the other way true.
   */
  bool GrabAccessibilityHighlight();

  /**
   * @brief The metod presents bitset of control's states.
   *
   * @return Dali::Accessibility::States is vector of enumerated State.
   */
  Dali::Accessibility::States GetAccessibilityStates();

  /**
   * @brief The method force sending notifications about current states to accessibility clients
   *
   * @param states  mask with states expected to broadcast
   * @param recurse flag pointing if notifications of children's state would be sent
   */
  void NotifyAccessibilityStateChange(Dali::Accessibility::States states, bool recurse);

  /**
   * @brief The method to get the control's accessibility created or not.
   *
   * @return True if accessible were created. False otherwise.
   */
  bool IsAccessibleCreated();

  /**
   * @brief The method to set creatable of control's accessibility.
   * @note This method doesn't remove that already created accessible.
   *
   * @param enable True if we allow to create accessible. False otherwise.
   */
  void EnableCreateAccessible(bool enable);

  /**
   * @brief The method to get creatable of control's accessibility.
   *
   * @return True if we allow to create accessible. False otherwise.
   */
  bool IsCreateAccessibleEnabled();

public: // Accessibility Methods
  /**
   * @brief The method to emit accessibility state-changed event to accessibility clients
   *
   * @param state  The accessibility state.
   * @param newValue new value to set.
   */
  void EmitAccessibilityStateChanged(Accessibility::State state, int newValue);

public: // Intended for control developers
  /**
   * @brief Creates an initialized Control.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The implementation for this control
   * @note Should NOT be called to create a handle from the implementation. As stated, this allocates a NEW Dali
   * resource.
   */
  explicit Control(Internal::Control& implementation);

  /**
   * @brief This constructor is used by CustomActor within Dali core to create additional Control handles
   * using an Internal CustomActor pointer.
   *
   * @SINCE_1_0.0
   * @param[in] internal A pointer to a newly allocated Dali resource
   */
  explicit Control(Dali::Internal::CustomActor* internal);

public: // Templates for Deriving Classes
  /**
   * @brief Template to allow deriving controls to DownCast handles to deriving handle classes.
   *
   * @tparam     T      The handle class
   * @tparam     I      The implementation class
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a class T or an uninitialized handle
   * @see DownCast(BaseHandle)
   */
  template<typename T, typename I>
  DALI_INTERNAL static T DownCast(BaseHandle handle)
  {
    T result;

    CustomActor custom = Dali::CustomActor::DownCast(handle);
    if(custom)
    {
      CustomActorImpl& customImpl = custom.GetImplementation();

      I* impl = dynamic_cast<I*>(&customImpl);

      if(impl)
      {
        result = T(customImpl.GetOwner());
      }
    }

    return result;
  }

  /**
   * @brief Template to allow deriving controls to verify whether the Internal::CustomActor* is actually an
   * implementation of their class.
   *
   * @tparam     I       The implementation class
   * @SINCE_1_0.0
   * @param[in] internal Pointer to the Internal::CustomActor
   */
  template<typename I>
  DALI_INTERNAL void VerifyCustomActorPointer(Dali::Internal::CustomActor* internal)
  {
    // Can have a NULL pointer so we only need to check if the internal implementation is our class
    // when there is a value.
    if(internal)
    {
      DALI_ASSERT_DEBUG(dynamic_cast<I*>(&CustomActor(internal).GetImplementation()));
    }
  }
};

/**
 * @}
 */
} // namespace Ui

} // namespace Dali

#endif // DALI_UI_CONTROL_H
