#pragma once

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
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/focus-manager/focus-manager-devel.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Integration
{
class SceneHolder;

} // namespace Integration

namespace Ui
{
namespace Internal
{
/**
 * @copydoc Ui::FocusManager
 */
class FocusManager : public Dali::BaseObject, public ConnectionTracker
{
public:
  typedef Ui::DevelFocusManager::CustomAlgorithmInterface CustomAlgorithmInterface;

  struct FocusChangeContext
  {
    Ui::FocusDevice device = Ui::FocusDevice::UNKNOWN;
    Dali::String    deviceName;
    Ui::InputEvent  inputEvent;
  };

  enum FocusIndicatorState
  {
    UNKNOWN = -1, ///< Unknown state
    HIDE    = 0,  ///< FocusIndicator is hidden
    SHOW    = 1,  ///< FocusIndicator is shown
  };

  enum EnableFocusedIndicatorState
  {
    DISABLE = 0, ///< FocusIndicator is disable
    ENABLE  = 1, ///< FocusIndicator is enable
  };

  enum FocusedIndicatorModeState
  {
    NONE        = 0, ///< Set nothing
    ALWAYS_SHOW = 1, ///< FocusIndicator is always shown
  };

  /**
   * @copydoc Ui::FocusManager::Get
   */
  static Ui::FocusManager Get();

  /**
   * Construct a new FocusManager.
   */
  FocusManager();

  /**
   * @copydoc Ui::FocusManager::SetCurrentFocusActor
   */
  bool SetCurrentFocusActor(Actor actor);

  /**
   * @copydoc Ui::FocusManager::GetCurrentFocusActor
   */
  Actor GetCurrentFocusActor();

  /**
   * @copydoc Ui::FocusManager::MoveFocus
   */
  bool MoveFocus(Ui::FocusDirection direction, const Dali::String& deviceName = "");

  /**
   * @brief Move the focus with device information
   */
  bool MoveFocus(Ui::FocusDirection direction, const FocusChangeContext& context);

  /**
   * @copydoc Ui::FocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Ui::FocusManager::SetAsFocusGroup
   */
  void SetAsFocusGroup(Actor actor, bool isFocusGroup);

  /**
   * @copydoc Ui::FocusManager::IsFocusGroup
   */
  bool IsFocusGroup(Actor actor) const;

  /**
   * @copydoc Ui::FocusManager::GetFocusGroup
   */
  Actor GetFocusGroup(Actor actor);

  /**
   * @copydoc Ui::FocusManager::SetFocusGroupLoop
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @copydoc Ui::FocusManager::GetFocusGroupLoop
   */
  bool GetFocusGroupLoop() const;

  /**
   * @copydoc Ui::FocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(View indicator);

  /**
   * @copydoc Ui::FocusManager::GetFocusIndicatorActor
   */
  Actor GetFocusIndicatorActor();

  /**
   * Move current focus to backward
   */
  void MoveFocusBackward();

  /**
   * @copydoc Ui::DevelFocusManager::SetCustomAlgorithm
   */
  void SetCustomAlgorithm(CustomAlgorithmInterface& interface);

  /**
   * @copydoc Ui::DevelFocusManager::UseFocusIndicator
   */
  void EnableFocusIndicator(bool enable);

  /**
   * @copydoc Ui::DevelFocusManager::UseFocusIndicator
   */
  bool IsFocusIndicatorEnabled() const;

  /**
   * @copydoc Ui::DevelFocusManager::EnableDefaultAlgorithm
   */
  void EnableDefaultAlgorithm(bool enable);

  /**
   * @copydoc Ui::DevelFocusManager::IsDefaultAlgorithmEnabled
   */
  bool IsDefaultAlgorithmEnabled() const;

  /**
   * @copydoc Ui::DevelFocusManager::SetFocusFinderRootActor
   */
  void SetFocusFinderRootActor(Actor actor);

  /**
   * @copydoc Ui::DevelFocusManager::ResetFocusFinderRootActor
   */
  void ResetFocusFinderRootActor();

  /**
   * Returns the last focus change context (device and optional device name).
   */
  const FocusChangeContext& FocusChangedContext() const;

  /**
   * @brief Sets whether to clear focus when window loses focus
   * @param enabled Whether to clear focus when window loses focus
   */
  void SetClearFocusOnWindowFocusLost(bool enabled);

  /**
   * @brief Gets whether to clear focus when window loses focus
   * @return Whether clear focus is enabled when window loses focus
   */
  bool GetClearFocusOnWindowFocusLost() const;

public:
  /**
   * @copydoc Ui::FocusManager::PreFocusChangeSignal()
   */
  Ui::FocusManager::PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @copydoc Ui::FocusManager::FocusChangedSignal()
   */
  Ui::FocusManager::FocusChangedSignalType& FocusChangedSignal();

