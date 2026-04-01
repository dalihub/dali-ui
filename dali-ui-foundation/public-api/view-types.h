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

namespace Dali
{
namespace Ui
{

/**
 * @brief Specifies how corner radius values are interpreted.
 *
 * @note Enum values match Ui::Visual::Transform::Policy::Type internally.
 */
enum class CornerRadiusPolicy
{
  RELATIVE = 0, ///< Relative to the view size (percentage [0.0, 0.5] of the shorter side)
  ABSOLUTE = 1, ///< Absolute value in world units (default)
};

} // namespace Ui
} // namespace Dali
