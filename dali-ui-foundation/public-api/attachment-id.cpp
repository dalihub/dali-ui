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

#include <dali-ui-foundation/public-api/attachment-id.h>

#include <atomic>
#include <limits>

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace
{
std::atomic<uint32_t> gNextAttachmentId{1u};
}

AttachmentId AttachmentId::Alloc()
{
  uint32_t id = gNextAttachmentId.load(std::memory_order_relaxed);
  while(id != std::numeric_limits<uint32_t>::max())
  {
    if(gNextAttachmentId.compare_exchange_weak(id, id + 1u, std::memory_order_relaxed, std::memory_order_relaxed))
    {
      return AttachmentId(id);
    }
  }

  DALI_ASSERT_ALWAYS(false && "AttachmentId space exhausted");
  return AttachmentId(0u);
}

} // namespace Ui
} // namespace Dali
