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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class StateEffectTargetTraitImpl;

class StateEffectTargetTrait : public BaseHandle
{
public:
  StateEffectTargetTrait() = default;

  static StateEffectTargetTrait New();
  static StateEffectTargetTrait DownCast(BaseHandle handle);

  StateEffectTargetTraitImpl&       GetImpl();
  const StateEffectTargetTraitImpl& GetImpl() const;

private:
  explicit StateEffectTargetTrait(StateEffectTargetTraitImpl* impl);
};

class StateEffectTargetTraitImpl : public TraitObject
{
public:
  static constexpr int32_t INVALID_TARGET_ID = -1;

  StateEffectTargetTraitImpl();

  void    SetTargetId(int32_t targetId);
  int32_t GetTargetId() const;

protected:
  ~StateEffectTargetTraitImpl() override;

private:
  int32_t mTargetId;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
