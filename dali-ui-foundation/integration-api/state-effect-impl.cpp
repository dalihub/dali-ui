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
#include <dali-ui-foundation/integration-api/state-effect-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

StateEffectImpl::StateEffectImpl() = default;

StateEffectImpl::~StateEffectImpl() = default;

void StateEffectImpl::OnViewStateChanged(Ui::View /*view*/, const StateEvent& /*event*/)
{
}

void StateEffectImpl::OnInteractiveAttached(Ui::View /*view*/)
{
}

void StateEffectImpl::OnStateEffectTargetsChanged(Ui::View /*view*/)
{
}

bool StateEffectImpl::ShouldSuppressDefaultFocusIndicator(Ui::View /*view*/) const
{
  return false;
}

void StateEffectImpl::InvalidateDefaultFocusIndicatorSuppression(Ui::View view)
{
  if(view)
  {
    GetImpl(view).InvalidateDefaultFocusIndicatorSuppression(*this);
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
