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
#include <dali-ui-foundation/public-api/ui-style-key.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <atomic>
#include <limits>

namespace Dali
{
namespace Ui
{
namespace
{
std::atomic<uint32_t> gNextStyleKey{1u};
}

UiStyleKey UiStyleKey::Alloc()
{
  uint32_t id = gNextStyleKey.load(std::memory_order_relaxed);
  while(id != std::numeric_limits<uint32_t>::max())
  {
    if(gNextStyleKey.compare_exchange_weak(id, id + 1u, std::memory_order_relaxed, std::memory_order_relaxed))
    {
      return UiStyleKey(id);
    }
  }

  DALI_ASSERT_ALWAYS(false && "UiStyleKey space exhausted");
  return UiStyleKey(0u);
}

} // namespace Ui
} // namespace Dali
