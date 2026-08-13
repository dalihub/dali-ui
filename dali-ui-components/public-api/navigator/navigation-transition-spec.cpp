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
#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/navigator/navigation-transition-spec-impl.h>

namespace Dali
{
namespace Ui
{

NavigationTransitionSpec::NavigationTransitionSpec() = default;

NavigationTransitionSpec NavigationTransitionSpec::New()
{
  return NavigationTransitionSpec(new Integration::NavigationTransitionSpecImpl());
}

NavigationTransitionSpec::NavigationTransitionSpec(const NavigationTransitionSpec& other) = default;

NavigationTransitionSpec::NavigationTransitionSpec(NavigationTransitionSpec&& rhs) noexcept = default;

NavigationTransitionSpec::~NavigationTransitionSpec() = default;

NavigationTransitionSpec& NavigationTransitionSpec::operator=(const NavigationTransitionSpec& other) = default;

NavigationTransitionSpec& NavigationTransitionSpec::operator=(NavigationTransitionSpec&& rhs) noexcept = default;

NavigationTransitionSpec NavigationTransitionSpec::DownCast(BaseHandle handle)
{
  NavigationTransitionSpec result;
  if(handle)
  {
    auto* implementation = dynamic_cast<Integration::NavigationTransitionSpecImpl*>(&handle.GetBaseObject());
    if(implementation)
    {
      result = NavigationTransitionSpec(implementation);
    }
  }
  return result;
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpec::EnterSignal()
{
  return GetImplementation(*this).EnterSignal();
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpec::ExitSignal()
{
  return GetImplementation(*this).ExitSignal();
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpec::PopEnterSignal()
{
  return GetImplementation(*this).PopEnterSignal();
}

NavigationTransitionSpec::AnimatorSignalType& NavigationTransitionSpec::PopExitSignal()
{
  return GetImplementation(*this).PopExitSignal();
}

NavigationTransitionSpec::SnapSignalType& NavigationTransitionSpec::SnapIncomingSignal()
{
  return GetImplementation(*this).SnapIncomingSignal();
}

NavigationTransitionSpec::SnapSignalType& NavigationTransitionSpec::SnapOutgoingSignal()
{
  return GetImplementation(*this).SnapOutgoingSignal();
}

void NavigationTransitionSpec::SetDuration(float duration)
{
  GetImplementation(*this).SetDuration(duration);
}

float NavigationTransitionSpec::GetDuration() const
{
  return GetImplementation(*this).GetDuration();
}

NavigationTransitionSpec::NavigationTransitionSpec(Integration::NavigationTransitionSpecImpl* implementation)
: BaseHandle(implementation)
{
}

} // namespace Ui
} // namespace Dali
