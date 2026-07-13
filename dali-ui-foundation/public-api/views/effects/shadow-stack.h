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
#include <initializer_list>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/shadow.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Describes an ordered stack of View shadows.
 */
class DALI_UI_API ShadowStack
{
public:
  /**
   * @brief Creates an empty shadow stack.
   */
  ShadowStack();

  /**
   * @brief Creates a shadow stack from a list of shadows.
   *
   * @param[in] shadows The shadows to add in order
   */
  ShadowStack(std::initializer_list<Shadow> shadows)
  : ShadowStack()
  {
    for(const auto& shadow : shadows)
    {
      Add(shadow);
    }
  }

  ShadowStack(const ShadowStack& rhs);
  ShadowStack(ShadowStack&& rhs) noexcept;
  ShadowStack& operator=(const ShadowStack& rhs);
  ShadowStack& operator=(ShadowStack&& rhs) noexcept;
  ~ShadowStack();

public:
  /**
   * @brief Adds a shadow to the end of the stack.
   *
   * @param[in] shadow The shadow to add
   * @return This shadow stack
   */
  ShadowStack& Add(const Shadow& shadow);

  /**
   * @brief Clears all shadows from the stack.
   */
  void Clear();

  /**
   * @brief Gets the number of shadows in the stack.
   *
   * @return The number of shadows
   */
  uint32_t GetShadowCount() const;

  /**
   * @brief Gets a shadow from the stack.
   *
   * The returned reference remains valid until the ShadowStack is modified or destroyed.
   *
   * @param[in] index The shadow index
   * @return The shadow at @a index
   */
  const Shadow& GetShadowAt(uint32_t index) const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
