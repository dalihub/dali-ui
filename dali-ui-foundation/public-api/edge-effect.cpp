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
#include <dali-ui-foundation/public-api/edge-effect.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/edge-effect-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Ui
{

EdgeEffect EdgeEffect::New()
{
  EdgeEffectImplPtr impl = EdgeEffectImpl::New();
  return EdgeEffect(impl.Get());
}

EdgeEffect::EdgeEffect(EdgeEffectImpl* impl)
: BaseHandle(impl)
{
}

EdgeEffectImpl& EdgeEffect::GetImpl()
{
  DALI_ASSERT_ALWAYS(*this && "EdgeEffect handle is empty");
  return static_cast<EdgeEffectImpl&>(GetBaseObject());
}

const EdgeEffectImpl& EdgeEffect::GetImpl() const
{
  DALI_ASSERT_ALWAYS(*this && "EdgeEffect handle is empty");
  return static_cast<const EdgeEffectImpl&>(GetBaseObject());
}

void EdgeEffect::SetSource(View source)
{
  GetImpl().SetSource(source);
}

View EdgeEffect::GetSource() const
{
  return GetImpl().GetSource();
}

EdgeEffect::State EdgeEffect::GetState() const
{
  return GetImpl().GetState();
}

void EdgeEffect::OnPull(float deltaDistance, float displacement)
{
  GetImpl().OnPull(deltaDistance, displacement);
}

void EdgeEffect::OnRelease()
{
  GetImpl().OnRelease();
}

void EdgeEffect::OnAbsorb(float velocity)
{
  GetImpl().OnAbsorb(velocity);
}

void EdgeEffect::Finish()
{
  GetImpl().Finish();
}

EdgeEffect::PullSignalType& EdgeEffect::PullSignal()
{
  return GetImpl().PullSignal();
}

EdgeEffect::ReleaseSignalType& EdgeEffect::ReleaseSignal()
{
  return GetImpl().ReleaseSignal();
}

EdgeEffect::AbsorbSignalType& EdgeEffect::AbsorbSignal()
{
  return GetImpl().AbsorbSignal();
}

EdgeEffect::FinishedSignalType& EdgeEffect::FinishedSignal()
{
  return GetImpl().FinishedSignal();
}

} // namespace Ui

} // namespace Dali
