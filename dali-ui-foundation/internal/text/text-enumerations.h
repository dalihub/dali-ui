#ifndef DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H
#define DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{

/**
 * @brief Enumeration for text layout direction.
 */
enum class Direction : uint8_t
{
  /**
   * @brief Left-to-right text direction.
   */
  LEFT_TO_RIGHT = 0,
  /**
   * @brief Right-to-left text direction.
   */
  RIGHT_TO_LEFT = 1
};

} // namespace Text

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H
