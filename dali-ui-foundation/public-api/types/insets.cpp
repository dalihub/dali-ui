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

#include <dali-ui-foundation/public-api/types/insets.h>

#include <algorithm>
#include <iostream>
#include <limits>

namespace Dali
{
namespace Ui
{
Insets::Insets()
: start(0.0f),
  end(0.0f),
  top(0.0f),
  bottom(0.0f)
{
}

Insets::Insets(float start, float end, float top, float bottom)
: start(start),
  end(end),
  top(top),
  bottom(bottom)
{
}

Insets::Insets(float horizontal, float vertical)
: start(horizontal),
  end(horizontal),
  top(vertical),
  bottom(vertical)
{
}

Insets::Insets(const Dali::Extents& extents)
: start(static_cast<float>(extents.start)),
  end(static_cast<float>(extents.end)),
  top(static_cast<float>(extents.top)),
  bottom(static_cast<float>(extents.bottom))
{
}

Insets& Insets::operator=(const float* array)
{
  start  = array[0];
  end    = array[1];
  top    = array[2];
  bottom = array[3];

  return *this;
}

Insets::operator Dali::Extents() const
{
  const auto toExtent = [](float value)
  {
    return static_cast<int16_t>(std::clamp(value,
                                           static_cast<float>(std::numeric_limits<int16_t>::min()),
                                           static_cast<float>(std::numeric_limits<int16_t>::max())));
  };
  return Dali::Extents(toExtent(start), toExtent(end), toExtent(top), toExtent(bottom));
}

bool Insets::operator==(const Insets& rhs) const
{
  return start == rhs.start && end == rhs.end && top == rhs.top && bottom == rhs.bottom;
}

bool Insets::operator!=(const Insets& rhs) const
{
  return !(*this == rhs);
}

} // namespace Ui
} // namespace Dali
