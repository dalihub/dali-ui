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
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/state-effect-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

namespace Dali
{
namespace Ui
{
namespace
{

class NoneStateEffectImpl;

// Non-owning cache. NoneStateEffectImpl clears this from OnDestroy().
NoneStateEffectImpl* gNoneStateEffectImpl = nullptr;

class NoneStateEffectImpl : public Integration::StateEffectImpl
{
protected:
  void OnDestroy() override
  {
    gNoneStateEffectImpl = nullptr;
    BaseObject::OnDestroy();
  }
};

} // namespace

StateEffect StateEffect::DownCast(BaseHandle handle)
{
  return StateEffect(dynamic_cast<Integration::StateEffectImpl*>(handle.GetObjectPtr()));
}

StateEffect StateEffect::None()
{
  if(!gNoneStateEffectImpl)
  {
    gNoneStateEffectImpl = new NoneStateEffectImpl();
  }
  return StateEffect(gNoneStateEffectImpl);
}

StateEffect StateEffect::DefaultForInteractive()
{
  return UiConfig::GetCurrent().GetDefaultStateEffectForInteractive();
}

bool StateEffect::IsNone() const
{
  return dynamic_cast<NoneStateEffectImpl*>(GetObjectPtr()) != nullptr;
}

StateEffect::StateEffect(Integration::StateEffectImpl* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali
