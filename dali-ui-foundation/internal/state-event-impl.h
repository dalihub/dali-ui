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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/state-event.h>
#include <dali-ui-foundation/public-api/ui-state.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

class StateEventImpl;
using StateEventImplPtr = IntrusivePtr<StateEventImpl>;

/**
 * @brief Internal implementation of StateEvent.
 */
class DALI_UI_API StateEventImpl : public BaseObject
{
public:
  /**
   * @brief Creates a new StateEventImpl.
   *
   * @param[in] prev    State before the transition
   * @param[in] current State after the transition
   * @param[in] cause   Input event that caused the transition (may be NONE)
   */
  StateEventImpl(UiState prev, UiState current, InputEvent cause);

  /**
   * @brief Creates a new StateEventImpl.
   */
  static StateEventImplPtr New(UiState prev, UiState current, InputEvent cause);

  /**
   * @copydoc Dali::Ui::StateEvent::GetPrev
   */
  const UiState& GetPrev() const;

  /**
   * @copydoc Dali::Ui::StateEvent::GetCurrent
   */
  const UiState& GetCurrent() const;

  /**
   * @copydoc Dali::Ui::StateEvent::GetCause
   */
  const InputEvent& GetCause() const;

private:
  virtual ~StateEventImpl() override;

  StateEventImpl(const StateEventImpl&)            = delete;
  StateEventImpl(StateEventImpl&&)                 = delete;
  StateEventImpl& operator=(const StateEventImpl&) = delete;
  StateEventImpl& operator=(StateEventImpl&&)      = delete;

private:
  UiState    mPrev;
  UiState    mCurrent;
  InputEvent mCause;
};

} // namespace Internal

inline Internal::StateEventImpl& GetImpl(StateEvent& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::StateEventImpl&>(handle);
}

inline const Internal::StateEventImpl& GetImpl(const StateEvent& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::StateEventImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
