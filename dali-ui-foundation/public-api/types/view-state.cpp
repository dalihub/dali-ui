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
#include <dali-ui-foundation/public-api/types/view-state.h>

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
  std::unordered_map<std::string, uint32_t> states;
  int                                       nextBit = 0;
};

StateRegistry& GetRegistry()
{
  static StateRegistry registry;
  return registry;
}

} // unnamed namespace

uint32_t ViewState::Register(StringView name)
{
  auto&       reg = GetRegistry();
  std::string key(name.Data(), name.Size());

  if(key == "All")
  {
    return UINT32_MAX;
  }

  if(key == "Normal")
  {
    return 0u;
  }

  auto it = reg.states.find(key);
  if(it != reg.states.end())
  {
    return it->second;
  }

  DALI_ASSERT_ALWAYS(reg.nextBit < MAX_BITS && "No more room to register a new ViewState");

  uint32_t bit = 1u << reg.nextBit;
  reg.states.emplace(std::move(key), bit);
  reg.nextBit++;
  return bit;
}

ViewState ViewState::Create(StringView name)
{
  return ViewState(Register(name));
}

String ViewState::ToString() const
{
  if(mBits == 0u)
  {
    return "Normal";
  }

  constexpr uint32_t fullMask = UINT32_MAX;
  if(mBits == fullMask)
  {
    return "All";
  }

  auto&       reg = GetRegistry();
  std::string result;

  for(const auto& [name, bitMask] : reg.states)
  {
    if((mBits & bitMask) != 0u)
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

const ViewState ViewState::NORMAL{};
const ViewState ViewState::ALL{UINT32_MAX};
const ViewState ViewState::FOCUSED         = ViewState::Create("Focused");
const ViewState ViewState::FOCUS_INDICATED = ViewState::Create("FocusIndicated");
const ViewState ViewState::PRESSED         = ViewState::Create("Pressed");
const ViewState ViewState::DISABLED        = ViewState::Create("Disabled");
const ViewState ViewState::PSEUDO_DISABLED = ViewState::Create("PseudoDisabled");
const ViewState ViewState::HOVERED         = ViewState::Create("Hovered");
const ViewState ViewState::SELECTED        = ViewState::Create("Selected");

// --- Predefined Composite States ---

const ViewState ViewState::SELECTED_PRESSED  = ViewState::SELECTED + ViewState::PRESSED;
const ViewState ViewState::DISABLED_SELECTED = ViewState::DISABLED + ViewState::SELECTED;

const ViewState ViewState::SELECTED_FOCUSED = ViewState::SELECTED + ViewState::FOCUSED;

} // namespace Ui

} // namespace Dali
