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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

class UiStyle;

/**
 * @brief Allocates and returns the next unique style key value.
 *
 * This is shared by all UiStyleKey<T> instantiations so key values can be used
 * as dense indices in one UiStyleSheet storage.
 *
 * @return A new unique style key value
 */
DALI_UI_API uint32_t AllocateUiStyleKeyValue();

/**
 * @brief A lightweight identifier for a style entry in UiStyleSheet.
 *
 * UiStyleKey values are allocated sequentially via UiStyleKey<T>::Alloc().
 * Allocate one key per logical style slot and store the returned key in static
 * storage so it can be reused for style sheet registration and lookup.
 *
 * The template parameter is a typed marker for the concrete style handle. It
 * does not change the stored key value, but it binds the key to the style type
 * used by UiStyleSheet::GetStyle() and helps prevent key/type mismatches at
 * compile time.
 */
template<typename StyleT = UiStyle>
struct UiStyleKey
{
  /**
   * @brief Allocates and returns the next unique UiStyleKey.
   *
   * Uses an internal atomic counter. Thread-safe and lock-free.
   * If the uint32_t id space is exhausted, an assertion is raised instead of
   * wrapping around to an already allocated key.
   *
   * @return A new unique UiStyleKey
   */
  static UiStyleKey Alloc()
  {
    return UiStyleKey(AllocateUiStyleKeyValue());
  }

  /**
   * @brief Equality operator.
   */
  bool operator==(const UiStyleKey& other) const
  {
    return value == other.value;
  }

  /**
   * @brief Inequality operator.
   */
  bool operator!=(const UiStyleKey& other) const
  {
    return value != other.value;
  }

  uint32_t value;

private:
  explicit UiStyleKey(uint32_t v)
  : value(v)
  {
  }
};

} // namespace Ui
} // namespace Dali
