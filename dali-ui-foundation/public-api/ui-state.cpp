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
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <string>
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

uint64_t UiState::Register(StringView name)
{
  auto&       reg = GetRegistry();
  std::string key(name.Data(), name.Size());

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

UiState UiState::Create(StringView name)
{
  return UiState(Register(name));
}

String UiState::ToString() const
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

  return String(result.c_str());
}

// --- Predefined States ---

const UiState UiState::NORMAL{};
const UiState UiState::ALL{(1ULL << MAX_BITS) - 1ULL};
const UiState UiState::FOCUSED         = UiState::Create("Focused");
const UiState UiState::PRESSED         = UiState::Create("Pressed");
const UiState UiState::DISABLED        = UiState::Create("Disabled");
const UiState UiState::PSEUDO_DISABLED = UiState::Create("PseudoDisabled");
const UiState UiState::SELECTED        = UiState::Create("Selected");

// --- Predefined Composite States ---

const UiState UiState::SELECTED_PRESSED  = UiState::SELECTED + UiState::PRESSED;
const UiState UiState::DISABLED_SELECTED = UiState::DISABLED + UiState::SELECTED;

const UiState UiState::SELECTED_FOCUSED = UiState::SELECTED + UiState::FOCUSED;

} // namespace Ui

} // namespace Dali