  /**
   * @copydoc Ui::FocusManager::FocusGroupChangedSignal()
   */
  Ui::FocusManager::FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @copydoc Ui::FocusManager::FocusedActorEnterKeySignal()
   */
  Ui::FocusManager::FocusedActorEnterKeySignalType& FocusedActorEnterKeySignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor);

protected:
  /**
   * Destructor
   */
  virtual ~FocusManager();

private:
  typedef std::vector<WeakHandle<Actor>> FocusStack;         ///< Define Dali::Vector< Dali::BaseObject* > as FocusStack to contain focus history
  typedef FocusStack::iterator           FocusStackIterator; ///< Define FocusStack::Iterator as FocusStackIterator to navigate FocusStack

  /**
   * This will be called when the adaptor is initialized
   */
  void OnAdaptorInit();

  /**
   * This will be called when a new scene holder is created
   * @param sceneHolder The new scene holder
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder);

  /**
   * Get configuration from UiConfig.
   */
  void GetConfiguration();

  /**
   * Get the focus group of current focused actor.
   * @pre The FocusManager has been initialized.
   * @return A handle to the parent of the current focused actor which is a focus group,
   * or an empty handle if no actor is focused.
   */
  Actor GetCurrentFocusGroup();

  /**
   * Move the focus to the specified actor and send notification for the focus change.
   * @param actor The actor to be queried
   * @param context The context that caused the focus change (device, name)
   * @return Whether the focus is successful or not
   */
  bool DoSetCurrentFocusActor(Actor actor, const FocusChangeContext& context);

  /**
   * Move the focus to the next actor towards the specified direction within the layout view
   * @param view The layout view to move the focus in
   * @param actor The current focused actor
   * @param direction The direction of focus movement
   * @param context The context that caused the focus change (device, name)
   * @return Whether the focus is successful or not
   */
  bool DoMoveFocusWithinLayoutView(Ui::View view, Actor actor, Ui::FocusDirection direction, const FocusChangeContext& context);

  /**
   * Move the focus to the first focusable actor in the next focus group in the forward
   * or backward direction. The "Tab" key changes the focus group in the forward direction
   * and the "Shift-Tab" key changes it in the backward direction.
   * @param forward Whether the direction of focus group change is forward or backward
   * @param context The context that caused the focus change (device, name)
   * @return Whether the focus group change is successful or not
   */
  bool DoMoveFocusToNextFocusGroup(bool forward, const FocusChangeContext& context);

  /**
   * Enter has been pressed on the actor. If the actor is view, call the OnKeybaordEnter virtual function.
   * This function will emit FocusedActorEnterKeySignal.
   * @param actor The actor to notify
   */
  void DoKeyboardEnter(Actor actor);

  /**
   * Check whether the actor is a layout view that supports two dimensional keyboard navigation.
   * The layout view needs to internally set the focus order for the child actor and be able to
   * tell FocusManager the next focusable actor in the given direction.
   * @pre The FocusManager has been initialized.
   * @pre The Actor has been initialized.
   * @param actor The actor to be checked.
   * @return Whether the actor is a layout view or not.
   */
  bool IsLayoutView(Actor actor) const;

  /**
   * Returns the closest ancestor of the given actor that is a layout view.
   * @param actor The actor to be checked for its parent layout view
   * @return The parent layout view the given actor belongs to or an empty handle if the given actor doesn't belong to a layout view
   */
  Ui::View GetParentLayoutView(Actor actor) const;

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] event The KeyEvent event.
   */
  void OnKeyEvent(const KeyEvent& event);

  /**
   * Callback for the touch event when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   * @param[in] touch The touch information
   */
  void OnTouch(const TouchEvent& touch);

  /**
   * Callback for the wheel event when the custom wheel event occurs.
   * @param[in] wheel The WheelEvent information
   */
  bool OnCustomWheelEvent(const WheelEvent& wheel);

  /**
   * Callback for the wheel event when the wheel event occurs.
   * @param[in] wheel The WheelEvent information
   */
  void OnWheelEvent(const WheelEvent& wheel);

  /**
   * Called when the window focus is changed.
   * @param[in] window The window whose focus is changed
   * @param[in] focusIn Whether the focus is in/out
   */
  void OnWindowFocusChanged(Window window, bool focusIn);

