#pragma once

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

#include <dali-ui-components/public-api/window/window-frame-decoration.h>

#include <dali/public-api/actors/actor.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace Dali::Ui::Internal
{
constexpr std::size_t DECORATION_SLOT_COUNT = 6u;
constexpr std::size_t DECORATION_ROLE_COUNT = 6u;

struct DecorationTopology
{
  std::array<uint32_t, DECORATION_SLOT_COUNT> slotIds{};
  std::array<uint32_t, DECORATION_ROLE_COUNT> roleIds{};
  std::array<int32_t, DECORATION_ROLE_COUNT>  roleOwners{-1, -1, -1, -1, -1, -1};
  bool                                        hasParentedSlot{false};
};

WindowFrameDecorationResult ValidateDecorationTopology(const DecorationTopology& topology);

/**
 * @brief Validates slot and role topology before a decoration is installed.
 *
 * @param[in] decoration Decoration to validate
 * @param[in] installedSlotParent Parent that already holds the currently
 *            installed slots.  Slots parented to it are accepted so that the
 *            installed decoration can be resubmitted with new layout or
 *            style.  Pass an empty handle to reject every parented slot.
 */
WindowFrameDecorationResult ValidateWindowFrameDecoration(const WindowFrameDecoration& decoration, Dali::Actor installedSlotParent = Dali::Actor());
const char*                 WindowFrameDecorationResultName(WindowFrameDecorationResult result);

} // namespace Dali::Ui::Internal
