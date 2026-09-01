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
#include <dali/public-api/common/unique-ptr.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-reveal-bridge.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/traits/attachment-id.h>

namespace Dali::Ui::Internal::Text
{
namespace
{
const AttachmentId INLINE_REPLACEMENT_DATA_ATTACHMENT_ID = AttachmentId::Alloc();
}

InlineReplacementData::InlineReplacementData(Ui::View owner)
: host(owner, Dali::Ui::Integration::DepthIndex::CONTENT + 1)
{
}

InlineReplacementData* GetInlineReplacementData(Ui::View owner)
{
  return owner ? owner.GetAttachment<InlineReplacementData>(INLINE_REPLACEMENT_DATA_ATTACHMENT_ID)
               : nullptr;
}

InlineReplacementData* GetInlineReplacementData(Ui::ViewImpl& owner)
{
  using StoredType       = Dali::UniquePtr<InlineReplacementData>;
  UniqueAny*  attachment = Internal::ViewDataImpl::Get(owner).GetAttachment(INLINE_REPLACEMENT_DATA_ATTACHMENT_ID);
  StoredType* data       = attachment ? attachment->Get<StoredType>() : nullptr;
  return data ? data->Get() : nullptr;
}

InlineReplacementData& GetOrCreateInlineReplacementData(Ui::View owner)
{
  DALI_ASSERT_ALWAYS(owner && "Inline replacement data requires a valid owner");

  InlineReplacementData* data = GetInlineReplacementData(owner);
  if(!data)
  {
    owner.SetAttachment(INLINE_REPLACEMENT_DATA_ATTACHMENT_ID, Dali::MakeUnique<InlineReplacementData>(owner));
    data = GetInlineReplacementData(owner);
  }

  DALI_ASSERT_ALWAYS(data && "Inline replacement data creation failed");
  return *data;
}

void RemoveInlineReplacementData(Ui::View owner)
{
  if(owner)
  {
    owner.RemoveAttachment(INLINE_REPLACEMENT_DATA_ATTACHMENT_ID);
  }
}

void RemoveInlineReplacementData(Ui::ViewImpl& owner)
{
  Internal::ViewDataImpl::Get(owner).RemoveAttachment(INLINE_REPLACEMENT_DATA_ATTACHMENT_ID);
}

} // namespace Dali::Ui::Internal::Text

namespace Dali::Ui::Internal
{
bool PublishInlineReplacementRevealTimings(
  Ui::View                                         owner,
  const Vector<Ui::Text::ReplacementRevealTiming>& timings,
  uint64_t                                         sourceRevision,
  Property::Index                                  progressPropertyIndex)
{
  Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data && data->manager.ApplyRevealTimings(timings, sourceRevision, progressPropertyIndex);
}

void ClearInlineReplacementReveal(Ui::View owner)
{
  if(Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner))
  {
    data->manager.ClearReveal();
  }
}

bool IsCurrentInlineReplacementRender(Ui::View owner, uint64_t layoutGeneration)
{
  const Text::InlineReplacementData* data = Text::GetInlineReplacementData(owner);
  return data && data->lastRenderGeneration == layoutGeneration;
}

} // namespace Dali::Ui::Internal
