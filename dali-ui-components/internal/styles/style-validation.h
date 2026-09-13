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

#include <cmath>

#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali/public-api/common/insets.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace DALI_NAMESPACE::Ui::Internal::StyleValidation
{
inline bool IsNonNegative(float value)
{
  return std::isfinite(value) && value >= 0.0f;
}

inline bool IsNonNegative(const Vector2& value)
{
  return IsNonNegative(value.x) && IsNonNegative(value.y);
}

inline bool IsNonNegative(const Vector4& value)
{
  return IsNonNegative(value.x) && IsNonNegative(value.y) &&
         IsNonNegative(value.z) && IsNonNegative(value.w);
}

inline bool IsNonNegative(const Insets& value)
{
  return IsNonNegative(value.start) && IsNonNegative(value.end) &&
         IsNonNegative(value.top) && IsNonNegative(value.bottom);
}

inline bool IsRequestedDimension(float value)
{
  return value == WRAP_CONTENT || value == MATCH_PARENT || IsNonNegative(value);
}

inline bool IsBlurRadius(float value)
{
  // Bound both float-to-integer conversion and the renderer's radius reduction.
  return IsNonNegative(value) && value <= 4096.0f;
}

inline bool IsMaximumDimension(float value)
{
  return value == UNCONSTRAINED_MAX_SIZE || IsNonNegative(value);
}
} // namespace DALI_NAMESPACE::Ui::Internal::StyleValidation