  /**
   * Called when the scene holder focus is changed.
   * @param[in] sceneHolder The scene holder whose focus is changed
   * @param[in] focusIn Whether the focus is in/out
   */
  void OnSceneHolderFocusChanged(Dali::Integration::SceneHolder sceneHolder, bool focusIn);

  /**
   * Get the focus Actor from current window
   */
  Actor GetFocusActorFromCurrentWindow();

  /**
   * Convert Device::Class to FocusDevice
   * @param deviceClass The device class from the touch event
   * @return The corresponding FocusDevice
   */
  Ui::FocusDevice ConvertDeviceClassToKeyboardFocusDevice(Device::Class::Type deviceClass) const;

  /**
   * Recursively deliver events to the view and its parents, until the event is consumed or the stage is reached.
   * @param[in]  actor  The actor got WheelEvent.
   * @param[in]  event  The WheelEvent.
   * @return True if WheelEvent is consumed.
   */
  bool EmitCustomWheelSignals(Actor actor, const WheelEvent& event);

  /**
   * Clear the focus actor
   * @param[in] actor Actor to be cleared of focus
   */
  void ClearFocus(Actor actor);

  /**
   * Clear the focus indicator actor.
   * @param[in] actor Actor to be cleared of focus indicator.
   */
  void ClearFocusIndicator(Actor actor);

  /**
   * Gets the current native window id
   */
  uint32_t GetCurrentWindowId() const;

  /**
   * Signal handler called when a focused actor is removed from Scene.
   * @param[in] actor The actor removed from the scene.
   */
  void OnSceneDisconnection(Dali::Actor actor);

private:
  // Undefined
  FocusManager(const FocusManager&);

  FocusManager& operator=(const FocusManager& rhs);

private:
  Ui::FocusManager::PreFocusChangeSignalType       mPreFocusChangeSignal;       ///< The signal to notify the focus will be changed
  Ui::FocusManager::FocusChangedSignalType         mFocusChangedSignal;         ///< The signal to notify the focus change
  Ui::FocusManager::FocusGroupChangedSignalType    mFocusGroupChangedSignal;    ///< The signal to notify the focus group change
  Ui::FocusManager::FocusedActorEnterKeySignalType mFocusedActorEnterKeySignal; ///< The signal to notify that enter has been pressed on the focused actor

  WeakHandle<Actor> mCurrentFocusActor; ///< A weak handle to the current focused actor

  View mFocusIndicatorActor; ///< The focus indicator actor shared by all the keyboard focusable actors for highlight

  WeakHandle<Actor> mFocusFinderRootActor; ///< The root actor from which the focus finder is started.

  FocusStack mFocusHistory; ///< Stack to contain pre-focused actor's BaseObject*

  SlotDelegate<FocusManager> mSlotDelegate;

  CustomAlgorithmInterface* mCustomAlgorithmInterface; ///< The user's (application / ui) implementation of CustomAlgorithmInterface

  typedef std::vector<std::pair<WeakHandle<Layer>, WeakHandle<Actor>>> FocusActorContainer;

  FocusActorContainer mCurrentFocusActors; ///< A container of focused actors

  WeakHandle<Layer> mCurrentFocusedWindow; ///< A weak handle to the current focused window's root layer

  FocusIndicatorState mIsFocusIndicatorShown; ///< Whether indicator should be shown / hidden when getting focus. It could be enabled
                                              ///< when keyboard focus feature is enabled and navigation keys or 'Tab' key are pressed.

  EnableFocusedIndicatorState mEnableFocusIndicator; ///< Whether use focus indicator

  FocusedIndicatorModeState mAlwaysShowIndicator; ///< Whether always show indicator. If true, the indicator would be
                                                  ///< directly shown when focused

  bool mFocusGroupLoopEnabled : 1; ///< Whether the focus movement is looped within the same focus group

  bool mIsWaitingKeyboardFocusChangeCommit : 1; /// A flag to indicate PreFocusChangeSignal emitted but the proposed
                                                /// focus actor is not committed by the application yet.

  bool mClearFocusOnTouch : 1; ///< Whether clear focus on touch.

  bool mEnableDefaultAlgorithm : 1; ///< Whether use default algorithm focus

  bool mClearFocusOnWindowFocusLost : 1; ///< Whether clear focus when window loses focus

  uint32_t mCurrentWindowId; ///< The current native window id

  FocusChangeContext mLastFocusChangeContext; ///< The last focus change context (device & name)
};

} // namespace Internal

inline Internal::FocusManager& GetImpl(Dali::Ui::FocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::FocusManager&>(handle);
}

inline const Internal::FocusManager& GetImpl(const Dali::Ui::FocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::FocusManager&>(handle);
}

} // namespace Ui

} // namespace Dali
