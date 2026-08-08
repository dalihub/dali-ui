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
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/focus-indication-policy.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view.h>

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
    Ui::InputEvent  inputEvent = Ui::InputEvent::Programmatic();
    Window          window;
    bool            focusIndicated = false;
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
   * @brief Sets focus directly to the given view with the state-change cause.
   *
   * @param view The view to be focused
   * @param cause The event that caused the focus change
   * @return Whether the focus was successfully set
   */
  bool SetCurrentFocusView(View view, InputEvent cause);

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
   * @copydoc Ui::FocusManager::SetFocusNavigationFallback
   */
  void SetFocusNavigationFallback(FocusNavigationCallback callback);

  /**
   * @copydoc Ui::FocusManager::ClearFocus
   */
  void ClearFocus();

  /**
   * @copydoc Ui::FocusManager::ClearFocusIndication
   */
  void ClearFocusIndication(InputEvent cause = InputEvent::Programmatic());

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
   * Move current focus to backward
   */
  void MoveFocusBackward();

  /**
   * @copydoc Ui::FocusManager::SetDefaultFocusIndicatorEnabled
   */
  void SetDefaultFocusIndicatorEnabled(bool enabled);

  /**
   * @copydoc Ui::FocusManager::IsDefaultFocusIndicatorEnabled
   */
  bool IsDefaultFocusIndicatorEnabled() const;

  /**
   * Decide using default focus algorithm or not.
   */
  void EnableDefaultAlgorithm(bool enable);

  /**
   * Check whether default focus algorithm is enabled.
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

  /**
   * @copydoc Ui::FocusManager::SetClearFocusIndicationOnTouch
   */
  void SetClearFocusIndicationOnTouch(bool clear);

  /**
   * @copydoc Ui::FocusManager::IsClearFocusIndicationOnTouchEnabled
   */
  bool IsClearFocusIndicationOnTouchEnabled() const;

  /**
   * @copydoc Ui::FocusManager::SetClearFocusIndicationOnHover
   */
  void SetClearFocusIndicationOnHover(bool clear);

  /**
   * @copydoc Ui::FocusManager::IsClearFocusIndicationOnHoverEnabled
   */
  bool IsClearFocusIndicationOnHoverEnabled() const;

  /**
   * Re-applies focus indicator visibility for the current focused view.
   * @param[in] view The view whose focus indicator setting changed
   */
  void RefreshFocusIndicator(View view);

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
    FocusNavigationResult result = FocusNavigationResult::NotHandled();
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
   * Finds an explicitly configured directional target.
   */
  FocusNavigationResult FindNextFocusByProperty(View currentFocusView, Ui::FocusDirection direction);

  /**
   * Propagates navigation through the parent chain, including the Focus Group
   * View itself, and stops before policies outside that boundary.
   */
  ParentNavigationResult FindNextFocusByParentNavigation(View currentFocusView, FocusNavigationContext context);

  /**
   * Finds the framework fallback using geometry or linear ordering.
   * Uses focusGroup as the search root when present.
   */
  View FindNextFocusByFinder(View currentFocusView, FocusNavigationContext context);

  /**
   * Creates the immutable public context shared by all policies for one request.
   */
  FocusNavigationContext CreateFocusNavigationContext(View currentFocusView, Ui::FocusDirection direction, const FocusChangeContext& context);

  /**
   * Resolves and validates a handled navigation result, then commits a move.
   */
  bool ApplyFocusNavigationResult(const FocusNavigationResult& result, View originalFocusView, FocusNavigationContext context, const FocusChangeContext& changeContext);

  /**
   * Checks whether a candidate remains in the request's Window and Focus Group.
   */
  bool IsValidNavigationCandidate(View candidate, FocusNavigationContext context) const;

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

  bool OnHover(Actor actor, HoverEvent hover);

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
   * Detaches the shared focus indicator from a view.
   * @param[in] view View to detach the focus indicator from
   */
  void DetachFocusIndicator(View view);

  View GetFocusIndicatorView();

  void SetFocusIndicated(View view, bool indicated, InputEvent cause = InputEvent::Programmatic());

  bool ShouldIndicateFocus(const FocusChangeContext& context, bool previousFocusIndicated) const;

  /**
   * Resolves the policy decision for focusedView while it remains the focused view,
   * then applies the result via SetFocusIndicated().
   */
  void SetFocusIndicationWithPolicy(View focusedView, bool proposedIndicated, FocusDevice device, InputEvent inputEvent);

  /**
   * Clears the pending touch focus candidate.
   */
  void ClearTouchFocusCandidate();

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
  Ui::FocusManager::FocusChangedSignalType mFocusChangedSignal;  ///< The signal to notify the focus change
  WeakHandle<View>                         mCurrentFocusView;    ///< A weak handle to the current focused view
  WeakHandle<View>                         mTouchFocusCandidate; ///< A weak handle to the view that may receive focus when touch is released

  View mFocusIndicatorView; ///< The focus indicator view shared by all the keyboard focusable views for highlight

  FocusStack                                                          mFocusHistory; ///< Stack to contain pre-focused view history
  SlotDelegate<FocusManager>                                          mSlotDelegate;
  typedef std::vector<std::pair<WeakHandle<Layer>, WeakHandle<View>>> FocusViewContainer;
  FocusViewContainer                                                  mCurrentFocusViews;    ///< A container of focused views per window
  WeakHandle<Layer>                                                   mCurrentFocusedWindow; ///< A weak handle to the current focused window's root layer

  FocusChangeContext mLastFocusChangeContext; ///< The last focus change context (device & name)

  Extension::FocusIndicationPolicy::Function mFocusIndicationPolicy; ///< Extension policy for automatic focus indication changes.

  FocusNavigationCallback mFocusNavigationFallback; ///< Application-wide policy invoked before FocusFinder.

  uint32_t mCurrentWindowId;      ///< The current native window id
  int32_t  mTouchFocusDeviceId;   ///< The device id for the pending touch focus candidate
  bool     mNavigationInProgress; ///< Prevents focus mutation and nested navigation from policy callbacks.

  bool mDefaultFocusIndicatorEnabled : 1; ///< Whether FocusManager's default focus indicator is enabled.
  bool mClearFocusIndicationOnTouch : 1;  ///< Whether touch outside the focused view clears focus indication.
  bool mClearFocusIndicationOnHover : 1;  ///< Whether hover outside the focused view clears focus indication.
  bool mConfigurationLoaded : 1;          ///< Whether default configuration has been loaded from UiConfig.
  bool mEnableDefaultAlgorithm : 1;       ///< Whether use default algorithm focus.
  bool mClearFocusOnWindowFocusLost : 1;  ///< Whether clear focus when window loses focus.
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
