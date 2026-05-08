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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{

/**
 * @brief A lightweight identifier for an attachment slot.
 *
 * AttachmentId values are allocated sequentially via AttachmentId::Alloc().
 * Allocate one id per logical attachment type and store the returned id in
 * static storage so it can be reused for SetAttachment(), GetAttachment(), and
 * DetachAttachment().
 *
 * Do not allocate a new id per object instance or per API call. Repeated
 * one-shot allocation wastes the process-wide id space and makes previously
 * stored attachments impossible to retrieve.
 *
 * @code
 * // One-time allocation per attachment type (e.g. in an anonymous namespace or as a static)
 * static const AttachmentId kMyViewDataAttachment = AttachmentId::Alloc();
 * @endcode
 */
struct DALI_UI_API AttachmentId
{
  /**
   * @brief Allocates and returns the next unique AttachmentId.
   *
   * Uses an internal atomic counter. Thread-safe and lock-free.
   * Call once per logical attachment type and store the result statically.
   * If the uint32_t id space is exhausted, an assertion is raised instead of
   * wrapping around to an already allocated id.
   *
   * @return A new unique AttachmentId
   */
  static AttachmentId Alloc();

  /**
   * @brief Equality operator.
   */
  bool operator==(const AttachmentId& other) const
  {
    return value == other.value;
  }

  /**
   * @brief Inequality operator.
   */
  bool operator!=(const AttachmentId& other) const
  {
    return value != other.value;
  }

  uint32_t value;

private:
  explicit AttachmentId(uint32_t v)
  : value(v)
  {
  }
};

} // namespace Ui
} // namespace Dali
