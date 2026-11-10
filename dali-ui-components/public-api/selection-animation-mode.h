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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{
/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Controls whether a selection-state change animates.
 * @SINCE_3_0.0
 */
enum class SelectionAnimationMode : uint8_t
{
  AUTO     = 0, ///< Animate user-initiated changes while on-scene and visible; programmatic changes snap. @SINCE_3_0.0
  ENABLED  = 1, ///< Animate all state changes while on-scene and visible. @SINCE_3_0.0
  DISABLED = 2  ///< Never animate state changes. @SINCE_3_0.0
};

/** @} */

} // namespace Ui
} //namespace DALI_NAMESPACE
