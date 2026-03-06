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

#include <cstdint>

namespace Dali::Ui
{
/**
 * @brief Defines the policy for when a click event is triggered via key input.
 * This policy is specifically for D-Pad, Remote, or Keyboard interactions.
 */
enum class KeyClickPolicy : uint32_t
{
  /**
   * @brief Trigger click when the key is released.
   */
  ON_RELEASE = 0,

  /**
   * @brief Trigger click immediately when the key is pressed.
   */
  ON_PRESS = 1,

  /**
   * @brief Disables click events from key inputs.
   */
  DISABLED = 2,

  /**
   * @brief Reserved for future expansion.
   */
  RESERVED = 99
};
} // namespace Dali::Ui
