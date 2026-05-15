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

namespace Dali
{

namespace Ui
{

namespace Gradient
{

/**
 * @brief The type of coordinate system for certain attributes of the points in a gradient.
 *
 * This applies to the:
 * - Start (x1, y1) and End (x2 and y2) points of a line if using a linear gradient.
 * - Center point (cx, cy) and radius (r) of a circle if using a radial gradient.
 * - Center point (cx, cy) of a circle if using a conic gradient.
 */
enum class Units : uint8_t
{
  OBJECT_BOUNDING_BOX, ///< Uses the normals for the start, end & center points, i.e. top-left is (-0.5, -0.5) and
                       ///< bottom-right is (0.5, 0.5).
  USER_SPACE           ///< Uses the user coordinates for the start, end & center points, i.e. in a 200 by 200 view, top-left
                       ///< is (0, 0) and bottom-right is (200, 200).
};

/**
 * @brief Policies that define what happens if the gradient starts or ends inside the bounds of the target rectangle.
 */
enum class SpreadMethod : uint8_t
{
  PAD,     ///< Uses the terminal colors of the gradient to fill the remainder of the quad.
  REFLECT, ///< Reflect the gradient pattern start-to-end, end-to-start, start-to-end etc. until the quad is filled.
  REPEAT   ///< Repeat the gradient pattern start-to-end, start-to-end, start-to-end etc. until the quad is filled.
};

} //namespace Gradient

} // namespace Ui

} // namespace Dali
