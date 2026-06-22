#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/events/long-press-gesture-detector.h>
#include <dali/public-api/events/tap-gesture-detector.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/interactive-trait.h>

namespace Dali
{

namespace Ui
{

class InputEvent;

namespace Internal
{
class PendingPressManager;
class CoreInteractionObject;

/**
 * @brief Internal implementation of InteractiveTrait stored inside CoreInteractionObject.
 */
class InteractiveTraitImpl : public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::Ui::InteractiveTrait::InteractiveTrait
   */
  InteractiveTraitImpl();

  /**
   * @brief Enable touch-based long press detection.
   *
   * By default, the long press gesture detector is not created.
   * Call this method to enable touch-based long press detection on demand.
   * This is also called automatically when LongPressedSignal() is accessed.
   */
  void EnableLongPressDetection();

public: // Signals
  /**
   * @copydoc Dali::Ui::InteractiveTrait::PressedChangedSignal
   */
  Signal<void(View, bool, InputEvent)>& PressedChangedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::PseudoDisabledChangedSignal
   */
  Signal<void(View, bool)>& PseudoDisabledChangedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::ClickedSignal
   */
  Signal<void(View, InputEvent)>& ClickedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::LongPressedSignal
   */
  Signal<bool(View, InputEvent)>& LongPressedSignal();

public: // API
  /**
   * @copydoc Dali::Ui::InteractiveTrait::IsPressed
   */
  bool IsPressed() const;

  /**
   * @copydoc Dali::Ui::InteractiveTrait::IsPseudoDisabled
   */
  bool IsPseudoDisabled() const;

  /**
   * @copydoc Dali::Ui::InteractiveTrait::SetPseudoDisabled
   */
  void SetPseudoDisabled(bool pseudoDisabled);

  /**
   * @copydoc Dali::Ui::InteractiveTrait::IsClickable
   */
  bool IsClickable() const;

  /**
   * @copydoc Dali::Ui::InteractiveTrait::SetClickable
   */
  void SetClickable(bool clickable);

  /**
   * @copydoc Dali::Ui::InteractiveTrait::GetKeyClickPolicy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @copydoc Dali::Ui::InteractiveTrait::SetKeyClickPolicy
   */
  void SetKeyClickPolicy(KeyClickPolicy policy);

  /**
   * @copydoc Dali::Ui::InteractiveTrait::~InteractiveTrait
   */
  virtual ~InteractiveTraitImpl();

public: // InteractiveTrait
  /**
   * @brief Called when the view gains or loses key input focus.
   */
  virtual void OnFocusedChanged(View view, bool focused);

  /**
   * @brief Called after a key-event is received by the actor that has had its focus set.
   *
   * @return True if the event should be consumed
   */
  virtual bool OnKeyEvent(View view, const KeyEvent& event);

  /**
   * @brief Called when the view becomes enabled or disabled.
   */
  virtual void OnEnabledChanged(View view, bool enabled);

  /**
   * @brief Called when the view is connected to a scene.
   */
  virtual void OnSceneConnection(View view);

  /**
   * @brief Called when the view is disconnected from a scene.
   */
  virtual void OnSceneDisconnection(View view);

protected:
  /**
   * @brief Gets the owner view
   */
  View GetOwner() const;

  void OnAttached(View& view);

  void OnDetaching(View& view);

  void OnViewDestroying(ViewImpl* viewImpl);

  /**
   * @brief This is called when touch input is received.
   */
  virtual bool OnTouch(View view, TouchEvent touchEvent);

  /**
   * @brief This is called when tap gesture is received.
   */
  virtual void OnTap(View view, TapGesture tap);

  /**
   * @brief Pressed changed event. override this method to handle pressed event.
   */
  virtual void OnPressedChanged(View view, InputEvent inputEvent);

  /**
   * @brief Clicked event. override this method to handle clicked event.
   */
  virtual void OnClicked(View view, InputEvent inputEvent);

  /**
   * @brief LongPressed event. override this method to handle long pressed event.
   */
  virtual bool OnLongPressed(View view, InputEvent inputEvent);

  /**
   * @brief Check if the key is for execution.
   */
  virtual bool IsExecutionKey(const Dali::String& keyName) const;

private:
  friend class CoreInteractionObject;
  friend class Dali::Ui::Internal::PendingPressManager;

  bool OnTouchInternal(Actor actor, TouchEvent touchEvent);
  void OnTapInternal(Actor actor, TapGesture event);
  void OnLongPressedInternal(Actor actor, LongPressGesture event);
  void RecordPressedExecutionKey(const Dali::String& keyName);
  void ClearKeyPressedHistory();
  void SetPressedInternal(bool value, InputEvent event);
  bool ShouldTapTriggerClicked() const;
  bool ShouldKeyReleaseTriggerClicked() const;
  bool ShouldKeyPressTriggerClicked() const;
  bool ShouldKeyPressTriggerLongPressed() const;
  bool HandleKeyPressed(View view, InputEvent event);
  bool HandleKeyReleased(View view, InputEvent event);

private:
  WeakHandle<View>                     mOwner;
  TapGestureDetector                   mTapGestureDetector;
  LongPressGestureDetector             mLongPressGestureDetector;
  Signal<void(View, bool, InputEvent)> mPressedChangedSignal;
  Signal<void(View, bool)>             mPseudoDisabledChangedSignal;
  Signal<void(View, InputEvent)>       mClickedSignal;
  Signal<bool(View, InputEvent)>       mLongPressedSignal;
  KeyClickPolicy                       mKeyClickPolicy;
  UniquePtr<Dali::String>              mPressedExecutionKey;
  uint32_t                             mPressedExecutionKeyCount;
  bool                                 mPseudoDisabled : 1;
  bool                                 mPressed : 1;
  bool                                 mClickable : 1;
  bool                                 mClickBlockedByTouch : 1;
  bool                                 mClickBlockedByKey : 1;
};

} // namespace Internal

Internal::InteractiveTraitImpl&       GetImpl(InteractiveTrait& obj);
const Internal::InteractiveTraitImpl& GetImpl(const InteractiveTrait& obj);

} // namespace Ui

} // namespace Dali
