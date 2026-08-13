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
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/navigator/navigation-transition-spec.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Internal data object for NavigationTransitionSpec.
 */
class NavigationTransitionSpecImpl : public Dali::BaseObject
{
public:
  NavigationTransitionSpecImpl();
  ~NavigationTransitionSpecImpl() override;

  NavigationTransitionSpec::AnimatorSignalType& EnterSignal();
  NavigationTransitionSpec::AnimatorSignalType& ExitSignal();
  NavigationTransitionSpec::AnimatorSignalType& PopEnterSignal();
  NavigationTransitionSpec::AnimatorSignalType& PopExitSignal();
  NavigationTransitionSpec::SnapSignalType&     SnapIncomingSignal();
  NavigationTransitionSpec::SnapSignalType&     SnapOutgoingSignal();

  void  SetDuration(float duration);
  float GetDuration() const;

private:
  NavigationTransitionSpecImpl(const NavigationTransitionSpecImpl&)            = delete;
  NavigationTransitionSpecImpl(NavigationTransitionSpecImpl&&)                 = delete;
  NavigationTransitionSpecImpl& operator=(const NavigationTransitionSpecImpl&) = delete;
  NavigationTransitionSpecImpl& operator=(NavigationTransitionSpecImpl&&)      = delete;

private:
  NavigationTransitionSpec::AnimatorSignalType mEnterSignal;
  NavigationTransitionSpec::AnimatorSignalType mExitSignal;
  NavigationTransitionSpec::AnimatorSignalType mPopEnterSignal;
  NavigationTransitionSpec::AnimatorSignalType mPopExitSignal;
  NavigationTransitionSpec::SnapSignalType     mSnapIncomingSignal;
  NavigationTransitionSpec::SnapSignalType     mSnapOutgoingSignal;
  float                                        mDuration{0.0f};
};

} // namespace Integration

inline Integration::NavigationTransitionSpecImpl& GetImplementation(Ui::NavigationTransitionSpec& handle)
{
  DALI_ASSERT_ALWAYS(handle && "NavigationTransitionSpec handle is empty");
  return static_cast<Integration::NavigationTransitionSpecImpl&>(handle.GetBaseObject());
}

inline const Integration::NavigationTransitionSpecImpl& GetImplementation(const Ui::NavigationTransitionSpec& handle)
{
  DALI_ASSERT_ALWAYS(handle && "NavigationTransitionSpec handle is empty");
  return static_cast<const Integration::NavigationTransitionSpecImpl&>(handle.GetBaseObject());
}

} // namespace Ui
} // namespace Dali
