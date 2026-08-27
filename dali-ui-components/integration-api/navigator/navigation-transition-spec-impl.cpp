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
#include <dali-ui-components/integration-api/navigator/navigation-transition-spec-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

NavigationTransitionSpecImpl::NavigationTransitionSpecImpl() = default;

NavigationTransitionSpecImpl::~NavigationTransitionSpecImpl() = default;

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpecImpl::EnterSignal()
{
  return mEnterSignal;
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpecImpl::ExitSignal()
{
  return mExitSignal;
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpecImpl::PopEnterSignal()
{
  return mPopEnterSignal;
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpecImpl::PopExitSignal()
{
  return mPopExitSignal;
}

NavigationTransitionSpec::SnapSignalType& NavigationTransitionSpecImpl::SnapIncomingSignal()
{
  return mSnapIncomingSignal;
}

NavigationTransitionSpec::SnapSignalType& NavigationTransitionSpecImpl::SnapOutgoingSignal()
{
  return mSnapOutgoingSignal;
}

void NavigationTransitionSpecImpl::SetDuration(float duration)
{
  mDuration = duration > 0.0f ? duration : 0.0f;
}

float NavigationTransitionSpecImpl::GetDuration() const
{
  return mDuration;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
