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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace UI
{
namespace Integration
{
class TraitImpl;
}

/**
 * @brief Trait is a reference-counted object that can be attached to a View.
 *
 * It shares the lifecycle with the View it is attached to.
 * When the View is destroyed or the attachment is removed, the reference count is decremented.
 * Each Trait type is unique per View (one instance per type).
 */
class DALI_UI_API Trait : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized Trait handle.
   */
  Trait() = default;

  /**
   * @brief Destructor.
   */
  ~Trait() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  Trait(const Trait& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  Trait(Trait&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  Trait& operator=(const Trait& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  Trait& operator=(Trait&& rhs) noexcept = default;

  /**
   * @brief Downcasts a handle to Trait handle.
   *
   * If handle points to a Trait, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a Trait or an uninitialized handle
   */
  static Trait DownCast(BaseHandle handle);

protected:
  /**
   * @brief This constructor is used by Trait::New() methods.
   *
   * @param[in] impl The implementation of the trait
   */
  explicit Trait(Integration::TraitImpl* impl);
};

} // namespace UI
} // namespace Dali
