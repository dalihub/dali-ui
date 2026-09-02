/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#include "window-frame-decoration-validator.h"

namespace Dali::Ui::Internal
{
namespace
{
using ViewArray = std::array<Dali::Ui::View, DECORATION_SLOT_COUNT>;

ViewArray GetSlotRoots(const WindowFrameDecoration& decoration)
{
  return {decoration.GetTopSlot(),
          decoration.GetBottomSlot(),
          decoration.GetLeftSlot(),
          decoration.GetRightSlot(),
          decoration.GetBottomLeftResizeHandle(),
          decoration.GetBottomRightResizeHandle()};
}

ViewArray GetRoles(const WindowFrameDecoration& decoration)
{
  return {decoration.GetMoveRegion(),
          decoration.GetMinimizeControl(),
          decoration.GetMaximizeRestoreControl(),
          decoration.GetCloseControl(),
          decoration.GetBottomLeftResizeHandle(),
          decoration.GetBottomRightResizeHandle()};
}

int32_t FindOwningSlot(const Dali::Ui::View& role, const ViewArray& slots)
{
  if(!role)
  {
    return -1;
  }

  Dali::Actor current = role;
  while(current)
  {
    for(std::size_t index = 0u; index < slots.size(); ++index)
    {
      if(slots[index] && current == slots[index])
      {
        return static_cast<int32_t>(index);
      }
    }
    current = current.GetParent();
  }
  return -2;
}

template<std::size_t Size>
bool HasDuplicateNonZero(const std::array<uint32_t, Size>& ids)
{
  for(std::size_t left = 0u; left < ids.size(); ++left)
  {
    if(ids[left] == 0u)
    {
      continue;
    }
    for(std::size_t right = left + 1u; right < ids.size(); ++right)
    {
      if(ids[left] == ids[right])
      {
        return true;
      }
    }
  }
  return false;
}
} // unnamed namespace

WindowFrameDecorationResult ValidateDecorationTopology(const DecorationTopology& topology)
{
  if(topology.hasParentedSlot)
  {
    return WindowFrameDecorationResult::SLOT_ALREADY_PARENTED;
  }
  if(HasDuplicateNonZero(topology.slotIds))
  {
    return WindowFrameDecorationResult::DUPLICATE_SLOT;
  }
  if(HasDuplicateNonZero(topology.roleIds))
  {
    return WindowFrameDecorationResult::DUPLICATE_ROLE;
  }
  for(std::size_t index = 0u; index < topology.roleIds.size(); ++index)
  {
    if(topology.roleIds[index] != 0u && topology.roleOwners[index] < 0)
    {
      return WindowFrameDecorationResult::ROLE_OUTSIDE_DECORATION;
    }
  }
  return WindowFrameDecorationResult::INSTALLED;
}

WindowFrameDecorationResult ValidateWindowFrameDecoration(const WindowFrameDecoration& decoration, Dali::Actor installedSlotParent)
{
  const ViewArray slots = GetSlotRoots(decoration);
  const ViewArray roles = GetRoles(decoration);

  DecorationTopology topology;
  for(std::size_t index = 0u; index < slots.size(); ++index)
  {
    if(slots[index])
    {
      topology.slotIds[index] = slots[index].GetId();

      // A slot held by the frame that is about to reinstall it is not a foreign
      // owner, so resubmitting the installed decoration stays valid.
      const Dali::Actor parent        = slots[index].GetParent();
      const bool        foreignParent = parent && !(installedSlotParent && parent == installedSlotParent);
      topology.hasParentedSlot        = topology.hasParentedSlot || foreignParent;
    }
  }
  for(std::size_t index = 0u; index < roles.size(); ++index)
  {
    if(roles[index])
    {
      topology.roleIds[index] = roles[index].GetId();
    }
    topology.roleOwners[index] = FindOwningSlot(roles[index], slots);
  }
  return ValidateDecorationTopology(topology);
}

const char* WindowFrameDecorationResultName(WindowFrameDecorationResult result)
{
  switch(result)
  {
    case WindowFrameDecorationResult::INSTALLED:
      return "installed";
    case WindowFrameDecorationResult::SLOT_ALREADY_PARENTED:
      return "slot-already-parented";
    case WindowFrameDecorationResult::DUPLICATE_SLOT:
      return "duplicate-slot";
    case WindowFrameDecorationResult::DUPLICATE_ROLE:
      return "duplicate-role";
    case WindowFrameDecorationResult::ROLE_OUTSIDE_DECORATION:
      return "role-outside-decoration";
  }
  return "unknown";
}

} // namespace Dali::Ui::Internal
