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
#include <dali-ui-foundation/public-api/ui-state.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <unordered_map>

namespace Dali
{

namespace Ui
{

namespace
{

struct StateRegistry
{
  std::unordered_map<std::string, uint64_t> states;
  int                                       nextBit = 0;
};

StateRegistry& GetRegistry()
{
  static StateRegistry registry;
  return registry;
}

} // unnamed namespace

uint64_t UiState::Register(std::string_view name)
{
  auto&       reg = GetRegistry();
  std::string key(name);

  if(key == "All")
  {
    return (1ULL << MAX_BITS) - 1ULL;
  }

  if(key == "Normal")
  {
    return 0ULL;
  }

  auto it = reg.states.find(key);
  if(it != reg.states.end())
  {
    return it->second;
  }

  DALI_ASSERT_ALWAYS(reg.nextBit < MAX_BITS && "No more room to register a new UiState");

  uint64_t bit = 1ULL << reg.nextBit;
  reg.states.emplace(std::move(key), bit);
  reg.nextBit++;
  return bit;
}

UiState UiState::Create(std::string_view name)
{
  return UiState(Register(name));
}

std::string UiState::ToString() const
{
  if(mBits == 0ULL)
  {
    return "Normal";
  }

  constexpr uint64_t fullMask = (1ULL << MAX_BITS) - 1ULL;
  if(mBits == fullMask)
  {
    return "All";
  }

  auto&       reg = GetRegistry();
  std::string result;

  for(const auto& [name, bitMask] : reg.states)
  {
    if((mBits & bitMask) != 0ULL)
    {
      if(!result.empty())
      {
        result += ", ";
      }
      result += name;
    }
  }

  return result;
}

// --- Predefined States ---

const UiState UiState::Normal{};
const UiState UiState::All{(1ULL << MAX_BITS) - 1ULL};
const UiState UiState::Focused        = UiState::Create("Focused");
const UiState UiState::Pressed        = UiState::Create("Pressed");
const UiState UiState::Disabled       = UiState::Create("Disabled");
const UiState UiState::PseudoDisabled = UiState::Create("PseudoDisabled");
const UiState UiState::Selected       = UiState::Create("Selected");

// --- Predefined Composite States ---

const UiState UiState::SelectedPressed  = UiState::Selected + UiState::Pressed;
const UiState UiState::DisabledSelected = UiState::Disabled + UiState::Selected;
const UiState UiState::DisabledFocused  = UiState::Disabled + UiState::Focused;
const UiState UiState::SelectedFocused  = UiState::Selected + UiState::Focused;

} // namespace Ui

} // namespace Dali
