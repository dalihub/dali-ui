#ifndef DALI_UI_INTERNAL_KEYINPUT_FOCUS_MANAGER_H
#define DALI_UI_INTERNAL_KEYINPUT_FOCUS_MANAGER_H

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
class SceneHolder;

} // namespace Integration

namespace Ui
{
namespace Internal
{
class KeyInputFocusManager;

/**
 * @copydoc KeyInputFocusManager
 */
class KeyInputFocusManagerImpl : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  /**
   * Construct a new KeyInputFocusManagerImpl.
   */
  KeyInputFocusManagerImpl();

  /**
   * @copydoc KeyInputFocusManager::SetFocus(View)
   */
  void SetFocus(Ui::View view);

  /**
   * @copydoc KeyInputFocusManager::RemoveFocus(View)
   */
  void RemoveFocus(Ui::View view);

  /**
   * @copydoc KeyInputFocusManager::GetCurrentFocusView()const
   */
  Ui::View GetCurrentFocusView() const;

  /**
   * @copydoc KeyInputFocusManager::GetCurrentFocusView(Window)const
   */
  Ui::View GetCurrentFocusView(Window window) const;

  /**
   * @copydoc KeyInputFocusManager::SetIndependentWindow(Window,bool)
   */
  void SetIndependentWindow(Window window, bool enabled);

  /**
   * @copydoc KeyInputFocusManager::SetPrimaryWindow(Window)
   */
  void SetPrimaryWindow(Window window);

public:
  /**
   * @copydoc KeyInputFocusManager::KeyInputFocusChangedSignal()
   */
  KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusChangedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is
   * responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName,
                              FunctorDelegate* functor);

protected:
  /**
   * Destructor
   */
  virtual ~KeyInputFocusManagerImpl();

private:
  /**
   * @brief Checks whether any primary or independent scope still owns the target.
   * @param[in] view The target to query
   * @return True if a live focus scope owns it
   */
  bool HasFocusTarget(Ui::View view) const;

  /**
   * @brief Disconnects target observers and notifies one actual focus loss.
   * @param[in] view The non-empty target whose ownership was already removed
   * @param[in] window Its owning Window, including during scene disconnection
   */
  void NotifyFocusLost(Ui::View view, Window window);

  /**
   * This will be called when a new scene holder is created
   * @param sceneHolder The new scene holder
   */
  void OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder);

  /**
   * Callback for the key event when no actor in the stage has gained the key input focus
   * @param[in] sceneHolder The scene holder
   * @param[in] event The KeyEvent event.
   */
  bool OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, KeyEvent event);

  /**
   * Signal handler called when a focused View is removed from Scene.
   * @param[in]  view  The view removed from the scene.
   */
  void OnFocusViewSceneDisconnection(Dali::Actor view);

  /**
   * @brief Removes an independent key target when its effective visibility is lost.
   * @param[in] actor The View whose effective visibility changed
   * @param[in] visible Whether the View is effectively visible
   */
  void OnFocusViewVisibilityChanged(Dali::Actor actor, bool visible);

  /**
   *  Recursively deliver events to the view and its parents, until the event is consumed or the stage is reached.
   * @param[in]  view  The view got KeyEvent.
   * @param[in]  event    The KeyEvent.
   * @return True if KeyEvent is consumed.
   */
  bool NotifyKeyEvent(Ui::View view, const KeyEvent& event);

  /**
   * Gets the current native window id
   */
  uint32_t GetCurrentWindowId() const;

private:
  // Undefined
  KeyInputFocusManagerImpl(const KeyInputFocusManagerImpl&);

  KeyInputFocusManagerImpl& operator=(const KeyInputFocusManagerImpl& rhs);

private:
  // The key input focus change signal
  KeyInputFocusManager::KeyInputFocusChangedSignalType mKeyInputFocusChangedSignal;

  SlotDelegate<KeyInputFocusManagerImpl> mSlotDelegate;

  Ui::View mCurrentFocusView; ///< The current focused view
  uint32_t mCurrentWindowId;  ///< The native window id of current focused view

  struct WindowKeyTarget
  {
    WeakHandle<Window> window;
    WeakHandle<View>   view;
  };
  std::vector<WindowKeyTarget> mWindowTargets;
};

inline KeyInputFocusManagerImpl& GetImpl(KeyInputFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<KeyInputFocusManagerImpl&>(handle);
}

inline const KeyInputFocusManagerImpl& GetImpl(const KeyInputFocusManager& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const KeyInputFocusManagerImpl&>(handle);
}

} // namespace Internal

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_INTERNAL_KEYINPUT_FOCUS_MANAGER_H
