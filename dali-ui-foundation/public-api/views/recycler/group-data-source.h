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
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief Describes the group structure of a grouped RecyclerView list.
 *
 * Implement this and pass it to GroupAdapter::SetDataSource. GroupAdapter
 * uses this to build the flat position index that RecyclerView operates on.
 * Call GroupAdapter::NotifyDataSetChanged after the source data changes.
 *
 * @note This is a public subclassing ABI. Its virtual function set is frozen
 * after the first release. Future optional group-source behavior must be
 * exposed through a separate versioned extension interface rather than by
 * adding virtual functions to this class.
 */
class DALI_UI_API GroupDataSource
{
public:
  virtual ~GroupDataSource() = default;

  virtual uint32_t GetGroupCount() const = 0;

  // Number of body items in the specified group (not counting header or gap).
  virtual uint32_t GetGroupItemCount(uint32_t groupIndex) const = 0;

  // Return true if the group has a header row above the body items.
  virtual bool HasGroupHeader(uint32_t groupIndex) const
  {
    return false;
  }
};

} // namespace Ui
} // namespace Dali
