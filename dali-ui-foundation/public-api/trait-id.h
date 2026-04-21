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
 * @brief A lightweight handle representing a unique identifier for traits.
 *
 * TraitId values are allocated sequentially via TraitId::Alloc().
 * Framework-reserved IDs are defined in the ReservedTraitId namespace.
 * Extension or application code should allocate IDs with TraitId::Alloc()
 * and store them in a static variable so each logical trait gets exactly one ID.
 *
 * @code
 * // One-time allocation per trait type (e.g. in an anonymous namespace or as a static)
 * static const TraitId kMyScrollStateTrait = TraitId::Alloc();
 * @endcode
 */
struct DALI_UI_API TraitId
{
  /**
   * @brief Allocates and returns the next unique TraitId.
   *
   * Uses an internal atomic counter. Thread-safe and lock-free.
   * Call once per logical trait type and store the result statically.
   *
   * @return A new unique TraitId
   */
  static TraitId Alloc();

  /**
   * @brief Equality operator.
   */
  bool operator==(const TraitId& other) const
  {
    return value == other.value;
  }

  /**
   * @brief Inequality operator.
   */
  bool operator!=(const TraitId& other) const
  {
    return value != other.value;
  }

  uint32_t value;

private:
  explicit TraitId(uint32_t v)
  : value(v)
  {
  }
};

} // namespace Ui
} // namespace Dali
