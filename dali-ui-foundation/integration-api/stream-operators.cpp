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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/stream-operators.h>

// EXTERNAL INCLUDES
#include <ostream>

namespace Dali
{
namespace Ui
{
std::ostream& operator<<(std::ostream& stream, const Insets& insets)
{
  return stream << "[" << insets.start << ", " << insets.end << ", " << insets.top << ", " << insets.bottom << "]";
}

} // namespace Ui
} // namespace Dali
