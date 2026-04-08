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
#include <dali-ui-foundation/internal/state-event-impl.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

StateEventImpl::StateEventImpl(UiState prev, UiState current, InputEvent cause)
: BaseObject(),
  mPrev(prev),
  mCurrent(current),
  mCause(cause)
{
}

StateEventImpl::~StateEventImpl()
{
}

StateEventImplPtr StateEventImpl::New(UiState prev, UiState current, InputEvent cause)
{
  return StateEventImplPtr(new StateEventImpl(prev, current, cause));
}

const UiState& StateEventImpl::GetPrev() const
{
  return mPrev;
}

const UiState& StateEventImpl::GetCurrent() const
{
  return mCurrent;
}

const InputEvent& StateEventImpl::GetCause() const
{
  return mCause ? mCause : InputEvent::None();
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
