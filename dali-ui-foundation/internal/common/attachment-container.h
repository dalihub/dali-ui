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

#include <dali-ui-foundation/public-api/traits/attachment-id.h>
#include <dali-ui-foundation/public-api/types/unique-any.h>
#include <utility>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Stores attachments by AttachmentId.
 */
class AttachmentContainer
{
public:
  /**
   * @brief Sets a type-erased attachment.
   *
   * Replaces the existing attachment when @p id is already present.
   */
  void SetAttachment(AttachmentId id, UniqueAny attachment)
  {
    for(auto& entry : mAttachments)
    {
      if(entry.first == id)
      {
        entry.second = Dali::Move(attachment);
        return;
      }
    }
    mAttachments.emplace_back(id, Dali::Move(attachment));
  }

  /**
   * @brief Removes an attachment.
   *
   * @return True if an attachment was removed
   */
  bool RemoveAttachment(AttachmentId id)
  {
    for(auto iter = mAttachments.begin(); iter != mAttachments.end(); ++iter)
    {
      if(iter->first == id)
      {
        mAttachments.erase(iter);
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Gets an attachment.
   */
  UniqueAny* GetAttachment(AttachmentId id)
  {
    for(auto& entry : mAttachments)
    {
      if(entry.first == id)
      {
        return &entry.second;
      }
    }
    return nullptr;
  }

  /**
   * @brief Gets a const attachment.
   */
  const UniqueAny* GetAttachment(AttachmentId id) const
  {
    for(auto& entry : mAttachments)
    {
      if(entry.first == id)
      {
        return &entry.second;
      }
    }
    return nullptr;
  }

  /**
   * @brief Detaches an attachment.
   *
   * @return The detached attachment, or an empty UniqueAny if missing
   */
  UniqueAny DetachAttachment(AttachmentId id)
  {
    for(auto iter = mAttachments.begin(); iter != mAttachments.end(); ++iter)
    {
      if(iter->first == id)
      {
        UniqueAny attachment = Dali::Move(iter->second);
        mAttachments.erase(iter);
        return attachment;
      }
    }
    return UniqueAny();
  }

private:
  std::vector<std::pair<AttachmentId, UniqueAny>> mAttachments;
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
