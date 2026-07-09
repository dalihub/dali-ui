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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <cstdint>
#include <limits>

namespace Dali
{
namespace Ui
{

/**
 * @brief Position of an item within its group structure.
 *
 * NONE means the item is not part of any group layout (standard flat list).
 * GAP rows are spacer items injected between groups by GroupAdapter.
 */
enum class GroupRowType
{
  NONE,        ///< Standalone item (no group context)
  HEADER,      ///< Group header
  BODY_SINGLE, ///< Only item in the group body
  BODY_TOP,    ///< First item of a multi-item group body
  BODY_MIDDLE, ///< Interior item of a group body
  BODY_BOTTOM, ///< Last item of a multi-item group body
  GAP,         ///< Spacing row between groups
};

/**
 * @brief Data container for a single recycled item.
 *
 * Passed by reference to all adapter signals. The adapter populates
 * holder.view in CreateViewHolderSignal. GroupAdapter also sets rowType
 * and groupIndex before delegating to the inner adapter.
 */
struct DALI_UI_API ItemViewHolder
{
  static constexpr uint32_t INVALID_GROUP_INDEX = std::numeric_limits<uint32_t>::max();

  uint32_t     position{0u};
  uint32_t     viewType{0u};
  View         view;
  GroupRowType rowType{GroupRowType::NONE};
  uint32_t     groupIndex{INVALID_GROUP_INDEX};
};

} // namespace Ui
} // namespace Dali
