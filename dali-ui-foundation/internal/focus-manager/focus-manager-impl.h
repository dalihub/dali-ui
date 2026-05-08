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
   * @copydoc Ui::FocusManager::SetCurrentFocusView
   */
  bool SetCurrentFocusView(View view);

  /**
   * @brief Requests focus on the given view, delegating to its RequestFocus()
   * for child-first resolution, then committing the result.
   * @param view The view to request focus on
   * @return Whether the focus was successfully set
   */
  bool RequestFocus(View view);

  /**
   * @copydoc Ui::FocusManager::GetCurrentFocusView
   */
  View GetCurrentFocusView();

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
  void SetAsFocusGroup(View view, bool isFocusGroup);

  /**
   * @copydoc Ui::FocusManager::IsFocusGroup
   */
  bool IsFocusGroup(View view) const;

  /**
   * @copydoc Ui::FocusManager::GetFocusGroup
   */
  View GetFocusGroup(View view);

  /**
   * @copydoc Ui::FocusManager::SetFocusIndicatorActor
   */
  void SetFocusIndicatorActor(View indicator);

  /**
   * @copydoc Ui::FocusManager::GetFocusIndicatorView
   */
  View GetFocusIndicatorView();

  /**
   * Move current focus to backward
   */
  void MoveFocusBackward();

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
   * @copydoc Ui::FocusManager::FocusChangedSignal()
   */
  Ui::FocusManager::FocusChangedSignalType& FocusChangedSignal();

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
  typedef std::vector<WeakHandle<View>> FocusStack;         ///< Focus history stack
  typedef FocusStack::iterator          FocusStackIterator; ///< Define FocusStack::Iterator as FocusStackIterator to navigate FocusStack

  struct ParentNavigationResult
  {
    View candidate;
    View focusGroupBoundary;
  };

  /**
   * This will be called when the adaptor is initialized
   */
  void OnAdaptorInit();

  /**
   * This will be called when a new scene holder is created
   * @param sceneHolder The new scene holder
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder);

  /**
   * Get configuration from UiConfig.
   */
  void GetConfiguration();

  /**
   * Step 1: Find next focus from explicit directional properties.
   */
  View FindNextFocusByProperty(View currentFocusView, Ui::FocusDirection direction);

  /**
   * Step 2: Find next focus by propagating up the parent chain.
   * Stops at FocusGroup boundary and records it when encountered.
   */
  ParentNavigationResult FindNextFocusByParentNavigation(View currentFocusView, Ui::FocusDirection direction);

  /**
   * Step 3: Find next focus using FocusFinder (geometry or linear ordering).
   * Uses focusGroup as search root if present.
   */
  View FindNextFocusByFinder(View currentFocusView, View focusGroup, Ui::FocusDirection direction);

  /**
   * Commit the focus change to the specified view. No validation is performed —
   * the caller must ensure the view is a valid focus target.
   * @param view The view to receive focus
   * @param context The context that caused the focus change (device, name)
   * @return Whether the focus commit is successful or not
   */
  bool DoSetCurrentFocusView(View view, const FocusChangeContext& context);

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] sceneHolder The scene holder
   * @param[in] event The KeyEvent event.
   */
  void OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, KeyEvent event);

  /**
   * Callback for the touch event when the screen is touched and when the touch ends
   * (i.e. the down & up touch events only).
   * @param[in] sceneHolder The scene holder
   * @param[in] touch The touch information
   */
  void OnTouch(Dali::Integration::SceneHolder sceneHolder, TouchEvent touch);

  /**
   * Callback for the wheel event when the custom wheel event occurs.
   * @param[in] sceneHolder The scene holder
   * @param[in] wheel The WheelEvent information
   */
  bool OnCustomWheelEvent(Dali::Integration::SceneHolder sceneHolder, WheelEvent wheel);

  /**
   * Callback for the wheel event when the wheel event occurs.
   * @param[in] sceneHolder The scene holder
   * @param[in] wheel The WheelEvent information
   */
  void OnWheelEvent(Dali::Integration::SceneHolder sceneHolder, WheelEvent wheel);

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
   * Get the focus View from current window
   */
  View GetFocusViewFromCurrentWindow();

  /**
   * Convert Device::Class to FocusDevice
   * @param deviceClass The device class from the touch event
   * @return The corresponding FocusDevice
   */
  Ui::FocusDevice ConvertDeviceClassToKeyboardFocusDevice(Device::Class::Type deviceClass) const;

  /**
   * Recursively deliver events to the view and its parents, until the event is consumed or the stage is reached.
   * @param[in]  view  The view got WheelEvent.
   * @param[in]  event  The WheelEvent.
   * @return True if WheelEvent is consumed.
   */
  bool EmitCustomWheelSignals(View view, const WheelEvent& event);

  /**
   * Clear the focus view
   * @param[in] view View to be cleared of focus
   */
  void ClearFocus(View view);

  /**
   * Clear the focus indicator view.
   * @param[in] view View to be cleared of focus indicator.
   */
  void ClearFocusIndicator(View view);

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
  Ui::FocusManager::FocusChangedSignalType mFocusChangedSignal; ///< The signal to notify the focus change
  WeakHandle<View>                         mCurrentFocusView;   ///< A weak handle to the current focused view

  View mFocusIndicatorView; ///< The focus indicator view shared by all the keyboard focusable views for highlight

  FocusStack                                                          mFocusHistory; ///< Stack to contain pre-focused view history
  SlotDelegate<FocusManager>                                          mSlotDelegate;
  typedef std::vector<std::pair<WeakHandle<Layer>, WeakHandle<View>>> FocusViewContainer;
  FocusViewContainer                                                  mCurrentFocusViews;     ///< A container of focused views per window
  WeakHandle<Layer>                                                   mCurrentFocusedWindow;  ///< A weak handle to the current focused window's root layer
  FocusIndicatorState                                                 mIsFocusIndicatorShown; ///< Whether indicator should be shown / hidden when getting focus. It could be enabled
                                                                                              ///< when keyboard focus feature is enabled and navigation keys or 'Tab' key are pressed.
  EnableFocusedIndicatorState mEnableFocusIndicator;                                          ///< Whether use focus indicator
  FocusedIndicatorModeState   mAlwaysShowIndicator;                                           ///< Whether always show indicator. If true, the indicator would be
                                                                                              ///< directly shown when focused

  FocusChangeContext mLastFocusChangeContext; ///< The last focus change context (device & name)

  uint32_t mCurrentWindowId; ///< The current native window id

  bool mClearFocusOnTouch : 1;           ///< Whether clear focus on touch.
  bool mEnableDefaultAlgorithm : 1;      ///< Whether use default algorithm focus
  bool mClearFocusOnWindowFocusLost : 1; ///< Whether clear focus when window loses focus
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
