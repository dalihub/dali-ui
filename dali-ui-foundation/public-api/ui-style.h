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
namespace Ui
{

class UiStyleImpl;

/**
 * @brief Base handle for immutable UI style objects.
 */
class DALI_UI_API UiStyle : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UiStyle handle.
   */
  UiStyle() = default;

  /**
   * @brief Downcasts a handle to UiStyle.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized UiStyle on success, otherwise empty
   */
  static UiStyle DownCast(BaseHandle handle);

protected:
  /**
   * @brief Creates a UiStyle handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit UiStyle(UiStyleImpl* impl);
};

/**
 * @brief Function pointer type used to create immutable style objects.
 *
 * UiStyleSheet calls a creator when a style entry is first resolved, then
 * caches the returned style. It is not called for each component instance.
 *
 * Capture-less lambdas can be converted to UiStyleCreator. Capturing lambdas
 * and stateful callables are intentionally not supported by this API.
 *
 * @return A style object
 */
using UiStyleCreator = UiStyle (*)();

} // namespace Ui
} // namespace Dali
