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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/interactive-trait-interface.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>

namespace Dali
{

namespace Ui
{

class InputEvent;

namespace Integration
{
/**
 * @brief Internal implementation of Interaction trait.
 */
class DALI_UI_API InteractiveTraitImpl : public TraitImpl, public ConnectionTracker, public IInteractiveTrait
{
public:
  /**
   * @copydoc Dali::Ui::InteractiveTrait::InteractiveTrait
   */
  InteractiveTraitImpl();

public: // Signals
  /**
   * @copydoc Dali::Ui::InteractiveTrait::PressedChangedSignal
   */
  Signal<void(View, bool, const InputEvent&)>& PressedChangedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::PseudoDisabledChangedSignal
   */
  Signal<void(View, bool)>& PseudoDisabledChangedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::ClickedSignal
   */
  Signal<void(View, const InputEvent&)>& ClickedSignal();

  /**
   * @copydoc Dali::Ui::InteractiveTrait::LongPressedSignal
   */
  Signal<bool(View, const InputEvent&)>& LongPressedSignal();

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

public: // InteractiveTrait
  /**
   * @copydoc Dali::Ui::Integration::IInteractiveTrait::OnFocusedChanged
   */
  void OnFocusedChanged(View view, bool focused) override;

  /**
   * @copydoc Dali::Ui::Integration::IInteractiveTrait::OnKeyEvent
   */
  bool OnKeyEvent(View view, const KeyEvent& event) override;

protected:
  /**
   * @copydoc Dali::Ui::InteractiveTrait::~InteractiveTrait
   */
  virtual ~InteractiveTraitImpl() override;

  /**
   * @brief Gets the owner view
   */
  View GetOwner() const;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnBeforeAttached
   */
  void OnBeforeAttached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnAttached
   */
  void OnAttached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnDetached
   */
  void OnDetached(TraitId id, View& view) override;

  /**
   * @copydoc Dali::Ui::TraitImpl::OnViewDestroying
   */
  void OnViewDestroying(ViewImpl* viewImpl) override;

  /**
   * @brief This is called when touch input is received.
   */
  virtual bool OnTouch(View view, const TouchEvent& touchEvent);

  /**
   * @brief This is called when tap gesture is received.
   */
  virtual void OnTap(View view, const TapGesture& tap);

  /**
   * @brief Pressed changed event. override this method to handle pressed event.
   */
  virtual void OnPressedChanged(View view, const InputEvent& inputEvent);

  /**
   * @brief Clicked event. override this method to handle clicked event.
   */
  virtual void OnClicked(View view, const InputEvent& inputEvent);

  /**
   * @brief LongPressed event. override this method to handle long pressed event.
   */
  virtual bool OnLongPressed(View view, const InputEvent& inputEvent);

  /**
   * @brief Check if the key is for execution.
   */
  virtual bool IsExecutionKey(const std::string& keyName) const;

private:
  bool OnTouchInternal(Actor actor, const TouchEvent& touchEvent);
  void OnTapInternal(Actor actor, const TapGesture& event);
  void OnLongPressedInternal(Actor actor, const LongPressGesture& event);
  void RecordPressedExecutionKey(const std::string& keyName);
  void ClearKeyPressedHistory();
  void SetPressedInternal(bool value, const InputEvent& event);
  bool ShouldTapTriggerClicked() const;
  bool ShouldKeyReleaseTriggerClicked() const;
  bool ShouldKeyPressTriggerClicked() const;
  bool ShouldKeyPressTriggerLongPressed() const;
  bool HandleKeyPressed(View view, const InputEvent& event);
  bool HandleKeyReleased(View view, const InputEvent& event);

private:
  WeakHandle<View>                            mOwner;
  TapGestureDetector                          mTapGestureDetector;
  LongPressGestureDetector                    mLongPressGestureDetector;
  Signal<void(View, bool, const InputEvent&)> mPressedChangedSignal;
  Signal<void(View, bool)>                    mPseudoDisabledChangedSignal;
  Signal<void(View, const InputEvent&)>       mClickedSignal;
  Signal<bool(View, const InputEvent&)>       mLongPressedSignal;
  KeyClickPolicy                              mKeyClickPolicy;
  std::string                                 mPressedExecutionKey;
  uint32_t                                    mPressedExecutionKeyCount;
  bool                                        mPseudoDisabled : 1;
  bool                                        mPressed : 1;
  bool                                        mClickable : 1;
  bool                                        mClickBlockedByTouch : 1;
  bool                                        mClickBlockedByKey : 1;
};

} // namespace Integration

inline Integration::InteractiveTraitImpl& GetImpl(InteractiveTrait& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::InteractiveTraitImpl&>(handle);
}

inline const Integration::InteractiveTraitImpl& GetImpl(const InteractiveTrait& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::InteractiveTraitImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
